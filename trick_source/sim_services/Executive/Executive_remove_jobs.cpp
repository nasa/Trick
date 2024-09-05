
#include	<iostream>
#include	<math.h>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/TrickConstant.hh"

/**
@details
*/
int	Trick::Executive::remove_jobs(	Trick::SimObject	*	in_sim_object	)	{

				/*	erase	jobs	from	the	all_jobs	map	whose	parent_object	is	this	sim_object	*/
				for	(	std::multimap	<std::string,	Trick::JobData	*>::iterator	mit	=	all_jobs.begin()	;	mit	!=	all_jobs.end()	;	)	{
								if	(	(*mit).second->parent_object	==	in_sim_object	)	{
												all_jobs.erase(mit++)	;
								}	else	{
												mit++	;
								}
				}

				/*	erase	all	jobs	from	the	all_jobs_vector	whose	parent_object	is	this	sim_object	*/
				for	(	std::vector	<Trick::JobData	*>::iterator	jit	=	all_jobs_vector.begin()	;	jit	!=	all_jobs_vector.end()	;	)	{
								if	(	(*jit)->parent_object	==	in_sim_object	)	{
												jit	=	all_jobs_vector.erase(jit)	;
								}	else	{
												jit++	;
								}
				}

				/*	erase	all	jobs	from	the	all_tagged_jobs	map	whose	parent_object	is	this	sim_object	*/
				for	(	std::multimap<	std::string,	Trick::JobData	*	>::iterator	mit	=	all_tagged_jobs.begin()	;	mit	!=	all_tagged_jobs.end()	;	)	{
								if	(	(*mit).second->parent_object	==	in_sim_object	)	{
												all_tagged_jobs.erase(mit++)	;
								}	else	{
											mit++	;
								}
				}

				/*	search	the	executive	queues	for	the	job	and	remove	it	if	found	*/
				for	(	std::vector	<Trick::JobData	*>::iterator	jit	=	in_sim_object->jobs.begin()	;	jit	!=	in_sim_object->jobs.end()	;	++jit	)	{
								std::map<std::string,	int>::iterator	class_id_it	;
								std::map<int,	Trick::ScheduledJobQueue	*>::iterator	queue_it	;
								Trick::ScheduledJobQueue	*	curr_queue	;
								if	(	(class_id_it	=	class_map.find((*jit)->job_class_name))	!=	class_map.end()	)	{
												if	(	(*jit)->thread	!=	0	)	{
																/*	Add	threaded	scheduled	jobs	to	the	thread	scheduled	queue	*/
																if	(	(*jit)->job_class	>=	scheduled_start_index	)	{
																				threads[(*jit)->thread]->job_queue.remove((*jit))	;
																				//	Add	all	scheduled	jobs	to	the	scheduled_queue	for	use	in	the	multi-threaded	loop
																				scheduled_queue.remove((*jit))	;
																/*	Threaded	top_of_frame/end_of_frame	jobs	go	to	thread	specific	queues.	*/
																}	else	if	(	!	(*jit)->job_class_name.compare("top_of_frame"))	{
																				threads[(*jit)->thread]->top_of_frame_queue.remove((*jit))	;
																}	else	if	(	!	(*jit)->job_class_name.compare("end_of_frame"))	{
																				threads[(*jit)->thread]->end_of_frame_queue.remove((*jit))	;
																/*	Other	jobs	classes	are	put	into	the	main	thread	*/
																}	else	if	(	(queue_it	=	class_to_queue.find((*jit)->job_class))	!=	class_to_queue.end()	)	{
																				/*	for	non-scheduled	jobs,	the	class_to_queue	map	holds	the	correct	queue	to	insert	the	job	*/
																				curr_queue	=	queue_it->second	;
																				curr_queue->remove(	(*jit)	)	;
																}
												}	else	{
																/*	if	the	job	is	a	"scheduled"	type	job,	insert	the	job	into	the	proper	thread	queue	*/
																if	(	(*jit)->job_class	>=	scheduled_start_index	)	{
																				threads[0]->job_queue.remove((*jit))	;
																				//	Add	all	scheduled	jobs	to	the	scheduled_queue	for	use	in	the	multi-threaded	loop
																				scheduled_queue.remove((*jit))	;
																}	else	if	(	(queue_it	=	class_to_queue.find((*jit)->job_class))	!=	class_to_queue.end()	)	{
																				/*	for	non-scheduled	jobs,	the	class_to_queue	map	holds	the	correct	queue	to	insert	the	job	*/
																				curr_queue	=	queue_it->second	;
																				curr_queue->remove(	(*jit)	)	;
																}
												}
								}
				}

				return	0	;

}
