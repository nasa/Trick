
#include	"trick/Scheduler.hh"

Trick::Scheduler::~Scheduler()	{}

//	Default	implementation	of	add_sim_object	adds	sim_object	jobs	to	the	scheduler	queues
//	if	this	scheduler	handles	that	job	type
int	Trick::Scheduler::add_sim_object(	Trick::SimObject	*	in_object	)	{

				unsigned	int	jj	;
				std::map<std::string,	int>::iterator	class_id_it	;
				JobData	*	job	;
				std::map<int,	Trick::ScheduledJobQueue	*>::iterator	queue_it	;
				Trick::ScheduledJobQueue	*	curr_queue	;

				for	(	jj	=	0	;	jj	<	in_object->jobs.size()	;	jj++	)	{
								job	=	in_object->jobs[jj]	;
								if	(	(class_id_it	=	class_map.find(job->job_class_name))	!=	class_map.end()	)	{
												job->job_class	=	class_id_it->second	;
												if	(	(queue_it	=	class_to_queue.find(job->job_class))	!=	class_to_queue.end()	)	{
																	curr_queue	=	queue_it->second	;
																	curr_queue->push(	job	)	;
												}
								}
				}

				return	0	;
}

//	Default	implementation	of	delete_sim_object	removes	all	jobs	associated	with	the	incoming
//	sim_object	from	the	queues	in	this	scheduler.
int	Trick::Scheduler::delete_sim_object(	Trick::SimObject	*	in_object	)	{

				unsigned	int	jj	;
				std::map<std::string,	int>::iterator	class_id_it	;
				JobData	*	job	;
				std::map<int,	Trick::ScheduledJobQueue	*>::iterator	queue_it	;
				Trick::ScheduledJobQueue	*	curr_queue	;

				for	(	jj	=	0	;	jj	<	in_object->jobs.size()	;	jj++	)	{
								job	=	in_object->jobs[jj]	;
								if	(	(class_id_it	=	class_map.find(job->job_class_name))	!=	class_map.end()	)	{
												if	(	(queue_it	=	class_to_queue.find(class_id_it->second))	!=	class_to_queue.end()	)	{
																	curr_queue	=	queue_it->second	;
																	curr_queue->remove(	job	)	;
												}
								}
				}

				return	0	;
}

//	These	2	routines	should	no	longer	be	needed.		(Alex	9/12/12)
int	Trick::Scheduler::write_sched_queue(	FILE	*	fp	,	Trick::ScheduledJobQueue	*	curr_queue	)	{
				return	curr_queue->write_sched_queue(fp)	;
}

int	Trick::Scheduler::write_non_sched_queue(	FILE	*	fp	,	Trick::ScheduledJobQueue	*	curr_queue	)	{
				return	curr_queue->write_non_sched_queue(fp)	;
}

