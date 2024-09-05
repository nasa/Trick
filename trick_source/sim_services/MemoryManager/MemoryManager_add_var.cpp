
#include	<string.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<iostream>
#include	<sstream>

#include	"trick/value.h"
#include	"trick/vval.h"
#include	"trick/MemoryManager.hh"

void*	Trick::MemoryManager::add_var(	TRICK_TYPE			type,
																																					const	char*		class_name,
																																					VAR_DECLARE*	var_declare,
																																					char*								units	__attribute__	((unused)))	{

				int	ii;
				int	n_stars;
				int	n_cdims;
				int	cdims[TRICK_MAX_INDEX];
				VARIABLE_MAP::iterator	variable_pos;
				std::string	class_name_string;
				void*	address;

				/**	@par	Design	Details:	*/

				class_name_string	=	"";
				if	(class_name)	{
								class_name_string	=	class_name;
				}

				/**	@li	Determine	whether	the	name	of	the	given	variable	declaration	is	already	in	use.	*/
				pthread_mutex_lock(&mm_mutex);
				variable_pos	=	variable_map.find(	var_declare->name);
				if	(variable_pos	!=	variable_map.end())	{
								std::stringstream	ss;
								ss	<<	"Variable	\""<<		var_declare->name	<<	"\"	is	already	declared.\n";
								emitError(ss.str());
								pthread_mutex_unlock(&mm_mutex);
								return	((void*)NULL);
				}
				pthread_mutex_unlock(&mm_mutex);

				/**	@li	From	VAR_DECLARE,	extract:	1)	unconstrained	dimensions
								(i.e.,	n_stars)	and	2)	constrain	dimensions	(i.e.,	ndims,	dims[]).	*/
				n_stars	=	var_declare->num_pointers;
				n_cdims	=	var_declare->dim_list->num_val	;
				for	(	ii=0	;	ii<	n_cdims;	ii++)	{
								cdims[ii]	=	vval_int(	&var_declare->dim_list->v_data[ii]);
				}

				/**	@li	Call	declare_var	to	create	a	named-allocation.	*/
				if	((address	=	declare_var(	type,	class_name_string,	n_stars,	var_declare->name,	n_cdims,	cdims))	==	NULL)	{
								std::stringstream	ss;
								ss	<<	"Declaration	\""	;
								ss	<<	make_decl_string(	type,	class_name_string,	n_stars,	var_declare->name,	n_cdims,	cdims);
								ss	<<	"\"	failed.";
								emitError(	ss.str())	;
								return	((void*)NULL);
				}

				/**	@li	Return	the	address	of	the	allocation.	*/
				return(address);
}

/**
	@par	Detailed	Description:
	*/
int	Trick::MemoryManager::add_vars(	TRICK_TYPE			type,
																																				const	char*		class_name,
																																				VAR_LIST*				var_list,
																																				char*								units)	{
				int	i;
				int	ret	=	0;

				/**	@par	Design:	*/

				/**	@li	For	each	of	the	declarators	in	the	VAR_LIST	call	add_var().	*/
				for	(i	=	0;	i	<	var_list->num_vars;	i++)	{
								if(	add_var(type,	class_name,	&var_list->var_declare[i],	units)	==	NULL)	{
												ret	=	1;
								}
				}
				/**	@li	Return	the	error	status.	*/
				return	(ret);
}
