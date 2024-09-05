#include	"EnumDataType.hh"
#include	"EnumDictionary.hh"
#include	"NumericValue.hh"
#include	"IntegerValue.hh"
#include	<iostream>
#include	<sstream>

EnumDataType::EnumDataType(	EnumDictionary	*	enumDictionary,
																												std::string	name,
																												size_t	enumSize)	{

				if	((enumSize	==	sizeof(int))	||	(enumSize	==	sizeof(short))	||	(enumSize	==	sizeof(char)))	{
								this->enumSize	=	enumSize;
				}	//	FIXME:	else	throw?

				this->enumDictionary	=	enumDictionary;
				this->name	=	name;
}

/*	====================================================================	*/
/*																									RULE	OF	THREE	INTERFACE																						*/
/*	====================================================================	*/

EnumDataType::EnumDataType	(	const	EnumDataType	&	original	)	{
			this->enumSize	=	original.enumSize;

			int	enum_count	=	original.enum_list.size()	;
			for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
							Enumerator*	dupl_enum	=	new	Enumerator(	*original.enum_list[ii]);
							this->enum_list.push_back(	dupl_enum	);
			}
}

EnumDataType::~EnumDataType	()	{

				int	enum_count	=	enum_list.size()	;
				for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
								delete	enum_list[ii];
				}
}

EnumDataType	&	EnumDataType::operator=(	const	EnumDataType	&	rhs	)	{
				if	(this	!=	&rhs)	{

								std::vector<Enumerator*>	new_enum_list;
								int	enum_count;

								//	Copy	the	RHS	members	before	messing	with	the	LHS.
								try	{
												enum_count	=	rhs.enum_list.size()	;
												for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
																Enumerator*	dupl_enum	=	new	Enumerator(	*rhs.enum_list[ii]);
																new_enum_list.push_back(	dupl_enum	);
												}
								}	catch	(std::logic_error)	{
												enum_count	=	new_enum_list.size()	;
												for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
																delete	new_enum_list[ii];
												}
												throw;
								}

								//	Delete	any	pre-existing	LHS	members.
								enum_count	=	enum_list.size()	;
								for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
												delete	this->enum_list[ii];
								}

								//	Assign	the	copied	RHS	members	to	the	LHS.
								enum_count	=	new_enum_list.size()	;
								for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
												enum_list.push_back(	new_enum_list[ii]	);
								}

				}
				return	*this;
}

/*	====================================================================	*/
/*																											VIRTUAL	INTERFACE																										*/
/*	====================================================================	*/

DataType	*	EnumDataType::clone	()	const	{
				return	new	EnumDataType(	*this	);
}

bool	EnumDataType::validate()	{
				return	true;
}

size_t	EnumDataType::getSize()	const	{
				return	enumSize;
}

void*	EnumDataType::createInstance(unsigned	int	num)	const	{
				void*	temp	=	(void*)calloc(num,	enumSize);
				return	temp;
}

void	EnumDataType::deleteInstance(void*	address)	const	{
				free(address);
}

void	EnumDataType::clearValue(void	*	address)	const	{

			if	(enumSize	==	sizeof(int))	{
							*(int*)address	=	0;
			}	else	if	(enumSize	==	sizeof(short))	{
							*(short*)address	=	0;
			}	else	if	(enumSize	==	sizeof(char))	{
							*(char*)address	=	0;
			}	else	{
							std::cerr	<<	"ERROR:	Enumeration	of	size	"	<<	enumSize	<<	"is	not	supported.";
			}
}

void	EnumDataType::assignValue(void	*	address,	Value	*	value)	const	{

				NumericValue	*	numeric_value_p	=	dynamic_cast<NumericValue*>(value);
				if	(numeric_value_p)	{

							if	(enumSize	==	sizeof(int))	{
											*(int*)address	=		numeric_value_p->getIntegerValue();
							}	else	if	(enumSize	==	sizeof(short))	{
											*(short*)address	=		numeric_value_p->getIntegerValue();
							}	else	if	(enumSize	==	sizeof(char))	{
											*(char*)address	=		numeric_value_p->getIntegerValue();
							}	else	{
											std::cerr	<<	"ERROR:	Enumeration	of	size	"	<<	enumSize	<<	"is	not	supported.";
							}
				}	else	{
								std::cerr	<<	"ERROR:	Attempt	to	assign	non-numeric	value	to	a	numeric	type.";
				}
}

void	EnumDataType::printValue(std::ostream	&s,	void	*address)	const	{

				int	value;

				if	(enumSize	==	sizeof(int))	{
								value	=	*(int*)address;
				}	else	if	(enumSize	==	sizeof(short))	{
								value	=	*(short*)address;
				}	else	if	(enumSize	==	sizeof(char))	{
								value	=	*(char*)address;
				}	else	{
								std::cerr	<<	"ERROR:	Enumeration	of	size	"	<<	enumSize	<<	"is	not	supported.";
				}

				//	search	the	enum_list	for	the	name/value	pair.
				int	enum_count	=	enum_list.size()	;
				int	ii	=	0;
				bool	found	=	false;
				while	((ii	<	enum_count)	&&	!found	)	{
								if	(value	==	enum_list[ii]->getValue())	{
													s	<<	enum_list[ii]->getName();
													found	=	true;
								}
								ii	++;
				}
				if	(!found)	{
								s	<<	value;
				}
}

//	MEMBER	FUNCTION
std::string	EnumDataType::toString()	const	{
				std::stringstream	ss;
				int	enum_count	=	enum_list.size()	;
				ss	<<	"enum	{\n";
				for	(int	ii=0;	ii	<	enum_count	;	ii++)	{
								if	(ii)	{
												ss	<<	",\n";
								}
								ss	<<	enum_list[ii]->toString();
				}
				ss	<<	"}\n";
				return	ss.str();
}


//	MEMBER	FUNCTION
std::string	EnumDataType::getTypeSpecName()	const	{
				return	name;
}

void	EnumDataType::addEnumerator(	std::string	name,	int	value)		{

				Enumerator*	enumerator	=	new	Enumerator(	name,	value	);
				enum_list.push_back(enumerator);

				enumDictionary->addEnumerator(name,	value);
}
