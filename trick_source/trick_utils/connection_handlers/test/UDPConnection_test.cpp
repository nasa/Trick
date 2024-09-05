
#include	<gtest/gtest.h>
#include	<errno.h>
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

#include	"trick/UDPConnection.hh"
#include	"SystemInterfaceMock/SystemInterfaceMock.hh"


class	UDPConnectionTest	:	public	testing::Test	{

			protected:
						UDPConnectionTest()	:	system_context(new	SystemInferfaceMock()),	connection	(system_context)	{}
						~UDPConnectionTest()	{}

						SystemInferfaceMock	*	system_context;
						Trick::UDPConnection	connection;
};



TEST_F(	UDPConnectionTest,	initialize_localhost_0	)	{
				//	ARRANGE
				char	name[80];
				gethostname(name,	(size_t)	80);

				//	ACT
				connection.initialize("localhost",	0);
				connection.start();

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	true);
				EXPECT_EQ(connection.getHostname(),	std::string(name));
}

TEST_F(	UDPConnectionTest,	initialize_localhost_54321	)	{
				//	ARRANGE
				//	ACT
				connection.initialize("localhost",	54321);
				connection.start();

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	true);
				EXPECT_EQ(connection.getPort(),	54321);
}

TEST_F(	UDPConnectionTest,	initialize_localhost_numerical_54321	)	{
				//	ARRANGE
				//	ACT
				connection.initialize("127.0.0.1",	12345);
				connection.start();

				//	ASSERT
				EXPECT_EQ(connection.getPort(),	12345);
}

TEST_F(	UDPConnectionTest,	initialize_invalid_hostname	)	{
				//	ARRANGE
				//	ACT
				connection.initialize("some_invalid_hostname",	0);
				connection.start();

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	false);
}

TEST_F(	UDPConnectionTest,	failed_socket	)	{
				//	ARRANGE
				system_context->register_socket_impl([](int	a,	int	b,	int	c)	{	
								errno	=	EPERM;
								return	-1;	
				});

				//	ACT
				connection.initialize("localhost",	54321);

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	false);
}

TEST_F(	UDPConnectionTest,	failed_setsockopt_reuseaddr	)	{
				//	ARRANGE
				system_context->register_setsockopt_impl([](int	sockfd,	int	level,	int	optname,	const	void	*optval,	socklen_t	optlen)	{	
								errno	=	EINVAL;
								return	-1;	
				});

				//	ACT
				connection.initialize("localhost",	54321);

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	false);
}

TEST_F(	UDPConnectionTest,	failed_bind	)	{
				//	ARRANGE
				system_context->register_bind_impl([](int	sockfd,	const	struct	sockaddr	*addr,socklen_t	addrlen)	{	
								errno	=	EADDRINUSE;
								return	-1;
				});

				//	ACT
				connection.initialize("localhost",	54321);

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	false);
}

TEST_F(	UDPConnectionTest,	failed_sockname	)	{
				//	ARRANGE
				system_context->register_getsockname_impl([](int	sockfd,	struct	sockaddr	*addr,	socklen_t	*addrlen)	{
								((struct	sockaddr_in	*)	addr)->sin_port	=	htons(1234);
								return	0;
				});

				//	ACT
				connection.initialize("localhost",	3412);

				//	ASSERT
				EXPECT_EQ(connection.isInitialized(),	false);
}


TEST_F(	UDPConnectionTest,	setBlockMode_nonblocking)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_socket_impl([&socket_fd](int	a,	int	b,	int	c)	{
								socket_fd	=	::socket(a,	b,	c);
								return	socket_fd;
				});

				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				int	status	=	connection.setBlockMode(false);

				//	ASSERT
				EXPECT_EQ(status,	0);

				int	flag	=	fcntl(socket_fd,	F_GETFL,	0);
				EXPECT_TRUE(flag	&	O_NONBLOCK);

				close(socket_fd);
}


TEST_F(	UDPConnectionTest,	setBlockMode_blocking)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_socket_impl([&socket_fd](int	a,	int	b,	int	c)	{
								socket_fd	=	::socket(a,	b,	c);
								return	socket_fd;
				});

				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				int	status	=	connection.setBlockMode(true);

				//	ASSERT
				EXPECT_EQ(status,	0);

				int	flag	=	fcntl(socket_fd,	F_GETFL,	0);
				EXPECT_FALSE(flag	&	O_NONBLOCK);

				close(socket_fd);
}

TEST_F(	UDPConnectionTest,	setBlockMode_fcntl_getfl_fail)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_socket_impl([&socket_fd](int	a,	int	b,	int	c)	{
								socket_fd	=	::socket(a,	b,	c);
								return	socket_fd;
				});

				connection.initialize("localhost",	0);
				connection.start();

				system_context->register_fcntl_impl([](int	a,	int	b,	int	c)	{
								errno	=	EACCES;
								return	-1;
				});

				//	ACT
				
				int	status	=	connection.setBlockMode(true);

				//	ASSERT
				EXPECT_EQ(status,	-1);

				close(socket_fd);
}

TEST_F(	UDPConnectionTest,	setBlockMode_fcntl_setfl_fail)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_socket_impl([&socket_fd](int	a,	int	b,	int	c)	{
								socket_fd	=	::socket(a,	b,	c);
								return	socket_fd;
				});

				connection.initialize("localhost",	0);
				connection.start();

				system_context->register_fcntl_impl([](int	a,	int	cmd,	int	c)	{
								if	(cmd	==	F_SETFL)	{
												errno	=	EBADF;
												return	-1;
								}
								return	0;
				});

				//	ACT
				int	status	=	connection.setBlockMode(true);

				//	ASSERT
				EXPECT_EQ(status,	-1);

				close(socket_fd);
}


TEST_F(	UDPConnectionTest,	write_string	)	{
				//	ARRANGE
				char	sent_data[100];
				memset(sent_data,	0,	100);
				system_context->noop_fcntl_impl();
				system_context->register_sendto_impl([&](int	socket,	const	void	*	buffer,	size_t	length,	int	flags,	const	struct	sockaddr	*	dest_addr,	socklen_t	dest_len)	->	ssize_t	{
								memcpy	(sent_data,	buffer,	length);
								return	length;
				});
				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				std::string	str	=	"This	is	a	message	to	write";
				int	result	=	connection.write(str);

				//	ASSERT
				ASSERT_EQ(result,	str.length());
				ASSERT_STREQ(str.c_str(),	sent_data);
}

TEST_F(	UDPConnectionTest,	write_binary_buf	)	{
				//	ARRANGE
				char	to_send[8]	=	{0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07};
				char	sent_data[100];

				memset(sent_data,	0,	100);
				system_context->noop_fcntl_impl();
				system_context->register_sendto_impl([&](int	socket,	const	void	*	buffer,	size_t	length,	int	flags,	const	struct	sockaddr	*	dest_addr,	socklen_t	dest_len)	->	ssize_t	{
								memcpy	(sent_data,	buffer,	length);
								return	length;
				});
				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				int	result	=	connection.write(to_send,	sizeof(to_send));

				//	ASSERT
				ASSERT_EQ(result,	sizeof(to_send));
				for	(int	i	=	0;	i	<	8;	i++)	{
								ASSERT_EQ(to_send[i],	sent_data[i]);
				}
}


TEST_F(	UDPConnectionTest,	write_string_uninitialized	)	{
				//	ARRANGE
				std::string	str	=	"This	is	a	message	to	write";

				//	ACT
				int	result	=	connection.write(str);

				//	ASSERT
				ASSERT_EQ(result,	-1);
}

TEST_F(	UDPConnectionTest,	write_binary_buf_uninitialized	)	{
				//	ARRANGE
				char	to_send[8]	=	{0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07};

				//	ACT
				int	result	=	connection.write(to_send,	sizeof(to_send));

				//	ASSERT
				ASSERT_EQ(result,	-1);
}


TEST_F(	UDPConnectionTest,	read_nonewline	)	{
				//	ARRANGE
				std::string	data_to_read	=	"Here	is	an	incomplete	message	from	a	socket";
				
				system_context->noop_fcntl_impl();
				system_context->register_recvfrom_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	ssize_t	{
								int	data_to_read_size	=	data_to_read.size()	<	length	?	data_to_read.size()	:	length;

								memcpy	(buffer,	data_to_read.c_str(),	data_to_read_size);
								return	data_to_read_size;
				});
				connection.initialize("localhost",	0);
				connection.start();


				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				ASSERT_EQ(result,	std::string(""));
				ASSERT_EQ(nbytes,	0);
}


TEST_F(	UDPConnectionTest,	read	)	{
				//	ARRANGE
				std::string	data_to_read	=	"Here	is	a	complete	message	from	a	socket\n	This	part	is	incomplete";
				
				system_context->noop_fcntl_impl();
				system_context->register_recvfrom_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	ssize_t	{
								int	data_to_read_size	=	data_to_read.size()	<	length	?	data_to_read.size()	:	length;
								memcpy	(buffer,	data_to_read.c_str(),	data_to_read_size);
								return	data_to_read_size;
				});
				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				std::string	expected	=	"Here	is	a	complete	message	from	a	socket\n";
				expected	+=	'\0';
				ASSERT_EQ(result,	expected);
				ASSERT_EQ(nbytes,	expected.size());

}


TEST_F(	UDPConnectionTest,	read_nodata	)	{
				//	ARRANGE				
				system_context->noop_fcntl_impl();
				system_context->register_recvfrom_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	ssize_t	{
								errno	=	EAGAIN;
								return	-1;
				});
				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				std::string	expected	=	"";
				ASSERT_EQ(result,	expected);
				ASSERT_EQ(nbytes,	0);
}

TEST_F(	UDPConnectionTest,	read_other_error	)	{
				//	ARRANGE				
				system_context->noop_fcntl_impl();
				system_context->register_recvfrom_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	ssize_t	{
								errno	=	EBADF;
								return	-1;
				});
				connection.initialize("localhost",	0);
				connection.start();

				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				std::string	expected	=	"";
				ASSERT_EQ(result,	expected);
				ASSERT_EQ(nbytes,	-1);

}

TEST_F(	UDPConnectionTest,	read_uninitialized	)	{
				//	ARRANGE
				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				std::string	expected	=	"";
				ASSERT_EQ(result,	expected);
				ASSERT_EQ(nbytes,	0);
}
