#include	<string.h>
#include	<stdio.h>
#include	"trick/MemoryManager.hh"
#include	"trick/value.h"
#include	"trick/vval.h"

int	Trick::MemoryManager::ref_var(	REF2*	R,
																																			char*	name)	{

				ALLOC_INFO	*alloc_info;
				VARIABLE_MAP::iterator	pos;

				pthread_mutex_lock(&mm_mutex);
				pos	=	variable_map.find(	name);
				if	(pos	!=	variable_map.end())	{
								alloc_info	=	pos->second;
								R->attr	=	make_reference_attr(	alloc_info);
								R->ref_attr	=	R->attr;
								R->address	=	alloc_info->start;

								if	(	R->create_add_path	)	{
												ADDRESS_NODE	*	address_node	;

												address_node	=	new	ADDRESS_NODE	;
												address_node->operator_	=	AO_ADDRESS	;
												address_node->operand.address	=	R->address	;
												DLL_AddTail(address_node	,	R->address_path)	;
								}

								pthread_mutex_unlock(&mm_mutex);
								return	(MM_OK);
				}	else	{
								//	ERROR.
				}
				pthread_mutex_unlock(&mm_mutex);

				return	(MM_UNDEFINED_REFERENCE);
}
