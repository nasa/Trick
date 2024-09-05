
#include	<iostream>
#include	<math.h>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/TrickConstant.hh"

/**
@details
-#	For	each	job	in	the	sim_object
				-#	Assign	the	job's	sim_object	id	to	the	parent	object's	id.
				-#	Assign	the	job's	parent	object	pointer	to	the	parent	object.
				-#	Assign	the	job's	name	if	we	are	not	restarting	this	sim.
				-#	Assign	the	job's	frame	id	as	(<sim_object>.<job_name>)
				-#	If	the	job	is	a	system	job	or	automatic	job,	Mark	the	job	as	a	system	class	job.		System	class
							jobs	next	call	times	are	not	recalculated	by	the	scheduler.
				-#	If	the	job	is	an	instrumentation	job,	mark	the	job	as	handled	by	the	Executive.
				-#	If	the	job	is	the	advance_sim_time	job
							-#	Assign	this	job	to	#advance_sim_time_job
							-#	If	the	sim	is	not	restarting,	set	the	next	job	call	time	to	the	advance_sim_time
										job	to	the	current	time.
				-#	The	maximum	number	of	execution	threads	required	by	each	sim_object	is	searched	for
							as	each	sim_object	is	added	to	the	scheduler.
								-#	If	the	incoming	thread	id	is	greater	than	the	current	number	of	threads	in	the
											executive,	create	a	new	Trick::Threads	object.
											Requirement	[@ref	r_exec_thread_4]
								-#	Add	the	new	thread	object	to	the	list	of	threads	handled	by	the	executive.
				-#	Call	Trick::Executive::add_job_to_queue(JobData	*)	to	add	job	to	the	individual	job	class
							Trick::ScheduledQueue
				-#	If	the	sim	is	not	restarting,	convert	the	initial	start,	stop,	and	next	call	times	to
							simulation	tics.		The	next	call	time	is	based	on	the	current	simulation	time	+	job	offset.
							Requirement	[@ref	r_exec_jobs_3]
*/
int	Trick::Executive::add_jobs_to_queue(	Trick::SimObject	*	in_sim_object	,	bool	restart_flag	)	{

				unsigned	int	jj	,	kk	;
				double	max_time	;
				Trick::JobData	*	temp_job		;
				Trick::Threads	*	curr_thread	;
				int	ret	;

				max_time	=	TRICK_MAX_LONG_LONG	/	time_tic_value	;

				for	(	jj	=	0	;	jj	<	in_sim_object->jobs.size()	;	jj++	)	{
								temp_job	=	in_sim_object->jobs[jj]	;

								//	TODO:	should	we	make	unique	names	for	jobs	with	same	name	here?
								/*	assign	job	sim_object_id	for	job	priority	sorting	*/
								temp_job->sim_object_id	=	in_sim_object->id	;
								temp_job->parent_object	=	in_sim_object	;
								if	(	restart_flag	==	false	)	{
												temp_job->name	=	in_sim_object->name	+	"."	+	temp_job->name	;
								}
								temp_job->frame_id	=	temp_job->sim_object_id	+	((double)temp_job->id	/	100.0);

								/*	Find	the	system	and	automatic	jobs	and	mark	them	as	system	jobs.	*/
								if	(	!	temp_job->job_class_name.compare(0,7,"system_")	||
													!	temp_job->job_class_name.compare(0,9,"automatic")	||
													!	temp_job->job_class_name.compare("input_processor_run")	||
													!	temp_job->job_class_name.compare("freeze_automatic")	)	{
												temp_job->set_system_job_class(true)	;
								}

								/*	Some	jobs	are	not	added	to	a	queue	so	should	be	marked	handled	by	default.	*/
								/*	This	avoids	getting	unnecessary	warning	messages	about	jobs	not	being	added	to	any	queue.	*/
								if	((	!	temp_job->job_class_name.compare("instrumentation"))	||
												(	!	temp_job->job_class_name.compare("malfunction"))	)	{
												temp_job->set_handled(true)	;
								}

								/*	search	for	the	advance_sim_time	job	*/
								if	(	!	temp_job->job_class_name.compare("system_advance_sim_time"))	{
												advance_sim_time_job	=	temp_job	;
												if	(	restart_flag	==	false	)	{
																temp_job->next_tics	=	time_tics	;
												}
								}

								/*	Create	a	new	thread	structure	if	the	job	is	specified	to	be	on	a	higher	numbered	thread	*/
								if	(	(temp_job->thread	+	1)	>	threads.size()	)	{
												for	(	kk	=	threads.size()	;	kk	<=	temp_job->thread	;	kk++	)	{
																curr_thread	=	new	Trick::Threads(kk,	rt_nap)	;
																threads.push_back(curr_thread)	;
												}
								}

								/*	Call	add_job_to_queue(JobData	*)	to	add	job	to	proper	Trick::ScheduledQueue	*/
								ret	=	add_job_to_queue(temp_job)	;

								/*	If	add_jobs	is	called	during	initialization,	restart_flag	==	false,
											calcluate	the	cycle/start/stop	times	for	the	job	*/
								if	(	(restart_flag	==	false)	and	(temp_job->job_class	>=	scheduled_start_index)	and	(ret	==	0	))	{

												temp_job->calc_cycle_tics()	;

												if	(	temp_job->start	>	max_time	)	{
																temp_job->disabled	=	true	;
																temp_job->start_tics	=	TRICK_MAX_LONG_LONG	;
												}	else	{
																temp_job->start_tics	=	(long	long)(temp_job->start	*	time_tic_value)	;
												}

												if	(	temp_job->stop	>	max_time	)	{
																temp_job->stop_tics	=	TRICK_MAX_LONG_LONG	;
												}	else	if	(	temp_job->stop	<	temp_job->start	)	{
																temp_job->disabled	=	true	;
												}	else	{
																temp_job->stop_tics	=	(long	long)(temp_job->stop	*	time_tic_value)	;
												}

												if	(	temp_job->disabled	==	false	)	{
																if	(	!	temp_job->job_class_name.compare("integ_loop"))	{
																				temp_job->next_tics		=	time_tics	+	temp_job->cycle_tics	+	temp_job->start_tics	;
																}	else	{
																				temp_job->next_tics		=	time_tics	+	temp_job->start_tics	;
																}
												}	else	{
																temp_job->next_tics		=	TRICK_MAX_LONG_LONG	;
												}
								}
								if	(	!	temp_job->job_class_name.compare("freeze_scheduled"))	{
												temp_job->calc_cycle_tics()	;
												temp_job->stop_tics	=	TRICK_MAX_LONG_LONG	;
								}
				}

				return(0)	;

}

/**
@details
-#	If	the	job	class	matches	one	of	the	following	job	classes:	default_data,	initialization,
			all	scheduled	classes	as	defined	in	the	S_define	file,	and	shutdown.
-#	If	the	job	class	is	a	cyclic	scheduled	job,	add	it	to	the	Trick::ScheduledJobQueue	corresponding
			to	the	thread	number	specified	by	the	job.
-#	Else	add	the	job	the	the	non-cyclic	job	class	Trick::ScheduledJobQueue.
*/
int	Trick::Executive::add_job_to_queue(	Trick::JobData	*	job	)	{

				std::map<std::string,	int>::iterator	class_id_it	;
				std::map<int,	Trick::ScheduledJobQueue	*>::iterator	queue_it	;
				Trick::ScheduledJobQueue	*	curr_queue	;
				Trick::JobData::TagsIterator	it	;

				/*	Add	job	to	convienience	constructs	for	easy	retrival	*/
				all_jobs.insert(std::pair<std::string,	JobData	*>(job->name,job))	;
				all_jobs_vector.push_back(job)	;
				for	(	it	=	job->tags_begin()	;	it	!=	job->tags_end()	;	++it	)	{
								all_tagged_jobs.insert(std::pair<std::string,	JobData	*>(*it,job))	;
				}

				/*	Get	the	job	class	id	from	the	string	of	the	job	class	if	the	class	name	is	known	to	the	scheduler	*/
				if	(	(class_id_it	=	class_map.find(job->job_class_name))	!=	class_map.end()	)	{
								job->job_class	=	class_id_it->second	;

								if	(	job->thread	!=	0	)	{
												/*	Add	threaded	scheduled	jobs	to	the	thread	scheduled	queue	*/
												if	(	job->job_class	>=	scheduled_start_index	)	{
																threads[job->thread]->job_queue.push(job)	;
																//	Add	all	scheduled	jobs	to	the	scheduled_queue	for	use	in	the	multi-threaded	loop
																scheduled_queue.push(job)	;
																return	0	;
												/*	Threaded	top_of_frame/end_of_frame	jobs	go	to	thread	specific	queues.	*/
												}	else	if	(	!	job->job_class_name.compare("top_of_frame"))	{
																threads[job->thread]->top_of_frame_queue.push(job)	;
																return	0	;
												}	else	if	(	!	job->job_class_name.compare("end_of_frame"))	{
																threads[job->thread]->end_of_frame_queue.push(job)	;
																return	0	;
												/*	Other	jobs	classes	are	put	into	the	main	thread	*/
												}	else	if	(	(queue_it	=	class_to_queue.find(job->job_class))	!=	class_to_queue.end()	)	{
																/*	for	non-scheduled	jobs,	the	class_to_queue	map	holds	the	correct	queue	to	insert	the	job	*/
																curr_queue	=	queue_it->second	;
																curr_queue->push(	job	)	;
																return	0	;
												}
								}	else	{
												/*	if	the	job	is	a	"scheduled"	type	job,	insert	the	job	into	the	proper	thread	queue	*/
												if	(	job->job_class	>=	scheduled_start_index	)	{
																threads[0]->job_queue.push(job)	;
																//	Add	all	scheduled	jobs	to	the	scheduled_queue	for	use	in	the	multi-threaded	loop
																scheduled_queue.push(job)	;
																return	0	;
												}	else	if	(	(queue_it	=	class_to_queue.find(job->job_class))	!=	class_to_queue.end()	)	{
																/*	for	non-scheduled	jobs,	the	class_to_queue	map	holds	the	correct	queue	to	insert	the	job	*/
																curr_queue	=	queue_it->second	;
																curr_queue->push(	job	)	;
																return	0	;
												}
								}
				}

				return	-1	;

}
