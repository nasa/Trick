
#include	<iostream>

#include	"trick/Executive.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

int	Trick::Executive::set_job_cycle(std::string	job_name,	int	instance_num,	double	in_cycle)	{

				Trick::JobData	*	job	;
				std::multimap<std::string	,	Trick::JobData	*>::iterator	it	;
				std::pair<std::multimap<std::string	,	Trick::JobData	*>::iterator	,	std::multimap<std::string	,	Trick::JobData	*>::iterator>	range	;

				if	(	in_cycle	<	0.0	)	{
								message_publish(MSG_WARNING,	"Warning:	Invalid	cycle	%g	specified	in	Executive::set_job_cycle	for	%s\n"	,	in_cycle,	job_name.c_str())	;
								return	-1	;
				}

				job	=	get_job(job_name,	instance_num)	;

				if	(	job	!=	NULL	)	{
								//	set	cycle	for	one	job	(the	given	job_name)
								job->set_cycle(in_cycle)	;
								job->set_next_call_time(time_tics)	;
				}	else	{
								//	job_name	may	be	a	tag	name:	find	all	jobs	that	have	the	given	tag	name	and	hold	them	in	a	list
								range	=	all_tagged_jobs.equal_range(job_name)	;
								if	(range.first	!=	range.second)	{
												//	set	cycle	for	all	jobs	with	this	tag
												for	(	it	=	range.first;	it	!=	range.second	;	++it	)	{
																(*it).second->set_cycle(in_cycle)	;
																(*it).second->set_next_call_time(time_tics)	;
												}
								}	else	{
												message_publish(MSG_WARNING,	"Warning:	Job	%s	not	found	in	Executive::set_job_cycle\n"	,	job_name.c_str())	;
												return	-2	;
								}
				}

				return(0)	;

}

