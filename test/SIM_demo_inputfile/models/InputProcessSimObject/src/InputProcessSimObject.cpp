#ifndef	TRICK_ICG
#include	<Python.h>
#endif
#include	<iostream>
#include	<sstream>

#include	"InputProcessSimObject/include/InputProcessSimObject.hh"

Trick::InputProcessSimObject::InputProcessSimObject(std::string	in_function)	{

				python_call_function	=	in_function	;
				return_value	=	0	;

}

int	Trick::InputProcessSimObject::call_function(	Trick::JobData	*	curr_job	)	{

				std::stringstream	ss	;

				ss	<<	python_call_function	<<	"("	<<	curr_job->id	<<	")"	;
				PyRun_SimpleString(ss.str().c_str())	;

				return(	return_value	)	;
}

double	Trick::InputProcessSimObject::call_function_double(	Trick::JobData	*	curr_job	)	{

				(void)curr_job	;
				return(0.0)	;

}

