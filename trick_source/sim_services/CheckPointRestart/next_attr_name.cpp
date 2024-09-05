
#include	<string>
#include	"trick/CheckPointRestart_c_intf.hh"

/**
	This	function	strips	off	any	dimensions	from	the	type_id	call	in	the	io_src.
	E.g.		"blah	[3][3]"	becomes	"blah"
*/
int	next_attr_name(	std::string	&	name	)	{

				size_t	found	;

				found	=	name.find("	[")	;
				if	(	found	!=	std::string::npos	)	{
								name.erase(found)	;
				}

				return	0	;
}

