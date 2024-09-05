#include	<gtest/gtest.h>
#include	<stddef.h>
#include	<iostream>
#include	"TypeDictionary.hh"
#include	"PointerDataType.hh"
#include	"PointerValue.hh"

/*	================================================================================
																																									Test	Cases
			================================================================================
*/
TEST(	PointerDataType	,	constructor_exception	)	{

				//	Attempt	to	create	type	a	PointerDataType	with	0	pointers.
				//	This	attempt	should	throw	an	exception.

				std::cout	<<	"=====	Expecting	exception	and	corresponding	message.	====="	<<	std::endl;

				PointerDataType	*	ptrTypeSpec;
				bool	constructor_result	=	true;
				try	{
								ptrTypeSpec	=	new	PointerDataType("double",	0);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								constructor_result	=	false;
				}

				//	Verify	that	an	exception	was	raised.
				EXPECT_EQ(false,	constructor_result);

				//	Clean	up.
				if	(constructor_result)	{
								delete	ptrTypeSpec;
				}
}

TEST(	PointerDataType,	copy_constructor	)	{

				//	Create	a	PointerDataType.
				PointerDataType*	orig	=	new	PointerDataType("float",	3);

				//	Duplicate	it.
				PointerDataType*	copy	=	new	PointerDataType(	*(const	PointerDataType*)orig	);

				//	Verify	that	the	duplicate	PointerDataType	is	what	we	expect.
				std::stringstream	ss;
				ss	<<	copy->toString();
				int	result	=	ss.str().compare("float***");
				EXPECT_EQ(0,	result);

				//	Clean	up.
				delete	copy;
				delete	orig;
}

TEST(	PointerDataType	,	operator_equal	)	{

				//	Create	a	PointerDataType.
				PointerDataType*	orig	=	new	PointerDataType("long",	2);

				//	Assign	it	to	another	PointerDataType.
				PointerDataType	copy	=	*(const	PointerDataType*)orig;

				//	Verify	that	the	assigned	PointerDataType	is	what	we	expect.
				std::stringstream	ss;
				ss	<<	copy.toString();
				int	result	=	ss.str().compare("long**");
				EXPECT_EQ(0,	result);

				//	Clean	up.
				delete	orig;
}

TEST(	PointerDataType	,	clone	)	{

				//	Create	a	PointerDataType.
				PointerDataType*	orig	=	new	PointerDataType("int",	2);

				//	Clone	it.
				DataType	*	copy	=	orig->clone();

				//	Verify	that	the	cloned	PointerDataType	is	what	we	expect.
				std::stringstream	ss;
				ss	<<	copy->toString();
				int	result	=	ss.str().compare("int**");
				EXPECT_EQ(0,	result);

				//	Clean	up.
				delete	orig;
				delete	copy;
}

TEST(	PointerDataType	,	getSize	)	{

				//	Create	a	PointerDataType.
				PointerDataType*	ptrTypeSpec	=	new	PointerDataType("long",	2);

				//	Verify	that	getSize	returns	the	size	of	a	pointer.
				EXPECT_EQ(	sizeof(void*),	ptrTypeSpec->getSize());
}

TEST(	PointerDataType	,	assignValue	)	{

				double	d	=	1.2345;
				double	*	d_ptr;

				//	Create	a	PointerDataType.
				PointerDataType*	ptrTypeSpec	=	new	PointerDataType("double",	1);

				PointerValue	*	ptrValue	=	new	PointerValue(&d);
				ptrTypeSpec->assignValue(&d_ptr,	ptrValue);

				EXPECT_EQ(1.2345,	*d_ptr);
}

TEST(	PointerDataType	,	printValue	)	{

				double	*	d_ptr	=	(double*)0x12345678;

				//	Create	a	PointerDataType.
				PointerDataType*	ptrTypeSpec	=	new	PointerDataType("double",	1);

				//	Verify	that	printValue	prints	the	value	that	we	expect.
				std::stringstream	ss;
				ptrTypeSpec->printValue(ss,	&d_ptr);
				int	result	=	ss.str().compare("0x12345678");

				EXPECT_EQ(0,	result);
}

TEST(	PointerDataType	,	validate_1	)	{

				//	Create	a	PointerDataType.
				PointerDataType*	ptrTypeSpec	=	new	PointerDataType("double",	1);

				//	Validate	the	Type.
				bool	validation_result	=	ptrTypeSpec->validate();

				//	Verify	that	validation	succeeded.
				ASSERT_EQ(true,	validation_result);

}

TEST(	PointerDataType	,	validate_2	)	{

				std::cout	<<	"=====	Expecting	an	error	message	about	an	undefined	type.	====="	<<	std::endl;

				//	Create	a	PointerDataType.
				PointerDataType*	ptrTypeSpec	=	new	PointerDataType("Undefined_Type",	1);

				//	Validate	the	Type.
				bool	validation_result	=	ptrTypeSpec->validate();

				//	Verify	that	validation	failed.
				ASSERT_EQ(false,	validation_result);

}

TEST(	PointerDataType	,	getDereferencedType_1	)	{

				//	Create	type:	double**
				PointerDataType	*	ptrTypeSpec;
				bool	constructor_result	=	true;
				try	{
								ptrTypeSpec	=	new	PointerDataType("double",	2);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								constructor_result	=	false;
				}
				ASSERT_EQ(true,	constructor_result);

				//	Validate	the	Type.
				bool	validation_result	=	ptrTypeSpec->validate();
				ASSERT_EQ(true,	validation_result);

				//	Dereference	the	type.
				const	DataType	*	dereferencedPtrTypeSpec;
				bool		dereference_result	=	true;
				try	{
								dereferencedPtrTypeSpec	=	ptrTypeSpec->getDereferencedType();
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								dereference_result	=	false;
				}
				ASSERT_EQ(true,	dereference_result);

				//	Result	of	the	dereference	should	be:	double*
				std::stringstream	ss;
				ss	<<	dereferencedPtrTypeSpec->toString();
				int	comparision_result	=	ss.str().compare("double*");

				EXPECT_EQ(0,	comparision_result);

}

TEST(	PointerDataType	,	getDereferencedType_2	)	{

				//	Create	type:	double*
				PointerDataType	*	ptrTypeSpec;
				bool	constructor_result	=	true;
				try	{
								ptrTypeSpec	=	new	PointerDataType("double",	1);
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								constructor_result	=	false;
				}
				ASSERT_EQ(true,	constructor_result);

				//	Validate	the	Type.
				bool	validation_result	=	ptrTypeSpec->validate();
				ASSERT_EQ(true,	validation_result);

				//	Dereference	the	type.
				const	DataType	*	dereferencedPtrTypeSpec;
				bool		dereference_result	=	true;
				try	{
								dereferencedPtrTypeSpec	=	ptrTypeSpec->getDereferencedType();
				}	catch	(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								dereference_result	=	false;
				}
				ASSERT_EQ(true,	dereference_result);

				//	Result	of	the	dereference	should	be:	double
				std::stringstream	ss;
				ss	<<	dereferencedPtrTypeSpec->toString();
				int	comparision_result	=	ss.str().compare("double");

				EXPECT_EQ(0,	comparision_result);
}
