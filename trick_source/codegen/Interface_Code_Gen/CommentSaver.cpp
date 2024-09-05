
#include	<iostream>
#include	<sstream>
#include	<regex.h>
#include	<algorithm>

#include	"clang/Basic/FileManager.h"

#include	"CommentSaver.hh"
#include	"Utilities.hh"

CommentSaver::CommentSaver(clang::CompilerInstance	&	in_ci	,	HeaderSearchDirs	&	in_hsd	)	:	ci(in_ci)	,	hsd(in_hsd)	{}

bool	CommentSaver::HandleComment(clang::Preprocessor	&PP,	clang::SourceRange	Comment)	{

				//Comment.getBegin().dump(sm)	;
				if	(	isInUserOrTrickCode(	ci	,	Comment.getBegin()	,	hsd	)	)	{
#if	(LIBCLANG_MAJOR	<	4)	//	TODO	delete	when	RHEL	7	no	longer	supported
								std::string	file_name	=	ci.getSourceManager().getBufferName(Comment.getBegin())	;
#else
								std::string	file_name	=	ci.getSourceManager().getBufferName(Comment.getBegin()).str()	;
#endif
								char	*	resolved_path	=	almostRealPath(	file_name.c_str()	)	;
								if	(	resolved_path	!=	NULL	)	{
												unsigned	int	line_no	=	ci.getSourceManager().getSpellingLineNumber(Comment.getBegin())	;
												comment_map[std::string(resolved_path)][line_no]	=	Comment	;
												free(resolved_path)	;
								}
				}

				//	returning	false	means	we	did	not	push	any	text	back	to	the	stream	for	further	reads.
				return	false	;
}

std::string	CommentSaver::getComment(	clang::SourceRange	sr	)	{

				if	(	sr.isValid()	)	{
								/*	fsl_begin	and	fsl_end	are	two	pointers	into	the	header	file.		We	want	the	text	between	the	two	pointers.	*/
								clang::FullSourceLoc	fsl_begin(sr.getBegin()	,	ci.getSourceManager())	;
								clang::FullSourceLoc	fsl_end(sr.getEnd()	,	ci.getSourceManager())	;
								std::string	comment_text(	fsl_begin.getCharacterData()	,
									(size_t)(fsl_end.getCharacterData()	-	fsl_begin.getCharacterData()))	;
								return	comment_text	;
				}

				/*	return	an	empty	string	if	no	comment	found	on	this	line	of	the	file.	*/
				return	std::string()	;
}

std::string	CommentSaver::getComment(	std::string	file_name	,	unsigned	int	line_no	)	{
				return	getComment(comment_map[file_name][line_no])	;
}

std::string	CommentSaver::getTrickHeaderComment(	std::string	file_name	)	{

				std::map	<	unsigned	int	,	clang::SourceRange	>::iterator	cit	;
				std::string	ret	;

				char	*	resolved_path	=	almostRealPath(	file_name.c_str()	)	;
				if	(	resolved_path	!=	NULL	)	{
								if	(	trick_header_comments.find(resolved_path)	==	trick_header_comments.end()	)	{
												trick_header_comments[resolved_path]	=	std::string()	;
												for	(	cit	=	comment_map[resolved_path].begin()	;	cit	!=	comment_map[resolved_path].end()	;	++cit	)	{
																std::string	comment_str	=	getComment((*cit).second)	;
																if	(	comment_str.find("@trick_parse")	!=	std::string::npos	or
																					comment_str.find("\\trick_parse")	!=	std::string::npos	)	{
																				trick_header_comments[resolved_path]	=	getComment((*cit).second)	;
																				break	;
																}	else	{
																				std::transform(comment_str.begin(),	comment_str.end(),	comment_str.begin(),	::toupper)	;
																				if	(	comment_str.find("PURPOSE")	!=	std::string::npos	)	{
																								trick_header_comments[resolved_path]	=	getComment((*cit).second)	;
																								break	;
																				}
																}
												}
								}
								ret	=	trick_header_comments[resolved_path]	;
								free(resolved_path)	;
				}

				return	ret	;

}

void	CommentSaver::getICGField(	std::string	file_name	)	{

				/*	default	the	icg_no	flags	to	false	*/
				icg_no_found[file_name]	=	false	;
				icg_no_comment_found[file_name]	=	false	;

				std::string	th_str	=	getTrickHeaderComment(file_name)	;
				if	(	!	th_str.empty()	)	{

								size_t	trick_parse_keyword	;
								if	(	(trick_parse_keyword	=	th_str.find("trick_parse"))	!=	std::string::npos	)	{
												size_t	trick_parse_everything	=	th_str.find("everything",	trick_parse_keyword)	;
												size_t	trick_parse_attributes	=	th_str.find("attributes",	trick_parse_keyword)	;
												size_t	trick_parse_dep_only	=	th_str.find("dependencies_only",	trick_parse_keyword)	;
												size_t	closing_brace	=	th_str.find("}",	trick_parse_keyword)	;
												if	(	closing_brace	!=	std::string::npos	)	{
																if	(	trick_parse_everything	!=	std::string::npos	and
																					trick_parse_everything	<	closing_brace	)	{
																				icg_no_comment_found[file_name]	=	false	;
																				icg_no_found[file_name]	=	false	;
																}	else	if	(	trick_parse_attributes	!=	std::string::npos	and
																					trick_parse_attributes	<	closing_brace	)	{
																				icg_no_comment_found[file_name]	=	true	;
																				icg_no_found[file_name]	=	false	;
																}	else	if	(	trick_parse_dep_only	!=	std::string::npos	and
																					trick_parse_dep_only	<	closing_brace	)	{
																				icg_no_comment_found[file_name]	=	false	;
																				icg_no_found[file_name]	=	true	;
																}	else	{
																				std::cout	<<	file_name	<<	":	trick_parse	directive	found	but	no	argument	matched."	<<	std::endl	;
																				std::cout	<<	"Valid	arguments	are	(everything|attributes|dependencies_only)"	<<	std::endl	;
																				exit(-1)	;
																}
												}
								}	else	{

												std::transform(th_str.begin(),	th_str.end(),	th_str.begin(),	::toupper)	;
												int	ret	;
												regex_t	reg_expr	;
												regmatch_t	pmatch[10]	;
												memset(pmatch	,	0	,	sizeof(pmatch))	;

												/*	POSIX	regular	expressions	are	always	greedy,	making	our	job	harder.
															We	have	to	use	several	regular	expressions	to	get	the	types.		This	was
															so	much	easier	in	perl!	*/

												/*	find	the	ICG	field	*/
												regcomp(	&reg_expr	,	"(ICG:)"	,	REG_EXTENDED	|	REG_ICASE	)	;
												ret	=	regexec(	&reg_expr	,	th_str.c_str()	,	10	,	pmatch	,	0	)	;
												regfree(&reg_expr)	;
												if	(	ret	!=	0	)	{
																return	;
												}
												th_str	=	th_str.substr(pmatch[1].rm_eo)	;

												/*	find	the	end	of	the	ICG	field	*/
												memset(pmatch	,	0	,	sizeof(pmatch))	;
												regcomp(	&reg_expr	,	"(\\))"	,	REG_EXTENDED	)	;
												ret	=	regexec(	&reg_expr	,	th_str.c_str()	,	10	,	pmatch	,	0	)	;
												regfree(&reg_expr)	;

												if	(	ret	!=	0	)	{
																return	;
												}
												th_str	=	th_str.substr(0	,	pmatch[1].rm_so)	;

												/*	test	for	NoComment	*/
												memset(pmatch	,	0	,	sizeof(pmatch))	;
												regcomp(	&reg_expr	,	"(NOCOMMENT)$"	,	REG_EXTENDED	)	;
												ret	=	regexec(	&reg_expr	,	th_str.c_str()	,	10	,	pmatch	,	0	)	;
												regfree(&reg_expr)	;

												if	(	ret	==	0	)	{
																icg_no_comment_found[file_name]	=	true	;
												}

												/*	test	for	No	*/
												memset(pmatch	,	0	,	sizeof(pmatch))	;
												regcomp(	&reg_expr	,	"(NO)$"	,	REG_EXTENDED	)	;
												ret	=	regexec(	&reg_expr	,	th_str.c_str()	,	10	,	pmatch	,	0	)	;
												regfree(&reg_expr)	;

												if	(	ret	==	0	)	{
																icg_no_found[file_name]	=	true	;
																icg_no_comment_found[file_name]	=	true	;
												}
								}
				}

}

bool	CommentSaver::hasTrickHeader(	std::string	file_name	)	{
				std::string	th_str	=	getTrickHeaderComment(file_name)	;
				return	(!	th_str.empty())	;
}

bool	CommentSaver::hasICGNo(	std::string	file_name	)	{

				if	(	icg_no_found.find(file_name)	==	icg_no_found.end()	)	{
								getICGField(file_name)	;
				}

				return	icg_no_found[file_name]	;
}

bool	CommentSaver::hasICGNoComment(	std::string	file_name	)	{

				if	(	icg_no_comment_found.find(file_name)	==	icg_no_comment_found.end()	)	{
								getICGField(file_name)	;
				}

				return	icg_no_comment_found[file_name]	;
}

std::set<	std::string	>	CommentSaver::getIgnoreTypes(	std::string	file_name	)	{
				std::set<	std::string	>	ignore_types	;
				std::string	th_str	=	getTrickHeaderComment(file_name)	;

				if	(	th_str.empty()	)	{
								return	ignore_types;
				}

				int	ret	;
				regex_t	reg_expr	;
				regmatch_t	pmatch[10]	;
				//std::cout	<<	"here	in	getIgnoreTypes\n"	<<	th_str	<<	std::endl	;

				size_t	start	=	0	;
				start	=	th_str.find(	"trick_exclude_typename")	;
				while	(	start	!=	std::string::npos	)	{
								start	+=	strlen("trick_exclude_typename")	;
								std::string	temp_str	=	th_str.substr(start)	;
								memset(pmatch	,	0	,	sizeof(pmatch))	;
								regcomp(	&reg_expr	,	"^\\s*\\{\\s*(\\S+)\\s*\\}"	,	REG_EXTENDED	)	;
								ret	=	regexec(	&reg_expr	,	temp_str.c_str()	,	10	,	pmatch	,	0	)	;
								regfree(&reg_expr)	;
								if	(	ret	==	0	)	{
												std::string	item	=	temp_str.substr(pmatch[1].rm_so,	pmatch[1].rm_eo)	;
												//	regular	expression	leaving	trailing	space	and	brace.	Why?
												item.erase(item.find_first_of("	\t}"))	;
												//std::cout	<<	"[31micg_ignore_types[00m	"	<<	item	<<	std::endl	;
												ignore_types.insert(item)	;
								}
								start	=	th_str.find(	"trick_exclude_typename",	start	)	;
				}

				//std::transform(th_str.begin(),	th_str.end(),	th_str.begin(),	::toupper)	;

				memset(pmatch	,	0	,	sizeof(pmatch))	;

				/*	POSIX	regular	expressions	are	always	greedy,	making	our	job	harder.
							We	have	to	use	several	regular	expressions	to	get	the	types.		This	was
							so	much	easier	in	perl!	*/

				/*	find	the	start	of	the	ICG_IGNORE_TYPES	field	*/
				regcomp(	&reg_expr	,	"(ICG[	_]IGNORE[	_]TYPE(S)?:)"	,	REG_EXTENDED	|	REG_ICASE	)	;
				ret	=	regexec(	&reg_expr	,	th_str.c_str()	,	10	,	pmatch	,	0	)	;
				regfree(&reg_expr)	;
				if	(	!ret	)	{
								th_str	=	th_str.substr(pmatch[1].rm_eo)	;

								/*	find	the	end	of	the	ICG_IGNORE_TYPES	field	*/
								memset(pmatch	,	0	,	sizeof(pmatch))	;
								regcomp(	&reg_expr	,	"(\\)\\s*\\))"	,	REG_EXTENDED	)	;
								ret	=	regexec(	&reg_expr	,	th_str.c_str()	,	10	,	pmatch	,	0	)	;
								regfree(&reg_expr)	;
								if	(	!ret	)	{
												th_str	=	th_str.substr(0	,	pmatch[1].rm_so)	;
												std::replace(	th_str.begin(),	th_str.end(),	'(',	'	');
												std::replace(	th_str.begin(),	th_str.end(),	')',	'	');
												std::replace(	th_str.begin(),	th_str.end(),	'\n',	'	');

												std::stringstream	ss(th_str);
												std::string	item;
												while	(std::getline(ss,	item,	'	'))	{
																if	(	!	item.empty()	)	{
																				//std::cout	<<	"[31micg_ignore_types[00m	"	<<	item	<<	std::endl	;
																				ignore_types.insert(item)	;
																}
												}
								}
				}

				return	ignore_types	;
}

