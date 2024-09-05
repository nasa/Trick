
#include	<iostream>
#include	<sstream>
#include	<stdio.h>

#include	"trick/Threads.hh"

Trick::Threads::Threads(int	in_id	,	bool	in_rt_nap)	:
	enabled(true)	,
	thread_id(in_id)	,
	curr_time_tics(0)	,
	amf_cycle(0.0)	,
	amf_cycle_tics(0)	,
	amf_next_tics(0)	,
	curr_job(NULL)	,
	rt_nap(in_rt_nap)	,
	process_type(PROCESS_TYPE_SCHEDULED)	,
	child_complete(false)	,
	running(false)	,
	shutdown_wait_async(false)	{
				std::stringstream	oss	;
				oss	<<	"Child_"	<<	in_id	;
				name	=	oss.str()	;
}

void	Trick::Threads::set_pthread_id(pthread_t	in_pthread_id)	{
				pthread_id	=	in_pthread_id	;
}

void	Trick::Threads::set_enabled(	bool	in_enabled	)	{
				enabled	=	in_enabled	;
}

void	Trick::Threads::clear_scheduled_queues()	{
				job_queue.clear()	;
				top_of_frame_queue.clear()	;
				end_of_frame_queue.clear()	;
}

void	Trick::Threads::dump(	std::ostream	&	oss	)	{
				oss	<<	"Trick::Threads	("	<<	name	<<	")"	<<	std::endl	;
				oss	<<	"				process_type	=	"	;
				//	Regardless	of	type,	amf_cycle	is	used	to	store	the	cycle	time	set.
				//	If	thread_id	is	0	which	means	main	thread,	amf_cycle	can't	be	set	so	the	init	value	0	will	be	dumped.
				switch	(	process_type	)	{
								case	PROCESS_TYPE_SCHEDULED:	oss	<<	"scheduled	=	"	<<	amf_cycle	<<	std::endl	;	break	;
								case	PROCESS_TYPE_ASYNC_CHILD:	oss	<<	"asynchronous	=	"	<<	amf_cycle	<<	std::endl	;	break	;
								case	PROCESS_TYPE_AMF_CHILD:	oss	<<	"asynchronous	must	finish	with	amf_cycle	=	"	<<	amf_cycle	<<	std::endl	;	break	;
				}
				trigger_container.getThreadTrigger()->dump(oss)	;
				oss	<<	"				number	of	scheduled	jobs	=	"	<<	job_queue.size()	<<	std::endl	;
				Trick::ThreadBase::dump(oss)	;
}
