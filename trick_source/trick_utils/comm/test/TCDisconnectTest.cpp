
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/attributes.h"
#include	"trick/tc_proto.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"


class	TCDisconnectTest	:	public	testing::Test	{

			protected:
						TCDisconnectTest(){}
						~TCDisconnectTest(){}

						TCDevice*	device;

						void	SetUp(){

									device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)device,'\0',sizeof(TCDevice)	);
						}

						void	TearDown(){

									free(device);
						}
};

TEST_F(	TCDisconnectTest,	testNoDevice	)	{

			int	disconnect_status	=	tc_disconnect(	NULL	);

			EXPECT_EQ(	disconnect_status,	TC_DRIVER_ALREADY_DISCONNECTED	);
}

TEST_F(	TCDisconnectTest,	testSuccess	)	{

			int	disconnect_status	=	tc_disconnect(	device	);

			EXPECT_EQ(	disconnect_status,	TC_SUCCESS	);
}
