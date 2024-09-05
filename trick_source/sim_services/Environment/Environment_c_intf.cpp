
#include	<iostream>
#include	<stdio.h>

#include	"trick/Environment.hh"
#include	"trick/env_proto.h"

extern	Trick::Environment	*	the_env	;

/**
	@relates	Trick::Environment
	@copydoc	Trick::Environment::add_var
	*/
extern	"C"	int	env_add_var(	const	char	*	variable_name,	const	char	*	value)	{

				std::string	var_string	;
				std::string	value_string	;

				//	Add	handling	for	NULL	string.
				if	(	variable_name	==	NULL	)	{
								var_string.clear()	;
				}	else	{
								var_string	=	std::string(variable_name)	;
				}

				if	(	value	==	NULL	)	{
								value_string.clear()	;
				}	else	{
								value_string	=	std::string(value)	;
				}

				return	(	the_env->add_var(var_string,	value_string)	);

}

/**
	@relates	Trick::Environment
	@copydoc	Trick::Environment::get_var
	*/
extern	"C"	const	char	*	get_trick_env(	char	*	variable_name	)	{
				return	(	env_get_var(variable_name)	)	;
}

/**
	@relates	Trick::Environment
	@copydoc	Trick::Environment::get_var
	*/
extern	"C"	const	char	*	env_get_var(	const	char	*	variable_name	)	{

				std::string	var_string	;

				//	Add	handling	for	NULL	string.
				if	(	variable_name	==	NULL	)	{
								var_string.clear()	;
				}	else	{
								var_string	=	std::string(variable_name)	;
				}

				return	(	the_env->get_var(var_string)	);

}

/**
	@relates	Trick::Environment
	@copydoc	Trick::Environment::print_vars
	*/
extern	"C"	int	env_print_vars(	FILE	*	fp)	{
				return	(	the_env->print_vars(fp)	)	;
}
