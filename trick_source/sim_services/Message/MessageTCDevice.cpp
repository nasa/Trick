
#include	<string.h>
#include	<stdlib.h>

#include	"trick/MessageTCDevice.hh"
#include	"trick/message_type.h"
#include	"trick/montecarlo_c_intf.h"
#include	"trick/tc_proto.h"

Trick::MessageTCDeviceListenThread::MessageTCDeviceListenThread(MessageTCDevice	*	in_mtcd)	:
	Trick::SysThread("MessageListen"),
	mtcd(in_mtcd)	,
	listen_dev()	{
				/*	And	a	TCDevice	for	message	server	@e	listen_device	is	configured.	*/
				listen_dev.disabled	=	TC_COMM_FALSE	;
				listen_dev.disable_handshaking	=	TC_COMM_TRUE	;
				listen_dev.blockio_limit	=	0.0	;
				listen_dev.blockio_type	=	TC_COMM_BLOCKIO	;
				listen_dev.client_id	=	0	;
				listen_dev.port	=	0	;
				strcpy(listen_dev.client_tag,	"")	;
				listen_dev.error_handler	=	(TrickErrorHndlr	*)	calloc(1,	(int)sizeof(TrickErrorHndlr));
				listen_dev.error_handler->report_level	=	TRICK_ERROR_CAUTION;
}

Trick::MessageTCDeviceListenThread::~MessageTCDeviceListenThread()	{
				free(listen_dev.error_handler)	;
				listen_dev.error_handler	=	NULL;
				if	(	listen_dev.hostname	)	{
							free((char*)listen_dev.hostname)	;
				}
				close(listen_dev.socket)	;
}

int	Trick::MessageTCDeviceListenThread::get_port()	{
				return	listen_dev.port	;
}

int	Trick::MessageTCDeviceListenThread::init_listen_device()	{
				return	tc_init(&listen_dev);
}

int	Trick::MessageTCDeviceListenThread::restart()	{
				struct	sockaddr_in	s_in;
				int	s_in_size	=		sizeof(s_in)	;
				getsockname(	listen_dev.socket	,	(struct	sockaddr	*)&s_in,	(socklen_t	*)&s_in_size)	;
				printf("restart	message	port	=	%d\n"	,	ntohs(s_in.sin_port))	;
				return	0;
}

void	*	Trick::MessageTCDeviceListenThread::thread_body()	{

	/**	@par	Design	Details:	*/
				int	status	;
				fd_set	rfds;
				struct	timeval	timeout_time	=	{	2,	0	};
				TCDevice	*	new_connection	;

				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,	NULL);

				/**	@li	If	a	proper	port	is	found,	make	a	new	connection	to	the	message	server	that	is	listening	if	possible.
					*						Once	the	connection	is	established,	the	new	connection	is	added	to	the	subscriber	connection	list.
					*/
				while	(1)	{
								FD_ZERO(&rfds);
								FD_SET(listen_dev.socket,	&rfds);
								timeout_time.tv_sec	=	2	;
								select(listen_dev.socket	+	1,	&rfds,	NULL,	NULL,	&timeout_time);

								if	(FD_ISSET(listen_dev.socket,	&rfds))	{
												new_connection	=	new	TCDevice()	;
												new_connection->disabled	=	TC_COMM_FALSE	;
												new_connection->disable_handshaking	=	TC_COMM_TRUE	;
												new_connection->blockio_limit	=	0.0	;
												new_connection->blockio_type	=	TC_COMM_BLOCKIO	;
												new_connection->client_id	=	0	;
												strcpy(new_connection->client_tag,	"")	;
												new_connection->error_handler	=	(TrickErrorHndlr	*)	calloc(1,	(int)sizeof(TrickErrorHndlr));
												new_connection->error_handler->report_level	=	TRICK_ERROR_CAUTION;

												status	=	tc_accept(&listen_dev,	new_connection);

												if	(status	==	TC_SUCCESS)	{
																mtcd->add_connection(new_connection)	;
												}	
								}
				}

				return	NULL	;

}

void	Trick::MessageTCDeviceListenThread::dump(	std::ostream	&	oss	)	{
				oss	<<	"Trick::MessageTCDeviceListenThread	("	<<	name	<<	")\n";
				oss	<<	"				port	=	"	<<	get_port()	<<	std::endl	;
				Trick::ThreadBase::dump(oss)	;
}

/*	---------------------------------------------------------------------------	*/

Trick::MessageTCDevice::MessageTCDevice()	:
	listen_thread(this)
{
				combined_message	=	new	char[20480]	;
				name	=	"tcdevice"	;
				return	;
}

//	DESTRUCTOR
Trick::MessageTCDevice::~MessageTCDevice()	{
				delete[]	combined_message	;
}

Trick::MessageTCDeviceListenThread	&	Trick::MessageTCDevice::get_listen_thread()	{
				return	listen_thread	;
}

void	Trick::MessageTCDevice::add_connection(	TCDevice	*	new_conn	)	{
				connections.push_back(new_conn)	;
}

int	Trick::MessageTCDevice::shutdown()	{
				listen_thread.cancel_thread()	;
				return	0	;
}

int	Trick::MessageTCDevice::default_data()	{

				int	ret	;

				if	(	mc_is_slave()	)	{
								enabled	=	false	;
				}
				/**	@par	Design	Details:	*/
				/**	@li	Reserve	a	listen	socket	*/
				if	(	enabled	)	{
								ret	=	listen_thread.init_listen_device()	;
								if	(	ret	==	TC_SUCCESS	)	{
												port	=	listen_thread.get_port()	;
								}	else	{
												fprintf(stderr,	"ERROR:	Could	not	create	listen	port	for	message	server.		Aborting.\n");
												return	(-1);
								}
				}

				return(0)	;
}

/**
@details
-#	If	this	subscriber	is	enabled,	creates	a	thread	for	a	new	socket	connection
*/
int	Trick::MessageTCDevice::init()	{
				if	(	enabled	)	{
								listen_thread.create_thread()	;
				}
				return(0)	;
}

/**
@details
-#	Call	the	listen	thread	restart	routine
-#	Get	the	port	number	from	the	listen	thread
-#	If	the	listen	thread	has	not	been	created,	call	the	listen	thread	create	thread	method
*/
int	Trick::MessageTCDevice::restart()	{
				if	(	enabled	)	{
								listen_thread.restart()	;
								port	=	listen_thread.get_port()	;
								if	(	listen_thread.get_pthread_id()	==	0	)	{
												listen_thread.create_thread()	;
								}
				}
				return	0	;
}


//	MEMBER	FUNCTION
void	Trick::MessageTCDevice::update(	unsigned	int	level	,	std::string	header	,	std::string	message	)	{

				std::vector	<	TCDevice	*>::iterator	it	;
				int	nbytes	;
				int	length	;
				std::string	color_code	;

				/**	@par	Design	Details:	*/

				switch	(level)	{
								case	MSG_NORMAL	:
												color_code	=	"\033[00m"	;	//	normal
												break	;
								case	MSG_INFO	:
												color_code	=	"\033[32m"	;	//	green
												break	;
								case	MSG_WARNING	:
												color_code	=	"\033[33m"	;	//	yellow
												break	;
								case	MSG_ERROR	:
												color_code	=	"\033[31m"	;	//	red
												break	;
								case	MSG_DEBUG	:
												color_code	=	"\033[36m"	;	//	cyan
												break	;
								default	:
												color_code	=	"\033[00m"	;	//	normal
												break	;
				}

				/**	@li	If	this	subscriber	is	not	enabled,	do	nothing.	Otherwise,	it	gets	the	update.	*/
				if	(	enabled	&&	level	<	100	)	{

						/**	@li	The	message	is	composed	as	message	header	+	message	text	*/
								if	(	color	)	{
												snprintf(	combined_message	,	20480	,	"%s%s%s\033[00m"	,	header.c_str()	,	color_code.c_str()	,	message.c_str())	;
								}	else	{
												snprintf(	combined_message	,	20480	,	"%s%s"	,	header.c_str()	,	message.c_str())	;
								}
								length	=	(int)strlen(combined_message)	;

								/**	@li	The	message	is	broadcast	to	all	socket	connection	this	subscriber	has.	*/
								it	=	connections.begin()	;
								while	(	it	!=	connections.end()	)	{
												nbytes	=	tc_write(	*it,	combined_message,	length)	;

												/**	@li	All	those	connections	that	can	not	receive	the	message	get	disconnected.		*/
												if	(	nbytes	!=	length	)	{
																tc_disconnect(*it)	;
																it	=	connections.erase(it)	;
												}	else	{
																it++	;
												}
								}
				}

				return	;
}

