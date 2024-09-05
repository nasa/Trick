#include	<gtest/gtest.h>
#include	<stddef.h>
#include	<iostream>
#include	"TypeDictionary.hh"
#include	"PrimitiveDataType.hh"

class	PrimitiveDataTypeTest	:	public	::testing::Test	{
				protected:
				TypeDictionary	*typeDictionary;
				PrimitiveDataTypeTest()	{	typeDictionary	=	new	TypeDictionary;	}
				~PrimitiveDataTypeTest()	{	delete	typeDictionary;	}
				void	SetUp()	{}
				void	TearDown()	{}
};

template	<typename	T>
void	validate_getSize(size_t	expectedSize)	{
				PrimitiveDataType<T>	*	primTypeSpec	=	new	PrimitiveDataType<T>();
				size_t	test_result	=	primTypeSpec->getSize();
				delete	primTypeSpec;
				EXPECT_EQ(expectedSize,	test_result);
}

template	<typename	T>
void	validate_printValue(T&	testVar,	const	char*	expectedString)	{
				std::stringstream	ss;
				PrimitiveDataType<T>	*	primTypeSpec	=	new	PrimitiveDataType<T>();
				primTypeSpec->printValue(ss,	&testVar);
				int	result	=	ss.str().compare(expectedString);
				delete	primTypeSpec;
				EXPECT_EQ(0,	result);
}

template	<typename	T>
void	validate_toString(const	char*	expectedString)	{
				std::string	s;
				PrimitiveDataType<T>	*	primTypeSpec	=	new	PrimitiveDataType<T>();
				s	=	primTypeSpec->toString();
				int	result	=	s.compare(expectedString);
				delete	primTypeSpec;
				EXPECT_EQ(0,	result);
}

template	<typename	T>
void	validate_isFloatingPoint(bool	expectedResult)	{
				PrimitiveDataType<T>	*	primTypeSpec	=	new	PrimitiveDataType<T>();
				bool	result	=	primTypeSpec->isFloatingPoint();
				delete	primTypeSpec;
				EXPECT_EQ(expectedResult,	result);
}

template	<typename	T>
void	validate_isSigned(bool	expectedResult)	{
				PrimitiveDataType<T>	*	primTypeSpec	=	new	PrimitiveDataType<T>();
				bool	result	=	primTypeSpec->isSigned();
				delete	primTypeSpec;
				EXPECT_EQ(expectedResult,	result);
}

template	<typename	T>
void	validate_isVoid(bool	expectedResult)	{
				PrimitiveDataType<T>	*	primTypeSpec	=	new	PrimitiveDataType<T>();
				bool	result	=	primTypeSpec->isVoid();
				delete	primTypeSpec;
				EXPECT_EQ(expectedResult,	result);
}

/*	================================================================================
																																									Test	Cases
			================================================================================
*/
TEST_F(PrimitiveDataTypeTest,	validate	)	{

				/*	PrimitiveDataType	is	always	valid,	because	it	is	a	fundemental,	builtin	type.	*/
				PrimitiveDataType<char>	*	primTypeSpec	=	new	PrimitiveDataType<char>();

				bool	test_result;
				test_result	=	primTypeSpec->validate();

				EXPECT_EQ(true,	test_result);
}

TEST_F(PrimitiveDataTypeTest,	getSize_void)	{
				validate_getSize<void>(0);
}

TEST_F(PrimitiveDataTypeTest,	getSize_char)	{
				validate_getSize<char>((int)sizeof(char));
}

TEST_F(PrimitiveDataTypeTest,	getSize_short)	{
				validate_getSize<short>((int)sizeof(short));
}

TEST_F(PrimitiveDataTypeTest,	getSize_int)	{
				validate_getSize<int>((int)sizeof(int));
}

TEST_F(PrimitiveDataTypeTest,	getSize_long)	{
				validate_getSize<long>((int)sizeof(long));
}

TEST_F(PrimitiveDataTypeTest,	getSize_long_long)	{
				validate_getSize<long	long>((int)sizeof(long	long));
}

TEST_F(PrimitiveDataTypeTest,	getSize_unsigned_char)	{
				validate_getSize<unsigned	char>((int)sizeof(unsigned	char));
}

TEST_F(PrimitiveDataTypeTest,	getSize_unsigned_short)	{
				validate_getSize<unsigned	short>((int)sizeof(unsigned	short));
}

TEST_F(PrimitiveDataTypeTest,	getSize_unsigned_int)	{
				validate_getSize<unsigned	int>((int)sizeof(unsigned	int));
}

TEST_F(PrimitiveDataTypeTest,	getSize_unsigned_long)	{
				validate_getSize<unsigned	long>((int)sizeof(unsigned	long));
}

TEST_F(PrimitiveDataTypeTest,	getSize_unsigned_long_long)	{
				validate_getSize<unsigned	long	long>((int)sizeof(unsigned	long	long));
}

TEST_F(PrimitiveDataTypeTest,	getSize_wchar_t)	{
				validate_getSize<wchar_t>((int)sizeof(wchar_t));
}

TEST_F(PrimitiveDataTypeTest,	getSize_float)	{
				validate_getSize<float>((int)sizeof(float));
}

TEST_F(PrimitiveDataTypeTest,	getSize_double)	{
				validate_getSize<double>((int)sizeof(double));
}

//	-----------------------------------------------------------------------------------------
//																																				assignValue	tests
//	-----------------------------------------------------------------------------------------

TEST_F(PrimitiveDataTypeTest,	assignValue_char)	{
				char	test_var	=	'\0';
				PrimitiveDataType<char>	*	primTypeSpec	=	new	PrimitiveDataType<char>();
				IntegerValue	*	value	=	new	IntegerValue((int)'A');
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_EQ('A',	test_var);
}

TEST_F(PrimitiveDataTypeTest,	assignValue_short)	{
				short	test_var	=	0;
				PrimitiveDataType<short>	*	primTypeSpec	=	new	PrimitiveDataType<short>();
				IntegerValue	*	value	=	new	IntegerValue(314);
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_EQ(314,	test_var);
}

TEST_F(PrimitiveDataTypeTest,	assignValue_int)	{
				int	test_var	=	0;
				PrimitiveDataType<int>	*	primTypeSpec	=	new	PrimitiveDataType<int>();
				IntegerValue	*	value	=	new	IntegerValue(723);
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_EQ(723,	test_var);
}

TEST_F(PrimitiveDataTypeTest,	assignValue_long)	{
				long	test_var	=	0;
				PrimitiveDataType<long>	*	primTypeSpec	=	new	PrimitiveDataType<long>();
				IntegerValue	*	value	=	new	IntegerValue(723);
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_EQ(723,	test_var);
}

TEST_F(PrimitiveDataTypeTest,	assignValue_long_long)	{
				long	long	test_var	=	0;
				PrimitiveDataType<long	long>	*	primTypeSpec	=	new	PrimitiveDataType<long	long>();
				IntegerValue	*	value	=	new	IntegerValue(723);
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_EQ(723,	test_var);
}

TEST_F(PrimitiveDataTypeTest,	assignValue_float)	{
				float	test_var	=	0.0;
				PrimitiveDataType<float>	*	primTypeSpec	=	new	PrimitiveDataType<float>();
				FloatingPointValue	*	value	=	new	FloatingPointValue(2.7182818);
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_NEAR(2.7182818,	test_var,	0.000001);
}

TEST_F(PrimitiveDataTypeTest,	assignValue_double)	{
				double	test_var	=	0.0;
				PrimitiveDataType<double>	*	primTypeSpec	=	new	PrimitiveDataType<double>();
				FloatingPointValue	*	value	=	new	FloatingPointValue(3.1415927);
				primTypeSpec->assignValue(&test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_NEAR(3.1415927,	test_var,	0.000001);
}

//	-----------------------------------------------------------------------------------------
//																																				printValue	tests
//	-----------------------------------------------------------------------------------------
TEST_F(PrimitiveDataTypeTest,	printValue_char	)	{
				char	testVar	=	'B';
				validate_printValue(testVar,	"'B'");
}

TEST_F(PrimitiveDataTypeTest,	printValue_char_2	)	{
				/*	Unprintable	chars.	*/
				char	testVar	=	'\01';
				validate_printValue(testVar,	"1");
}

TEST_F(PrimitiveDataTypeTest,	printValue_short	)	{
				short	testVar	=	-1234;
				validate_printValue(testVar,	"-1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_int	)	{
				int	testVar	=	-1234;
				validate_printValue(testVar,	"-1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_long	)	{
				long	testVar	=	-1234;
				validate_printValue(testVar,	"-1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_long_long	)	{
				long	long	testVar	=	-1234;
				validate_printValue(testVar,	"-1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_unsigned_char	)	{
				unsigned	char	testVar	=	'B';
				validate_printValue(testVar,	"'B'");
}

TEST_F(PrimitiveDataTypeTest,	printValue_unsigned_short	)	{
				unsigned	short	testVar	=	1234;
				validate_printValue(testVar,	"1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_unsigned_int	)	{
				unsigned	int	testVar	=	1234;
				validate_printValue(testVar,	"1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_unsigned_long	)	{
				unsigned	long	testVar	=	1234;
				validate_printValue(testVar,	"1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_unsigned_long_long	)	{
				unsigned	long	long	testVar	=	1234;
				validate_printValue(testVar,	"1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_wchar_t	)	{
				wchar_t	testVar	=	1234;
				validate_printValue(testVar,	"1234");
}

TEST_F(PrimitiveDataTypeTest,	printValue_float	)	{
				float	testVar	=	3.14159;
				validate_printValue(testVar,	"3.14159");
}

TEST_F(PrimitiveDataTypeTest,	printValue_double	)	{
				double	testVar	=	3.14159;
				validate_printValue(testVar,	"3.14159");
}

//	-----------------------------------------------------------------------------------------
//																																				toString	tests
//	-----------------------------------------------------------------------------------------
TEST_F(PrimitiveDataTypeTest,	toString_void	)	{
				validate_toString<void>("void");
}

TEST_F(PrimitiveDataTypeTest,	toString_char	)	{
				validate_toString<char>("char");
}

TEST_F(PrimitiveDataTypeTest,	toString_short	)	{
				validate_toString<short>("short");
}

TEST_F(PrimitiveDataTypeTest,	toString_int	)	{
				validate_toString<int>("int");
}

TEST_F(PrimitiveDataTypeTest,	toString_long	)	{
				validate_toString<long>("long");
}

TEST_F(PrimitiveDataTypeTest,	toString_long_long	)	{
				validate_toString<long	long>("long	long");
}

TEST_F(PrimitiveDataTypeTest,	toString_unsigned_char	)	{
				validate_toString<unsigned	char>("unsigned	char");
}

TEST_F(PrimitiveDataTypeTest,	toString_unsigned_short	)	{
				validate_toString<unsigned	short>("unsigned	short");
}

TEST_F(PrimitiveDataTypeTest,	toString_unsigned_int	)	{
				validate_toString<unsigned	int>("unsigned	int");
}

TEST_F(PrimitiveDataTypeTest,	toString_unsigned_long	)	{
				validate_toString<unsigned	long>("unsigned	long");
}

TEST_F(PrimitiveDataTypeTest,	toString_unsigned_long_long	)	{
				validate_toString<unsigned	long	long>("unsigned	long	long");
}

TEST_F(PrimitiveDataTypeTest,	toString_wchar_t	)	{
				validate_toString<wchar_t>("wchar_t");
}

TEST_F(PrimitiveDataTypeTest,	toString_float	)	{
				validate_toString<float>("float");
}

TEST_F(PrimitiveDataTypeTest,	toString_double	)	{
				validate_toString<double>("double");
}

//	-----------------------------------------------------------------------------------------
//																																				isFloatingPoint	tests
//	-----------------------------------------------------------------------------------------
TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_void	)	{
				validate_isFloatingPoint<void>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_char	)	{
				validate_isFloatingPoint<char>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_short	)	{
				validate_isFloatingPoint<short>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_int	)	{
				validate_isFloatingPoint<int>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_long	)	{
				validate_isFloatingPoint<long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_long_long	)	{
				validate_isFloatingPoint<long	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_unsigned_char	)	{
				validate_isFloatingPoint<unsigned	char>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_unsigned_short	)	{
				validate_isFloatingPoint<unsigned	short>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_unsigned_int	)	{
				validate_isFloatingPoint<unsigned	int>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_unsigned_long	)	{
				validate_isFloatingPoint<unsigned	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_unsigned_long_long	)	{
				validate_isFloatingPoint<unsigned	long	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_float	)	{
				validate_isFloatingPoint<float>(true);
}

TEST_F(PrimitiveDataTypeTest,	isFloatingPoint_double	)	{
				validate_isFloatingPoint<double>(true);
}

//	-----------------------------------------------------------------------------------------
//																																		isSigned	tests
//	-----------------------------------------------------------------------------------------
TEST_F(PrimitiveDataTypeTest,	isSigned_void	)	{
				validate_isSigned<void>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_char	)	{
				validate_isSigned<char>(true);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_short	)	{
				validate_isSigned<short>(true);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_int	)	{
				validate_isSigned<int>(true);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_long	)	{
				validate_isSigned<long>(true);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_long_long	)	{
				validate_isSigned<long	long>(true);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_unsigned_char	)	{
				validate_isSigned<unsigned	char>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_unsigned_short	)	{
				validate_isSigned<unsigned	short>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_unsigned_int	)	{
				validate_isSigned<unsigned	int>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_unsigned_long	)	{
				validate_isSigned<unsigned	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_unsigned_long_long	)	{
				validate_isSigned<unsigned	long	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_wchar_t	)	{
				validate_isSigned<wchar_t>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_float	)	{
				validate_isSigned<float>(false);
}

TEST_F(PrimitiveDataTypeTest,	isSigned_double	)	{
				validate_isSigned<double>(false);
}

//	-----------------------------------------------------------------------------------------
//																																		isVoid	tests
//	-----------------------------------------------------------------------------------------
TEST_F(PrimitiveDataTypeTest,	isVoid_void	)	{
				validate_isVoid<void>(true);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_char	)	{
				validate_isVoid<char>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_short	)	{
				validate_isVoid<short>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_int	)	{
				validate_isVoid<int>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_long	)	{
				validate_isVoid<long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_long_long	)	{
				validate_isVoid<long	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_unsigned_char	)	{
				validate_isVoid<unsigned	char>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_unsigned_short	)	{
				validate_isVoid<unsigned	short>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_unsigned_int	)	{
				validate_isVoid<unsigned	int>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_unsigned_long	)	{
				validate_isVoid<unsigned	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_unsigned_long_long	)	{
				validate_isVoid<unsigned	long	long>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_wchar_t	)	{
				validate_isVoid<wchar_t>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_float	)	{
				validate_isVoid<float>(false);
}

TEST_F(PrimitiveDataTypeTest,	isVoid_double	)	{
				validate_isVoid<double>(false);
}

TEST_F(PrimitiveDataTypeTest,	createInstance_char)	{

				PrimitiveDataType<char>	*	primTypeSpec	=	new	PrimitiveDataType<char>();
				char*	test_var	=	(char*)primTypeSpec->createInstance(1);
				IntegerValue	*	value	=	new	IntegerValue((int)'A');
				primTypeSpec->assignValue(test_var,	value);
				delete	value;
				delete	primTypeSpec;
				EXPECT_EQ('A',	*test_var);
}

