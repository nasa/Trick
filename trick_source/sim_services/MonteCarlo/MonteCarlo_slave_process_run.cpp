
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<stdio.h>
#include	<sstream>

#include	"trick/MonteCarlo.hh"
#include	"trick/command_line_protos.h"
#include	"trick/input_processor_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/tc_proto.h"

/**	@par	Detailed	Design:	*/
int	Trick::MonteCarlo::slave_process_run()	{
				int	size;
				/**	<ul><li>	Read	the	length	of	the	incoming	message.	*/
				if	(tc_read(&connection_device,	(char	*)&size,	(int)sizeof(size))	!=	(int)sizeof(size)	||	(size	=	ntohl(size))	<	0)	{
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[%s:%d]	Lost	connection	to	Master	while	receiving	new	run.\nShutting	down.\n",
																												machine_name.c_str(),	slave_id)	;
								}
								slave_shutdown();
				}
				char	*input	=	new	char[size	+	1];
				/**	<li>	Read	the	incoming	message.	*/
				if	(tc_read(&connection_device,	input,	size)	!=	size)	{
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[%s:%d]	Lost	connection	to	Master	while	receiving	new	run.\nShutting	down.\n",
																												machine_name.c_str(),	slave_id)	;
								}
								slave_shutdown();
				}
				tc_disconnect(&connection_device);

				/**
					*	<li>	fork()	a	child	process	to	execute	the	simulation.
					*	This	allows	the	slave	to	monitor	the	child	and	continue	running
					*	if	it	crashes	or	times	out.
					*/
				pid_t	pid	=	fork();
				if	(pid	==	-1)	{
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[%s:%d]	Unable	to	fork	new	process	for	run.\nShutting	down.\n",
																												machine_name.c_str(),	slave_id)	;
								}
								slave_shutdown();
				/**	<li>Parent	process:	*/
				}	else	if	(pid	!=	0)	{
								int	return_value	=	0	;
								/**	<li>	Wait	for	the	child	to	finish.	*/
								if	(waitpid(pid,	&return_value,	0)	==	-1)	{
												/*	(Alex)	On	the	Mac	this	check	gives	a	lot	of	false	positives.		I've	commented	out	the	code	for	now.	*/
												/*
												if	(verbosity	>=	MC_ERROR)	{
																message_publish(MSG_ERROR,	"Monte	[%s:%d]	Error	while	waiting	for	run	to	finish.\nShutting	down.\n",
																																machine_name.c_str(),	slave_id)	;
												}
												slave_shutdown();
												*/
								}

								if	(WIFEXITED(return_value))	{
												//	A	successful	sim	sends	its	exit	status	to	the	master	itself	in
												//	its	shutdown	job.	Users	can	subvert	this	by	calling	exit,	in
												//	which	case	the	master	will	eventually	deem	this	run	to	have
												//	timed	out.	But	who	would	do	that?!
												return	0;
								}

								int	signal	=	WTERMSIG(return_value);
								/**	<li>	Extract	the	exit	status	of	the	child.	*/
								MonteRun::ExitStatus	exit_status	=	signal	==	SIGALRM	?	MonteRun::MC_RUN_TIMED_OUT	:	MonteRun::MC_RUN_DUMPED_CORE;
								if	(verbosity	>=	MC_ERROR)	{
												message_publish(MSG_ERROR,	"Monte	[%s:%d]	Run	killed	by	signal	%d:	%s\n",
																												machine_name.c_str(),	slave_id,	signal,	strsignal(signal))	;
								}
								connection_device.port	=	master_port;
								if	(tc_connect(&connection_device)	!=	TC_SUCCESS)	{
												if	(verbosity	>=	MC_ERROR)	{
																message_publish(MSG_ERROR,	"Monte	[%s:%d]	Lost	connection	to	Master	before	results	could	be	returned.\nShutting	down.\n",
																																machine_name.c_str(),	slave_id)	;
												}
												slave_shutdown();
								}
								if	(verbosity	>=	MC_ALL)	{
												message_publish(MSG_INFO,	"Monte	[%s:%d]	Sending	run	exit	status	to	master	%d.\n",
																												machine_name.c_str(),	slave_id,	exit_status)	;
								}
								/**	<li>	Write	the	slaves	id	to	the	master.	</ul>	*/
								int	id	=	htonl(slave_id);
								tc_write(&connection_device,	(char	*)&id,	(int)sizeof(id));
								/**	<li>	Write	the	child's	exit	status	to	the	master.	</ul>	*/
								return_value	=	htonl(exit_status);
								tc_write(&connection_device,	(char	*)&return_value,	(int)sizeof(return_value));
								tc_disconnect(&connection_device);
								return	0;
				/**	<li>	Child	process:	*/
				}	else	{
								input[size]	=	'\0';
								if	(	ip_parse(input)	!=	0	)	{
												exit(MonteRun::MC_PROBLEM_PARSING_INPUT);
								}

								/**	<ul><li>	Create	the	run	directory.	*/
								std::string	output_dir	=	command_line_args_get_output_dir();
								if	(access(output_dir.c_str(),	F_OK)	!=	0)	{
												if	(mkdir(output_dir.c_str(),	0775)	==	-1)	{
																exit(MonteRun::MC_CANT_CREATE_OUTPUT_DIR);
												}
								}

								std::stringstream	ss_monte_input;
								ss_monte_input	<<	output_dir	<<	"/monte_input";
								FILE	*fp	=	fopen(ss_monte_input.str().c_str(),	"w");

								fprintf(fp,
										"#	This	run	can	be	executed	in	stand	alone	(non-Monte	Carlo)	mode	by	running\n"
										"#	the	S_main	executable	with	this	file	specified	as	the	input	file.\n\n");
								fprintf(fp,	"if	(sys.version_info	>	(3,	0)):\n");
								fprintf(fp,	"				exec(open(\"%s\").read())\n",	command_line_args_get_input_file());
								fprintf(fp,	"else:\n");
								fprintf(fp,	"				execfile(\"%s\")\n\n",	command_line_args_get_input_file());
								fprintf(fp,	"trick.mc_set_enabled(0)\n");
								fprintf(fp,	"%s"	,	input);
								fclose(fp);
								delete	[]	input;

								/**	<li>	redirect	stdout	and	stderr	to	files	in	the	run	directory	*/
								std::stringstream	ss_stdout;
								ss_stdout	<<	output_dir	<<	"/stdout";
								freopen(ss_stdout.str().c_str(),	"w",	stdout);
								std::stringstream	ss_stderr;
								ss_stderr	<<	output_dir	<<	"/stderr";
								freopen(ss_stderr.str().c_str(),	"w",	stderr);

								/**	<li>	Run	the	pre	run	jobs.	*/
								run_queue(&slave_pre_queue,	"in	slave_pre	queue")	;

								/**	<li>	Set	a	timer	to	interrupt	us	after	the	timeout	value.	*/
								struct	sigaction	default_alarm;
								default_alarm.sa_handler	=	SIG_DFL;
								sigaction(SIGALRM,	&default_alarm,	NULL);
								alarm((unsigned	int)timeout);

								/**
									*	<li>	Return	a	non-zero	result	so	the	calling	function	(#slave)
									*	will	return,	allowing	the	slave	sim	to	complete.
									*/
								return	1;
				}
				return	0;
}
