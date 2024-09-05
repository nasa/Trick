#include	"trick/MemoryManager.hh"
#include	<string.h>

/**
	*	MEMBER	FUNCTION
	*/
int	Trick::MemoryManager::get_enumerated(const	char*	name,
																																									V_DATA*					v_data)	{

				int	ii;
				int	found	=	0;
				ENUMERATION_MAP::iterator	pos;
				ENUM_ATTR*	enum_attr;

				for	(	pos=enumeration_map.begin()	;	pos	!=	enumeration_map.end()	&&	!found	;	pos++	)	{
								enum_attr	=	pos->second;
								for	(ii=0	;	enum_attr[ii].label[0]	!=	'\0'	&&	!found	;	ii++	)	{
												if	(	!strcmp(	name,	enum_attr[ii].label))	{
														found	=	1;
														v_data->type	=	TRICK_ENUMERATED;
														v_data->value.i	=	enum_attr[ii].value;
												}
								}
				}

				if	(!found)	{
								return	(	MM_ERROR);
				}

				return	(MM_OK);
}

