
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/stat.h>
#include	<sstream>

#include	"trick/Executive.hh"
#include	"trick/command_line_protos.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	If	the	incoming	file	pointer	is	NULL
			-#	Get	the	output	directory
			-#	Get	path	to	S_job_execution	file	as	%<output	directory%>/S_job_execution
			-#	Open	the	output	file	for	writing
			-#	Return	if	the	file	could	not	be	opened	for	writing
-#	Write	out	the	s_job_execution	header	open	the	S_job_execution	file
-#	Write	default_data	and	initialization	job	entries	to	S_job_execution
-#	Write	scheduled	job	entries	to	S_job_execution
-#	Write	top_of_frame	job	entries	to	S_job_execution
-#	Write	end_of_frame	job	entries	to	S_job_execution
-#	Write	freeze_init,	freeze,	and	unfreeze	job	entries	to	S_job_execution
-#	Write	shutdown	job	entries	to	S_job_execution
-#	Call	the	write_s_job_execution	job	of	any	other	schedulers	registered	with	the	executive.
*/
int	Trick::Executive::write_s_job_execution(FILE	*fp)	{

				char	buf[1024];
				unsigned	int	ii	;
				std::string	output_dir	;

				if	(	fp	==	NULL	)	{

								/*	Get	full	path	to	S_job_execution	*/
								output_dir	=	command_line_args_get_output_dir()	;
								snprintf(buf,	sizeof(buf),	"%s/S_job_execution",	output_dir.c_str());

								/*	Create	the	output	directory	if	it	does	not	exist	*/
								if	(access(output_dir.c_str(),	F_OK)	!=	0)	{
												if	(mkdir(output_dir.c_str(),	0775)	!=	0)	{
																message_publish(MSG_ERROR,	"Error	while	trying	to	create	directory	%s.\n",	output_dir.c_str())	;
												return	-1;
												}
								}

								/*	Open	the	S_job_execution	file.		If	it	fails,	it's	not	a	fatal	error,	return	0.	*/
								if	((fp	=	fopen(buf,	"w"))	==	NULL)	{
												message_publish(MSG_ERROR,	"Could	not	open	%s/S_job_execution	for	writing\n",	output_dir.c_str());
												return	(0)	;
								}
				}

				fprintf(fp,	"Executive	time	tic	value	(tics/second)	information\n");
				fprintf(fp,	"				time	tic	value	=	%d\n\n",	time_tic_value);

				fprintf(fp,	"Thread	information\n");
				for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
								std::stringstream	oss	;
								threads[ii]->dump(oss)	;
								fprintf(fp,"%s",	oss.str().c_str())	;
				}
				fprintf(fp,	"\n")	;

				/*	Write	out	the	s_job_execution	header	open	the	S_job_execution	file.	*/
				fprintf(fp,	"Job	information\n");
				fprintf(fp,	"Enabled	|	PID	|Class																				|	Phase	|		Start			|		Cycle			|			Stop			|	ID		|	Job	Name\n");
				fprintf(fp,	"=================================================================================================\n")	;
				fprintf(fp,	"Initialization:\n")	;

				/*	Write	default_data	and	initialization	job	entries	to	S_job_execution.	*/
				default_data_queue.write_non_sched_queue(fp)	;
				initialization_queue.write_non_sched_queue(fp)	;

				/*	Write	scheduled	job	entries	to	S_job_execution.	*/
				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"Scheduled	Loop:\n\n")	;
				for	(	ii	=	0	;	ii	<	threads.size()	;	ii++	)	{
								fprintf(fp,	"Thread	%u:\n",	ii)	;
								threads[ii]->job_queue.write_sched_queue(fp)	;
								fprintf(fp,	"\n")	;
				}

				/*	Write	top_of_frame	job	entries	to	S_job_execution.	*/
				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"Top	of	Frame:\n\n")	;
				fprintf(fp,	"Thread	0:\n");
				top_of_frame_queue.write_sched_queue(fp)	;
				for	(	ii	=	1	;	ii	<	threads.size()	;	ii++	)	{
								fprintf(fp,	"Thread	%u:\n",	ii)	;
								threads[ii]->top_of_frame_queue.write_sched_queue(fp)	;
								fprintf(fp,	"\n")	;
				}

				/*	Write	end_of_frame	job	entries	to	S_job_execution.	*/
				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"End	of	Frame:\n\n")	;
				fprintf(fp,	"Thread	0:\n");
				end_of_frame_queue.write_sched_queue(fp)	;
				for	(	ii	=	1	;	ii	<	threads.size()	;	ii++	)	{
								fprintf(fp,	"Thread	%u:\n",	ii)	;
								threads[ii]->end_of_frame_queue.write_sched_queue(fp)	;
								fprintf(fp,	"\n")	;
				}

				/*	Write	freeze_init,	freeze,	and	unfreeze	job	entries	to	S_job_execution.	*/
				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"Freeze	Loop:\n")	;
				freeze_init_queue.write_non_sched_queue(fp)	;
				freeze_scheduled_queue.write_sched_queue(fp)	;
				freeze_queue.write_non_sched_queue(fp)	;
				unfreeze_queue.write_non_sched_queue(fp)	;

				/*	Write	shutdown	job	entries	to	S_job_execution.	*/
				fprintf(fp,	"\n===================================================================================================\n")	;
				fprintf(fp,	"Shutdown:\n")	;
				shutdown_queue.write_non_sched_queue(fp)	;


				/*	Write	entries	from	other	schedulers		*/
				for	(	ii	=	0	;	ii	<	other_schedulers.size()	;	ii++	)	{
								other_schedulers[ii]->write_s_job_execution(fp)	;
				}

				fprintf(fp,	"\n===================================================================================================\n")	;

				fclose(fp);

				return(0)	;

}

