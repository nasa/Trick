#include	<sys/time.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/command_line_protos.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/tc_proto.h"
#include	"trick/exec_proto.h"
#include	"trick/SysThread.hh"

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::master_shutdown()	{

				/**	<ul><li>	Run	the	user-defined	shutdown	jobs.	*/
				run_queue(&master_shutdown_queue,	"in	master_shutdown	queue")	;

				/**	<ul><li>	Shutdown	the	active	slaves.	*/
				shutdown_slaves()	;

				/**	<li>	Shut	down	the	sockets.	*/
				tc_disconnect(&listen_device);
				tc_disconnect(&connection_device);

				struct	timeval	time_val;
				gettimeofday(&time_val,	NULL)	;
				end_time	=	time_val.tv_sec	+	(double)time_val.tv_usec	/	1000000;

				/**	<li>	Print	the	statistics.	*/
				if	(!except_file.empty())	{
								message_publish(MSG_INFO,	"SIMULATION	TERMINATED	IN\n");
								message_publish(MSG_INFO,	"		PROCESS:	%d\n",	exec_get_process_id());
								message_publish(MSG_INFO,	"		ROUTINE:	%s\n",	except_file.c_str());
								message_publish(MSG_INFO,	"		DIAGNOSTIC:	%s\n\n",	except_message.c_str());
				}
				FILE	*file_ptr;
				if	(open_file(run_directory	+	std::string("/run_summary"),	&file_ptr)	==	-1)	exit(-2)	;
				print_statistics(&file_ptr)	;
				print_statistics(&stdout)	;
				fclose(file_ptr)	;

				if	(	!except_return	and	failed_runs.size()	>	0	)	{
								except_return	=	-2	;
				}

				SysThread::ensureAllShutdown();

				exit(except_return);
}

void	Trick::MonteCarlo::shutdown_slaves()	{

				if	(verbosity	>=	MC_INFORMATIONAL)	{
								message_publish(MSG_INFO,	"Monte	[Master]	Simulation	complete.	Shutting	down	slaves.\n\n")	;
				}

				for	(std::vector<MonteSlave	*>::size_type	i	=	0;	i	<	slaves.size()	;	++i)	{
								slaves[i]->state	=	MonteSlave::MC_FINISHED;
								connection_device.hostname	=	(char*)slaves[i]->machine_name.c_str();
								connection_device.port	=	slaves[i]->port;
								if	(tc_connect(&connection_device)	==	TC_SUCCESS)	{
												int	command	=	htonl(MonteSlave::MC_SHUTDOWN);
												tc_write(&connection_device,	(char*)&command,	sizeof(command));
								}
				}
}

void	Trick::MonteCarlo::print_statistics(FILE**	fp)	{

				fprintf(*fp,
						"\nMonte	Carlo	complete:	%u	runs	(%zu	successful)	(%zu	non-zero	exit	status)	(%zu	errors)	(%u	out	of	range)\n",
						num_runs,	num_results	-	failed_runs.size()	-	error_runs.size(),	failed_runs.size(),
						error_runs.size(),	num_runs	-	num_results);

				fprintf(*fp,	"\nMachine	work	unit	breakdown:\n");
				fprintf(*fp,	"----------------------------------------------------------------------\n");
				fprintf(*fp,	"%25s		%7s		(%7s)		%12s		%12s\n",
								"machine",	"units",	"units	%",	"cpu_time	(s)",	"cpu/unit	(s)");
				fprintf(*fp,	"----------------------------------------------------------------------\n");

				double	total_time	=	0.0;
				for	(std::vector<MonteSlave	*>::size_type	j	=	0;	j	<	slaves.size();	++j)	{
								fprintf(*fp,	"%3d.		%20s	%7u		(%6.2f%%)		%12.2lf		%12.2lf\n",
										slaves[j]->id,	slaves[j]->machine_name.c_str(),
										slaves[j]->num_results,
										num_results	?	slaves[j]->num_results	/	(float)num_results	*	100	:	0,
										slaves[j]->cpu_time,
										slaves[j]->num_results	?	slaves[j]->cpu_time	/	slaves[j]->num_results	:	0);
								total_time	+=	slaves[j]->cpu_time;
				}
				double	monte_time	=	end_time	-	start_time;
				double	average_time	=	actual_num_runs	?	total_time	/	actual_num_runs	:	0;
				double	effective_time	=	actual_num_runs	?	monte_time	/	actual_num_runs	:	0;
				double	speed_up	=	total_time	/	monte_time;
				double	efficency	=	speed_up	*	100.0	/	slaves.size();

				fprintf(*fp,	"\nTotal	monte	carlo	time	taken:	%.2lf\n",	monte_time);
				fprintf(*fp,	"Actual	average	time	per	unit	(sum	of	CPU	time	/	number	of	runs):	%.2lf\n",	average_time);
				fprintf(*fp,	"Effective	average	time	per	unit	(total	time	/	number	of	runs):	%.2lf\n",	effective_time);
				fprintf(*fp,	"Speedup	(sum	of	CPU	time	/	total	time):	%.2lf\n",	speed_up);
				fprintf(*fp,	"Efficency	(speedup	/	num	slaves):	%.2lf%%\n",	efficency);

				if	(failed_runs.size())	{
								fprintf(*fp,	"\nThe	following	runs	completed	with	a	non-zero	process	exit	status:\n");
								for	(const	MonteRun*	run	:	failed_runs)	{
												fprintf(*fp,	"RUN_%05d\n",	run->id);
								}
				}

				if	(error_runs.size())	{
								fprintf(*fp,	"\nThe	following	runs	failed	to	complete:\n");
								for	(const	MonteRun*	run	:	error_runs)	{
												fprintf(*fp,	"RUN_%05d	MonteRun::ExitStatus	=	%s	(%d)\n",	run->id,
														run->exit_status_string(),	run->exit_status);
								}
				}
}
