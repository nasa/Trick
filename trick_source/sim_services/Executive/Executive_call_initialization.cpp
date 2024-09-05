
#include	<iostream>
#include	<sys/resource.h>
#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/clock_proto.h"

/**
@details
-#	Call	the	initialization	jobs.
			-#	The	scheduler	will	immediately	return	if	the	initialization	job	returns	a
						non-zero	return	code.		The	return	code	of	the	initialization	job	will	be
						returned	from	Trick::Executive::init().
-#	If	no	execption	is	thrown	return	0
*/
int	Trick::Executive::call_initialization()	{

				int	ret	=	0	;

				except_file	=	"in	intialization	queue"	;

				/*	Call	the	initialization	jobs.	*/
				initialization_queue.reset_curr_index()	;
				while	(	(curr_job	=	initialization_queue.get_next_job())	!=	NULL	)	{
								long	long	start	=	clock_wall_time();
								ret	=	curr_job->call()	;
								long	long	end	=	clock_wall_time();
								if(init_log_stream.is_open())	{
												init_log_stream	<<	"init,"	<<	curr_job->name	<<	','	<<	(double)(end-start)/clock_tics_per_sec()	<<	'\n';
								}
								if	(	ret	!=	0	)	{
												throw	Trick::ExecutiveException(ret	,	curr_job->name.c_str()	,	0	,	"initialization	job	did	not	return	0")	;
								}
				}

				/*	return	0	if	there	are	no	errors.	*/
				return	ret	;
}
