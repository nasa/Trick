
#include	<iostream>
#include	<sstream>

#include	"trick/TrickConstant.hh"
#include	"trick/Executive.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

int	Trick::Executive::get_freeze_job(std::string	sim_object_name)	{
				freeze_job	=	get_job(sim_object_name	+	".sched_freeze_to_exec_command")	;
				if	(	freeze_job	==	NULL	)	{
								exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	"Executive	could	not	find	freeze	job"	)	;
				}	else	{
								freeze_job->next_tics	=	TRICK_MAX_LONG_LONG	;
				}

				return	0	;
}

/**
@details
-#	Set	async_freeze_command	to	true		Requirement	[@ref	r_exec_mode_4]
*/
int	Trick::Executive::freeze()	{

				async_freeze_command	=	true	;

				return(0)	;

}

int	Trick::Executive::freeze(double	in_time)	{

				long	long	new_time	;
				new_time	=	(long	long)(in_time	*	time_tic_value)	;

				if	(new_time	>	time_tics	)	{
								freeze_times.push(new_time)	;
								if	(	new_time	<	freeze_job->next_tics	)	{
												freeze_job->next_tics	=	new_time	;
								}
								//std::cout	<<	"\033[33mSET	FREEZE	TIME	"	<<	in_time	<<	"	"	<<	new_time	<<	"\033[0m"	<<	std::endl	;
				}	else	{
								message_publish(MSG_ERROR,	"Freeze	time	specified	in	the	past.	specified	%f,	current_time	%f\n",
									in_time	,	get_sim_time())	;
				}

				return(0)	;

}

/**
@details
-#	Sets	exec_command	to	FreezeCmd	at	the	end	of	the	software	frame
*/
int	Trick::Executive::async_freeze_to_exec_command()	{

				if	(	async_freeze_command	)	{
								exec_command	=	FreezeCmd	;
								async_freeze_command	=	false	;
				}

				return(0)	;
}

int	Trick::Executive::sched_freeze_to_exec_command(bool	end_of_frame)	{

				if	(	end_of_frame	or	!freeze_on_frame_boundary	)	{
								//	remove	all	times	at	the	top	of	the	queue	that	match	the	current	time.
								while	(	!freeze_times.empty()	and	(freeze_times.top()	<=	time_tics)	)	{
												exec_command	=	FreezeCmd	;
												freeze_times.pop()	;
								}

								if	(	freeze_job	!=	NULL	)	{
												if	(	!freeze_times.empty()	)	{
																freeze_job->next_tics	=	freeze_times.top()	;
												}	else	{
																freeze_job->next_tics	=	TRICK_MAX_LONG_LONG	;
												}
								}
				}

				return(0)	;

}

