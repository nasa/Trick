#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sstream>
#include	<fstream>

#include	"trick/MemoryManager.hh"
#include	"trick/ClassicCheckPointAgent.hh"

int	Trick::MemoryManager::set_restore_stls_default	(bool	on_off)	{
				restore_stls_default	=	on_off;
				return	0;
}

int	Trick::MemoryManager::read_checkpoint(	std::istream	*is,	bool	do_restore_stl)	{

				ALLOC_INFO_MAP::iterator	pos;
				ALLOC_INFO*	alloc_info;

				if	(debug_level)	{
								std::cout	<<	std::endl	<<	"-	Reading	checkpoint."	<<	std::endl;
								std::cout.flush();
				}

				if	(currentCheckPointAgent->restore(	is)	!=0	)	{
							emitError("Checkpoint	restore	failed.")	;
				}


				//	Search	for	stls	and	restore	them
				if(do_restore_stl)	{
								for	(	pos=alloc_info_map.begin()	;	pos!=alloc_info_map.end()	;	pos++	)	{
												restore_stls(pos->second)	;
								}
				}

				//	Go	through	all	of	the	allocations	that	have	been	created	looking
				//	for	those	whose	names	start	with	the	temporary-variable	prefix	and:
				pthread_mutex_lock(&mm_mutex);
				for	(	pos=alloc_info_map.begin()	;	pos!=alloc_info_map.end()	;	pos++	)	{

								alloc_info	=	pos->second;


								if	(	alloc_info->stcl	==	TRICK_LOCAL)	{

												//	If	the	temporary-variable	prefix	occurs	at	the	beginning	of	the	name	...
												if	(	(alloc_info->name	!=NULL)	&&
																	((strstr(	alloc_info->name,	local_anon_var_prefix))	==	alloc_info->name	))	{

																std::string	name	=	alloc_info->name;

																//	1)	Unregister	the	associated	variable.
																variable_map.erase(	name);

																//	2)	free	the	name
																free(	alloc_info->name);
																alloc_info->name	=	NULL;

												}
								}
				}
				pthread_mutex_unlock(&mm_mutex);
				return(0);
}

int	Trick::MemoryManager::read_checkpoint(	const	char*	filename,	bool	restore_stls	)	{

				//	Create	a	stream	from	the	named	file.
				std::ifstream	infile(filename	,	std::ios::in);
				if	(infile.is_open())	{
								return	(	read_checkpoint(	&infile,	restore_stls	))	;
				}	else	{
								std::stringstream	message;
								message	<<	"Couldn't	open	\""	<<	filename	<<	"\"."	;
								emitError(message.str());
				}
				return	1;
}

int	Trick::MemoryManager::read_checkpoint_from_string(const	char*	s,	bool	restore_stls	)	{

				//	Create	a	stream	from	the	string	argument.
				std::stringstream	ss;

				if	(	s!=	NULL)	{
								ss	<<	s;
								if	(	ss.str().find(';')	==	std::string::npos	)	{
												/*	Unlike	python,	the	old	Trick	input	parser	requires	a	semicolon
													*	after	each	input	file	assignment	command.	Append	one	if	missing.
													*/
												ss	<<	";"	;
								}
								return	(	read_checkpoint(	&ss,	restore_stls));
				}	else	{
								emitError("Checkpoint	string	is	NULL.")	;
				}
				return	1;
}

int	Trick::MemoryManager::init_from_checkpoint(const	char*	filename,	bool	restore_stls	)	{

				if	(debug_level)	{
								std::cout	<<	std::endl	<<	"Initializing	from	checkpoint"	<<	std::endl;
								std::cout	<<	std::endl	<<	"-	Resetting	managed	memory."	<<	std::endl;
								std::cout.flush();
				}

				reset_memory();

				if	(debug_level)	{
								std::cout	<<	std::endl	<<	"-	Reading	checkpoint."	<<	std::endl;
								std::cout.flush();
				}

				read_checkpoint(	filename,	restore_stls);

				if	(debug_level)	{
								std::cout	<<	std::endl	<<	"Initialization	from	checkpoint	finished."	<<	std::endl;
								std::cout.flush();
				}

				return	0	;
}

int	Trick::MemoryManager::init_from_checkpoint(	std::istream	*is,	bool	restore_stls)	{

				if	(debug_level)	{
								std::cout	<<	std::endl	<<	"Initializing	from	checkpoint"	<<	std::endl;
								std::cout	<<	std::endl	<<	"-	Resetting	managed	memory."	<<	std::endl;
								std::cout.flush();
				}

				reset_memory();

				read_checkpoint(	is,	restore_stls	);

				if	(debug_level)	{
								std::cout	<<	std::endl	<<	"Initialization	from	checkpoint	finished."	<<	std::endl;
								std::cout.flush();
				}

				return	0	;
}
