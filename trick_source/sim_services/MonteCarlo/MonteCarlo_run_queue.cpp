
#include	<iostream>
#include	<sys/resource.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/ExecutiveException.hh"
#include	"trick/exec_proto.hh"

/**
@details
-#	Call	the	monte_carlo	jobs
			-#	The	scheduler	will	immediately	return	if	the	job	returns	a
						non-zero	return	code.		The	return	code	of	the	job	will	be
						returned	from	Trick::Executive::init().
-#	If	no	execption	is	thrown	return	0
*/
int	Trick::MonteCarlo::run_queue(Trick::ScheduledJobQueue*	queue,	std::string	in_string)	{
				int	ret	=	0	;

				queue->reset_curr_index()	;
				while	(	(curr_job	=	queue->get_next_job())	!=	NULL	)	{
								ret	=	curr_job->call()	;
								if	(	ret	!=	0	)	{
												except_file	=	in_string	;
												throw	Trick::ExecutiveException(ret	,	curr_job->name.c_str()	,	0	,
																																								curr_job->job_class_name	+	"	job	did	not	return	0")	;
								}
				}

				/*	return	0	if	there	are	no	errors.	*/
				return	ret	;
}

