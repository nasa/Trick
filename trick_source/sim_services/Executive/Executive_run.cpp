
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"
#include	"trick/Message_proto.hh"

int	Trick::Executive::run()	{

				/**	@par	Detailed	Design	*/
				/**	@li	Set	exec_command	to	RunCmd.		Requirement	[@ref	r_exec_mode_5]	*/
				exec_command	=	RunCmd	;
				return(0)	;

}
