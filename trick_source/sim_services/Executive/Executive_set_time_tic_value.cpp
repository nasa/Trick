
#include	<iostream>
#include	<math.h>

#include	"trick/Executive.hh"
#include	"trick/TrickConstant.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

int	Trick::Executive::set_time_tic_value(int	in_tics)	{

				int	ret	;
				std::priority_queue<	long	long,	std::vector<	long	long	>,	std::greater<	long	long	>	>	new_freeze_times	;

				old_time_tic_value	=	time_tic_value	;

				time_tic_value	=	in_tics	;

				//	set	the	JobData	copy	of	the	new	tic	value
				Trick::JobData::set_time_tic_value(in_tics)	;

				/*	reset	time	tics	*/
				time_tics	=	(long	long)round((double)time_tics	/	old_time_tic_value	*	time_tic_value)	;

				/*	reset	software	frames	*/
				software_frame_tics	=	(long	long)(software_frame	*	time_tic_value)	;
				freeze_frame_tics	=	(long	long)(freeze_frame	*	time_tic_value)	;

				/*	reset	job	times	*/
				reset_job_cycle_times()	;
				reset_job_call_times()	;

				/*	reset	freeze	times	*/
				while	(	!	freeze_times.empty()	)	{
								new_freeze_times.push(	(long	long)round((freeze_times.top()	/	old_time_tic_value	)	*	time_tic_value)	)	;
								freeze_times.pop()	;
				}
				freeze_times	=	new_freeze_times	;

				if	(	freeze_job	!=	NULL	)	{
								if	(	freeze_job->next_tics	!=	TRICK_MAX_LONG_LONG	)	{
												freeze_job->next_tics	=		(long	long)round((freeze_job->next_tics	/	old_time_tic_value)	*	time_tic_value)	;
								}
				}

				/*	reset	stop	time	*/
				if	(	terminate_time	!=	TRICK_MAX_LONG_LONG-1	)	{
								terminate_time	=	(long	long)round((terminate_time	/	old_time_tic_value)	*	time_tic_value)	;
				}

				/*	call	jobs	for	other	objects	that	need	to	know	the	time_tic	changed.	*/
				time_tic_changed_queue.reset_curr_index()	;
				/*	job	pointer	for	each	job	in	time_tic_changed_queue	*/
				JobData*	next_job;
				while	(	(next_job	=	time_tic_changed_queue.get_next_job())	!=	NULL	)	{
								ret	=	next_job->call()	;
								if	(	ret	!=	0	)	{
												message_publish(MSG_ERROR,	"ERROR:	time_tic_changed	job:	%s	FAILED.\n",	curr_job->name.c_str())	;
												return(ret)	;
								}
				}

				//	reset	the	thread	cycle	times
				unsigned	int	ii	;
				for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
								threads[ii]->time_tic_changed(old_time_tic_value	,	time_tic_value)	;
				}

				return(0)	;
}

