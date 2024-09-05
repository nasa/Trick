#include	"ArrayDataType.hh"
#include	"PointerDataType.hh"
#include	"TypeDictionary.hh"
#include	"PointerValue.hh"
#include	"CompositeValue.hh"
#include	<iostream>
#include	<sstream>

bool	ArrayDataType::initArrayDataType(	TypeDictionary*	typeDictionary,	std::string	typeSpecName,	unsigned	int	n_dims,	int	dims[]	)	{

				bool	errorCondition	=	false;
				//	NOTE:	(dim[0]	>=	0)	indicates	an	array.	(dim[0]	<	0)	indicates	a	pointer.
				if	((	n_dims	>=	1	)	&&	(	dims[0]	>=	0	))	{
								if	(	n_dims	==	1	)	{
												this->typeSpecName	=	typeSpecName;
												ownDataType	=	NULL;
												this->subType	=	NULL;
								}	else	if	(n_dims	>	1)	{
												this->typeSpecName.clear();
												if	(dims[1]	<	0)	{
																this->ownDataType	=	new	PointerDataType(	typeDictionary,	typeSpecName,	n_dims-1,	&dims[1]	);
												}	else	{
																this->ownDataType	=	new	ArrayDataType(	typeDictionary,	typeSpecName,	n_dims-1,	&dims[1]	);
												}
												this->subType	=	ownDataType;
								}
								elementCount	=	dims[0];
								this->typeDictionary	=	typeDictionary;

								is_valid	=	false;
								typeSize	=	0;	//	This	is	set	when	this	ArrayDataType	is	validated.
				}	else	{
								//	n_dims	and	dims	don't	specify	an	array.
								errorCondition=	true;
				}

				return	errorCondition;
}

//	CONSTRUCTOR
ArrayDataType::	ArrayDataType(	TypeDictionary*	typeDictionary,	std::string	typeSpecName,	unsigned	int	n_dims,	int	dims[]	)		{

				if(	this->initArrayDataType(	typeDictionary,	typeSpecName,	n_dims,	dims)	)	{
								throw	std::logic_error("ArrayDataType	constructor(1)	error.");
				}
}

//	CONSTRUCTOR
ArrayDataType::	ArrayDataType(	TypeDictionary*	typeDictionary,	std::string	typeSpecName,	unsigned	int	dimensionSize	)		{

				int	n	=	dimensionSize;
				if(	this->initArrayDataType(	typeDictionary,	typeSpecName,	1,	&n)	)	{
								throw	std::logic_error("ArrayDataType	constructor(2)	error.");
				}
}

//	CONSTRUCTOR
ArrayDataType::ArrayDataType	(	ArrayDataType	const	&	original,	unsigned	int	newSize	)	{

				is_valid	=	original.is_valid;
				typeSpecName	=	original.typeSpecName;
				elementCount	=	newSize;
				typeDictionary	=	original.typeDictionary;

				if	(original.ownDataType	!=	NULL)	{
								ownDataType	=	original.ownDataType->clone();
				}	else	{
								ownDataType	=	NULL;
				}

				if	(	typeSpecName.empty())		{
								subType	=	ownDataType;
				}	else	{
								if	(	is_valid	)	{
												subType	=	original.subType;
								}	else	{
												subType	=	NULL;
								}
				}
}

//	COPY	CONSTRUCTOR
ArrayDataType::ArrayDataType	(	ArrayDataType	const	&	original)	{

				is_valid	=	original.is_valid;
				typeSpecName	=	original.typeSpecName;
				elementCount	=	original.elementCount;
				typeDictionary	=	original.typeDictionary;

				if	(original.ownDataType	!=	NULL)	{
								ownDataType	=	original.ownDataType->clone();
				}	else	{
								ownDataType	=	NULL;
				}

				if	(	typeSpecName.empty())		{
								subType	=	ownDataType;
				}	else	{
								if	(	is_valid	)	{
												subType	=	original.subType;
								}	else	{
												subType	=	NULL;
								}
				}
}

DataType	*	ArrayDataType::clone	()	const	{
				return	new	ArrayDataType(	*this	);
}

//	DESTRUCTOR
ArrayDataType::~ArrayDataType	()	{
				if	(	typeSpecName.empty())	{
								delete	subType;
				}
}

//	ASSIGNMENT	OPERATOR
ArrayDataType&	ArrayDataType::operator=(	const	ArrayDataType	&	rhs	)	{

				if	(	this	!=	&rhs	)	{

								//	Copy	the	RHS	members	before	messing	with	the	LHS.
								DataType	*clonedDataType;
								if	(	rhs.typeSpecName.empty()	)	{
												clonedDataType	=	rhs.subType->clone();
								}

								//	Delete	any	pre-existing	LHS	members.
								if	(	typeSpecName.empty()	)	{
												delete	subType;
								}

								//	Assign	the	copied	RHS	members	to	the	LHS.
								elementCount	=	rhs.elementCount;
								is_valid	=	rhs.is_valid;
								typeSpecName	=	rhs.typeSpecName;

								if	(	typeSpecName.empty()	)	{
												subType	=	clonedDataType;
								}	else	{
												subType	=	rhs.subType;
								}
				}
				return	*this;
}

//	MEMBER	FUNCTION
bool	ArrayDataType::validate()	{

				if	(!is_valid)	{
								//	if	we're	not	referencing	a	dictionary	type.
								if	(typeSpecName.empty())	{
												is_valid	=	ownDataType->validate();
												if	(is_valid)	{
																subType	=	ownDataType;
																typeSize	=	elementCount	*	subType->getSize();
												}
								}	else	{
												subType	=	typeDictionary->getDataType(	typeSpecName	);

												if	(subType	==	NULL)	{
																std::cerr	<<	"ERROR:	Type	\""	<<	typeSpecName	<<	"\"	not	found."	<<	std::endl;
												}	else	if	(subType->isVoid())	{
																std::cerr	<<	"ERROR:	Cannot	make	an	array	of	type	void."	<<	std::endl;
												}	else	{
																is_valid	=	true;
																typeSize	=	elementCount	*	subType->getSize();
												}
								}
				}
				return	is_valid;
}

//	MEMBER	FUNCTION
size_t	ArrayDataType::getSize()	const	{

				if	(is_valid)	{
								return	typeSize;
				}	else	{
								std::cerr	<<	"ERROR:	Can't	getSize()	because	type	hasn't	been	validated."	<<	std::endl;
								return	0;
				}
}

//	MEMBER	FUNCTION
unsigned	int	ArrayDataType::getTotalElementCount()	const	{
				unsigned	int	totalElementCount	=	elementCount	*	ownDataType->getTotalElementCount();
				return	totalElementCount;
}

//	MEMBER	FUNCTION
bool	ArrayDataType::containsPointers()	const	{

				if	((subType->containsPointers()))	{
								return	true;
				}
				return	false;
}

//	MEMBER	FUNCTION
void*	ArrayDataType::createInstance(unsigned	int	num)	const	{

				if	(subType	!=	NULL)	{
								return	subType->createInstance(	num	*	elementCount	);
				}	else	{
								std::cerr	<<	"ERROR:	Can't	create	an	instance	of	an	unvalidated	type."	<<	std::endl;
								return	(void*)	NULL;
				}
}

//	MEMBER	FUNCTION
void	ArrayDataType::deleteInstance(void*	address)	const	{
				if	(subType	!=	NULL)	{
								return	subType->deleteInstance(address);
				}	else	{
								std::cout	<<	"WARNING:	Can't	delete	an	instance	of	an	unvalidated	type."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
void	ArrayDataType::clearValue(void	*	address)	const	{
				if	(is_valid)	{
								for	(int	ii=0;	ii	<	elementCount	;	ii++)	{
												void	*	elementAddress	=	(char*)address	+	(ii	*	subType->getSize());
												subType->clearValue(	elementAddress	);
								}
				}	else	{
								std::cerr	<<	"ERROR:	Type	is	not	yet	validated.";
				}
}

//	MEMBER	FUNCTION
void	ArrayDataType::assignValue(void	*	address,	Value*value)	const	{

				if	(is_valid)	{
								CompositeValue	*	composite_value_p	=	dynamic_cast<CompositeValue*>(value);
								if	(composite_value_p)	{
												for	(int	ii=0;	ii	<	elementCount	;	ii++)	{
																void	*	elementAddress	=	(char*)address	+	(ii	*	subType->getSize());
																subType->assignValue(	elementAddress,	(*composite_value_p)[ii]	);
												}
								}	else	{
												std::cerr	<<	"ERROR:	Value	assigned	to	an	array	must	be	a	composite	value.";
								}
				}	else	{
								std::cerr	<<	"ERROR:	Type	is	not	valid	for	assignment.";
				}
}

//	MEMBER	FUNCTION
void	ArrayDataType::printValue(std::ostream	&s,	void	*	address)	const	{

				if	(is_valid)	{
								s	<<	"{";
								for	(int	ii=0;	ii	<	elementCount	;	ii++)	{
												if	(ii)	{
												s	<<	",";
												}
												void	*	elementAddress	=	(char*)	address	+	(ii	*	subType->getSize());
												subType->printValue(	s,	elementAddress	);
								}
								s	<<	"}";
				}	else	{
								std::cerr	<<	"ERROR:	Type	is	not	valid.";
				}
}

//	MEMBER	FUNCTION
std::string	ArrayDataType::getTypeSpecName()	const	{
				if	(	!typeSpecName.empty())		{
								return	typeSpecName;
				}	else	{
								return	subType->getTypeSpecName();
				}
}

//	MEMBER	FUNCTION
std::string	ArrayDataType::makeDeclaration(std::string	declarator,	bool	isDirectDeclarator)	const	{
				std::string	outTypeName;
				std::stringstream	ss;

				if	(	isDirectDeclarator	)	{
								ss	<<	declarator	<<	"["	<<	getElementCount()	<<	"]"	;
				}	else	{
								ss	<<	"("	<<	declarator	<<	")"	<<	"["	<<	getElementCount()	<<	"]"	;
				}
				if	(subType	!=	NULL)	{
								outTypeName	=	subType->makeDeclaration(	ss.str(),	true	);
				}	else	{
								outTypeName	=	getTypeSpecName()	+	ss.str();
				}
				return	outTypeName;
}

//	MEMBER	FUNCTION
unsigned	int	ArrayDataType::getElementCount()	const	{
				return	elementCount;
}

//	MEMBER	FUNCTION
const	DataType	*	ArrayDataType::getSubType()	const	{
				return	subType;
}

#ifdef	NEWSTUFF
//	MEMBER	FUNCTION
bool	ArrayDataType::getElementInfo(	LexicalAnalyzer*	lexer,	void*	baseAddress,	VarAccessInfo&	varAccessInfo	)	{

				bool	errorCondition	=	false;
				nextToken	=	lexer->getToken();
				if	(nextToken	=	Token::Integer)	{
							int	index	=	std::atoi(	(lexer->getText()).c_str()	);
				}
				if	(	(index	>=	0)	&&	(index	<	elementCount))	{
								const	DataType*			elementDataType	=	subType->getDataType();
								TypeClass::e	elementDataTypeClass	=	subType->getTypeClass();
								void*														elementAddress	=	(char*)	baseAddress	+	(index	*	subType->getSize());
								nextToken	=	lexer->getToken();
								if	(nextToken	==	Token::RightBracket)	{

												nextToken	=	lexer->getToken();
												if	(nextToken	==	Token::EndOfFile)	{
																varAccessInfo.dataType	=	elementDataType;
																varAccessInfo.address		=	elementAddress;
												}	else	if	(	nextToken	==	Token::period)	{
																if	(elementDataTypeClass	==	TypeClass::COMPOSITE)	{
																				errorCondition	|=	((const	CompositeDataType*)searchType)->getMemberInfo(	lexer,	elementAddress,	varAccessInfo);
																}	else	{
																				errorCondition	=	true;
																}
												}	else	if	(nextToken	==	Token::LeftBracket)	{
																if	(	typeClass	==	TypeClass::ARRAY	)	{
																				errorCondition	|=	((const	ArrayDataType*)searchType)->getElementInfo(	lexer,	elementAddress,	varAccessInfo);
																}	else	{
																				errorCondition	=	true;
																}
												}	else	{
																errorCondition	=	true;
												}
								}	else	{
												errorCondition	=	true;
								}
				}	else	{
								errorCondition	=	true;
				}
				return	errorCondition;
}
#endif
