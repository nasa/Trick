
/*
PURPOSE:
				(	Clock	clock	)
PROGRAMMERS:
					((Alex	Lin)	(NASA)	(April	2009)	(--)	(c++	port)))
*/

#include	<stdlib.h>

#include	"trick/Clock.hh"
#include	"trick/exec_proto.h"
#include	"trick/release.h"

Trick::Clock	*	the_clock	=	NULL	;

/**
@details
-#	Sets	the	global	clock	pointer	to	the	current	instance
*/
Trick::Clock::Clock(unsigned	long	long	in_clock_tics_per_sec,	std::string	in_name)	:
	name(in_name)	,
	clock_tics_per_sec(in_clock_tics_per_sec)	,
	rt_clock_ratio(1.0)	,
	sim_tic_ratio(1.0)	,
	ref_time_tics(0)	{	}

int	Trick::Clock::set_global_clock()	{
				the_clock	=	this	;
				return	0	;
}

int	Trick::Clock::calc_sim_time_ratio(int	in_time_tic_value	)	{
				sim_tic_ratio	=	in_time_tic_value	/	(double)clock_tics_per_sec	;
				return	0	;
}

double	Trick::Clock::get_rt_clock_ratio()	{
				return	rt_clock_ratio	;
}

int	Trick::Clock::set_rt_clock_ratio(double	in_rt_clock_ratio)	{
				long	long	curr_time	=	clock_time()	;
				rt_clock_ratio	=	in_rt_clock_ratio	;
				clock_reset(curr_time)	;
				return	0	;
}

int	Trick::Clock::adjust_ref_time(long	long	in_tics)	{
				ref_time_tics	+=	(long	long)(in_tics	/	(	rt_clock_ratio	*	sim_tic_ratio	))	;
				return	0	;
}

int	Trick::Clock::sync_to_wall_clock(double	align_tic_mult,	int	tics_per_sec)	{

				long	long	curr_time	;
				long	long	sync_time	;

				curr_time	=	wall_clock_time()	;
				sync_time	=	curr_time	-	(	curr_time	%	int(align_tic_mult	*	tics_per_sec	/	(rt_clock_ratio	*	sim_tic_ratio)	)	)	+
					int(align_tic_mult	*	tics_per_sec	/	(rt_clock_ratio	*	sim_tic_ratio)	)	;
				set_reference(sync_time)	;
				return	0	;
}

/**
@details
-#	Get	the	hardware	specific	wall	clock	time
-#	Subtract	the	simulation	time	to	give	a	reference	time.
-#	Save	the	reference	time.
*/
long	long	Trick::Clock::clock_time()	{
				long	long	curr_time	;
				curr_time	=	wall_clock_time()	;
				return	((long	long)((curr_time	-	ref_time_tics)	*	rt_clock_ratio	*	sim_tic_ratio));
}

/**
@details
-#	Get	the	current	real	time.
-#	While	the	current	real	time	is	less	than	the	requested	time
			-#	Release	the	processor	momentarily
			-#	Get	the	current	real	time.
*/
long	long	Trick::Clock::clock_spin(long	long	req_time)	{
				long	long	curr_time	;
				curr_time	=	clock_time()	;
				/*	Perform	spin	loop	to	allow	current	time	to	catch	up	to	requested	time	*/
				while	(curr_time	<	req_time)	{
								if	(	exec_get_rt_nap()	==	1	)	{
												RELEASE();
								}
								curr_time	=	clock_time();
				}
				return	curr_time	;
}

/**
@details
-#	Call	the	hardware	specific	wall	clock	time
-#	Subtract	the	reference	time	to	give	the	elapsed	real	time.
-#	return	the	elapsed	real	time.
*/
long	long	Trick::Clock::clock_reset(long	long	ref)	{
				long	long	curr_time	;
				curr_time	=	wall_clock_time()	;
				ref_time_tics	=	curr_time	-	(long	long)(ref	/	(rt_clock_ratio	*	sim_tic_ratio));
				return	(long	long)(ref_time_tics	*	sim_tic_ratio	*	rt_clock_ratio	)	;
}

/**
@details
-#	Sets	the	clock's	reference	time	to	incoming	value
*/
int	Trick::Clock::set_reference(long	long	ref)	{
				ref_time_tics	=	ref	;
				return	0	;
}

/**
@details
-#	Return	the	name	of	the	clock
*/
const	char	*	Trick::Clock::get_name()	{
				return	name.c_str()	;
}
