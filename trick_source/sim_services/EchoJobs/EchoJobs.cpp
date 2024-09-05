
#include	<iostream>
#include	<iomanip>

#include	"trick/EchoJobs.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

Trick::EchoJobs	*	the_ej	=	NULL	;

Trick::EchoJobs::EchoJobs()	{
				echo_job_flag	=	false	;

				the_ej	=	this	;
}

int	Trick::EchoJobs::echo_job(Trick::JobData	*	curr_job	)	{

				//	The	target	job	was	copied	to	sup_class_data	in	Trick::ScheduledJobQueueInstrument::call()
				Trick::JobData	*	target_job	=	(Trick::JobData	*)curr_job->sup_class_data	;

				if	(	target_job	!=	NULL	)	{
								if	(	target_job->phase	!=	60000	)	{
												message_publish(	MSG_NORMAL	,	"(P%d	%s)	%s\n",	target_job->phase,
																																																							target_job->job_class_name.c_str(),
																																																							target_job->name.c_str()	)	;
								}	else	{
												message_publish(	MSG_NORMAL	,	"(%s)	%s\n",	target_job->job_class_name.c_str(),
																																																							target_job->name.c_str()	)	;
								}
				}	else	{
								message_publish(	MSG_ERROR	,	"echo	job	called	but	target	job	pointer	is	NULL\n"	)	;
				}
				return(0)	;

}

int	Trick::EchoJobs::echojobs_on()	{

				if	(	echo_job_flag	==	true	)	{
								return(0)	;
				}
				echo_job_flag	=	true	;
				exec_instrument_before("trick_instruments.echo_jobs.echo_job")	;
				return(0)	;

}

int	Trick::EchoJobs::echojobs_off()	{

				if	(	echo_job_flag	==	false	)	{
								return(0)	;
				}
				echo_job_flag	=	false	;
				exec_instrument_remove("trick_instruments.echo_jobs.echo_job")	;
				return(0)	;

}

