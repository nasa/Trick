/*
			PURPOSE:	(	Handle	the	simulation	command	line	args	)

			REFERENCE:	(	Trick	Simulation	Environment	)

			ASSUMPTIONS	AND	LIMITATIONS:	(	None	)

			CLASS:	(	N/A	)

			LIBRARY	DEPENDENCY:	(	None	)

			PROGRAMMERS:	(	Keith	Vetter	LinCom	6/2003	)	*/


#include	<iostream>
#include	<sys/stat.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<cstring>
#include	<cerrno>
#include	<unistd.h>
#include	<sys/types.h>
#include	<pwd.h>

#include	"trick/CommandLineArguments.hh"
#include	"trick/memorymanager_c_intf.h"

Trick::CommandLineArguments	*	the_cmd_args	;

Trick::CommandLineArguments::CommandLineArguments()	{
				the_cmd_args	=	this	;

				output_dir	=	std::string(".")	;
				default_dir	=	std::string(".")	;

				argc	=	0;
				argv	=	NULL;
}

int	Trick::CommandLineArguments::get_argc()	{
				return(argc)	;
}

char	**	Trick::CommandLineArguments::get_argv()	{
				return(argv)	;
}

std::string	Trick::CommandLineArguments::get_output_dir()	{
				return(output_dir)	;
}

std::string	&	Trick::CommandLineArguments::get_output_dir_ref()	{
				return(output_dir)	;
}

std::string	Trick::CommandLineArguments::get_user_output_dir()	{
				return(user_output_dir)	;
}

std::string	&	Trick::CommandLineArguments::get_user_output_dir_ref()	{
				return(user_output_dir)	;
}

std::string	Trick::CommandLineArguments::get_input_file()	{
				return(input_file)	;
}

std::string	&	Trick::CommandLineArguments::get_input_file_ref()	{
				return(input_file)	;
}

std::string	Trick::CommandLineArguments::get_default_dir()	{
				return(default_dir)	;
}

std::string	&	Trick::CommandLineArguments::get_default_dir_ref()	{
				return(default_dir)	;
}

std::string	Trick::CommandLineArguments::get_cmdline_name()	{
				return(cmdline_name)	;
}

std::string	&	Trick::CommandLineArguments::get_cmdline_name_ref()	{
				return(cmdline_name)	;
}

//	Helper	function	-	create	a	full	path	with	error	checking	along	the	way
int	Trick::CommandLineArguments::create_path(const	std::string&	dirname)	{
				size_t	cur_index	=	0;

				std::string	full_dir	(dirname);

				//	These	syscalls	don't	seem	to	take	care	of	home	special	character,	so	do	it	manually
				//	I	think	the	shell	should	handle	it	before	it	gets	here,	but	just	in	case	check	for	it
				if	(dirname.at(0)	==	'~')	{
								struct	passwd	*pw	=	getpwuid(getuid());
								full_dir	=	std::string(pw->pw_dir)	+	dirname.substr(1,	dirname.size());
				}

				while	(cur_index	!=	full_dir.size())	{
								cur_index	=	full_dir.find('/',	cur_index+1);
								if	(cur_index	==	std::string::npos)	{
												cur_index	=	full_dir.size();
								}
								std::string	cur_dir	=	full_dir.substr(0,	cur_index);
								
								struct	stat	info;
								if(stat(	cur_dir.c_str(),	&info	)	!=	0)	{
												//	does	not	exist	-	make	it
												if	(mkdir(cur_dir.c_str(),	0775)	==	-1)	{
																std::cerr	<<	"Error	creating	directory	"	<<	cur_dir	<<	std::endl;
																return	1;
												}
								}	else	{
												//	Does	exist
												if(info.st_mode	&	S_IFDIR)	{
																//	Is	a	directory
																if	(info.st_mode	&	S_IWUSR)	{
																				//	Perfect,	nothing	to	do	here
																}	else	{
																				//	Not	writeable
																				std::cerr	<<	"Intermediate	directory	"	<<	cur_dir	<<	"	is	not	writable,	unable	to	create	output	directory."	<<	std::endl;
																				return	1;
																}
												}	else	{
																//	Does	exist,	but	is	a	file
																std::cerr	<<	"Intermediate	directory	"	<<	cur_dir	<<	"	is	not	a	directory,	unable	to	create	output	directory."	<<	std::endl;
																return	1;
												}
								}
				}
				return	0;
}

/**
@details
-#	Save	the	number	of	command	line	arguments.
-#	Save	the	command	line	arguments.
-#	Determine	the	directory	and	simulation	executable	name	from	the	first
			command	line	argument
			-#	If	no	directory	is	specified
						-#	Save	the	simulation	executable	name	as	the	full	first	argument
						-#	Get	the	current	working	directory
						-#	Save	the	current	working	directory	as	the	default	input	directory.
			-#	If	a	relative	or	full	directory	is	present	in	the	command	line	arguments
						-#	split	the	command	line	argument	on	the	last	"/"	character
						-#	Save	the	back	half	of	the	split	as	the	simulation	executable	name
						-#	Save	the	current	working	directory
						-#	Change	the	current	working	directory	to	the	first	half	of	the	split
									command	line	argument.
						-#	Get	the	current	working	directory
						-#	Save	the	current	working	directory	as	the	default	input	directory.
						-#	Change	the	current	working	directory	to	the	saved	working	directory
-#	If	present,	the	second	argument	will	be	the	run	directory	and	the	input	file.
			-#	Save	the	run	input	file		name	as	the	full	second	argument
			-#	Split	the	second	argument	on	the	last	"/"	character
			-#	If	no	directory	is	specified	set	the	default	output	and	current	output
						directories	to	the	current	directory
			-#	Else	set	the	default	and	the	current	output	directories	to	the	first
						half	of	the	split
-#	Search	the	remaining	command	line	arguments	for	"-O"	or	"-OO"
			-#	If	found	set	the	output	direcory	to	the	following	argument
			-#	Attempt	to	create	the	output	directory	if	it	does	not	exist.
						-#	Exit	if	the	simulation	cannot	be	created.
*/
int	Trick::CommandLineArguments::process_sim_args(int	nargs	,	char	**args)	{

				char	*buf,	*buf2;
				size_t	found	;

				argc	=	nargs	;
				argv	=	(char	**)TMM_declare_var_1d("char	*",	argc)	;
				for	(int	ii	=	0	;	ii	<	argc	;	ii++	)	{
							argv[ii]	=	TMM_strdup(args[ii])	;
				}

				buf	=	(char	*)malloc(4096)	;
				buf2	=	(char	*)malloc(4096)	;

				default_dir	=	argv[0]	;
				found	=	default_dir.find_last_of("/")	;
				if	(	found	==	std::string::npos	)	{
								cmdline_name	=	default_dir	;
								getcwd(buf,	(size_t)	256);
								default_dir	=	buf	;
				}	else	{
								cmdline_name	=	default_dir.substr(found	+	1)	;

								//	save	the	current	directory
								getcwd(buf,	(size_t)	256);

								//	change	to	the	default	directory
								chdir(default_dir.substr(0,found).c_str());

								//	get	the	full	path	default	directory
								getcwd(buf2,	(size_t)	256);
								default_dir	=	buf2	;

								//	change	back	to	the	current	directory
								chdir(buf);
				}

				free(buf)	;
				free(buf2)	;

				if	(	argc	>	1	)	{

				/*	First	occurnance	of	"RUN_*"	is	the	input	file	name:	'<Run_dir>/<file_name>'.	
							If	not	found,	defaults	to	first	argument	*/
								
								input_file	=	argv[1];
								run_dir	=	argv[1];

								for(int	ii	=	1;	ii	<	argc;	ii++)	{
												if(std::string(argv[ii]).find("RUN_")	!=	std::string::npos)	{
														input_file	=	argv[ii];
														run_dir	=	argv[ii];
														break;
												}
								}

								if	(access(input_file.c_str(),	F_OK)	!=	0)	{
										input_file	=	"";
												if(strcmp(argv[1],	"trick_version")	&&	strcmp(argv[1],	"sie")	&&	strcmp(argv[1],	"-help")		&&	strcmp(argv[1],	"--help")	&&
																strcmp(argv[1],	"-h")	&&	strcmp(argv[1],	"help"))	{
																std::cerr	<<	"\nERROR:	Invalid	input	file	or	command	line	argument."	<<	std::endl;
																exit(1);
												}
								}

								found	=	run_dir.find_last_of("/")	;
								if	(	found	!=	std::string::npos	)	{
												run_dir.erase(found)	;
								}	else	{
												run_dir	=	"."	;
								}
								/*	check	existence	of	run	directory	*/
								if	(access(run_dir.c_str(),	F_OK)	!=	0)	{
												std::cerr	<<	"\nERROR:	while	accessing	input	file	directory	\""	<<	run_dir	<<	"\"	:	"	<<	std::strerror(errno)	<<	std::endl	;
												exit(1);
								}

								output_dir	=	run_dir	;

								for	(int	ii	=	1;	ii	<	argc;	ii++)	{
												if	(!strncmp("-OO",	argv[ii],	(size_t)	3)	||	!strncmp("-O",	argv[ii],	(size_t)	2))	{
																if	(ii	==	(	argc	-	1	))	{
																				std::cerr	<<	"\nERROR:	No	directory	specified	after	-O	or	-OO	argument"	<<	std::endl	;
																				exit(1)	;
																}
																/*	Output	data	directory	*/
																output_dir	=	user_output_dir	=	argv[++ii];
																if	(!strncmp("-OO",	argv[ii-1],	(size_t)	3))	{
																				output_dir	=	output_dir	+	"/"	+	run_dir;
																}	
												}
								}

								/*	Create	output	directory	if	necessary.	*/
								if	(access(output_dir.c_str(),	F_OK)	!=	0)	{
												if	(create_path(output_dir)	!=	0)	{
																std::cerr	<<	"\nERROR:	While	trying	to	create	output	directory	\""	<<	output_dir	<<	"\"	:	"	<<	std::strerror(errno)	<<	std::endl	;
																exit(1)	;
												}
								}	else	{
												/*	check	permissions	on	output	directory	*/
												if	(access(output_dir.c_str(),	(W_OK|X_OK)	)	==	-1)	{
																std::cerr	<<	"\nERROR:	while	writing	to	output	directory	\""	<<	output_dir	<<	"\"	:	"	<<	std::strerror(errno)	<<	std::endl	;
																exit(2)	;
												}
								}
				}

				return(0)	;

}


/**
@design
-#	Get	the	current	system	date	and	time.
-#	Set	the	subdirectory	within	the	current	output	directory	to
			DATA_<year>_<month>_<day>_<hour>_<minute>_<second>
			-#	Attempt	to	create	the	output	directory	if	it	does	not	exist.
						-#	Exit	if	the	simulation	cannot	be	created.
*/
int	Trick::CommandLineArguments::output_dir_timestamped_on()	{

				time_t	date	;
				struct	tm	*curr_time	;
				char	temp_str[256]	;

				date	=	time(NULL)	;
				curr_time	=	localtime(&date)	;

				snprintf(temp_str,	sizeof(temp_str),	"DATA_%4d_%02d_%02d_%02d_%02d_%02d",
												curr_time->tm_year	+	1900	,	curr_time->tm_mon	+	1	,	curr_time->tm_mday,
												curr_time->tm_hour	,	curr_time->tm_min	,	curr_time->tm_sec	);

				time_stamp_dir	=	std::string(temp_str)	;

				if	(	!	user_output_dir.empty()	)	{
								output_dir	=	user_output_dir	+	"/"	+	time_stamp_dir	;
				}	else	{
								output_dir	=	run_dir	+	"/"	+	time_stamp_dir	;
				}

				/*	Create	directories	if	necessary.	*/
				if	(access(output_dir.c_str(),	F_OK)	!=	0)	{
								if	(mkdir(output_dir.c_str(),	0775)	==	-1)	{
												std::cerr	<<	"\nERROR:	While	trying	to	create	dir	\""	<<	output_dir	<<	"\"	Exiting!"	<<	std::endl	;
												exit(1)	;
								}
				}

				output_dir_timestamped	=	1	;
				return(0)	;
}

/**
@design
-#	If	the	user	had	specified	an	output	directory	reset
			the	output	directory	to	the	user	specified	directory
-#	Else	set	the	output	directory	to	the	run	directory.
*/
int	Trick::CommandLineArguments::output_dir_timestamped_off()	{

				if	(	!	user_output_dir.empty()	)	{
								output_dir	=	user_output_dir	;
				}	else	{
								output_dir	=	run_dir	;
				}

				output_dir_timestamped	=	0	;
				return(0)	;

}

/**
@design
-#		Sets	the	value	of	output_dir
*/
void	Trick::CommandLineArguments::set_output_dir(std::string	output_directory)	{
				output_dir	=	output_directory;
}
