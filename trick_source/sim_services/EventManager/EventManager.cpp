/*
			PURPOSE:	(	Python	input	processor	)
			REFERENCE:	(	Trick	Simulation	Environment	)
			ASSUMPTIONS	AND	LIMITATIONS:	(	None	)
			CLASS:	(	N/A	)
			LIBRARY	DEPENDENCY:	(	None	)
			PROGRAMMERS:	(	Alex	Lin	NASA	2009	)
*/

#include	<iostream>
#include	<sstream>
#include	<vector>
#include	<string>
#include	<cmath>

#include	"trick/EventManager.hh"
#include	"trick/EventInstrument.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/exec_proto.h"
#include	"trick/exec_proto.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

Trick::EventManager	*	the_em	;

Trick::EventManager::EventManager()	:
	active_events(NULL),
	num_active_events(0),
	num_allocated(0)
{	the_em	=	this	;	}

//Command	to	get	the	event	object	given	the	event's	name
Trick::Event	*	Trick::EventManager::get_event(std::string	event_name)	{
				unsigned	int	ii	;
				Trick::Event*	ret	=	NULL;

				/*	Find	the	event	name	in	the	active	event	list	and	return	the	pointer	to	that	event	*/
				for	(	ii	=	0	;	ii	<	num_active_events	;	ii++	)	{
								if	(	!	active_events[ii]->get_name().compare(event_name)	)	{
												ret	=	active_events[ii];
												break	;
								}
				}

				return(ret)	;

}

//Add	user's	event	to	the	active	event	list.
int	Trick::EventManager::add_to_active_events(Trick::Event	*	in_event)	{

				for	(	unsigned	int	ii	=	0	;	ii	<	num_active_events	;	ii++	)	{
								if	(in_event	==	active_events[ii])	{
												return	(0)	;
								}
				}
				num_active_events++;
				if	(num_active_events	==	1)	{
								active_events	=	(Trick::Event	**)TMM_declare_var_s("Trick::Event*	[100]");
								num_allocated	=	100	;
				}	else	if	(	num_active_events	>=	num_allocated	)	{
								num_allocated	+=	100	;
								active_events	=	(Trick::Event	**)TMM_resize_array_1d_a(active_events,	num_allocated);
								for	(	unsigned	int	ii	=	num_active_events	;	ii	<	num_allocated	;	ii++	)	{
												active_events[ii]	=	NULL	;
								}
				}
				active_events[num_active_events-1]	=	in_event	;
				return	(0)	;
}

//Command	to	insert	a	user's	input	file	event	into	the	input	processor's	list	of	events	to	process.
int	Trick::EventManager::add_event(Trick::Event	*	in_event)	{

				int	ret	=	0	;

				add_to_active_events(in_event)	;

				if	(in_event->get_thread()	<	event_processors.size()	)	{
								event_processors[in_event->get_thread()]->add_event(in_event)	;
								in_event->add()	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	thread	number	is	not	in	range:	thread	%d	requested,	only	%d	child	threads	in	simulation.\n",	in_event->get_thread()	,	event_processors.size()	-	1)	;
								ret	=	-1	;
				}

				return	ret	;

}

/**
@details
Command	to	insert	an	instrument	job	(containing	in_event)	into	job	queue	before	target	job.

-#	Find	the	target	job.
-#	If	the	target	job	exists
	-#	Save	the	target	job	information	to	the	event.
	-#	Create	an	EventInstrument	with	the	event	and	target	job	information
	-#	Add	the	event	instrument	to	the	target	job
	-#	call	the	event's	add	routine	if	it	needs	to	do	anything	once	it	is	activated.
-#	else	print	a	warning	that	the	target	job	does	not	exist.
-#	Add	the	event	to	the	list	of	active	events.
*/
int	Trick::EventManager::add_event_before(Trick::Event	*	in_event,	std::string	target_name,	unsigned	int	target_inst)	{

				Trick::JobData	*	target_job	;

				target_job	=	exec_get_job(	target_name.c_str()	,	target_inst	)	;
				if	(target_job	!=	NULL	)	{
								in_event->set_before_after(Trick::EVENT_BEFORETARGET)	;
								in_event->set_target_name(target_name)	;
								in_event->set_target_inst(target_inst)	;
								Trick::EventInstrument	*	instru_job	=	new	Trick::EventInstrument(in_event	,	target_job)	;
								target_job->add_inst_before(instru_job)	;
								events_instrumented.push_back(instru_job)	;
								in_event->add()	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	not	added	before	job:	%s	does	not	exist.\n",	target_name.c_str())	;
								return	(0);
				}
				/*	Add	in_event	to	list	of	active	events.	*/
				add_to_active_events(in_event)	;
				return(0);

}

/**
@details
Command	to	insert	an	instrument	job	(containing	in_event)	into	job	queue	after	target	job.

-#	Find	the	target	job.
-#	If	the	target	job	exists
	-#	Save	the	target	job	information	to	the	event.
	-#	Create	an	EventInstrument	with	the	event	and	target	job	information
	-#	Add	the	event	instrument	to	the	target	job
	-#	call	the	event's	add	routine	if	it	needs	to	do	anything	once	it	is	activated.
-#	else	print	a	warning	that	the	target	job	does	not	exist.
-#	Add	the	event	to	the	list	of	active	events.
*/
int	Trick::EventManager::add_event_after(Trick::Event	*	in_event,	std::string	target_name,	unsigned	int	target_inst)	{

				Trick::JobData	*	target_job	;

				target_job	=	exec_get_job(	target_name.c_str()	,	target_inst	)	;
				if	(target_job	!=	NULL	)	{
								in_event->set_before_after(Trick::EVENT_AFTERTARGET)	;
								in_event->set_target_name(target_name)	;
								in_event->set_target_inst(target_inst)	;
								Trick::EventInstrument	*	instru_job	=	new	Trick::EventInstrument(in_event	,	target_job)	;
								target_job->add_inst_after(instru_job)	;
								events_instrumented.push_back(instru_job)	;
								in_event->add()	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	not	added	before	job:	%s	does	not	exist.\n",	target_name.c_str())	;
								return	(0);
				}
				/*	Add	in_event	to	list	of	active	events.	*/
				add_to_active_events(in_event)	;
				return(0);

}

//	Find	the	event	by	name	and	activate	it.
int	Trick::EventManager::activate_event(const	char	*	event_name)	{
				Trick::Event*	ret	=	NULL;

				ret	=	get_event(event_name)	;
				if	(ret	!=	NULL)	{
								ret->activate()	;
				}
				return	0	;
}

//	Find	the	event	by	name	and	deactivate	it.
int	Trick::EventManager::deactivate_event(const	char	*	event_name)	{
				Trick::Event*	ret	=	NULL;

				ret	=	get_event(event_name)	;
				if	(ret	!=	NULL)	{
								ret->deactivate()	;
				}
				return	0	;
}

//Command	to	remove	an	event	from	everywhere	it	was	added	(it	can	still	be	added	back	again).
int	Trick::EventManager::remove_event(Trick::Event	*	in_event)	{

				unsigned	int	ii	,	jj	;

				if	(	!	in_event	)	{
								return	0	;
				}

				if	(	in_event->get_before_after()	==	Trick::EVENT_NOTARGET	)	{
								/*	If	the	event	is	cyclic,	remove	the	event	from	the	event	processor	on	the	event's	thread	*/
								event_processors[in_event->get_thread()]->remove_event(in_event)	;
				}	else	{
								std::vector<	Trick::EventInstrument	*	>::iterator	ei_it	;
								for	(	ei_it	=	events_instrumented.begin()	;	ei_it	!=	events_instrumented.end()	;	)	{
												if	(	(*ei_it)->get_event()	==	in_event	)	{
																/*	If	the	event	is	tied	to	a	job,	call	the	target	jobs	remove	instrument	routine	*/
																Trick::EventInstrument	*	found_event_instru	=	*ei_it	;
																ei_it	=	events_instrumented.erase(ei_it)	;
																found_event_instru->get_target_job()->remove_inst(found_event_instru->name)	;
																delete	found_event_instru	;
												}	else	{
																ei_it++	;
												}
								}
				}

				in_event->remove()	;

				/*	Remove	it	from	active	event	list.	*/
				for	(	ii	=	0	;	ii	<	num_active_events	;	ii++	)	{
								if	(in_event	==	active_events[ii])	{
												for	(	jj	=	ii	+	1	;	jj	<	num_active_events	;	jj++	)	{
																active_events[jj	-	1]	=	active_events[jj]	;
												}
												num_active_events--	;
												if	(num_active_events	==	0)	{
																TMM_delete_var_a(active_events);
																active_events	=	NULL;
												}
												else	{
																active_events[num_active_events]	=	NULL	;
												}

												break	;
								}
				}

				if	(	in_event->get_free_on_removal()	)	{
								TMM_delete_var_a(in_event)	;
				}

				return	0	;
}

/**
@details
This	is	called	from	the	S_define	file.		There	will	be	one	event	processor	assigned	to	each	thread.
-#	Add	the	incoming	input	processor	to	the	list	of	known	processors.
*/
void	Trick::EventManager::add_event_processor(Trick::EventProcessor	*	in_ep)	{
				event_processors.push_back(in_ep)	;
}

//Executive	time_tic	changed.		Update	all	event	times
int	Trick::EventManager::time_tic_changed()	{

				int	old_tt	;
				int	tt	;
				unsigned	int	ii	;

				old_tt	=	exec_get_old_time_tic_value()	;
				tt	=	exec_get_time_tic_value()	;

				for	(	ii	=	0	;	ii	<	num_active_events	;	ii++	)	{
								active_events[ii]->set_next_tics((long	long)round((active_events[ii]->get_next_tics()	/	old_tt)	*	tt))	;
				}

				return	0	;
}

//	Remove	all	events	attached	to	jobs	before	a	checkpoint	is	reloaded.
int	Trick::EventManager::preload_checkpoint()	{
				std::vector<	Trick::EventInstrument	*	>::iterator	ei_it	;
				for	(	ei_it	=	events_instrumented.begin()	;	ei_it	!=	events_instrumented.end()	;	)	{
								Trick::EventInstrument	*	found_event_instru	=	*ei_it	;
								ei_it	=	events_instrumented.erase(ei_it)	;
								found_event_instru->get_target_job()->remove_inst(found_event_instru->name)	;
								delete	found_event_instru	;
				}
				return	0	;
}

//Restart	job	that	reloads	event_list	from	checkpointable	structures
int	Trick::EventManager::restart()	{
				unsigned	int	ii	;

				for	(	ii	=	0	;	ii	<	num_active_events	;	ii++	)	{
								//	rebuild	the	event_list	of	all	events	that	were	"added"

								/*	call	the	event	restart	routine	to	reset	anything	the	event	needs	to	do.	*/
								active_events[ii]->restart()	;

								if	(	active_events[ii]->get_before_after()	==	Trick::EVENT_BEFORETARGET	)	{
												/*	Update	event	list	and	insert	an	instrument	job	(containing	in_event)	target	job's	"before"	queue.	*/
												add_event_before(	active_events[ii]	,	active_events[ii]->get_target_name()	,	active_events[ii]->get_target_inst()	)	;
								}	else	if	(	active_events[ii]->get_before_after()	==	Trick::EVENT_AFTERTARGET	)	{
												/*	Update	event	list	and	insert	an	instrument	job	(containing	in_event)	target	job's	"after"	queue.	*/
												add_event_after(	active_events[ii]	,	active_events[ii]->get_target_name()	,	active_events[ii]->get_target_inst()	)	;
								}	else	{
												/*	Update	event	list	for	normal	user	events	and	read	events.	*/
												add_event(active_events[ii]);
								}
				}
				return	(0);
}

