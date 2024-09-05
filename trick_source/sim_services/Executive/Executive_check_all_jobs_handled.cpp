
#include	<iostream>
#include	<sstream>
#include	<cstring>

#include	"trick/Executive.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	Loop	through	all	jobs	checking	the	job	handled	flag
-#	Warn	the	user	of	all	jobs	not	handled	by	any	scheduler
-#	Return	the	number	of	unhandled	jobs
*/
int	Trick::Executive::check_all_jobs_handled()	{

				unsigned	int	ii	,	jj	;
				int	ret	=	0	;
				Trick::SimObject	*	curr_obj	;
				Trick::JobData	*	current_job	;

				/**	@par	Detailed	Design	*/
				/**	@li	Check	all	job->handled	flags	*/
				for	(	ii	=	0	;	ii	<	sim_objects.size()	;	ii++	)	{
								curr_obj	=	sim_objects[ii]	;
								for	(	jj	=	0	;	jj	<	curr_obj->jobs.size()	;	jj++	)	{
												current_job	=	curr_obj->jobs[jj]	;
												if	(	!	current_job->get_handled()	)	{
																	message_publish(MSG_WARNING,	"Warning:	Job	%s	of	job	class	\"%s\"is	not	handled	by	any	scheduler\n"	,
																	current_job->name.c_str(),	current_job->job_class_name.c_str())	;
																	ret++	;
												}
								}
				}


				return(ret)	;

}
