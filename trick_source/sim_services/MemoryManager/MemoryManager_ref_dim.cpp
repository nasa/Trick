#include	<stdio.h>
#include	<sys/types.h>
#include	<string.h>
#include	<sstream>

#include	"trick/MemoryManager.hh"
#include	"trick/vval.h"
#include	"trick/attributes.h"
#include	"trick/reference.h"
#include	"trick/parameter_types.h"

/*
	Updates	R,	a	reference	to	an	arrayed	object,	to	a	reference	to	the	indexed	sub-element	of	that	arrayed	object.
*/
int	Trick::MemoryManager::ref_dim(	REF2*	R,	V_DATA*	V)	{

				int	jj;
				int	item_size;

				if	(R->ref_type	!=	REF_ADDRESS)	{
								emitError("Attempt	to	index	into	a	non-address	reference	is	bogus	in	ref_dim.")	;
								return	(1);
				}

				R->num_index_left--;
				if	(R->num_index_left	<	0)	{
								/*	if	we	have	too	many	dimensions,	flag	an	error	*/
								emitError("Too	many	dimensions	in	ref_dim.\n")	;
								return	(TRICK_PARAMETER_ARRAY_SIZE);
				}

				/*Calculate	the	size	of	the	items	in	this	array.	*/
				item_size	=		R->attr->size;
				for	(jj	=	(R->attr->num_index	-	1);	jj	>	R->num_index;	jj--)	{
								if	(R->attr->index[jj].size	>	0)	{
												item_size	*=	R->attr->index[jj].size;
								}	else	{
												item_size	=	sizeof(void	*);
												R->pointer_present	=	1	;
								}
				}

				/*	if	current	dimension	is	a	constrained	...	*/
				if	(R->attr->index[R->num_index].size	!=	0)	{

								/*	for	constrained	dimensions,	we	can	check	the	validity	of	the	index	value	*/
								if	(vval_int(V)	>=	R->attr->index[R->num_index].size	||	vval_int(V)	<	0)	{
												emitError("Memory	Manager	ERROR:	Array	index	out	of	bounds.")	;
												return	(TRICK_PARAMETER_ARRAY_SIZE);
								}

				}	else	{

								R->pointer_present	=	1	;
								if	(	R->create_add_path	)	{
												ADDRESS_NODE	*	address_node	;

												address_node	=	new	ADDRESS_NODE	;
												address_node->operator_	=	AO_DEREFERENCE	;
												address_node->operand.address	=	NULL	;
												DLL_AddTail(address_node	,	R->address_path)	;
								}

								//	Dereference	the	pointer.
								R->address	=	*(void**)R->address;
								if	(	R->address	==	NULL)	{
												std::stringstream	message;
												message	<<	"Reference	("	<<	R->reference	<<	")	address	is	NULL	in	ref_dim.";
												emitError(message.str());
												return(TRICK_PARAMETER_ADDRESS_NULL)	;
								}
				}

				if	(	R->create_add_path	)	{

								ADDRESS_NODE	*	address_node	;

								if	(	vval_int(V)	>	0	)	{
												address_node	=	(ADDRESS_NODE	*)DLL_GetAt(DLL_GetTailPosition(R->address_path),	R->address_path)	;
												switch	(	address_node->operator_	)	{
																case	AO_ADDRESS:
																				address_node->operand.address	=	(void	*)((char	*)address_node->operand.address	+		vval_int(V)	*	item_size)	;
																				break	;
																case	AO_DEREFERENCE:
																				address_node	=	new	ADDRESS_NODE	;
																				address_node->operator_	=	AO_OFFSET	;
																				address_node->operand.offset	=	vval_int(V)	*	item_size	;
																				DLL_AddTail(address_node	,	R->address_path)	;
																				break	;
																case	AO_OFFSET:
																				address_node->operand.offset	+=	vval_int(V)	*	item_size	;
																				break	;
												}
								}
				}

				R->address	=		(void*)((char*)R->address	+	vval_int(V)	*	item_size);
				R->num_index++;

				return	(TRICK_NO_ERROR);
}
