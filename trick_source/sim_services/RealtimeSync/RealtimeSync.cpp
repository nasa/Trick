/*
PURPOSE:
				(	RealtimeSync	)
PROGRAMMERS:
					((Alex	Lin)	(NASA)	(April	2009)	(--)	(c++	port)))
*/

#include	<stdio.h>
#include	<iostream>
#include	<sstream>
#include	<iomanip>
#include	<cmath>
#include	"trick/RealtimeSync.hh"
#include	"trick/exec_proto.h"
#include	"trick/sim_mode.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/TrickConstant.hh"

Trick::RealtimeSync	*	the_rts	=	NULL	;

/**
@details
-#	Start	disabled	(non-real-time).		Requirement	[@ref	non-real-time]
*/
Trick::RealtimeSync::RealtimeSync(	Trick::Clock	*	in_clock	,	Trick::Timer	*	in_timer	)	{

				/*	Start	disabled	(non-real-time).	*/
				enable_flag	=	false	;
				disable_flag	=	false	;

				active	=	false	;

				rt_max_overrun_cnt	=	100000000;
				rt_max_overrun_time	=	1.0e37;
				rt_overrun_freeze	=	false	;

				freeze_shutdown	=	false	;

				default_clock	=	in_clock	;

				change_clock(in_clock)	;
				change_timer(in_timer)	;

				align_sim_to_wall_clock	=	false	;
				align_tic_mult	=	1.0	;

				sim_start_time	=	0	;
				sim_end_init_time	=	0	;
				sim_end_time	=	0	;

				actual_run_ratio	=	0.0;

				the_rts	=	this	;

}

/**
@details
-#	Sets	real-time	enable	flag	to	true.	Requirement	[@ref	enable]
*/
int	Trick::RealtimeSync::enable()	{
				enable_flag	=	true	;
				return(0)	;
}

/**
@details
-#	Sets	real-time	disabled	flag	to	true.	Requirement	[@ref	disable]
*/
int	Trick::RealtimeSync::disable()	{
				disable_flag	=	true	;
				return(0)	;
}

/**
@details
-#	Sets	the	real_time	clock	to	the	incoming	class	[@ref	disable]
*/
int	Trick::RealtimeSync::change_clock(Trick::Clock	*	in_clock)	{
				int	ret	;
				ret	=	in_clock->clock_init()	;
				if	(	ret	==	0	)	{
								rt_clock	=	in_clock	;
				}
				return	ret	;
}

/**
@details
-#	Sets	the	real_time	clock	to	the	incoming	class	[@ref	disable]
*/
const	char	*	Trick::RealtimeSync::clock_get_name()	{
				return	rt_clock->get_name()	;
}

/**
@details
-#	Sets	the	sleep	timer	to	the	incoming	class	[@ref	disable]
*/
int	Trick::RealtimeSync::change_timer(Trick::Timer	*	in_sleep_timer)	{
				sleep_timer	=	in_sleep_timer	;
				return	0	;
}

/**
@details
-#	Calls	Trick::Clock::set_rt_clock_ratio
-#	Set	the	sleep	timer	to	inactive	for	this	frame	as	the	ratio	changes.
*/
int	Trick::RealtimeSync::set_rt_clock_ratio(double	in_clock_ratio)	{
				rt_clock->set_rt_clock_ratio(in_clock_ratio)	;
				sleep_timer->set_active(false)	;
				return	0	;
}

void	Trick::RealtimeSync::get_sim_start_time()	{
				sim_start_time	=	default_clock->wall_clock_time()	;
}

void	Trick::RealtimeSync::get_sim_end_init_time()	{
				sim_end_init_time	=	default_clock->wall_clock_time()	;
}

void	Trick::RealtimeSync::get_sim_end_time()	{
				sim_end_time	=	default_clock->wall_clock_time()	;
}

/**
@details
-#	If	real-time	synchronization	has	been	enabled:
			-#	Set	the	active	flag	to	true.
-#	If	real-time	synchronization	has	been	disabled	set	the	active	flag	to	false.
*/
int	Trick::RealtimeSync::initialize()	{

				if	(	enable_flag	)	{
								active	=	true	;
								enable_flag	=	false	;
				}

				if	(	disable_flag	)	{
								active	=	false	;
								disable_flag	=	false	;
				}

				tics_per_sec	=		exec_get_time_tic_value();

				/*	Start	the	sleep	timer	hardware	if	realtime	is	active	*/
				start_sleep_timer();

				if	(	align_sim_to_wall_clock	)	{
								rt_clock->clock_reset(0)	;
								rt_clock->sync_to_wall_clock(	align_tic_mult	,	tics_per_sec	)	;
								message_publish(MSG_INFO,	"Syncing	sim	to	%f	second	wall	clock	interval\n",	align_tic_mult	)	;
								rt_clock->clock_spin(0)	;
								if	(	exec_get_mode()	==	Freeze	)	{
												rt_clock->clock_reset(exec_get_freeze_time_tics())	;
								}	else	{
												rt_clock->clock_reset(exec_get_time_tics())	;
								}
				}

				return(0)	;
}

/**
@details
-#	If	real-time	synchronization	is	active
			-#	Call	the	real-time	clock	initialization	routine	[@ref	clock_init]
			-#	Call	the	sleep	timer	initialization
			-#	Set	the	sleep	timer	frame	period	to	software	frame	period
			-#	Calculate	the	maximum	overrun	time	in	simulation	tics.
*/
int	Trick::RealtimeSync::start_sleep_timer()	{

				if	(	active	&&	(exec_get_time_tics()	>=	0.0))	{
								/*	Call	sleep	timer	init	to	start	sleep	timer	hardware	*/
								sleep_timer->init()	;

								if	(	rt_max_overrun_time	>	1e36	)	{
												rt_max_overrun_time_tics	=	TRICK_MAX_LONG_LONG	;
								}	else	{
												rt_max_overrun_time_tics	=	(long	long)(rt_max_overrun_time	*	tics_per_sec)	;
								}

				}

				return(0)	;
}

/**
@details
-#	Get	the	sim_mode
-#	Reset	the	real-time	clock	reference
-#	If	sim_mode	is	Run
			-#	Call	start_realtime	to	start	the	real	time	clock
-#	Else	if	sim_mode	is	Freeze
			-#	Call	freeze	init	to	set	the	sleep	timer	to	freeze	mode.
*/
int	Trick::RealtimeSync::restart(long	long	ref_time)	{

				SIM_MODE	sim_mode	=	exec_get_mode()	;

				rt_clock->clock_reset(ref_time)	;
				if	(	sim_mode	==	Run	)	{
								start_realtime(exec_get_software_frame()	,	ref_time)	;
				}	else	if	(	sim_mode	==	Freeze	)	{
								freeze_init(exec_get_freeze_frame())	;
				}

				return	0	;
}

/**
@details
-#	If	real-time	synchronization	is	active
			-#	If	the	sim	time	is	0	or	higher	(do	not	run	in	real	time	for	negative	sim	time)
						-#	Reset	the	real-time	clock	to	the	incoming	reference	time
						-#	Save	the	current	real-time	as	the	start	of	the	frame	reference
						-#	Start	the	sleep	timer
			-#	Else	reset	active	to	false	and	enable_flag	to	true
*/
int	Trick::RealtimeSync::start_realtime(double	in_frame_time	,	long	long	ref_time)	{

				if	(	active	)	{

								/*	Only	run	in	real	time	when	sim	time	reaches	0.0	*/
								if	(ref_time	>=	0)	{

												/*	Reset	the	clock	reference	time	to	the	desired	reference	time	*/
												rt_clock->clock_reset(ref_time)	;

												/*	Start	the	sleep	timer	hardware	*/
												start_sleep_timer();

												/*	Start	the	sleep	timer	*/
												sleep_timer->start(in_frame_time	/	rt_clock->get_rt_clock_ratio())	;

								}	else	{

												/*	Reset	active	and	enable_flag	so	rt_monitor	will	try	and	start
															real	time	at	the	end	of	next	software	frame																			*/
												active	=	false;
												enable_flag	=	true;
								}

				}

				/*	Set	top	of	frame	time	for	1st	frame	(used	in	frame	logging).	*/
				last_clock_time	=	rt_clock->clock_time()	;

				return(0)	;
}

template	<size_t	N>
class	Run_Ratio	{
		public:
				Run_Ratio()	:	num_samples(0)	{}
				Run_Ratio&	operator()(long	long	sample,	double	in_rt_ratio)
				{
								samples[num_samples++	%	N]	=	sample;
								rt_ratio	=	in_rt_ratio;
								return	*this;
				}

				operator	double()	const	{
								if	(	num_samples	<=	1	)	{
												return	0.0	;
								}	else	if	(	num_samples	<	N	)	{
												return	round(exec_get_software_frame()	*	(num_samples-1)	*	exec_get_time_tic_value()	*	rt_ratio	/	(double)(samples[num_samples	-	1]	-	samples[0])*100.0)/100.0	;
								}	else	{
												return	round(exec_get_software_frame()	*	(N-1)	*	exec_get_time_tic_value()	*	rt_ratio	/	(double)(samples[(num_samples	-	1)	%	N]	-	samples[num_samples	%	N])*100.0)/100.0	;
								}
				}

		private:
				long	long	samples[N];
				size_t	num_samples;
				double	rt_ratio;
};

/**
@details
-#	If	real-time	is	not	active:
			-#	If	real-time	synchronization	has	been	enabled:
						-#	Set	the	active	flag	to	true.
						-#	Start	real-time	setting	the	real-time	clock	to	the	current	simulation	time.
			-#	exit	end	of	frame	routine
-#	If	real-time	is	active:
			-#	If	real-time	synchronization	has	been	disabled:
						-#	Set	the	active	flag	to	false.
-#	Get	the	current	real-time.
-#	Calculate	the	real-time	taken	for	the	last	frame	of	execution.
-#	if	the	frame	has	overrun
			-#	Increment	the	number	of	consecutive	overruns
			-#	If	the	maximum	number	of	consecutive	overrun	frames	has
						been	reached	or	the	maximum	single	overrun	time	has	been	exceeded
						-#	If	the	freeze/terminate	action	was	set
									-#	set	the	freeze_terminate	flag
									-#	freeze	the	simulation
						-#	Else	terminate	the	simulation
			-#	Stop	the	sleep	timer.
-#	Else	the	frame	has	underrun
			-#	Reset	the	number	of	consecutive	overruns	to	0.
			-#	Pause	for	the	sleep	timer	to	expire
			-#	Spin	for	the	real-time	clock	to	match	the	simulation	time
			-#	Reset	the	sleep	timer	for	the	next	frame
-#	Save	the	current	real-time	as	the	start	of	the	frame	reference
*/
int	Trick::RealtimeSync::rt_monitor(long	long	sim_time_tics)	{

				long	long	curr_clock_time	;
				char	buf[512];
				static	Run_Ratio<100>	run_ratio	;

				/*	calculate	the	current	underrun/overrun	*/
				curr_clock_time	=	rt_clock->clock_time()	;
				frame_sched_time	=	curr_clock_time	-	last_clock_time	;
				frame_time	=	frame_sched_time	*	(1.0/tics_per_sec);

				/*	Set	the	next	frame	overrun/underrun	reference	time	to	the	current	time	*/
				last_clock_time	=	curr_clock_time	;

				/*	determine	if	the	state	of	real-time	has	changed	this	frame	*/
				if	(	!	active	)	{
								if	(	enable_flag	)	{
												active	=	true	;
												enable_flag	=	false	;
												start_realtime(exec_get_software_frame()	,	sim_time_tics)	;
								}
								if	(	disable_flag	)	{
												disable_flag	=	false	;
								}
								/*	calculate	run	ratio	in	non-realtime	mode	*/
								actual_run_ratio	=	run_ratio(curr_clock_time,	1.0);
								return(0)	;
				}
				if	(	enable_flag	)	{
								enable_flag	=	false	;
				}
				if	(	disable_flag	)	{
								active	=	false	;
								disable_flag	=	false	;
				}

				frame_overrun_time	=	curr_clock_time	-	sim_time_tics	;

				frame_overrun	=	frame_overrun_time	*	(1.0/tics_per_sec);

				/*	If	the	wall	clock	time	is	greater	than	the	sim	time	an	overrun	occurred.	*/
				if	(curr_clock_time	>	sim_time_tics)	{

								/*	Update	the	overrun	counter	and	current	overrun	time	*/
								frame_overrun_cnt++;
								total_overrun++;

								/*	If	the	number	overruns	surpass	the	maximum	allowed	freeze	or	shutdown.	*/
								if	(frame_overrun_cnt	>=	rt_max_overrun_cnt	||	frame_overrun_time	>=	rt_max_overrun_time_tics)	{

												/*	If	the	rt_overrun_freeze	flag	is	set,	enter	freeze	mode	else	terminate	the	simulation.	*/
												if	(rt_overrun_freeze	==	true)	{
																freeze_shutdown	=	true	;
																message_publish(MSG_ERROR,	"\nMaximum	overrun	condition	exceeded:\n"
																				"consecutive	overruns/allowed	overruns:	%d/%d\n"
																				"total	overrun	time/allowed	time:	%f/%g\n"
																				"Entering	Freeze-Shutdown	Mode\n"	,
																				frame_overrun_cnt,	rt_max_overrun_cnt,
																				(double)(frame_overrun_time/tics_per_sec),	rt_max_overrun_time);
																exec_freeze()	;
												}	else	{
																snprintf(buf,	sizeof(buf),	"\nMaximum	overrun	condition	exceeded:\n"
																				"consecutive	overruns/allowed	overruns:	%d/%d\n"
																				"total	overrun	time/allowed	time:	%f/%g\n",
																				frame_overrun_cnt,	rt_max_overrun_cnt,
																				(double)(frame_overrun_time/tics_per_sec),	rt_max_overrun_time);
																exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	buf);
												}
								}

								/*	stop	the	sleep	timer	in	an	overrun	condition	*/
								sleep_timer->stop()	;

								/*	Call	clock_spin	to	allow	interrupt	driven	clocks	to	service	their	interrupts	*/
								curr_clock_time	=	rt_clock->clock_spin(sim_time_tics)	;

				}	else	{

								/*	Else	an	underrun	condition	occurred.	*/

								/*	Reset	consecutive	overrun	counter	frame_overrun_cnt	*/
								frame_overrun_cnt	=	0;

								/*	pause	for	the	timer	to	signal	the	end	of	frame	*/
								sleep_timer->pause()	;

								/*	Spin	to	make	sure	that	we	are	at	the	top	of	the	frame	*/
								curr_clock_time	=	rt_clock->clock_spin(sim_time_tics)	;

								/*	If	the	timer	requires	to	be	reset	at	the	end	of	each	frame,	reset	it	here.	*/
								sleep_timer->reset(exec_get_software_frame()	/	rt_clock->get_rt_clock_ratio())	;

								/*	Calculate	the	run	ratio	after	sleeping	*/
								actual_run_ratio	=	run_ratio(curr_clock_time,	rt_clock->get_rt_clock_ratio());
				}

				return(0)	;
}

/**
@details
-#	If	real-time	synchronization	is	active
			-#	Set	the	sleep	timer	frame	period	to	freeze	frame	period
			-#	Start	real-time	setting	the	real-time	clock	at	0.
*/
int	Trick::RealtimeSync::freeze_init(double	freeze_frame_sec)	{

				if	(	active	)	{
								sleep_timer->start(	freeze_frame_sec	/	rt_clock->get_rt_clock_ratio())	;
				}
				freeze_frame	=	(long	long)(freeze_frame_sec	*	tics_per_sec)	;
				freeze_time_tics	=	exec_get_time_tics()	;
				return	0		;
}

/**
@details
-#	If	real-time	is	not	active:
			-#	If	real-time	synchronization	has	been	enabled:
						-#	Set	the	active	flag	to	true.
						-#	Set	the	sleep	timer	frame	period	to	freeze	frame	period
						-#	Start	real-time	setting	the	real-time	clock	at	currnent	freeze	time.
			-#	exit	end	of	frame	routine
#	If	real-time	synchronization	has	been	disabled:
		-#	Set	the	active	flag	to	false.
-#	Pause	for	the	sleep	timer	to	expire
-#	Spin	for	the	real-time	clock	to	match	the	simulation	time
-#	Advanced	freeze	time.
*/
int	Trick::RealtimeSync::freeze_pause(double	freeze_frame_sec)	{

				/*	Determine	if	the	state	of	real-time	has	changed	this	frame	*/
				if	(	!	active	)	{
								if	(	enable_flag	)	{
												active	=	true	;
												enable_flag	=	false	;
												start_realtime(freeze_frame_sec	,	freeze_time_tics)	;
								}
								if	(	disable_flag	)	{
												disable_flag	=	false	;
								}
								return(0)	;
				}

				if	(	enable_flag	)	{
								enable_flag	=	false	;
				}
				if	(	disable_flag	)	{
								active	=	false	;
								disable_flag	=	false	;
				}

				/*	If	a	sleep	timer	has	been	defined	pause	for	the	timer	to	signal	the	end	of	frame	*/
				sleep_timer->pause()	;

				/*	Spin	to	make	sure	that	we	are	at	the	top	of	the	frame	*/
				rt_clock->clock_spin(freeze_time_tics	+	freeze_frame)	;

				/*	If	the	timer	requires	to	be	reset	at	the	end	of	each	frame,	reset	it	here.	*/
				sleep_timer->reset(freeze_frame_sec	/	rt_clock->get_rt_clock_ratio())	;

				freeze_time_tics	+=	freeze_frame	;

				return(0)	;

}

/**
@details
-#	If	real-time	is	active:
			-#	If	the	freeze_terminate	flag	is	set,	terminate	the	simulation
-#	Set	the	sleep	timer	frame	period	to	software	frame	period
-#	Start	real-time	setting	the	real-time	clock	to	the	current	simulation	time.
*/
int	Trick::RealtimeSync::unfreeze(long	long	sim_time_tics,	double	software_frame_sec)	{

				if	(	active	)	{

								/*	If	the	overrun	freeze_shutdown	condition	was	met	terminate	the	simulation	*/
								if	(freeze_shutdown)	{
												exec_terminate_with_return(-1	,	__FILE__	,	__LINE__	,	"Freeze-Shutdown	condition	reached.");
								}

								/*	Adjust	the	real-time	clock	reference	by	the	amount	of	time	we	were	frozen	*/
								rt_clock->adjust_ref_time(freeze_time_tics	-	sim_time_tics)	;

								/*	Start	the	sleep	timer	with	the	software	frame	expiration	*/
								sleep_timer->start(software_frame_sec	/	rt_clock->get_rt_clock_ratio())	;

				}

				/*	Set	top	of	frame	time	for	1st	frame	(used	in	frame	logging).	*/
				last_clock_time	=	rt_clock->clock_time()	;

				return(0)	;
}

/**
@details
-#	If	real-time	is	active:
			-#	Stop	the	real-time	clock	hardware
			-#	Stop	the	sleep	timer	hardware
			-#	Print	the	overrun	count
*/
int	Trick::RealtimeSync::shutdown()	{

				if	(	active	)	{
								/*	Stop	the	clock	*/
								rt_clock->clock_stop()	;

								/*	If	a	sleep	timer	has	been	defined,	stop	the	timer	*/
								sleep_timer->shutdown()	;
				}

				std::stringstream	os	;
				double	actual_time	=	(sim_end_time	-	sim_start_time)	/	(double)default_clock->clock_tics_per_sec	;
				os	<<	"\n"	<<
					"					REALTIME	SHUTDOWN	STATS:\n"	;
				if	(	active	)	{
								os	<<	"					REALTIME	TOTAL	OVERRUNS:	"	<<	std::setw(12)	<<	total_overrun	<<	"\n"	;
				}
				if	(	sim_end_init_time	!=	0	)	{
								double	init_time	=	(sim_end_init_time	-	sim_start_time)	/	(double)default_clock->clock_tics_per_sec	;
								os	<<		"												ACTUAL	INIT	TIME:	"	;
								os	<<		std::fixed	<<	std::setw(12)	<<	std::setprecision(3)	<<	init_time	<<	"\n"	;
				}
				os	<<		"									ACTUAL	ELAPSED	TIME:	"	;
				os	<<	std::fixed	<<	std::setw(12)	<<	std::setprecision(3)	<<	actual_time	<<	"\n"	;
				message_publish(MSG_NORMAL,os.str().c_str())	;

				return(0)	;
}

bool	Trick::RealtimeSync::is_active()	{
				return	active;
}
