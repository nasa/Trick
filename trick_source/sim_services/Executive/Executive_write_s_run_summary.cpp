
#include	<iostream>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>

#include	"trick/Executive.hh"
#include	"trick/command_line_protos.h"
#include	"trick/env_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
@details
-#	If	the	incoming	file	pointer	is	NULL
			-#	Get	the	output	directory
			-#	Get	path	to	S_run_summary	file	as	%<output	directory%>/S_run_summary
			-#	Open	the	output	file	for	writing
			-#	Return	if	the	file	could	not	be	opened	for	writing
-#	The	scheduler	writes	out	the	S_run_summary	header
-#	The	scheduler	writes	out	the	build	time	Trick	enviromenment	variables
*/
int	Trick::Executive::write_s_run_summary(FILE	*fp)	{

				char	buf[1024];
				std::string	output_dir	;

				if	(fp	==	NULL)	{

								/*	Get	full	path	to	S_run_summary	*/
								output_dir	=	command_line_args_get_output_dir()	;
								snprintf(buf,	sizeof(buf),	"%s/S_run_summary",	output_dir.c_str());

								/*	Open	the	S_run_summary	file.		If	it	fails,	it's	not	a	fatal	error,	return	0.	*/
								if	((fp	=	fopen(buf,	"w"))	==	NULL)	{
												message_publish(MSG_ERROR,	"Could	not	open	%s/S_run_summary	for	writing",	output_dir.c_str());
												return(0);
								}
				}

				/*	Write	out	the	s_run_summary	header	*/
				fprintf(fp,	"\n===============================================================================\n");
				fprintf(fp,	"General:\n\n");

				fprintf(fp,	"Executable	called	=	%s/%s\n",	command_line_args_get_default_dir(),	command_line_args_get_cmdline_name());
				fprintf(fp,	"Input	file	=	%s\n",	command_line_args_get_input_file());

				fprintf(fp,	"S_main	build	time	=	%s\n",	build_date.c_str());
				fprintf(fp,	"Trick	version	=	%s\n",	current_version.c_str());

				/*	Write	out	the	build	time	Trick	enviromenment	variables.	*/
				fprintf(fp,	"\n===============================================================================\n");
				fprintf(fp,	"Build	time	Environment:\n\n");

				env_print_vars(fp)	;

				fprintf(fp,	"\n");
				fclose(fp);

				return(0)	;

}

