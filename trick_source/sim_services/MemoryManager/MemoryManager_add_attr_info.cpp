#include	<iostream>
#include	<sstream>
#include	<algorithm>
#include	<dlfcn.h>

#ifdef	__GNUC__
#include	<cxxabi.h>
#endif

#include	"trick/SimObject.hh"
#include	"trick/MemoryManager.hh"

/**
	*
	*/
int	Trick::MemoryManager::add_attr_info(	const	std::string	&	user_type_string	,	ATTRIBUTES	*	attr	,	const	char	*	file_name	,	unsigned	int	line_num	)	{

				std::string	user_type_name	;
				std::string	sub_attr_name	;
				std::string	enum_attr_name	;
				std::string	sub_attr_init_name	;
				std::string	size_func_name	;
				size_t	spos	;
				ATTRIBUTES*	sub_attr	=	NULL	;
				ENUM_ATTR*	enum_attr	=	NULL	;
				void	(*init_sub_attr)(void)	=	NULL	;
				size_t	(*size_func)(void)	=	NULL	;
				unsigned	int	ii	;
				std::set<std::string>::iterator	it	;

				/**	@par	Design	Details:	*/

				user_type_name	=	user_type_string	;

				//	Check	if	the	type	is	a	primitive	type.		If	so,	we	don't	need	to	continue
				it	=	primitive_types.find(user_type_string)	;
				if	(	it	!=	primitive_types.end()	)	{
								return	0	;
				}

				//	The	user	type	name	may	start	as	const	Foo<int>::Bar[4].		We	need	to	convert	that	to	Foo_int___Bar
				//	remove	const	from	the	typename	if	it	exists
				spos	=	user_type_name.find("const	")	;
				if	(	spos	!=	std::string::npos	)	{
								user_type_name.erase(	spos	,	spos	+	6)	;
				}
				//	remove	any	brackets
/*
				spos	=	user_type_name.find("[")	;
				if	(	spos	!=	std::string::npos	)	{
								user_type_name.erase(	spos	)	;
				}
*/
				//	replace	":<>,*[]"	with	'_'
				std::replace(	user_type_name.begin(),	user_type_name.end(),	':',	'_')	;
				std::replace(	user_type_name.begin(),	user_type_name.end(),	'<',	'_')	;
				std::replace(	user_type_name.begin(),	user_type_name.end(),	'>',	'_')	;
				std::replace(	user_type_name.begin(),	user_type_name.end(),	',',	'_')	;
				std::replace(	user_type_name.begin(),	user_type_name.end(),	'*',	'_')	;
				std::replace(	user_type_name.begin(),	user_type_name.end(),	'[',	'_')	;
				std::replace(	user_type_name.begin(),	user_type_name.end(),	']',	'_')	;
				//	remove	spaces
				user_type_name.erase(std::remove_if(user_type_name.begin(),	user_type_name.end(),	(int(*)(int))std::isspace),	user_type_name.end())	;

				//	Attempt	to	find	an	io_src_sizeof	function	for	the	named	user	type.
				size_func_name	=	"io_src_sizeof_"	+	user_type_name	;
				for	(	ii	=	0	;	ii	<	dlhandles.size()	&&	size_func	==	NULL	;	ii++	)	{
								size_func					=	(size_t	(*)(void))dlsym(	dlhandles[ii]	,	size_func_name.c_str())	;
								if	(	size_func	!=	NULL	)	{
												attr->size	=	(*size_func)()	;
								}
				}

				if	(	size_func	==	NULL)		{
								std::stringstream	message;
								message	<<	"("	<<	file_name	<<	":"	<<	line_num
												<<	"):	Couldn't	find	an	io_src_sizeof_	function	for	type	"
												<<	user_type_string.c_str()	<<	"["	<<	size_func_name.c_str()	<<	"()].";
								emitWarning(message.str());
				}

				//	Attempt	to	find	an	attributes	list	for	the	named	user	type.
				sub_attr_name	=	"attr"	+	user_type_name	;
				for	(	ii	=	0	;	ii	<	dlhandles.size()	&&	sub_attr	==	NULL	;	ii++	)	{
								sub_attr						=	(ATTRIBUTES	*)dlsym(	dlhandles[ii]	,	sub_attr_name.c_str())	;
				}

				if	(sub_attr	!=	NULL)	{		//	If	attributes	for	the	named	type	were	found	...

								//	Set	type	and	attr	of	the	ATTRIBUTES	structure	that	we're	populating.
								attr->type	=	TRICK_STRUCTURED	;
								attr->attr	=	sub_attr;

								//	Attempt	to	find	the	initialization	function	for	the	attributes	that	we	just	found.
								sub_attr_init_name	=	"init_attr"	+	user_type_name	+	"_c_intf"	;

								for	(	ii	=	0	;	ii	<	dlhandles.size()	&&	init_sub_attr	==	NULL	;	ii++	)	{
												init_sub_attr	=	(void	(*)(void))dlsym(	dlhandles[ii]	,	sub_attr_init_name.c_str())	;
								}

								if	(	init_sub_attr	!=	NULL	)	{					//	If	the	initialization	function	was	found,
												(*init_sub_attr)()	;											//	then	call	it.
								}	else	{
												std::stringstream	message;
												message	<<	"	ATTRIBUTES	init	routine	for	type	\""
																				<<	user_type_name.c_str()	<<	"\"	not	found.";
												emitWarning(message.str());
												return(1)	;
								}

				}	else	{	//	The	named	user	type	isn't	a	structure	type.	Maybe	it's	an	enumeration	type.

								//	Check	to	see	whether	the	user	type	name	is	in	the	enumeration_map.
								ENUMERATION_MAP::iterator	curr_pos	=	enumeration_map.find(	user_type_name);

								//	If	it's	not	in	the	enumeration	map	then
								if	(curr_pos	==	enumeration_map.end())	{

												//	Construct	the	name	of	the	enumeration	attributes.
												enum_attr_name	=	"enum"	+	user_type_name	;

												//	Attempt	to	find	enumeration	attributes	for	the	named	user	type.
												for	(	ii	=	0	;	ii	<	dlhandles.size()	&&	enum_attr	==	NULL	;	ii++	)	{
																enum_attr	=	(ENUM_ATTR	*)dlsym(	dlhandles[ii]	,	enum_attr_name.c_str())	;
												}

												//	If	enumeration	attributes	for	the	named	type	were	found	then
												if	(	enum_attr	!=	NULL	)	{

																//	Put	them	in	the	enumeration	map.
																enumeration_map[	user_type_name]	=	enum_attr;

												//	otherwise	...
												}	else	{
																std::stringstream	message;
																message	<<	"ATTRIBUTES	for	type	\""	<<	user_type_name.c_str()	<<	"\"	not	found.";
																emitWarning(message.str());
																return(1)	;
												}
								}	else	{
												enum_attr	=	curr_pos->second;
								}

								//	Set	the	type	information	in	the	ATTRIBUTES	structure	that	we	are	populating.
								attr->type	=	TRICK_ENUMERATED	;
								attr->attr	=	enum_attr;
				}
				return(0)	;
}

