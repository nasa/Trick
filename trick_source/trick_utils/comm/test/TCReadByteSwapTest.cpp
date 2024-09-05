
#include	<gtest/gtest.h>

#include	"trick/tc.h"
#include	"trick/attributes.h"
#include	"trick/tc_proto.h"
#include	"trick/trick_byteswap.h"
#include	"trick/trick_error_hndlr.h"


class	TCReadByteSwapTest	:	public	testing::Test	{

			protected:
						TCReadByteSwapTest(){}
						~TCReadByteSwapTest(){}

						TCDevice*	device;

						void	SetUp(){

									device	=	(TCDevice	*)	malloc(sizeof(TCDevice));
									memset(	(void	*)device,'\0',sizeof(TCDevice)	);
						}

						void	TearDown(){

									free(device);
						}
};
