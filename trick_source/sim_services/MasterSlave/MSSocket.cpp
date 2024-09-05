
#include	<iostream>
#include	<sstream>
#include	<string.h>
#include	<unistd.h>

#include	"trick/MSSocket.hh"
#include	"trick/Master.hh"
#include	"trick/tc_proto.h"
#include	"trick/exec_proto.h"
#include	"trick/command_line_protos.h"

Trick::MSSocket::MSSocket()	:	tc_dev()	{

				tc_dev.disabled	=	TC_COMM_FALSE	;
				tc_dev.error_handler	=	NULL	;
				tc_dev.blockio_type	=	TC_COMM_TIMED_BLOCKIO	;
				tc_dev.blockio_type	=	TC_COMM_BLOCKIO	;
				tc_dev.port	=	0	;	//	tc_multiconnect	needs	this	so	that	it	will	send	broadcast	msg	!

				//	default	is	a	non-zero	sync	wait	limit;	helpful	when	slave	reading	initial	data	from	master
				sync_wait_limit	=	5.0	;
}

int	Trick::MSSocket::set_sync_wait_limit(double	in_limit)	{
				/**	@par	Detailed	Design	*/
				sync_wait_limit	=	in_limit	;
				if	(	in_limit	>	0.0	)	{
								/**	@li	if	the	incoming	limit	time	is	greater	than	zero,	set	the	socket
																to	TC_COMM_TIMED_BLOCKIO	with	the	time	limit	of	in_limit	*/
								tc_blockio(&tc_dev,	TC_COMM_TIMED_BLOCKIO);
								tc_set_blockio_timeout_limit(&tc_dev,	sync_wait_limit);
				}	else	{
								/**	@li	if	the	incoming	limit	time	is	less	than	or	equal	to	zero,	set	the	socket
																to	TC_COMM_BLOCKIO	(infinite	block)	*/
								tc_blockio(&tc_dev,	TC_COMM_BLOCKIO);
				}
				return(0)	;
}

std::string	Trick::MSSocket::add_sim_args(	std::string	slave_type	)	{

				char	master_host[80];
				std::stringstream	temp_stream	;

				/**	@par	Detailed	Design	*/

				/**	@li	create	a	unique	identifier	based	on	the	<machine>_<current	pid>.	*/
				gethostname(master_host,	(size_t)	80);

				temp_stream	<<	master_host	<<	"_"	<<	getpid()	;

				sync_port_tag	=	temp_stream.str()	;

				/**	@li	return	the	unique	identifier	with	the	"-p"	argument	that	is
												specific	to	master/slave	communications.	*/
				return("-p	"	+	temp_stream.str())	;
}

int	Trick::MSSocket::process_sim_args()	{

				int	ii	;
				int	argc	;
				char	**	argv	;

				argc	=	command_line_args_get_argc()	;
				argv	=	command_line_args_get_argv()	;

				/**	@par	Detailed	Design	*/

				/**	@li	search	for	the	"-p"	argument.		If	found	get	the	master	identifier	as	the	next	argument	*/
				for	(ii	=	1;	ii	<	argc;	ii++)	{
								if	(!strncmp("-p",	argv[ii],	(size_t)	2))	{
												if	(argc	>=	ii	+	1)	{
																sync_port_tag	=	argv[ii+1]	;
																return(1)	;
												}
								}
				}

				return(0)	;
}

int	Trick::MSSocket::accept()	{

				int	ret	;
				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_multiconnect	to	connect	this	master.		Use	"master"	as	the	my_tag	argument	*/
				ret	=	tc_multiconnect(&tc_dev,	(char	*)"master",	(char	*)sync_port_tag.c_str(),	NULL);

				return(ret)	;
}

int	Trick::MSSocket::connect()	{
				int	ret	;
				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_multiconnect	to	connect	this	slave.		Use	"slave"	as	the	my_tag	argument	*/
				ret	=	tc_multiconnect(&tc_dev,	(char	*)"slave",	(char	*)sync_port_tag.c_str(),	NULL);

				return(ret)	;
}

int	Trick::MSSocket::disconnect()	{
				int	ret;
				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_disconnect	to	disconnect	this	device.	*/
				ret	=	tc_disconnect(	&tc_dev	);

				tc_dev.disabled	=	TC_COMM_FALSE	;
				tc_dev.error_handler	=	NULL	;
				tc_dev.blockio_type	=	TC_COMM_TIMED_BLOCKIO	;
				tc_dev.blockio_type	=	TC_COMM_BLOCKIO	;
				tc_dev.port	=	0	;

				return(ret);
}

long	long	Trick::MSSocket::read_time()	{

				long	long	in_time	=	0	;
				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_read	to	get	the	time	*/
				ret	=	tc_read(&tc_dev	,	(char	*)&in_time,	sizeof(long	long));

				/**	@li	If	the	read	call	returned	the	correct	number	of	bytes	return	the	read	in	time	*/
				if	(	ret	==	sizeof(long	long))	{
								return(in_time)	;
				}
				/**	@li	Else	return	the	special	"error"	time	*/
				return	(MS_ERROR_TIME)	;
}

MS_SIM_COMMAND	Trick::MSSocket::read_command()	{

				MS_SIM_COMMAND	command	;
				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_read	to	get	the	command	*/
				ret	=	tc_read(&tc_dev	,	(char	*)&command,	sizeof(MS_SIM_COMMAND));

				/**	@li	If	the	read	call	returned	the	correct	number	of	bytes	return	the	read	in	command	*/
				if	(	ret	==	sizeof(MS_SIM_COMMAND))	{
								return(command)	;
				}

				/**	@li	Else	return	ErrorCmd		*/
				return(MS_ErrorCmd)	;
}

int	Trick::MSSocket::read_port()	{

				int	in_port	=	0	;
				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_read	to	get	the	port	number	*/
				ret	=	tc_read(&tc_dev	,	(char	*)&in_port,	sizeof(int));

				/**	@li	If	the	read	call	returned	the	correct	number	of	bytes	return	the	read	in	port	*/
				if	(	ret	==	sizeof(int))	{
								return(in_port)	;
				}
				/**	@li	Else	return	the	special	"error"	port	*/
				return	(MS_ERROR_PORT)	;
}

char	Trick::MSSocket::read_name(char	*	read_data,	size_t	size)	{

				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_read	to	get	the	name	(character	array)	*/
				ret	=	tc_read(&tc_dev	,	(char	*)read_data,	(int)size);

				/**	@li	If	the	read	call	returned	the	correct	number	of	bytes	return	1st	character	read	*/
				if	(	ret	==	int(size))	{
								return(read_data[0])	;
				}
				/**	@li	Else	return	the	special	"error"	name	character	*/
				return	(MS_ERROR_NAME)	;
}

int	Trick::MSSocket::write_time(long	long	in_time)	{

				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_write	to	write	the	time	*/
				ret	=	tc_write(&tc_dev	,	(char	*)&in_time,	sizeof(long	long));

				/**	@li	Return	the	number	of	bytes	written	*/
				return(ret)	;
}

int	Trick::MSSocket::write_command(MS_SIM_COMMAND	command)	{

				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_write	to	write	the	command	*/
				ret	=	tc_write(&tc_dev	,	(char	*)&command,	sizeof(unsigned	int));

				/**	@li	Return	the	number	of	bytes	written	*/
				return(ret)	;
}

int	Trick::MSSocket::write_port(int	in_port)	{

				int	ret	;

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_write	to	write	the	port	number	*/
				ret	=	tc_write(&tc_dev	,	(char	*)&in_port,	sizeof(int));

				/**	@li	Return	the	number	of	bytes	written	*/
				return(ret)	;
}

int	Trick::MSSocket::write_name(char	*	in_data,	size_t	size)	{

				/**	@par	Detailed	Design	*/
				/**	@li	Call	tc_write	to	write	the	name	(character	array)	*/
				tc_write(&tc_dev	,	(char	*)in_data,	size);

				/**	@li	Return	the	number	of	bytes	written	*/
				return(size)	;
}
