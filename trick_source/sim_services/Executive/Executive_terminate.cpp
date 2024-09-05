
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/exec_proto.h"

int	Trick::Executive::exec_terminate(const	char	*file_name,	const	char	*error)	{
				exec_command	=	ExitCmd	;
				throw	Trick::ExecutiveException(-1	,	file_name	,	0	,	error)	;
				return	-1	;
}
