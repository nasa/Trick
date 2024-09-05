#include	<algorithm>
#include	<iostream>
#include	<sstream>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"trick/ScheduledJobQueue.hh"
#include	"trick/ScheduledJobQueueInstrument.hh"
#include	"trick/TrickConstant.hh"

/**
@design
-#	Set	#list	to	NULL
-#	Set	#list_list	to	0
-#	Set	#curr_index	to	0
-#	Set	#next_job_time	to	TRICK_MAX_LONG_LONG
*/
Trick::ScheduledJobQueue::ScheduledJobQueue(	)	{

				list	=	NULL	;
				list_size	=	0	;
				curr_index	=	0	;
				next_job_time	=	TRICK_MAX_LONG_LONG	;

}

/**
@design
-#	free	list	if	it	is	not	empty
*/
Trick::ScheduledJobQueue::~ScheduledJobQueue(	)	{
				if	(list		!=	NULL	)	{
								free(list)	;
				}
}

static	bool	compare_job_data(const	Trick::JobData	*a,	const	Trick::JobData	*b)	{
				{
								int	ajc	=	a->job_class;
								int	bjc	=	b->job_class;
								if	(ajc	<	bjc)
												return	true;
								if	(ajc	>	bjc)
												return	false;
				}
				{
								unsigned	short	ap	=	a->phase;
								unsigned	short	bp	=	b->phase;
								if	(ap	<	bp)
												return	true;
								if	(ap	>	bp)
												return	false;
				}
				{
								int	asoi	=	a->sim_object_id;
								int	bsoi	=	b->sim_object_id;
								if	(asoi	<	bsoi)
												return	true;
								if	(asoi	>	bsoi)
												return	false;
				}
				return	a->id	<	b->id;
}

/**
@design
-#	Allocate	additional	memory	for	the	incoming	job
-#	Find	the	insertion	point	in	the	queue	based	on	the	job_class,	the	phase,
			the	sim_object	id,	and	the	job_id
-#	Move	the	jobs	after	the	insertion	point	to	the	right	by	one.
-#	Insert	the	new	job	at	the	insertion	point.
-#	Increment	the	size	of	the	queue.
*/
int	Trick::ScheduledJobQueue::push(	JobData	*	new_job	)	{

				/*	Allocate	additional	memory	for	the	additional	job	in	the	queue	*/
				JobData	**	new_list	=	(JobData	**)realloc(list,	(list_size	+	1)	*	sizeof(JobData	*))	;
				if	(!new_list)	{
								abort();
				}
				list	=	new_list;
				JobData	**	list_end	=	list	+	list_size;
				JobData	**	insert_pt	=	std::upper_bound(list,	list_end,	new_job,	compare_job_data);
				if	(insert_pt	!=	list_end)	{
								memmove(insert_pt	+	1,	insert_pt,	(list_end	-	insert_pt)	*	sizeof(JobData	*));
				}
				*insert_pt	=	new_job;

				new_job->set_handled(true)	;

				/*	Increment	the	size	of	the	queue	*/
				list_size++	;

				return(0)	;

}

/**
@design
-#	Temporarily	assign	a	high	sim_object	id	to	the	incoming	job.		This
			effectively	removes	the	sim_object_id	as	an	ordering	field
-#	Call	Trick::ScheduledJobQueue::push(	JobData	*	)	to	add	the	job	to	the	queue.
-#	Restore	the	original	sim_object	id.
*/
int	Trick::ScheduledJobQueue::push_ignore_sim_object(	JobData	*	new_job	)	{

				int	save_sim_object_id	;
				int	ret	;

				/*	Temorarily	assign	a	really	high	sim_object	id		*/
				save_sim_object_id	=	new_job->sim_object_id	;
				new_job->sim_object_id	=	1000000	;
				/*	push	the	job	onto	the	scheduling	queue	as	normal	*/
				ret	=	push(new_job)	;
				/*	restore	the	original	sim_object	id	*/
				new_job->sim_object_id	=	save_sim_object_id	;
				return(ret)	;
}

/**
@design
-#	Traverse	the	list	of	jobs	looking	for	the	job	to	delete.
	-#	If	the	job	to	delete	is	found
		-#	Allocate	a	new	list	that	holds	1	less	job	than	the	current	list
		-#	Copy	all	of	the	jobs	that	precede	the	deleted	job	to	the	new	list
		-#	Copy	all	of	the	jobs	that	are	after	the	delete	job	to	the	new	list
		-#	Decrement	the	size	of	the	list
		-#	Free	the	memory	associated	with	the	current	list
		-#	Point	the	current	list	to	the	newly	allocated	list
*/
int	Trick::ScheduledJobQueue::remove(	JobData	*	delete_job	)	{

				unsigned	int	ii	,	jj	;

				/*	Find	the	job	to	delete	in	the	queue.	*/
				for	(	ii	=	0	;	ii	<	list_size	;	ii++	)	{
								if	(	list[ii]	==	delete_job	)	{
												/*	allocate	a	new	list	that	holds	one	less	element	than	the	current	list.	*/
												JobData	**	new_list	=	(JobData	**)calloc(	list_size	-	1	,	sizeof(JobData	*))	;
												/*	copy	all	of	the	jobs	that	are	before	the	deleted	job	to	the	new	list	*/
												for	(	jj	=	0	;	jj	<	ii	;	jj++	)	{
																new_list[jj]	=	list[jj]	;
												}
												/*	copy	all	of	the	jobs	that	are	after	the	deleted	job	to	the	new	list	*/
												for	(	jj	=	ii	+	1	;	jj	<	list_size	;	jj++	)	{
																new_list[jj-1]	=	list[jj]	;
												}
												if	(	ii	<=	curr_index	)	{
																curr_index--	;
												}
												/*	Decrement	the	size	of	the	queue	*/
												list_size--	;
												/*	Free	the	old	queue	space	*/
												free(list)	;
												/*	Assign	the	queue	pointer	to	the	new	space	*/
												list	=	new_list	;
												return	0	;
								}
				}
				return	-1	;
}

/**
@design
-#	Returns	the	#curr_index	of	the	queue.
*/
unsigned	int	Trick::ScheduledJobQueue::get_curr_index()	{
				return	curr_index	;
}

/**
@design
-#	Sets	#curr_index	to	the	incoming	value.
*/
int	Trick::ScheduledJobQueue::set_curr_index(unsigned	int	value	)	{

				if	(	value	<	list_size	)	{
								curr_index	=	value	;
				}
				return	0	;
}

/**
@design
-#	Sets	#curr_index	to	0.
*/
int	Trick::ScheduledJobQueue::reset_curr_index()	{

				curr_index	=	0	;
				return(0)	;
}

/**
@design
-#	Returns	#list_size
*/
unsigned	int	Trick::ScheduledJobQueue::size()	{
				return(list_size)	;
}

/**
@design
-#	Returns	!(#list_size)
*/
bool	Trick::ScheduledJobQueue::empty()	{
				return(!list_size)	;
}

/**
@design
-#	If	#list	is	not	NULL	free	it.
-#	Set	#list	to	NULL
-#	Set	#list_list	to	0
-#	Set	#curr_index	to	0
-#	Set	#next_job_time	to	TRICK_MAX_LONG_LONG
*/
int	Trick::ScheduledJobQueue::clear()	{

				/*	free	job	list	if	one	exists		*/
				if	(list	!=	NULL	)	{
								free(list)	;
				}
				/*	set	all	list	variables	to	initial	cleared	values	*/
				list	=	NULL	;
				list_size	=	0	;
				curr_index	=	0	;
				next_job_time	=	TRICK_MAX_LONG_LONG	;
				return(0)	;
}

/**
@design
-#	If	the	list	is	empty	return	NULL
-#	Else	return	the	current	job	without	incrementing	the	#curr_index	index.
*/
Trick::JobData	*	Trick::ScheduledJobQueue::top()	{
				/*	return	NULL	if	list	is	empty		*/
				if	(	list_size	==	0	)	{
								return(NULL)	;
				}
				/*	else	return	current	list	item		*/
				return(list[curr_index])	;
}


/**
@design
-#	If	the	#curr_index	is	greater	than	or	equal	to	the	#list_size
				-#	Set	the	#curr_index	to	the	#list_size
				-#	Return	NULL
-#	Else	while	the	list	#curr_list	is	less	than	the	list	size
				-#	Increment	the	#curr_index.
				-#	Return	the	current	job	if	the	job	is	enabled.
*/
Trick::JobData	*	Trick::ScheduledJobQueue::get_next_job()	{

				JobData	*	curr_job	;

				/*	return	NULL	if	we	are	at	the	end	of	the	list		*/
				if	(	curr_index	>=	list_size	)	{
								curr_index	=	list_size	;
								return(NULL)	;
				}	else	{
								/*	return	the	next	enabled	job,	or	NULL	if	we	reach	the	end	of	the	list	*/
								while	(curr_index	<	list_size	)	{
												curr_job	=	list[curr_index++]	;
												if	(	!curr_job->disabled	)	{
																return(curr_job)	;
												}
								}
				}
				return(NULL)	;
}

/**
@design
-#	While	the	list	#curr_list	is	less	than	the	list	size
				-#	If	the	current	queue	job	next	call	matches	the	incoming	simulation	time
								-#	If	the	job	class	is	not	a	system	class	job,	calculate	the	next
											time	it	will	be	called	by	current	time	+	job	cycle.
								-#	Set	the	next	job	call	time	to	TRICK_MAX_LONG_LONG	if	the	next	job	call	time
											is	greater	than	the	stop	time.
								-#	If	the	job's	next	job	call	time	is	lower	than	the	overall	next	job	call	time
											set	the	overall	job	call	time	to	the	current	job's	next	job	call	time.
								-#	Increment	the	#curr_index.
								-#	Return	the	current	job	if	the	job	is	enabled.
				-#	Else
								-#	If	the	job's	next	job	call	time	is	lower	than	the	overall	next	job	call	time
											set	the	overall	job	call	time	to	the	current	job's	next	job	call	time.
								-#	Increment	the	#curr_index.
-#	Return	NULL	when	the	end	of	the	list	is	reached.
*/
Trick::JobData	*	Trick::ScheduledJobQueue::find_next_job(long	long	time_tics	)	{

				JobData	*	curr_job	;
				long	long	next_call	;

				/*	Search	through	the	rest	of	the	queue	starting	at	curr_index	looking	for
							the	next	job	with	it's	next	execution	time	is	equal	to	the	current	simulation	time.	*/
				while	(curr_index	<	list_size	)	{

								curr_job	=	list[curr_index]	;

								if	(	curr_job->next_tics	==	time_tics	)	{

												/*	If	the	job	does	not	reschedule	itself	(system_job_classes),	calculate	the	next	time	it	will	be	called.	*/
												if	(	!	curr_job->system_job_class	)	{
																//	calculate	the	next	job	call	time
																next_call	=	curr_job->next_tics	+	curr_job->cycle_tics	;
																/*	If	the	next	time	does	not	exceed	the	stop	time,	set	the	next	call	time	for	the	module	*/
																if	(next_call	>	curr_job->stop_tics)	{
																				curr_job->next_tics	=	TRICK_MAX_LONG_LONG	;
																}	else	{
																				curr_job->next_tics	=	next_call;
																}
																/*	Track	next	lowest	job	call	time	after	the	current	time	for	jobs	that	match	the	current	time.	*/
																if	(	curr_job->next_tics	<		next_job_time	)	{
																				next_job_time	=	curr_job->next_tics	;
																}
												}
												curr_index++	;
												if	(	!curr_job->disabled	)	{
																return(curr_job)	;
												}
								}	else	{
												/*	Track	next	lowest	job	call	time	after	the	current	time	for	jobs	that	do	not	match	the	current	time	*/
												if	(	curr_job->next_tics	>	time_tics	&&	curr_job->next_tics	<	next_job_time	)	{
																next_job_time	=	curr_job->next_tics	;
												}
												curr_index++	;
								}
				}
				return(NULL)	;
}

/**
@design
-#	While	the	list	#curr_list	is	less	than	the	list	size
				-#	If	the	current	queue	job	next	call	matches	the	incoming	simulation	time
								-#	Increment	the	#curr_index.
								-#	Return	the	current	job	if	the	job	is	enabled.
				-#	Increment	the	#curr_index.
-#	Return	NULL	when	the	end	of	the	list	is	reached.
*/
Trick::JobData*	Trick::ScheduledJobQueue::find_job(long	long	time_tics)	{
				JobData	*	curr_job	;

				/*	Search	through	the	rest	of	the	queue	starting	at	curr_index	looking	for													*/
				/*	the	next	job	with	it's	next	execution	time	is	equal	to	the	current	simulation	time.	*/
				while	(curr_index	<	list_size)	{
								curr_job	=	list[curr_index]	;

								if	(curr_job->next_tics	==	time_tics	)	{
												if	(!curr_job->disabled)	{
																curr_index++	;
																return(curr_job)	;
												}
								}
								curr_index++	;
				}
				return(NULL)	;
}

/**
@details
-#	Sets	#next_job_time	to	the	incoming	time
*/
int	Trick::ScheduledJobQueue::set_next_job_call_time(long	long	in_time)	{
				next_job_time	=	in_time	;
				return(0)	;
}

/**
@details
-#	Return	the	next_job_call_time	in	counts	of	tics/second
			Requirement	[@ref	r_exec_time_0]
*/
long	long	Trick::ScheduledJobQueue::get_next_job_call_time()	{
				unsigned	int	temp_index	=	curr_index	;
				while	(temp_index	<	list_size	)	{
								if	(	list[temp_index]->next_tics	<		next_job_time	)	{
												next_job_time	=	list[temp_index]->next_tics	;
								}
								temp_index++	;
				}
				return(next_job_time)	;
}

/**
@details
-#	If	the	current	job	next	call	time	is	less	than	the	overall	next	job	call	time,	and	is
			greater	than	the	current	simulation	time,	set	the	overall	next	job	call	time	to	be	the
			job	next	call	time.
*/
int	Trick::ScheduledJobQueue::test_next_job_call_time(Trick::JobData	*	curr_job,	long	long	time_tics)	{
				if	(	curr_job->next_tics	>	time_tics	&&	curr_job->next_tics	<	next_job_time	)	{
								next_job_time	=	curr_job->next_tics	;
				}
				return(0)	;
}

//	Executes	the	jobs	in	a	queue.		saves	and	restores	Trick::Executive::curr_job
int	Trick::ScheduledJobQueue::execute_all_jobs()	{
				Trick::JobData	*	curr_job	;
				int	ret	;

				reset_curr_index()	;
				while	(	(curr_job	=	get_next_job())	!=	NULL	)	{
								ret	=	curr_job->call()	;
								if	(	ret	!=	0	)	{
												return	ret	;
								}
				}
				/*	return	0	if	there	are	no	errors.	*/
				return	0	;
}

int	Trick::ScheduledJobQueue::write_sched_queue(	FILE	*	fp	)	{

				Trick::JobData	*	curr_job	;
				unsigned	int	save_index	;

				save_index	=	get_curr_index()	;
				reset_curr_index()	;
				while	(	(curr_job	=	get_next_job())	!=	NULL	)	{
								if	(	curr_job->job_class_name.compare("instrumentation")	)	{
												/*	for	each	non	instrumentation	job	write	the	job	information	to	the	open	file	pointer	"fp"	*/
												fprintf(fp,	"%7d	|	%3d	|%-25s|	%-5d	|	%08.6f	|	%8.6g	|	%8g	|	%5.02f	|	%s\n",
																				!curr_job->disabled,	curr_job->thread,	curr_job->job_class_name.c_str(),	curr_job->phase,
																				curr_job->start,	curr_job->cycle,	curr_job->stop,	curr_job->frame_id,
																				curr_job->name.c_str());
								}
				}
				set_curr_index(save_index)	;
				return(0)	;
}

int	Trick::ScheduledJobQueue::write_non_sched_queue(	FILE	*	fp	)	{

				Trick::JobData	*	curr_job	;
				unsigned	int	save_index	;

				save_index	=	get_curr_index()	;
				reset_curr_index()	;
				while	(	(curr_job	=	get_next_job())	!=	NULL	)	{
								if	(	curr_job->job_class_name.compare("instrumentation")	)	{
												/*	for	each	non	instrumentation	job	write	the	job	information	to	the	open	file	pointer	"fp"	*/
												fprintf(fp,	"%7d	|	%3d	|%-25s|	%-5d	|										|										|										|	%5.02f	|	%s\n",
																				!curr_job->disabled,	curr_job->thread,	curr_job->job_class_name.c_str(),	curr_job->phase,
																				curr_job->frame_id,	curr_job->name.c_str());
								}
				}

				set_curr_index(save_index)	;
				return(0)	;
}

/**
@details
-#	For	all	jobs	in	the	queue
				-#	Create	a	new	ScheduledJobQueueInstrument	instance
				-#	Add	the	new	instrumentation	job	to	the	"before"	instrumentation	job	list
-#	Return	0
*/
int	Trick::ScheduledJobQueue::instrument_before(Trick::JobData	*	instrumentation_job)	{

				unsigned	int	ii	;
				ScheduledJobQueueInstrument	*	new_job	;

				for	(	ii	=	0	;	ii	<	list_size	;	ii++	)	{
								new_job	=	new	ScheduledJobQueueInstrument(	instrumentation_job,	list[ii]	);
								list[ii]->add_inst_before(new_job)	;
				}

				return	0	;
}

/**
@details
-#	For	all	jobs	in	the	queue
				-#	Create	a	new	ScheduledJobQueueInstrument	instance
				-#	Add	the	new	instrumentation	job	to	the	"after"	instrumentation	job	list
-#	Return	0
*/
int	Trick::ScheduledJobQueue::instrument_after(Trick::JobData	*	instrumentation_job)	{

				unsigned	int	ii	;
				ScheduledJobQueueInstrument	*	new_job	;

				/*	Count	the	number	of	non-instrumentation	jobs	in	the	current	queue.	*/
				for	(	ii	=	0	;	ii	<	list_size	;	ii++	)	{
								new_job	=	new	ScheduledJobQueueInstrument(	instrumentation_job,	list[ii]	);
								list[ii]->add_inst_after(new_job)	;
				}

				return	0	;
}

/**
@details
-#	For	all	jobs	in	the	queue
				-#	Create	a	new	ScheduledJobQueueInstrument	instance
				-#	Add	the	new	instrumentation	job	to	the	list
-#	Return	0
*/
int	Trick::ScheduledJobQueue::instrument_remove(std::string	job_name)	{

				unsigned	int	ii	;

				for	(	ii	=	0	;	ii	<	list_size	;	ii++	)	{
								list[ii]->remove_inst(job_name)	;
				}

				return	0	;
}
