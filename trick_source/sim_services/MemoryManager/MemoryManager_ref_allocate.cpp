#include	<iostream>
#include	<sys/types.h>
#include	<string.h>
#include	<sstream>

#include	"trick/attributes.h"
#include	"trick/reference.h"
#include	"trick/MemoryManager.hh"

int	Trick::MemoryManager::ref_allocate(REF2	*	R	,	int	num	)	{

				const	char	*type_name;

				/**	@par	Design:	*/

				/**	@li	Validate	Parameters.*/
				if	(R	==	NULL)	{
								emitError("R	is	NULL	in	call	to	ref_allocate(R,num).")	;
								return	(1);
				}
				if	(R->attr	==	NULL)	{
								emitError("R->attr	is	NULL	in	call	to	ref_allocate(R,num).")	;
								return	(1);
				}
				if	(num	<=	0)	{
								emitError("num	is	<=	0	in	call	to	ref_allocate(R,num).")	;
								return	(1);
				}

				type_name	=	R->attr->type_name;

				/**	@li	If	the	reference	address	is	NULL,	call	declare_var()	to	allocate	an	anonymous,
								one	dimensional	array	the	elements	of	which	are	specified	by	R->attr->type_name
								and	whose	length	is	specified	by	@b	num.	Otherwise,	if	the	address	is	not	NULL
								then	call	reallocate	to	resize	the	array.
					*/
				if	(	*((char	**)R->address)	==	NULL)	{
								*((char	**)R->address)	=	(char*)declare_var(type_name,	num);
				}	else	{
								//	FIXME:	Is	this	correct?	Or	is	this	an	old	Bogus	kludge?
								*((char	**)R->address)	=	(char*)resize_array(	*((char	**)	R->address),	num);
				}

				/**	@li	Return	the	error	status.*/
				return	(0);

}
