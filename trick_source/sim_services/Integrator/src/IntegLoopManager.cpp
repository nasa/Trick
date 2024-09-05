/*******************************************************************************

Purpose:
		(Define	the	class	IntegrationManager,	which	provides	the	ability	to
			move	sim	object	from	one	integration	loop	scheduler	to	another.)

Programmers:
		(((David	Hammen)	(Odyssey	Space	Research)	(Oct	2011)	(Initial	version)))
		(((Donna	Panter)	(L3)																					(2012)))
		(((David	Hammen)	(Odyssey	Space	Research)	(Nov	2013)	(Reintegrate	w/	JEOD)))

*******************************************************************************/


//	Local	includes
#include	"trick/IntegLoopManager.hh"

#include	"trick/Integrator.hh"
#include	"trick/IntegJobClassId.hh"
#include	"trick/IntegLoopScheduler.hh"

//	Trick	includes
#include	"trick/MemoryManager.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/exec_proto.hh"
#include	"trick/exec_proto.h"

//	System	includes
#include	<cmath>


/**
	IntegrationManager	default	constructor.
	*/
Trick::IntegrationManager::IntegrationManager()
:
				sim_objects_info()
{}

/**
	Build	representations	of	each	sim	object	in	a	simulation.
	*/
void	Trick::IntegrationManager::build_sim_object_info()	{

				typedef	std::vector<Trick::SimObject*>	SimObjectVector;
				SimObjectVector	sim_object_vector;

				exec_get_sim_objects	(sim_object_vector);

				bool	rebuild;

				//	Determine	if	sim_objects_info	needs	to	be	rebuilt.
				//	There	should	be	a	one-to-one	relation	between	the	just	obtained	vector
				//	and	sim_objects_info.

				//	Simple	case:	Rebuild	is	needed	if	sizes	are	different.
				if	(sim_object_vector.size()	!=	sim_objects_info.size())	{
								rebuild	=	true;
				}

				//	Sizes	match:
				//	Check	that	every	sim	object	is	in	sim_objects_info,	in	order.
				else	{
								rebuild	=	false;
								for	(unsigned	int	ii	=	0;	ii	<	sim_object_vector.size();	++ii)	{
												if	(sim_object_vector[ii]	!=	sim_objects_info[ii].sim_object)	{
																rebuild	=	true;
																break;
												}
								}
				}

				//	Nothing	to	do	if	a	rebuild	isn't	needed.
				//	Note:	This	assumes	that	sim	objects'	jobs	haven't	changed.
				if	(!	rebuild)	{
								return;
				}

				//	Whatever	is	in	sim_objects_info	is	out	of	date	garbage.	Trash	it.
				sim_objects_info.clear();

				//	Construct	an	entry	for	each	sim	object	in	the	sim.
				for	(SimObjectVector::iterator	iter	=	sim_object_vector.begin();
									iter	!=	sim_object_vector.end();
									++iter)
				{
								Trick::SimObject*	sim_object	=	*iter;
								create_sim_object_info	(sim_object);
				}

				//	Now	that	every	sim	object	is	represented,	jobs	that	are	relegated
				//	to	some	other	sim	object	can	be	assigned	to	those	other	sim	objects.
				for	(SimObjectIntegInfoVector::iterator	obj_iter	=	sim_objects_info.begin();
									obj_iter	!=	sim_objects_info.end();
									++obj_iter)
				{
								SimObjectIntegInfo	&	sim_obj_info	=	*obj_iter;
								transfer_relegated_jobs	(sim_obj_info);
				}
}

/**
	Create	a	SimObjectIntegInfo	that	represents	the	specified	sim	object,
	adding	the	representation	to	the	set	of	such	and	populating	the
	object	with	the	jobs	in	the	sim	object	that	pertain	to	the	integration.
	@param	sim_object	The	sim	object	to	be	represented.
	*/
void	Trick::IntegrationManager::create_sim_object_info	(
				Trick::SimObject	*	sim_object)
{
				sim_objects_info.push_back	(SimObjectIntegInfo(sim_object));
				SimObjectIntegInfo	&	sim_obj_info	=	sim_objects_info.back();

				for	(std::vector<Trick::JobData*>::iterator	iter	=	sim_object->jobs.begin();
									iter	!=	sim_object->jobs.end();
									++iter)
				{
								Trick::JobData	*	job	=	*iter;
								void	*	assign_to;
								std::string	*	job_class_name;
								SimObjectIntegInfo::JobClassMap::iterator	class_iter;
//	The	below	code	is	commented	out	because	it	will	not	work	with	the	new	EchoJobs	class
//	I	will	fix	in	the	future.
//						if	(job->job_class	==	Trick::InstrumentationJobClassId)	{
//										Trick::InstrumentationJobData	*	instrument_job	=
//																				dynamic_cast<Trick::InstrumentationJobData*>(job);
//										class_iter	=	sim_obj_info.job_class_to_info.find	(
//																													instrument_job->target_job->job_class_name);
//										job_class_name	=	&instrument_job->target_job->job_class_name;
//										assign_to	=	instrument_job->target_job->sup_class_data;
//						}	else	{
								class_iter	=	sim_obj_info.job_class_to_info.find(job->job_class_name);
								job_class_name	=	&job->job_class_name;
								assign_to	=	job->sup_class_data;
//						}

								if	(class_iter	!=	sim_obj_info.job_class_to_info.end())	{
												JobClassJobs	*	job_info	=	class_iter->second;

												//	All	integration	jobs	and	non-integration	jobs	that	have	a	null
												//	sup_class_data	field	"belong"	directly	to	the	sim	object.
												if	((assign_to	==	NULL)	||
																(job_class_name->compare("integration")	==	0))	{
																job_info->owned_jobs.push_back	(job);

												//	Add	non-integration	jobs	with	non-null	sup_class_data	field
												//	to	the	relegated_jobs	so	these	jobs	can	later	be	assigned	to
												//	the	sim	object	represented	by	the	sup_class_data	field.
												}
												else	{
																job_info->relegated_jobs.push_back	(job);
												}
								}
				}
}

/**
	Transfer	the	relegated	jobs	in	a	SimObjectIntegInfo	to	the	SimObjectIntegInfo
	specfied	by	the	relegated	job's	sup_class_data	handle.
	@param	sim_obj_info	A	member	of	the	manager's	sim_objects_info	vector.
	*/
void	Trick::IntegrationManager::transfer_relegated_jobs	(
				SimObjectIntegInfo	&	sim_obj_info)
{
				//	Walk	over	all	of	the	job	classes,	transferring	relegated	jobs	in	each.
				SimObjectIntegInfo::JobClassMap	&	obj_map	=	sim_obj_info.job_class_to_info;
				for	(SimObjectIntegInfo::JobClassMap::iterator	map_iter	=	obj_map.begin();
									map_iter	!=	obj_map.end();
									++map_iter)
				{
								const	std::string	&	key	=	map_iter->first;
								JobClassJobs	*	jobs	=	map_iter->second;

								//	Walk	over	all	the	relegated	jobs	in	this	job	class,
								//	transferring	each	relegated	job	to	the	sim	object	specified
								//	by	the	job's	sup_class_data	field.
								for	(JobVector::iterator	job_iter	=	jobs->relegated_jobs.begin();
													job_iter	!=	jobs->relegated_jobs.end();
													++job_iter)
								{
												Trick::JobData	*	job	=	*job_iter;

												//	The	job's	sup_class_data	field	should	be	a	pointer	to	an	instance
												//	of	a	SimObject	derived	class.	Find	the	SimObjectIntegInfo
												//	(*if	any*)	that	represents	the	job's	sup_class_data.
												//	Note	that	the	call	precludes	rebuilding.	Rebuilding	the
												//	sim_objects_info	would	be	of	no	value	here.
												SimObjectIntegInfo	*	assign_to	=
																find_sim_object_info	(job->sup_class_data,	false);
												if	(assign_to	!=	NULL)	{
																assign_to->job_class_to_info[key]->owned_jobs.push_back	(job);
												}	else	{
																message_publish	(MSG_ERROR,
																																	"job	%s	lost	it	relegated	sim	object\n",
																																	job->name.c_str());
												}
								}
				}
}

/**
	Clear	all	indications	of	sim	objects	integrated	by	the	specified	scheduler,
	called	in	anticipation	of	respecifying	the	integrated	objects.
	@param	integrator	The	integ	loop	whose	job	queues	are	being	rebuilt.
	*/
void	Trick::IntegrationManager::clear_integrated_by	(
			IntegLoopScheduler	*	integrator)
{
				for	(SimObjectIntegInfoVector::iterator	obj_iter	=	sim_objects_info.begin();
									obj_iter	!=	sim_objects_info.end();
									++obj_iter)
				{
								SimObjectIntegInfo	&	sim_obj_info	=	*obj_iter;
								if	(sim_obj_info.integrated_by	==	integrator)	{
												sim_obj_info.integrated_by	=	NULL;
								}
				}
}

/**
	Find	the	scheduler	responsible	for	integrating	the	specified	sim	object.
	@return	The	integ	loop	that	integrates	the	specified	sim	object.
	@param	sim_object	A	sim	object,	which	may	or	may	not	be	integrated	yet.
	*/
Trick::IntegLoopScheduler*	Trick::IntegrationManager::get_integrated_by	(
				Trick::SimObject*	sim_object)
{
				SimObjectIntegInfo	*	sim_obj_info	=
								find_sim_object_info	(sim_object,	false);

				if	(sim_obj_info	!=	NULL)	{
								return	sim_obj_info->integrated_by;
				}	else	{
								return	NULL;
				}
}

/**
	Note	the	scheduler	responsible	for	integrating	the	specified	sim	object.
	This	annotation	makes	it	possible	to	automatically	remove	a	sim	object	from
	some	integration	loop	if	that	object	is	added	to	another	loop.
	@param	sim_object	A	sim	object	integrated	by	the	specified	integ	loop.
	@param	integrator	The	integ	loop	that	integrates	the	specified	sim	object.
	*/
void	Trick::IntegrationManager::set_integrated_by	(
				Trick::SimObject*	sim_object,
				Trick::IntegLoopScheduler	*	integrator)
{
				//	Find	the	SimObjectIntegInfo	that	represents	the	sim	object.
				SimObjectIntegInfo	*	sim_obj_info	=
								find_sim_object_info	(sim_object,	true);

				if	(sim_obj_info	!=	NULL)	{
								sim_obj_info->integrated_by	=	integrator;
				}
}

/**
	Find	the	managed	sim	object	that	represents	the	specified	sim	object.
	@param	derived_sim_object		pointer	to	the	derived	sim	object	user	wants	to	find.
	*/
Trick::SimObject	*
Trick::IntegrationManager::find_sim_object	(
			const	void	*	derived_sim_object)
{
				SimObjectIntegInfo	*	sim_obj_info	=
								find_sim_object_info	(derived_sim_object,	true);

				if	(sim_obj_info	!=	NULL)	{
								return	sim_obj_info->sim_object;
				}	else	{
								return	NULL;
				}
}

/**
	Clear	all	cached	information	about	sim	objects.
	This	function	should	be	called	as	a	preload_checkpoint	job.
	*/
void	Trick::IntegrationManager::clear_sim_object_info	()	{
				if	(!	sim_objects_info.empty())	{
								sim_objects_info.clear();
				}
}

/**
	Find	the	manager's	representation	(if	any)	of	the	specified	sim	object.
	@return	Pointer	to	the	SimObjectIntegInfo	that	represents	the	sim	object.
	@param	sim_object	Pointer	to	the	sim	object	to	be	found.
	@param	allow_rebuild	boolean	of	whether	the	sim	object	info	should	be	re-built.
	*/
Trick::IntegrationManager::SimObjectIntegInfo*
Trick::IntegrationManager::find_sim_object_info(
				const	Trick::SimObject*	sim_object,
				bool	allow_rebuild)
{
				FindSimObject	search_predicate	(sim_object);

				SimObjectIntegInfoVector::iterator	found_iter	=
								std::find_if	(sim_objects_info.begin(),	sim_objects_info.end(),
																						search_predicate);
				if	(allow_rebuild	&&	(found_iter	==	sim_objects_info.end()))	{
								build_sim_object_info();
								found_iter	=	std::find_if	(sim_objects_info.begin(),
																																			sim_objects_info.end(),
																																			search_predicate);
				}
				if	(found_iter	==	sim_objects_info.end())	{
								return	NULL;
				}
				else	{
								return	&(*found_iter);
				}
}

/**
	Find	the	manager's	representation	(if	any)	of	the	specified	sim	object.
	@return	Pointer	to	the	derived	sim	object.
	@param	derived_sim_object	Pointer	to	the	sim	object	to	be	found.
	@param	allow_rebuild	boolean	to	determine	whether	sim_object_info	should	be	re-built.
	*/
Trick::IntegrationManager::SimObjectIntegInfo*
Trick::IntegrationManager::find_sim_object_info(
				const	void*	derived_sim_object,
				bool	allow_rebuild)
{
				FindDerivedSimObject	search_predicate	(derived_sim_object);

				SimObjectIntegInfoVector::iterator	found_iter	=
								std::find_if	(sim_objects_info.begin(),	sim_objects_info.end(),
																						search_predicate);
				if	(allow_rebuild	&&	(found_iter	==	sim_objects_info.end()))	{
								build_sim_object_info();
								found_iter	=	std::find_if	(sim_objects_info.begin(),
																																			sim_objects_info.end(),
																																			search_predicate);
				}
				if	(found_iter	==	sim_objects_info.end())	{
								return	NULL;
				}
				else	{
								return	&(*found_iter);
				}
}


/**
	Add	jobs	for	the	specified	sim	object	to	the	specified	job	queue.
	@param	job_class_name	The	job	class	name.
	@param	job_class_id			The	job	class	ID	number.
	@param	sim_object					The	sim	object	whose	jobs	of	the	specified	class
																							are	to	be	added	to	the	provided	queue.
	@param	queue										The	job	queue	to	be	updated.
	*/
void	Trick::IntegrationManager::add_jobs_to_queue(const	std::string	&	job_class_name,
																																																		int	job_class_id,
																																																		Trick::SimObject*	sim_object,
																																																		Trick::ScheduledJobQueue	&	queue)
{
				//	Find	the	SimObjectIntegInfo	that	represents	the	specified	sim	object.
				SimObjectIntegInfo	*	sim_obj_info	=	find_sim_object_info	(sim_object);

				//	Sanity	check:	The	SimObjectIntegInfo	should	be	non-null.
				//	A	null	value	means	something	is	very,	very	wrong.
				//	Famous	last	words:	"This	should	never	happen."
				if	(sim_obj_info	==	NULL)	{
								throw	std::out_of_range	("bad	sim	object");
				}

				//	Find	the	JobClassMap	that	represents	the	jobs	for	the	specified	job	class
				//	within	the	SimObjectIntegInfo.
				SimObjectIntegInfo::JobClassMap::iterator	map_iter	=
																								sim_obj_info->job_class_to_info.find	(job_class_name);

				//	Sanity	check:	The	JobClassMap	should	be	valid	(i.e.,	not	the	end()).
				//	An	invalid	value	again	means	something	is	very,	very	wrong.
				if	(map_iter	==	sim_obj_info->job_class_to_info.end())	{
								throw	std::out_of_range	("bad	class	name	"	+	job_class_name);
				}

				//	All	is	cool.	Add	the	owned	jobs	to	the	queue.
				//	Note:	The	jobs	are	added	via	push	rather	than	push_ignore_sim_object.
				JobClassJobs	*	jobs	=	map_iter->second;
				for	(JobVector::iterator	job_iter	=	jobs->owned_jobs.begin();
									job_iter	!=	jobs->owned_jobs.end();	++job_iter)
				{
								Trick::JobData	*	job	=	*job_iter;
								job->job_class	=	job_class_id;
								queue.push_ignore_sim_object	(job);
				}
}

/**
	Complete	the	construction	of	a	SimObjectIntegInfo.
	All	constructors	but	the	copy	constructor	call	this	method.
	*/
void	Trick::IntegrationManager::SimObjectIntegInfo::init_job_class_to_info	()	{
				job_class_to_info["pre_integration"]		=	&pre_integ_jobs;
				job_class_to_info["derivative"]							=	&deriv_jobs;
				job_class_to_info["integration"]						=	&integ_jobs;
				job_class_to_info["dynamic_event"]				=	&dynamic_event_jobs;
				job_class_to_info["post_integration"]	=	&post_integ_jobs;
}
