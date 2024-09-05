
#include	<iostream>
#include	<sstream>
#include	<stdlib.h>

#include	"trick/Environment.hh"

Trick::Environment	*	the_env	;

Trick::Environment::Environment()	{
				the_env	=	this	;
}

int	Trick::Environment::add_var(	std::string	variable_name	,	std::string	value	)	{

				local_env[variable_name]	=	value	;

				return(0)	;

}

const	char	*	Trick::Environment::get_var(	std::string	variable_name)	{

				char	*ret_env;
				std::map<std::string,	std::string	>::iterator	it	;

				/*	var	might	be	in	environment	*/
				if	((ret_env	=	getenv(variable_name.c_str()))	!=	NULL)	{
								return	(ret_env);
				}

				/*	Check	environment	compiled	into	S_source.c	*/
				it	=	local_env.find(variable_name)	;
				if	(	it	!=	local_env.end()	)	{
								return(	it->second.c_str()	)	;
				}

				/*	It's	not	there!	*/
				return	(NULL);

}

int	Trick::Environment::print_vars(	FILE	*	fp	)	{

				std::map<std::string,	std::string	>::iterator	it	;

				for	(	it	=	local_env.begin()	;	it	!=	local_env.end()	;	++it	)	{
								fprintf(fp,	"%s	=	%s\n",	it->first.c_str(),	it->second.c_str());
				}

				return(0)	;

}

