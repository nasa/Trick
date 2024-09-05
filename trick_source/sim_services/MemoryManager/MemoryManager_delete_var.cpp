#include	<stdlib.h>
#include	<iostream>

#include	<sstream>
#include	<algorithm>
#include	<dlfcn.h>
#include	"trick/MemoryManager.hh"

//	MEMBER	FUNCTION
int	Trick::MemoryManager::delete_var(void*	address	)	{

				if	(address	==	0)	{
								if	(debug_level)	{
												std::stringstream	message;
												message	<<	"Cannot	delete	memory	at	NULL.";
												emitWarning(message.str());
								}
								return	1;
				}

				//	Find	the	allocation	record	for	this	address.
				ALLOC_INFO	*	alloc_info	=	get_alloc_info_at(	address);

				if	((alloc_info	!=	NULL))	{
								/*
											Remove	the	allocation	from	the	map	first	so	we	don't	have	trouble
											if	io_src_destruct_class	or	io_src_delete_class	calls	delete_var	recursively.
									*/
								//	BEGIN	PROTECTION	of	the	alloc_info_map.
								pthread_mutex_lock(&mm_mutex);
								alloc_info_map.erase(	address);
								//	END	PROTECTION	of	the	alloc_info_map.
								pthread_mutex_unlock(&mm_mutex);

								/*	Verify	that	it's	OK	to	free	the	memory	at	the	given	address.
											It's	OK	if	an	allocation	record	exists	for	this	address	and	the
											MemoryManager	allocated	it.
									*/
								if	(	alloc_info->stcl	==	TRICK_LOCAL	)	{
												if	(	alloc_info->alloc_type	==	TRICK_ALLOC_MALLOC	)	{

																//	This	will	call	a	destructor	ONLY	if	alloc_info->type	is	TRICK_STRUCTURED.
																//	Otherwise	it	does	nothing.
																io_src_destruct_class(	alloc_info	);

		//	The	destructor	that	we	just	called	MAY	have	deleted	addresses
		//	that	are	already	planned	for	deletion,	say	during	reset_memory.
		//	So,	keep	a	record	of	what	we've	recently	deleted	so	we	don't
		//	to	warn	that	we	can't	find	it,	when	reset_memory	also	tries	to
		//	delete	that	same	address.
		deleted_addr_list.push_back(address);

																free(	address);
												}	else	if	(	alloc_info->alloc_type	==	TRICK_ALLOC_NEW	)	{
																io_src_delete_class(	alloc_info	);
												}
								}

								if	(debug_level)	{
												std::cout	<<	"Deleted	allocation	at	address	"	<<	alloc_info->start	<<	"."	<<	std::endl;
												std::cout.flush();
								}

								if	(alloc_info->name	)	{
												pthread_mutex_lock(&mm_mutex);
												variable_map.erase(	alloc_info->name);
												pthread_mutex_unlock(&mm_mutex);
												free(alloc_info->name);
								}

								if	(	alloc_info->user_type_name	)	{
												free(alloc_info->user_type_name);
								}

								//	Delete	the	alloc_info	record.
								free(alloc_info);

				}	else	{

								//	The	allocation	(address)	we're	tring	to	delete	may	have	just	been
	//	deleted	by	a	user's	destructor	(using	delete_var).	Check	the	deleted_addr_list
	//	to	see	if	the	allocation	was	just	deleted.	If	it	was,	then	there's	no
	//	problem.	If	it	wasn't,	then	the	MemoryManager	never	knew	about	it,	and
	//	this	call	to	delete_var()	is	a	problem.

	if	(	resetting_memory	==	true)	{
												//	Check	the	deleted_addr_list	to	see	whether	we	recently	deleted	this	address.
					std::list<void*>::iterator	iter	=
									std::find(	deleted_addr_list.begin(),	deleted_addr_list.end(),	address	);
					//	If	we	didn't	recently	delete	it,	then	there's	a	problem.
					if	(	iter	==	deleted_addr_list.end()	)	{
																std::stringstream	message;
																message	<<	"The	MemoryManager	cannot	delete	memory	at	address	["
																								<<	address	<<	"]	because	it	has	no	record	of	it.	Furthermore,"
			<<	"	the	MemoryManager	has	not	recently	deleted	it	while"
			<<	"	resetting	memory	for	a	checkpoint	reload.";
																emitWarning(message.str());
					}
	}	else	{
												std::stringstream	message;
												message	<<	"The	MemoryManager	cannot	delete	memory	at	address	["
																				<<	address	<<	"]	because	it	has	no	record	of	it.";
												emitWarning(message.str());
	}

								return	1	;
				}

				return	0;
}

//	MEMBER	FUNCTION
int	Trick::MemoryManager::delete_var(	std::string	name)	{

				VARIABLE_MAP::iterator	pos;
				ALLOC_INFO	*alloc_info;

				pthread_mutex_lock(&mm_mutex);
				pos	=	variable_map.find(	name);

				if	(pos	!=	variable_map.end())	{
								alloc_info	=	pos->second;

								pthread_mutex_unlock(&mm_mutex);
								return(	delete_var(	alloc_info->start));
				}	else	{
								std::stringstream	message;
								message	<<	"Cannot	delete	variable	\""	<<	name
																<<	"\"	because	it	doesn't	exist."	;
								emitError(message.str());
				}
				pthread_mutex_unlock(&mm_mutex);
				return	1;
}

//	MEMBER	FUNCTION
int	Trick::MemoryManager::delete_extern_var(	void*	address)	{
				//	delete_var	will	remove	the	external	variable	from	the	allocation	map
				return	delete_var(address)	;
}

//	MEMBER	FUNCTION
int	Trick::MemoryManager::delete_extern_var(	std::string	name)	{
				//	delete_var	will	remove	the	external	variable	from	the	allocation	map
				return	delete_var(name)	;
}

