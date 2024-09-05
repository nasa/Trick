
#include	<iostream>
#include	<sstream>

#include	"trick/Executive.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	Find	target	job.		Return	error	if	target	job	not	found.
-#	Find	depends	job.		Return	error	if	depends	job	not	found.
-#	Return	an	error	if	both	jobs	are	on	the	same	thread.
-#	The	depend	passed	all	checks,	add	the	depends	job	to	the	target	job	dependency	list
*/
int	Trick::Executive::add_depends_on_job(	std::string	target_job_string	,	unsigned	int	t_instance	,
																																										std::string	depend_job_string	,	unsigned	int	d_instance	)	{

				Trick::JobData	*	target_job	;
				Trick::JobData	*	depend_job	;

				/*	Find	target	job.		Return	error	if	target	job	not	found.	*/
				target_job	=	get_job(target_job_string,	t_instance)	;
				if	(	target_job	==	NULL	)	{
								message_publish(MSG_ERROR,	"add_depends_on_job:	Could	not	find	target	job	%s	instance	%d\n",
									target_job_string.c_str(),	t_instance)	;
								return(-1)	;
				}

				/*	Find	depends	job.		Return	error	if	depends	job	not	found.	*/
				depend_job	=	get_job(depend_job_string,	d_instance)	;
				if	(	depend_job	==	NULL	)	{
								message_publish(MSG_ERROR,	"add_depends_on_job:	Could	not	find	depend	job	%s	instance	%d\n",
									depend_job_string.c_str(),	d_instance)	;
								return(-2)	;
				}

				/*	Return	an	error	if	both	jobs	are	on	the	same	thread	*/
				if(	target_job->thread	==	depend_job->thread)	{
								message_publish(MSG_ERROR,	"add_depends_on_job:	target	job	%s	and	depend	job	%s	on	the	same	thread",
									target_job_string.c_str()	,	depend_job_string.c_str())	;		;
								return(-3)	;
				}

				/*	Passed	all	checks,	add	the	depends	job	to	the	target	job	dependency	list	*/
				target_job->add_depend(depend_job)	;

				return(0)	;

}
