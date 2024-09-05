
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/tc_proto.h"
#include	"trick/attributes.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"

class	TCIsValidTest	:	public	testing::Test	{

			protected:
						TCIsValidTest(){}
						~TCIsValidTest(){}

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

TEST_F(	TCIsValidTest,	testNullDevice	)	{

			int	tcisvalid_status	=	tc_isValid(	NULL	);

			EXPECT_EQ(	tcisvalid_status,	0	);
}

TEST_F(	TCIsValidTest,	testInvalidSocket	)	{

			device->socket	=	TRICKCOMM_INVALID_SOCKET;

			int	tcisvalid_status	=	tc_isValid(	device	);

			EXPECT_EQ(	tcisvalid_status,	0	);
}

TEST_F(	TCIsValidTest,	testValidSocket	)	{

			int	tcisvalid_status	=	tc_isValid(	device	);

			EXPECT_EQ(	tcisvalid_status,	0	);
}

