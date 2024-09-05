/*
			PURPOSE:	(Graceful	Simulation	termination	with	timing	data	output.)

			PROGRAMMERS:
					(((Robert	W.	Bailey)	(LinCom)	(Feb	1991)	(v1.0)	(Initial	Release.))
						((Robert	W.	Bailey)	(LinCom)	(April	1992)	(--)	(Realtime	clock))
						((Scott	Killingsworth)	(LinCom)	(September	1997)	(--)	(Issue	#1016)))
	*/

#include	<stdlib.h>
#include	<fcntl.h>
#include	<iomanip>
#include	<iostream>
#include	<sys/resource.h>

#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/release.h"
#include	"trick/SysThread.hh"
#include	"trick/VariableServer.hh"
extern	Trick::VariableServer	*	the_vs	;

/**
@design
-#	Set	the	mode	to	Exit.
			Requirement		[@ref	r_exec_mode_3]
-#	Wait	for	asynchronous	or	AMF	threads	to	complete	if	the	user	has	specified	to	do	so
-#	The	scheduler	calls	shutdown	jobs
			Requirement		[@ref	r_exec_discrete_4]
-#	If	an	exception	is	caught	during	calling	the	shutdown	jobs	the	exception	is	ignored	as	we	are	already	terminating.
-#	The	scheduler	prints	a	simulation	shutdown	message
-#	The	scheduler	kills	all	child	threads
-#	Return	the	exception_return	value.		(Set	to	0	if	no	exception	thrown.)
*/
int	Trick::Executive::shutdown()	{

				double	sim_elapsed_time;
				double	cpu_time;
				double	user_cpu_time,	kernal_cpu_time;
				double	sim_to_cpu;
				unsigned	int	ii;
				int	process_id	=	0	;
				struct	rusage	cpu_usage_buf	;
				double	sim_mem;
				long	v_context_switch_run,	iv_context_switch_run;

				SIM_MODE	prev_mode	=	mode	;

				/*	Set	mode	to	ExitMode.	*/
				mode	=	ExitMode;

				/*		If	we	made	it	past	initialization	*/
				if	(	prev_mode	!=	Initialization	)	{
								/*	Wait	for	asynchronous	threads	to	complete	if	the	user	has	specified	to	wait.	*/
								for	(ii	=	1;	ii	<	threads.size()	;	ii++)	{
									if	(	(threads[ii]->process_type	==	Trick::PROCESS_TYPE_ASYNC_CHILD	||	threads[ii]->process_type	==	Trick::PROCESS_TYPE_AMF_CHILD)	&&	threads[ii]->shutdown_wait_async	)	{
																message_publish(MSG_INFO,	"Waiting	for	asynchronous	or	AMF	thread	%d	to	finish\n",	ii	)	;
																while	(threads[ii]->child_complete	==	false)	{
																				RELEASE_1();
																}
												}
								}
				}

				/*	Stop	new	connections	to	the	Variable	Server.	*/
				the_vs->shutdownConnections();

				try	{

								/*	Call	the	shutdown	jobs.	*/
								if	(initialization_complete	==	true)	{
												shutdown_queue.reset_curr_index()	;
												while	(	(curr_job	=	shutdown_queue.get_next_job())	!=	NULL	)	{
																curr_job->call()	;
												}
								}
				}	catch	(Trick::ExecutiveException	&	ex	)	{
								/*	Handle	exception	type	Trick::ExecutiveException.		Set	the	file	name	and	error	message.
											Return	the	exception	return	code.	*/
								except_return	=	ex.ret_code	;
								except_file	+=	std::string("	then	exception	caught	in	")	+	ex.file	;
								except_message	+=	std::string("	then	exception	Message:	")	+	ex.message	;
				}

				Trick::SysThread::ensureAllShutdown();

				getrusage(RUSAGE_SELF,	&cpu_usage_buf);
				cpu_time	=	((double)	cpu_usage_buf.ru_utime.tv_sec)	+	((double)	cpu_usage_buf.ru_utime.tv_usec	/	1000000.0);
				
				/*	Get	memory	usage	in	MB	for	the	calling	process.	Note	that	ru_maxrss	returns	long	value	in	bytes	on	Mac	and	kilobytes	on	Linux.	*/
				#if	__APPLE__	
								sim_mem	=	(double)cpu_usage_buf.ru_maxrss	/	(1024	*	1024);
				#else	
								sim_mem	=	(double)cpu_usage_buf.ru_maxrss	/	1024;
				#endif

				/*	Calculate	simulation	elapsed	sim	time	and	actual	cpu	time	*/
				sim_elapsed_time	=	get_sim_time()	-	sim_start;
				user_cpu_time	=	cpu_time	-	user_cpu_start;

				/*	*/
				cpu_time	=	((double)	cpu_usage_buf.ru_stime.tv_sec)	+	((double)	cpu_usage_buf.ru_stime.tv_usec	/	1000000.0);
				kernal_cpu_time	=	cpu_time	-	kernal_cpu_start;

				if	((user_cpu_time	+	kernal_cpu_time)	<=	1e-6)	{
								sim_to_cpu	=	1e8;
				}	else	{
								sim_to_cpu	=	sim_elapsed_time	/	(user_cpu_time	+	kernal_cpu_time);
				}

				/*	Calculate	voluntary	and	involuntary	context	switch	usage	during	run	*/
				v_context_switch_run	=	cpu_usage_buf.ru_nvcsw	-	v_context_switch_init	;
				iv_context_switch_run	=	cpu_usage_buf.ru_nivcsw	-	iv_context_switch_init	;

				/*	Print	a	shutdown	message.	*/
				message_publish(MSG_NORMAL	,	"\n"
												"SIMULATION	TERMINATED	IN\n"
												"		PROCESS:	%d\n"
												"		ROUTINE:	%s\n"
												"		DIAGNOSTIC:	%s\n\n"
												"														SIMULATION	START	TIME:	%12.3f\n"
												"															SIMULATION	STOP	TIME:	%12.3f\n"
												"												SIMULATION	ELAPSED	TIME:	%12.3f\n"
												"																	USER	CPU	TIME	USED:	%12.3f\n"
												"															SYSTEM	CPU	TIME	USED:	%12.3f\n"
												"														SIMULATION	/	CPU	TIME:	%12.3f\n"
												"							INITIALIZATION	USER	CPU	TIME:	%12.3f\n"
												"					INITIALIZATION	SYSTEM	CPU	TIME:	%12.3f\n"
												"															SIMULATION	RAM	USAGE:	%12.3fMB\n"
												"						(External	program	RAM	usage	not	included!)\n"
												"		VOLUNTARY	CONTEXT	SWITCHES	(INIT):	%12ld\n"
												"INVOLUNTARY	CONTEXT	SWITCHES	(INIT):	%12ld\n"
												"			VOLUNTARY	CONTEXT	SWITCHES	(RUN):	%12ld\n"
												"	INVOLUNTARY	CONTEXT	SWITCHES	(RUN):	%12ld\n\n",
												process_id,	except_file.c_str(),	except_message.c_str(),
												sim_start,	get_sim_time(),	sim_elapsed_time,	
												user_cpu_time,	kernal_cpu_time,	sim_to_cpu,	
												user_cpu_init,	kernal_cpu_init,	sim_mem,
												v_context_switch_init,	iv_context_switch_init,
												v_context_switch_run,	iv_context_switch_run)	;

				/*	Kill	all	threads.	*/
				for	(ii	=	1;	ii	<	threads.size()	;	ii++)	{
								if	(	threads[ii]->running	)	{
												pthread_cancel(threads[ii]->get_pthread_id())	;
								}
#if	!	(	__APPLE__	||	__CYGWIN__	||	__Lynx__	)
								//	Even	Linux	is	hanging	on	this	call	right	now...	hmmm.
								//pthread_join(threads[ii]->thread_id,	(void	**)	NULL);
#endif
				}

				/*	Return	the	exception_return	value.		This	defaults	to	0	if	no	exceptions	were	thrown.	*/
				return(except_return)	;

}
