
#include	"SegmentedExecutive/include/Segment.hh"

Trick::Segment::Segment(	std::string	in_name	)	:	segment_name(in_name)	{}	;

Trick::Segment::Segment(	std::string	in_name	,
	std::string	enter_job_class_str	,
	std::string	exit_job_class_str	)	:
		segment_name(in_name)	{
				set_enter_job_class_name(enter_job_class_str)	;
				set_exit_job_class_name(exit_job_class_str)	;
}	;

std::string	Trick::Segment::get_segment_name()	{
				return	segment_name	;
}

void	Trick::Segment::set_segment_name(std::string	in_segment_name)	{
				segment_name	=	in_segment_name	;
}

void	Trick::Segment::set_enter_job_class_name(std::string	job_class_name)	{
				enter_job_class_name	=	job_class_name	;
				job_queues[enter_job_class_name]	=	&enter_jobs	;
}

void	Trick::Segment::set_exit_job_class_name(std::string	job_class_name)	{
				exit_job_class_name	=	job_class_name	;
				job_queues[exit_job_class_name]	=	&exit_jobs	;
}

//	Call	the	enter_jobs	when	entering	this	segment.
int	Trick::Segment::enter(	std::string	prev_segment	__attribute__((unused))	)	{
				return	enter_jobs.execute_all_jobs()	;
}

//	Call	the	exit_jobs	when	exiting	this	segment.
int	Trick::Segment::exit(	std::string	next_segment	__attribute__((unused))	)	{
				return	exit_jobs.execute_all_jobs()	;
}

std::map	<	std::string	,	Trick::ScheduledJobQueue	*	>	&	Trick::Segment::get_job_queues()	{
				return	job_queues	;
}

