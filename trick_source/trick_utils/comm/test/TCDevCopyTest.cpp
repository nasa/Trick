
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/attributes.h"
#include	"trick/tc_proto.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"


class	TCDevCopyTest	:	public	testing::Test	{

			protected:
						TCDevCopyTest(){}
						~TCDevCopyTest(){}

						TCDevice*	src_device;
						TCDevice*	dest_device;

						void	SetUp(){

									src_device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)src_device,'\0',sizeof(TCDevice)	);

									dest_device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)dest_device,'\0',sizeof(TCDevice)	);
						}

						void	TearDown(){

									free(src_device);
									free(dest_device);
						}
};

TEST_F(	TCDevCopyTest,	testNullSrcDest	)	{

			int	tcdev_status	=	tc_dev_copy(	NULL,	NULL	);

			EXPECT_EQ(	tcdev_status,	-1	);
}

TEST_F(	TCDevCopyTest,	testNullSrc	)	{

			int	tcdev_status	=	tc_dev_copy(	dest_device,	NULL	);

			EXPECT_EQ(	tcdev_status,	-1	);
}

TEST_F(	TCDevCopyTest,	testNullDest	)	{

			int	tcdev_status	=	tc_dev_copy(	NULL,	src_device	);

			EXPECT_EQ(	tcdev_status,	-1	);
}
