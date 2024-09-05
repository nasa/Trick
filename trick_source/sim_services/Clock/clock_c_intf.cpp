
#include	"trick/Clock.hh"
#include	"trick/clock_proto.h"

extern	Trick::Clock	*	the_clock	;

extern	"C"	long	long	clock_time(void)	{
				return	the_clock->clock_time()	;
}

extern	"C"	long	long	clock_wall_time(void)	{
				return	the_clock->wall_clock_time()	;
}

extern	"C"	long	long	clock_reset(long	long	ref)	{
				return	the_clock->clock_reset(ref)	;
}

extern	"C"	int	clock_set_reference(long	long	ref)	{
				return	the_clock->set_reference(ref)	;
}

extern	"C"	int	clock_spin(long	long	ref)	{
				return	the_clock->clock_spin(ref)	;
}

extern	"C"	double	clock_get_rt_clock_ratio(void)	{
				return	the_clock->get_rt_clock_ratio()	;
}

extern	"C"	int	clock_set_rt_clock_ratio(double	in_clock_ratio)	{
				return	the_clock->set_rt_clock_ratio(in_clock_ratio)	;
}

extern	"C"	unsigned	long	long	clock_tics_per_sec(void)	{
	return	the_clock->clock_tics_per_sec;
}
