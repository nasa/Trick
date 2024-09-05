
#include	<iostream>
#include	<string.h>

#include	"trick/Executive.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/ScheduledJobQueue.hh"
#include	"trick/Threads.hh"

/**
@details
-#	verify	that	all	jobs	in	the	checkpoint	are	present	in	the	current	simulation
-#	Clear	all	current	job	information	in	scheduler
-#	Clear	all	current	sim_object	information	in	scheduler
-#	Add	the	sim_objects	in	checkpoint	to	the	scheduler
-#	Call	post_checkpoint	to	delete	memory	for	checkpoint
*/
int	Trick::Executive::restart()	{

				unsigned	int	ii	;
				std::multimap<std::string,	Trick::JobData	*>::iterator	it	;
				std::pair<std::multimap<std::string,	Trick::JobData	*>::iterator,	std::multimap<std::string,	Trick::JobData	*>::iterator>	ret	;
				std::vector	<Trick::SimObject	*>::iterator	sit	;
				std::vector	<Trick::JobData	*>::iterator	jit	;
				std::multimap<std::string,	Trick::JobData	*>	all_jobs_from_checkpointed_objects	;

				/*
							Save	the	current	job	index	position	of	the	input	processor	queue.	This	queue	position	is	special
							because	of	the	possibility	of	restarting	from	a	checkpoint	read	in	from	the	input	file.		When
							we	restart	from	a	checkpoint	we	reset	the	queues.		If	we	reset	the	input_processor	queue	we'll
							call	the	input_processor	again...	and	we	can	end	up	in	recursive	hell.		Save	the	current	position
							and	replace	the	queue	position	after	the	queues	have	been	rebuilt.		This	ensures	that	we	don't
							call	the	input	file	twice,	or	more!
					*/
				unsigned	int	current_ip_index	=	input_processor_queue.get_curr_index()	;

				/*	clear	all	current	job	information	in	scheduler.		These	will	be	repopulated	when	we	call	add_job_to_queue	*/
				clear_scheduled_queues()	;
				all_jobs.clear()	;
				all_jobs_vector.clear()	;
				all_tagged_jobs.clear()	;

				/*	Create	a	temporary	all_jobs	map	to	use	to	restore	job	data	from	all_jobs_for_checkpoint	*/
				for	(	sit	=	sim_objects.begin()	;	sit	!=	sim_objects.end()	;	++sit	)	{
								for	(	jit	=	(*sit)->jobs.begin()	;	jit	!=	(*sit)->jobs.end()	;	++jit	)	{
												/*	Dynamically	created	sim_objects,	like	data_recording	objects,	require	that
															some	of	the	job	class	data	be	repopulated	before	we	start
															copying	job	data	back	into	the	sim.	*/
												(*jit)->sim_object_id	=	(*sit)->id	;
												(*jit)->parent_object	=	(*sit)	;
												(*jit)->frame_id	=	(*jit)->sim_object_id	+	((double)(*jit)->id	/	100.0);
												std::string	so_name_and_period	=	(*sit)->name	+	"."	;
												if	(	strncmp(	(*jit)->name.c_str()	,	so_name_and_period.c_str(),	so_name_and_period.length()))	{
																(*jit)->name	=	(*sit)->name	+	"."	+	(*jit)->name	;
												}
												all_jobs_from_checkpointed_objects.insert(std::pair<std::string,	JobData	*>((*jit)->name,(*jit)))	;
								}
				}

				/*	verify	that	all	jobs	in	the	checkpoint	are	present	in	the	current	simulation	*/
				for	(	ii	=	0	;	ii	<	num_all_jobs	;	ii++	)	{
								ret	=	all_jobs_from_checkpointed_objects.equal_range(all_jobs_for_checkpoint[ii].name)	;
								if	(	ret.first	==	all_jobs_from_checkpointed_objects.end()	)	{
												message_publish(MSG_ERROR,	"Could	not	find	job	%s\n",	all_jobs_for_checkpoint[ii].name.c_str())	;
												exec_terminate_with_return(	-1	,	__FILE__	,	__LINE__	,	"Job	in	checkpoint	file	does	not	exist	in	current	sim."	)	;
								}	else	{
												for	(	it	=	ret.first	;	it	!=	ret.second	;	++it	)	{
																//	The	JobData::id	and	JobData::sim_object_id	together	uniquely	identify	a	job.
																if	((	it->second->id												==	all_jobs_for_checkpoint[ii].id	)	&&
																				(	it->second->sim_object_id	==	all_jobs_for_checkpoint[ii].sim_object_id	))	{
																				//	copy	the	job	information	from	the	checkpoint	back	to	the	job.
																				it->second->copy_from_checkpoint(&all_jobs_for_checkpoint[ii])	;
																}
												}
								}
				}

				/*	restore	the	executive	sim_objects	vector	from	the	checkpoint	and	add	back	all	of
							the	jobs	to	the	schedulers	*/
				for	(	sit	=	sim_objects.begin()	;	sit	!=	sim_objects.end()	;	++sit	)	{
								add_jobs_to_queue(*sit,	true)	;
								for	(	ii	=	0	;	ii	<	other_schedulers.size()	;	ii++	)	{
												other_schedulers[ii]->add_sim_object(*sit)	;
								}
				}
				num_sim_objects	=	sim_objects.size()	;

				//	The	queues	have	been	rebuilt,	restore	the	current	position	of	the	input	processor	queue.
				input_processor_queue.set_curr_index(current_ip_index)	;

				//	set	the	restart_called	flag	to	true.		This	flag	is	checked	during	Executive::init.		If
				//	we	have	restarted	from	a	checkpoint,	we	will	be	skipping	initialization	jobs.
				restart_called	=	true	;

				/*	Delete	temporary	memory	used	to	restore	jobs	*/
				TMM_delete_var_a(all_jobs_for_checkpoint)	;
				all_jobs_for_checkpoint	=	NULL	;

				/*	Set	the	main	thread	current	time	to	the	simulation	time	tics	value,	used	with	Executive::get_sim_time()	*/

				threads[0]->curr_time_tics	=	time_tics	;

				//	if	we	restarted	from	scratch	without	calling	init	jobs,	need	to	create	child	threads
				create_threads()	;

				//	make	sure	asynchronous	must	finish	jobs	restart	at	correct	time
				for	(ii	=	1;	ii	<	threads.size()	;	ii++)	{
								Threads	*	curr_thread	=	threads[ii]	;
								if	(	(curr_thread->process_type	==	PROCESS_TYPE_AMF_CHILD)	||
													(curr_thread->process_type	==	PROCESS_TYPE_ASYNC_CHILD)	)	{
												if	(	curr_thread->amf_cycle_tics	>	0	and	time_tics	%	curr_thread->amf_cycle_tics	)	{
																curr_thread->amf_next_tics	=	time_tics	+	curr_thread->amf_cycle_tics	-	(time_tics	%	curr_thread->amf_cycle_tics)	;
												}	else	{
																curr_thread->amf_next_tics	=	time_tics	;
												}
								}
				}

				//	If	we	are	in	freeze,	reset	the	freeze_scheduled	queue,	we'll	restart	freeze	time	at	0
				if	(	mode	==	Freeze	)	{
								init_freeze_scheduled()	;
				}


				return(0)	;
}
