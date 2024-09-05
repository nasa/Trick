/*
			PURPOSE:	(	Add	a	shared	library	)
			REFERENCE:
			ASSUMPTIONS	AND	LIMITATIONS:
			PROGRAMMERS:	(	(Alex	Lin)	(NASA)	(3/17/03)	)
	*/

#include	<iostream>
#include	<sstream>
#include	<dlfcn.h>

#include	"trick/MemoryManager.hh"

/*	This	routine	only	returns	the	size	if	ptr	points	to	the	beginning	of	the	allocation	area	*/
int	Trick::MemoryManager::add_shared_library_symbols(const	char	*	file_name)	{

				void	*	new_handle	;

				new_handle	=	dlopen(	file_name	,	RTLD_LAZY	)	;
				if	(	new_handle	!=	NULL	)	{
								dlhandles.push_back(new_handle)	;
				}	else	{
								std::stringstream	message;
								message	<<	"add_shared_library_symbols	could	not	find	library	file:	\""	<<	file_name	<<	"\".";
								emitError(message.str());
				}

				return	0	;
}

