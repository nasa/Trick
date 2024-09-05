
#include	<iostream>
#include	"clang/Lex/Token.h"
#include	"clang/Basic/IdentifierTable.h"
#include	"FindTrickICG.hh"
#include	"Utilities.hh"

FindTrickICG::FindTrickICG(clang::CompilerInstance	&	in_ci,	HeaderSearchDirs	&	in_hsd	,	bool	in_print_msgs	)
	:	ci(in_ci)	,	hsd(in_hsd)	,	print_msgs(in_print_msgs)	,	header_printed(false)	{	}

void	FindTrickICG::FileChanged(clang::SourceLocation	Loc,	FileChangeReason	Reason,
																									clang::SrcMgr::CharacteristicKind	FileType,
																									clang::FileID	PrevFID	)	{
				/*
							We	are	interested	when	the	file	changes,	specifically	when	we	enter	and	exit	a	file.
							We	are	maintaining	a	stack	of	file	names.		When	a	file	is	entered	the	name	is
							pushed	to	the	stack.		When	a	file	is	exited,	we	pop	a	name	from	the	stack.
				*/
				if	(	Loc.isValid()	and	Loc.isFileID()	)	{
								clang::PresumedLoc	PLoc	=	ci.getSourceManager().getPresumedLoc(Loc)	;
								std::string	loc_str	;
								if	(	!	PLoc.isInvalid())	{
												loc_str	=	PLoc.getFilename()	;
												switch	(Reason)	{
																case	EnterFile	:
																				included_files.push_back(loc_str)	;
																				break	;
																case	ExitFile	:
																				included_files.pop_back()	;
																				break	;
																default:
																				break	;
												}
								}
				}
}

#if	(LIBCLANG_MAJOR	<	10)	//	TODO	delete	when	RHEL	7	no	longer	supported
void	FindTrickICG::FileSkipped(const	clang::FileEntry	&SkippedFile,
																											const	clang::Token	&FilenameTok,
																											clang::SrcMgr::CharacteristicKind	FileType)	{
				std::string	file_name	=	SkippedFile.getName()	;
#else
void	FindTrickICG::FileSkipped(const	clang::FileEntryRef	&	SkippedFile,	const	clang::Token	&	FilenameTok,
																								clang::SrcMgr::CharacteristicKind	FileType)	{
				/*	Files	that	have	header	guards	are	only	preprocessed	once	because	of	an	optimization.
				We	still	need	to	add	its	include	chain	to	compat15	if	TRICK_ICG	was	found	when	it	was
				originally	preprocessed	*/
				std::string	file_name	=	SkippedFile.getName().str()	;
#endif
				std::string	file_path;
				{
								char*	path_cstr	=	almostRealPath(file_name.c_str());
								if(path_cstr	!=	NULL)	{
												file_path	=	std::string(path_cstr);
												free(path_cstr);
								}
				}

				//	Check	if	skipped	header	is	in	Compat15
				if(hsd.isPathInCompat15(file_path))	{
								//	for	each	header	in	the	stack,	mark	them	as	being	exposed	to	TRICK_ICG
								for	(std::string&	file	:	included_files)	{
												hsd.addTrickICGFoundFile(file);
								}
				}
}


#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	5))
void	FindTrickICG::If(clang::SourceLocation	Loc,	clang::SourceRange	ConditionRange,	clang::PPCallbacks::ConditionValueKind	ConditionValue)
#else
void	FindTrickICG::If(clang::SourceLocation	Loc,	clang::SourceRange	ConditionRange,	bool	ConditionValue)
#endif
{
				if	(	ConditionRange.isValid()	)	{
								//	Get	the	full	text	of	the	if	statement	into	a	string
								llvm::StringRef	ref	=	clang::Lexer::getSourceText(clang::CharSourceRange::getCharRange(ConditionRange),	ci.getSourceManager(),	clang::LangOptions());
								//	if	the	if	statement	contains	TRICK_ICG	we	need	to	mark	it.
								if	(	ref.str().find("TRICK_ICG")	!=	std::string::npos	)	{
												//	for	each	header	in	the	stack,	mark	them	as	being	exposed	to	TRICK_ICG
												std::vector<std::string>::iterator	it	;
												for	(	it	=	included_files.begin()	;	it	!=	included_files.end()	;	++it	)	{
																hsd.addTrickICGFoundFile(*it)	;
												}
												//	print	warning	messages.
												if	(	print_msgs	)	{
																print_header()	;
																std::string	loc_str	=	Loc.printToString(ci.getSourceManager())	;
																char	*	rpath	=	almostRealPath(loc_str.c_str())	;
																if	(	rpath	)	{
																					std::cout	<<	color(WARNING,
																						std::string("Warning:	TRICK_ICG	used	in	preprocessor	conditional	")	+	rpath)	<<	std::endl	;
																}	else	{
																					std::cout	<<	color(WARNING,
																						std::string("Warning:	TRICK_ICG	used	in	preprocessor	conditional	")	+	loc_str)	<<	std::endl	;
																}
												}
								}
				}
}

#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	5))
void	FindTrickICG::ElIf(clang::SourceLocation	Loc,	clang::SourceRange	ConditionRange,	clang::PPCallbacks::ConditionValueKind	ConditionValue)
#else
void	FindTrickICG::ElIf(clang::SourceLocation	Loc,	clang::SourceRange	ConditionRange,	bool	ConditionValue)
#endif
{
				//	Do	the	same	processing	for	an	#elif	statement	as	an	#if	statement.
				If(Loc,ConditionRange,ConditionValue)	;
}

#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	7))
void	FindTrickICG::Ifdef(clang::SourceLocation	Loc,	const	clang::Token	&MacroNameTok,	const	clang::MacroDefinition	&MD)
#else
void	FindTrickICG::Ifdef(clang::SourceLocation	Loc,	const	clang::Token	&MacroNameTok,	const	clang::MacroDirective	*MD)
#endif
{
				//	Get	the	token	name	that	is	being	tested.
				std::string	name	=	MacroNameTok.getIdentifierInfo()->getName().str()	;
				if	(	!	name.compare("TRICK_ICG")	)	{
								std::string	loc_str	=	Loc.printToString(ci.getSourceManager())	;
								//	There	is	a	singular	ifdef	TRICK_ICG	in	S_source.hh	we	need	to	ignore.
								if	(	(loc_str.find("S_source.hh")	==	std::string::npos)	)	{
												//	for	each	header	in	the	stack,	mark	them	as	being	exposed	to	TRICK_ICG
												std::vector<std::string>::iterator	it	;
												for	(	it	=	included_files.begin()	;	it	!=	included_files.end()	;	++it	)	{
																hsd.addTrickICGFoundFile(*it)	;
												}
												//	print	warning	messages.
												if	(	print_msgs	)	{
																print_header()	;
																std::cout	<<	color(WARNING,std::string("Warning:	ifdef	TRICK_ICG	found	")	+	loc_str)	<<	std::endl	;
												}
								}
				}

}

#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	7))
void	FindTrickICG::Ifndef(clang::SourceLocation	Loc,	const	clang::Token	&MacroNameTok,	const	clang::MacroDefinition	&MD)
#else
void	FindTrickICG::Ifndef(clang::SourceLocation	Loc,	const	clang::Token	&MacroNameTok,	const	clang::MacroDirective	*MD)
#endif
{
				//	Get	the	token	name	that	is	being	tested.
				std::string	name	=	MacroNameTok.getIdentifierInfo()->getName().str()	;
				if	(	!	name.compare("TRICK_ICG")	)	{
								std::string	loc_str	=	Loc.printToString(ci.getSourceManager())	;
								//	for	each	header	in	the	stack,	mark	them	as	being	exposed	to	TRICK_ICG
								std::vector<std::string>::iterator	it	;
								for	(	it	=	included_files.begin()	;	it	!=	included_files.end()	;	++it	)	{
												hsd.addTrickICGFoundFile(*it)	;
								}
								//	print	warning	messages.
								if	(	print_msgs	)	{
												print_header()	;
												std::cout	<<	color(WARNING,	std::string("Warning:	ifndef	TRICK_ICG	found	")	+	loc_str)	<<	std::endl	;
								}
				}
}

void	FindTrickICG::print_header()	{
				//	Print	the	warning	message	once.
				if	(	!	header_printed	)	{
								std::cout	<<	color(WARNING,	std::string(
									"Warning:	TRICK_ICG	found	in	one	or	more	header	files.		The	use	of	TRICK_ICG	precludes	the	use	of\n"
									"some	C++	class/structures	in	these	headers.		Consider	removing	the	TRICK_ICG	preprocessor	conditionals\n"))	;
								header_printed	=	true	;
				}
}
