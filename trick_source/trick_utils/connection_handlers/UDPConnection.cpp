#include	"trick/UDPConnection.hh"

#include	<sstream>
#include	<iostream>
#include	<cstring>
#include	<strings.h>
#include	<arpa/inet.h>

Trick::UDPConnection::UDPConnection	()	:	UDPConnection(new	SystemInterface())	{}
Trick::UDPConnection::UDPConnection	(SystemInterface	*	system_interface)	:	_started(false),	_initialized(false),	_port(0),	_hostname(""),	_system_interface(system_interface),	_socket(0)	{}

int	Trick::UDPConnection::initialize(const	std::string&	in_hostname,	int	in_port)	{


				if	((_socket	=	_system_interface->socket(AF_INET,	SOCK_DGRAM,	0))	<	0)	{
								perror	("Server:	Unable	to	open	socket");
								return	-1;
				}

				int	option_val	=	1;

				//	Do	not	replicate	the	socket's	handle	if	this	process	is	forked
				//	Removing	this	will	cause	all	kinds	of	problems	when	starting	a	variable	server	client	via	input	file
				_system_interface->fcntl(_socket,	F_SETFD,	FD_CLOEXEC);

				//	Allow	socket's	bound	address	to	be	used	by	others
				if	(_system_interface->setsockopt(_socket,	SOL_SOCKET,	SO_REUSEADDR,	(const	char	*)	&option_val,	(socklen_t)	sizeof(option_val))	!=	0)	{
								perror("Server:	Could	not	set	socket	to	reuse	addr");
								_system_interface->close	(_socket);
								return	-1;
				}

				struct	sockaddr_in	s_in;
				memset(&s_in,	0	,	sizeof(struct	sockaddr_in))	;

				//	Look	up	the	hostname
				char	name[80];
				gethostname(name,	(size_t)	80);

				struct	hostent	*ip_host	;
				socklen_t	s_in_size	=		sizeof(s_in)	;

				s_in.sin_family	=	AF_INET;

				if	(in_hostname	==	""	||	in_hostname	==	"localhost"	||	in_hostname	==	"127.0.0.1"	||	strcmp(in_hostname.c_str(),name)	==	0)	{
								s_in.sin_addr.s_addr	=	INADDR_ANY;
								_hostname	=	std::string(name);
				}	else	if	(	inet_pton(AF_INET,	in_hostname.c_str(),	(struct	in_addr	*)&s_in.sin_addr.s_addr)	==	1	)	{
								/*	numeric	character	string	address	*/
								_hostname	=	in_hostname;
				}	else	if	(	(ip_host	=	gethostbyname(in_hostname.c_str()))	!=	NULL	)	{
								/*	some	name	other	than	the	default	name	was	given	*/
								memcpy((void	*)	&(s_in.sin_addr.s_addr),	(const	void	*)	ip_host->h_addr,	(size_t)	ip_host->h_length);
								_hostname	=	in_hostname;
				}	else	{
								perror("UDP	socket:	Could	not	determine	source	address");
								return	-1	;
				}

				//	Set	port
				s_in.sin_port	=	htons((short)	in_port);

				//	Bind	to	socket
				if	(_system_interface->bind(_socket,	(struct	sockaddr	*)&s_in,	sizeof(s_in))	<	0)	{

								perror("UDP	socket:	Could	not	bind	to	socket");
								_system_interface->close	(_socket);
								return	-1;
				}	

				//	Check	that	correct	port	was	bound	to
				_system_interface->getsockname(	_socket	,	(struct	sockaddr	*)&s_in,	&s_in_size)	;
				int	bound_port	=	ntohs(s_in.sin_port);

				if	(in_port	!=	0	&&	bound_port	!=	in_port)	{
								std::cerr	<<	"UDP	socket:	Could	not	bind	to	requested	port	"	<<	in_port	<<	std::endl;
								_system_interface->close(_socket);
								return	-1;
				}

				//	Save	port	number
				_port	=	bound_port;
				
				setBlockMode(false);

				//	Done!
				_initialized	=	true;
				return	0;
}

int	Trick::UDPConnection::start()	{
				//	We've	already	set	everything	up,	just	make	sure	it	was	successful
				if	(_initialized)	{
								_started	=	true;
								return	0;
				}
				return	-1;
}

int	Trick::UDPConnection::write	(char	*	message,	int	size)	{
				if	(!_started)
								return	-1;

				socklen_t	sock_size	=	sizeof(_remote_serv_addr);

				return	_system_interface->sendto(_socket,	message,	size,	0,	(struct	sockaddr	*)	&_remote_serv_addr,	sock_size	);
}

int	Trick::UDPConnection::write	(const	std::string&	message)	{
				if	(!_started)
								return	-1;

				char	send_buf[message.length()+1];
				strcpy	(send_buf,	message.c_str());
				socklen_t	sock_size	=	sizeof(_remote_serv_addr);

				return	_system_interface->sendto(_socket,	send_buf,	message.length(),	0,	(struct	sockaddr	*)	&_remote_serv_addr,	sock_size	);
}

int	Trick::UDPConnection::read		(std::string&	message,	int	max_len)	{
				if	(!_started)
								return	0;

				char	incoming_msg[max_len];
				int	nbytes	=	_system_interface->recvfrom(	_socket,	incoming_msg,	MAX_CMD_LEN,	MSG_PEEK,	NULL,	NULL	)	;
				if	(nbytes	==	0	)	{
								return	0;
				}

				if	(nbytes	==	-1)	{
								if	(errno	==	EAGAIN	||	errno	==	EWOULDBLOCK)	{
												//	There's	nothing	ready	in	the	socket,	just	return	an	empty	string
												message	=	"";
												return	0;
								}

								//	Otherwise,	some	other	system	error	has	occurred.	Return	an	error.
								return	-1;
				}

				/*	find	the	last	newline	that	is	present	on	the	socket	*/
				incoming_msg[nbytes]	=	'\0'	;
				char	*last_newline	=	rindex(	incoming_msg	,	'\n')	;

				/*	if	there	is	a	newline	then	there	is	a	complete	command	on	the	socket	*/
				if	(	last_newline	!=	NULL	)	{
								socklen_t	sock_size	=	sizeof(_remote_serv_addr);
								/*	Save	the	remote	host	information	so	we	know	where	to	send	replies	*/
								/*	only	remove	up	to	(and	including)	the	last	newline	on	the	socket	*/
								int	size	=	last_newline	-	incoming_msg	+	1;
								nbytes	=	_system_interface->recvfrom(	_socket,	incoming_msg,	size,	0	,	(struct	sockaddr	*)	&_remote_serv_addr,	&sock_size	)	;
				}	else	{
								nbytes	=	0	;
				}
				
				std::stringstream	msg_stream;

				if	(	nbytes	>	0	)	{
								//	Strip	out	\r	characers

								int	msg_len	=	nbytes	;
								incoming_msg[msg_len]	=	'\0'	;

								for(	int	ii	=	0	,	jj	=	0	;	ii	<=	msg_len	;	ii++	)	{
												if	(	incoming_msg[ii]	!=	'\r'	)	{
																msg_stream	<<	incoming_msg[ii]	;
												}
								}
				}

				message	=	msg_stream.str();
				return	message.size();
}

int	Trick::UDPConnection::disconnect	()	{
				if	(!_initialized)	{
								return	-1;
				}

				_system_interface->shutdown(_socket,	SHUT_RDWR);
				_system_interface->close	(_socket);
				_initialized	=	false;
				_started	=	false;

				return	0;	
}

int	Trick::UDPConnection::setBlockMode(bool	blocking)	{
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

int	Trick::UDPConnection::restart()	{
				_system_interface	=	new	SystemInterface();
				return	0;
}

bool	Trick::UDPConnection::isInitialized()	{
				return	_started;
}

int	Trick::UDPConnection::getPort()	{
				return	_port;
}

std::string	Trick::UDPConnection::getHostname()	{
				return	_hostname;
}

std::string	Trick::UDPConnection::getClientTag	()	{
				return	_client_tag;
}

int	Trick::UDPConnection::setClientTag	(std::string	tag)	{
				_client_tag	=	tag;
				return	0;
}

std::string	Trick::UDPConnection::getClientHostname()	{
				if	(!_initialized)	{
								return	"";
				}

				return	inet_ntoa(_remote_serv_addr.sin_addr);
}

int	Trick::UDPConnection::getClientPort()	{
				if	(!_initialized)	{
								return	0;
				}

				return	ntohs(_remote_serv_addr.sin_port);
}