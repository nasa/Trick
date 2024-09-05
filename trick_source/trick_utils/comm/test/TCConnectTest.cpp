
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/tc_proto.h"
#include	"trick/attributes.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"

class	TCConnectTest	:	public	testing::Test	{

			protected:
						TCConnectTest(){}
						~TCConnectTest(){}

						TCDevice*	device;

						void	SetUp(){

									/*	device	*/
									device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)device,'\0',sizeof(TCDevice)	);
									device->hostname	=	strdup("127.0.0.1");
									device->disabled	=	TC_COMM_FALSE;
									device->disable_handshaking	=	TC_COMM_DISABLED;
									strcpy(device->client_tag,	"<empty>");

						}

						void	TearDown(){

									free(device->hostname);
									free(device);
						}
};


TEST_F(	TCConnectTest,	testNoDevice	)	{

			int	connect_status	=	tc_connect(	NULL	);

			EXPECT_EQ(	connect_status,	-1	);
}

TEST_F(	TCConnectTest,	testDisabledDevice	)	{

			device->disabled	=	TC_COMM_TRUE;

			int	connect_status	=	tc_connect(	device	);

			EXPECT_EQ(	connect_status,	TC_CONN_DISABLED	);

}

TEST_F(	TCConnectTest,	testNullHostname	)	{

			device->hostname	=	NULL;

			int	connect_status	=	tc_connect(	device	);

			EXPECT_EQ(	connect_status,	TC_CONN_DISABLED	);
}

