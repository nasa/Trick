/*
			PURPOSE:	(	Python	input	processor	event	capability	)
			REFERENCE:	(	Trick	Simulation	Environment	)
			ASSUMPTIONS	AND	LIMITATIONS:	(	None	)
			CLASS:	(	N/A	)
			LIBRARY	DEPENDENCY:	(	None	)
			PROGRAMMERS:	(	Alex	Lin	NASA	2009	)
*/

#include	<iostream>
#include	<string>
#include	<stdlib.h>
#include	<string.h>

#include	"trick/IPPythonEvent.hh"
#include	"trick/IPPython.hh"
#include	"trick/MemoryManager.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/exec_proto.hh"

/*	Global	singleton	pointer	to	the	memory	manager	*/
//TODO	Use	external	MM	interface
extern	Trick::MemoryManager	*	trick_MM	;

/*	static	variables	in	the	IPPythonEvent	class	*/
Trick::IPPython	*	Trick::IPPythonEvent::ip	;
Trick::MTV	*	Trick::IPPythonEvent::mtv	;
bool	Trick::IPPythonEvent::info_msg	=	false	;
bool	Trick::IPPythonEvent::terminate_sim_on_event_python_error	=	false;

Trick::condition_t::condition_t()	{
				enabled	=	0	;
				hold	=	0	;
				fired	=	0	;
				fired_count	=	0	;
				fired_time	=	-1.0	;
				ref	=	NULL	;
				job	=	NULL	;
}

Trick::action_t::action_t()	{
				enabled	=	0	;
				ran	=	0	;
				ran_count	=	0	;
				ran_time	=	-1.0	;
				job	=	NULL	;
				act_type	=	0	;
}

//Constructor
Trick::IPPythonEvent::IPPythonEvent()	{


				is_user_event	=	true	;
				manual	=	false	;
				manual_fired	=	false	;
				condition_count	=	0	;
				cond_all	=	false	;
				action_count	=	0	;
				fired_count	=	0	;
				fired_time	=	-1.0	;
				ran_count	=	0	;
				ran_time	=	-1.0	;
				fired	=	false	;
				hold	=	false	;
				ran	=	false	;
				action_list	=	NULL;
				condition_list	=	NULL;
}

Trick::IPPythonEvent::~IPPythonEvent()	{

				if	(TMM_is_alloced((char	*)condition_list))
				{
							for	(int	ii=0;	ii<condition_count;	ii++)	{
											if	(TMM_is_alloced((char	*)condition_list[ii]))
											{
														TMM_delete_var_a(condition_list[ii]);
											}
											condition_list[ii]	=	0x0;
							}
							TMM_delete_var_a(condition_list);
				}
				condition_list	=	0x0;

				if	(TMM_is_alloced((char	*)action_list))
				{
							for	(int	ii=0;	ii<action_count;	ii++)	{
											if	(TMM_is_alloced((char	*)action_list[ii]))
											{
														TMM_delete_var_a(action_list[ii]);
											}
											action_list[ii]	=	0x0;
							}
							TMM_delete_var_a(action_list);
				}
				action_list	=	0x0;
}

void	Trick::IPPythonEvent::set_python_processor(Trick::IPPython	*	in_ip)	{
				ip	=	in_ip	;
}

void	Trick::IPPythonEvent::set_mtv(Trick::MTV	*	in_mtv)	{
				mtv	=	in_mtv	;
}

//Command	to	make	event	evaluation	require	that	ALL	of	event's	conditions	be	true	to	make	action(s)	run.
int	Trick::IPPythonEvent::condition_all()	{
				cond_all	=	true	;
				return(0);
}

//Command	to	make	event	evaluation	require	that	ANY	of	event's	conditions	be	true	to	make	action(s)	run	(default).
int	Trick::IPPythonEvent::condition_any()	{
				cond_all	=	false	;
				return(0);
}

//Command	to	manually	fire	the	event	next	cycle	and	hold	it	fired	(enter	manual	mode,	bypasses	normal	condition	processing).
void	Trick::IPPythonEvent::manual_on()	{
				manual	=	true	;
				manual_fired	=	true	;
				hold	=	true	;
				fired	=	false	;
}

//Command	to	manually	fire	the	event	once	NOW	(enter	manual	mode,	bypasses	normal	condition	processing).
void	Trick::IPPythonEvent::manual_fire()	{
				manual	=	true	;
				manual_fired	=	true	;
				hold	=	false	;
				fired	=	false	;
				process_user_event(exec_get_time_tics())	;
}

//Command	to	manually	set	the	event	as	not	fired	(enter	manual	mode,	bypasses	normal	condition	processing).
void	Trick::IPPythonEvent::manual_off()	{
				manual	=	true	;
				manual_fired	=	false	;
				hold	=	false	;
				fired	=	false	;
}

//Command	to	return	to	normal	event	processing	(needed	to	end	manual	mode	after	any	manual	commands).
void	Trick::IPPythonEvent::manual_done()	{
				manual	=	false	;
				manual_fired	=	false	;
				hold	=	false	;
}

void	Trick::IPPythonEvent::add()	{
				added	=	true	;
}

void	Trick::IPPythonEvent::remove()	{
				added	=	false	;
}

//	Command	to	turn	on	info	messages
void	Trick::IPPythonEvent::set_event_info_msg_on()	{
				info_msg	=	true;
}

//	Command	to	turn	off	info	messages
void	Trick::IPPythonEvent::set_event_info_msg_off()	{
				info_msg	=	false;
}

void	Trick::IPPythonEvent::terminate_on_event_parse_error(bool	on_off)	{
				terminate_sim_on_event_python_error	=	on_off;
}

void	Trick::IPPythonEvent::restart()	{
				int	jj	;

				for	(jj=0;	jj<condition_count;	jj++)	{
								if	(condition_list[jj]->cond_type==1)	{	//	condition	variable
												condition_list[jj]->ref	=	ref_attributes(condition_list[jj]->str.c_str());
								}
								if	(condition_list[jj]->cond_type==2)	{	//	condition	job
												condition_list[jj]->job	=	exec_get_job(condition_list[jj]->str.c_str(),1);
								}
				}
				for	(jj=0;	jj<action_count;	jj++)	{
								if	(action_list[jj]->act_type!=0)	{	//	action	job
												action_list[jj]->job	=	exec_get_job(action_list[jj]->str.c_str(),1);
								}
				}

}

//Command	to	create	a	new	condition	using	a	model	variable	(or	reset	an	existing	condition	variable),	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::condition_var(int	num,	std::string	varname,	std::string	comment)	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Find	the	variable	reference	for	the	varname	string	and	pass	it	to	condition()	*/
				REF2*	ref	=	ref_attributes(varname.c_str());
				/**	@li	Emit	an	error	if	specified	varname	does	not	exist.	*/
				if	(ref==NULL)	{
								message_publish(MSG_WARNING,	"Event	condition	variable	%s	not	found.	Setting	condition	to	False.\n",	varname.c_str())	;
								condition(num,	"False",	comment,	NULL,	NULL	);
				}	else	{
								condition(num,	varname,	comment,	ref,	NULL	);
				}
				return(0);
}

//Command	to	create	a	new	condition	using	a	model	job	(or	reset	an	existing	condition	job),	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::condition_job(int	num,	std::string	jobname,	std::string	comment)	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Find	the	job	for	the	jobname	string	and	pass	it	to	condition()	*/
				JobData	*job	=	exec_get_job(jobname.c_str(),1);
				/**	@li	Emit	an	error	if	specified	jobname	does	not	exist.	*/
				if	(job==NULL)	{
								message_publish(MSG_WARNING,	"Event	condition	job	%s	not	found.	Setting	condition	to	False.\n",	jobname.c_str())	;
								condition(num,	"False",	comment,	NULL,	NULL	);
				}	else	{
								if	(!	job->job_class_name.compare("malfunction"))	{
												//	enable	it	if	it's	a	malf	job	because	they	are	not	in	any	queue
												job->disabled	=	false;
								}
								condition(num,	jobname,	comment,	NULL,	job	);
				}
				return(0);
}

//Command	to	create	a	new	condition	and	set	its	input	string	(or	reset	an	existing	condition	string),	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::condition(int	num,	std::string	str,	std::string	comment,	REF2*	ref,	JobData*	job)	{
				/**	@par	Detailed	Design:	*/

				if	(num	==	condition_count)	{
								/**	@li	Add	a	new	condition	when	num	is	sequential,	i.e.	it	is	equal	to	condition_count	*/
								condition_count++;
								if	(condition_count	==	1)	{
												condition_list	=
																(Trick::condition_t	**)TMM_declare_var_s("Trick::condition_t*[1]");
								}	else	{
												condition_list	=
																(Trick::condition_t	**)TMM_resize_array_1d_a(condition_list,	condition_count);
								}
								condition_list[num]	=
												(Trick::condition_t	*)TMM_declare_var_s("Trick::condition_t");
									
								condition_list[num]->fired_count	=	0;
								condition_list[num]->fired_time	=	-1.0;
				}
				if	((num	>=0)	&&	(num	<	condition_count))	{
								/**	@li	This	is	either	a	new	condition	or	user	is	changing	the	condition.	*/
								/**	@li	Initialize	condition	variables	-	default	as	enabled.	*/
								condition_list[num]->ref	=	ref	;
								condition_list[num]->job	=	job	;
								condition_list[num]->enabled	=	true;
								condition_list[num]->hold	=	false;
								condition_list[num]->fired	=	false;
								if	(ref	!=	NULL)	{
												condition_list[num]->cond_type	=	1;
								}	else	if	(job	!=	NULL)	{
												condition_list[num]->cond_type	=	2;
								}	else	condition_list[num]->cond_type	=	0;
								condition_list[num]->str	=	str;
								//	comment	is	for	display	in	mtv,	if	not	supplied	create	a	comment	containing	up	to	50	characters	of	cond	string
								if	(comment.empty())	{
												condition_list[num]->comment	=	str.substr(0,50);
								}	else	{
												condition_list[num]->comment	=	comment;
								}
								//	dummy	must	contain	max	conditions	used	in	any	event	so	it	can	replace	any	event	in	mtv	when	deleted
				}	else	{
								/**	@li	Emit	an	error	if	specified	index	num	is	not	sequential.	*/
								message_publish(MSG_WARNING,	"Event	condition	not	added:	condition	number	%d	is	not	sequential.\n",	num)	;
				}
				return(0);
}

//Command	to	set	an	existing	condition	to	hold,	so	that	when	it	fires	it	stays	in	the	fired	state.
int	Trick::IPPythonEvent::condition_hold_on(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								condition_list[num]->hold	=	true	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	hold	not	set.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(0);
}

//Command	to	set	an	existing	condition	to	not	hold	its	fired	state.
int	Trick::IPPythonEvent::condition_hold_off(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								condition_list[num]->hold	=	false	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	hold	not	set.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(0);
}

//Command	to	enable	an	existing	condition	(default	is	enabled)	so	that	it	is	evaluated	during	event	processing.
int	Trick::IPPythonEvent::condition_enable(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								condition_list[num]->enabled	=	true	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	not	enabled.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(0);
}

//Command	to	disable	an	existing	condition	so	that	it	is	not	evaluated	during	event	processing.
int	Trick::IPPythonEvent::condition_disable(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								condition_list[num]->enabled	=	false	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	not	disabled.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(0);
}

//Accessor	function	to	test	if	the	condition,	indicated	by	num,	fired	(is	currently	in	the	fired	state).
bool	Trick::IPPythonEvent::condition_fired(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								return	(condition_list[num]->fired	?	true	:	false)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	fired	state	not	returned.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(false);
}

//Accessor	function	to	see	how	many	times	the	condition,	indicated	by	num,	fired.
int	Trick::IPPythonEvent::condition_fired_count(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								return	(condition_list[num]->fired_count)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	fired	count	not	returned.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(0)	;
}

//Accessor	function	to	see	when	was	the	last	time	the	condition,	indicated	by	num,	fired.
double	Trick::IPPythonEvent::condition_fired_time(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								return	(condition_list[num]->fired_time)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	fired	time	not	returned.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return(0.0)	;
}

//Accessor	function	to	return	the	condition	string,	indicated	by	num.
std::string	Trick::IPPythonEvent::condition_string(int	num)	{

				if	((num	>=0)	&&	(num	<	condition_count))	{
								return	(condition_list[num]->str)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	condition	string	not	returned.	Condition	number	%d	is	invalid.\n",	num)	;
				}
				return	""	;
}

//Command	to	create	a	new	action	using	a	model	job	(or	reset	an	existing	action	job),	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::action_job(int	num,	std::string	jobname,	std::string	comment)	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Find	the	job	for	the	jobname	string	and	pass	it	to	action()	*/
				JobData	*job	=	exec_get_job(jobname.c_str(),1);
				/**	@li	Emit	an	error	if	specified	jobname	does	not	exist.	*/
				if	(job==NULL)	{
								message_publish(MSG_WARNING,	"Event	action	job	%s	not	found.	No	action	was	added.\n",	jobname.c_str())	;
				}	else	{
								if	(!	job->job_class_name.compare("malfunction"))	{
												//	enable	it	if	it's	a	malf	job	because	they	are	not	in	any	queue
												job->disabled	=	false;
								}
								action(num,	jobname,	comment,	job,	3	);
				}
				return(0);
}

//Command	to	create	a	new	action	to	turn	a	model	job	ON,	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::action_job_on(int	num,	std::string	jobname,	std::string	comment)	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Find	the	job	for	the	jobname	string	and	pass	it	to	action()	*/
				JobData	*job	=	exec_get_job(jobname.c_str(),1);
				/**	@li	Emit	an	error	if	specified	jobname	does	not	exist.	*/
				if	(job==NULL)	{
								message_publish(MSG_WARNING,	"Event	action	job	%s	not	found.	No	action	was	added.\n",	jobname.c_str())	;
				}	else	{
								action(num,	jobname,	comment,	job,	1	);
				}
				return(0);
}

//Command	to	create	a	new	action	to	turn	a	model	job	OFF,	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::action_job_off(int	num,	std::string	jobname,	std::string	comment)	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Find	the	job	for	the	jobname	string	and	pass	it	to	action()	*/
				JobData	*job	=	exec_get_job(jobname.c_str(),1);
				/**	@li	Emit	an	error	if	specified	jobname	does	not	exist.	*/
				if	(job==NULL)	{
								message_publish(MSG_WARNING,	"Event	action	job	%s	not	found.	No	action	was	added.\n",	jobname.c_str())	;
				}	else	{
								action(num,	jobname,	comment,	job,	2	);
				}
				return(0);
}

//Command	to	create	a	new	action	and	set	its	input	string	(or	reset	an	existing	action	string),	num	is	index	starting	at	0.
int	Trick::IPPythonEvent::action(int	num,	std::string	str,	std::string	comment,	JobData	*job,	int	act_type)	{
				/**	@par	Detailed	Design:	*/

				if	(num	==	action_count)	{
								/**	@li	Add	a	new	action	when	num	is	sequential,	i.e.	it	is	equal	to	action_count	*/
								action_count++;
								if	(action_count	==	1)	{
												action_list	=	(Trick::action_t	**)TMM_declare_var_s("Trick::action_t*[1]");
								}	else	{
												action_list	=	(Trick::action_t	**)TMM_resize_array_1d_a(action_list,	action_count);
								}
								action_list[num]	=
												(Trick::action_t	*)TMM_declare_var_s("Trick::action_t");

								action_list[num]->ran_count	=	0;
								action_list[num]->ran_time	=	-1.0;
				}
				if	((num	>=0)	&&	(num	<	action_count))	{
								/**	@li	This	is	either	a	new	action	or	user	is	changing	the	action.	*/
								/**	@li	Initialize	action	variables	-	default	as	enabled.	*/
								action_list[num]->job	=	job	;
								action_list[num]->act_type	=	act_type	;
								action_list[num]->enabled	=	true;
								action_list[num]->ran	=	false;
								action_list[num]->str	=	str;
								//	comment	is	for	display	in	mtv,	if	not	supplied	create	a	comment	containing	up	to	50	characters	of	act	string
								if	(comment.empty())	{
												action_list[num]->comment	=	str.substr(0,50);
								}	else	{
												action_list[num]->comment	=	comment;
								}
								//	dummy	must	contain	max	actions	used	in	any	event	so	it	can	replace	any	event	in	mtv	when	deleted
#if	0
								if	(num	==	ip->dummy_event.action_count)	{
												ip->dummy_event.action(num,	"XXX_DELETED_ACT");
								}
#endif
				}	else	{
								/**	@li	Emit	an	error	if	specified	index	num	is	not	sequential.	*/
								message_publish(MSG_WARNING,	"Event	action	not	added:	action	number	%d	is	not	sequential.\n",	num)	;
				}
				return(0);
}

//Command	to	enable	an	existing	action	(default	is	enabled)	so	that	it	is	run	when	fired	during	event	processing.
int	Trick::IPPythonEvent::action_enable(int	num)	{

				if	((num	>=0)	&&	(num	<	action_count))	{
								action_list[num]->enabled	=	true	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	action	not	enabled.	Action	number	%d	is	invalid.\n",	num)	;
				}
				return(0);
}

//Command	to	disable	an	existing	action	so	that	it	is	not	run	when	fired	during	event	processing.
int	Trick::IPPythonEvent::action_disable(int	num)	{

				if	((num	>=0)	&&	(num	<	action_count))	{
								action_list[num]->enabled	=	false	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	action	not	disabled.	Action	number	%d	is	invalid.\n",	num)	;
				}
				return(0);
}

//Accessor	function	to	test	if	the	action,	indicated	by	num,	ran.
bool	Trick::IPPythonEvent::action_ran(int	num)	{

				if	((num	>=0)	&&	(num	<	action_count))	{
								return	(action_list[num]->ran	?	true	:	false)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	action	ran	state	not	returned.	Action	number	%d	is	invalid.\n",	num)	;
				}
				return(false);
}

//Accessor	function	to	see	how	many	times	the	action,	indicated	by	num,	ran.
int	Trick::IPPythonEvent::action_ran_count(int	num)	{

				if	((num	>=0)	&&	(num	<	action_count))	{
								return	(action_list[num]->ran_count)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	action	ran	count	not	returned.	Action	number	%d	is	invalid.\n",	num)	;
				}
				return(0)	;
}

//Accessor	function	to	see	when	was	the	last	time	the	action,	indicated	by	num,	ran.
double	Trick::IPPythonEvent::action_ran_time(int	num)	{

				if	((num	>=0)	&&	(num	<	action_count))	{
								return	(action_list[num]->ran_time)	;
				}	else	{
								message_publish(MSG_WARNING,	"Event	action	ran	time	not	returned.	Action	number	%d	is	invalid.\n",	num)	;
				}
				return(0.0)	;
}

int	Trick::IPPythonEvent::process(	long	long	curr_time	)	{

				if	(	active	||	manual_fired	)	{
												/**	@li	If	it's	a	user's	input	event,	process	it	by	calling	Trick::IPPython::process_triggers.	*/
												if	(	is_user_event	)	{
																//	it's	an	input	file	event
																process_user_event(curr_time)	;
												/**	@li	Otherwise	if	it's	a	trick	event	(like	a	read),	process	it.	*/
												}	else	{
																//	it's	a	read	event
																active	=	false	;
																int	ret	=	ip->parse(action_list[0]->str)	;
																if	(ret	!=	0	&&	terminate_sim_on_event_python_error)	{
																				exec_terminate_with_return(	ret	,	__FILE__	,	__LINE__	,	"Python	error	in	event	processing"	)	;
																}
																//	keep	stats	so	mtv	will	show	when	it	ran
																fired_count++	;
																fired_time	=	curr_time	;
																ran	=	true	;
																ran_count++	;
																ran_time	=	curr_time	;
																action_list[0]->ran	=	true	;
																action_list[0]->ran_count++	;
																action_list[0]->ran_time	=	curr_time	;
												}
				}
				return	0	;
}

bool	Trick::IPPythonEvent::process_user_event(	long	long	curr_time	)	{

				int	ii	;
				int	return_val	;
				bool	it_fired,	it_ran;

				fired	=	false	;
				ran	=	false	;
				/**	@li	No	need	to	evaluate	any	conditions	if	in	manual	mode.	*/
				if	(!	manual)	{
								hold	=	false	;
								/**	@li	Loop	thru	all	conditions.	*/
								for	(ii=0;	ii<condition_count;	ii++)	{
												/**	@li	Skip	condition	if	it's	been	disabled.	*/
												if	(!	condition_list[ii]->enabled	)	{
																condition_list[ii]->fired	=	false	;
																continue	;
												}
												/**	@li	No	need	to	evaluate	condition	if	previously	fired	and	hold	is	on.	*/
												if	(condition_list[ii]->hold	&&	condition_list[ii]->fired)	{
																;
												}	else	{
																/**	@li	Evaluate	the	condition	and	set	its	fired	state.	*/
																condition_list[ii]->fired	=	false	;
																return_val	=	0	;
																if	(condition_list[ii]->ref	!=	NULL)	{
																//	if	it's	a	variable,	get	it	as	a	boolean
																				if	(	condition_list[ii]->ref->pointer_present	)	{
																								condition_list[ii]->ref->address	=	follow_address_path(condition_list[ii]->ref)	;
																				}
																				if	(	condition_list[ii]->ref->address	!=	NULL	)	{
																								return_val	=	*(bool	*)condition_list[ii]->ref->address	;
																				}
																}	else	if	(condition_list[ii]->job	!=	NULL)	{
																//	if	it's	a	job,	get	its	return	value
																				bool	save_disabled_state	=	condition_list[ii]->job->disabled;
																				condition_list[ii]->job->disabled	=	false;
																				return_val	=	condition_list[ii]->job->call();
																				condition_list[ii]->job->disabled	=	save_disabled_state;
																}	else	{
																//	otherwise	use	python	to	evaluate	string
																				std::string	full_in_string	;
																				int	python_ret	=	ip->parse_condition(condition_list[ii]->str,	return_val)	;
																				if	(python_ret	!=	0	&&	terminate_sim_on_event_python_error)	{
																								exec_terminate_with_return(	python_ret	,	__FILE__	,	__LINE__	,	"Python	error	in	event	condition	processing"	)	;
																				}
																}
																if	(return_val)	{
																//TODO:	write	to	log/send_hs	that	trigger	fired
																				condition_list[ii]->fired	=	true	;
																				condition_list[ii]->fired_count++	;
																				condition_list[ii]->fired_time	=	curr_time	;
																}
												}	//	end	evaluate	condition
												/**	@li	If	cond_all	is	true,	only	set	event	fired/hold	after	all	enabled	conditions	evaluated.	*/
												if	(ii==0)	{
																fired	=	condition_list[ii]->fired	;
																hold		=	condition_list[ii]->hold	;
												}		else	{
																if	(cond_all)	{
																				fired	&=	condition_list[ii]->fired	;
																				hold		&=	condition_list[ii]->hold	;
																}	else	{
																				fired	|=	condition_list[ii]->fired	;
																				hold		|=	condition_list[ii]->hold	;
																}
												}
								}	//end	condition	loop
				}
				it_fired	=	manual_fired	||	fired	;
				/**	@li	Set	the	event's	fired	state...cond_all:	if	all	conditions	fired	,	otherwise	if	any	condition	fired.	*/
				if	(it_fired)	{
								fired_count++	;
								fired_time	=	curr_time	;
								if	(info_msg)	{
												message_publish(	MSG_INFO	,	"%12.6f	Event	%s	fired.\n"	,	exec_get_sim_time()	,	name.c_str())	;
								}
								if	(!manual_fired)	{
												active	=	false	;
								}
				}

				/**	@li	If	the	user	specified	any	actions,	run	them	here	if	a	condition	fired	or	manually	fired.	*/
				it_ran	=	false	;
				if	(it_fired	&&	(action_count	>	0))	{
								/**	@li	Loop	thru	all	actions.	*/
								for	(ii=0;	ii<action_count;	ii++)	{
												/**	@li	No	need	to	run	action	if	it's	been	disabled.	*/
												if	(!	action_list[ii]->enabled	)	{
																action_list[ii]->ran	=	false	;
																continue	;
												}
												/**	@li	Run	the	action	and	set	its	ran	state.	*/
												if	(action_list[ii]->job	!=	NULL)	{
												//	if	it's	a	job,	do	what	the	action	type	tells	you
																switch	(action_list[ii]->act_type)	{
																				case	0	:	//	python,	should	not	get	here
																								break;
																				case	1	:	//	On
																								action_list[ii]->job->disabled	=	false;
																								break;
																				case	2	:	//	Off
																								action_list[ii]->job->disabled	=	true;
																								break;
																				case	3	:	//	Call
																								bool	save_disabled_state	=	action_list[ii]->job->disabled;
																								action_list[ii]->job->disabled	=	false;
																								action_list[ii]->job->call();
																								action_list[ii]->job->disabled	=	save_disabled_state;
																								break;
																}
												}	else	{
																//	otherwise	use	python	to	evaluate	string
																int	ret	=	ip->parse(action_list[ii]->str)	;
																if	(ret	!=	0	&&	terminate_sim_on_event_python_error)	{
																				exec_terminate_with_return(	ret	,	__FILE__	,	__LINE__	,	"Python	error	in	event	action	processing"	)	;
																}
												}
												it_ran	=	true	;
												action_list[ii]->ran	=	true	;
												action_list[ii]->ran_count++	;
												action_list[ii]->ran_time	=	curr_time	;
												ran	=	true	;
								}
								/**	@li	Leave	event	fired	state	on	if	hold	is	on.	*/
								manual_fired	&=	hold	;
								fired	&=	hold	;
				}
				/**	@li	Set	the	event's	ran	state	if	any	actions	were	run.	*/
				if	(it_ran)	{
								ran_count++	;
								ran_time	=	curr_time	;
				}

				/**	@li	Return	true	if	the	event	fired.	*/
				return(it_fired)	;
}

