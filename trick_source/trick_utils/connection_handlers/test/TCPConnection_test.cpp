
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
#include	"trick/TCPConnection.hh"
#include	"SystemInterfaceMock/SystemInterfaceMock.hh"


class	TCPConnectionTest	:	public	testing::Test	{

			protected:
						TCPConnectionTest()	:	system_context(new	SystemInferfaceMock()),	connection	(fake_listen_socket_fd,	system_context)	{}
						~TCPConnectionTest(){}

						SystemInferfaceMock	*	system_context;
						Trick::TCPConnection	connection;
						const	static	int	fake_listen_socket_fd;
};

const	int	TCPConnectionTest::fake_listen_socket_fd	=	5;

TEST_F(	TCPConnectionTest,	initialized	)	{
				//	ARRANGE
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();

				//	ACT
				int	result	=	connection.start();

				//	ASSERT
				ASSERT_EQ(result,	0);
}

TEST_F(	TCPConnectionTest,	initialized_accept_failure	)	{
				//	ARRANGE
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								errno	=	EWOULDBLOCK;
								return	-1;
				});

				//	ACT
				int	result	=	connection.start();

				//	ASSERT
				ASSERT_EQ(result,	-1);
}


TEST_F(	TCPConnectionTest,	setBlockMode_nonblocking)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								socket_fd	=	::socket(AF_INET,	SOCK_STREAM,	0);
								return	socket_fd;
				});
				connection.start();

				//	ACT
				int	status	=	connection.setBlockMode(false);

				//	ASSERT
				EXPECT_EQ(status,	0);

				int	flag	=	fcntl(socket_fd,	F_GETFL,	0);
				EXPECT_TRUE(flag	&	O_NONBLOCK);

				close(socket_fd);
}


TEST_F(	TCPConnectionTest,	setBlockMode_blocking)	{
//	ARRANGE
				int	socket_fd;
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								socket_fd	=	::socket(AF_INET,	SOCK_STREAM,	0);
								return	socket_fd;
				});
				connection.start();

				//	ACT
				int	status	=	connection.setBlockMode(true);

				//	ASSERT
				EXPECT_EQ(status,	0);

				int	flag	=	fcntl(socket_fd,	F_GETFL,	0);
				EXPECT_FALSE(flag	&	O_NONBLOCK);

				close(socket_fd);
}

TEST_F(	TCPConnectionTest,	setBlockMode_fcntl_getfl_fail)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								socket_fd	=	::socket(AF_INET,	SOCK_STREAM,	0);
								return	socket_fd;
				});
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

TEST_F(	TCPConnectionTest,	setBlockMode_fcntl_setfl_fail)	{
				//	ARRANGE
				int	socket_fd;
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								socket_fd	=	::socket(AF_INET,	SOCK_STREAM,	0);
								return	socket_fd;
				});

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

TEST_F(	TCPConnectionTest,	write_string	)	{
				//	ARRANGE
				char	sent_data[100];
				memset(sent_data,	0,	100);
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();
				system_context->register_send_impl([&](int	socket,	const	void	*	buffer,	size_t	length,	int	flags)	->	ssize_t	{
								memcpy	(sent_data,	buffer,	length);
								return	length;
				});
				connection.start();

				//	ACT
				std::string	str	=	"This	is	a	message	to	write";
				int	result	=	connection.write(str);

				//	ASSERT
				ASSERT_EQ(result,	str.length());
				ASSERT_STREQ(str.c_str(),	sent_data);
}

TEST_F(	TCPConnectionTest,	write_binary_buf	)	{
				//	ARRANGE
				char	to_send[8]	=	{0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07};
				char	sent_data[100];

				memset(sent_data,	0,	100);
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();
				system_context->register_send_impl([&](int	socket,	const	void	*	buffer,	size_t	length,	int	flags)	->	ssize_t	{
								memcpy	(sent_data,	buffer,	length);
								return	length;
				});
				connection.start();

				//	ACT
				int	result	=	connection.write(to_send,	sizeof(to_send));

				//	ASSERT
				ASSERT_EQ(result,	sizeof(to_send));
				for	(int	i	=	0;	i	<	8;	i++)	{
								ASSERT_EQ(to_send[i],	sent_data[i]);
				}
}


TEST_F(	TCPConnectionTest,	write_string_uninitialized	)	{
				//	ARRANGE
				std::string	str	=	"This	is	a	message	to	write";

				//	ACT
				int	result	=	connection.write(str);

				//	ASSERT
				ASSERT_EQ(result,	-1);
}

TEST_F(	TCPConnectionTest,	write_binary_buf_uninitialized	)	{
				//	ARRANGE
				char	to_send[8]	=	{0x00,	0x01,	0x02,	0x03,	0x04,	0x05,	0x06,	0x07};

				//	ACT
				int	result	=	connection.write(to_send,	sizeof(to_send));

				//	ASSERT
				ASSERT_EQ(result,	-1);
}


TEST_F(	TCPConnectionTest,	read_nonewline	)	{
				//	ARRANGE
				std::string	data_to_read	=	"Here	is	an	incomplete	message	from	a	socket";
				
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();
				system_context->register_recv_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags)	->	ssize_t	{
								int	data_to_read_size	=	data_to_read.size()	<	length	?	data_to_read.size()	:	length;

								memcpy	(buffer,	data_to_read.c_str(),	data_to_read_size);
								return	data_to_read_size;
				});
				connection.start();

				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				ASSERT_EQ(result,	std::string(""));
				ASSERT_EQ(nbytes,	0);
}


TEST_F(	TCPConnectionTest,	read	)	{
				//	ARRANGE
				std::string	data_to_read	=	"Here	is	a	complete	message	from	a	socket\n	This	part	is	incomplete";
				
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();
				system_context->register_recv_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags)	->	ssize_t	{
								int	data_to_read_size	=	data_to_read.size()	<	length	?	data_to_read.size()	:	length;
								memcpy	(buffer,	data_to_read.c_str(),	data_to_read_size);
								return	data_to_read_size;
				});
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


TEST_F(	TCPConnectionTest,	read_nodata	)	{
				//	ARRANGE				
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();
				system_context->register_recv_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags)	->	ssize_t	{
								errno	=	EAGAIN;
								return	-1;
				});
				connection.start();

				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);


				//	ASSERT
				std::string	expected	=	"";
				ASSERT_EQ(result,	expected);
				EXPECT_EQ(nbytes,	0);
}

TEST_F(	TCPConnectionTest,	read_other_error	)	{
				//	ARRANGE				
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	6;
				});
				system_context->noop_fcntl_impl();
				system_context->register_recv_impl([&](int	socket,	void	*	buffer,	size_t	length,	int	flags)	->	ssize_t	{
								errno	=	EBADF;
								return	-1;
				});

				connection.start();

				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				std::string	expected	=	"";
				ASSERT_EQ(result,	expected);
				EXPECT_EQ(nbytes,	-1);

}



TEST_F(	TCPConnectionTest,	read_uninitialized	)	{
				//	ARRANGE
				//	ACT
				std::string	result;
				int	nbytes	=	connection.read(result);

				//	ASSERT
				std::string	expected	=	"";
				ASSERT_EQ(result,	expected);
				ASSERT_EQ(nbytes,	0);

}


TEST_F(	TCPConnectionTest,	disconnect	)	{
				//	ARRANGE
				int	socket_fd	=	6;
				int	close_called	=	0;
				int	shutdown_called	=	0;
				system_context->register_accept_impl([&](int	socket,	struct	sockaddr	*	address,	socklen_t	*	address_len)	->	int	{
								return	socket_fd;
				});
				system_context->register_close_impl([&](int	socket)	{
								if	(socket	==	socket_fd)
												close_called++;
								return	0;
				});
				system_context->register_shutdown_impl([&](int	socket,	int	how)	{
								if	(socket	==	socket_fd)
												shutdown_called++;
								return	0;
				});
				system_context->noop_fcntl_impl();
				connection.start();

				//	ACT
				int	result	=	connection.disconnect();

				//	ASSERT
				ASSERT_EQ(result,	0);
				ASSERT_EQ(close_called,	1);
				ASSERT_EQ(shutdown_called,	1);

				//	Don't	shut	down	if	already	shut	down
				//	ACT
				result	=	connection.disconnect();

				//	ASSERT
				ASSERT_EQ(result,	-1);
				ASSERT_EQ(close_called,	1);
				ASSERT_EQ(shutdown_called,	1);

}