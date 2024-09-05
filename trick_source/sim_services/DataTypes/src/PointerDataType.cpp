#include	"PointerDataType.hh"
#include	"ArrayDataType.hh"
#include	"TypeDictionary.hh"
#include	"PointerValue.hh"
#include	"CompositeValue.hh"
#include	<iostream>
#include	<sstream>
#include	<stdlib.h>	//	for	free()



bool	PointerDataType::initPointerDataType(	TypeDictionary	*	typeDictionary,	std::string	typeSpecName,	unsigned	int	n_dims,	int	dims[])	{

				bool	errorCondition	=	false;
				//	NOTE:	(dim[0]	<	0)	indicates	a	pointer.	(dim[0]	>=	0)	indicates	an	array.
				if	((	n_dims	>=	1	)	&&	(	dims[0]	<	0	))	{
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
								is_valid	=	false;
								this->typeDictionary	=	typeDictionary;
				}	else	{
								errorCondition=	true;
				}
				return	errorCondition;
}

//	CONSTRUCTOR
PointerDataType::	PointerDataType(	TypeDictionary	*	typeDictionary,	std::string	typeSpecName,	unsigned	int	n_dims,	int	dims[])		{

			if(	this->initPointerDataType(	typeDictionary,	typeSpecName,	n_dims,	dims)	)	{
							throw	std::logic_error("PointerDataType	constructor(1)	error.");
			}
}

//	CONSTRUCTOR
PointerDataType::PointerDataType(	TypeDictionary*	typeDictionary,	std::string	typeSpecifierName)		{

			int	dims[1]	=	{-1};
			if(	this->initPointerDataType(	typeDictionary,	typeSpecName,	1,	dims)	)	{
							throw	std::logic_error("PointerDataType	constructor(2)	error.");
			}
}

//	COPY	CONSTRUCTOR
PointerDataType::PointerDataType	(	PointerDataType	const	&	original)	{

				is_valid	=	original.is_valid;
				typeSpecName	=	original.typeSpecName;
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

//	DESTRUCTOR
PointerDataType::~PointerDataType	()	{

				if	(	typeSpecName.empty())	{
								delete	subType;
				}
}

//	ASSIGNMENT	OPERATOR
PointerDataType&	PointerDataType::operator=(	const	PointerDataType	&	rhs	)	{

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
//	FIXME:	make	sure	that	this	method	in	other	DataTypes	checks	for	PointerData	types	rather	than	ArrayDataTypes.
bool	PointerDataType::containsPointers()	const	{
				return	true;
}

//	CLONE
DataType	*	PointerDataType::clone	()	const	{
				return	new	PointerDataType(	*this	);
}

//	MEMBER	FUNCTION
bool	PointerDataType::validate()	{

				if	(!is_valid)	{
								//	if	we're	not	referencing	a	dictionary	type.
								if	(typeSpecName.empty())	{
												is_valid	=	ownDataType->validate();
												if	(is_valid)	{
																subType	=	ownDataType;
												}
								}	else	{
												subType	=	typeDictionary->getDataType(	typeSpecName	);
												if	(subType	!=	NULL)	{
																is_valid	=	true;
												}	else	{
																std::cerr	<<	"ERROR:	Type	\""	<<	typeSpecName	<<	"\"	not	found."	<<	std::endl;
												}
								}
				}
				return	is_valid;
}

//	MEMBER	FUNCTION
size_t	PointerDataType::getSize()	const	{
				return	sizeof(void*);
}

//	MEMBER	FUNCTION
void*	PointerDataType::createInstance(unsigned	int	num)	const	{
				void*	temp	=	calloc(num,	sizeof(void*));
				return	temp;
}

//	MEMBER	FUNCTION
void	PointerDataType::deleteInstance(void*	address)	const	{
				free(address);
}

//	MEMBER	FUNCTION
void	PointerDataType::clearValue(void	*	address)	const	{
				*(void**)address	=		(void*)NULL;
}

//	MEMBER	FUNCTION
void	PointerDataType::assignValue(void	*	address,	Value*value)	const	{

				PointerValue	*	pointer_value_p	=	dynamic_cast<PointerValue*>(value);
				if	(pointer_value_p)	{
								*(void**)address	=		pointer_value_p->getPointer();
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	assign	non-pointer	value	to	a	pointer.";
				}
}

//	MEMBER	FUNCTION
void	PointerDataType::printValue(std::ostream	&s,	void	*	address)	const	{
				s	<<	*(void**)address;
}

//	MEMBER	FUNCTION
std::string	PointerDataType::getTypeSpecName()	const	{
				return	typeSpecName;
}

//	MEMBER	FUNCTION
std::string	PointerDataType::makeDeclaration(std::string	declarator,	bool	isDirectDeclarator)	const	{
				std::string	outTypeName;
				std::stringstream	ss;

				ss	<<	"*"	<<	declarator	;
				if	(subType	!=	NULL)	{
								outTypeName	=	subType->makeDeclaration(	ss.str(),	false	);
				}	else	{
								outTypeName	=	getTypeSpecName()	+	ss.str();
				}
				return	outTypeName;
}

//	MEMBER	FUNCTION
const	DataType	*	PointerDataType::getSubType()	const	{
				return	subType;
}


