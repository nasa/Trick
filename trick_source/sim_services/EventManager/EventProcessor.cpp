/*
			PURPOSE:	(	Process	an	event.	)
			REFERENCE:	(	Trick	Simulation	Environment	)
			ASSUMPTIONS	AND	LIMITATIONS:	(	None	)
			CLASS:	(	N/A	)
			LIBRARY	DEPENDENCY:	(	None	)
			PROGRAMMERS:	(	Alex	Lin	NASA	2014	)
*/

#include	<iostream>

#include	"trick/EventProcessor.hh"
#include	"trick/TrickConstant.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	Add	the	incoming	event	to	the	list	of	events	to	be	added	to	the	processor.
*/
void	Trick::EventProcessor::add_event(Trick::Event	*	in_event)	{
				pending_events.push_back(in_event)	;
}

/**
@details
-#	Remove	the	incoming	event	from	the	set	of	events	being	processed
-#	If	the	event	set	is	not	empty	set	the	process_event	job	to	the	first	event's	next	job	call	time.
-#	Else	set	the	process_event	job	to	maximum	time
*/
void	Trick::EventProcessor::remove_event(Trick::Event	*	in_event)	{
				std::multiset<	Trick::Event	*,	CompareEventPtrs	>::iterator	sit	=	event_set.begin()	;
				for	(sit=event_set.begin();	sit!=event_set.end();	++sit)	{
								if	((*sit)	==	in_event)	{
												event_set.erase(sit)	;
												break;
								}
				}
				if	(	!event_set.empty()	)	{
								process_event_job->next_tics	=	(*(event_set.begin()))->get_next_tics()	;
				}	else	{
								//	if	there	are	no	events,	set	the	next	job	call	time	to	max.
								process_event_job->next_tics	=	TRICK_MAX_LONG_LONG	;
				}
}

//Top	of	frame	job	to	add	new	events	to	processing	set.
void	Trick::EventProcessor::add_pending_events(	long	long	curr_tics	,	bool	is_restart	)	{
				std::vector<	Trick::Event	*	>::iterator	it	;
				for	(	it	=	pending_events.begin()	;	it	!=	pending_events.end()	;	)	{
								if	(	(*it)->get_cycle_tics()	!=	0	)	{
												//	this	is	a	cyclic	event
												//	If	we	are	not	calling	this	routine	during	a	restart
												if	(	!	is_restart	)	{
																//	Set	the	next	time	to	the	current	time
																(*it)->set_next_tics(	curr_tics	)	;
												}
								}	else	{
												//	this	is	a	one	time	event,	test	to	see	if	the	event	time	has	past.
												if	(	(*it)->get_next_tics()	<	curr_tics	)	{
																//	if	the	time	has	past,	print	warning	and	move	to	the	next	event.
																message_publish(MSG_WARNING,	"One	time	event	fire	time	has	already	past.\n")	;
																it	=	pending_events.erase(it)	;
																continue	;
												}
								}
								event_set.insert(*it)	;
								process_event_job->next_tics	=	(*(event_set.begin()))->get_next_tics()	;
								it	=	pending_events.erase(it)	;
				}
}

//Automatic	job	to	process	input	file	events.
int	Trick::EventProcessor::process_event(	long	long	curr_tics	)	{

				std::multiset<	Trick::Event	*,	CompareEventPtrs	>::iterator	sit	=	event_set.begin()	;

				//	while	we	have	an	event	that	matches	the	current	time.
				while	(	sit	!=	event_set.end()	and	(*sit)->get_next_tics()	==	curr_tics	)	{
								Trick::Event	*	curr_event	=	*sit	;

								//	erase	the	current	event	out	of	the	set.
								event_set.erase(sit)	;

								//	if	the	event	is	active	process	it
								if	(	curr_event->is_active()	)	{
												curr_event->process(curr_tics)	;
								}

								//	if	the	event	has	a	cycle	time,	update	the	time	and	put	the	item	back	in	the	set
								if	(	curr_event->get_cycle_tics()	!=	0	)	{
												curr_event->advance_next_tics()	;
												event_set.insert(curr_event)	;
								}

								//	get	the	next	top	element	of	the	set.
								sit	=	event_set.begin()	;
				}

				if	(	!event_set.empty()	)	{
								//	set	the	next	call	time	to	the	first	item	of	the	set.
								process_event_job->next_tics	=	(*(event_set.begin()))->get_next_tics()	;
				}	else	{
								//	if	there	are	no	events,	set	the	next	job	call	time	to	max.
								process_event_job->next_tics	=	TRICK_MAX_LONG_LONG	;
				}

				return	0		;

}

void	Trick::EventProcessor::preload_checkpoint()	{
				event_set.clear()	;
}
