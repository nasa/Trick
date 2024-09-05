#include	<gtest/gtest.h>
#include	<stddef.h>
#include	<iostream>

#include	"MemMgr.hh"
#include	"AllocInfo.hh"
#include	"DataType.hh"
#include	"ClassicChkPtAgent.hh"
#include	"TypeDictionary.hh"
#include	"EnumDictionary.hh"
#include	"CompositeDataType.hh"
#include	"ArrayDataType.hh"
#include	"PointerDataType.hh"

#include	"DataTypeTestSupport.hh"

//	Framework
class	ClassicChkPtAgentTest	:	public	::testing::Test	{
				protected:
				TypeDictionary	*	typeDictionary;
				EnumDictionary	*	enumDictionary;
				ClassicChkPtAgent*	checkPointAgent;
				MemMgr	*	memMgr;

				ClassicChkPtAgentTest()	{
								typeDictionary	=	new	TypeDictionary;
								enumDictionary	=	new	EnumDictionary;
								memMgr	=	new	MemMgr();
								checkPointAgent	=	new	ClassicChkPtAgent(	std::cout	);
				}

				~ClassicChkPtAgentTest()	{
								delete	memMgr	;
								delete	typeDictionary	;
								delete	enumDictionary	;
				}
				void	SetUp()	{}
				void	TearDown()	{}
};

/*	================================================================================
																																									Test	Cases
			================================================================================
*/
TEST_F(ClassicChkPtAgentTest,	nameStackFunctions)	{

				std::string	leftSideName;

				checkPointAgent->pushName("A");
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A",	leftSideName);

				checkPointAgent->pushIndex(2);
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A[2]",	leftSideName);

				checkPointAgent->pushName("B");
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A[2].B",	leftSideName);

				checkPointAgent->pushIndex(3);
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A[2].B[3]",	leftSideName);

				checkPointAgent->popNameElement();
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A[2].B",	leftSideName);

				checkPointAgent->popNameElement();
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A[2]",	leftSideName);

				checkPointAgent->popNameElement();
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("A",	leftSideName);

				checkPointAgent->popNameElement();
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("",	leftSideName);

				std::cout	<<	"NOTE:	A	Warning	is	expected	here."	<<	std::endl;
				checkPointAgent->popNameElement();
				leftSideName	=	checkPointAgent->getLeftSideName();
				EXPECT_EQ("",	leftSideName);

}

//	================================================================================
//																															writeDeclaration	tests
//	================================================================================

TEST_F(ClassicChkPtAgentTest,	writeDeclaration1)	{

				//	A	is	an	int.
				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								allocInfo	=	new	AllocInfo("A",	"int",	0,	NULL,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("int	A;\n",	output_declaration.str());
}

TEST_F(ClassicChkPtAgentTest,	writeDeclaration2)	{

				//	A	is	a	pointer	to	int.
				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								int	dimSize[]	=	{-1};
								allocInfo	=	new	AllocInfo("A",	"int",	1,	dimSize,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("int	*A;\n",	output_declaration.str());
}

TEST_F(ClassicChkPtAgentTest,	writeDeclaration3)	{

				//	A	is	an	array	of	pointers	to	int.
				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								int	dimSize[]	=	{3,-1};
								allocInfo	=	new	AllocInfo("A",	"int",	2,	dimSize,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("int	*A[3];\n",	output_declaration.str());
}

TEST_F(ClassicChkPtAgentTest,	writeDeclaration4)	{

				//	A	is	a	pointer	to	a	3	element	array	of	int.
				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								int	dimSize[]	=	{-1,3};
								allocInfo	=	new	AllocInfo("A",	"int",	2,	dimSize,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("int	(*A)[3];\n",	output_declaration.str());
}

TEST_F(ClassicChkPtAgentTest,	writeDeclaration5)	{

				//	A	is	a	3	element	array	of	4	element	arrays	of	pointers	to	5	element	arrays	of	pointers	to	double.
				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								int	dimSize[]	=	{3,4,-1,5,-1};
								allocInfo	=	new	AllocInfo("A",	"int",	5,	dimSize,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("int	*(*A[3][4])[5];\n",	output_declaration.str());
}

TEST_F(ClassicChkPtAgentTest,	writeDeclaration_DayOfWeekEnum)	{

				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				bool	result	=	addDayOfWeekEnumToTypeDictionary(	typeDictionary,	enumDictionary	);
				EXPECT_EQ(true,	result);
				AllocInfo	*	allocInfo;
				try	{
								allocInfo	=	new	AllocInfo("A",	"DayOfWeek",	0,	NULL,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("DayOfWeek	A;\n",	output_declaration.str());
}

//	================================================================================
//																															writeAssignment	tests
//	================================================================================

TEST_F(ClassicChkPtAgentTest,	writeAssignment)	{
				std::stringstream	AssignmentStatement;
				checkPointAgent->pushName("X");
				checkPointAgent->pushName("Y");
				checkPointAgent->pushIndex(4);
				checkPointAgent->writeAssignment(	AssignmentStatement,	"3.14");
				EXPECT_EQ("X.Y[4]	=	3.14;",	AssignmentStatement.str());
}

//	================================================================================
//																																getSubName	tests
//	================================================================================

TEST_F(ClassicChkPtAgentTest,	getSubName1)	{

				bool	result	=	addClassOneToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				const	DataType*	classOneDataType	=	typeDictionary->getDataType("ClassOne");
				const	DataType*	intDataType	=	typeDictionary->getDataType("int");
				const	DataType*	doubleDataType	=	typeDictionary->getDataType("double");

				ClassOne*	classOneVariable	=	(ClassOne*)classOneDataType->createInstance(1);

				std::stringstream	nameStream1;

				checkPointAgent->getSubName(	nameStream1,	&(classOneVariable->a),	intDataType,	classOneVariable,	classOneDataType);
				EXPECT_EQ(".a",	nameStream1.str());

				std::stringstream	nameStream2;
				checkPointAgent->getSubName(	nameStream2,	&(classOneVariable->b),	doubleDataType,	classOneVariable,	classOneDataType);
				EXPECT_EQ(".b",	nameStream2.str());
}

TEST_F(ClassicChkPtAgentTest,	getSubName2)	{

				bool	result	=	false;

				result	=	addClassOneToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				result	=	addClassTwoToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				const	DataType*	classOneDataType	=	typeDictionary->getDataType("ClassOne");
				const	DataType*	classTwoDataType	=	typeDictionary->getDataType("ClassTwo");
				const	DataType*	intDataType	=	typeDictionary->getDataType("int");
				const	DataType*	doubleDataType	=	typeDictionary->getDataType("double");

				ClassTwo*	classTwoVariable	=	(ClassTwo*)classTwoDataType->createInstance(1);

				std::stringstream	nameStream1;
				checkPointAgent->getSubName(	nameStream1,	&(classTwoVariable->x),	intDataType,	classTwoVariable,	classTwoDataType);
				EXPECT_EQ(".x",	nameStream1.str());

				std::stringstream	nameStream2;
				checkPointAgent->getSubName(	nameStream2,	&(classTwoVariable->y),	doubleDataType,	classTwoVariable,	classTwoDataType);
				EXPECT_EQ(".y",	nameStream2.str());

				std::stringstream	nameStream3;
				checkPointAgent->getSubName(	nameStream3,	&(classTwoVariable->c1),	classOneDataType,	classTwoVariable,	classTwoDataType);
				EXPECT_EQ(".c1",	nameStream3.str());

				std::stringstream	nameStream4;
				checkPointAgent->getSubName(	nameStream4,	&(classTwoVariable->c1.a),	intDataType,	classTwoVariable,	classTwoDataType);
				EXPECT_EQ(".c1.a",	nameStream4.str());

				std::stringstream	nameStream5;
				checkPointAgent->getSubName(	nameStream5,	&(classTwoVariable->c1.b),	doubleDataType,	classTwoVariable,	classTwoDataType);
				EXPECT_EQ(".c1.b",	nameStream5.str());
}

TEST_F(ClassicChkPtAgentTest,	getSubName3)	{

				bool	result	=	addClassThreeToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				const	DataType*	classThreeDataType	=	typeDictionary->getDataType("ClassThree");
				const	DataType*	doubleDataType	=	typeDictionary->getDataType("double");

				ClassThree*	classThreeVariable	=	(ClassThree*)classThreeDataType->createInstance(1);

				std::stringstream	nameStream1;
				checkPointAgent->getSubName(	nameStream1,	&(classThreeVariable->pos[0]),	doubleDataType,	classThreeVariable,	classThreeDataType);
				EXPECT_EQ(".pos[0]",	nameStream1.str());

				std::stringstream	nameStream2;
				checkPointAgent->getSubName(	nameStream2,	&(classThreeVariable->pos[1]),	doubleDataType,	classThreeVariable,	classThreeDataType);
				EXPECT_EQ(".pos[1]",	nameStream2.str());

				std::stringstream	nameStream3;
				checkPointAgent->getSubName(	nameStream3,	&(classThreeVariable->vel[0]),	doubleDataType,	classThreeVariable,	classThreeDataType);
				EXPECT_EQ(".vel[0]",	nameStream3.str());

				std::stringstream	nameStream4;
				checkPointAgent->getSubName(	nameStream4,	&(classThreeVariable->vel[1]),	doubleDataType,	classThreeVariable,	classThreeDataType);
				EXPECT_EQ(".vel[1]",	nameStream4.str());
}

TEST_F(ClassicChkPtAgentTest,	writXXXXXXXation1)	{

				//	A	is	an	int.
				std::stringstream	output_declaration;
				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								allocInfo	=	new	AllocInfo("A",	"int",	0,	NULL,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);
				checkPointAgent->writeDeclaration(	output_declaration,	allocInfo->getName(),	allocInfo->getDataType());
				delete	allocInfo;
				EXPECT_EQ("int	A;\n",	output_declaration.str());
}

TEST_F(ClassicChkPtAgentTest,	writeVariable1)	{

				bool	result	=	addClassOneToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				bool	construction_result	=	true;
				AllocInfo	*	allocInfo;
				try	{
								allocInfo	=	new	AllocInfo("A",	"ClassOne",	0,	NULL,	typeDictionary);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								construction_result	=	false;
				}
				ASSERT_EQ(true,	construction_result);

				ClassOne*	classOneVariable	=	(ClassOne*)allocInfo->getStart();
				const	DataType*	classOneDataType	=	allocInfo->getDataType();

				classOneVariable->a	=	4;
				classOneVariable->b	=	7.1;

				checkPointAgent->writeDeclaration(	std::cout,	allocInfo->getName(),	allocInfo->getDataType());
				checkPointAgent->writeVariable(	std::cout,	memMgr,	classOneVariable,	classOneDataType);
}

TEST_F(ClassicChkPtAgentTest,	writeVariable2)	{

				bool	result;
				result	=	addClassOneToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				result	=	addClassTwoToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				const	DataType*	classTwoDataType	=	typeDictionary->getDataType("ClassTwo");

				ClassTwo*	classTwoVariable	=	(ClassTwo*)classTwoDataType->createInstance(1);
				classTwoVariable->x	=	2;
				classTwoVariable->y	=	8.7;
				classTwoVariable->c1.a	=	9;
				classTwoVariable->c1.b	=	18.4;

				checkPointAgent->writeVariable(	std::cout,	memMgr,	classTwoVariable,	classTwoDataType);
}

TEST_F(ClassicChkPtAgentTest,	writeVariable3)	{

				bool	result;
				result	=	addClassThreeToTypeDictionary(	typeDictionary	);
				EXPECT_EQ(true,	result);

				const	DataType*	classThreeDataType	=	typeDictionary->getDataType("ClassThree");

				ClassThree*	classThreeVariable	=	(ClassThree*)classThreeDataType->createInstance(1);
				classThreeVariable->pos[0]	=	2.3;
				classThreeVariable->pos[1]	=	3.4;
				classThreeVariable->vel[0]	=	4.5;
				classThreeVariable->vel[1]	=	5.6;

				checkPointAgent->setMakeCompactArrays(true);
				checkPointAgent->writeVariable(	std::cout,	memMgr,	classThreeVariable,	classThreeDataType);
}
