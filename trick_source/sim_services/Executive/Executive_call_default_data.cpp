
#include	<iostream>
#include	<sys/resource.h>

#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/clock_proto.h"

/**
@details
-#	Call	the	default	data	jobs	to	set	the	default	state	of	the	simulation
			(pseudo-constructors	for	simulation	structures).
			Requirement	[@ref	r_exec_discrete_0].
			-#	The	scheduler	will	immediately	return	if	the	default_data	job	returns	a
						non-zero	return	code.		The	return	code	of	the	default_data	job	will	be
						returned	from	Trick::Executive::init().
-#	If	no	execption	is	thrown	return	0
*/
int	Trick::Executive::call_default_data()	{

				int	ret	=	0	;

				except_file	=	"in	default_data	queue"	;

				/*	Call	the	default	data	jobs.	*/
				default_data_queue.reset_curr_index()	;
				while	(	(curr_job	=	default_data_queue.get_next_job())	!=	NULL	)	{
								long	long	start	=	clock_wall_time();
								ret	=	curr_job->call()	;
								long	long	end	=	clock_wall_time();
								if(init_log_stream.is_open())	{
										init_log_stream	<<	"default_data,"	<<	curr_job->name	<<	','	<<	(double)(end-start)/clock_tics_per_sec()	<<	'\n';
								}
								if	(	ret	!=	0	)	{
												throw	Trick::ExecutiveException(ret	,	curr_job->name.c_str()	,	0	,	"default_data	job	did	not	return	0")	;
								}
				}

				/*	return	0	if	there	are	no	errors.	*/
				return	ret	;
}

