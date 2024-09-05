
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/attributes.h"
#include	"trick/tc_proto.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"

class	TCPendingTest	:	public	testing::Test	{

			protected:
						TCPendingTest(){}
						~TCPendingTest(){}

						TCDevice*	device;

						void	SetUp(){

									device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)device,'\0',sizeof(TCDevice)	);

									device->socket	=	1;
						}

						void	TearDown(){

									free(device);
						}
};

TEST_F(	TCPendingTest,	testNullDevice	)	{

			int	tcpending_status	=	tc_pending(	NULL	);

			EXPECT_EQ(	tcpending_status,	-1	);
}

/*
TEST_F(	TCPendingTest,	testInvalidSocket	)	{

			device->socket	=	TRICKCOMM_INVALID_SOCKET;

			int	tcpending_status	=	tc_pending(	device	);

			EXPECT_EQ(	tcpending_status,	0	);
}
*/
