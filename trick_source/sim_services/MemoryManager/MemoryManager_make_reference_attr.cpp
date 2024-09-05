
#include	<stdlib.h>

#include	"trick/MemoryManager.hh"

static	const	char	*	nil_units	=	"1";

//	MEMBER	FUNCTION
ATTRIBUTES*	Trick::MemoryManager::make_reference_attr(	ALLOC_INFO*	alloc_info)	{

				int	ii;
				ATTRIBUTES*		reference_attr;

				//	This	must	be	a	named	allocation.

				reference_attr	=	(ATTRIBUTES	*)	calloc(1,	(int)	sizeof(ATTRIBUTES));
				reference_attr->name	=	alloc_info->name;
				reference_attr->type_name	=	NULL;
				reference_attr->units	=	nil_units;
				reference_attr->alias	=	NULL;
				reference_attr->user_defined	=	NULL;
				reference_attr->des	=	NULL;
				reference_attr->io	=	TRICK_VAR_OUTPUT	|	TRICK_VAR_INPUT	|	TRICK_CHKPNT_OUTPUT	|	TRICK_CHKPNT_INPUT	;
				reference_attr->type	=	alloc_info->type;
				reference_attr->size	=	alloc_info->size;
				reference_attr->range_min	=	0.0;
				reference_attr->range_max	=	0.0;
				reference_attr->language	=	alloc_info->language;
				reference_attr->mods	=	0;
				reference_attr->offset	=	0;
				reference_attr->attr	=	alloc_info->attr;
				reference_attr->num_index	=	alloc_info->num_index;
				for(ii=0;	ii<	reference_attr->num_index;	ii++)	{
								reference_attr->index[ii].start=	0;
								reference_attr->index[ii].size	=	alloc_info->index[ii];
				}
				return(	reference_attr);
}

//	MEMBER	FUNCTION
void	Trick::MemoryManager::free_reference_attr(	ATTRIBUTES*	reference_attr)	{

				if	(reference_attr)	{
								free	(reference_attr);
				}
}
