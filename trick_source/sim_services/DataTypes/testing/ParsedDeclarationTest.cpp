#include	<gtest/gtest.h>
#include	<stddef.h>
#include	"ParsedDeclaration.hh"
#include	<iostream>

TEST(ParsedDeclaration,	test1)	{

				std::cout	<<	"TEST1:	float*baz[3]"	<<	std::endl;
				ParsedDeclaration*	parsedDeclaration	=	new	ParsedDeclaration("float*baz[3]");

				std::string	typeSpecString	=	parsedDeclaration->getTypeSpecifier();
				EXPECT_EQ(	"float",	typeSpecString	);

				std::string	varName	=	parsedDeclaration->getVariableName();
				EXPECT_EQ(	"baz",	varName	);

				unsigned	int	ndims	=	parsedDeclaration->getNumberOfDimensions();
				unsigned	int	dim_0	=	parsedDeclaration->getDimensionSize(0);
				unsigned	int	dim_1	=	parsedDeclaration->getDimensionSize(1);

				EXPECT_EQ(	2,	ndims	);
				EXPECT_EQ(	3,	dim_0	);
				EXPECT_EQ(-1,	dim_1	);
}


TEST(ParsedDeclaration,	test2)	{

				std::cout	<<	"TEST2:	float(*)[3]"	<<	std::endl;
				ParsedDeclaration*	parsedDeclaration	=	new	ParsedDeclaration("float(*)[3]");

				std::string	typeSpecString	=	parsedDeclaration->getTypeSpecifier();
				EXPECT_EQ(	"float",	typeSpecString	);

				std::string	varName	=	parsedDeclaration->getVariableName();
				EXPECT_EQ(	"",	varName	);

				unsigned	int	ndims	=	parsedDeclaration->getNumberOfDimensions();
				ASSERT_EQ(	2,	ndims	);

				unsigned	int	dim_0	=	parsedDeclaration->getDimensionSize(0);
				unsigned	int	dim_1	=	parsedDeclaration->getDimensionSize(1);

				EXPECT_EQ(-1,	dim_0	);
				EXPECT_EQ(	3,	dim_1	);
}


TEST(ParsedDeclaration,	test3)	{

				ParsedDeclaration*	parsedDeclaration	=	new	ParsedDeclaration("int**[3][4]");

				std::string	typeSpecString	=	parsedDeclaration->getTypeSpecifier();
				EXPECT_EQ(	"int",	typeSpecString	);

				std::string	varName	=	parsedDeclaration->getVariableName();
				EXPECT_EQ(	"",	varName	);

				unsigned	int	ndims	=	parsedDeclaration->getNumberOfDimensions();
				ASSERT_EQ(	4,	ndims	);

				unsigned	int	dim_0	=	parsedDeclaration->getDimensionSize(0);
				unsigned	int	dim_1	=	parsedDeclaration->getDimensionSize(1);
				unsigned	int	dim_2	=	parsedDeclaration->getDimensionSize(2);
				unsigned	int	dim_3	=	parsedDeclaration->getDimensionSize(3);

				EXPECT_EQ(	3,	dim_0	);
				EXPECT_EQ(	4,	dim_1	);
				EXPECT_EQ(-1,	dim_2	);
				EXPECT_EQ(-1,	dim_3	);
}

TEST(ParsedDeclaration,	test4)	{

				ParsedDeclaration*	parsedDeclaration	=	new	ParsedDeclaration("int*(*foo)[3][4]");

				std::string	typeSpecString	=	parsedDeclaration->getTypeSpecifier();
				EXPECT_EQ(	"int",	typeSpecString	);

				std::string	varName	=	parsedDeclaration->getVariableName();
				EXPECT_EQ(	"foo",	varName	);

				unsigned	int	ndims	=	parsedDeclaration->getNumberOfDimensions();
				ASSERT_EQ(	4,	ndims	);

				unsigned	int	dim_0	=	parsedDeclaration->getDimensionSize(0);
				unsigned	int	dim_1	=	parsedDeclaration->getDimensionSize(1);
				unsigned	int	dim_2	=	parsedDeclaration->getDimensionSize(2);
				unsigned	int	dim_3	=	parsedDeclaration->getDimensionSize(3);

				EXPECT_EQ(-1,	dim_0	);
				EXPECT_EQ(	3,	dim_1	);
				EXPECT_EQ(	4,	dim_2	);
				EXPECT_EQ(-1,	dim_3	);
}

TEST(ParsedDeclaration,	test5)	{

				ParsedDeclaration*	parsedDeclaration	=	new	ParsedDeclaration("int*(*)[3][4]");

				std::string	typeSpecString	=	parsedDeclaration->getTypeSpecifier();
				EXPECT_EQ(	"int",	typeSpecString	);

				std::string	varName	=	parsedDeclaration->getVariableName();
				EXPECT_EQ(	"",	varName	);

				unsigned	int	ndims	=	parsedDeclaration->getNumberOfDimensions();
				ASSERT_EQ(	4,	ndims	);

				unsigned	int	dim_0	=	parsedDeclaration->getDimensionSize(0);
				unsigned	int	dim_1	=	parsedDeclaration->getDimensionSize(1);
				unsigned	int	dim_2	=	parsedDeclaration->getDimensionSize(2);
				unsigned	int	dim_3	=	parsedDeclaration->getDimensionSize(3);

				EXPECT_EQ(-1,	dim_0	);
				EXPECT_EQ(	3,	dim_1	);
				EXPECT_EQ(	4,	dim_2	);
				EXPECT_EQ(-1,	dim_3	);
}
