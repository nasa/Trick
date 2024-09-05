#include	"trick/MemoryManager.hh"
#include	<sstream>
#include	<string.h>

ALLOC_INFO*	Trick::MemoryManager::get_alloc_info_of(	void*	addr)	{

				ALLOC_INFO_MAP::iterator	pos	=	alloc_info_map.lower_bound(addr);
				if	(pos	!=	alloc_info_map.end())	{
								ALLOC_INFO*	alloc_info	=	pos->second;
								if	((	addr	>=	alloc_info->start)	&&	(	addr	<=	alloc_info->end))	{
												return(alloc_info);
								}
				}
				return	NULL;
}

ALLOC_INFO*	Trick::MemoryManager::get_alloc_info_at(	void*	addr)	{

				ALLOC_INFO_MAP::iterator	pos	=	alloc_info_map.find(	addr);
				if	(pos	==	alloc_info_map.end())	{
								return	NULL;
				}	else	{
								return(	pos->second);
				}
}

int	Trick::MemoryManager::set_name_at(	void*	addr	,	const	char	*	name	)	{

				int	ret	=	0	;

				ALLOC_INFO_MAP::iterator	pos	=	alloc_info_map.find(	addr);
				if	(pos	!=	alloc_info_map.end())	{
								pos->second->name	=	strdup(name)	;
								pthread_mutex_lock(&mm_mutex);
								VARIABLE_MAP::iterator	variable_pos;
								variable_pos	=	variable_map.find(name);
								if	(variable_pos	!=	variable_map.end())	{
												std::stringstream	ss;
												ss	<<	"Call	to	set_name_at()	failed	because	a	variable	named	\""
															<<	name	<<	"\"	already	exists.";
												emitError(ss.str());
												pthread_mutex_unlock(&mm_mutex);
												ret	=	-1	;
								}	else	{
												variable_map[name]	=	pos->second	;
								}
								pthread_mutex_unlock(&mm_mutex);
				}	else	{
								ret	=	-1	;
				}
				return	ret	;
}
