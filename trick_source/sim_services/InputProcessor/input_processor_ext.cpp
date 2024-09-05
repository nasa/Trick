
#include	<iostream>
#include	<sstream>
#include	<cmath>

#include	"trick/input_processor_proto.h"
#include	"trick/IPPython.hh"
#include	"trick/IPPythonEvent.hh"
#include	"trick/MTV.hh"
#include	"trick/EventManager_c_intf.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/exec_proto.h"

extern	Trick::InputProcessor	*	the_ip	;

extern	"C"	int	ip_parse(const	char	*	in_string)	{
				return	the_ip->parse(in_string)	;
}

/**
@details
Command	to	create	new	user	event,	the	given	name	will	be	used	for	display	in	mtv.
This	command	is	tied	to	"trick.new_event(event_name)"	in	the	input	processor.

-#	Allocate	space	for	the	event.
*/
Trick::IPPythonEvent	*	ippython_new_event(std::string	event_name)	{
				std::ostringstream	oss	;
				//	Allocate	space	for	the	event.
				oss	<<	"Trick::IPPythonEvent	"	<<	event_name	<<	"[1]"	;
				Trick::IPPythonEvent	*	event	=	(Trick::IPPythonEvent*)	TMM_declare_var_s(oss.str().c_str());
				if	(event	!=	NULL)	{
								event->set_name(event_name)	;
				}

				return	(event);
}

/**
@details
Command	to	add	an	event	to	be	processed	by	an	event	processor.

-#	Add	the	event	to	the	event	manager.
-#	Add	the	event	to	the	mtv	list.
*/
int	ippython_add_event(	Trick::IPPythonEvent	*	in_event	)	{
				event_manager_add_event(in_event)	;
				mtv_add_event(in_event)	;
				return	0	;
}

/**
@details
Command	to	add	an	event	to	be	processed	immediately	before	a	target	job.

-#	Add	the	event	to	the	event	manager.
-#	Add	the	event	to	the	mtv	list.
*/
int	ippython_add_event_before(	Trick::IPPythonEvent	*	in_event,	std::string	target_name,	unsigned	int	target_inst	)	{
				event_manager_add_event_before(in_event,	target_name,	target_inst)	;
				mtv_add_event(in_event)	;
				return	0	;
}

/**
@details
Command	to	add	an	event	to	be	processed	immediately	after	a	target	job.

-#	Add	the	event	to	the	event	manager.
-#	Add	the	event	to	the	mtv	list.
*/
int	ippython_add_event_after(	Trick::IPPythonEvent	*	in_event,	std::string	target_name,	unsigned	int	target_inst	)	{
				event_manager_add_event_after(in_event,	target_name,	target_inst)	;
				mtv_add_event(in_event)	;
				return	0	;
}

/**
@details
Command	to	create	a	new	read	event.
This	command	is	tied	to	"trick.add_read(thread_id,	time,	event_text)"	in	the	input	processor.

-#	Allocate	space	for	the	event.
-#	Set	the	event's	action	to	the	incoming	event	string.
-#	Set	the	event	as	not	a	user	event.
-#	Set	the	event	cycle	to	0
-#	Activate	the	event
-#	Set	the	event	thread	to	the	incoming	thread_id
-#	Add	the	event	to	the	event	manager
*/
int	ippython_add_read(	unsigned	int	thread_id	,	double	in_time	,	char	*	in_string	)	{

				Trick::IPPythonEvent	*	event	=	ippython_new_event();

				event->action(0,	in_string)	;
				event->is_user_event	=	false	;

				event->set_cycle(0)	;
				event->activate()	;
				event->set_thread(thread_id)	;
				event->set_next_tics((long	long)round(in_time	*	exec_get_time_tic_value()))	;

				event_manager_add_event(event)	;
				return	0	;
}

/**
@details
Command	to	create	a	new	read	event.		This	variant	is	the	backwards	compatible	2	argument
version	where	a	thread_id	is	not	specified.		All	events	of	this	type	will	be	run
on	the	main	thread.

-#	Call	ippython_add_read	with	thread_id	=	0.
*/
int	ippython_add_read(	double	in_time	,	char	*	in_string	)	{
				return	ippython_add_read(	0	,	in_time	,	in_string	)	;
}

/**
@details
Command	to	completely	delete	an	event.

-#	Remove	the	event	from	the	event	manager.
-#	Remove	the	event	from	the	mtv	list.
*/
int	ippython_delete_event(Trick::IPPythonEvent	*	in_event)	{
				event_manager_remove_event(in_event)	;
				mtv_delete_event(in_event)	;
				return	0	;
}

/**
@details
-#	If	the	event	exists,	call	manual_on	for	the	event.
*/
int	ippython_manual_on(std::string	event_name)	{
				Trick::IPPythonEvent	*	event	=	mtv_get_event(event_name)	;
				if	(	event	!=	NULL	)	{
								event->manual_on()	;
				}
				return	0	;
}

/**
@details
-#	If	the	event	exists,	call	manual_fire	for	the	event.
*/
int	ippython_manual_fire(std::string	event_name)	{
				Trick::IPPythonEvent	*	event	=	mtv_get_event(event_name)	;
				if	(	event	!=	NULL	)	{
								event->manual_fire()	;
				}
				return	0	;
}

/**
@details
-#	If	the	event	exists,	call	manual_off	for	the	event.
*/
int	ippython_manual_off(std::string	event_name)	{
				Trick::IPPythonEvent	*	event	=	mtv_get_event(event_name)	;
				if	(	event	!=	NULL	)	{
								event->manual_off()	;
				}
				return	0	;
}

/**
@details
-#	If	the	event	exists,	call	manual_done	for	the	event.
*/
int	ippython_manual_done(std::string	event_name)	{
				Trick::IPPythonEvent	*	event	=	mtv_get_event(event_name)	;
				if	(	event	!=	NULL	)	{
								event->manual_done()	;
				}
				return	0	;
}
