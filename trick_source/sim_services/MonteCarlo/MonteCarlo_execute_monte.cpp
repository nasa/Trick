
#include	<sys/resource.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**	@par	Detailed	Design:	*/
int	Trick::MonteCarlo::execute_monte()	{

				if	(enabled)	{
								if	(is_master())	{
												if	(master_init()	!=	0)	{
																message_publish(MSG_ERROR,	"Monte	:	An	error	occurred	during	Monte	Carlo	initialization.	Exiting.\n")	;
																exit(0);
												}
												master();
								}	else	{
												slave_init();
												execute_as_slave();
								}
				}
				return(0);
}
