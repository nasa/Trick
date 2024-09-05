
#include	<iostream>

#include	"trick/Executive.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

int	Trick::Executive::set_job_onoff(std::string	job_name,	int	instance_num	,	int	on)	{

				Trick::JobData	*	job	;
				std::multimap<std::string	,	Trick::JobData	*>::iterator	it	;
				std::pair<std::multimap<std::string	,	Trick::JobData	*>::iterator	,	std::multimap<std::string	,	Trick::JobData	*>::iterator>	range	;

				job	=	get_job(job_name,	instance_num)	;

				if	(	job	!=	NULL	)	{
								//	set	disabled	flag	accordingly	for	one	job	(the	given	job_name)
								job->disabled	=	!on	;
				}	else	{
								//	job_name	may	be	a	tag	name:	find	all	jobs	that	have	the	given	tag	name	and	hold	them	in	a	list
								range	=	all_tagged_jobs.equal_range(job_name)	;
								if	(range.first	!=	range.second)	{
												//	set	disabled	flag	accordingly	for	all	jobs	with	this	tag
												for	(	it	=	range.first;	it	!=	range.second	;	++it	)	{
																it->second->disabled	=	!on	;
												}
								}	else	{
												message_publish(MSG_WARNING,	"Warning:	Job	%s	not	found	in	Executive::set_job_onoff\n"	,	job_name.c_str())	;
												return	-1	;
								}
				}


				return(0)	;



}

