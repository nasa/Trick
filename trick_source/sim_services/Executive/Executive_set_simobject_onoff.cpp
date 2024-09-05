
#include	<iostream>

#include	"trick/Executive.hh"

int	Trick::Executive::set_sim_object_jobs_onoff(std::string	sim_object_name,	int	on)	{

				SimObject	*	sim_object	=	get_sim_object_by_name(sim_object_name);
				if	(sim_object	==	NULL)	{
								return	-1;
				}

				if	(on)	{
								sim_object->enable_all_jobs();
				}	else	{
								sim_object->disable_all_jobs();
				}

				return	0;
}


int	Trick::Executive::set_sim_object_onoff(std::string	sim_object_name,	int	on)	{
				
				SimObject	*	sim_object	=	get_sim_object_by_name(sim_object_name);
				if	(sim_object	==	NULL)	{
								return	-1;
				}

				sim_object->object_disabled	=	!on;
				if	(on)	{
								//	Apply	saved	state
								for	(	int	i	=	0	;	i	<	sim_object->jobs.size()	;	i++	)	{
												std::map<Trick::JobData	*,	bool>::iterator	saved_state_it	=	sim_object->saved_job_states.find(sim_object->jobs[i]);
												if	(saved_state_it	!=	sim_object->saved_job_states.end())	{
																sim_object->jobs[i]->disabled	=	saved_state_it->second;
												}	else	{
																//	If	job	is	not	in	map,	turn	it	on
																sim_object->jobs[i]->disabled	=	false;
												}
								}
								sim_object->saved_job_states.clear();
				}	else	{
								for	(	int	i	=	0	;	i	<	sim_object->jobs.size()	;	i++	)	{
												//	Save	state,	turn	off
												sim_object->saved_job_states[sim_object->jobs[i]]	=	sim_object->jobs[i]->disabled;
												sim_object->jobs[i]->disabled	=	true	;
								}
				}
				return(0)	;
}

int	Trick::Executive::get_sim_object_onoff	(std::string	sim_object_name)	{

					SimObject	*	sim_object	=	get_sim_object_by_name(sim_object_name);
				if	(sim_object	==	NULL)	{
								return	-1;
				}

				return	!(sim_object->object_disabled)	;
}

