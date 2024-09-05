
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"

int	Trick::Executive::register_scheduler(	Trick::Scheduler	*	scheduler	)	{
				other_schedulers.push_back(scheduler)	;
				return(0)	;

}
