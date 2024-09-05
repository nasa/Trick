#include	"trick/TCPConnection.hh"

#include	<sstream>
#include	<iostream>
#include	<cstring>
#include	<strings.h>

Trick::TCPConnection::TCPConnection	()	:	TCPConnection(0,	new	SystemInterface())	{}

Trick::TCPConnection::TCPConnection	(SystemInterface	*	system_interface)	:	TCPConnection(0,	system_interface)	{}

Trick::TCPConnection::TCPConnection	(int	listen_socket)	:	TCPConnection(listen_socket,	new	SystemInterface())	{}

Trick::TCPConnection::TCPConnection	(int	listen_socket,	SystemInterface	*	system_interface)	:	_system_interface(system_interface),	_listen_socket(listen_socket),	_socket(0),	_connected(false)	{
				_connection_type	=	TCP;
}

int	Trick::TCPConnection::start()	{
				if	(_listen_socket	<=	0)	{
								return	-1;
				}

				//	Accept	a	waiting	connection
				struct	sockaddr_storage	their_addr;
				socklen_t	addr_size	=	sizeof	their_addr;

				if	((_socket	=	_system_interface->accept(_listen_socket,	(struct	sockaddr	*)&their_addr,	&addr_size))	<	0)	{
								perror	("Unable	to	accept	incoming	connection");
								return	-1;
				}

				//	Set	to	non	blocking
				setBlockMode(false);


				_connected	=	true;

				return	0;
}

int	Trick::TCPConnection::write	(char	*	message,	int	size)	{
				if	(!_connected)
								return	-1;

				return	_system_interface->send(_socket,	message,	size,	0);
}

int	Trick::TCPConnection::write	(const	std::string&	message)	{
				if	(!_connected)
								return	-1;

				char	send_buf[message.length()+1];
				strcpy	(send_buf,	message.c_str());

				return	_system_interface->send(_socket,	send_buf,	message.length(),	0);
}

int	Trick::TCPConnection::read		(std::string&	message,	int	max_len)	{
				if	(!_connected)	{
								std::cerr	<<	"Trying	to	read	from	a	socket	that	is	not	connected"	<<	std::endl;
								message	=	"";
								return	0;
				}

				char	incoming_msg[max_len];

				int	max_receive_length	=	max_len	<	MAX_CMD_LEN	?	max_len	:	MAX_CMD_LEN;
				int	nbytes	=	_system_interface->recv(_socket,	incoming_msg,	max_receive_length,	MSG_PEEK);

				if	(nbytes	==	0	)	{
								message	=	"";
								return	0;
				}

				if	(nbytes	==	-1)	{
								if	(errno	==	EAGAIN)	{
												message	=	"";
												return	0;
								}	else	{
												std::string	error_msg	=	"Error	while	reading	from	socket	"	+	std::to_string(_socket);
												perror(error_msg.c_str());
												message	=	"";
												return	-1;
								}
				}

				/*	find	the	last	newline	that	is	present	on	the	socket	*/
				incoming_msg[nbytes]	=	'\0'	;
				char	*last_newline	=	rindex(	incoming_msg	,	'\n')	;

				/*	if	there	is	a	newline	then	there	is	a	complete	command	on	the	socket	*/
				if	(	last_newline	!=	NULL	)	{
								/*	only	remove	up	to	(and	including)	the	last	newline	on	the	socket	*/
								int	size	=	last_newline	-	incoming_msg	+	1;
								nbytes	=		_system_interface->recv(	_socket,	incoming_msg,	size,	0)	;
				}	else	{
								nbytes	=	0	;
				}
				
				std::stringstream	msg_stream;

				if	(	nbytes	>	0	)	{
								int	msg_len	=	nbytes	;
								incoming_msg[msg_len]	=	'\0'	;

								//	Strip	off	any	\r	characters
								for(	int	ii	=	0	,	jj	=	0	;	ii	<=	msg_len	;	ii++	)	{
												if	(	incoming_msg[ii]	!=	'\r'	)	{
																msg_stream	<<	incoming_msg[ii]	;
												}
								}
				}

				message	=	msg_stream.str();
				return	message.size();
}


int	Trick::TCPConnection::disconnect	()	{
				if	(!_connected)	{
								return	-1;
				}

				_system_interface->shutdown(_socket,	SHUT_RDWR);
				_system_interface->close	(_socket);
				_connected	=	false;

				return	0;	
}

int	Trick::TCPConnection::setBlockMode(bool	blocking)	{

				int	flag	=	_system_interface->fcntl(_socket,	F_GETFL,	0);

				if	(flag	==	-1)	{
								std::string	error_message	=	"Unable	to	get	flags	for	fd	"	+	std::to_string(_socket);
								perror	(error_message.c_str());
								return	-1;
				}

				if	(blocking)	{
								flag	&=	~O_NONBLOCK;
				}	else	{
								flag	|=	O_NONBLOCK;
				}

				if	(_system_interface->fcntl(_socket,	F_SETFL,	flag)	==	-1)	{
								std::string	error_message	=	"Unable	to	set	fd	"	+	std::to_string(_socket)	+	"	block	mode	to	"	+	std::to_string(blocking);
								perror	(error_message.c_str());
								return	-1;
				}

				return	0;
}

bool	Trick::TCPConnection::isInitialized()	{
				return	_connected;
}


int	Trick::TCPConnection::restart()	{
				_system_interface	=	new	SystemInterface();
				return	0;
}

std::string	Trick::TCPConnection::getClientTag	()	{
				return	_client_tag;
}

int	Trick::TCPConnection::setClientTag	(std::string	tag)	{
				_client_tag	=	tag;
				return	0;
}

std::string	Trick::TCPConnection::getClientHostname()	{
				if	(!_connected)	{
								return	"";
				}

				struct	sockaddr_in	otherside;
				socklen_t	len	=	(socklen_t)sizeof(otherside);

				if	(getpeername(_socket,	(struct	sockaddr*)&otherside,	&len)	!=	0)
								return	"";

				return	inet_ntoa(otherside.sin_addr);
}

int	Trick::TCPConnection::getClientPort()	{
				if	(!_connected)	{
								return	0;
				}

				struct	sockaddr_in	otherside;
				socklen_t	len	=	(socklen_t)sizeof(otherside);

				if	(getpeername(_socket,	(struct	sockaddr*)&otherside,	&len)	!=	0)
								return	0;

				return	ntohs(otherside.sin_port);
}