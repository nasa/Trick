
#include	<iostream>
#include	<udunits2.h>

ut_system	*	init_unit_system()	{
				ut_system	*	u_system	;
				/*	Initialize	the	udunits-2	library	*/
				ut_set_error_message_handler(ut_ignore)	;
				if(	(u_system	=	ut_read_xml(	NULL	))	==	NULL	)	{
								std::cerr	<<	"Error	initializing	udunits-2	unit	system"	<<	std::endl	;
								return	NULL	;
				}
				ut_set_error_message_handler(ut_write_to_stderr)	;
				return	u_system	;
}

//	Global	units	system	pointer
ut_system	*	u_system	=	init_unit_system()	;

