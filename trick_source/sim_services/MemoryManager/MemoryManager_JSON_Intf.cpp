#include	<iostream>
#include	"trick/MemoryManager.hh"

void	Trick::MemoryManager::write_JSON_alloc_info(std::ostream&	s,	ALLOC_INFO	*alloc_info)	{
				s	<<	"{\n";
				if	(!alloc_info->name)	{
								s	<<	"\"name\":null,\n";
				}	else	{
								s	<<	"\"name\":\""	<<	alloc_info->name	<<	"\",\n";
				}
				s	<<	"\"start\":\""	<<	alloc_info->start	<<	"\",\n";
				s	<<	"\"end\":\""	<<	alloc_info->end	<<	"\",\n";
				s	<<	"\"num\":\""	<<	alloc_info->num	<<	"\",\n";
				s	<<	"\"size\":\""	<<	alloc_info->size	<<	"\",\n";
				s	<<	"\"type\":\""	<<	trickTypeCharString(alloc_info->type,	alloc_info->user_type_name)	<<	"\",\n";
				s	<<	"\"stcl\":"	;
				if	(alloc_info->stcl	==	TRICK_LOCAL)	{
								s	<<	"\"TRICK_LOCAL\",\n";
				}
				if	(alloc_info->stcl	==	TRICK_EXTERN)	{
								s	<<	"\"TRICK_EXTERN\",\n";
				}
				s	<<	"\"language\":";
				if	(alloc_info->language	==	Language_C		)	{
								s	<<	"\"Language_C\",\n";
				}
				if	(alloc_info->language	==	Language_CPP)	{
								s	<<	"\"Language_CPP\",\n";
				}
				s	<<	"\"index\":	["	;
				for	(int	ii=0;	ii<alloc_info->num_index;	ii++)	{
								if	(ii	!=	0)	{
												s	<<	","	;
								}	
								s	<<	alloc_info->index[ii]	;
				}
				s	<<	"]}"	<<	std::endl;
}


void	Trick::MemoryManager::write_JSON_alloc_list(std::ostream&	s,	int	chunk_start,	int	chunk_size)	{

				ALLOC_INFO_MAP::iterator	pos;
				ALLOC_INFO*	alloc_info;

				int	size	=	alloc_info_map.size();
				s	<<	"{\n";
				s	<<	"\"alloc_total\":"	<<	size	<<	",\n";
				s	<<	"\"chunk_size\":"	<<	chunk_size	<<	",\n";
				s	<<	"\"chunk_start\":"	<<	chunk_start	<<	",\n";
				s	<<	"\"alloc_list\":[\n";
				pos	=	alloc_info_map.begin();
				std::advance(pos,	chunk_start);

				for	(int	count	=	0;	count	<	chunk_size	&&	pos!=alloc_info_map.end()	;	pos++,	count++)	{
								alloc_info	=	pos->second;
								if	(count	!=	0)	{
												s	<<	",\n";
								}	
								write_JSON_alloc_info(s,	alloc_info);
				}
				s	<<	"]}"	<<	std::endl;
}

