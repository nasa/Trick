#include	<gtest/gtest.h>

#include	"trick/MulticastGroup.hh"
#include	"SystemInterfaceMock/SystemInterfaceMock.hh"

class	MulticastGroupTest	:	public	testing::Test	{

			protected:
						MulticastGroupTest()	:	system_context(new	SystemInferfaceMock()),	mcast	(system_context)	{}
						~MulticastGroupTest(){
								mcast.disconnect();
						}

						SystemInferfaceMock	*	system_context;
						Trick::MulticastGroup	mcast;
};

TEST_F(MulticastGroupTest,	not_initialized)	{
				//	ARRANGE
				//	ACT
				//	ASSERT
				EXPECT_EQ(mcast.isInitialized(),	0);
}

TEST_F(MulticastGroupTest,	initialize)	{
				//	ARRANGE
				//	ACT
				mcast.initialize();
				//	ASSERT
				EXPECT_EQ(mcast.isInitialized(),	1);
}

TEST_F(MulticastGroupTest,	initialize_socket_fails)	{
				//	ARRANGE
				system_context->register_socket_impl([](int	a,	int	b,	int	c)	{	
								errno	=	EPERM;
								return	-1;	
				});

				//	ACT
				mcast.initialize();
				//	ASSERT
				EXPECT_EQ(mcast.isInitialized(),	0);
}

TEST_F(MulticastGroupTest,	initialize_sockopt_reuseaddr_fails)	{
				//	ARRANGE
				system_context->register_setsockopt_impl([](int	sockfd,	int	level,	int	optname,	const	void	*optval,	socklen_t	optlen)	{	
								if	(optname	==	SO_REUSEADDR)	{
												errno	=	EINVAL;
												return	-1;	
								}
								return	0;
				});

				//	ACT
				mcast.initialize();
				//	ASSERT
				EXPECT_EQ(mcast.isInitialized(),	0);
}

TEST_F(MulticastGroupTest,	initialize_sockopt_reuseport_fails)	{
				//	ARRANGE
				system_context->register_setsockopt_impl([](int	sockfd,	int	level,	int	optname,	const	void	*optval,	socklen_t	optlen)	{	
								if	(optname	==	SO_REUSEPORT)	{
												errno	=	EINVAL;
												return	-1;	
								}
								return	0;
				});

				//	ACT
				mcast.initialize();
				//	ASSERT
				EXPECT_EQ(mcast.isInitialized(),	0);
}

TEST_F(MulticastGroupTest,	add_address)	{
				//	ARRANGE
				//	ACT
				int	result	=	mcast.addAddress("239.3.14.15",	9265);

				//	ASSERT
				EXPECT_EQ(result,	0);
}

TEST_F(MulticastGroupTest,	add_address_fails)	{
				//	ARRANGE
				system_context->register_inet_addr_impl([](const	char	*	addr)	{
								return	-1;
				});

				//	ACT
				int	result	=	mcast.addAddress("239.3.14.15",	9265);

				//	ASSERT
				EXPECT_EQ(result,	-1);
}

TEST_F(MulticastGroupTest,	broadcast_uninitialized)	{
				//	ARRANGE
				//	ACT
				int	result	=	mcast.broadcast("Some	message");

				//	ASSERT
				EXPECT_EQ(result,	-1);
}

TEST_F(MulticastGroupTest,	broadcast)	{
				//	ARRANGE
				mcast.initialize();
				mcast.addAddress("239.3.14.15",	9265);

				//	ACT
				int	result	=	mcast.broadcast("Some	message");

				//	ASSERT
				EXPECT_EQ(result,	0);
}


TEST_F(MulticastGroupTest,	broadcast_send_fails)	{
				//	ARRANGE
				system_context->register_sendto_impl([](int	socket,	const	void	*	buffer,	size_t	length,	int	flags,	const	struct	sockaddr	*	dest_addr,	socklen_t	dest_len)	{
								return	-1;
				});
				
				mcast.initialize();
				mcast.addAddress("239.3.14.15",	9265);

				//	ACT
				int	result	=	mcast.broadcast("Some	message");

				//	ASSERT
				EXPECT_EQ(result,	0);
}