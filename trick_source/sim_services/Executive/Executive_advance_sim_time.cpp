
#include	<iostream>

#include	"trick/Executive.hh"

/**
@design
-#	Save	the	current	#time_tics	to	#time_last_pass_tics
-#	Set	the	simulation	time	to	the	next	lowest	job	call	time	by
			calling	Trick::ScheduledThread::get_next_job_call_time().
-#	Wait	for	all	synchronous	threads	to	finish
			-#	If	the	thread	next_job_call_time	is	less	than	the	overall	time,	set	the	overall	time	to
						the	thread	next_jobs_call_time.
-#	Set	the	next	job	call	time	of	this	job	to	the	simulation	time
*/
int	Trick::Executive::advance_sim_time()	{

				unsigned	int	ii	;

				//	synchronize	the	scheduled	threads	before	advancing	time.
				scheduled_thread_sync()	;

				/*	Save	the	current	time	to	time_last_pass	*/
				time_last_pass_tics	=	time_tics	;

				/*	start	with	time	tics	set	to	the	next	end	of	frame	*/
				time_tics	=	next_frame_check_tics	;

				/*	Set	the	simulation	time	to	the	next	lowest	job	call	time	by
							calling	Trick::ScheduledThread::get_next_job_call_time().	*/
				long	long	next_job_call_time	=	threads[0]->job_queue.get_next_job_call_time()	;
				if	(	next_job_call_time	<	time_tics	&&	next_job_call_time	>	time_last_pass_tics	)		{
								time_tics	=		threads[0]->job_queue.get_next_job_call_time()	;
				}

				/*	Check	to	see	if	there	is	an	input	processor	event	that	happens	before	the	next	job	*/
				next_job_call_time	=	input_processor_run_queue.get_next_job_call_time()	;
				if	(	next_job_call_time	<	time_tics	&&	next_job_call_time	>	time_last_pass_tics	)		{
								time_tics	=		next_job_call_time	;
				}

				/*	Adjust	time_tics	if	one	of	the	threads	has	a	job	or	async	cycle	time	less	than	the	main	thread's	next	job	*/
				for	(ii	=	1;	ii	<	threads.size()	;	ii++)	{
								Threads	*	curr_thread	=	threads[ii]	;
								if	(	curr_thread->enabled	)	{
												if	(	(curr_thread->process_type	==	PROCESS_TYPE_SCHEDULED)	&&
																	(curr_thread->job_queue.get_next_job_call_time()	<	time_tics)	)	{
																time_tics	=	curr_thread->job_queue.get_next_job_call_time()	;
												}
												if	(	(curr_thread->process_type	==	PROCESS_TYPE_AMF_CHILD	)	&&
																	(curr_thread->amf_cycle_tics	>	0	)	&&
																	(curr_thread->amf_next_tics	<	time_tics)	)	{
																time_tics	=	curr_thread->amf_next_tics	;
												}
								}
				}

				/*	Adjust	time_tics	to	the	terminate	time	if	terminate	is	the	next	event	*/
				if	(	terminate_time	<	time_tics	)	{
								time_tics	=	terminate_time	;
				}

				//	set	the	default	next	job	call	time	to	the	next	software	frame	;
				input_processor_run_queue.set_next_job_call_time(time_tics	+	software_frame_tics)	;
				threads[0]->job_queue.set_next_job_call_time(time_tics	+	software_frame_tics)	;

				/*	Set	the	advance_sim_time()	next	job	call	time	to	be	the	new	simulation	elapsed	time.	*/
				advance_sim_time_job->next_tics	=	time_tics	;

				/*	Set	the	main	thread	current	time	to	the	simulation	time	tics	value,	used	with	Executive::get_sim_time()	*/
				threads[0]->curr_time_tics	=	time_tics	;

				return(0)	;
}

