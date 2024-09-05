
#include	<iostream>
#include	<sstream>

#include	<fcntl.h>
#include	<iomanip>
#include	<errno.h>
#include	<unistd.h>

#include	"trick/DebugPause.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/exec_proto.h"
#include	"trick/exec_proto.hh"


Trick::DebugPause	*	the_debug_pause	=	NULL	;

Trick::DebugPause::DebugPause()	{
				debug_pause_flag	=	false	;
				the_debug_pause	=	this	;
}

int	Trick::DebugPause::debug_pause(Trick::JobData	*	curr_job)	{

				//	The	target	job	was	copied	to	sup_class_data	in	Trick::ScheduledJobQueueInstrument::call()
				Trick::JobData	*	target_job	=	(Trick::JobData	*)curr_job->sup_class_data	;
				int	sem_ret	;

				if	(	target_job	!=	NULL	)	{

								//	Pause	for	all	job	classes	that	are	not	initialization	or	for	init	jobs	greater	than	phase	1.
								if	(	target_job->job_class_name.compare("initialization")	or	target_job->phase	>	1	)	{

												message_publish(MSG_NORMAL,	"%12.6f	pausing	before	%s\n"	,	exec_get_sim_time()	,
													target_job->name.c_str()	)	;

												do	{
																sem_ret	=	sem_wait(debug_sem);
												}	while	((sem_ret	<	0)	&&	(errno	==	EINTR));

												if	(sem_ret	!=	0)	{
																perror("Call	to	sem_wait");
																fflush(stderr);
												}
								}
				}


				return(0)	;

}

int	Trick::DebugPause::debug_signal()	{

				sem_post(debug_sem);
				return(0)	;

}

int	Trick::DebugPause::debug_pause_on()	{

				std::stringstream	sem_name_stream	;

				if	(	debug_pause_flag	==	true	)	{
								return(0)	;
				}

				debug_pause_flag	=	true	;

				sem_name_stream	<<	"itimersepmaphore_"	<<	getpid()	;
				sem_name	=	sem_name_stream.str()	;

				debug_sem	=	sem_open(sem_name.c_str(),	O_CREAT,	S_IRWXU	,	0);

				exec_instrument_before("trick_instruments.debug_pause.debug_pause")	;

				//TODO:	turn	off	real-time	clock	if	on.

				return(0)	;

}

int	Trick::DebugPause::debug_pause_off()	{

				if	(	debug_pause_flag	==	false	)	{
								return(0)	;
				}
				debug_pause_flag	=	false	;

				exec_instrument_remove("trick_instruments.debug_pause.debug_pause")	;

				debug_signal()	;

				sem_unlink(sem_name.c_str())	;

				//TODO:	turn	back	on	real-time	clock	if	on	before	debug_pause	started.
				return(0);

}
