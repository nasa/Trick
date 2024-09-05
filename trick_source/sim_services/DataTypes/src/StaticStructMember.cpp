#include	"StaticStructMember.hh"
#include	"TypeDictionary.hh"
#include	"ArrayDataType.hh"
#include	<sstream>
#include	<iostream>

//	CONSTRUCTOR
StaticStructMember::StaticStructMember(std::string	memberName,
																																							void	*	memberAddress,
																																							TypeDictionary*	typeDictionary,
																																							std::string	typeSpecName,
																																							unsigned	int	n_dims,
																																							int	dims[]	)	:	StructMember(memberName)	{

			if	(	n_dims	>	0	)	{
							this->typeSpecName.clear();
							declDataType	=	new	ArrayDataType(	typeDictionary,	typeSpecName,	n_dims,	dims);
			}	else	{
							this->typeSpecName	=	typeSpecName;
							declDataType	=	NULL;
			}

			subType	=	NULL;
			this->memberAddress	=	memberAddress;
			this->typeDictionary	=	typeDictionary;
			is_valid	=	false;
}

//	CONSTRUCTOR
StaticStructMember::StaticStructMember	(	const	StaticStructMember	&	original	)
				:StructMember(	original	)	{

				memberAddress	=	original.memberAddress;
				is_valid	=	original.is_valid;
				typeSpecName	=	original.typeSpecName;

				if	(	original.declDataType	!=	NULL)	{
								declDataType	=	original.declDataType->clone();
				}	else	{
								declDataType	=	NULL;
				}

				if	(is_valid)	{
								if	(typeSpecName.empty())	{
												subType	=	declDataType;
								}	else	{
												subType	=	original.subType;
								}
				}
}

StructMember	*	StaticStructMember::clone	()	const	{
				return	new	StaticStructMember(	*this	);
}

StaticStructMember::~StaticStructMember()	{
				if	(declDataType	!=	NULL)	{
								delete	declDataType;
				}
}

bool	StaticStructMember::validate()	{

				if	(!is_valid)	{
								if	(typeSpecName.empty())	{
												is_valid	=	declDataType->validate();
												if	(is_valid)	{
																subType	=	declDataType;
												}
								}	else	{
												subType	=	typeDictionary->getDataType(	typeSpecName	);

												if	(subType	!=	NULL)	{
																is_valid	=	true;
												}	else	{
																std::cerr	<<	"ERROR:	Type	\""	<<	typeSpecName
																										<<	"\"	not	found	in	the	TypeDictionary."	<<	std::endl;
												}
								}
				}
				return	is_valid;
}


//	MEMBER	FUNCTION
bool	StaticStructMember::containsPointers()	const	{
				if	(is_valid)	{
								return	subType->containsPointers();
				}
				return	false;
}

//	MEMBER	FUNCTION
const	DataType	*	StaticStructMember::getDataType()	{
				return	subType;
}

//	MEMBER	FUNCTION
void*	StaticStructMember::getAddress()	{
				return	memberAddress;
}

//	MEMBER	FUNCTION
void	StaticStructMember::clearValue(void	*struct_address)	const	{

				if	(is_valid)	{
								subType->clearValue(	memberAddress);
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	clear	a	value	via	unvalidated	DataType."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
void	StaticStructMember::assignValue(void	*struct_address,	Value	*v	)	const	{

				if	(is_valid)	{
								subType->assignValue(	memberAddress,	v);
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	assign	a	value	via	unvalidated	DataType."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
void	StaticStructMember::printValue(std::ostream	&s,	void	*struct_address)	const	{

				//	Note	that	struct_address	is	not	used.	We	still	need	to	satisfy	the	virtual	interface.
				if	(is_valid)	{
								subType->printValue(s,	memberAddress)	;
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	print	a	value	via	unvalidated	DataType."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
std::string	StaticStructMember::	toString()	const	{
				std::ostringstream	oss;
				oss	<<	"static	";
				if	(!	typeSpecName.empty())	{
								oss	<<	typeSpecName	<<	"	"	<<	getName();
				}	else	{
								oss	<<	subType->makeDeclaration(getName(),	true);
				}
				return	oss.str();
}
