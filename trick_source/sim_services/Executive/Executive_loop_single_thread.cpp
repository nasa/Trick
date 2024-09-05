
#include	<iostream>
#include	<fstream>
#include	<sstream>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.h"

/**
@details
-#	Enter	an	infinite	loop.		A	call	to	exec_terminate(const	char	*)	or
			exec_terminate_with_return(int,	const	char	*,	int	,	const	char	*	)	will	exit	the	loop.
				-#	If	the	exec_command	equals	freeze	mode,	call	Trick::Executive::freeze_loop().		Transition	from
							freeze	back	to	run	will	continue	execution	at	this	point.
							Requirement		[@ref	r_exec_mode_1]
				-#	Set	the	main	thread	current	time	to	the	simulation	time	tics	value
				-#	For	each	scheduled	jobs	whose	next	call	time	is	equal	to	the	current	simulation	time	[@ref	ScheduledJobQueue]
								-#	Call	the	job.		Requirement		[@ref	r_exec_periodic_0]
								-#	If	the	job	is	a	system	job,	check	to	see	if	the	next	job	call	time	is	the	lowest	next	time	by
											calling	Trick::ScheduledJobQueue::test_next_job_call_time(Trick::JobData	*,	long	long)
				-#	If	the	exec_command	equals	ExitCmd
							-#	Call	Trick::Executive::exec_terminate_with_return(int,	char	*,	int,	char	*)
				-#	If	the	elapsed	time	has	reached	the	termination	time
							-#	Call	Trick::Executive::exec_terminate_with_return(int,	char	*,	int,	char	*)
				-#	If	the	elapsed	time	equals	the	next	software	frame	time
							-#	Call	the	end_of_frame	jobs.	Requirement		[@ref	r_exec_periodic_2]
							-#	Set	the	end	of	frame	execution	time	to	the	current	time	+	software_frame
*/
int	Trick::Executive::loop_single_thread()	{

				Trick::ScheduledJobQueue	*	main_sched_queue	;
				int	ret	;

				/*	The	main	scheduler	queue	is	the	queue	in	thread	0	*/
				main_sched_queue	=	&(threads[0]->job_queue)	;

				while	(1)	{

								/*	Call	freeze_loop()	if	commanded	by	freeze()	*/
								if	(exec_command	==	FreezeCmd)	{
												exec_command	=	NoCmd;
												freeze_loop();
								}

								/*	Call	all	top	of	frame	jobs	if	the	simulation	time	equals	to	the	time	software	frame	boundary.	*/
								if	(time_tics	==	(next_frame_check_tics	-	software_frame_tics))	{
												top_of_frame_queue.reset_curr_index()	;
												while	(	(curr_job	=	top_of_frame_queue.get_next_job())	!=	NULL	)	{
																ret	=	curr_job->call()	;
																if	(	ret	!=	0	)	{
																				exec_terminate_with_return(ret	,	curr_job->name.c_str()	,	0	,	"top_of_frame	job	did	not	return	0")	;
																}
												}
												frame_count++	;
								}

								/*	Call	the	input_processor_run	queue	jobs.	*/
								input_processor_run_queue.reset_curr_index()	;
								while	(	(curr_job	=	input_processor_run_queue.find_next_job(	time_tics	))	!=	NULL	)	{
												ret	=	curr_job->call()	;
												if	(	ret	!=	0	)	{
																exec_terminate_with_return(ret	,	curr_job->name.c_str()	,	0	,	"input_processor_run	job	did	not	return	0")	;
												}
												input_processor_run_queue.test_next_job_call_time(curr_job	,	time_tics)	;
								}

								/*	Call	all	scheduled	jobs	that	are	scheduled	to	run	at	the	current	simulation	time	step.	*/
								main_sched_queue->reset_curr_index()	;
								while	(	(curr_job	=	main_sched_queue->find_next_job(	time_tics	))	!=	NULL	)	{
												//std::cout	<<	"[33mtime	=	"	<<	time_tics	<<	"	"	<<	curr_job->name	<<	"	job	next	=	"	<<	curr_job->next_tics	<<	"[00m"	<<	std::endl	;
												ret	=	curr_job->call()	;
												if	(	ret	!=	0	)	{
																exec_terminate_with_return(ret	,	curr_job->name.c_str()	,	0	,	"scheduled	job	did	not	return	0")	;
												}
												/*	System	jobs	next	call	time	are	not	set	until	after	they	run.
															Test	their	next	job	call	time	after	they	have	been	called	*/
												if	(	curr_job->system_job_class	)	{
																main_sched_queue->test_next_job_call_time(curr_job	,	time_tics)	;
												}
								}

								/*	Call	Executive::exec_terminate_with_return(int	,	const	char	*	,	int	,	const	char	*)
											if	exec_command	equals	ExitCmd.	*/
								if	(exec_command	==	ExitCmd)	{
												exec_terminate_with_return(	0	,	__FILE__	,	__LINE__	,	"Sim	control	Shutdown"	)	;
								}

								/*	We	are	already	in	run...	clear	any	redundant	exec_commands	to	go	to	run	*/
								if	(	exec_command	==	RunCmd	)	{
												exec_command	=	NoCmd	;
								}

								/*	Call	Executive::exec_terminate_with_return(	int	,	const	char	*	,	int	,	const	char	*)
											if	simulation	elapsed	time	has	reached	the	termination	time.	*/
								if	(terminate_time	<=	time_last_pass_tics	)	{
												time_tics	=	time_last_pass_tics	;
												exec_command	=	ExitCmd	;
												exec_terminate_with_return(	0	,	__FILE__	,	__LINE__	,	"Reached	termination	time"	)	;
								}

								/*	Call	all	end	of	frame	jobs	if	the	simulation	time	equals	to	the	time	software	frame	boundary.	*/
								if	(time_tics	==	next_frame_check_tics	)	{
												end_of_frame_queue.reset_curr_index()	;
												while	(	(curr_job	=	end_of_frame_queue.get_next_job())	!=	NULL	)	{
																ret	=	curr_job->call()	;
																if	(	ret	!=	0	)	{
																				exec_terminate_with_return(ret	,	curr_job->name.c_str()	,	0	,	"end_of_frame	job	did	not	return	0")	;
																}
												}

												next_frame_check_tics	+=	software_frame_tics	;
								}

				}

				return(0)	;
}

