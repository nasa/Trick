
#include	<netdb.h>
#include	<iostream>
#include	"trick/VariableServer.hh"
#include	"trick/tc_proto.h"

Trick::VariableServer	*	the_vs	;

Trick::VariableServer::VariableServer()	:
	enabled(true)	,
	info_msg(false),
	log(false)
{
				the_vs	=	this	;
				pthread_mutex_init(&map_mutex,	NULL);
}

Trick::VariableServer::~VariableServer()	{
				the_vs	=	NULL;
}

void	Trick::VariableServer::shutdownConnections()	{
				listen_thread.shutdownConnections();
}

std::ostream&	Trick::operator<<	(std::ostream&	s,	Trick::VariableServer&	vs)	{
				std::map	<	pthread_t	,	VariableServerSessionThread	*	>::iterator	it	;

				s	<<	"{\"variable_server_connections\":[\n";
				int	count	=	0;
				int	n_connections	=	(int)vs.var_server_threads.size();
				for	(	it	=	vs.var_server_threads.begin()	;	it	!=	vs.var_server_threads.end()	;	++it	)	{
								s	<<	"{\n";
								s	<<	*(*it).second;
								s	<<	"}";
								if	((n_connections-count)>1)	{
												s	<<	","	;
								}
								s	<<	"\n";
								count	++;
				}
				s	<<	"]}"	<<	std::endl;
				return	s;
}

bool	Trick::VariableServer::get_enabled()	{
				return	enabled	;
}

void	Trick::VariableServer::set_enabled(bool	on_off)	{
				enabled	=	on_off	;
}

bool	Trick::VariableServer::get_info_msg()	{
				return	info_msg	;
}

void	Trick::VariableServer::set_var_server_info_msg_on()	{
				info_msg	=	true;
				for	(	auto&	session_it	:	var_server_sessions	)	{
								session_it.second->set_info_message(info_msg);
				}
}

void	Trick::VariableServer::set_var_server_info_msg_off()	{
				info_msg	=	false;
				for	(	auto&	session_it	:	var_server_sessions	)	{
								session_it.second->set_info_message(info_msg);
				}
}

bool	Trick::VariableServer::get_log()	{
				return	log	;
}

bool	Trick::VariableServer::get_session_log()	{
				return	session_log	;
}

void	Trick::VariableServer::set_var_server_log_on()	{
				log	=	true;
				//	turn	log	on	for	all	current	vs	clients
				for	(	auto&	session_it	:	var_server_sessions	)	{
								session_it.second->set_log(true);
				}
}

void	Trick::VariableServer::set_var_server_log_off()	{
				log	=	false;
				//	turn	log	off	for	all	current	vs	clients
				for	(	auto&	session_it	:	var_server_sessions	)	{
								session_it.second->set_log(false);
				}
}

void	Trick::VariableServer::set_var_server_session_log_on()	{
				session_log	=	true;
				//	turn	log	on	for	all	current	vs	clients
				for	(	auto&	session_it	:	var_server_sessions	)	{
								session_it.second->set_session_log(true);
				}
}

void	Trick::VariableServer::set_var_server_session_log_off()	{
				session_log	=	false;
				//	turn	log	off	for	all	current	vs	clients
				for	(	auto&	session_it	:	var_server_sessions	)	{
								session_it.second->set_session_log(false);
				}
}

const	char	*	Trick::VariableServer::get_hostname()	{
				return	listen_thread.get_hostname();
}

Trick::VariableServerListenThread	&	Trick::VariableServer::get_listen_thread()	{
				return	listen_thread	;
}

void	Trick::VariableServer::add_vst(pthread_t	in_thread_id,	VariableServerSessionThread	*	in_vst)	{
				pthread_mutex_lock(&map_mutex)	;
				var_server_threads[in_thread_id]	=	in_vst	;
				pthread_mutex_unlock(&map_mutex)	;
}

void	Trick::VariableServer::add_session(pthread_t	in_thread_id,	VariableServerSession	*	in_session)	{
				pthread_mutex_lock(&map_mutex)	;
				var_server_sessions[in_thread_id]	=	in_session	;
				pthread_mutex_unlock(&map_mutex)	;
}

Trick::VariableServerSessionThread	*	Trick::VariableServer::get_vst(pthread_t	thread_id)	{
				std::map	<	pthread_t	,	Trick::VariableServerSessionThread	*	>::iterator	it	;
				Trick::VariableServerSessionThread	*	ret	=	NULL	;
				pthread_mutex_lock(&map_mutex)	;
				it	=	var_server_threads.find(thread_id)	;
				if	(	it	!=	var_server_threads.end()	)	{
								ret	=	(*it).second	;
				}
				pthread_mutex_unlock(&map_mutex)	;
				return	ret	;
}

Trick::VariableServerSession	*	Trick::VariableServer::get_session(pthread_t	thread_id)	{
				Trick::VariableServerSession	*	ret	=	NULL	;
				pthread_mutex_lock(&map_mutex)	;
				auto	it	=	var_server_sessions.find(thread_id)	;
				if	(	it	!=	var_server_sessions.end()	)	{
								ret	=	(*it).second	;
				}
				pthread_mutex_unlock(&map_mutex)	;
				return	ret	;
}

void	Trick::VariableServer::delete_vst(pthread_t	thread_id)	{
				pthread_mutex_lock(&map_mutex)	;
				var_server_threads.erase(thread_id)	;
				pthread_mutex_unlock(&map_mutex)	;
}

void	Trick::VariableServer::delete_session(pthread_t	thread_id)	{
				pthread_mutex_lock(&map_mutex)	;
				var_server_sessions.erase(thread_id)	;
				pthread_mutex_unlock(&map_mutex)	;
}

void	Trick::VariableServer::set_copy_data_job(	Trick::JobData	*	in_job	)	{
				copy_data_job	=	in_job	;
}

void	Trick::VariableServer::set_copy_and_write_freeze_job(	Trick::JobData	*	in_job	)	{
				copy_and_write_freeze_job	=	in_job	;
}
