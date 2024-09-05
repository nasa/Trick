
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	Set	the	mode	to	Freeze.		Requirement		[@ref	r_exec_mode_2]
-#	Call	freeze_init	jobs.		Requirement	[@ref	r_exec_discrete_2].
-#	While	mode	is	Freeze
				-#	Call	the	freeze	jobs	Requirement	[@ref	r_exec_periodic_1].
				-#	If	the	exec_command	is	set	to	RunCmd,	set	the	mode	to	Run.
				-#	If	the	exec_command	is	set	to	ExitCmd,	call	exec_terminate_with_return
-#	Call	unfreeze	jobs.		Requirement	[@ref	r_exec_discrete_3].
*/
int	Trick::Executive::freeze_loop()	{

				int	ret	;

				/*	Set	the	mode	to	Freeze	*/
				mode	=	Freeze;

				/*	Execute	the	Freeze	Init	Jobs.	*/
				freeze_init_queue.reset_curr_index()	;
				while	(	(curr_job	=	freeze_init_queue.get_next_job())	!=	NULL	)	{
								curr_job->call()	;
				}

				message_publish(MSG_INFO,	"Freeze	ON.	Simulation	time	holding	at	%f	seconds.\n"	,	get_sim_time())	;

				while	(mode	==	Freeze)	{

								/*	Run	the	Freeze	Scheduled	Jobs.	*/
								while	(	freeze_time_tics	<	next_freeze_frame_check_tics	)	{
												/*	Call	all	freeze	loop	jobs	that	are	scheduled	to	run	at	the	current	freeze	loop	time	step.	*/
												freeze_scheduled_queue.reset_curr_index()	;
												while	(	(curr_job	=	freeze_scheduled_queue.find_next_job(	freeze_time_tics	))	!=	NULL	)	{
																ret	=	curr_job->call()	;
																if	(	ret	!=	0	)	{
																				exec_terminate_with_return(ret	,	curr_job->name.c_str()	,	0	,	"job	did	not	return	0")	;
																}
																if	(	curr_job->system_job_class	)	{
																				freeze_scheduled_queue.test_next_job_call_time(curr_job	,	freeze_time_tics)	;
																}
												}
												//	Get	the	next	job	call	time.		Set	the	freeze	clock	to	this	time
												freeze_time_tics	=	freeze_scheduled_queue.get_next_job_call_time()	;

												//	If	the	next	freeze	frame	time	is	smaller	than	then	current	freeze	time,	set	it	to	the	frame	time
												if	(	next_freeze_frame_check_tics	<	freeze_time_tics	)	{
																freeze_time_tics	=	next_freeze_frame_check_tics	;
												}
												//	Start	the	next	job	call	time	for	the	next	loop	to	be	1	software	frame	from	the	current	time.
												freeze_scheduled_queue.set_next_job_call_time(freeze_time_tics	+	freeze_frame_tics)	;
								}
								freeze_frame_count++	;
								next_freeze_frame_check_tics	+=	freeze_frame_tics	;

								/*	Enter	loop	that	continually	executes	the	Freeze	Jobs.	*/
								freeze_queue.reset_curr_index()	;
								while	(	(curr_job	=	freeze_queue.get_next_job())	!=	NULL	)	{
												ret	=	curr_job->call()	;
												if	(	ret	!=	0	)	{
																exec_terminate_with_return(ret	,	curr_job->name.c_str()	,	0	,	"job	did	not	return	0")	;
												}
								}

								/*	Exit	loop	executing	freeze	jobs	if	commanded	from	run()	*/
								if	(exec_command	==	RunCmd)	{
												mode	=	Run	;
												exec_command	=	NoCmd	;
								}	else	if	(	exec_command	==	FreezeCmd	)	{
												/*	redundant	freeze	command.		Clear	it.	*/
												exec_command	=	NoCmd	;
								}

								/*	Call	Executive::exec_terminate_with_return(int	,	const	char	*	,	int	,	const	char	*)
											if	exec_command	equals	ExitCmd.	*/
								if	(exec_command	==	ExitCmd)	{
												exec_terminate_with_return(	0	,	__FILE__	,	__LINE__	,	"Sim	control	Shutdown"	)	;
								}


				}

				message_publish(MSG_INFO,"Freeze	OFF.\n")	;

				/*	Execute	the	Unfreeze	Jobs.	*/
				unfreeze_queue.reset_curr_index()	;
				while	(	(curr_job	=	unfreeze_queue.get_next_job())	!=	NULL	)	{
								curr_job->call()	;
				}


				return(0)	;

}
