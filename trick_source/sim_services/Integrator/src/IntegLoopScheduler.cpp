

//	Local	includes
#include	"trick/IntegLoopScheduler.hh"

#include	"trick/IntegLoopManager.hh"
#include	"trick/IntegJobClassId.hh"

//	Trick	includes
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/JobData.hh"

//	System	includes
#include	<iostream>
#include	<iomanip>
#include	<cstdarg>
#include	<math.h>


//	Anonymous	namespace	for	local	functions
namespace	{
				/**
					*	Process	the	enabled	job	in	the	provided	job	queue.
					*	@param	job_queue		Job	queue	to	be	processed.
					*/
				inline	void	call_jobs	(
								Trick::ScheduledJobQueue	&	job_queue)
				{
								Trick::JobData	*	curr_job;
								job_queue.reset_curr_index();
								while	((curr_job	=	job_queue.get_next_job())	!=	NULL)	{
												curr_job->call();
								}
				}
}


/**
	The	singleton	IntegrationManager	object.
	It	is	a	protected	static	member	of	the	IntegLoopScheduler	class.
	*/
Trick::IntegrationManager	Trick::IntegLoopScheduler::manager;

/**
	The	Integrator	currently	being	processed.
	*/
Trick::Integrator*	trick_curr_integ	=	NULL;

/**
	Non-default	constructor.
	@param	in_cycle	The	time	interval	at	which	the	loop's	integrate	function	is	called
	@param	in_parent_so	Pointer	to	the	parent	sim	object.
	*/
Trick::IntegLoopScheduler::IntegLoopScheduler(
				double	in_cycle,	Trick::SimObject	*	in_parent_so)
:
				Scheduler	(),

				verbosity	(0),
				last_step_deriv	(false),
				first_step_deriv	(false),
				integ_ptr	(NULL),
				sim_objects	(),

				nominal_cycle	(in_cycle),
				next_cycle	(in_cycle),
				parent_sim_object	(in_parent_so),
				pre_integ_jobs	(),
				deriv_jobs	(),
				integ_jobs	(),
				dynamic_event_jobs	(),
				post_integ_jobs	()
{
				complete_construction();
}

/**
	Default	constructor
	*/
Trick::IntegLoopScheduler::IntegLoopScheduler()
:
				Scheduler	(),

				verbosity	(0),
				last_step_deriv	(false),
				first_step_deriv	(false),
				integ_ptr	(NULL),
				sim_objects	(),

				nominal_cycle	(),
				next_cycle	(),
				parent_sim_object	(NULL),
				pre_integ_jobs	(),
				deriv_jobs	(),
				integ_jobs	(),
				dynamic_event_jobs	(),
				post_integ_jobs	()
{
				complete_construction();
}

/**
	Complete	the	construction	of	an	integration	loop.
	All	constructors	but	the	copy	constructor	call	this	method.
	*/
void	Trick::IntegLoopScheduler::complete_construction()
{

				//	Create	the	class_name	to	class_id	mapping.
				class_map["derivative"]	=	Trick::DerivativeJobClassId;
				class_to_queue[Trick::DerivativeJobClassId]	=	&deriv_jobs;

				class_map["integration"]	=	Trick::IntegrationJobClassId;
				class_to_queue[Trick::IntegrationJobClassId]	=	&integ_jobs;

				class_map["dynamic_event"]	=	Trick::DynamicEventJobClassId;
				class_to_queue[Trick::DynamicEventJobClassId]	=	&dynamic_event_jobs;

				class_map["post_integration"]	=	Trick::PostIntegrationJobClassId;
				class_to_queue[Trick::PostIntegrationJobClassId]	=	&post_integ_jobs;

				class_map["pre_integration"]	=	Trick::PreIntegrationJobClassId;
				class_to_queue[Trick::PreIntegrationJobClassId]	=	&pre_integ_jobs;
}

/**
	Find	the	specified	sim	object	in	the	set	of	objects	managed	by	this	loop.
	@return	Iterator	pointing	to	the	object,	or	to	sim_objects.end()	if	not	found.
	@param	sim_obj	Object	to	be	found.
	*/
Trick::IntegLoopScheduler::SimObjectVector::iterator
Trick::IntegLoopScheduler::find_sim_object(Trick::SimObject	&	sim_obj)
{
				return	std::find	(sim_objects.begin(),	sim_objects.end(),	&sim_obj);
}

/**
	Add	jobs	from	specified	object.
	@param	in_obj	Pointer	to	the	sim	object	from	which	the	integration	jobs
								should	be	added.
	Deprecated!
	*/
int	Trick::IntegLoopScheduler::add_integ_jobs_from_sim_object	(
				Trick::SimObject	*	in_obj)
{

				unsigned	int	ii	;
				std::map<std::string,	int>::iterator	class_id_it	;
				std::map<int,	Trick::ScheduledJobQueue	*>::iterator	queue_it	;
				Trick::ScheduledJobQueue	*	curr_queue	;
				JobData	*	job	;

				//	TODO:		add	check	to	make	sure	the	integ	job	is	not	set	to	another	object	before	adding	job	to	queue
				//	TODO:		Check	other	simobjects	for	jobs	that	are	to	be	added	to	this	integration
				//	TODO:		check	all	integration	schemes	for	integrate	structures	are	the	same

				//	For	each	of	the	jobs	in	the	SimObject
				for	(	ii	=	0	;	ii	<	in_obj->jobs.size()	;	ii++	)	{
								job	=	in_obj->jobs[ii]	;

								//	if	the	job's	job_class_name	is	in	the	class	map	then	set	its	job_class	to	the	corresponding	class_id
								if	(	(class_id_it	=	class_map.find(job->job_class_name))	!=	class_map.end()	)	{

												//	set	this	integration	loop	frequency	to	all	its	individual	jobs
												job->cycle	=	nominal_cycle;

												job->job_class	=	class_id_it->second	;

												if	(	(queue_it	=	class_to_queue.find(job->job_class))	!=	class_to_queue.end()	)	{
																curr_queue	=	queue_it->second	;
																curr_queue->push_ignore_sim_object(	job	)	;
												}
								}
				}

				return	0;

}

//	Overload	of	Scheduler::add_sim_object	that	does	nothing.
int	Trick::IntegLoopScheduler::add_sim_object(Trick::SimObject	*	sim_obj	__attribute((unused)))	{
				return	0;
}

/**
	Add	a	sim	object	to	the	set	of	objects	integrated	by	this	integration	loop.
	The	job	queues	for	this	loop	are	rebuild	after	adding	the	sim	object.
	@param	sim_obj	Address	of	the	sim	object	that	should	be	added	to	scheduler.
	*/
int	Trick::IntegLoopScheduler::add_sim_object(Trick::SimObject	&	sim_obj)
{
				//	Disallow	duplicate	objects.
				if	(find_sim_object	(sim_obj)	!=	sim_objects.end())	{
								message_publish	(
												MSG_ERROR,
												"Integ	Scheduler	ERROR:	"
												"SimObject	'%s'	is	already	managed	by	this	integration	loop.\n",
												sim_obj.name.c_str());
								return	1;
				}

				//	Object	is	not	in	the	set.	Add	it.
				sim_objects.push_back	(&sim_obj);
				//	Rebuild	jobs,	but	only	if	the	manager	has	been	initialized.
				//	Adding	jobs	prior	to	the	initial	build	are	legal;	they	are	addressed
				//	by	an	S_define-level	call	to	rebuild_jobs.
				if	(!	manager.is_empty())	{
								//	If	this	object	is	already	managed	by	a	different	integration	loop,
								//	remove	it	from	that	other	loop	to	avoid	integrating	the	object	twice.
								IntegLoopScheduler	*	other	=	manager.get_integrated_by	(&sim_obj);
								if	(other	!=	NULL)	{
												other->remove_sim_object	(sim_obj);
								}

								//	Now	rebuild	our	jobs.
								rebuild_jobs();
				}
				return	0;
}

/**
	Remove	a	sim	object	from	the	objects	integrated	by	this	integration	loop.
	The	job	queues	for	this	loop	are	rebuilt	after	removing	the	sim	object.
	@param	sim_obj	Address	of	sim	object	that	should	be	removed	from	scheduler.
	*/
int	Trick::IntegLoopScheduler::remove_sim_object(Trick::SimObject	&	sim_obj)
{
				//	Find	the	object	in	the	set	of	objects	managed	by	this	loop;
				//	The	object	must	be	in	that	set	if	it	is	to	be	removed.
				SimObjectVector::iterator	iter	=	find_sim_object	(sim_obj);
				if	(iter	==	sim_objects.end())	{
								message_publish	(
												MSG_ERROR,
												"Integ	Scheduler	ERROR:	"
												"SimObject	'%s'	is	not	managed	by	this	integration	loop.\n",
												sim_obj.name.c_str());
								return	1;
				}

				//	Object	is	in	the	set.	Remove	it.
				sim_objects.erase	(iter);

				//	Rebuild	jobs,	but	only	if	the	manager	has	been	initialized.
				//	Calls	to	remove	prior	to	the	initial	build	are	legal	(but	silly).
				if	(!	manager.is_empty())	{
								rebuild_jobs();
				}
				return	0;
}

/**
	Prepare	for	a	restart.
	*/
void	Trick::IntegLoopScheduler::restart_checkpoint()
{
				manager.clear_sim_object_info();
}

/**
	Rebuild	the	integration	loop's	job	queues.
	This	method	is	called	internally	upon	adding	or	deleting	sim	objects,
	and	is	called	externally	(S_define	level)	during	initialization	and
	after	a	checkpoint	restart.
	*/
void	Trick::IntegLoopScheduler::rebuild_jobs	()
{
				//	Rebuild	the	manager.
				manager.build_sim_object_info();

				//	Reset	the	manager's	notion	of	which	objects	this	integrator	integrates.
				manager.clear_integrated_by	(this);
				for	(SimObjectVector::iterator	so_iter	=	sim_objects.begin();
									so_iter	!=	sim_objects.end();
									++so_iter)	{
								Trick::SimObject	*	sim_object	=	*so_iter;
								manager.set_integrated_by	(sim_object,	this);
				}

				//	Rebuild	each	of	this	integration	loop's	job	queues.
				for	(std::map<std::string,	int>::iterator	id_iter	=	class_map.begin();
									id_iter	!=	class_map.end();
									++id_iter)	{
								const	std::string	&	job_class_name	=	id_iter->first;
								int	job_class_id	=	id_iter->second;
								Trick::ScheduledJobQueue	&	queue	=	*(class_to_queue.at(job_class_id));

								//	Rebuild	the	queue	by	first	tearing	it	down	to	nothing
								//	and	then	rebuilding	from	scratch.
								clear_queue	(queue);
								for	(SimObjectVector::iterator	so_iter	=	sim_objects.begin();
													so_iter	!=	sim_objects.end();
													++so_iter)	{
												Trick::SimObject	*	sim_object	=	*so_iter;
												manager.add_jobs_to_queue	(
																job_class_name,	job_class_id,	sim_object,	queue);
								}
				}
}

/**
	Empty	a	job	queue	in	anticipation	of	the	queue	being	rebuilt.
	@param	job_queue	Address	of	job	queue	that	should	be	cleared.
	*/
void	Trick::IntegLoopScheduler::clear_queue	(
				Trick::ScheduledJobQueue	&	job_queue)
{
				if	(job_queue.size()	!=	0)	{
								Trick::JobData	*	curr_job;
								job_queue.reset_curr_index();
								while	((curr_job	=	job_queue.get_next_job())	!=	NULL)	{
												curr_job->set_handled	(false);
								}
								job_queue.clear	();
				}
}

/**
	Call	the	enabled	derivative	jobs.
	*/
void	Trick::IntegLoopScheduler::call_deriv_jobs	()
{
				call_jobs	(deriv_jobs);
}

/**
	Integrate	objects	to	the	current	simulation	time.
	Note	that	sim_time	is	advanced	before	integration,	so	integration	has	to
	be	from	sim_time-cycle	to	sim_time.
	*/
int	Trick::IntegLoopScheduler::integrate()
{
				double	t_end	=	exec_get_sim_time();
				double	t_start	=	t_end	-	next_cycle;	//	This	is	the	time	of	the	current	state	vector.
				int	status;

				//	Call	all	of	the	jobs	in	the	pre-integration	job	queue.
				call_jobs	(pre_integ_jobs);

				//	Integrate	sim	objects	to	the	current	time.
				status	=	integrate_dt	(t_start,	next_cycle);
				if	(status	!=	0)	{
								return	status;
				}

				//	Process	dynamic	events,	if	any.
				if	(!	dynamic_event_jobs.empty())	{

								status	=	process_dynamic_events	(t_start,	t_end);

								if	(status	!=	0)	{
												return	status;
								}
				}

				//	Call	the	jobs	in	the	derivative	job	queue	one	more	time	if	indicated.
				if	(get_last_step_deriv())	{
									call_jobs	(deriv_jobs);
				}

				//	Call	all	of	the	jobs	in	the	post-integration	job	queue.
				call_jobs	(post_integ_jobs);

				//	We	should	now	be	back	in	sync	with	the	nominal	cycle	rate.
				next_cycle	=	nominal_cycle;

				return	0;
}

/**
	Determine	whether	derivative	jobs	should	be	called	on	the	first	step.
	True	if	any	integrator	needs	derivatives	on	the	first	step,	false	otherwise.
	*/
bool	Trick::IntegLoopScheduler::get_first_step_deriv_from_integrator()
{
				//	We	need	derivatives	if	the	integ_ptr	says	so.
				if	((integ_ptr	!=	NULL)	&&	integ_ptr->first_step_deriv)	{
								return	true;
				}

				//	Other	integrators	may	be	set	in	sup_class_data.	Check	them.
				else	{
								Trick::JobData	*	curr_job;
								void*	sup_class_data;
								Trick::Integrator*	trick_integrator;

								integ_jobs.reset_curr_index();
								while	((curr_job	=	integ_jobs.get_next_job())	!=	NULL)	{
												if	((curr_job->job_class	==	Trick::IntegrationJobClassId)	&&
																((sup_class_data	=	curr_job->sup_class_data)	!=	NULL)	&&
																((trick_integrator	=
																						*(static_cast<Trick::Integrator**>(sup_class_data)))	!=	NULL)	&&
																trick_integrator->first_step_deriv)	{
																return	true;
												}
								}
				}

				return	false;
}

/**
	Determine	whether	derivative	jobs	should	be	called	prior	to	exiting	the
	main	integration	function.	True	if	this	object's	last_deriv_flag	is	set,
	or	if	any	integrator's	last_deriv_flag	is	set.	False	otherwise.
	*/
bool	Trick::IntegLoopScheduler::get_last_step_deriv()
{
				//	We	need	to	calculate	post-integration	derivatives	if	this	object
				//	or	the	integ_ptr	says	to	do	so.
				if	(last_step_deriv	||
								((integ_ptr	!=	NULL)	&&	integ_ptr->last_step_deriv))	{
								return	true;
				}

				//	Other	integrators	may	be	set	in	sup_class_data.	Check	them.
				else	{
								Trick::JobData	*	curr_job;
								void*	sup_class_data;
								Trick::Integrator*	trick_integrator;

								integ_jobs.reset_curr_index();
								while	((curr_job	=	integ_jobs.get_next_job())	!=	NULL)	{
												if	((curr_job->job_class	==	Trick::IntegrationJobClassId)	&&
																((sup_class_data	=	curr_job->sup_class_data)	!=	NULL)	&&
																((trick_integrator	=
																						*(static_cast<Trick::Integrator**>(sup_class_data)))	!=	NULL)	&&
																trick_integrator->last_step_deriv)	{
																return	true;
												}
								}
				}

				return	false;
}

/**
	Integrate	over	the	specified	time	interval.
	*/
int	Trick::IntegLoopScheduler::integrate_dt	(	double	t_start,	double	dt)	{

				int	ipass	=	0;
				int	ex_pass	=	0;
				bool	need_derivs	=	get_first_step_deriv_from_integrator();

				double	target_time	=	t_start	+	dt;

				do	{
								ex_pass	++;
								//	Call	all	of	the	jobs	in	the	derivative	job	queue	if	needed.
								if	(need_derivs)	{
													call_jobs	(deriv_jobs);
								}
								need_derivs	=	true;

								//	Call	all	of	the	jobs	in	the	integration	job	queue.
								Trick::JobData	*	curr_job;
								integ_jobs.reset_curr_index();
								while	((curr_job	=	integ_jobs.get_next_job())	!=	NULL)	{


												void*	sup_class_data	=	curr_job->sup_class_data;
												//	Jobs	without	supplemental	data	use	the	default	integrator.
												if	(sup_class_data	==	NULL)	{
																trick_curr_integ	=	integ_ptr;
												}
												//	Non-null	supplemental	data:
												//	Resolve	as	a	pointer-to-a-pointer	to	a	Trick::Integrator.
												else	{
																trick_curr_integ	=
																				*(static_cast<Trick::Integrator**>(sup_class_data));
												}


												if	(trick_curr_integ	==	NULL)	{
																message_publish	(
																				MSG_ERROR,
																				"Integ	Scheduler	ERROR:	"
																				"Integrate	job	has	no	associated	Integrator.\n");
																return	1;
												}

												if	(ex_pass	==	1)	{
																trick_curr_integ->time	=	t_start;
																trick_curr_integ->dt			=	dt;
																trick_curr_integ->target_integ_time	=	target_time;
												}

												if	(verbosity	||	trick_curr_integ->verbosity)	{
																message_publish	(MSG_DEBUG,	"Job:	%s,	target_integ_time:	%f,	integ_time:	%f,	dt:	%f,	ipass	=	%d\n",
																																	curr_job->name.c_str(),	target_time,	t_start,	dt,	ipass);
												}

												ipass	=	curr_job->call();

												//	Trick	integrators	are	expected	to	advance	from	step	one	to	step
												//	two,	etc.,	and	then	back	to	zero	to	indicate	completion.
												//	All	integrators	are	expected	to	march	to	the	same	beat.
												//	FIXME,	future:	This	restricts	Trick	to	using	only	rather
												//	simple	integration	techniques.
												if	((ipass	!=	0)	&&	(ipass	!=	ex_pass))	{
																message_publish	(
																				MSG_ERROR,
																				"Integ	Scheduler	ERROR:	Integrators	not	in	sync.\n");
																return	1;
												}
								}
				}	while	(ipass);

				return	0;
}

int	Trick::IntegLoopScheduler::process_dynamic_events	(	double	t_start,	double	t_end,	unsigned	int	depth)	{

				bool	fired	=	false;
				double	t_to	=	t_end;
				double	t_from	=	t_start;
				Trick::JobData	*	curr_job;
				dynamic_event_jobs.reset_curr_index();
				while	((curr_job	=	dynamic_event_jobs.get_next_job())	!=	NULL)	{
								//	Call	the	dynamic	event	job	to	calculate	an	estimated	time-to-go.
								double	tgo	=	curr_job->call_double();
							if	(tgo	<	0.0)	{			//	If	there	is	a	root	in	this	interval	...
												fired	=	true;
												while	(tgo	!=	0.0)	{
																if	(tgo	>	0.0)	{
																				t_from	=	t_to;
																				t_to	=	t_to	+	tgo;
																				int	status	=	integrate_dt(	t_from,	(t_to-t_from));
																				if	(status	!=	0)	{	return	status;	}
																}	else	{
#define	FORWARD_INTEGRATION	0
#if	FORWARD_INTEGRATION
																				integ_ptr->state_reset();	//	We	always	want	to	integrate	forward.
																				t_to	=	t_to	+	tgo;
#else
																				t_from	=	t_to;
																				t_to			=	t_from	+	tgo;
#endif
																				int	status	=	integrate_dt(	t_from,	(t_to-t_from));
																				if	(status	!=	0)	{	return	status;	}
																}
																tgo	=	curr_job->call_double();
												}
								}
				}
				/*
				Integrate	to	the	end	of	the	integration	cycle	using	the	updated	derivatives.
				*/
				if	(fired)	{
								t_from	=	t_to;
								t_to	=	t_end;
								/*
								Because	the	derivatives	have	likely	changed,	we	need	to	recursively
								check	for	new	events	in	the	remaining	interval.
								*/
								int	status	=	integrate_dt(	t_from,	(t_to-t_from));
								if	(status	!=	0)	{	return	status;	}
								status	=	process_dynamic_events(t_from,	t_to,	depth+1);
								if	(status	!=	0)	{	return	status;	}
				}
				return	0;
}

/**
	Utility	function	to	get	an	integrator.
	@param	alg	The	integration	algorithm	to	use.
	@param	state_size	the	total	number	of	integration	variables	and	derivative	variables.
	*/
Trick::Integrator	*	Trick::IntegLoopScheduler::getIntegrator	(
				Integrator_type	alg,	unsigned	int	state_size)
{
				integ_ptr	=	Trick::getIntegrator	(alg,	state_size);
				trick_curr_integ	=	integ_ptr;
				return	integ_ptr;
}

/**
	Set	the	interval	at	which	this	job	is	called.
	@param	in_cycle	The	frequency	for	the	integration	cycle.
	*/
int	Trick::IntegLoopScheduler::set_integ_cycle	(
				double	in_cycle)
{
				JobData	*	found_job	=	NULL;
				int	loops_found	=	0;

				//	Search	through	the	parent	sim	object	looking	for	integ_loop	jobs.
				//	There	should	be	exactly	one	such	job.
				for	(std::vector<Trick::JobData	*>::iterator	vit	=
													parent_sim_object->jobs.begin();
									vit	!=	parent_sim_object->jobs.end();
									++vit)	{
								JobData	*	vit_job	=	*vit;
								if	(!	vit_job->job_class_name.compare("integ_loop"))	{
												++loops_found;
												found_job	=	vit_job;
								}
				}

				//	Not	finding	exactly	one	integ_loop	job	is	an	error.
				if	(loops_found	!=	1)	{
								message_publish	(
												MSG_ERROR,
												"Integ	Scheduler	ERROR:	%s\n",
												(loops_found	==	0)	?	"integ_loop	job	not	found."	:
																																	"multiple	integ_loop	jobs	found.");
								return	1;
				}

				//	Found	exactly	one	integ_loop	job.
				//	Set	the	cycle	rate	for	that	one	job	and	for	this	object.
				//	Note:	This	assumes	that	the	one	found	job	pertains	to	this	scheduler.
				else	{
								long	long	prev_tics	=	found_job->next_tics	-	found_job->cycle_tics;
								long	long	curr_tics	=	exec_get_time_tics();
								found_job->set_cycle	(in_cycle);
								found_job->set_next_call_time	(curr_tics);
								nominal_cycle	=	in_cycle;
								next_cycle	=	double((found_job->next_tics	-	prev_tics))	/
																					double(found_job->time_tic_value);
								return	0;
				}
}

/**
	Write	the	content	of	the	S_job_execution	file.
	@param	fp	The	pointer	to	the	file	to	write	the	S_job_execution	file.
	*/
int	Trick::IntegLoopScheduler::write_s_job_execution(FILE	*fp)
{

				if	(fp	==	NULL)	{
								return	0;
				}

				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"Integration	Loop:\n\n")	;
				pre_integ_jobs.write_sched_queue(fp)	;
				deriv_jobs.write_sched_queue(fp)	;
				integ_jobs.write_sched_queue(fp)	;
				dynamic_event_jobs.write_sched_queue(fp)	;
				post_integ_jobs.write_sched_queue(fp)	;

				return	0;
}

/**
	Adds	the	incoming	instrumentation	job	before	target	job	if	specified,	or	all	jobs	in	the	list.
	@param	instrument_job	Pointer	to	the	job	that	should	be	run	previously.
	*/
int	Trick::IntegLoopScheduler::instrument_job_before	(
				Trick::JobData	*	instrument_job)
{
				int	count	=	0	;
				/**	@par	Detailed	Design	*/

				count	+=	pre_integ_jobs.instrument_before(instrument_job)	;
				count	+=	deriv_jobs.instrument_before(instrument_job)	;
				count	+=	integ_jobs.instrument_before(instrument_job)	;
				count	+=	dynamic_event_jobs.instrument_before(instrument_job)	;
				count	+=	post_integ_jobs.instrument_before(instrument_job)	;

				/**	@li	Return	how	many	insertions	were	done.	*/
				return	count;

}

/**
	Adds	the	incoming	instrumentation	job	after	target	job	if	specified,	or	all	jobs	in	the	list.
	@param	instrument_job	Pointer	to	the	job	that	should	be	run	after	each	job.
	*/
int	Trick::IntegLoopScheduler::instrument_job_after	(
				Trick::JobData	*	instrument_job)
{
				int	count	=	0	;

				count	+=	pre_integ_jobs.instrument_after(instrument_job)	;
				count	+=	deriv_jobs.instrument_after(instrument_job)	;
				count	+=	integ_jobs.instrument_after(instrument_job)	;
				count	+=	dynamic_event_jobs.instrument_after(instrument_job)	;
				count	+=	post_integ_jobs.instrument_after(instrument_job)	;

				return	count;

}

/**
	Removes	all	jobs	in	the	list	that	match	the	name	in_job.
	@param	in_job	name	of	the	job	that	should	be	removed.
	*/
int	Trick::IntegLoopScheduler::instrument_job_remove(std::string	in_job)
{
				pre_integ_jobs.instrument_remove(in_job)	;
				deriv_jobs.instrument_remove(in_job)	;
				integ_jobs.instrument_remove(in_job)	;
				dynamic_event_jobs.instrument_remove(in_job)	;
				post_integ_jobs.instrument_remove(in_job)	;

				return	0;
}
