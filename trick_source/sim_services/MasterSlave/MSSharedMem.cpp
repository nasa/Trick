
#include	<iostream>
#include	<sstream>
#include	<cstring>	//	for	memcpy
#include	<unistd.h>

#include	"trick/MSSharedMem.hh"
#include	"trick/tsm_proto.h"
#include	"trick/command_line_protos.h"

Trick::MSSharedMem::MSSharedMem()	:	tsm_dev()	{
				tsm_dev.default_val	=	-1;

				//	default	is	a	non-zero	sync	wait	limit;	helpful	when	slave	reading	initial	data	from	master
				sync_wait_limit	=	5.0	;
}

Trick::MSSharedMem::~MSSharedMem()	{
				//	detach	shared	memory
				tsm_disconnect(&tsm_dev);
}

int	Trick::MSSharedMem::set_sync_wait_limit(double	in_limit)	{
				/**	@par	Detailed	Design	*/
				if	(	in_limit	>	0.0	)	{
								/**	@li	if	the	incoming	limit	time	is	greater	than	zero,	we	will	use	it
																as	the	timeout	value	during	read	and	write	*/
								sync_wait_limit	=	in_limit	;
				}	else	{
								/**	@li	if	the	incoming	limit	time	is	less	than	or	equal	to	zero,	wait	forever	*/
								sync_wait_limit	=	TSM_MAX_TIMEOUT_LIMIT;
				}
				return(0)	;
}

std::string	Trick::MSSharedMem::add_sim_args(std::string	slave_type	__attribute__((unused)))	{

				/**	@par	Detailed	Design	*/

				/**	@li	nothing	to	do	here	for	shared	memory.	*/
				return("");
}

int	Trick::MSSharedMem::process_sim_args()	{

				//int	ii	;
				//int	argc	;
				//char	**	argv	;

				//argc	=	command_line_args_get_argc()	;
				//argv	=	command_line_args_get_argv()	;

				/**	@par	Detailed	Design	*/

				/**	@li	no	shared	memory	arguments	to	search	for,	return	1	to	enable	slave	*/
				//for	(ii	=	2;	ii	<	argc;	ii++)	{
				//}
				return(1)	;
}

int	Trick::MSSharedMem::accept()	{

				int	ret	;
				/**	@par	Detailed	Design	*/
				/**	@li	Call	tsm_init	to	create	shared	memory	for	master.	*/
				tsm_dev.size	=	sizeof(MSSharedMemData);
				ret	=	tsm_init(&tsm_dev);
				shm_addr	=	(MSSharedMemData*)	tsm_dev.addr;
				/**	@li	Save	master	process	id	so	we	can	keep	master	and	slave	data	seperate.	*/
				//	(the	master	calls	accept)
				if	(ret==TSM_SUCCESS)	{
								shm_addr->master_pid	=	getpid();
//fprintf(stderr,	"====accept	master=%d\n",	getpid());
								MSQ_INIT(shm_addr->master_time);
								MSQ_INIT(shm_addr->master_command);
								MSQ_INIT(shm_addr->slave_command);
								shm_addr->slave_port	=	MS_ERROR_PORT;
								shm_addr->chkpnt_name[0]	=	MS_ERROR_NAME;
				}	else	{
//fprintf(stderr,	"====accept	SHARED	MEMORY	ERROR\n");
				}
				return(ret)	;
}

int	Trick::MSSharedMem::connect()	{
				int	ret	;
				/**	@par	Detailed	Design	*/
				/**	@li	Call	tsm_init	to	create	shared	memory	for	slave.	*/
				if	(tsm_dev.size	==	0)	{
								tsm_dev.size	=	sizeof(MSSharedMemData);
								ret	=	tsm_init(&tsm_dev);
				}	else	{
								ret	=	tsm_reconnect(&tsm_dev);
				}
				shm_addr	=	(MSSharedMemData*)	tsm_dev.addr;
				return(ret)	;
}

int	Trick::MSSharedMem::disconnect()	{
				return(0)	;
}


double	Trick::MSSharedMem::read_wait(struct	timespec	*in_start)	{
				struct	timespec	ts_Current,	ts_Difference;

				RELEASE();
				clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Current);
				if	((ts_Current.tv_nsec	-	in_start->tv_nsec)	<	0)	{
								ts_Difference.tv_sec	=	ts_Current.tv_sec	-	in_start->tv_sec	-	1;
								ts_Difference.tv_nsec	=	1000000000	+	ts_Current.tv_nsec	-	in_start->tv_nsec;
				}	else	{
								ts_Difference.tv_sec	=	ts_Current.tv_sec	-	in_start->tv_sec;
								ts_Difference.tv_nsec	=	ts_Current.tv_nsec	-	in_start->tv_nsec;
				}
				//	return	the	total	elapsed	time	we've	waited	since	in_start
				return	((double)ts_Difference.tv_nsec	/	1000000000.0)	+	ts_Difference.tv_sec;
}

long	long	Trick::MSSharedMem::read_time()	{

				long	long	in_time;
				double	readtry_time	=	0.0;
				struct	timespec	ts_Start;

				/**	@par	Detailed	Design	*/
				clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Start);

				/**	@li	Get	time	from	shared	memory	*/
				while	(MSQ_ISEMPTY(shm_addr->master_time))	{
								if	(readtry_time	>=	sync_wait_limit)	{
												break;
								}
								readtry_time	=	read_wait(&ts_Start);
				}
				if	(!MSQ_ISEMPTY(shm_addr->master_time))	{
								in_time	=	MSQ_FRONT(shm_addr->master_time);
//fprintf(stderr,	"+++++read_time	pid=%d	time=%lld	readtry=%f\n",	getpid(),	in_time,	readtry_time);
								MSQ_POP(shm_addr->master_time);
								return	(in_time)	;
				/**	@li	If	no	new	data	before	timeout	limit,	return	"error"	time	*/
				}	else	{
//fprintf(stderr,	"+++++read_time	pid=%d	time=%lld	ERROR	readtry=%f\n",	getpid(),	in_time,	readtry_time,);	fflush(stderr);
								return	(MS_ERROR_TIME)	;
				}
}

MS_SIM_COMMAND	Trick::MSSharedMem::read_command()	{

				MS_SIM_COMMAND	command;
				double	readtry_time	=	0.0;
				struct	timespec	ts_Start;

				/**	@par	Detailed	Design	*/
				clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Start);

				if	(getpid()	==	shm_addr->master_pid)	{
				/**	@li	Get	slave	command	from	shared	memory	*/
				//	I	am	master,	so	read	slave	command
								while	(MSQ_ISEMPTY(shm_addr->slave_command))	{
												if	(readtry_time	>=	sync_wait_limit)	{
																break;
												}
												readtry_time	=	read_wait(&ts_Start);
								}
								if	(!MSQ_ISEMPTY(shm_addr->slave_command))	{
												command	=	MSQ_FRONT(shm_addr->slave_command);
//fprintf(stderr,	"+++++read_command	pid=%d	command=%d	(from	slave)\n",	getpid(),	command);
												MSQ_POP(shm_addr->slave_command);
												return	(command)	;
								/**	@li	If	no	new	data	before	timeout	limit,	return	"error"	command	*/
								}	else	{
//fprintf(stderr,	"+++++read_command	pid=%d	command=%d	(from	slave)	ERROR	readtry=%f\n",	getpid(),	command,	readtry_time);
												return	(MS_ErrorCmd)	;
								}
				}	else	{
				/**	@li	Get	master	command	from	shared	memory	*/
				//	I	am	slave,	so	read	master	command
								while	(MSQ_ISEMPTY(shm_addr->master_command))	{
												if	(readtry_time	>=	sync_wait_limit)	{
																break;
												}
												readtry_time	=	read_wait(&ts_Start);
								}
								if	(!MSQ_ISEMPTY(shm_addr->master_command))	{
												command	=	MSQ_FRONT(shm_addr->master_command);
//fprintf(stderr,	"+++++read_command	pid=%d	command=%d	(from	master)\n",	getpid(),	command);
												MSQ_POP(shm_addr->master_command);
												return	(command)	;
								/**	@li	If	no	new	data	before	timeout	limit,	return	"error"	command	*/
								}	else	{
//fprintf(stderr,	"+++++read_command	pid=%d	command=%d	(from	master)	ERROR	readtry=%f\n",	getpid(),	command,	readtry_time);
												return	(MS_ErrorCmd)	;
								}
				}
}

int	Trick::MSSharedMem::read_port()	{

				int	in_port;
				double	readtry_time	=	0.0;
				struct	timespec	ts_Start;

				/**	@par	Detailed	Design	*/
				clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Start);

				/**	@li	Get	port	number	from	shared	memory	*/
				while	(shm_addr->slave_port	==	MS_ERROR_PORT)	{
								if	(readtry_time	>=	sync_wait_limit)	{
												break;
								}
								readtry_time	=	read_wait(&ts_Start);
				}
				if	(shm_addr->slave_port	!=	MS_ERROR_PORT)	{
								in_port	=	shm_addr->slave_port;
								shm_addr->slave_port	=	MS_ERROR_PORT;	//	default	value
								return	(in_port)	;
				/**	@li	If	no	new	data	before	timeout	limit,	return	"error"	port	*/
				}	else	{
								return	(MS_ERROR_PORT)	;
				}
}

char	Trick::MSSharedMem::read_name(char	*	read_data,	size_t	size)	{

				double	readtry_time	=	0.0;
				struct	timespec	ts_Start;

				/**	@par	Detailed	Design	*/
				clock_gettime(CLOCK_THREAD_CPUTIME_ID,	&ts_Start);

				/**	@li	Get	name	(character	array)	from	shared	memory	*/
				while	(shm_addr->chkpnt_name[0]	==	MS_ERROR_NAME)	{
								if	(readtry_time	>=	sync_wait_limit)	{
												break;
								}
								readtry_time	=	read_wait(&ts_Start);
				}
				if	(shm_addr->chkpnt_name[0]	!=	MS_ERROR_NAME)	{
								memcpy(read_data,	shm_addr->chkpnt_name,	size);
								shm_addr->chkpnt_name[0]	=	MS_ERROR_NAME;	//	default	value
								return	(0)	;
				/**	@li	If	no	new	data	before	timeout	limit,	return	"error"	name	*/
				}	else	{
								return	(MS_ERROR_NAME)	;
				}
}

int	Trick::MSSharedMem::write_time(long	long	in_time)	{

				/**	@par	Detailed	Design	*/
				/**	@li	Write	time	to	shared	memory	*/
//fprintf(stderr,	"====write_time	pid=%d	time=%lld\n",	getpid(),	in_time);
				MSQ_PUSH(shm_addr->master_time,	in_time);

				/**	@li	Return	the	number	of	bytes	written	*/
				return(sizeof(long	long))	;
}

int	Trick::MSSharedMem::write_command(MS_SIM_COMMAND	command)	{

				/**	@par	Detailed	Design	*/
				/**	@li	Write	command	to	shared	memory	*/
				if	(getpid()	==	shm_addr->master_pid)	{
//fprintf(stderr,	"====write_command	pid=%d	command=%d	(master)\n",	getpid(),	command);
								MSQ_PUSH(shm_addr->master_command,	command);
				}	else	{
//fprintf(stderr,	"====write_command	pid=%d	command=%d	(slave)\n",	getpid(),	command);
								MSQ_PUSH(shm_addr->slave_command,	command);
				}

				/**	@li	Return	the	number	of	bytes	written	*/
				return(sizeof(MS_SIM_COMMAND))	;
}

int	Trick::MSSharedMem::write_port(int	in_port)	{

				/**	@par	Detailed	Design	*/
				/**	@li	Write	port	number	to	shared	memory	*/
				shm_addr->slave_port	=	in_port;

				/**	@li	Return	the	number	of	bytes	written	*/
				return(sizeof(int))	;
}

int	Trick::MSSharedMem::write_name(char	*	in_data,	size_t	size)	{

				/**	@par	Detailed	Design	*/
				/**	@li	Write	name	(character	array)	to	shared	memory	*/
				memcpy(shm_addr->chkpnt_name,	in_data,	size);

				/**	@li	Return	the	number	of	bytes	written	*/
				return(size);
}
