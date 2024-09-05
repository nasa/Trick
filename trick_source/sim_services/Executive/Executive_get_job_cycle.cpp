
#include	<iostream>

#include	"trick/Executive.hh"

double	Trick::Executive::get_job_cycle(std::string	job_name)	{

				Trick::JobData	*	job	=	NULL	;

				if	(	job_name.empty()	)	{
								return(get_curr_job()->cycle)	;
				}	else	{
								if	(	(job	=	get_job(job_name))	!=	NULL	)	{
												return(job->cycle)	;
								}
				}

				return(0.0)	;

}

