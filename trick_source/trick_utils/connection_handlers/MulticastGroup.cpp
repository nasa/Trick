#include	"trick/MulticastGroup.hh"
#include	<string.h>
#include	<vector>
#include	<sstream>


Trick::MulticastGroup::MulticastGroup()	:	MulticastGroup(new	SystemInterface)	{}

Trick::MulticastGroup::MulticastGroup	(SystemInterface	*	system_interface)	:	_initialized(false),	_system_interface(system_interface)	{}

Trick::MulticastGroup::~MulticastGroup()	{}

int	Trick::MulticastGroup::restart	()	{
				//	Keep	address	list	the	same,	but	we	may	need	to	get	a	new	socket
				_system_interface	=	new	SystemInterface();
				//	return	initialize();
				//	return	0;
}


int	Trick::MulticastGroup::broadcast	(std::string	message)	{
				if	(!isInitialized())	{
								return	-1;
				}

				const	char	*	message_send	=	message.c_str();
				for	(struct	sockaddr_in&	address	:	_addresses)	{
								int	status	=	_system_interface->sendto(_socket	,	message_send	,	strlen(message_send)	,	0	,	(struct	sockaddr	*)&address	,	(socklen_t)sizeof(address))	;
								if	(status	==	-1)	{
												char	*	readable_addr	=	inet_ntoa(	address.sin_addr	);
												std::string	err_msg	=	"MulticastGroup:	Failed	to	broadcast	to	address	"	+	std::string(readable_addr);
												perror(err_msg.c_str());
								}
				}
				return	0;
}

int	Trick::MulticastGroup::addAddress	(std::string	addr,	int	port)	{
				auto	in_addr	=	_system_interface->inet_addr(addr.c_str());
				if	(in_addr	==	-1)	{
								std::string	error_msg	=	"MulticastGroup:	Cannot	add	address	"	+	addr	+	"	to	multicast	group";
								perror(error_msg.c_str());
								return	-1;
				}

				struct	sockaddr_in	mcast_addr;
				memset(&mcast_addr,	0,	sizeof(mcast_addr));
				mcast_addr.sin_family	=	AF_INET;
				mcast_addr.sin_addr.s_addr	=	in_addr;
				mcast_addr.sin_port	=	htons((uint16_t)	port);;
				_addresses.emplace_back(mcast_addr);
				return	0;
}

bool	Trick::MulticastGroup::isInitialized	()	{
				return	_initialized;
}

int	Trick::MulticastGroup::initialize()	{
				_socket	=	_system_interface->socket(AF_INET,	SOCK_DGRAM,	0);
				if	(_socket	<	0)	{
								perror("MulticastGroup:	socket");
								return	-1;
				}

				int	option_value	=	1;
				if	(_system_interface->setsockopt(_socket,	SOL_SOCKET,	SO_REUSEADDR,	(char	*)	&option_value,	(socklen_t)	sizeof(option_value))	<	0)	{
								perror("MulticastGroup:	setsockopt:	reuseaddr");
								return	-1;
				}
#ifdef	SO_REUSEPORT
				if	(_system_interface->setsockopt(_socket,	SOL_SOCKET,	SO_REUSEPORT,	(char	*)	&option_value,	sizeof(option_value))	<	0)	{
								perror("MulticastGroup:	setsockopt:	reuseport");
								return	-1;
				}
#endif
				_initialized	=	true;
				return	0;
}

int	Trick::MulticastGroup::initialize_with_receiving(std::string	addr,	std::string	mcast_addr,	int	port)	{
				UDPConnection::initialize(addr,	port);

				struct	ip_mreq	mreq;
				mreq.imr_multiaddr.s_addr	=	inet_addr(mcast_addr.c_str());
				mreq.imr_interface.s_addr	=	htonl(INADDR_ANY);
				int	ret	=	setsockopt(_socket,	IPPROTO_IP,	IP_ADD_MEMBERSHIP,	&mreq,	sizeof(mreq))	;
				if	(	ret	<	0	)	{
								perror("ip_add_membership")	;
								return	ret	;
				}

				//	Save	our	info	so	that	we	know	to	ignore	messages	from	ourself
				memset(&_remote_serv_addr,	0,	sizeof(struct	sockaddr_in));
				_remote_serv_addr.sin_family	=	AF_INET;
				_remote_serv_addr.sin_addr.s_addr	=	inet_addr(mcast_addr.c_str());
				_remote_serv_addr.sin_port	=	htons(port);

				int	s_in_size	=		sizeof(_self_info)	;
				getsockname(	_socket	,	(struct	sockaddr	*)&_self_info,	(socklen_t	*)&s_in_size)	;
				if	(	_self_info.sin_addr.s_addr	==	0	)	{
								char	hname[80];
								struct	hostent	*ip_host	=	NULL;
								gethostname(hname,	(size_t)	80);
								ip_host	=	gethostbyname(hname);
								if	(ip_host	!=	NULL)	{	
												memcpy(&(_self_info.sin_addr.s_addr),	ip_host->h_addr,	(size_t)	ip_host->h_length);
								}	else	{
												_self_info.sin_addr.s_addr	=	inet_addr(addr.c_str());
												_self_info.sin_port	=	htons(port);
								}
				}

				setBlockMode(false);


				return	0;				
}

int	Trick::MulticastGroup::disconnect()	{
				if	(_initialized)	{
								_system_interface->close(_socket);
				}
				return	0;
}


int	Trick::MulticastGroup::write	(char	*	message,	int	size)	{
				if	(!_started)
								return	-1;

				socklen_t	sock_size	=	sizeof(_remote_serv_addr);

				return	_system_interface->sendto(_socket,	message,	size,	0,	(struct	sockaddr	*)	&_remote_serv_addr,	sock_size	);
}

int	Trick::MulticastGroup::write	(const	std::string&	message)	{
				if	(!_started)
								return	-1;

				char	send_buf[message.length()+1];
				strcpy	(send_buf,	message.c_str());
				socklen_t	sock_size	=	sizeof(_remote_serv_addr);

				return	_system_interface->sendto(_socket,	send_buf,	message.length(),	0,	(struct	sockaddr	*)	&_remote_serv_addr,	sock_size	);
}


int	Trick::MulticastGroup::read		(std::string&	message,	int	max_len)	{
				if	(!_started)
								return	0;

				struct	sockaddr_in	s_in	;
				socklen_t	sock_size	=	sizeof(s_in)	;

				char	incoming_msg[max_len];
				int	nbytes	=	_system_interface->recvfrom(	_socket,	incoming_msg,	MAX_CMD_LEN,	MSG_PEEK,(struct	sockaddr	*)&s_in,	&sock_size	)	;

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

				//	Make	sure	that	this	message	isn't	from	ourself
				if	(	s_in.sin_addr.s_addr	==	_self_info.sin_addr.s_addr	&&	s_in.sin_port	==	_self_info.sin_port)	{
								//	Clear	out	the	socket
								_system_interface->recvfrom(	_socket,	incoming_msg,	MAX_CMD_LEN,	0	,	0,	0)	;
								return	0;
				}

				/*	find	the	last	newline	that	is	present	on	the	socket	*/
				incoming_msg[nbytes]	=	'\0'	;
				char	*last_newline	=	rindex(	incoming_msg	,	'\n')	;

				/*	if	there	is	a	newline	then	there	is	a	complete	command	on	the	socket	*/
				if	(	last_newline	!=	NULL	)	{
								socklen_t	sock_size	=	sizeof(_remote_serv_addr);
								/*	only	remove	up	to	(and	including)	the	last	newline	on	the	socket	*/
								int	size	=	last_newline	-	incoming_msg	+	1;
								nbytes	=	_system_interface->recvfrom(	_socket,	incoming_msg,	size,	0	,	0,	0)	;
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

std::string	Trick::MulticastGroup::getClientHostname()	{
				if	(!_started)	{
								return	"";
				}

				return	inet_ntoa(_remote_serv_addr.sin_addr);
}

int	Trick::MulticastGroup::getClientPort()	{
				if	(!_started)	{
								return	0;
				}

				return	ntohs(_remote_serv_addr.sin_port);
}