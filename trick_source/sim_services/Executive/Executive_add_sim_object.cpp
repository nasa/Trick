
#include	<iostream>

#include	"trick/Executive.hh"

/**
@details
-#	Assign	the	name	of	the	sim_object.
-#	Call	add_sim_object(Trick::SimObject	*,	const	char	*)	for	component	objects	that
			are	to	be	processed	before	the	current	object.
-#	Assign	the	incoming	SimObject	a	unique	id.
-#	Push	the	object	onto	the	list	of	SimObjects	handled	by	the	Executive.
-#	Call	add_jobs_to_queue(SimObject	*,	bool)	to	add	the	jobs	in	the
			sim_object	to	the	scheduler.
-#	Call	add_sim_object(Trick::SimObject	*,	const	char	*)	for	component	objects	that
			are	to	be	processed	after	the	current	object.
	*/
int	Trick::Executive::add_sim_object(	Trick::SimObject	*	in_object	)	{
				return	add_sim_object(	in_object,	NULL	)	;
}

int	Trick::Executive::add_sim_object(	Trick::SimObject	*	in_object	,	const	char	*	in_name	)	{

				unsigned	int	ii,	jj	;

				if	(	in_name	!=	NULL	)	{
								in_object->name	=	in_name	;
				}

				for	(	jj	=	0	;	jj	<	in_object->pre_component_objects.size()	;	jj++	)	{
								in_object->pre_component_objects[jj]->name.insert(0	,	in_object->name	+	std::string("."))	;
								add_sim_object(in_object->pre_component_objects[jj])	;
				}

				in_object->id	=	++num_sim_objects	;
				//std::cout	<<	"HERE	with	"	<<	in_object->name	<<	"	id	=	"	<<	in_object->id	<<	std::endl	;
				sim_objects.push_back(in_object)	;
				add_jobs_to_queue(	in_object	)	;

				for	(	ii	=	0	;	ii	<	other_schedulers.size()	;	ii++	)	{
								other_schedulers[ii]->add_sim_object(in_object)	;
				}

				for	(	jj	=	0	;	jj	<	in_object->post_component_objects.size()	;	jj++	)	{
								in_object->post_component_objects[jj]->name.insert(0	,	in_object->name	+	std::string("."))	;
								add_sim_object(in_object->post_component_objects[jj])	;
				}

				return(0)	;

}

