
#include	<iostream>

#include	"trick/SimObject.hh"
#include	"trick/message_proto.h"

Trick::SimObject::~SimObject()	{
				while	(	!jobs.empty()	)	{
								delete	jobs.back()	;
								jobs.pop_back()	;
				}
}

int	Trick::SimObject::add_tag(	std::string	job_tag	)	{
				std::vector	<Trick::JobData	*>::iterator	it	;
				for	(	it	=	jobs.begin()	;	it	!=	jobs.end()	;	++it	)	{
								(*it)->add_tag(job_tag)	;
				}
				return(0)	;

}

int	Trick::SimObject::add_tag_to_job(	std::string	job_tag	,	std::string	job_name	)	{
				Trick::JobData	*	jd	;
				if	(	(jd	=	get_job(name	+	"."	+	job_name))	!=	NULL	)	{
								return	jd->add_tag(job_tag)	;
				}
				return	-1	;

}

int	Trick::SimObject::add_pre_component_object(	SimObject	*	in_object	,	std::string	in_name	)	{

				/**	@par	Detailed	Design	*/

				/**	@li	push	the	component	object	onto	the	vector	of	pre_component	objects	*/
				in_object->name	=	in_name	;
				pre_component_objects.push_back(in_object)	;

				return(0)	;

}

int	Trick::SimObject::add_post_component_object(	SimObject	*	in_object	,	std::string	in_name	)	{

				/**	@par	Detailed	Design	*/

				/**	@li	push	the	component	object	onto	the	vector	of	post_component	objects	*/
				in_object->name	=	in_name	;
				post_component_objects.push_back(in_object)	;

				return(0)	;

}

Trick::JobData	*	Trick::SimObject::add_job(int	in_thread,	int	in_id,	const	char	*	in_job_class_name	,void*	sup_class_data,
																														double	in_cycle,	const	char	*	in_name,	const	char	*	in_tag,	int	in_phase	,
																														double	in_start	,	double	in_stop)	{

				/**	@par	Detailed	Design	*/
				/**	@li	create	a	new	job	with	the	incoming	characteristics	*/
				Trick::JobData	*	job_ptr	;
				job_ptr	=	new	Trick::JobData(in_thread,in_id,in_job_class_name,sup_class_data,in_cycle,in_name,in_tag,in_phase,in_start,in_stop)	;
				jobs.push_back(job_ptr)	;

				return(job_ptr)	;

}

Trick::JobData	*	Trick::SimObject::get_job(	std::string	job_name,	unsigned	int	j_instance	)	{

				std::vector	<Trick::JobData	*>::iterator	it	;

				//	Look	for	the	name
				for	(	it	=	jobs.begin()	;	it	!=	jobs.end()	;	++it	)	{
								if	(	!	(*it)->name.compare(job_name)	)	{
												if	(	j_instance	==	1	)	{
																return	*it	;
												}	else	{
																j_instance--	;
												}
								}
				}

				//	We	mess	with	the	job	name	and	prepend	the	sim_object	name.	:(
				if	(	job_name.find(name	+	".")	!=	0	)	{
								job_name	=	name	+	"."	+	job_name	;
				}
				//	Look	for	the	job	with	the	sim	object	name	prepended.
				for	(	it	=	jobs.begin()	;	it	!=	jobs.end()	;	++it	)	{
								if	(	!	(*it)->name.compare(job_name)	)	{
												if	(	j_instance	==	1	)	{
																return	*it	;
												}	else	{
																j_instance--	;
												}
								}
				}

				return	NULL	;
}

void	Trick::SimObject::enable_all_jobs()	{
				std::vector	<Trick::JobData	*>::iterator	it	;
				for	(	it	=	jobs.begin()	;	it	!=	jobs.end()	;	++it	)	{
							(*it)->enable()	;
				}
}

void	Trick::SimObject::enable()	{
				message_publish(MSG_WARNING,"SimObject::enable	has	been	deprecated,	use	SimObject::enable_all_jobs	instead\n")	;
				enable_all_jobs();
}


void	Trick::SimObject::disable_all_jobs()	{
				std::vector	<Trick::JobData	*>::iterator	it	;
				for	(	it	=	jobs.begin()	;	it	!=	jobs.end()	;	++it	)	{
							(*it)->disable()	;
				}
}

void	Trick::SimObject::disable()	{
				message_publish(MSG_WARNING,"SimObject::disable	has	been	deprecated,	use	SimObject::disable_all_jobs	instead\n")	;
				disable_all_jobs();
}

