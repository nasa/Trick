#include	"DataTypeTestSupport.hh"
#include	"CompositeDataType.hh"
#include	"CompositeValue.hh"
#include	"PrimitiveDataType.hh"
#include	"EnumDataType.hh"

int	ClassFive::count	=	0;

static	short	get_ClassFour__f1(void*	addr)	{	return		((ClassFour*)addr)->f1;	}
static	void	set_ClassFour__f1(void*	addr,	short	v)	{	((ClassFour*)addr)->f1	=	v;	}

static	short	get_ClassFour__f2(void*	addr)	{	return		((ClassFour*)addr)->f2;	}
static	void	set_ClassFour__f2(void*	addr,	short	v)	{	((ClassFour*)addr)->f2	=	v;	}

static	short	get_ClassFour__f3(void*	addr)	{	return		((ClassFour*)addr)->f3;	}
static	void	set_ClassFour__f3(void*	addr,	short	v)	{	((ClassFour*)addr)->f3	=	v;	}


bool	addClassOneToTypeDictionary(TypeDictionary*	typeDictionary)	{

				bool	result	=	false;
				try	{
								CompositeDataType*	classOneTypeSpec	=
												new	CompositeDataType(typeDictionary,	"ClassOne",	sizeof(ClassOne),	&construct<ClassOne>,	&destruct<ClassOne>	);
												classOneTypeSpec->addRegularMember(	"a",	offsetof(ClassOne,	a),	"int",	0,	NULL);
												classOneTypeSpec->addRegularMember(	"b",	offsetof(ClassOne,	b),	"double",	0,	NULL);

								typeDictionary->addTypeDefinition("ClassOne",	classOneTypeSpec);
								result	=	classOneTypeSpec->validate();
				}	catch(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								result	=	false;
				}
				return	result;
}

bool	addClassTwoToTypeDictionary(TypeDictionary*	typeDictionary)	{

				bool	result	=	false;
				try	{
								CompositeDataType	*	classTwoTypeSpec	=
												new	CompositeDataType(typeDictionary,	"ClassTwo",	sizeof(ClassTwo),	&construct<ClassTwo>,	&destruct<ClassTwo>);
												classTwoTypeSpec->addRegularMember(	"x",	offsetof(ClassTwo,	x),	"int",	0,	NULL);
												classTwoTypeSpec->addRegularMember(	"y",	offsetof(ClassTwo,	y),	"double",	0,	NULL);
												classTwoTypeSpec->addRegularMember(	"c1",	offsetof(ClassTwo,	c1),	"ClassOne",	0,	NULL);

								typeDictionary->addTypeDefinition("ClassTwo",	classTwoTypeSpec);

								result	=	classTwoTypeSpec->validate();
				}	catch(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								result	=	false;
				}
				return	result;
}

bool	addClassThreeToTypeDictionary(TypeDictionary*	typeDictionary)	{

				bool	result	=	false;
				try	{
								CompositeDataType	*	classThreeTypeSpec	=
												new	CompositeDataType(typeDictionary,	"ClassThree",	sizeof(ClassThree),	&construct<ClassThree>,	&destruct<ClassThree>);
												int	x_dims[]	=	{2};
												classThreeTypeSpec->addRegularMember(	"pos",	offsetof(ClassThree,	pos),	"double",	1,	x_dims);
												int	y_dims[]	=	{2};
												classThreeTypeSpec->addRegularMember(	"vel",	offsetof(ClassThree,	vel),	"double",	1,	y_dims);

								typeDictionary->addTypeDefinition("ClassThree",	classThreeTypeSpec);

								result	=	classThreeTypeSpec->validate();
				}	catch(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								result	=	false;
				}
				return	result;
}

bool	addClassFourToTypeDictionary(TypeDictionary*	typeDictionary)	{

				bool	result	=	false;
				try	{
								CompositeDataType	*	classFourTypeSpec	=
												new	CompositeDataType(typeDictionary,	"ClassFour",	sizeof(ClassFour),	&construct<ClassFour>,	&destruct<ClassFour>);
												int	x_dims[]	=	{2};
												classFourTypeSpec->addRegularMember(	"x",	offsetof(ClassFour,	x),	"double",	1,	x_dims);
												classFourTypeSpec->addBitFieldMember(	"f1",	get_ClassFour__f1,	set_ClassFour__f1);
												classFourTypeSpec->addBitFieldMember(	"f2",	get_ClassFour__f2,	set_ClassFour__f2);
												classFourTypeSpec->addBitFieldMember(	"f3",	get_ClassFour__f3,	set_ClassFour__f3);

								typeDictionary->addTypeDefinition("ClassFour",	classFourTypeSpec);

								result	=	classFourTypeSpec->validate();
				}	catch(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								result	=	false;
				}
				return	result;
}

bool	addClassFiveToTypeDictionary(TypeDictionary*	typeDictionary)	{

				bool	result	=	false;
				try	{
								CompositeDataType	*	classFiveTypeSpec	=
												new	CompositeDataType(typeDictionary,	"ClassFive",	sizeof(ClassFive),	&construct<ClassFive>,	&destruct<ClassFive>);
												classFiveTypeSpec->addRegularMember(	"x",	offsetof(ClassFive,	x),	"double",	0,	NULL);
												classFiveTypeSpec->addStaticMember(	"count",	&ClassFive::count,	"int",	0,	NULL	);

								typeDictionary->addTypeDefinition("ClassFive",	classFiveTypeSpec);

								result	=	classFiveTypeSpec->validate();
				}	catch(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								result	=	false;
				}
				return	result;
}

bool	addDayOfWeekEnumToTypeDictionary(TypeDictionary*	typeDictionary,		EnumDictionary*	enumDictionary)	{

				bool	result	=	false;
				try	{
												EnumDataType	*	dataType	=	new	EnumDataType(	enumDictionary,	"DayOfWeek",	sizeof(enum	DayOfWeek)	);
												dataType->addEnumerator(	"Sunday",			1);
												dataType->addEnumerator(	"Monday",			2);
												dataType->addEnumerator(	"Tuesday",		3);
												dataType->addEnumerator(	"Wednesday",4);
												dataType->addEnumerator(	"Thursday",	5);
												dataType->addEnumerator(	"Friday",			6);
												dataType->addEnumerator(	"Saturday",	7);

												typeDictionary->addTypeDefinition(	"DayOfWeek",	dataType	);
												result	=	dataType->validate();
				}	catch(	std::logic_error	e	)	{
								std::cerr	<<	e.what();
								result	=	false;
				}
				return	result;
}
