
#include	<gtest/gtest.h>
#include	<stddef.h>
#include	"EnumDeclaration.hh"
#include	"IntegerValue.hh"
#include	"TypeDictionary.hh"

enum	DayOfWeek	{
			Sunday				=	1,
			Monday				=	2,
			Tuesday			=	3,
			Wednesday	=	4,
			Thursday		=	5,
			Friday				=	6,
			Saturday		=	7
};

TEST(EnumDeclaration,	getters_1	)	{

				//	Create	a	EnumDeclaration.
				EnumDeclaration	*	enum_decl	=	new	EnumDeclaration(0,	0,	NULL,	sizeof(enum	DayOfWeek));
								enum_decl->addEnumerator(	"Sunday",			1);
								enum_decl->addEnumerator(	"Monday",			2);
								enum_decl->addEnumerator(	"Tuesday",		3);
								enum_decl->addEnumerator(	"Wednesday",4);
								enum_decl->addEnumerator(	"Thursday",	5);
								enum_decl->addEnumerator(	"Friday",			6);
								enum_decl->addEnumerator(	"Saturday",	7);

				EXPECT_EQ(1,	enum_decl->getElementCount());
				EXPECT_EQ(0,	enum_decl->getPointerCount());
				EXPECT_EQ(0,	enum_decl->getDimensionCount());
				EXPECT_EQ(	(int)sizeof(enum	DayOfWeek),	enum_decl->getElementSize());
				EXPECT_EQ(	(int)sizeof(enum	DayOfWeek),	enum_decl->getSize());
}

TEST(EnumDeclaration,	toString_1)	{

				//	Create	a	EnumDeclaration.
				EnumDeclaration	*	enum_decl	=	new	EnumDeclaration(0,	0,	NULL,	sizeof(enum	DayOfWeek));
								enum_decl->addEnumerator(	"Sunday",			1);
								enum_decl->addEnumerator(	"Monday",			2);
								enum_decl->addEnumerator(	"Tuesday",		3);
								enum_decl->addEnumerator(	"Wednesday",4);
								enum_decl->addEnumerator(	"Thursday",	5);
								enum_decl->addEnumerator(	"Friday",			6);
								enum_decl->addEnumerator(	"Saturday",	7);

				//	Print	the	EnumDeclaration.
				std::stringstream	ss;
				ss	<<	enum_decl->toString();

				//	Check	the	results.
				int	result	=	ss.str().compare("enum	{	Sunday	=	1,	Monday	=	2,	Tuesday	=	3,	Wednesday	=	4,	Thursday	=	5,	Friday	=	6,	Saturday	=	7}");
				EXPECT_EQ(	0,	result);
}

TEST(EnumDeclaration,	copy_constructor_1)	{

				//	Create	a	EnumDeclaration.
				EnumDeclaration	*	enum_decl	=	new	EnumDeclaration(0,	0,	NULL,	sizeof(enum	DayOfWeek));
								enum_decl->addEnumerator(	"Sunday",			1);
								enum_decl->addEnumerator(	"Monday",			2);
								enum_decl->addEnumerator(	"Tuesday",		3);
								enum_decl->addEnumerator(	"Wednesday",4);
								enum_decl->addEnumerator(	"Thursday",	5);
								enum_decl->addEnumerator(	"Friday",			6);
								enum_decl->addEnumerator(	"Saturday",	7);

				//	Create	a	duplicate	of	the	EnumDeclaration.
				EnumDeclaration	*
								duplicate_decl	=	new	EnumDeclaration(	*enum_decl	);

				//	Delete	the	original.
				delete	enum_decl;

				//	Print	the	duplicate	EnumDeclaration.
				std::stringstream	ss;
				ss	<<	duplicate_decl->toString();

				//	Check	the	results	with	what	we	expected.
				int	result	=	ss.str().compare("enum	{	Sunday	=	1,	Monday	=	2,	Tuesday	=	3,	Wednesday	=	4,	Thursday	=	5,	Friday	=	6,	Saturday	=	7}");
				EXPECT_EQ(0,	result);
}

#if	0
TEST(EnumDeclaration,	assignment_operator)	{

				EnumDeclaration	my_decl;

				//	Create	a	EnumDeclaration.
				unsigned	int	decl_dims[1]	=	{5};
				EnumDeclaration	*	original_decl	=	new	EnumDeclaration(1,	1,	decl_dims,	sizeof(ClassThree));
								unsigned	int	x_dims[1]	=	{3};
								original_decl->addRegularMember(	"x",	offsetof(ClassThree,	x),	new	PrimitiveDeclaration<double>(0,	1,	x_dims));
								original_decl->addBitFieldMember<short>(	"f1",	3);
								original_decl->addBitFieldMember<short>(	"f2",	5);
								original_decl->addBitFieldMember<short>(	"f3",	7);

				my_decl	=	*original_decl;

				//	Delete	the	original.
				delete	original_decl;

				//	Print	the	duplicate	EnumDeclaration.
				std::stringstream	ss;
				ss	<<	my_decl.toString();

				//	Check	the	results	with	what	we	expected.
				int	result	=	ss.str().compare("{\ndouble[3]	x;\nshort	f1	:	3;\nshort	f2	:	5;\nshort	f3	:	7;\n}*[5]");
				EXPECT_EQ(0,	result);
}
#endif

TEST(EnumDeclaration,	assign_singleton)	{

				//	Create	an	instance	of	type.
				enum	DayOfWeek	day	=	Monday;

				//	Create	our	own	description	of	the	type.
				EnumDeclaration	*	enum_decl	=	new	EnumDeclaration(0,	0,	NULL,	sizeof(enum	DayOfWeek));
								enum_decl->addEnumerator(	"Sunday",			1);
								enum_decl->addEnumerator(	"Monday",			2);
								enum_decl->addEnumerator(	"Tuesday",		3);
								enum_decl->addEnumerator(	"Wednesday",4);
								enum_decl->addEnumerator(	"Thursday",	5);
								enum_decl->addEnumerator(	"Friday",			6);
								enum_decl->addEnumerator(	"Saturday",	7);

				//	Create	a	CompositeValue	to	be	assigned	to	the	class	instance.
				IntegerValue	*intVal	=	new	IntegerValue(4);

				//	Perform	the	assignment.
				enum_decl->assignValue(	&day,	intVal);

				//	Check	that	values	were	correctly	assigned	to	the	class	instance.
				EXPECT_EQ(day	,	4);
}

TEST(EnumDeclaration,	print_singleton_value)	{

				//	Create	an	instance	of	type.
				enum	DayOfWeek	day	=	Monday;

				//	Create	our	own	description	of	the	type.
				EnumDeclaration	*	enum_decl	=	new	EnumDeclaration(0,	0,	NULL,	sizeof(enum	DayOfWeek));
								enum_decl->addEnumerator(	"Sunday",			1);
								enum_decl->addEnumerator(	"Monday",			2);
								enum_decl->addEnumerator(	"Tuesday",		3);
								enum_decl->addEnumerator(	"Wednesday",4);
								enum_decl->addEnumerator(	"Thursday",	5);
								enum_decl->addEnumerator(	"Friday",			6);
								enum_decl->addEnumerator(	"Saturday",	7);

				std::stringstream	ss;
				enum_decl->printValue(ss,	&day);

				int	result	=	ss.str().compare("Monday");

				EXPECT_EQ(0,	result);


				class	Foo	{	public	:	Foo	()	{
								TypeDictionary	*	type_dict	=	TypeDictionary::getInstance();

								EnumDeclaration	*	enum_decl	=	new	EnumDeclaration(0,	0,	NULL,	sizeof(enum	DayOfWeek));
												enum_decl->addEnumerator(	"Sunday",			1);
												enum_decl->addEnumerator(	"Monday",			2);
												enum_decl->addEnumerator(	"Tuesday",		3);
												enum_decl->addEnumerator(	"Wednesday",4);
												enum_decl->addEnumerator(	"Thursday",	5);
												enum_decl->addEnumerator(	"Friday",			6);
												enum_decl->addEnumerator(	"Saturday",	7);

								type_dict->addTypeDeclaration("Foo",	enum_decl);

								std::stringstream	ss;
								ss	<<	enum_decl->toString();
								ss	<<		"============================================"	<<	std::endl;
								ss	<<	type_dict->toString();
								std::cout	<<	ss.str()	<<	std::endl;


				}	};	Foo	__Foo;

}
