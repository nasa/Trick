
#include	<sys/time.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/ExecutiveException.hh"

/**	@par	Detailed	Design:	*/
int	Trick::MonteCarlo::master()	{
				struct	timeval	time_val;
				gettimeofday(&time_val,	NULL);
				start_time	=	time_val.tv_sec	+	(double)time_val.tv_usec	/	1000000;

				/**	<ul><li>	Run	the	Phase	0	master	initialization	jobs.	*/
				run_queue(&master_init_queue,	"in	master_initialization	queue")	;

				/**	<li>	If	this	is	a	dry	run:	*/
				if	(dry_run)	dryrun()	;

				try	{
								/**	<li>	While	we	do	not	have	a	result	for	every	run:	*/
								while	(num_results	<	actual_num_runs)	{

												/**
													*	<ul><li>	Spawn	any	uninitialized	slaves.
													*/
												spawn_slaves();

												/**	<li>	Receive	any	finished	runs.	*/
												receive_results();

												/**	<li>	Check	to	see	if	any	dispatched	units	have	timed	out.	*/
												check_timeouts();

												/**	<li>	Dispatch	the	next	run	to	a	ready	slave.	</ul>	*/
												dispatch_run_to_slave(get_next_dispatch(),	get_ready_slave());
								}
				}	catch	(Trick::ExecutiveException	&	ex	)	{

								/*	Handle	exception	type	Trick::ExecutiveException.		Set	the	file	name	and	error	message.
											Return	the	exception	return	code.	*/
								except_return	=	ex.ret_code	;
								except_file	=	ex.file	;
								except_message	=	ex.message	;
				}	catch	(const	std::exception	&ex)	{
								except_return	=	-1;
								if	(	curr_job	!=	NULL	)	{
												except_file	=	curr_job->name	;
								}	else	{
												except_file	=	"somewhere	in	Trick::MonteCarlo::master"	;
								}
								except_message	=	ex.what();
				}	catch	(...)	{
								/*	Handle	unknown	exceptions.		Set	the	file	name	and	error	message	to	unknown.	Return	-1.	*/
								except_return	=	-1	;
								except_file	=	"somewhere	in	Trick::MonteCarlo::master"	;
								except_message	=	"unknown	error"	;
				}


				/**	<li>	Shutdown.	*/
				master_shutdown();

				return	0;
}
