
#include	<iostream>
#include	<stdlib.h>
#include	<sys/resource.h>
#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/exec_proto.h"

/**
@details
-#	Set	the	mode	to	Initialization
			Requirement	[@ref	r_exec_mode_0].
-#	Start	the	cpu	usage	meter
-#	Call	the	default_data	jobs
-#	Call	the	initialization	jobs.
-#	Record	the	cpu	usage	during	initialization
-#	The	scheduler	initializes	simulation	timers.
			Requirement	[@ref	r_exec_time_1].
-#	If	an	exception	is	caught,	print	as	much	error	information	available	based
			on	execption	type	caught	and	exit.
-#	If	no	execption	is	caught	return	0
*/
int	Trick::Executive::init()	{

				double	cpu_time	;


				try	{

								mode	=	Initialization	;

								/*	Start	the	cpu	usage	meter	*/
								struct	rusage	cpu_usage_buf	;
								getrusage(RUSAGE_SELF,	&cpu_usage_buf);
								user_cpu_start	=			((double)	cpu_usage_buf.ru_utime.tv_sec)	+	((double)	cpu_usage_buf.ru_utime.tv_usec	/	1000000.0);
								
								/*	command	line	args	*/
								process_sim_args();

								call_default_data()	;
								call_input_processor()	;

								//	If	we	are	starting	from	a	checkpoint,	restart_called	will	be	true.		Skip	init	routines	in	this	case.
								if	(	!	restart_called	)	{
												call_initialization()	;
								}

								init_log_stream.close();

								/*	Set	the	initial	values	for	the	scheduler	times.	*/
								next_frame_check_tics	=	software_frame_tics	+	time_tics	;
								job_call_time_tics	=	next_frame_check_tics	;
								sim_start	=	get_sim_time();

								/*	Record	the	cpu	usage	for	initialization	*/
								getrusage(RUSAGE_SELF,	&cpu_usage_buf);
								cpu_time	=	((double)	cpu_usage_buf.ru_utime.tv_sec)	+	((double)	cpu_usage_buf.ru_utime.tv_usec	/	1000000.0);
								user_cpu_init	=	cpu_time	-	user_cpu_start;

								cpu_time	=	((double)	cpu_usage_buf.ru_stime.tv_sec)	+	((double)	cpu_usage_buf.ru_stime.tv_usec	/	1000000.0);
								kernal_cpu_init	=	cpu_time	-	kernal_cpu_start;

								/*	Record	both	voluntary	and	involuntary	context	switches	usage	for	initialization	*/
								v_context_switch_init	=	cpu_usage_buf.ru_nvcsw;
								iv_context_switch_init	=	cpu_usage_buf.ru_nivcsw;
								
								initialization_complete	=	true	;

				/*	Print	as	much	error	information	avaiable	for	all	exception	and	exit.	*/
				}	catch	(Trick::ExecutiveException	&	ex	)	{
								/*	Set	the	exit	return	code,	file	name,	and	error	message.	Return	-1	so	we	go	to	shutdown	*/
								except_return	=	ex.ret_code	;
								except_file	=	ex.file	;
								except_message	=	ex.message	;
								return(-1)	;
				}

				/*	return	0	if	there	are	no	errors.	*/
				return(0)	;
}

