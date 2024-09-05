/*
			PURPOSE:	(Slave	for	master/slave	synchronization)
	*/

#include	<iostream>
#include	<iomanip>	//	for	setprecision
#include	<sstream>
#include	<dlfcn.h>
#include	<stdlib.h>	//	for	getenv
#include	<cstring>

#include	"trick/Slave.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/CheckPointRestart_c_intf.hh"	//	for	checkpoint
#include	"trick/command_line_protos.h"	//	output	dir	get/set

Trick::Slave::Slave()	{
				enabled	=	false	;
				reconnected	=	false	;
				activated	=	false;
				msg_published	=	false;
				sent_reconnect_cmd	=	false;
}

int	Trick::Slave::set_connection_type(Trick::MSConnect	*	in_connection)	{
				connection	=	in_connection	;
				return	0	;
}

int	Trick::Slave::process_sim_args()	{
				/**	@par	Detailed	Design	*/
				if	(	connection	!=	NULL	)	{
								/**	@li	the	return_value	of	Trick::MSConnect::process_sim_args()	sets	the	enabled	flag	for	the	slave.	*/
								enabled	=	connection->process_sim_args()	;
				}

				return(0)	;
}

int	Trick::Slave::init()	{

				std::string	rts_disable_name	;
				std::string	ms_master_disable_name	;
				void*	dlhandle	;
				int	(*rts_disable)(void)	=	NULL	;
				int	(*ms_master_disable)(void)	=	NULL	;
				long	long	software_frame_tics	;
				long	long	sync_wait_limit_tics	;
				int	chkpnt_flag;

				/**	@par	Detailed	Design	*/

				if	(	enabled	)	{

								/**	@li	Connect	to	the	master	by	calling	Trick::MSConnect::connect()	*/
								connection->connect()	;

								dlhandle	=	dlopen(	NULL,	RTLD_LAZY)	;

								/**	@li	Turn	off	RealtimeSync	if	it	exists.		We	are	not	assuming	that	the	function	even
																exists.		Search	for	the	routine	and	execute	it	if	it	exists	*/
								rts_disable_name	=	"real_time_disable"	;
								rts_disable	=	(int	(*)(void))dlsym(	dlhandle	,	rts_disable_name.c_str())	;
								if	(	rts_disable	!=	NULL	)	{
												message_publish(MSG_INFO	,	"Slave	synchronization	starting.		Turning	realtime	synchronization	off.\n")	;
												(*rts_disable)()	;
								}

								/**	@li	Turn	off	Master	synchronization	if	it	exists.		We	are	not	assuming	that	the	function	even
																exists.		Search	for	the	routine	and	execute	it	if	it	exists	*/
								ms_master_disable_name	=	"ms_master_disable"	;
								ms_master_disable	=	(int	(*)(void))dlsym(	dlhandle	,	ms_master_disable_name.c_str())	;
								if	(	ms_master_disable	!=	NULL	)	{
												message_publish(MSG_INFO	,	"Slave	synchronization	starting.		Turning	master	off.\n")	;
												(*ms_master_disable)()	;
								}

								/**	@li	Read	and	set	the	slave	software	frame	and	time	tic	value	according	to	the	master.	*/
								software_frame_tics	=	connection->read_time()	;
								exec_set_time_tic_value	(connection->read_time());
								exec_set_software_frame	(double	(software_frame_tics)	/	double(exec_get_time_tic_value())	)	;
								/**	@li	Read	and	set	the	sync_wait_limit	according	to	the	master.	*/
								sync_wait_limit_tics	=	connection->read_time()	;
								sync_wait_limit	=	double	(sync_wait_limit_tics)	/	double(exec_get_time_tic_value())	;
								connection->set_sync_wait_limit(sync_wait_limit)	;

								/**	@li		Read	the	initial	freeze	command	from	the	master.	*/
								exec_set_freeze_command(int(connection->read_command()))	;
								/**	@li	Read	and	set	the	pre_init,	post_init,	and	end	checkpoint	flags	according	to	the	master.	*/
								chkpnt_flag	=	(int)connection->read_time();
								checkpoint_pre_init(chkpnt_flag>>2	&	0x1);
								checkpoint_post_init(chkpnt_flag>>1	&	0x1);
								checkpoint_end(chkpnt_flag	&	0x1);

								dlclose(dlhandle)	;

								//	Executive	freezes	are	only	allowed	on	freeze	frame	boundaries	in	Master/Slave
								exec_set_freeze_on_frame_boundary(true)	;

								activated	=	true;
				}

				return(0)	;
}

std::string	Trick::Slave::get_checkpoint_name(MS_SIM_COMMAND	command)	{

				std::string	dir;
				std::stringstream	file_name_stream	;
				/**	@par	Detailed	Design	*/
				/**	@li	Read	the	checkpoint	name	from	master,	it's	in	the	form	"dir/filename"	*/
				connection->read_name(chkpnt_name,	sizeof(chkpnt_name));	//	dir/filename
				/**	@li	Return	the	appropriate	checkpoint	name	to	use	for	given	dump/load	command	*/
				//	ascii	checkpoint()	only	wants	a	filename	--	no	dir	path
				if	(command	==	MS_ChkpntDumpAsciiCmd)	{
								if	(chkpnt_name[0]	!=	MS_ERROR_NAME)	{
												file_name_stream	<<	strrchr(chkpnt_name,	'/')+1;	//	filename	only
												//std::cout	<<	"---->	Slave:	parsed	checkpoint	file	name:	"	<<	file_name_stream.str()	<<	std::endl;
								}	else	{	//	create	default	name
												file_name_stream	<<	"chkpnt_"	<<	std::fixed	<<	std::setprecision(2)	<<	exec_get_sim_time()	;
								}
				}
				//	ascii	load_checkpoint()	wants	the	dir/filename	path
				if	(command	==	MS_ChkpntLoadAsciiCmd)	{
								dir	=	command_line_args_get_output_dir();	//	run	dir
								if	(chkpnt_name[0]	!=	MS_ERROR_NAME)	{
												file_name_stream	<<	dir	<<	std::string(strrchr(chkpnt_name,	'/'));	//	my	run	dir	/	filename
								}	else	{	//	create	default	name
												file_name_stream	<<	dir	<<	"/chkpnt_"	<<	std::fixed	<<	std::setprecision(2)	<<	exec_get_sim_time()	;
								}
				}

				return(file_name_stream.str())	;
}

int	Trick::Slave::end_of_frame()	{

				long	long	master_time	;
				MS_SIM_COMMAND	command	;
				MS_SIM_COMMAND	slave_command	;
				std::string	chkpt_name_str;

				/**	@par	Detailed	Design	*/
				if	(	(enabled)	and	(activated)	){

								/**	@li	write	the	current	slave	exec_command	to	the	master	*/
								slave_command	=	(MS_SIM_COMMAND)exec_get_exec_command();

								//	send	a	special	command	so	master	knows	to	print	a	"we	reconnected"	message
								if	(reconnected)	{
												slave_command	=	MS_ReconnectCmd;

												if	(msg_published)
																reconnected	=	false;
												else
																sent_reconnect_cmd	=	true;
								}

								//printf("DEBUG	slave	write	%d	command	to	master\n",	slave_command);	fflush(stdout);
								connection->write_command(slave_command)	;

								/**	@li	read	the	simulation	time	according	to	the	master	*/
								master_time	=	connection->read_time()	;

								if	(	master_time	==	MS_ERROR_TIME	)	{

												if	(	sync_error_terminate	==	true	)	{
																/**	@li	if	reading	the	master	time	returned	an	error	exit	the	sim	if	sync_error_terminate	==	true	*/
																message_publish(MSG_ERROR	,	"Slave	lost	sync	with	master.	sync_error_terminate	is	true:	Slave	is	terminating.\n")	;
																exec_terminate_with_return(-1,	__FILE__,	__LINE__	,	"Slave	lost	sync	with	master")	;
												}
												else	{
																if	(slave_command	!=	MS_FreezeCmd){
																				/**	@li	if	reading	the	master	time	returned	an	error	freeze	the	sim	if	sync_error_terminate	==	false	*/
																				activated	=	false;
																				message_publish(MSG_ERROR	,	"Slave	lost	sync	with	master.	sync_error_terminate	is	false:	Slave	is	entering	Freeze	mode.\n")	;
																				exec_set_exec_command(FreezeCmd)	;
																				connection->write_command(MS_FreezeCmd)	;
																				return(0);
																}
												}
								}

								/**	@li	read	the	master	mode	command	*/
								command	=	connection->read_command()	;
								//printf("DEBUG	slave	read	%d	command	from	master\n",	command);	fflush(stdout);

								switch	(	command	)	{
												case	(MS_ErrorCmd):
																if	(	sync_error_terminate	==	true	)	{
																				/**	@li	if	reading	the	master	mode	command	returned	an	error	exit	the	sim	if	sync_error_terminate	==	true	*/
																				message_publish(MSG_ERROR	,	"Slave	lost	sync	with	master.	sync_error_terminate	is	true:	Slave	is	terminating.\n")	;
																				exec_terminate_with_return(-1,	__FILE__,	__LINE__	,	"Slave	lost	sync	with	master")	;
																}
																else	{
																				if	(slave_command	!=	MS_FreezeCmd)	{
																								/**	@li	if	reading	the	master	mode	command	returned	an	error	freeze	the	sim	if	sync_error_terminate	==	false	*/
																								activated	=	false;
																								message_publish(MSG_ERROR	,	"Slave	lost	sync	with	master.	sync_error_terminate	is	false:	Slave	is	entering	Freeze	mode.\n")	;
																								exec_set_exec_command(FreezeCmd)	;
																								connection->write_command(MS_FreezeCmd)	;
																								return(0);
																				}
																}
																break;
												/**	@li	if	reading	the	master	mode	command	returned	a	checkpoint	dump	command,	dump	a	checkpoint	*/
												case	(MS_ChkpntDumpAsciiCmd):			//	Master	tells	slave	to	dump	an	ascii	checkpoint
																message_publish(MSG_WARNING	,	"Slave	received	Checkpoint	Dump	command	from	master.\n")	;
																chkpt_name_str	=	get_checkpoint_name(MS_ChkpntDumpAsciiCmd);
																checkpoint(chkpt_name_str.c_str());
																break;
												/**	@li	if	reading	the	master	mode	command	returned	a	checkpoint	load	command,	load	a	checkpoint	*/
												case	(MS_ChkpntLoadAsciiCmd):		//	Master	tells	slave	to	load	an	ascii	checkpoint
																message_publish(MSG_WARNING	,	"Slave	received	Checkpoint	Load	command	from	master.\n")	;
																chkpt_name_str	=	get_checkpoint_name(MS_ChkpntLoadAsciiCmd);
																load_checkpoint(chkpt_name_str.c_str());	//	load	done	in	freeze	or	end_of_frame	job
																//load_checkpoint_job();	//	do	the	load	NOW
																break;
												default:
																/**	@li	if	reading	the	master	mode	command	returned	an	Executive	mode,	set	the	slave	mode	command	to	the	master	mode	command	*/
																exec_set_exec_command((SIM_COMMAND)command)	;
																if	(reconnected)	{
																				message_publish(MSG_INFO	,	"Slave	has	reconnected	to	master.\n")	;
																				if	(sent_reconnect_cmd)
																								reconnected	=	false;
																				else
																								msg_published	=	true;
																}
																break;
								}
				}

				return(0)	;
}

int	Trick::Slave::freeze_init()	{
				/**	@par	Detailed	Design	*/
				if	(	enabled	)	{
								/**	@li	Set	the	connection	sync_wait_limit	to	infinite	upon	entering	freeze	*/
								connection->set_sync_wait_limit(-1.0)	;
				}
				return(0)	;
}

int	Trick::Slave::freeze()	{
				/**	@par	Detailed	Design	*/
				/**	@li	Call	Trick::Slave::end_of_frame()	*/

				if	(	enabled	)	{
								exec_set_exec_command(NoCmd)	;	//	so	master	won't	keep	reading	freeze	command	during	freeze
								end_of_frame()	;
				}
				return(0)	;
}

int	Trick::Slave::unfreeze()	{
				/**	@par	Detailed	Design	*/
				if	(	enabled	)	{
								/**	@li	Set	the	connection	sync_wait_limit	to	the	default	upon	exiting	freeze	*/
								connection->set_sync_wait_limit(sync_wait_limit)	;
				}
				return(0)	;
}

int	Trick::Slave::shutdown()	{
				/**	@par	Detailed	Design	*/
				if	(	enabled	)	{
								/**	@li	write	the	exit	mode	command	to	the	master	when	the	slave	is	shutting	down	*/
								connection->write_command(MS_ExitCmd)	;
				}
				return(0)	;
}

