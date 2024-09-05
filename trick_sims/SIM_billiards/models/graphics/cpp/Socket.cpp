#include	"Socket.hh"

#define	SOCKET_BUF_SIZE	20480

Socket::Socket()	:	_initialized(false)	{}

int	Socket::init	(std::string	hostname,	int	port)	{
				_hostname	=	hostname;
				_port	=	port;
				_socket_fd	=	socket(AF_INET,	SOCK_STREAM,	0);
				if	(_socket_fd	<	0)	{
								std::cout	<<	"Socket	connection	failed"	<<	std::endl;
								return	-1;
				}

				struct	sockaddr_in	serv_addr;
				serv_addr.sin_family	=	AF_INET;
				serv_addr.sin_port	=	htons(port);	//	convert	to	weird	network	byte	format

				if(inet_pton(AF_INET,	"127.0.0.1",	&serv_addr.sin_addr)<=0)	{	
								std::cout	<<	"Invalid	address/	Address	not	supported"	<<	std::endl;	
								return	-1;
				}	

				if	(connect(_socket_fd,	(struct	sockaddr	*)&serv_addr,	sizeof(serv_addr))	<	0)	{
								std::cout	<<	"Connection	failed"	<<	std::endl;
								return	-1;
				}

				_initialized	=	true;
}

int	Socket::send	(std::string	message)	{
				//	weird	syntax	I've	never	used	before	-	since	the	send	syscall	that	i'm	trying	to	use	is	overloaded	in	this	class,
				//	I	have	to	append	::	to	the	front	of	it	so	that	the	compiler	knows	to	look	in	the	global	namespace
				int	success	=	::send(_socket_fd,	message.c_str(),	message.size(),	0);
				if	(success	<	message.size())	{
								std::cout	<<	"Failed	to	send	message"	<<	std::endl;
				}
				return	success;
}

int	Socket::operator<<	(std::string	message)	{
				return	send(message);
}

std::string	Socket::receive	()	{
				char	buffer[SOCKET_BUF_SIZE];
				int	numBytes	=	read(_socket_fd,	buffer,	SOCKET_BUF_SIZE);
				if	(numBytes	<	0)	{
								std::cout	<<	"Failed	to	read	from	socket"	<<	std::endl;
				}	else	if	(numBytes	<	SOCKET_BUF_SIZE)	{
								buffer[numBytes]	=	'\0';
				}

				return	std::string(buffer);
}

void	Socket::operator>>	(std::string&	ret)	{
				ret	=	receive();
}