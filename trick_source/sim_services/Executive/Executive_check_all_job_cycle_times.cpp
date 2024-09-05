
#include	<math.h>
#include	"trick/Executive.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/TrickConstant.hh"

/**
@details
-#	Loop	through	all	jobs	checking	for	jobs	that	have	a	cycle	time	less	than	the	time_tic_value.
-#	Test	to	see	if	the	time_tic_value	is	only	divisible	by	2	and	5.
-#	Return	if	the	time_tic_value	has	factors	other	than	2	and	5.	(will	be	irrational)
-#	Loop	through	all	jobs	checking	to	see	if	the	time	tic	has	enough	resolution	to	handle	cycle	time.
*/
int	Trick::Executive::check_all_job_cycle_times()	{

				unsigned	int	ii	;
				int	ret	=	0	;
				int	temp_time_tic_value	;

				/*	Check	if	any	jobs	have	cycle	times	less	than	the	time_tic_value	*/
				for	(	ii	=	0	;	ii	<	all_jobs_vector.size()	;	ii++	)	{
								Trick::JobData	*	temp_job	=	all_jobs_vector[ii]	;
								if	(	temp_job->cycle	<	(1.0	/	time_tic_value)	)	{
												message_publish(MSG_WARNING,"Cycle	for	(%s)	is	less	than	time	tic	value.	cycle	=	%16.12f,	time_tic	=	%16.12f\n",
													temp_job->name.c_str()	,	temp_job->cycle,	1.0	/	time_tic_value	)	;
												temp_job->disabled	=	true	;
												temp_job->cycle_tics	=	TRICK_MAX_LONG_LONG	;
												temp_job->next_tics	=	TRICK_MAX_LONG_LONG	;
												ret	=	-1	;
								}
				}

				/*	Check	if	time_tic_value	is	only	divisible	by	2	and	5	*/
				temp_time_tic_value	=	time_tic_value	;
				while	(	temp_time_tic_value	%	2	==	0	)	{
								temp_time_tic_value	/=	2	;
				}
				while	(	temp_time_tic_value	%	5	==	0	)	{
								temp_time_tic_value	/=	5	;
				}
				if	(	temp_time_tic_value	!=	1	)	{
								return	ret	;
				}

				/*	Check	if	any	jobs	have	cycle	times	that	have	finer	resolution	than	time_tic_value.	*/
				for	(	ii	=	0	;	ii	<	all_jobs_vector.size()	;	ii++	)	{
								Trick::JobData	*	temp_job	=	all_jobs_vector[ii]	;
								double	test_rem	;

								/*	Calculate	the	if	the	cycle_tics	would	be	a	whole	number		*/
								test_rem	=	fmod(temp_job->cycle	*	(double)time_tic_value	,	1.0	)	;

								if	(	test_rem	>	0.001	)	{
												message_publish(MSG_WARNING,"Cycle	for	(%s)	cannot	be	exactly	scheduled	with	time	tic	value.	"
													"cycle	=	%16.12f,	cycle_tics	=	%lld	,	time_tic	=	%16.12f\n",
													temp_job->name.c_str()	,	temp_job->cycle,	temp_job->cycle_tics	,	1.0	/	time_tic_value	)	;
												ret	=	-1	;
								}
				}

				for	(	ii	=	1	;	ii	<	threads.size()	;	ii++	)	{
								Trick::Threads	*	curr_thread	=	threads[ii]	;
								if	(	curr_thread->process_type	==	PROCESS_TYPE_AMF_CHILD	and
													curr_thread->amf_cycle_tics	==	0	)	{
												message_publish(MSG_ERROR,"Asynchronous	must	finish	thread	%d	has	no	cycle	time	set\n"
													"Use	trick.exec_set_thread_amf_cycle_time(	%d	,	double	<cycle_time>	)\n"
													"You're	probably	deadlocked	here.\n"	,	ii	,	ii	)	;
												ret	=	-1	;
								}
				}

				return	ret		;

}
