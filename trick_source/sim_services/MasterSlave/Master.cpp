/*
			PURPOSE:	(Master	for	master/slave	syncrhonization)
	*/

#include	<iostream>
#include	<sstream>
#include	<string>
#include	<pwd.h>
#include	<stdlib.h>
#include	<unistd.h>

#include	<sys/types.h>
#include	<sys/wait.h>

#include	"trick/Master.hh"
#include	"trick/master_proto.h"
#include	"trick/MSSocket.hh"
#include	"trick/MSSharedMem.hh"
#include	"trick/exec_proto.h"
#include	"trick/sim_mode.h"
#include	"trick/command_line_protos.h"
#include	"trick/unix_commands.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/env_proto.h"
#include	"trick/CheckPointRestart_c_intf.hh"	//	for	checkpoint_get_output_file

Trick::Master	*	the_ms_master	;

Trick::SlaveInfo::SlaveInfo()	{

				enabled	=	true	;
				activated	=	false	;
				slave_type	=	"undefined";

				/*	default	the	slaves	to	start	with	ssh	*/
				remote_shell	=	TRICK_SSH	;

				/*	default	the	master	not	to	quit	if	a	slave	dies	*/
				sync_error_terminate	=	false	;

				/*	default	the	slaves	to	start	on	the	localhost	*/
				machine_name	=	"localhost"	;

				sync_wait_limit	=	0.0	;
				reconnect_wait_limit	=	0.0	;
				reconnect_count	=	0;
				chkpnt_dump_auto	=	true	;
				chkpnt_load_auto	=	true	;
}

int	Trick::SlaveInfo::set_connection_type(Trick::MSConnect	*	in_connection)	{

				connection	=	in_connection	;
				return(0)	;
}

int	Trick::SlaveInfo::start()	{

				int	arg_i;
				char	*display;

				std::stringstream	startup_command	;
				std::stringstream	temp_stream	;

				struct	passwd	*passp;

				int	argc	;
				char	**	argv	;

				std::string	slave	=	"undefined";

				/**	@par	Detailed	Design	*/
				argc	=	command_line_args_get_argc()	;
				argv	=	command_line_args_get_argv()	;

				/**	@li	If	the	slave	connection	type	is	not	specified	return	an	error	*/
				if	(	connection	==	NULL	)	{
								message_publish(MSG_ERROR	,	"Slave	connection	type	not	specified.\n")	;
								return(-1)	;
				}

				/**	@li	Start	remote	startup	command	with	remote	shell	to	use	and	remote	machine	name*/
				switch	(	remote_shell	)	{
								case	TRICK_RSH:
												startup_command	<<	unix_rsh	<<	"	"	<<	remote_shell_args	;
												break;
								case	TRICK_USER_REMOTE_SH:
												if	(	user_remote_shell.empty()	)	{
																message_publish(MSG_WARNING	,	"TRICK_USER_REMOTE_SH	specified	for	Slave	startup,	but	no	shell	given.\nDefaulting	to	%s.\n",	unix_ssh)	;
																user_remote_shell	=	unix_ssh	;
												}
												startup_command	<<	user_remote_shell	<<	"	"	<<	remote_shell_args	;
												break;
								case	TRICK_SSH:
								default:
												startup_command	<<	unix_ssh	<<	"	"	<<	remote_shell_args	;
												break;
				}
				startup_command	<<	"	"	<<	machine_name	;

				/**	@li	Add	the	remote	display.	If	a	remote	display	has	not	been	specified,	use	the	environment's	*/
				if	(	machine_display.empty()	)	{
								if	((display	=	(char	*)	env_get_var("DISPLAY"))	==	NULL)	{
												message_publish(MSG_ERROR,	"Cannot	get	environment	variable	$DISPLAY	for	Slave.\n")	;
								}	else	{
												machine_display	=	display	;
								}
				}

				/**	@li	cd	to	the	simulation	path	in	the	remote	startup	command	*/
				if	(	sim_path.empty()	)	{
								message_publish(MSG_ERROR,	"Slave	startup	sim_path	is	empty.\n")	;
								return(-1)	;
				}
				/**	@li	Set	the	DISPLAY	environment	variable	in	the	remote	startup	command.
												Use	setenv	for	*csh	or	export	for	all	other	shells	*/
				passp	=	getpwuid(getuid());
				if	(	!	machine_display.empty()	)	{
								if	(strstr(passp->pw_shell,	"csh"))	{
												startup_command	<<	"	'setenv	DISPLAY	"	<<	machine_display	;
								}	else	{
												startup_command	<<	"	'export	DISPLAY="	<<	machine_display	;
								}
								startup_command	<<	"	;	cd	"	<<	sim_path	<<	"	;	"	;
				}	else	{
								startup_command	<<	"	'cd	"	<<	sim_path	<<	"	;	"	;
				}

				if	(strstr(passp->pw_shell,	"csh"))	{
								startup_command	<<	"	setenv	TRICK_HOST_CPU	`trick-gte	TRICK_HOST_CPU`	;	"	;
				}	else	{
								startup_command	<<	"	export	TRICK_HOST_CPU=`trick-gte	TRICK_HOST_CPU`	;	"	;
				}


				/**	@li	start	the	simulation	in	the	remote	startup	command	*/
				if	(	S_main_name.empty()	)	{
								S_main_name	=	"./S_main_${TRICK_HOST_CPU}.exe"	;
				}
				startup_command	<<	S_main_name	;

				/**	@li	add	the	user	provided	run	input	file	if	provided	*/
				if	(	!	run_input_file.empty()	)	{

								startup_command	<<	"	"	<<	run_input_file	;
				}

				/**	@li	Add	the	connection	specific	arguments	to	the	startup	command	*/
				startup_command	<<	"	"	<<	connection->add_sim_args(	slave_type	)	;

				/*	@li	Add	the	additional	user	arguments	to	the	remote	command	*/
				if	(	!	other_args.empty()	)	{
								startup_command	<<	"	-u	"	<<	other_args	;
				}
				for	(arg_i	=	2;	arg_i	<	argc	;	arg_i++)	{
								startup_command	<<	"	"	<<	argv[arg_i]	;
				}

				/*	@li	start	the	remote	command	in	the	background	*/
				startup_command	<<	"'	&"		;

				message_publish(MSG_INFO,	startup_command.str().c_str())	;

				/*	@li	Execute	the	startup	command	*/
				if	(system(startup_command.str().c_str()))	{
								perror("Slave_start");
								return	(-2)	;
				}

				/*	@li	Wait	for	the	slave	to	connect	to	the	master	*/
				connection->accept()	;

				/*	@li	Set	the	synchronization	wait	limit	*/
				connection->set_sync_wait_limit(sync_wait_limit)	;

				/*	Set	slave	to	activated	*/
				activated	=	true;

				return	(0)	;
}

int	Trick::SlaveInfo::read_slave_status()	{

				MS_SIM_COMMAND	slave_command	;
				MS_SIM_COMMAND	exec_command	;

				/**	@par	Detailed	Design:	*/
				/**	@li	If	the	slave	is	an	active	synchronization	partner	(activated	==	true)	*/
				if	(activated	==	true)	{

								/**	@li	read	the	current	slave	exec_command	*/
								slave_command	=	connection->read_command()	;
								//printf("DEBUG	master	read	%d	command	from	slave\n",	slave_command);fflush(stdout);

								exec_command	=	(MS_SIM_COMMAND)exec_get_exec_command()	;
								//	fixup:	is	it	possible	we	won't	get	slave's	Exit	command	over	socket	when	it	terminates?,	set	it	here	if	that	happens
								if	(dynamic_cast<MSSocket*>(connection))	{
												if	((slave_command	==	MS_ErrorCmd)	&&	(reconnect_wait_limit	>	0.0)	&&	(reconnect_count	==	0))	{
																slave_command	=	MS_ExitCmd;
												}
								}

								/**	@li	If	the	master	is	not	currently	exiting,	change	modes	if	the	slave	is	freezing/exiting	or	has	an	error	*/
								if	(	exec_command	!=	MS_ExitCmd	)	{
												switch	(	slave_command	)	{
																case	(MS_ErrorCmd):
																				/**	@li	if	the	user	has	set	a	reconnect_wait_limit,	continue	on	if	we	are	still	under	that	limit,	otherwise
																												if	the	current	slave	mode	cannot	be	read,	exit	the	master	if	sync_error_terminate	==	true,
																												otherwise	set	the	activated	flag	to	false	*/
																				if	(	(reconnect_count	*	exec_get_freeze_frame())	<	reconnect_wait_limit)	{
																								reconnect_count++;
																				}	else	if	(sync_error_terminate	==	true)	{
																								message_publish(MSG_ERROR,	"Master	lost	sync	with	slave,	so	master	is	terminating.\n")	;
																								exec_terminate_with_return(-1,	__FILE__,	__LINE__	,	"Master	lost	sync	with	slave.")	;
																				}
																				else	{
																								message_publish(MSG_ERROR,	"Master	lost	sync	with	slave,	so	slave	is	being	deactivated.\n")	;
																								activated	=	false	;
																								return(0)	;
																				}
																				break	;
																case	(MS_ExitCmd):
																				/**	@li	if	the	current	slave	mode	is	exiting,	exit	the	master	if	sync_error_terminate	==	true.
																												otherwise	wait	for	slave	to	reconnect.	when	wait	limit	is	0,	set	the	activated	flag	to	false	*/
																				if		(sync_error_terminate	==	true){
																								message_publish(MSG_WARNING,	"sync_error_terminate	is	true:	Slave	is	exiting,	so	master	is	terminating.\n")	;
																								exec_terminate_with_return(-1,	__FILE__,	__LINE__	,	"Slave	is	exiting,	so	is	the	master.")	;
																				}
																				else	{
																								message_publish(MSG_WARNING,	"Slave	is	exiting.\n")	;
																								//	if	reconnect_wait_limit	is	set,	master	waits	for	slave	to	reconnect
																								if	(reconnect_wait_limit	>	0.0)	{
																												message_publish(MSG_WARNING,	"Master	will	wait	%f	seconds	for	slave	to	reconnect.\n",	reconnect_wait_limit)	;
																												//	make	reads	(shared	mem	connection)	return	quickly	so	we	don't	overrun	waiting	for	reconnect
																												connection->set_sync_wait_limit(exec_get_freeze_frame());
																								}
																								else	{
																												message_publish(MSG_WARNING,	"reconnect_wait_limit:	0.0	-	Master	will	stop	communicating	with	slave.\n")	;
																												activated	=	false	;
																								}
																								return(0)	;
																				}
																				break	;
																case	(MS_FreezeCmd):
																				/**	@li	if	the	current	slave	is	freezing,	freeze	the	master	too	*/
																				message_publish(MSG_INFO,	"Slave	is	freezing.\n")	;
																				exec_set_exec_command(FreezeCmd)	;
																				reconnect_count	=	0;
																				break	;
																case	(MS_ReconnectCmd):
																				//	set	the	sync	wait	limit	back	to	its	default
																				connection->set_sync_wait_limit(sync_wait_limit);
																				message_publish(MSG_INFO,	"Master	has	reconnected	to	slave.\n")	;
																				reconnect_count	=	0;
																				break	;
																default:
																				break	;
												}
								}
				}
				return(0)	;
}

int	Trick::SlaveInfo::write_master_status()	{
				/**	@par	Detailed	Design:	*/
				/**	@li	If	the	slave	is	an	active	synchronization	partner	(activated	==	true)	*/
				/**	@li	and	we	are	not	currently	waiting	for	slave	to	reconnect,	*/
				if	((	activated	==	true	)	&&	(reconnect_count	==	0))	{
								/**	@li	write	the	current	time	according	to	the	master	to	the	slave	*/
								connection->write_time(exec_get_time_tics())	;
								/**	@li	write	the	current	exec_command	according	to	the	master	to	the	slave	*/
								connection->write_command((MS_SIM_COMMAND)exec_get_exec_command())	;
				}
				return(0)	;
}

int	Trick::SlaveInfo::write_master_chkpnt_name(std::string	full_path_name)	{
				/**	@par	Detailed	Design:	*/
				/**	@li	If	the	slave	is	an	active	synchronization	partner	(activated	==	true)	*/
				if	(	activated	==	true	)	{
								/**	@li	write	the	checkpoint	dir/filename	to	the	slave	*/
								if	(full_path_name.length()	>	sizeof(chkpnt_name)-1)	{
												message_publish(MSG_ERROR,	"Master	could	not	send	checkpoint	name	to	slave	because	name	too	long	(max	=	%d).\n",
																												sizeof(chkpnt_name))	;
												chkpnt_name[0]	=	MS_ERROR_NAME;	//	send	error	character
								}	else	{
												strcpy(chkpnt_name,	full_path_name.c_str());
								}
								connection->write_name(chkpnt_name,	sizeof(chkpnt_name))	;
				}

				return(0)	;
}

Trick::Master::Master()	{
				enabled	=	false	;
				the_ms_master	=	this	;
				num_slaves	=	0	;
}

int	Trick::Master::enable()	{
				enabled	=	true	;
				return(0);
}

int	Trick::Master::disable()	{
				enabled	=	false	;
				return(0);
}

Trick::SlaveInfo	*	Trick::Master::add_slave(Trick::SlaveInfo	*	new_slave)	{


				/**	@par	Detailed	Design:	*/
				/**	@li	if	the	incoming	parameter	is	NULL	or	not	specified	allocate	space	for	a	new	slave	*/
				if	(new_slave	==	NULL	)	{
								new_slave	=	new	Trick::SlaveInfo	;
				}

				/**	@li	push	the	slave	onto	the	vector	of	slaves	*/
				slaves.push_back(new_slave)	;

				num_slaves++	;

				/**	@li	return	the	address	of	the	new	slave	*/
				return(new_slave)	;

}

int	Trick::Master::process_sim_args()	{

				return(0)	;
}

int	Trick::Master::init()	{

				unsigned	int	ii	;

				/**	@li	Call	Trick::SlaveInfo::start()	for	each	slave	*/
				if	(	enabled	)	{
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												slaves[ii]->start()	;
												/**	@li	Write	the	master's	software	frame	to	each	slave	*/
												slaves[ii]->connection->write_time(exec_get_software_frame_tics())	;
												/**	@li	Write	the	master's	time	tic	value	to	each	slave	*/
												slaves[ii]->connection->write_time(exec_get_time_tic_value())	;
												/**	@li	Write	the	master's	sync_wait_limit	to	each	slave	*/
												slaves[ii]->connection->write_time((long	long)(slaves[ii]->sync_wait_limit	*	exec_get_time_tic_value()))	;
												/**	@li	Cast	the	freeze	command	and	write	it	to	the	slave	*/
												slaves[ii]->connection->write_command((MS_SIM_COMMAND)exec_get_freeze_command())	;
												/**	@li	Write	a	flag	word	containing	the	checkpoint	pre_init,	post_init,	and	end	flags	to	the	slave	*/
												slaves[ii]->connection->write_time((long	long)	((get_checkpoint_pre_init()	<<	2)	+
																																																												(get_checkpoint_post_init()	<<	1)	+
																																																												(get_checkpoint_end()))	);
								}

								//	Freezes	are	only	allowed	on	frame	boundaries	when	Master/Slave	is	enabled.
								exec_set_freeze_on_frame_boundary(true)	;

				}

				return(0)	;
}

/**
@details
-#	Read	the	status	of	all	slaves
*/
int	Trick::Master::end_of_frame_status_from_slave()	{
				unsigned	int	ii	;
				if	(	enabled	)	{
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												slaves[ii]->read_slave_status()	;
								}
				}

				return(0)	;
}

/**
@details
-#	Write	the	master	status	to	all	slaves
*/
int	Trick::Master::end_of_frame_status_to_slave()	{
				unsigned	int	ii	;
				if	(	enabled	)	{
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												slaves[ii]->write_master_status()	;
								}
				}

				return(0)	;
}

int	Trick::Master::freeze_init()	{

				unsigned	int	ii	;
				Trick::SlaveInfo	*	curr_slave	;

				if	(	enabled	)	{
								/**	@par	Detailed	Design:	*/
								/**	@li	Set	the	connection	sync_wait_limit	to	infinite	as	we	enter	freeze	*/
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												curr_slave	=	slaves[ii]	;
												if	(	curr_slave->activated	==	true	)	{
																curr_slave->connection->set_sync_wait_limit(-1.0)	;
												}
								}
				}
				return(0)	;

}

int	Trick::Master::freeze()	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Sync	with	slave(s)	*/
				end_of_frame_status_from_slave()	;
				end_of_frame_status_to_slave()	;
				return(0)	;
}

int	Trick::Master::checkpoint()	{
				/**	@par	Detailed	Design:	*/
				/**	@li	If	chkpnt_dump_auto,	tell	slave	to	dump	a	checkpoint	*/
				unsigned	int	ii	;
				//	do	not	tell	slave	to	dump	if	this	is	a	pre_init,	post_init,	or	end	checkpoint
				//	those	are	handled	with	flags	sent	to	slave	in	init()
				if	((exec_get_mode()	==	Initialization)	||	(exec_get_mode()	==	ExitMode))	{
								return(0);
				}
				if	(enabled)	{
								//	Use	2	loops	to	read	all	slave	status	before	writing	any	status	out.
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												slaves[ii]->read_slave_status()	;
								}
								SIM_COMMAND	save_command	=	exec_get_exec_command()	;
								std::string	full_path_name	=	checkpoint_get_output_file();
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												if	(slaves[ii]->chkpnt_dump_auto)	{
																exec_set_exec_command((SIM_COMMAND)MS_ChkpntDumpAsciiCmd)	;
																slaves[ii]->write_master_status()	;
																slaves[ii]->write_master_chkpnt_name(full_path_name)	;
																exec_set_exec_command(save_command)	;
												}	else	{	//	no	auto	dump
																slaves[ii]->write_master_status()	;
												}
								}
				}
				return(0)	;
}
int	Trick::Master::preload_checkpoint()	{
				/**	@par	Detailed	Design:	*/
				/**	@li	If	chkpnt_load_auto,	tell	slave	to	load	a	checkpoint	*/
				unsigned	int	ii	;
				if	(enabled)	{
								//	Use	2	loops	to	read	all	slave	status	before	writing	any	status	out.
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												slaves[ii]->read_slave_status()	;
								}
								SIM_COMMAND	save_command	=	exec_get_exec_command()	;
								std::string	full_path_name	=	checkpoint_get_load_file();
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												if	(slaves[ii]->chkpnt_load_auto)	{
																exec_set_exec_command((SIM_COMMAND)MS_ChkpntLoadAsciiCmd)	;
																slaves[ii]->write_master_status()	;
																slaves[ii]->write_master_chkpnt_name(full_path_name)	;
																exec_set_exec_command(save_command)	;
												}	else	{	//	no	auto	load
																slaves[ii]->write_master_status()	;
												}
								}
				}
				return(0)	;
}

int	Trick::Master::unfreeze()	{

				unsigned	int	ii	;
				Trick::SlaveInfo	*	curr_slave	;

				if	(	enabled	)	{
								for	(	ii	=	0	;	ii	<	slaves.size()	;	ii++	)	{
												/**	@par	Detailed	Design:	*/
												/**	@li	Set	the	connection	sync_wait_limit	to	the	default	for	each	slave	*/
												curr_slave	=	slaves[ii]	;
												if	(	curr_slave->activated	==	true	)	{
																curr_slave->connection->set_sync_wait_limit(curr_slave->sync_wait_limit)	;
												}
								}
				}
				return(0)	;

}

int	Trick::Master::shutdown()	{
				/**	@par	Detailed	Design:	*/
				/**	@li	Sync	with	slave(s)	*/
				if	(	enabled	)	{
								end_of_frame_status_from_slave()	;
								end_of_frame_status_to_slave()	;
				}
				return(0)	;
}

/**
	*	@relates	Trick::Master
	*	C	binded	function	to	toggle	the	master/slave	synchronization	flag	to	on.
	*	@return	always	0
	*/
extern	"C"	int	ms_master_enable(void)	{
				the_ms_master->enable()	;
				return(0)	;
}

/**
	*	@relates	Trick::Master
	*	C	binded	function	to	toggle	the	master/slave	synchronization	flag	to	off.
	*	@return	always	0
	*/
extern	"C"	int	ms_master_disable(void)	{
				the_ms_master->disable()	;
				return(0)	;
}
