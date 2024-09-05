#include	<unistd.h>
#include	<iostream>
#include	<sys/select.h>
#include	<netinet/tcp.h>
#include	<fcntl.h>
#include	<string.h>
#include	<errno.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<arpa/inet.h>

#include	"trick/TCPClientListener.hh"

Trick::TCPClientListener::TCPClientListener	()	:	TCPClientListener	(new	SystemInterface())	{}
Trick::TCPClientListener::TCPClientListener	(SystemInterface	*	system_interface)	:	_listen_socket(-1),	_hostname(""),	_port(0),	_client_tag("<empty>"),	_initialized(false),	_system_interface(system_interface)	{}

Trick::TCPClientListener::~TCPClientListener	()	{
				//	Clean	up	our	socket	if	initialized
				if	(_initialized)	{
								close	(_listen_socket);
				}

				delete	_system_interface;
}

int	Trick::TCPClientListener::initialize(std::string	in_hostname,	int	in_port)	{

				if	((_listen_socket	=	_system_interface->socket(AF_INET,	SOCK_STREAM,	0))	<	0)	{
								perror	("Server:	Unable	to	open	socket");
								return	LISTENER_ERROR;
				}

				int	option_val	=	1;

				//	Do	not	replicate	the	socket's	handle	if	this	process	is	forked
				//	Removing	this	will	cause	all	kinds	of	problems	when	starting	a	variable	server	client	via	input	file
				_system_interface->fcntl(_listen_socket,	F_SETFD,	FD_CLOEXEC);

				//	Allow	socket's	bound	address	to	be	used	by	others
				if	(_system_interface->setsockopt(_listen_socket,	SOL_SOCKET,	SO_REUSEADDR,	(const	char	*)	&option_val,	(socklen_t)	sizeof(option_val))	!=	0)	{
								perror("Server:	Could	not	set	socket	to	reuse	addr");
								_system_interface->close	(_listen_socket);
								return	LISTENER_ERROR;
				}
				//	Turn	off	data	buffering	on	the	send	side
				if	(_system_interface->setsockopt(_listen_socket,	IPPROTO_TCP,	TCP_NODELAY,	(const	void	*)	&option_val,	(socklen_t)	sizeof(option_val))	!=	0)	{
								perror("Server:	Could	not	turn	off	data	buffering");
								_system_interface->close	(_listen_socket);
								return	LISTENER_ERROR;
				}

				struct	sockaddr_in	s_in;
				memset(&s_in,	0	,	sizeof(struct	sockaddr_in))	;

				//	Look	up	the	hostname
				char	name[80];
				gethostname(name,	(size_t)	80);

				struct	hostent	*ip_host	;
				socklen_t	s_in_size	=		sizeof(s_in)	;

				s_in.sin_family	=	AF_INET;

				if	(in_hostname	==	""	||	in_hostname	==	"localhost"	||	strcmp(in_hostname.c_str(),name)	==	0)	{
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
								perror("Server:	Could	not	determine	source	address");
								return	-1	;
				}

				//	Set	port
				s_in.sin_port	=	htons((short)	in_port);

				//	Bind	to	socket
				if	(_system_interface->bind(_listen_socket,	(struct	sockaddr	*)&s_in,	sizeof(s_in))	<	0)	{

								perror("Server:	Could	not	bind	to	socket");
								_system_interface->close	(_listen_socket);
								return	LISTENER_ERROR;
				}	

				//	Check	that	correct	port	was	bound	to
				_system_interface->getsockname(	_listen_socket	,	(struct	sockaddr	*)&s_in,	&s_in_size)	;
				int	bound_port	=	ntohs(s_in.sin_port);

				if	(in_port	!=	0	&&	bound_port	!=	in_port)	{
								std::cerr	<<	"Server:	Could	not	bind	to	requested	port	"	<<	in_port	<<	std::endl;
								_system_interface->close(_listen_socket);
								return	LISTENER_ERROR;
				}

				//	Save	port	number
				_port	=	bound_port;
				

				//	Start	listening
				if	(_system_interface->listen(_listen_socket,	SOMAXCONN)	<	0)	{
								std::string	error_message	=	"Server:	Could	not	listen	on	port	"	+	std::to_string(_port);
								perror	(error_message.c_str());
								_system_interface->close(_listen_socket);
								return	LISTENER_ERROR;
				}

				//	Done!
				_initialized	=	true;
				return	0;
}

int	Trick::TCPClientListener::initialize()	{
				return	initialize("",	0);
}


int	Trick::TCPClientListener::setBlockMode(bool	blocking)	{
				if	(!_initialized)
								return	-1;

				int	flag	=	_system_interface->fcntl(_listen_socket,	F_GETFL,	0);

				if	(flag	==	-1)	{
								std::string	error_message	=	"Unable	to	get	flags	for	fd	"	+	std::to_string(_listen_socket)	+	"	block	mode	to	"	+	std::to_string(blocking);
								perror	(error_message.c_str());
								return	-1;
				}

				if	(blocking)	{
								flag	&=	~O_NONBLOCK;
				}	else	{
								flag	|=	O_NONBLOCK;
				}

				if	(_system_interface->fcntl(_listen_socket,	F_SETFL,	flag)	==	-1)	{
								std::string	error_message	=	"Unable	to	set	fd	"	+	std::to_string(_listen_socket)	+	"	block	mode	to	"	+	std::to_string(blocking);
								perror	(error_message.c_str());
								return	-1;
				}

				return	0;
}


bool	Trick::TCPClientListener::checkForNewConnections()	{
				if	(!_initialized)
								return	false;

				fd_set	rfds;
				struct	timeval	timeout_time	=	{	2,	0	};
				FD_ZERO(&rfds);
				FD_SET(_listen_socket,	&rfds);
				timeout_time.tv_sec	=	2	;

				//	Listen	with	a	timeout	of	2	seconds
				int	result	=	_system_interface->select(_listen_socket	+	1,	&rfds,	NULL,	NULL,	&timeout_time);

				//	If	there's	some	kind	of	error,	just	ignore	it	and	return	false
				if	(result	<	0)	{
								return	false;
				}

				return	FD_ISSET(_listen_socket,	&rfds);
}



std::string	Trick::TCPClientListener::getHostname	()	{
				if	(!_initialized)
								return	"";

				return	_hostname;
}


int	Trick::TCPClientListener::getPort()	{
				if	(!_initialized)
								return	-1;

				return	_port;
}


int	Trick::TCPClientListener::disconnect()	{
				if	(!_initialized)	{
								return	-1;
				}

				_system_interface->shutdown(_listen_socket,	SHUT_RDWR);
				_system_interface->close	(_listen_socket);
				_initialized	=	false;

				return	0;	
}

bool	Trick::TCPClientListener::validateSourceAddress(std::string	in_hostname)	{
				//	struct	addrinfo	hints,	*res;
				//	memset(&hints,	0,	sizeof	hints);
				//	hints.ai_family	=	AF_INET;
				//	hints.ai_socktype	=	SOCK_STREAM;
				//	hints.ai_protocol	=	0;

				//	int	err;
				//	if	((err	=	_system_interface->getaddrinfo(requested_source_address.c_str(),	0,	&hints,	&res))	!=	0)	{
				//					std::cerr	<<	"Unable	to	lookup	address:	"	<<	gai_strerror(err)	<<	std::endl;
				//					return	false;
				//	}

								//	Look	up	the	hostname
				char	name[80];
				gethostname(name,	(size_t)	80);

				struct	hostent	*ip_host	;
				sockaddr_in	s_in;
				socklen_t	s_in_size	=		sizeof(s_in)	;

				s_in.sin_family	=	AF_INET;

				if	(in_hostname	==	""	||	in_hostname	==	"localhost"	||	strcmp(in_hostname.c_str(),name)	==	0)	{
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
								perror("Server:	Could	not	determine	source	address");
								return	false;
				}

				return	true;
}

int	Trick::TCPClientListener::checkSocket()	{
				if	(!_initialized)
								return	-1;

				struct	sockaddr_in	s_in;
				socklen_t	s_in_size	=		sizeof(s_in)	;
				_system_interface->getsockname(	_listen_socket	,	(struct	sockaddr	*)&s_in,	&s_in_size)	;
				_port	=	ntohs(s_in.sin_port);

				return	0;
}

bool	Trick::TCPClientListener::isInitialized()	{
				return	_initialized;
}

Trick::TCPConnection	*	Trick::TCPClientListener::setUpNewConnection	()	{
				if	(!_initialized)
								return	NULL;

				TCPConnection	*	connection	=	new	TCPConnection(_listen_socket);
				return	connection;
}

int	Trick::TCPClientListener::restart	()	{
				_system_interface	=	new	SystemInterface();
				return	0;
}
	

