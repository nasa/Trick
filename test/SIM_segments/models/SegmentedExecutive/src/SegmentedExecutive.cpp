
#include	<iostream>
#include	<algorithm>
#include	<stdio.h>
#include	<sys/stat.h>

#include	"SegmentedExecutive.hh"
#include	"sim_services/Message/include/message_proto.h"
#include	"sim_services/CommandLineArguments/include/command_line_protos.h"

Trick::SegmentedExecutive	*	the_phe	=	NULL	;

Trick::SegmentedExecutive::SegmentedExecutive()	:	curr_segment(NULL)	{
				//	Set	the	global	the_phe	pointer	for	use	in	C	bound	access	functions
				the_phe	=	this	;
}

int	Trick::SegmentedExecutive::add_segment(	Trick::Segment	*	ps	)	{

				//	Gets	the	job	class	to	job	queue	map	for	this	segment	and	adds	the	job	classes	to	the	executive.
				std::map	<	std::string	,	Trick::ScheduledJobQueue	*	>::iterator	it	;
				for	(	it	=	ps->get_job_queues().begin()	;	it	!=	ps->get_job_queues().end()	;	++it	)	{
								class_map[(*it).first]	=	num_classes	;
								class_to_queue[num_classes++]	=	(*it).second	;
				}

				//	Store	the	segment	information	in	a	map	keyed	by	the	segment	name
				segment_map[ps->get_segment_name()]	=	ps	;

				return	0	;
}

/**
@details
		Finds	all	jobs	that	have	1	or	more	job	tags	that	are	a	segment	string.
		Those	jobs	are	saved	to	a	set.
*/
int	Trick::SegmentedExecutive::gather_segmented_jobs()	{
				Trick::JobData	*	job	;
				unsigned	int	ii	;
				std::map	<	std::string	,	Trick::Segment	*	>::iterator	mit	;
				std::set<	std::string	>	segment_tags	;

				for	(	mit	=	segment_map.begin()	;	mit	!=	segment_map.end()	;	++mit	)	{
								segment_tags.insert((*mit).first)	;
				}

				//	all_jobs_vector	is	defined	in	the	Trick::Executive.
				for	(	ii	=	0	;	ii	<	all_jobs_vector.size()	;	ii++	)	{
								job	=	all_jobs_vector[ii]	;
								/*	Find	the	set	intersection	between	the	known	segments	and	the	current	job	tags.	*/
								std::set<	std::string	>	intersect	;
								std::set_intersection(	segment_tags.begin()	,	segment_tags.end()	,	job->tags_begin()	,	job->tags_end()	,
														std::inserter(	intersect,	intersect.begin()	)	)	;
								/*	If	the	set	intersection	is	not	empty	we	have	a	segment.		Add	this	job	to	our	set	of	segmented	jobs.	*/
								if	(	!	intersect.empty()	)	{
												all_segmented_jobs.insert(job)	;
								}
				}

				return	0	;
}

/**
@details
Tests	if	the	incoming	segment	string	is	a	valid	segment	name.
If	the	name	is	valid,	save	the	next	segment	for	the	next	call	of	segment_switch
*/
int	Trick::SegmentedExecutive::segment_set(	std::string	in_segment	)	{
				//	Check	to	see	if	this	segment	string	exists.
				if	(	segment_map.find(in_segment)	!=	segment_map.end()	)	{
								next_segment_str	=	in_segment	;
				}	else	{
								message_publish(	MSG_ERROR	,	"Bad	Segment	Name:	%s\n"	,	in_segment.c_str()	)	;
								return	-1	;
				}
				return	0	;
}

/**
@details
Performs	segment	switch.
-#	If	there	is	a	segment	change
				-#	If	a	current	segment	is	set.
								-#	Call	the	currrent	segment	exit	routine
								-#	Call	the	next	segment	enter	routine
				-#	else
								-#	Call	the	next	segment	enter	routine	with	an	empty	string	as	the	current	segment
				-#	Turn	on/off	all	segmented	jobs.
				-#	Set	the	current	segment	to	the	next	segment.
*/
int	Trick::SegmentedExecutive::segment_switch()	{

				Trick::Segment	*	next_segment	=	segment_map[next_segment_str]	;

				//	Only	do	something	if	the	next_segment	is	not	equal	to	the	current	segment.
				if	(	curr_segment	!=	next_segment	)	{
								if	(	curr_segment	!=	NULL	)	{
												//	Perform	current	segment	exit	routine.
												if	(	curr_segment->get_segment_name().compare(next_segment_str)	)	{
																message_publish(	MSG_INFO	,	"Switching	from	segment	%s	to	%s\n"	,
																	curr_segment->get_segment_name().c_str()	,	next_segment_str.c_str()	)	;
																curr_segment->exit(	next_segment_str	)	;
												}
												//	Perform	next	segment	enter	routines
												next_segment->enter(	curr_segment->get_segment_name()	)	;
								}	else	{
												//	If	curr_segment	is	NULL,	then	this	is	the	initial	segment	set.
												//	No	need	to	exit	current	segment.
												message_publish(	MSG_INFO	,	"Setting	initial	segment	to	%s\n"	,	next_segment_str.c_str()	)	;
												//	Enter	the	next	segment	with	the	prev_segment	set	as	an	empty	string.
												next_segment->enter(	std::string("")	)	;
								}

								//	Set	the	jobs	to	on/off	according	to	segment
								segment_set_jobs_onoff()	;
								curr_segment	=	next_segment	;
				}

				return	0	;
}

/**
@details
Traverses	all	jobs	that	have	been	marked	as	having	a	segment	tag.
If	the	job	contains	the	next	segment	tag,	enable	the	job,	else	disable	it.
*/
int	Trick::SegmentedExecutive::segment_set_jobs_onoff()	{
				Trick::JobData	*	job	;
				std::set	<	Trick::JobData	*	>::iterator	it	;

				for	(	it	=	all_segmented_jobs.begin()	;	it	!=	all_segmented_jobs.end()	;	++it	)	{
								job	=	*it	;
								//	Test	to	see	if	the	next	segment	is	present	in	the	job	tags.
								//	Set	the	disabled	flag	to	the	negation	of	the	tag's	presence
								job->disabled	=	!(job->tags.count(next_segment_str))	;
								message_publish(MSG_NORMAL,	"segment	set	job	%s	to	%s\n"	,	job->name.c_str()	,	job->disabled	?	"disabled"	:	"enabled"	)	;
				}

				return	0	;
}

/*	The	following	routines	add	the	segmented	jobs	to	the	S_job_execution	file	and
			instruments	the	segmented	jobs	just	like	ant	other	job.
*/

int	Trick::SegmentedExecutive::write_s_job_execution(FILE	*fp)	{

				char	buf[1024];
				std::string	output_dir	;

				//	call	the	base	Executive	function
				Trick::Executive::write_s_job_execution(fp)	;

				/*	Get	full	path	to	S_job_execution	*/
				output_dir	=	command_line_args_get_output_dir()	;
				snprintf(buf,	sizeof(buf),	"%s/S_job_execution",	output_dir.c_str());

				/*	Reopen	the	S_job_execution	file.		If	it	fails,	it's	not	a	fatal	error,	return	0.	*/
				if	((fp	=	fopen(buf,	"a"))	==	NULL)	{
								return	(0)	;
				}

				fprintf(fp,	"Segment	Jobs:\n")	;

				std::map	<	std::string	,	Trick::Segment	*	>::iterator	pmit	;
				for	(	pmit	=	segment_map.begin()	;	pmit	!=	segment_map.end()	;	++pmit	)	{
								Trick::Segment	*	pb	=	(*pmit).second	;
								std::map	<	std::string	,	Trick::ScheduledJobQueue	*	>::iterator	jqit	;
								for	(	jqit	=	pb->get_job_queues().begin()	;	jqit	!=	pb->get_job_queues().end()	;	++jqit	)	{
												(*jqit).second->write_non_sched_queue(fp)	;
								}
				}
				fprintf(fp,	"\n===================================================================================================\n")	;
				fclose(fp)	;

				return	0	;
}

int	Trick::SegmentedExecutive::instrument_job_before(	Trick::JobData	*	instrument_job	)	{

				unsigned	int	count	=	0	;

				if	(	instrument_job	!=	NULL	)	{
								//call	the	base	Executive	instrument	job
								count	=	Trick::Executive::instrument_job_before(	instrument_job	)	;

								//go	through	all	of	the	segmented	jobs	and	instrument	them.
								std::map	<	std::string	,	Trick::Segment	*	>::iterator	pmit	;
								for	(	pmit	=	segment_map.begin()	;	pmit	!=	segment_map.end()	;	++pmit	)	{
												Trick::Segment	*	pb	=	(*pmit).second	;
												std::map	<	std::string	,	Trick::ScheduledJobQueue	*	>::iterator	jqit	;
												for	(	jqit	=	pb->get_job_queues().begin()	;	jqit	!=	pb->get_job_queues().end()	;	++jqit	)	{
																(*jqit).second->instrument_before(instrument_job)	;
												}
								}
				}

				return	count	;
}

int	Trick::SegmentedExecutive::instrument_job_after(	Trick::JobData	*	instrument_job	)	{

				unsigned	int	count	=	0	;

				if	(	instrument_job	!=	NULL	)	{
								//call	the	base	Executive	instrument	job
								count	=	Trick::Executive::instrument_job_after(	instrument_job	)	;

								//go	through	all	of	the	segmented	jobs	and	instrument	them.
								std::map	<	std::string	,	Trick::Segment	*	>::iterator	pmit	;
								for	(	pmit	=	segment_map.begin()	;	pmit	!=	segment_map.end()	;	++pmit	)	{
												Trick::Segment	*	pb	=	(*pmit).second	;
												std::map	<	std::string	,	Trick::ScheduledJobQueue	*	>::iterator	jqit	;
												for	(	jqit	=	pb->get_job_queues().begin()	;	jqit	!=	pb->get_job_queues().end()	;	++jqit	)	{
																(*jqit).second->instrument_after(instrument_job)	;
												}
								}
				}
				return	count	;
}

int	Trick::SegmentedExecutive::instrument_job_remove(std::string	in_job)	{

				//call	the	base	Executive	instrument	job
				Trick::Executive::instrument_job_remove(	in_job	)	;

				std::map	<	std::string	,	Trick::Segment	*	>::iterator	pmit	;
				for	(	pmit	=	segment_map.begin()	;	pmit	!=	segment_map.end()	;	++pmit	)	{
								Trick::Segment	*	pb	=	(*pmit).second	;
								std::map	<	std::string	,	Trick::ScheduledJobQueue	*	>::iterator	jqit	;
								for	(	jqit	=	pb->get_job_queues().begin()	;	jqit	!=	pb->get_job_queues().end()	;	++jqit	)	{
												(*jqit).second->instrument_remove(in_job)	;
								}
				}
				return	0	;
}

//	This	function	proides	the	shortcut	access	to	set	the	segment	in	python
//	trick.phexec_segment_set("string_segment")
extern	"C"	int	exec_segment_set(	std::string	in_segment	)	{
				if	(	the_phe	!=	NULL	)	{
								return	the_phe->segment_set(in_segment)	;
				}
				return	-1	;
}

//	This	function	proides	the	shortcut	access	to	set	the	segment	switching	in	python
//	trick.phexec_segment_switch()
extern	"C"	int	exec_segment_switch()	{
				if	(	the_phe	!=	NULL	)	{
								return	the_phe->segment_switch()	;
				}
				return	-1	;
}

