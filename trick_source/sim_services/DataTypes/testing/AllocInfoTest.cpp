#include	<gtest/gtest.h>
#include	<stddef.h>
#include	<iostream>
#include	"AllocInfo.hh"
#include	"TypeDictionary.hh"
#include	"DataType.hh"

//	Framework
class	AllocInfoTest	:	public	::testing::Test	{
				protected:
				TypeDictionary	*typeDictionary;
				AllocInfoTest()	{	typeDictionary	=	new	TypeDictionary;	}
				~AllocInfoTest()	{	delete	typeDictionary;	}
				void	SetUp()	{}
				void	TearDown()	{}
};

/*	================================================================================
																																									Test	Cases
			================================================================================
*/
TEST_F(AllocInfoTest,	instantiation1)	{

				bool	construction_result	=	true;
				try	{
								AllocInfo	*	allocInfo	=	new	AllocInfo("Apple",	"double",	0,	NULL,	typeDictionary);
								std::cout	<<	allocInfo->toString();
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				EXPECT_EQ(true,	construction_result);
}

TEST_F(AllocInfoTest,	instantiation2)	{

				bool	construction_result	=	true;
				try	{
								int	dimSize[]	=	{4};
								AllocInfo	*	allocInfo	=	new	AllocInfo("Apple",	"double",	1,	dimSize,	typeDictionary);
								std::cout	<<	allocInfo->toString();
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				EXPECT_EQ(true,	construction_result);
}


TEST_F(AllocInfoTest,	instantiation3)	{

				bool	construction_result	=	true;
				try	{
								int	dimSize[]	=	{7,-1};
								AllocInfo	*	allocInfo	=	new	AllocInfo("",	"double",	2,	dimSize,	typeDictionary);
								std::cout	<<	allocInfo->toString();
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				EXPECT_EQ(true,	construction_result);
}

TEST_F(AllocInfoTest,	getters1)	{

				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;

				try	{
								int	dimSize[]	=	{7,-1};
								allocInfo	=	new	AllocInfo("",	"double",	2,	dimSize,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				EXPECT_EQ("",																		allocInfo->getName());
				EXPECT_NE((void*)NULL,									allocInfo->getStart());
				EXPECT_EQ((size_t)7*sizeof(void*),																			(size_t)allocInfo->getSize());
				EXPECT_EQ("double",												allocInfo->getTypeSpecifierName());
				EXPECT_EQ(2,																			allocInfo->getDimensionsCount());
				EXPECT_EQ(7,																			allocInfo->getDimensionSize(0));
				EXPECT_EQ(-1,																		allocInfo->getDimensionSize(1));
				EXPECT_EQ(StorageClass::LOCAL,	allocInfo->getStorageClass());
}

TEST_F(AllocInfoTest,	getters2)	{

				int	A[3][7];

				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;

				try	{
								int	dimSize[]	=	{3,7};
								allocInfo	=	new	AllocInfo("Orange",	"int",	2,	dimSize,	typeDictionary,	A);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				EXPECT_EQ("Orange",													allocInfo->getName());
				EXPECT_EQ((void*)&A,												allocInfo->getStart());
				EXPECT_EQ(sizeof(A),												(size_t)allocInfo->getSize());
				EXPECT_EQ("int",																allocInfo->getTypeSpecifierName());
				EXPECT_EQ(2,																				allocInfo->getDimensionsCount());
				EXPECT_EQ(3,																				allocInfo->getDimensionSize(0));
				EXPECT_EQ(7,																				allocInfo->getDimensionSize(1));
				EXPECT_EQ(StorageClass::EXTERN,	allocInfo->getStorageClass());
}

TEST_F(AllocInfoTest,	clear)	{

				double	A[3][2]	=	{	{11.0,	12.0},
																							{21.0,	22.0},
																							{31.0,	32.0}	};

				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								allocInfo	=	new	AllocInfo(	"double	Banana[3][2]",	typeDictionary,	A);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}

				ASSERT_EQ(true,	construction_result);
				EXPECT_NEAR(11.0,	A[0][0],	0.0000001);
				EXPECT_NEAR(12.0,	A[0][1],	0.0000001);
				EXPECT_NEAR(21.0,	A[1][0],	0.0000001);
				EXPECT_NEAR(22.0,	A[1][1],	0.0000001);
				EXPECT_NEAR(31.0,	A[2][0],	0.0000001);
				EXPECT_NEAR(32.0,	A[2][1],	0.0000001);

				allocInfo->clear();

				EXPECT_NEAR(0.0,	A[0][0],	0.0000001);
				EXPECT_NEAR(0.0,	A[0][1],	0.0000001);
				EXPECT_NEAR(0.0,	A[1][0],	0.0000001);
				EXPECT_NEAR(0.0,	A[1][1],	0.0000001);
				EXPECT_NEAR(0.0,	A[2][0],	0.0000001);
				EXPECT_NEAR(0.0,	A[2][1],	0.0000001);

				EXPECT_EQ(true,	construction_result);
}
