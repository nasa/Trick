#include	"TypeDictionary.hh"
#include	"CompositeDataType.hh"
#include	"CompositeValue.hh"
#include	"NormalStructMember.hh"
#include	"StaticStructMember.hh"
#include	"BitfieldStructMember.hh"
#include	"LexicalAnalyzer.hh"
#include	<iostream>

CompositeDataType::CompositeDataType()	{

				this->is_valid	=	false;
				this->structSize	=	0;
}

//	CONSTRUCTOR
CompositeDataType::CompositeDataType(	TypeDictionary*	typeDictionary,
																																						std::string	name,
																																						size_t	structSize,
																																						void*	(*allocator)(int),
																																						void	(*deAllocator)(void*)	)	{
				this->is_valid	=	false;
				this->name	=	name;
				this->structSize	=	structSize;
				this->allocator	=	allocator;
				this->deAllocator	=	deAllocator;
				this->typeDictionary	=	typeDictionary;
}

//	CONSTRUCTOR
CompositeDataType::CompositeDataType	(	const	CompositeDataType	&	original	)	{

				this->structSize	=	original.structSize;
				this->is_valid	=	original.is_valid;

				int	memberCount	=	original.memberList.size()	;
				for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
								StructMember*	cloned_member	=	original.memberList[ii]->clone();
								this->memberList.push_back(	cloned_member	);
				}
}

DataType	*	CompositeDataType::clone()	const	{
				return	new	CompositeDataType(	*this	);
}

//	DESTRUCTOR
CompositeDataType::~CompositeDataType()	{
				int	memberCount	=	memberList.size()	;
				for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
								delete	memberList[ii];
				}
}

bool	CompositeDataType::validate()	{

				//	FIXME:	Check	for	circular	dependencies.
				//	Also	need	a	to	check	that	this->subType	(ptr)	is	not	equal
				//	to	any	of	the		member->subType's.	Otherwise	we	will	have	a	circular	reference.

				if	(!is_valid)	{

								is_valid	=	true;

								int	memberCount	=	memberList.size()	;
								for	(int	ii=0	;	ii	<	memberCount	;	ii++)	{
												is_valid	=	is_valid	&&	memberList[ii]->validate();
								}

								/*	FIXME:	Also	need	to	verify	that	the	structSize	is	at	least	as	great	as	the	max
								member	offset	plus	the	size	of	that	member.	This	may	require	StructMember	to	have
								virtual	members	for	getSize	and	getOffset.	Note	that	this	has	to	work	with	bitfields.	*/

				}
				return	is_valid;
}

//	MEMBER	FUNCTION
size_t	CompositeDataType::getSize()	const	{
				return	structSize;
}

//	OPERATOR=
CompositeDataType	&	CompositeDataType::operator=(const	CompositeDataType	&	rhs)		{
//	FIXME:
//				int	memberCount;
//				if	(this	!=	&rhs)	{
//								std::vector<StructMember*>	new_member_list;
//
//								//	Copy	the	RHS	members	before	messing	with	the	LHS.
//								try	{
//												memberCount	=	rhs.memberList.size()	;
//												for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
//																StructMember*	cloned_member	=	rhs.memberList[ii]->clone();
//																new_member_list.push_back(	cloned_member	);
//												}
//								}	catch	(std::logic_error)	{
//												memberCount	=	new_member_list.size()	;
//												for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
//																delete	new_member_list[ii];
//												}
//												throw;
//								}
//
//								//	Delete	any	pre-existing	LHS	members.
//								memberCount	=	memberList.size()	;
//								for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
//												delete	this->memberList[ii];
//								}
//
//								//	Assign	the	copied	RHS	members	to	the	LHS.
//								memberCount	=	new_member_list.size()	;
//								for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
//												memberList.push_back(	new_member_list[ii]	);
//								}
//				}
						return	*this;
}

//	MEMBER	FUNCTION
bool	CompositeDataType::containsPointer()	const	{

				int	memberCount	=	memberList.size()	;
				for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
								if	(	memberList[ii]->containsPointer()	)	{
												return	true;
								}
				}
				return	false;
}

//	MEMBER	FUNCTION
void*	CompositeDataType::createInstance(unsigned	int	num)	const	{

				void*	address;
				if	(allocator	==	NULL)	{
								//	Allocate	using	calloc.
								if	((address	=	calloc(	(size_t)num,	(size_t)structSize	))		==	NULL)	{
												std::cerr	<<	"ERROR:	Out	of	memory."	<<	std::endl;
												return	((void*)NULL);
								}
				}	else	{
								//	Allocate	using	the	allocator.
								if	((address	=	allocator(	num))	==	NULL)	{
												std::cerr	<<	"ERROR:	Out	of	memory."	<<	std::endl;
												return	((void*)NULL);
								}
				}
				return	address;
}

//	MEMBER	FUNCTION
void	CompositeDataType::deleteInstance(void*	address)	const	{

				if	(allocator	!=	NULL)	{
								if(deAllocator	!=	NULL)	{
												deAllocator(address);
								}	else	{
												//	FIXME:	WARNING.
								}
				}	else	{
								free(address);
				}
}

//	MEMBER	FUNCTION
void	CompositeDataType::clearValue(void	*	address)	const	{

				int	memberCount	=	memberList.size()	;
				for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
								memberList[ii]->clearValue(	address);
				}
}

//	MEMBER	FUNCTION
void	CompositeDataType::assignValue(void	*	address,	Value	*	value)	const	{

				//	Confirm	that	the	value	is	a	CompositeValue.
				CompositeValue	*	composite_value	=	dynamic_cast<CompositeValue*>(value);

				if	(composite_value)	{

								int	memberCount	=	memberList.size()	;

								int	num_values	=	composite_value->getMemberCount();
								if	(	memberCount	!=	num_values)	{
												std::cerr	<<	"ERROR:	number	of	members	("	<<	memberCount
																						<<	")	and	number	of	values	("	<<	num_values
																						<<	")	is	not	the	same.";
								}

								for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
												memberList[ii]->assignValue(	address,	(*composite_value)[ii]);
								}

				}	else	{
								std::cerr	<<	"ERROR:	Cannot	assign	a	non-composite	value	to	a	composite	type.";
				}
}

//	MEMBER	FUNCTION
void	CompositeDataType::printValue(std::ostream	&s,	void	*address)	const	{

			int	memberCount	=	memberList.size()	;
			s	<<	"{";
			for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
							if	(ii)	{
											s	<<	",	";
							}
							memberList[ii]->printValue(s,	address);
			}
			s	<<	"}";
}

//	MEMBER	FUNCTION
std::string	CompositeDataType::toString()	const	{
			std::stringstream	ss;
			int	memberCount	=	memberList.size()	;
			ss	<<	"composite	{\n";
			for	(int	ii=0;	ii	<	memberCount	;	ii++)	{
							ss	<<	memberList[ii]->toString();
							ss	<<	";\n";
			}
			ss	<<	"}\n";
			return	ss.str();
}

//	MEMBER	FUNCTION
std::string	CompositeDataType::getTypeSpecName()	const	{
				return	name;
}

//	MEMBER	FUNCTION
void	CompositeDataType::addRegularMember(	std::string	memberName,
																																															int	member_offset,
																																															std::string	typeSpecName,
																																															unsigned	int	n_dims,
																																															int	dims[]	)		{

				int	memberCount	=	memberList.size()	;
				for	(int	ii=0	;	ii	<	memberCount	;	ii++)	{
								if	(memberName	==	memberList[ii]->getName())	{
												std::stringstream	error_stream	;
												error_stream	<<	"ERROR:	Attempt	to	re-define	member	\""	<<	memberName	<<	"\"."	<<	std::endl;
												throw	std::logic_error(	error_stream.str());
								}
				}

				memberList.push_back(
								new	NormalStructMember(memberName,	member_offset,	typeDictionary,	typeSpecName,	n_dims,	dims)
				);
}

//	MEMBER	FUNCTION
void	CompositeDataType::addStaticMember(	std::string	memberName,
																																									void	*	memberAddress,
																																									std::string	typeSpecName,
																																									unsigned	int	n_dims,
																																									int	dims[]	)		{

				int	memberCount	=	memberList.size()	;
				for	(int	ii=0	;	ii	<	memberCount	;	ii++)	{
								if	(memberName	==	memberList[ii]->getName())	{
												std::stringstream	error_stream	;
												error_stream	<<	"ERROR:	Attempt	to	re-define	member	\""	<<	memberName	<<	"\"."	<<	std::endl;
												throw	std::logic_error(	error_stream.str());
								}
				}

				memberList.push_back(
								new	StaticStructMember(	memberName,	memberAddress,	typeDictionary,	typeSpecName,	n_dims,	dims	)
				);

}


//	MEMBER	FUNCTION
int	CompositeDataType::getMemberCount()	const	{
				return	(	(int)memberList.size()	);
}

//	MEMBER	FUNCTION
StructMember*	CompositeDataType::getStructMember	(const	int	index)	const	{
				return	(	memberList[index]	);
}

#ifdef	NEWSTUFF
//	MEMBER	FUNCTION
bool	CompositeDataType::getMemberInfo(	LexicalAnalyzer*	lexer,	void*	baseAddress,	VarAccessInfo&	varAccessInfo	)	{

				bool	errorCondition	=	false;

				Token::e	nextToken	=	lexer->getToken();

				if	(nextToken	==	Token::Identifier)	{
								std::string	memberName	=	lexer->getText();
								int	memberCount	=	memberList.size()	;
								int	ii	=	0;
								while	(	(ii	<	memberCount)	&&
																(memberName	!=	memberList[ii]->getName()))	{
												ii	++;
								}
								if	(ii	<	memberCount)	{
												StructMember*													member	=	memberList[ii];
												MemberClass::e							memberClass	=	member->getMemberClass();
												const	DataType*			memberDataType	=	member->getDataType();
												TypeClass::e	memberDataTypeClass	=	memberDataType->getTypeClass();

												void*	memberAddress	=	NULL;
												if	(	memberClass	==	MemberClass::BITFIELD	)	{
																memberAddress	=	baseAddress;
												}	else	if	(	memberClass	==	MemberClass::STATIC	)	{
																memberAddress	=	((StaticStructMember*)member)->getAddress();
												}	else	if	(	memberClass	==	MemberClass::NORMAL	)	{
																memberAddress	=		baseAddress	+	((NormalStructMember*)member)->getOffset()	;
												}	else	{
																errorCondition	=	true;
												}

												if	(!errorCondition)	{
																nextToken	=	lexer->getToken();
																if	(nextToken	==	Token::EndOfFile)	{
																				varAccessInfo.dataType	=	memberDataType;
																				varAccessInfo.address	=	memberAddress;
																}	else	if	(nextToken	==	Token::period)	{
																				if	(memberDataTypeClass	==	TypeClass::COMPOSITE)	{
																								errorCondition	|=	((const	CompositeDataType*)memberDataType)->getMemberInfo(	lexer,	memberAddress,	varAccessInfo);
																				}	else	{
																								errorCondition	=	true;
																				}
																}	else	if	(nextToken	==	Token::LeftBracket)	{
																				if	(	typeClass	==	TypeClass::ARRAY	)	{
																								errorCondition	|=	((const	ArrayDataType*)searchType)->getElementInfo(	lexer,	memberAddress,	varAccessInfo);
																				}	else	{
																								errorCondition	=	true;
																				}
																}	else	{
																				errorCondition	=	true;
																}
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
