
#include	<iostream>
#include	"SegmentedExecutive/include/ExampleSegments.hh"

SegmentA::SegmentA(	std::string	in_segment_name	,
																std::string	enter_job_class_str	,
																std::string	exit_job_class_str	,
																Trick::IntegLoopScheduler	&	in_integ_sched	)	:
	Trick::Segment(in_segment_name	,	enter_job_class_str	,	exit_job_class_str)	,
	integ_sched(in_integ_sched)	{	}	;

/*	This	custom	enter	routine	changes	the	integrator	and	integrator	cycle	rate	*/
int	SegmentA::enter(	std::string	next_segment	)	{
				std::cout	<<	"[33mSwitching	integrators	in	segment	A![00m"	<<	std::endl	;
				integ_sched.getIntegrator(Runge_Kutta_2	,	4)	;
				integ_sched.set_integ_cycle(0.01)	;
				return	Trick::Segment::exit(next_segment)	;
}

/*	--------------------------------------------------------------------	*/

SegmentB::SegmentB(	std::string	in_segment_name	,
																std::string	enter_job_class_str	,
																std::string	exit_job_class_str	,
																Trick::IntegLoopScheduler	&	in_integ_sched	)	:
	Trick::Segment(in_segment_name	,	enter_job_class_str	,	exit_job_class_str)	,
	integ_sched(in_integ_sched)	{	}	;

/*	This	custom	enter	routine	changes	the	integrator	and	integrator	cycle	rate	*/
int	SegmentB::enter(	std::string	next_segment	)	{
				std::cout	<<	"[33mSwitching	integrators	in	segment	B![00m"	<<	std::endl	;
				integ_sched.getIntegrator(Runge_Kutta_4	,	4)	;
				integ_sched.set_integ_cycle(0.1)	;
				return	Trick::Segment::exit(next_segment)	;
}
