
#include	<string>

std::string	stl_type_name_convert(std::string	in_type)	{
				std::string	type_string(in_type)	;
				std::string	basic_string("std::__1::basic_string")	;
				std::string	cxx11_string("std::__cxx11::basic_string")	;
				if	(	!type_string.compare(0,	basic_string.length(),	basic_string)	||
									!type_string.compare(0,	cxx11_string.length(),	cxx11_string))	{
								type_string	=	"std::string"	;
				}
				return	type_string	;
}
