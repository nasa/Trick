
#include	<sys/stat.h>
#include	<libgen.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/tc_proto.h"

/**	@par	Detailed	Design:	*/
int	Trick::MonteCarlo::initialize_sockets()	{

				/**	set	tc_error	to	zero	for	TCDevices	to	turn	off	advisory	messages.	*/
				tc_error(&listen_device,	0);
				tc_error(&connection_device,	0);

				/**	<ul><li>	Initialize	the	sockets	for	communication	with	slaves.	*/
				int	return_value	=	tc_init(&listen_device);
				if	(return_value	!=	TC_SUCCESS)	{
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[Master]	Failed	to	initialize	status	communication	socket.\n")	;
								}
								return	return_value;
				}
				tc_blockio(&listen_device,	TC_COMM_NOBLOCKIO);

				/**	<li>	If	no	slaves	were	specified,	add	one	on	localhost.	*/
				if	(slaves.empty())	{
								if	(verbosity	>=	MC_ALL)	{
												message_publish(MSG_WARNING,	"Monte	[Master]	No	slaves	specified.	Adding	localhost	as	the	sole	slave.\n")	;
								}
								add_slave(new	MonteSlave());
				}
				return	TC_SUCCESS	;
}
