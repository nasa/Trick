#include	"NormalStructMember.hh"
#include	"TypeDictionary.hh"
#include	"ArrayDataType.hh"
#include	<sstream>
#include	<iostream>

//	CONSTRUCTOR
NormalStructMember::NormalStructMember(std::string	memberName,
																																							int	offset,
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
			byte_offset	=	offset;
			is_valid	=	false;
			this->typeDictionary	=	typeDictionary;
}

//	CONSTRUCTOR
NormalStructMember::NormalStructMember	(	const	NormalStructMember	&	original	)
				:StructMember(	original	)	{

				byte_offset	=	original.byte_offset;
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

StructMember	*	NormalStructMember::clone	()	const	{
				return	new	NormalStructMember(	*this	);
}

NormalStructMember::~NormalStructMember()	{
				if	(declDataType	!=	NULL)	{
								delete	declDataType;
				}
}

bool	NormalStructMember::validate()	{

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
bool	NormalStructMember::containsPointers()	const	{
				if	(is_valid)	{
								return	subType->containsPointers();
				}
				return	false;
}

//	MEMBER	FUNCTION
const	DataType	*	NormalStructMember::getDataType()	{
				return	subType;
}

//	MEMBER	FUNCTION
int	NormalStructMember::getOffset()	{
				return	byte_offset;
}

//	MEMBER	FUNCTION
void	NormalStructMember::clearValue(void	*struct_address	)	const	{

				if	(is_valid)	{
								void	*	member_address	=	(char*)struct_address	+	byte_offset;
								subType->clearValue(	member_address);
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	clear	a	value	via	unvalidate	DataType."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
void	NormalStructMember::assignValue(void	*struct_address,	Value	*v	)	const	{

				if	(is_valid)	{
								void	*	member_address	=	(char*)struct_address	+	byte_offset;
								subType->assignValue(	member_address,	v);
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	assign	a	value	via	unvalidate	DataType."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
void	NormalStructMember::printValue(std::ostream	&s,	void	*struct_address)	const	{
				if	(is_valid)	{
								void	*	member_address	=	(char*)struct_address	+	byte_offset;
								subType->printValue(s,	member_address)	;
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	print	a	value	via	unvalidate	DataType."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
std::string	NormalStructMember::	toString()	const	{
				std::ostringstream	oss;
				if	(!	typeSpecName.empty())	{
								oss	<<	typeSpecName	<<	"	"	<<	getName();
				}	else	{
								oss	<<	subType->makeDeclaration(	getName(),	true);
				}
				return	oss.str();
}
