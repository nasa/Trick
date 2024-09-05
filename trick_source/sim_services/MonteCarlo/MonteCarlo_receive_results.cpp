#include	"trick/MonteCarlo.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/tc_proto.h"

/**
	*	@par	Detailed	Design:
	*	This	function	performs	a	non-blocking	accept	on	the	listening	socket.
	*/
void	Trick::MonteCarlo::receive_results()	{

				/**	<li>	While	there	are	pending	connections:	*/
				while	(tc_accept(&listen_device,	&connection_device)	==	TC_SUCCESS)	{

								/**	<ul><li>	Read	the	slave	id.	*/
								int	id;
								tc_read(&connection_device,	(char*)&id,	(int)sizeof(id));
								id	=	ntohl(id);
								MonteSlave*	slave	=	get_slave(id);
								if	(!slave)	{
												message_publish(
														MSG_ERROR,
														"Monte	[Master]	Slave	returned	an	invalid	id	(%d)\n",
														id)	;
												tc_disconnect(&connection_device);
												return;
								}

								/**
									*	<li>	If	the	slave	is	in	the	MC_INITIALIZING	state,	it	is	sending	us	the
									*	machine	name	and	port	over	which	it	is	listening	for	new	runs.
									*/
								if	(slave->state	==	MonteSlave::MC_INITIALIZING)	{
												handle_initialization(*slave);
								}
								/**	<li>	Otherwise,	it's	sending	us	run	data.	*/
								else	{
												handle_run_data(*slave);
								}
				}
}

void	Trick::MonteCarlo::handle_initialization(Trick::MonteSlave&	slave)	{
								if	(verbosity	>=	MC_ALL)	{
												message_publish(
														MSG_INFO,
														"Monte	[Master]	Receiving	initialization	information	from	%s:%d.\n",
														slave.machine_name.c_str(),	slave.id);
								}

								int	size;
								if	(tc_read(&connection_device,	(char*)&size,	(int)sizeof(size))	!=	(int)sizeof(size))	{
												set_disconnected_state(slave);
												return;
								}
								size	=	ntohl(size);

								char	name[size	+	1];
								name[size]	=	'\0';
								if	(tc_read(&connection_device,	name,	size)	!=	size)	{
												set_disconnected_state(slave);
												return;
								}
								slave.machine_name	=	std::string(name);

								size	=	(int)sizeof(slave.port);
								if	(tc_read(&connection_device,	(char*)&slave.port,	size)	!=	size)	{
												set_disconnected_state(slave)	;
												return;
								}
								slave.port	=	ntohl(slave.port);

								slave.state	=	MonteSlave::MC_READY;
								tc_disconnect(&connection_device);
}

void	Trick::MonteCarlo::handle_run_data(Trick::MonteSlave&	slave)	{
				if	(verbosity	>=	MC_INFORMATIONAL)	{
								message_publish(MSG_INFO,	"Monte	[Master]	Receiving	results	for	run	%d	from	%s:%d.\n",
													slave.current_run->id,	slave.machine_name.c_str(),	slave.id)	;
				}

				/**
					*	<ul><li>	Try	to	remove	this	run	from	the	queue	in	case	it	was	requeued	by	#check_timeouts.
					*	This	covers	the	case	in	which	the	master	determines	that	a	slave	has	timed	out,	requeues
					*	the	run,	and	then	the	slave	reports	results.
					*/
				dequeue_run(slave.current_run);

				/**
					*	<li>	This	run	may	have	already	been	resolved	by	another	slave	if
					*	this	slave	was	marked	as	having	timed	out.	If	that	is	the	case,
					*	discard	these	results.
					*/
				if	(slave.current_run->exit_status	!=	MonteRun::MC_RUN_INCOMPLETE)	{
								//	TODO:	If	a	slave	times	out	or	core	dumps	in	it's	monte_slave_post
								//	jobs,	the	master	will	receive	a	MC_RUN_COMPLETE	status	from	the	slave's
								//	child	process	and	then	an	error	status	from	the	parent,	rendering
								//	this	message	incorrect.
								if	(verbosity	>=	MC_ALL)	{
												message_publish(
														MSG_INFO,
														"Monte	[Master]	Run	%d	has	already	been	resolved.	Discarding	results.\n",
														slave.current_run->id)	;
								}
				}	else	{
								/**	<li>	Otherwise,	check	the	exit	status:	*/
								int	exit_status;
								int	size	=	sizeof(exit_status);
								if	(tc_read(&connection_device,	(char*)&exit_status,	size)	!=	size)	{
												set_disconnected_state(slave)	;
												return;
								}
								exit_status	=	ntohl(exit_status);

								switch	(exit_status)	{

												case	MonteRun::MC_RUN_COMPLETE:
												case	MonteRun::MC_RUN_FAILED:
																resolve_run(slave,	static_cast<MonteRun::ExitStatus>(exit_status));
																run_queue(&master_post_queue,	"in	master_post	queue")	;
																break;

												case	MonteRun::MC_PROBLEM_PARSING_INPUT:
																if	(verbosity	>=	MC_ERROR)	{
																				message_publish(
																						MSG_ERROR,
																						"Monte	[Master]	%s:%d	reported	bad	input	for	run	%d.	Skipping.\n",
																						slave.machine_name.c_str(),	slave.id,	slave.current_run->id)	;
																}
																resolve_run(slave,	MonteRun::MC_PROBLEM_PARSING_INPUT);
																break;

												case	MonteRun::MC_RUN_DUMPED_CORE:
																if	(verbosity	>=	MC_ERROR)	{
																				message_publish(
																						MSG_ERROR,
																						"Monte	[Master]	%s:%d	reported	core	dump	for	run	%d.	Skipping.\n",
																						slave.machine_name.c_str(),	slave.id,	slave.current_run->id)	;
																}
																resolve_run(slave,	MonteRun::MC_RUN_DUMPED_CORE);
																break;

												case	MonteRun::MC_CANT_CREATE_OUTPUT_DIR:
																if	(verbosity	>=	MC_ERROR)	{
																				message_publish(
																						MSG_ERROR,
																						"Monte	[Master]	%s:%d	reported	a	failure	to	create	output	directories	for	run	%d.\n",
																						slave.machine_name.c_str(),	slave.id,	slave.current_run->id);
																}
																handle_retry(slave,	MonteRun::MC_CANT_CREATE_OUTPUT_DIR);
																break;

												case	MonteRun::MC_RUN_TIMED_OUT:
																if	(verbosity	>=	MC_ERROR)	{
																				message_publish(
																						MSG_ERROR,
																						"Monte	[Master]	%s:%d	reported	a	timeout	for	run	%d.\n",
																						slave.machine_name.c_str(),	slave.id,	slave.current_run->id);
																}
																handle_retry(slave,	MonteRun::MC_RUN_TIMED_OUT);
																break;

												default:
																if	(verbosity	>=	MC_ERROR)	{
																				message_publish(
																						MSG_ERROR,
																						"Monte	[Master]	%s:%d	reported	unrecognized	exit	status	(%d)	for	run	%d.	Skipping.\n",
																						slave.machine_name.c_str(),	slave.id,	exit_status,	slave.current_run->id);
																}
																resolve_run(slave,	MonteRun::MC_UNRECOGNIZED_RETURN_CODE);
																break;
								}
				}
				
				tc_disconnect(&connection_device);

				/**	<li>	Update	the	slave's	state.	*/
				if	(slave.state	==	MonteSlave::MC_RUNNING	||	slave.state	==	MonteSlave::MC_UNRESPONSIVE_RUNNING)	{
								slave.state	=	MonteSlave::MC_READY;
				}	else	if	(slave.state	==	MonteSlave::MC_STOPPING	||	slave.state	==	MonteSlave::MC_UNRESPONSIVE_STOPPING)	{
								slave.state	=	MonteSlave::MC_STOPPED;
				}
}

void	Trick::MonteCarlo::set_disconnected_state(Trick::MonteSlave&	slave)	{
				slave.state	=	Trick::MonteSlave::MC_DISCONNECTED;
				if	(verbosity	>=	MC_ERROR)	{
								message_publish(MSG_ERROR,	"Monte	[Master]	Lost	connection	to	%s:%d.\n",
																								slave.machine_name.c_str(),	slave.id)	;
				}
				tc_disconnect(&connection_device);
}
