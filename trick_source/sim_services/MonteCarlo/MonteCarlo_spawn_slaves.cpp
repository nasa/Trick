
#include	<sstream>
#include	<stdlib.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/command_line_protos.h"
#include	"trick/unix_commands.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

/**
	*	@par	Detailed	Design:
	*	This	function	is	responsible	for	actually	issuing	the	system	call	that
	*	will	create	the	slave	simulations	on	the	appropriate	machines.
	*/
void	Trick::MonteCarlo::spawn_slaves()	{
				/**	<ul><li>	For	all	slaves:	*/
				for	(std::vector<MonteSlave	*>::size_type	i	=	0;	i	<	slaves.size();	++i)	{
								/**	<ul><li>	If	the	slave	is	in	the	UNINITIALZED	state,	then
										*	set	up	the	command	string	for	starting	the	slave.
										*/
								if	(slaves[i]->state	==	MonteSlave::MC_UNINITIALIZED)	{
												initialize_slave(slaves[i])	;
								}
				}
}

void	Trick::MonteCarlo::initialize_slave(Trick::MonteSlave*	slave_to_init)	{
				std::stringstream	ss;
				if	(slave_to_init->remote_shell_args.empty())	{
								slave_to_init->remote_shell_args	=	"";
				}
				std::string	buffer;
				/**	<li>	If	this	is	a	custom	slave	dispatch:	add	the	#custom_pre_text.	*/
				if	(custom_slave_dispatch)	{
								buffer	=	custom_pre_text;
				/**	<li>	Otherwise	add	the	default	pre	text.					*/
				}	else	{
								default_slave_dispatch_pre_text(slave_to_init,	buffer)	;
				}

				if	(!slave_to_init->S_main_name.compare(""))	{
								slave_to_init->S_main_name	=	"./S_main_*.exe";
				}

				/**	<li>	Append	the	S_main	executable	and	slave-specific	arguments.	*/
				ss	<<	slave_to_init->S_main_name
							<<	"	"	<<	command_line_args_get_input_file()
							<<	"	--monte_host	"	<<	machine_name
							<<	"	--monte_sync_port	"	<<	listen_device.port
							<<	"	--monte_client_id	"	<<	slave_to_init->id
							<<	"	-O	"	<<	slave_output_directory;
				buffer	+=	ss.str();

				/**	<li>	Append	user	sim	options.	*/
				if	(!slave_sim_options.empty())	{
								buffer	+=	"	"	+	slave_sim_options;
				}

				/**	<li>	if	this	is	a	custom	slave	dispatch,	append	the	#custom_post_text.	*/
				if	(custom_slave_dispatch)	{
								buffer	+=	custom_post_text;
				}	else	{
								buffer	+=	std::string("'	&");
				}

				if	(verbosity	>=	MC_INFORMATIONAL)	{
								message_publish(MSG_INFO,	"Monte:	Spawning	Slave	%s:%d	:\n%s\n",
																								slave_to_init->machine_name.c_str(),	slave_to_init->id,	buffer.c_str())	;
				}
				/**	<li>	Set	the	slave's	state	to	MC_INITIALIZING.	*/
				slave_to_init->state	=	MonteSlave::MC_INITIALIZING;
				/**	<li>	Make	the	system	call	to	execute	the	shell.	*/
				system(buffer.c_str());
}

void	Trick::MonteCarlo::default_slave_dispatch_pre_text(Trick::MonteSlave*	slave_to_init,	std::string	&buffer)	{
				/**	<ul><li>	If	the	slave	is	running	locally,	use	a	local	shell.	*/
				if	(!localhost_as_remote	&&
								(equals_ignore_case(slave_to_init->machine_name,	machine_name)	||
									equals_ignore_case(slave_to_init->machine_name,	std::string("localhost"))))	{
								buffer	=	"/bin/sh	-c	'";
				/**	<li>	Otherwise,	use	the	appropriate	remote	shell.	*/
				}	else	{
								switch	(slave_to_init->remote_shell)	{
												case	TRICK_RSH:
																buffer	=	unix_rsh;
												break;
												case	TRICK_USER_REMOTE_SH:
																if	(slave_to_init->user_remote_shell.empty())	{
																				slave_to_init->user_remote_shell	=	unix_ssh;
																				if	(verbosity	>=	MC_ERROR)	{
																								message_publish(MSG_WARNING,	"Monte:	TRICK_USER_REMOTE_SH	specified	for	Slave	%s:%d,	but	no	shell	given.\n"
																																																					"Defaulting	to	%s.\n",
																																								machine_name.c_str(),	slave_to_init->id,	slave_to_init->user_remote_shell.c_str())	;
																				}
																}
																buffer	=	slave_to_init->user_remote_shell;
												break;
												default:
																buffer	=	unix_ssh;
												break;
								}
								/**	<ul><li>	Add	the	user-specified	remote	shell	arguments	and	the	remote	machine	name.	</ul>	*/
								buffer	+=	"	"	+	slave_to_init->remote_shell_args	+	"	"	+	slave_to_init->machine_name	+	"	'";
				}

				/**	<li>	Add	the	user	command	string	to	allow	passing
					*	of	environment	variables	and	such	to	the	slave.
					*/
				if	(!user_cmd_string.empty())	{
								buffer	+=	user_cmd_string	+	";	";
				}

				/**	<li>	Add	the	command	to	switch	to	the	appropriate	directory.	*/
				if	(slave_to_init->remote_shell	!=	TRICK_USER_REMOTE_SH)	{
								buffer	+=	"cd	";
								if	(!slave_to_init->sim_path.empty())	{
												buffer	+=	slave_to_init->sim_path;
								}	else	{
												buffer	+=	std::string(command_line_args_get_default_dir());
								}
								buffer	+=	std::string(";	");
				}
}
