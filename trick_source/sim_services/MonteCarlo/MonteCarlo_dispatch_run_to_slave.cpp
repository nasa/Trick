
#include	<iomanip>
#include	<sstream>
#include	<sys/time.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/tc_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

void	Trick::MonteCarlo::dispatch_run_to_slave(MonteRun	*run,	MonteSlave	*slave)	{
				if	(slave	&&	run)	{
								current_run	=	run->id;
								if	(prepare_run(run)	==	-1)	{
												return;
								}
								slave->state	=	MonteSlave::MC_RUNNING;
								connection_device.hostname	=	(char*)slave->machine_name.c_str();
								connection_device.port	=	slave->port;
								if	(tc_connect(&connection_device)	==	TC_SUCCESS)	{
												std::stringstream	buffer_stream;
												buffer_stream	<<	slave_output_directory	<<	"/RUN_"	<<	std::setw(5)	<<	std::setfill('0')	<<	run->id;
												std::string	buffer	=	"";
												for	(std::vector<std::string>::size_type	j	=	0;	j	<	run->variables.size();	++j)	{
																buffer	+=	run->variables[j]	+	"\n";
												}
												buffer	+=	std::string("trick.set_output_dir(\"")	+	buffer_stream.str()	+	std::string("\")\n");
												buffer_stream.str("");
												buffer_stream	<<	run->id	;
												buffer	+=	std::string("trick.mc_set_current_run(")	+	buffer_stream.str()	+	std::string(")\n");

												if	(verbosity	>=	MC_INFORMATIONAL)	{
																message_publish(MSG_INFO,	"Monte	[Master]	Dispatching	run	%d	to	%s:%d.\n",
							run->id,	slave->machine_name.c_str(),	slave->id)	;
												}

												int	command	=	htonl(MonteSlave::MC_PROCESS_RUN);
												tc_write(&connection_device,	(char	*)&command,	(int)sizeof(command));
												int	num_bytes	=	htonl(buffer.length());
												tc_write(&connection_device,	(char*)&num_bytes,	(int)sizeof(num_bytes));
												tc_write(&connection_device,	(char*)buffer.c_str(),	(int)buffer.length());

												if	(verbosity	>=	MC_ALL)	{
																message_publish(MSG_INFO,	"Parameterization	of	run	%d	:\n%s\n",	run->id,	buffer.c_str())	;
												}

												tc_disconnect(&connection_device);

												++slave->num_dispatches;
												slave->current_run	=	run;

												struct	timeval	time_val;
												gettimeofday(&time_val,	NULL);
												run->start_time	=	time_val.tv_sec	+	(double)time_val.tv_usec	/	1000000;
												++run->num_tries;
								}	else	{
												slave->state	=	Trick::MonteSlave::MC_DISCONNECTED;
												if	(verbosity	>=	MC_ERROR)	{
																message_publish(MSG_ERROR,	"Monte	[Master]	Failed	to	connect	to	%s:%d	to	dispatch	run.\n",
																																slave->machine_name.c_str(),	slave->id)	;
												}
								}
				}
}
