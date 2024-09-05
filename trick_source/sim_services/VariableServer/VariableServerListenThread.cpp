
#include	<iostream>
#include	<pwd.h>

#include	"trick/VariableServerListenThread.hh"
#include	"trick/VariableServerSessionThread.hh"
#include	"trick/exec_proto.h"
#include	"trick/command_line_protos.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

#define	MAX_MACHINE_NAME	80


Trick::VariableServerListenThread::VariableServerListenThread()	:	VariableServerListenThread	(NULL)	{}

Trick::VariableServerListenThread::VariableServerListenThread(TCPClientListener	*	listener)	:
	Trick::SysThread("VarServListen"),
	_requested_source_address(""),
	_requested_port(0),
	_user_requested_address(false),
	_broadcast(true),
	_listener(listener),
	_multicast(new	MulticastGroup())
{
				if	(_listener	!=	NULL)	{
								//	If	we	were	passed	a	listener
								//	We	assume	it	is	already	initialized
								_requested_source_address	=	_listener->getHostname();
								_requested_port	=	_listener->getPort();
								_user_requested_address	=	true;
				}	else	{
								//	Otherwise,	make	one
								_listener	=	new	TCPClientListener;
				}

				allowConnections	=	true;
				pendingConnections	=	0;
				pthread_mutex_init(	&connectionMutex,	NULL);
				pthread_cond_init(	&noPendingConnections_cv,	NULL);

				cancellable	=	false;
}

Trick::VariableServerListenThread::~VariableServerListenThread()	{
				delete	_listener;
				delete	_multicast;
}

void	Trick::VariableServerListenThread::set_multicast_group	(MulticastGroup	*	group)	{
				delete	_multicast;
				_multicast	=	group;
}

const	char	*	Trick::VariableServerListenThread::get_hostname()	{
				std::string	hostname	=	_requested_source_address;
				char	*	ret	=	(char	*)	malloc(hostname.length()	+	1);
				strncpy(ret,	hostname.c_str(),	hostname.length());
				ret[hostname.length()]	=	'\0';
				return	ret;
}

unsigned	short	Trick::VariableServerListenThread::get_port()	{
				return	_requested_port;
}

void	Trick::VariableServerListenThread::set_port(unsigned	short	in_port)	{
				_requested_port	=	in_port;
				_user_requested_address	=	true	;
}

std::string	Trick::VariableServerListenThread::get_user_tag()	{
				return	_user_tag	;
}

const	std::string&	Trick::VariableServerListenThread::get_user_tag_ref()	{
				return	_user_tag	;
}

void	Trick::VariableServerListenThread::set_user_tag(std::string	in_tag)	{
				_user_tag	=	in_tag	;
}

void	Trick::VariableServerListenThread::set_source_address(const	char	*	address)	{
				if	(	address	==	NULL	)	{
								_requested_source_address	=	std::string("")	;
				}	else	{
								_requested_source_address	=	std::string(address)	;
				}
				_user_requested_address	=	true;
}

std::string	Trick::VariableServerListenThread::get_source_address()	{
				return	_requested_source_address	;
}

bool	Trick::VariableServerListenThread::get_broadcast()	{
				return	_broadcast;
}

void	Trick::VariableServerListenThread::set_broadcast(bool	in_broadcast)	{
				_broadcast	=	in_broadcast;
}

//	Called	from	default	data
int	Trick::VariableServerListenThread::init_listen_device()	{
				int	ret	=	_listener->initialize();
				_requested_port	=	_listener->getPort();
				_requested_source_address	=	_listener->getHostname();
				return	ret;
}

//	Called	from	init	jobs
int	Trick::VariableServerListenThread::check_and_move_listen_device()	{
				int	ret	;

				if	(_user_requested_address)	{
								/*	The	user	has	requested	a	different	source	address	or	port	in	the	input	file	*/
								_listener->disconnect();
								ret	=	_listener->initialize(_requested_source_address,	_requested_port);
								_requested_port	=	_listener->getPort();
								_requested_source_address	=	_listener->getHostname();
								if	(ret	!=	0)	{
												message_publish(MSG_ERROR,	"ERROR:	Could	not	establish	variable	server	source_address	%s:	port	%d.	Aborting.\n",
																_requested_source_address.c_str(),	_requested_port);
												return	-1	;
								}
				}
				return	0	;
}

void	*	Trick::VariableServerListenThread::thread_body()	{
				//	This	thread	listens	for	incoming	client	connections,	and	when	one	is	received,	creates	a	new	thread	to	handle	the	session
				//	Also	broadcasts	on	multicast	channel

				test_shutdown();


				std::string	version	=	std::string(exec_get_current_version())	;
				version.erase(version.find_last_not_of("	\t\f\v\n\r")+1);

				//	get	username	to	_broadcast	on	multicast	channel
				struct	passwd	*passp	=	getpwuid(getuid())	;
				std::string	user_name;
				if	(	passp	==	NULL	)	{
								user_name	=	strdup("unknown")	;
				}	else	{
								user_name	=	strdup(passp->pw_name)	;
				}

				_listener->setBlockMode(true);

				if	(	_broadcast	)	{
								initializeMulticast();
				}

				while	(1)	{
								//	Quit	here	if	it's	time
								test_shutdown();

								//	Pause	here	if	we	need	to
								test_pause();

								//	Look	for	a	new	client	requesting	a	connection
								if	(_listener->checkForNewConnections())	{

												//	Create	a	new	thread	to	service	this	connection
												if	(	allowConnections)	{
																pthread_mutex_lock(&connectionMutex);
																pendingConnections	++;

																VariableServerSessionThread	*	vst	=	new	Trick::VariableServerSessionThread()	;
																vst->set_connection(_listener->setUpNewConnection());
																vst->copy_cpus(get_cpus())	;
																vst->create_thread()	;
																ConnectionStatus	status	=	vst->wait_for_accept()	;

																if	(status	==	CONNECTION_FAIL)	{
																				//	If	the	connection	failed,	the	thread	will	exit.
																				//	Make	sure	it	joins	fully	before	deleting	the	vst	object
																				vst->join_thread();
																				delete	vst;
																}
																pendingConnections	--;
																if	(	pendingConnections	==	0	)	{
																				pthread_cond_signal(	&noPendingConnections_cv	);
																}
																pthread_mutex_unlock(&connectionMutex);
												}
												
								}	else	if	(	_broadcast	)	{
												//	Otherwise,	broadcast	on	the	multicast	channel	if	enabled
												char	buf1[1024];
												snprintf(buf1	,	sizeof(buf1),	"%s\t%hu\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%hu\n"	,	_listener->getHostname().c_str(),	(unsigned	short)_listener->getPort()	,
													user_name.c_str()	,	(int)getpid()	,	command_line_args_get_default_dir()	,	command_line_args_get_cmdline_name()	,
													command_line_args_get_input_file()	,	version.c_str()	,	_user_tag.c_str(),	(unsigned	short)_listener->getPort()	)	;

												std::string	message(buf1);

												if	(!_multicast->isInitialized())	{
																//	In	case	broadcast	was	turned	on	after	this	loop	was	entered
																initializeMulticast();
												}
												_multicast->broadcast(message);
								}
				}

				return	NULL	;
}

void	Trick::VariableServerListenThread::shutdownConnections()	{
				pthread_mutex_lock(&connectionMutex);
								allowConnections	=	false;
								//		if	ANY	connections	are	pending,	then	wait	here	until	we’re	notified	that	NO	connections	are	pending.
								if	(pendingConnections	>	0)	{
												allowConnections	=	true;
												pthread_cond_wait(	&noPendingConnections_cv,	&connectionMutex);
								}
				pthread_mutex_unlock(	&connectionMutex	);
}

#include	<fcntl.h>

int	Trick::VariableServerListenThread::restart()	{

				int	ret	;

				_listener->restart();

				if	(	_user_requested_address	)	{
								//	If	the	use	requested	an	address	and/or	port,	make	sure	we	reinitialize	to	the	same	one
								if	(!_listener->validateSourceAddress(_requested_source_address))	{
												_requested_source_address.clear()	;
								}

								message_publish(MSG_INFO,	"variable	server	restart	user_port	requested	set	%s:%d\n",_requested_source_address.c_str(),	_requested_port);

								_listener->disconnect();
								ret	=	_listener->initialize(_requested_source_address,	_requested_port);

								if	(ret	!=	0)	{
												message_publish(MSG_ERROR,	"ERROR:	Could	not	establish	listen	port	%d	for	Variable	Server.	Aborting.\n",	_requested_port);
												return	(-1);
								}
				}	else	{
								//	Otherwise,	just	ask	the	listener	what	port	it's	using
								_listener->checkSocket();
								message_publish(MSG_INFO,	"restart	variable	server	message	port	=	%d\n",	_listener->getPort());
				}

				initializeMulticast();

				return	0	;
}

void	Trick::VariableServerListenThread::initializeMulticast()	{
				_multicast->initialize();
				_multicast->addAddress("239.3.14.15",	9265);
				_multicast->addAddress("224.3.14.15",	9265);
}

void	Trick::VariableServerListenThread::pause_listening()	{
				force_thread_to_pause();
}

void	Trick::VariableServerListenThread::restart_listening()	{
				_listener->restart();
				unpause_thread();
}

void	Trick::VariableServerListenThread::dump(	std::ostream	&	oss	)	{
				oss	<<	"Trick::VariableServerListenThread	("	<<	name	<<	")"	<<	std::endl	;
				oss	<<	"				source_address	=	"	<<	_listener->getHostname()	<<	std::endl	;
				oss	<<	"				port	=	"	<<	_listener->getPort()	<<	std::endl	;
				oss	<<	"				user_requested_address	=	"	<<	_user_requested_address	<<	std::endl	;
				oss	<<	"				user_tag	=	"	<<	_user_tag	<<	std::endl	;
				oss	<<	"				broadcast	=	"	<<	_broadcast	<<	std::endl	;
				Trick::ThreadBase::dump(oss)	;
}
