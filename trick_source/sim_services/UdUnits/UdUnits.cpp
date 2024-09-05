
#include	<iostream>

#include	"trick/UdUnits.hh"

ut_system	*	Trick::UdUnits::u_system	=	NULL	;

ut_system	*	Trick::UdUnits::get_u_system()	{
				return	u_system	;
}
int	Trick::UdUnits::read_default_xml()	{
				/*	Initialize	the	udunits-2	library	*/
				ut_set_error_message_handler(ut_ignore)	;
				if(	(u_system	=	ut_read_xml(	NULL	))	==	NULL	)	{
								std::cerr	<<	"Error	initializing	udunits-2	unit	system"	<<	std::endl	;
								return	-1	;
				}
				ut_set_error_message_handler(ut_write_to_stderr)	;

				return	0	;
}

int	Trick::UdUnits::read_user_xml(std::string	file_name)	{
				(void)file_name;
				return	0	;
}


