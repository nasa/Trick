
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/attributes.h"
#include	"trick/tc_proto.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"

class	TCErrorTest	:	public	testing::Test	{

			protected:
						TCErrorTest(){}
						~TCErrorTest(){}

						TCDevice*	device;

						void	SetUp(){

									device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)device,'\0',sizeof(TCDevice)	);
						}

						void	TearDown(){

									free(device);
						}
};

TEST_F(	TCErrorTest,	testNoDevice	)	{

			int	error_status	=	tc_error(	NULL,	0	);

			EXPECT_EQ(	error_status,	-1	);
}

TEST_F(	TCErrorTest,	testNoErrorHandler	)	{

			device->error_handler	=	NULL;

			int	error_status	=	tc_error(	device,	0	);

			EXPECT_EQ(	error_status,	0	);
}

TEST_F(	TCErrorTest,	testOn	)	{

			int	error_status	=	tc_error(	device,	1	);

			EXPECT_EQ(	device->error_handler->report_level,	TRICK_ERROR_ALL	);
			EXPECT_EQ(	error_status,	0	);
}

TEST_F(	TCErrorTest,	testOff	)	{

			int	error_status	=	tc_error(	device,	0	);

			EXPECT_EQ(	device->error_handler->report_level,	TRICK_ERROR_SILENT	);
			EXPECT_EQ(	error_status,	0	);
}

