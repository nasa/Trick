#include	"trick/VariableServerSession.hh"
#include	"trick/TrickConstant.hh"
#include	"trick/exec_proto.h"
#include	"trick/Message_proto.hh"
#include	"trick/message_proto.h"
#include	"trick/input_processor_proto.h"
#include	"trick/realtimesync_proto.h"

int	Trick::VariableServerSession::instance_counter	=	0;
std::string	Trick::VariableServerSession::log_subdir	=	"session_logs";

Trick::VariableServerSession::VariableServerSession()	{
				_debug	=	0;
				_enabled	=	true	;
				_log	=	false	;
				_session_log	=	false;
				_session_log_msg_stream	=	-1;
				_info_msg	=	false;
				_copy_mode	=	VS_COPY_ASYNC	;
				_write_mode	=	VS_WRITE_ASYNC	;
				_frame_multiple	=	1	;
				_frame_offset	=	0	;
				_freeze_frame_multiple	=	1	;
				_freeze_frame_offset	=	0	;
				_update_rate	=	0.1	;
				_cycle_tics	=	(long	long)(_update_rate	*	exec_get_time_tic_value())	;
				if	(_cycle_tics	==	0)	{
								_cycle_tics	=	1;
				}

				_next_tics	=	TRICK_MAX_LONG_LONG	;
				_freeze_next_tics	=	TRICK_MAX_LONG_LONG	;
				_byteswap	=	false	;
				_validate_address	=	false	;
				_send_stdio	=	false	;

				_binary_data	=	false;
				_byteswap	=	false;
				_binary_data_nonames	=	false;

				_exit_cmd	=	false;
				_pause_cmd	=	false;

				_instance_num	=	instance_counter++;

				pthread_mutex_init(&_copy_mutex,	NULL);
}

Trick::VariableServerSession::~VariableServerSession()	{
				for	(unsigned	int	ii	=	0	;	ii	<	_session_variables.size()	;	ii++	)	{
								delete	_session_variables[ii];
				}
	}


void	Trick::VariableServerSession::set_connection(ClientConnection	*	conn)	{
				_connection	=	conn;
				log_connection_opened();
}

bool	Trick::VariableServerSession::is_session_log_open()	{
				return	_session_log_msg_stream	!=	-1;
}

void	Trick::VariableServerSession::open_session_log()	{
				std::string	name	=	"VSSession"	+	std::to_string(_instance_num);
				_session_log_msg_stream	=	open_custom_message_file(log_subdir	+	"/"	+	name	+	".log",	name);
}


//	Command	to	turn	on	log	to	varserver_log	file
int	Trick::VariableServerSession::set_log(bool	on_off)	{
				_log	=	on_off;
				return(0)	;
}

//	Command	to	turn	on	individual	session	log	file
int	Trick::VariableServerSession::set_session_log(bool	on_off)	{
				_session_log	=	on_off;
				return(0)	;
}

bool	Trick::VariableServerSession::get_pause()	{
				return	_pause_cmd	;
}

void	Trick::VariableServerSession::set_pause(	bool	on_off)	{
				_pause_cmd	=	on_off	;
}


bool	Trick::VariableServerSession::get_exit_cmd()	{
				return	_exit_cmd	;
}

void	Trick::VariableServerSession::pause_copy()	{
				pthread_mutex_lock(&_copy_mutex);
}

void	Trick::VariableServerSession::unpause_copy()	{
				pthread_mutex_unlock(&_copy_mutex);
}

void	Trick::VariableServerSession::disconnect_references()	{
				for	(VariableReference	*	variable	:	_session_variables)	{
								variable->tagAsInvalid();
				}
}

long	long	Trick::VariableServerSession::get_next_tics()	const	{
				if	(	!	_enabled	)	{
								return	TRICK_MAX_LONG_LONG	;
				}
				return	_next_tics	;
}

long	long	Trick::VariableServerSession::get_freeze_next_tics()	const	{
				if	(	!	_enabled	)	{
								return	TRICK_MAX_LONG_LONG	;
				}
				return	_freeze_next_tics	;
}

void	Trick::VariableServerSession::log_connection_opened()	{
				if	(_session_log)	{
								if	(!is_session_log_open())	{
												open_session_log();
								}
				
								message_publish(_session_log_msg_stream,	"Variable	Server	Session	started	with	%s:%d\n",	_connection->getClientHostname().c_str(),	_connection->getClientPort());
				}
}

void	Trick::VariableServerSession::log_received_message(const	std::string&	msg)	{
				if	(_log)	{
								message_publish(MSG_PLAYBACK,	"tag=<%s>	time=%f	%s",	_connection->getClientTag().c_str(),	exec_get_sim_time(),	msg.c_str());
				}

				if	(_session_log)	{
								if	(!is_session_log_open())	
												open_session_log();

								message_publish(_session_log_msg_stream,	"tag=<%s>	time=%f	%s",	_connection->getClientTag().c_str(),	exec_get_sim_time(),	msg.c_str());
				}

				if	(_debug	>=	3)	{
								message_publish(MSG_DEBUG,	"%p	tag=<%s>	var_server	received	bytes	=	msg_len	=	%d\n",	_connection,	_connection->getClientTag().c_str(),	msg.size());
				}

				if	(_debug	>=	1	||	_info_msg)	{
								message_publish(MSG_DEBUG,	"tag=<%s>	time=%f	%s",	_connection->getClientTag().c_str(),	exec_get_sim_time(),	msg.c_str());
				}
				
}

int	Trick::VariableServerSession::handle_message()	{

				std::string	received_message;
				int	nbytes	=	_connection->read(received_message);
				if	(nbytes	>	0)	{
								log_received_message(received_message);
								ip_parse(received_message.c_str());	/*	returns	0	if	no	parsing	error	*/
				}

				return	nbytes;
}

Trick::VariableReference	*	Trick::VariableServerSession::find_session_variable(std::string	name)	const	{
				for	(VariableReference	*	ref	:	_session_variables)	{
								//	Look	for	matching	name
								if	(name.compare(ref->getName())	==	0)	{
												return	ref;
								}
				}

				return	NULL;
}

int	Trick::VariableServerSession::set_info_message(bool	on)	{
				_info_msg	=	on;
				return	0;
}

double	Trick::VariableServerSession::get_update_rate()	const	{
				return	_update_rate;
}

VS_WRITE_MODE	Trick::VariableServerSession::get_write_mode	()	const	{
				return	_write_mode;
}

VS_COPY_MODE	Trick::VariableServerSession::get_copy_mode	()	const	{
				return	_copy_mode;
}

long	long	Trick::VariableServerSession::get_cycle_tics()	const	{
				return	_cycle_tics;
}

int	Trick::VariableServerSession::get_frame_multiple	()	const	{
				return	_frame_multiple;
}

int	Trick::VariableServerSession::get_frame_offset	()	const	{
				return	_frame_offset;
}

int	Trick::VariableServerSession::get_freeze_frame_multiple	()	const	{
				return	_freeze_frame_multiple;
}

int	Trick::VariableServerSession::get_freeze_frame_offset	()	const	{
				return	_freeze_frame_offset;
}

bool	Trick::VariableServerSession::get_enabled	()	const	{
				return	_enabled;
}

void	Trick::VariableServerSession::set_freeze_next_tics(long	long	tics)	{
				_freeze_next_tics	=	tics;
}

void	Trick::VariableServerSession::set_next_tics(long	long	tics)	{
				_next_tics	=	tics;
}

void	Trick::VariableServerSession::set_exit_cmd()	{
				_exit_cmd	=	true;
}



std::ostream&	Trick::operator<<	(std::ostream&	s,	const	Trick::VariableServerSession&	session)	{
				if	(session._binary_data)	{
								s	<<	"				\"format\":\"BINARY\",\n";
				}	else	{
								s	<<	"				\"format\":\"ASCII\",\n";
				}
				s	<<	"				\"update_rate\":"	<<	session.get_update_rate()	<<	",\n";

				s	<<	"				\"variables\":[\n";

				int	n_vars	=	(int)session._session_variables.size();
				for	(int	i=0	;	i<n_vars	;	i++)	{
								s	<<	*(session._session_variables[i]);
								if	((n_vars-i)>1)	{
												s	<<	","	;
								}
								s	<<	"\n";
				}
				s	<<	"				]\n";

				return	s;
}