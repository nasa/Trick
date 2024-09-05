
#include	<math.h>
#include	"trick/Threads.hh"

/*
@design
-#	Set	amf_cycle	to	incoming	cycle	time
-#	calculate	amf_cycle_tics	=	in_cycle	*	time_tic_value
-#	return	successful	0
*/
int	Trick::Threads::set_amf_cycle(double	in_cycle	,	long	long	time_tic_value)	{
				amf_cycle	=	in_cycle	;
				amf_cycle_tics	=	(long	long)(in_cycle	*	time_tic_value)	;
				return(0)	;
}


/*
@design
-#	recalculate	amf_cycle_tics	using	new	time_tic_value
-#	recalculate	amf_next_tics	=	(amf_next_tics	/	old_time_tic_value)	*	time_tic_value
-#	return	successful	0
*/
int	Trick::Threads::time_tic_changed(long	long	old_time_tic_value	,	long	long	time_tic_value)	{
				amf_cycle_tics	=	(long	long)(amf_cycle	*	time_tic_value)	;
				amf_next_tics	=	(long	long)round((amf_next_tics	/	old_time_tic_value)	*	time_tic_value)	;
				return(0)	;
}
