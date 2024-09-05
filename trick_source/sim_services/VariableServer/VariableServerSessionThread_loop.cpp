
#include	<iostream>
#ifdef	__linux
#include	<cxxabi.h>
#endif

#include	"trick/VariableServer.hh"
#include	"trick/message_proto.h"
#include	"trick/realtimesync_proto.h"
#include	"trick/ExecutiveException.hh"
#include	"trick/exec_proto.h"

#include	"trick/VariableServerSessionThread.hh"

void	exit_var_thread(void	*in_vst)	;

void	*	Trick::VariableServerSessionThread::thread_body()	{

				//	Check	for	short	running	sims
				test_shutdown(NULL,	NULL);

				//		We	need	to	make	the	thread	to	VariableServerSessionThread	map	before	we	accept	the	connection.
				//		Otherwise	we	have	a	race	where	this	thread	is	unknown	to	the	variable	server	and	the
				//		client	gets	confirmation	that	the	connection	is	ready	for	communication.
				_vs->add_vst(	pthread_self()	,	this	)	;

				//	Accept	client	connection
				int	status	=	_connection->start();

				if	(status	!=	0)	{
								_vs->delete_vst(pthread_self());

								//	Tell	main	thread	that	we	failed	to	initialize
								pthread_mutex_lock(&_connection_status_mutex);
								_connection_status	=	CONNECTION_FAIL;
								pthread_cond_signal(&_connection_status_cv);
								pthread_mutex_unlock(&_connection_status_mutex);

								thread_shutdown();
				}

				//	if	log	is	set	on	for	variable	server	(e.g.,	in	input	file),	turn	log	on	for	each	client
				if	(_vs->get_log())	{
								_session->set_log(true);
				}

				if	(_vs->get_session_log())	{
								_session->set_session_log(true);
				}

				if	(_vs->get_info_msg())	{
								_session->set_info_message(true);
				}

				//	Give	the	initialized	connection	to	the	session
				//	Don't	touch	the	connection	anymore	until	we	shut	them	both	down
				_session->set_connection(_connection);
				_vs->add_session(	pthread_self(),	_session	);

				//	Tell	main	that	we	are	ready
				pthread_mutex_lock(&_connection_status_mutex);
				_connection_status	=	CONNECTION_SUCCESS;
				pthread_cond_signal(&_connection_status_cv);
				pthread_mutex_unlock(&_connection_status_mutex);

				try	{
								while	(1)	{
												//	Shutdown	here	if	it's	time
												test_shutdown(exit_var_thread,	(void	*)	this);

												//	Pause	here	if	we	are	in	a	restart	condition
												test_pause();

												//	Look	for	a	message	from	the	client
												//	Parse	and	execute	if	one	is	availible
												int	read_status	=	_session->handle_message();
												if	(	read_status	<	0	)	{
																break	;
												}

												//	Check	to	see	if	exit	is	necessary
												if	(_session->get_exit_cmd()	==	true)	{
																break;
												}

												//	Tell	session	it's	time	to	copy	and	write	if	the	mode	is	correct
												int	ret	=_session->copy_and_write_async();	
												if	(ret	<	0)	{
																break;
												}

												//	Sleep	for	the	appropriate	cycle	time
												usleep((unsigned	int)	(_session->get_update_rate()	*	1000000));
								}
				}	catch	(Trick::ExecutiveException	&	ex	)	{
								message_publish(MSG_ERROR,	"\nVARIABLE	SERVER	COMMANDED	exec_terminate\n		ROUTINE:	%s\n		DIAGNOSTIC:	%s\n"	,
									ex.file.c_str(),	ex.message.c_str())	;

								exec_signal_terminate();

				}	catch	(const	std::exception	&ex)	{
								message_publish(MSG_ERROR,	"\nVARIABLE	SERVER	caught	std::exception\n		DIAGNOSTIC:	%s\n"	,
									ex.what())	;
								
								exec_signal_terminate();

#ifdef	__linux
#ifdef	__GNUC__
#if	__GNUC__	>=	4	&&	__GNUC_MINOR__	>=	2
				//	for	post	gcc	4.1.2	or	whatever	glibc	version	is	used	in	RHEL6	and	above.
				}	catch	(abi::__forced_unwind&)	{
								//pthread_exit	and	pthread_cancel	will	cause	an	abi::__forced_unwind	to	be	thrown.	Rethrow	it.
								throw;
#endif
#endif
#endif
				}	catch	(...)	{
#ifdef	__linux
#ifdef	__GNUC__
#if	(__GNUC__	==	4	&&	__GNUC_MINOR__	==	1)	||	__GNUC__	==	12
								//	for	gcc	4.1.2	or	whatever	glib	version	in	RHEL	5	that	does	not	work	with	the	abi::__forced_unwind
								//	Also	seems	to	have	a	problem	with	gcc	12
								throw;
#else
								message_publish(MSG_ERROR,	"\nVARIABLE	SERVER	caught	unknown	exception\n"	)	;
								exec_signal_terminate();
#endif
#endif
#endif
				}

				if	(_debug	>=	3)	{
								message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	receive	loop	exiting\n",	_connection,	_connection->getClientTag().c_str());
				}

				thread_shutdown(exit_var_thread,	this);
				//	No	return	from	this.
}

