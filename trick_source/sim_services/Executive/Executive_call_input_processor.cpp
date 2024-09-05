
#include	<iostream>
#include	<sys/resource.h>

#include	"trick/Executive.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/clock_proto.h"

/**
@details
-#	Call	the	input_processor	jobs
			-#	The	scheduler	will	immediately	return	if	the	default_data	job	returns	a
						non-zero	return	code.		The	return	code	of	the	default_data	job	will	be
						returned	from	Trick::Executive::init().
-#	If	no	execption	is	thrown	return	0
*/
int	Trick::Executive::call_input_processor()	{

				int	ret	=	0	;

				except_file	=	"in	input_processor	queue"	;

				/*	Call	the	input	processor	jobs.	*/
				/*	A	restart	can	be	called	by	a	call	to	the	input	processor.		If	that	happens	the	Executive	restart	job	will	clear	this
							queue	and	restart	it	at	the	beginning.		We	don't	want	to	call	the	input	processor	jobs	again	in	this	case.
							So	we	test	for	the	restart_called	flag	and	break	out	of	the	loop	if	we	restart_called	is	set.	*/
				input_processor_queue.reset_curr_index()	;
				while	(	!restart_called	and	(curr_job	=	input_processor_queue.get_next_job())	!=	NULL	)	{
								long	long	start	=	clock_wall_time();
								ret	=	curr_job->call()	;
								long	long	end	=	clock_wall_time();
								if(init_log_stream.is_open())	{
										init_log_stream	<<	"input_processor,"	<<	curr_job->name	<<	','	<<	(double)(end-start)/clock_tics_per_sec()	<<	'\n';
								}
								if	(	ret	!=	0	)	{
												throw	Trick::ExecutiveException(ret	,	curr_job->name.c_str()	,	0	,	"input_processor	job	did	not	return	0")	;
								}
				}

				/*	return	0	if	there	are	no	errors.	*/
				return	ret	;
}

