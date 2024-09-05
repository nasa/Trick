
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<sstream>
#include	"trick/MemoryManager.hh"
#include	"trick/RefParseContext.hh"

extern	int	REF_debug;

REF2	*Trick::MemoryManager::ref_attributes(const	char*	name)	{

				std::stringstream	reference_sstream;
				REF2	*	result	=	NULL;
				RefParseContext*	context	=	NULL;

				/**	@par	Design	Details:	*/
				reference_sstream	<<	name;

				REF_debug	=	0;

				/**	@li	Create	a	parse	context.	*/
				context	=	new	RefParseContext(this,	&reference_sstream);

				/**	@li	Call	REF_parse	to	parse	the	variable	reference.	*/
				if	(context	!=	NULL)	{
								if	(	REF_parse(	context)	==	0)	{
												/*	throw	away	the	reference	returned	by	the	ref_parser,	it	isn't	the	same
															name	as	requested	as	"name"	*/
												if	(	context->result->reference	)	{
																free(context->result->reference)	;
												}
												context->result->reference	=	strdup(name);
												result	=	context->result;
								}
								/**	@li	Delete	the	parse	context.	*/
								delete(	context);
				}

				/**	@li	Return	the	the	REF2	object.*/
				return	(	result);
}

