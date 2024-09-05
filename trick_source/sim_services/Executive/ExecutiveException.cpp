
#include	<iostream>
#include	<sstream>

#include	"trick/ExecutiveException.hh"

Trick::ExecutiveException::ExecutiveException(int	in_ret	,	std::string	in_file	,	int	line	,	std::string	in_message	)	{

				std::ostringstream	o	;

				ret_code	=	in_ret	;
				o	<<	in_file	<<	":"	<<	line	;
				file	=	o.str()	;
				message	=	in_message	;

}

Trick::ExecutiveException::~ExecutiveException()	throw	()	{
}

const	char*	Trick::ExecutiveException::what()	const	throw()	{
				return	message.c_str()	;
}

