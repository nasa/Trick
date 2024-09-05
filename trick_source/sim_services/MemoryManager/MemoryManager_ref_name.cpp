/*
			PURPOSE:	(Construct	a	parse	tree	representation	of	a	parameter	reference	name	from	the	input	file.)

			PROGRAMMERS:	(((Robert	W.	Bailey)	(LinCom)	(7/95)	(CR#600)	(--))	((Robert	W.	Bailey)	(LinCom	Corp)	(Feb	1991)	(v1.0)
			(Initial	Release.)))	*/

#include	<string.h>
#include	<sstream>

#include	"trick/MemoryManager.hh"
#include	"trick/attributes.h"
#include	"trick/reference.h"
#include	"trick/parameter_types.h"
#include	"trick/mm_error.h"
#include	<stdlib.h>

//FIXME	TODO	make	a	error	file
///////	MM_NO_ERROR	0
#define	MM_PARAMETER_NAME	1
#define	MM_INPUT_NOT_ALLOWED	2


int	Trick::MemoryManager::ref_name(REF2	*	R,	char	*name)	{

				int	ii;
				char	*addr;
				ATTRIBUTES	*attr;

				attr	=	(ATTRIBUTES*)(R->attr->attr);

				if	(attr	==	NULL)	{
								return	(MM_PARAMETER_NAME);
				}

				if	(R->address	==	NULL)	{
								std::stringstream	message;
								message	<<	"ref_name:	Because	the	address	of	\""	<<	R->reference
																<<	"\"	is	NULL,	a	legitimate	address	can't	be	calculated	for	\""
																<<	R->reference	<<	"."	<<	name	<<	"\".";
								emitError(message.str());
								return	(MM_PARAMETER_NAME);
				}

				/*	Find	the	parameter	name	at	this	level	in	the	parameter	list,	'ii'	is	the	index	to	the	parameter	in	the	list.	*/
				ii	=	0;
				while	(strcmp(name,	attr[ii].name))	{
								if	(attr[ii].name[0]	==	'\0')
												return	(MM_PARAMETER_NAME);
								ii++;
				}

				attr	=	&(attr[ii]);

//				R->deprecated	|=	(attr->mods	&	0x80000000);

/*	Set	error_attr	just	in	case	we	have	an	error	*/
//				R->error_attr	=	attr;

				/*	If	the	variable	is	a	static	var,	offset	is	the	absolute	address	*/
				if	(attr->mods	&	2)	{
								addr	=	(char	*)	attr->offset;
								if	(	R->create_add_path	)	{
												DLLPOS	list_pos	;
												ADDRESS_NODE	*	address_node	;
												list_pos	=	DLL_GetHeadPosition(R->address_path)	;
												while	(	(list_pos	!=	NULL)	&&	(address_node	=	(ADDRESS_NODE	*)DLL_GetNext(&list_pos,	R->address_path))	!=	NULL	)	{
																if	(	(list_pos	!=	NULL)	&&	(list_pos->info	!=	NULL)	)	{
																				delete	(ADDRESS_NODE	*)(list_pos->info)	;
																}
												}
												DLL_RemoveAll(R->address_path)	;
												address_node	=	new	ADDRESS_NODE	;
												address_node->operator_	=	AO_ADDRESS	;
												address_node->operand.address	=	addr	;
												DLL_AddTail(address_node	,	R->address_path)	;
								}
				}	else	{
								addr	=	(char	*)R->address	+	attr->offset;
								if	(	R->create_add_path	)	{
												ADDRESS_NODE	*	address_node	;

												if	(	attr->offset	!=	0	)	{
																address_node	=	(ADDRESS_NODE	*)DLL_GetAt(DLL_GetTailPosition(R->address_path),	R->address_path)	;
																switch	(	address_node->operator_	)	{
																				case	AO_ADDRESS:
																								address_node->operand.address	=	(void	*)((char	*)address_node->operand.address	+		attr->offset)	;
																								break	;
																				case	AO_DEREFERENCE:
																								address_node	=	new	ADDRESS_NODE	;
																								address_node->operator_	=	AO_OFFSET	;
																								address_node->operand.offset	=	attr->offset	;
																								DLL_AddTail(address_node	,	R->address_path)	;
																								break	;
																				case	AO_OFFSET:
																								address_node->operand.offset	+=	attr->offset	;
																								break	;
																}
												}
								}
				}

				if	(attr->mods	&	1)	{
												ADDRESS_NODE	*	address_node	=	new	ADDRESS_NODE	;
												address_node->operator_	=	AO_DEREFERENCE	;
												address_node->operand.address	=	NULL	;
												DLL_AddTail(address_node	,	R->address_path)	;

												addr	=	*(char**)addr;
				}

				/*	Save	the	address	and	next	attributes	in	the	REF	structure.
							If	the	attributes	are	dynamically-allocated,	reference	attributes
							then	free	them	so	we	don't	leak	memory.
				*/
				if	(R->attr	==	R->ref_attr)	{
								free(R->ref_attr);
								R->ref_attr	=	NULL;
				}
				R->attr	=	attr;
				R->address	=	addr;


				return	(MM_OK);
}
