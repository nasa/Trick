
#include	<iostream>

#include	"trick/Executive.hh"

/**
@details
-#	Call	delete_sim_object(Trick::SimObject	*)	for	component	objects	that
			are	to	be	processed	before	the	current	object.
-#	Remove	the	object	from	the	sim_object	vector
-#	Call	remove_jobs_to_queue(SimObject	*)	to	remove	the	jobs	in	the	sim_object	from	the	scheduler.
-#	Call	delete_sim_object(Trick::SimObject	*)	for	component	objects	that
			are	to	be	processed	after	the	current	object.
	*/
int	Trick::Executive::delete_sim_object(	Trick::SimObject	*	in_object	)	{

				std::vector	<Trick::SimObject	*>::iterator	so_it	;
				std::vector	<Trick::Scheduler	*>::iterator	sched_it	;

				for	(	so_it	=	in_object->pre_component_objects.begin()	;	so_it	!=	in_object->pre_component_objects.end()	;	++so_it	)	{
								delete_sim_object((*so_it))	;
				}

				for	(	so_it	=	sim_objects.begin()	;	so_it	!=	sim_objects.end()	;	)	{
								if	(	(*so_it)	==	in_object	)	{
												so_it	=	sim_objects.erase(so_it)	;
								}	else	{
											so_it++	;
								}
				}

				remove_jobs(	in_object	)	;

				for	(	sched_it	=	other_schedulers.begin()	;	sched_it	!=	other_schedulers.end()	;	++sched_it	)	{
								(*sched_it)->delete_sim_object(in_object)	;
				}

				for	(	so_it	=	in_object->post_component_objects.begin()	;	so_it	!=	in_object->post_component_objects.end()	;	++so_it	)	{
								delete_sim_object((*so_it))	;
				}

				return(0)	;

}

