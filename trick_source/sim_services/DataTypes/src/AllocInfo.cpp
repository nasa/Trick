#include	"AllocInfo.hh"
#include	"PointerDataType.hh"
#include	"ArrayDataType.hh"
#include	"CompositeDataType.hh"
#include	"NormalStructMember.hh"
#include	"StaticStructMember.hh"
#include	"TypeDictionary.hh"
#include	"ChkPtAgent.hh"
#include	"MemMgr.hh"
#include	"ParsedDeclaration.hh"
#include	<sstream>
#include	<string.h>	//	for	memcpy

const	std::string	AllocInfo::localPrefix	=	"trick_local_";
const	std::string	AllocInfo::externPrefix	=	"trick_extern_";
unsigned	int	AllocInfo::nextSerialNumber	=	0;

//	PRIVATE	MEMBER	FUNCTION
void	AllocInfo::initialize(	const	std::string&	varName,
																												const	std::string&	typeSpecName,
																												unsigned	int							dimsCount,
																												int	*														dims,
																												TypeDictionary*				typeDictionary,
																												void*														suppliedAllocation)		{

				//	Because	they	can	be	resized,	all	allocations	are	really	arrays,
				//	whether	they	have	0,	1,	or	many	elements.
				name	=	varName;
				typeSpecifierName	=	typeSpecName;

				//	If	it's	not	specified	as	an	array,	then	make	it	an	array	of	1.
				if	((dimsCount	==	0)	||	(dims[0]	<	0))	{
								for(	int	ii=0	;	ii	<	dimsCount	;	ii++	)	{
												dimensions[ii+1]	=	dims[ii];
								}
								dimensionsCount	=	dimsCount	+	1;
								dimensions[0]	=	1;
				}	else	{
								dimensionsCount	=	dimsCount;
								for(	int	ii=0;	ii	<	dimensionsCount	;	ii++	)	{
												dimensions[ii]	=	dims[ii];
								}
				}

				ownDataType	=	new	ArrayDataType(	typeDictionary,	typeSpecifierName,	dimensionsCount,	dimensions);

				if	(ownDataType->validate())	{
								if	(ownDataType->getSize()	>	0	)	{			//	FIXME:	Is	this	correct?	Shouldn't	it	be:	(	ownDataType->getSubType()->getSize()	>	0	)	?
												dataType	=	ownDataType;
								}	else	{
												std::stringstream	errorStream	;
												errorStream	<<	"ERROR:	Data	type	\""	<<	ownDataType->makeDeclaration(	name,	true)
																								<<	"\"	is	a	useless	type	because	it's	size	can	only	ever	be	0."
																								<<	std::endl;
												delete	ownDataType;
												throw	std::logic_error(	errorStream.str());
								}
				}	else	{
								std::stringstream	errorStream	;
								errorStream	<<	"ERROR:	Data	type	\""	<<	ownDataType->makeDeclaration(	name,	true)
																				<<	"\"	can	not	be	validated.	Perhaps	the	typespecifier	\""
																				<<	typeSpecifierName	<<	"\"	is	mis-spelled?"
																				<<	std::endl;
								delete	ownDataType;
								throw	std::logic_error(	errorStream.str());
				}

				if	(	suppliedAllocation	!=	0	)	{
								start	=	suppliedAllocation;
								storageClass	=	StorageClass::EXTERN;
				}	else	{
//								start	=	dataType->createInstance(	dimensions[0]	);
								start	=	dataType->createInstance(1);
								storageClass	=	StorageClass::LOCAL;
				}

				serialNumber	=	nextSerialNumber;
				nextSerialNumber	++	;

				end	=	(char*)start	+	dataType->getSize();
}

//	CONSTRUCTOR
AllocInfo::AllocInfo(	const	std::string&	varName,
																						const	std::string&	typeSpecName,
																						unsigned	int							dimsCount,
																						int	*														dims,
																						TypeDictionary*				typeDictionary,
																						void*														suppliedAllocation)		{

				initialize(varName,	typeSpecName,	dimsCount,	dims,	typeDictionary,	suppliedAllocation);
}

//	CONSTRUCTOR
AllocInfo::AllocInfo(	const	std::string&	declaration,
																						TypeDictionary*				typeDictionary,
																						void*														suppliedAllocation)		{

				int	dims[TRICK_MAX_INDEX];
				ParsedDeclaration*	parsedDeclaration	=	new	ParsedDeclaration(	declaration	);

				std::string	variableName	=	parsedDeclaration->getVariableName();
				std::string	typeSpecifier	=	parsedDeclaration->getTypeSpecifier();
				unsigned	int	dimsCount	=	parsedDeclaration->getNumberOfDimensions();
				for	(int	ii=0	;	ii<dimsCount	;	ii++	)	{
								dims[ii]	=	parsedDeclaration->getDimensionSize(ii);
				}
				initialize(variableName,	typeSpecifier,	dimsCount,	dims,	typeDictionary,	suppliedAllocation);
}

//	DESTRUCTOR
AllocInfo::~AllocInfo()	{
				if	(ownDataType	!=	(DataType*)NULL)	{
								delete	ownDataType;
				}
				//	FIXME:	what	about	the	allocation?
}

//	PUBLIC	MEMBER	FUNCTION
std::string	AllocInfo::getName()	const	{
				return	name;
}

//	PUBLIC	MEMBER	FUNCTION
std::string	AllocInfo::getSerialName()	const	{
				std::ostringstream	oss;
				if	(storageClass	==	StorageClass::LOCAL)	{
								oss	<<	localPrefix	<<	serialNumber	;
				}	else	{
								oss	<<	externPrefix	<<	serialNumber	;
				}
				return	oss.str();
}

//	PUBLIC	MEMBER	FUNCTION
unsigned	int	AllocInfo::getSerialNumber()	const	{
				return	serialNumber;
}

//	PUBLIC	MEMBER	FUNCTION
void*	AllocInfo::getStart()	const	{
				return	start;
}

//	PUBLIC	MEMBER	FUNCTION
int	AllocInfo::getSize()	const	{
				return	dataType->getSize();
}

//	PUBLIC	MEMBER	FUNCTION
std::string	AllocInfo::getTypeSpecifierName()	const	{
				return	typeSpecifierName;
}

//	PUBLIC	MEMBER	FUNCTION
unsigned	int	AllocInfo::getDimensionsCount()	const	{
				return	dimensionsCount;
}

//	PUBLIC	MEMBER	FUNCTION
int	AllocInfo::getDimensionSize(unsigned	int	n)	const	{
				if	(	n	<	dimensionsCount	)	{
								return	dimensions[n];
				}	else	{
								return	0;
				}
}

//	PUBLIC	MEMBER	FUNCTION
const	DataType	*	AllocInfo::getDataType()	const	{
				const	DataType	*	returnType;
				if	(	ownDataType->getElementCount()	==	1	)	{
								returnType	=	ownDataType->getSubType();
				}	else	{
								returnType	=	ownDataType;
				}
				return	returnType;
}

//	PUBLIC	MEMBER	FUNCTION
StorageClass::e	AllocInfo::getStorageClass()	const	{
				return	storageClass;
}

//	PUBLIC	MEMBER	FUNCTION
void	AllocInfo::clear()	const	{
				dataType->clearValue(start);
}

//	PUBLIC	MEMBER	FUNCTION
void*	AllocInfo::resize(	size_t	newElementCount	)	{

				//	We	should	not	attempt	to	resize	memory	objects	that	were	supplied
				//	to	us.	We	should	only	attempt	to	resize	objects	that	we	created.
				if	(	storageClass	!=	StorageClass::LOCAL	)	{
								//	ERROR	CONDITION
								return	NULL;
				}
				ArrayDataType*	oldType	=	ownDataType;
				void	*	oldMemoryObject	=	start;
				size_t	oldByteCount	=	oldType->getSize();

				ArrayDataType*	newType	=	new	ArrayDataType(	*oldType,	newElementCount	);
				void	*	newMemoryObject	=	newType->createInstance(1);
				size_t	newByteCount	=	newType->getSize();

				size_t	minByteCount;
				if	(	oldByteCount	<	newByteCount	)	{
								minByteCount	=	oldByteCount;
				}	else	{
								minByteCount	=	newByteCount;
				}

				if	(minByteCount	>	0)	{
								memcpy(	newMemoryObject,	oldMemoryObject,	minByteCount);
				}

				delete	oldType;
				oldType->deleteInstance(	oldMemoryObject	);
				start	=	newMemoryObject;
				end	=	(char*)start	+	newType->getSize();
				ownDataType	=	newType;
				dataType	=	newType;
				dimensions[0]	=	newElementCount;
				return	start;
}

//	PUBLIC	MEMBER	FUNCTION
bool	AllocInfo::contains(void*	address)	const	{

				if	((	address	>=	start)	&&	(address	<=	end))	{
								return	true;
				}
				return	false;
}

//	PUBLIC	MEMBER	FUNCTION
void	AllocInfo::appendDependenciesfromComposite(	const	CompositeDataType	*	compDataType,
																																																	void	*address,
																																																	MemMgr*	memMgr,
																																																	std::vector<AllocInfo*>&	dependencies)	{

				int	memberCount	=	compDataType->getMemberCount();

				for	(int	ii	=	0;	ii	<	memberCount;	ii++)	{

								StructMember*	structMember	=	compDataType->getStructMember(ii);
								//int	ioControl	=	structMember->getIoControl();
								int	ioControl	=	1;	//	FIXME

								if	(	ioControl	==	1	||	ioControl	==	3)	{

												//	Get	the	Member	address.
												char	*memberAddress;
												const	DataType*	memberDataType;

												NormalStructMember*	normalStructMember	=	dynamic_cast<NormalStructMember*>(structMember);
												if	(	normalStructMember	)	{
															memberAddress	=	(char*)address	+	normalStructMember->getOffset();
															memberDataType	=	normalStructMember->getDataType();
												}	else	{
																StaticStructMember*	staticStructMember	=	dynamic_cast<StaticStructMember*>(structMember);
																if	(	staticStructMember	)	{
																				memberAddress	=		(char*)staticStructMember->getAddress();
																				memberDataType	=	staticStructMember->getDataType();
																}	else	{
																				memberAddress	=		NULL;	//	For	a	bitfield.
																				memberDataType	=	NULL;
																}
												}
												if	(memberAddress	!=	NULL)		{
																const	ArrayDataType*	memberArrayDataType	=	dynamic_cast<const	ArrayDataType*>(memberDataType);
																if	(	memberArrayDataType	)	{
																				appendDependenciesfromArray(	memberArrayDataType,	memberAddress,	memMgr,	dependencies);
																}	else	{
																				const	CompositeDataType*	memberCompositeDataType	=	dynamic_cast<const	CompositeDataType*>(memberDataType);
																				if	(	memberCompositeDataType	)	{
																								appendDependenciesfromComposite(	memberCompositeDataType,	memberAddress,	memMgr,	dependencies);
																				}
																}
												}
								}
				}
}

//	PUBLIC	MEMBER	FUNCTION
void	AllocInfo::appendDependenciesfromArray(	const	ArrayDataType*	arrayDataType,
																																													void*	address,
																																													MemMgr*	memMgr,
																																													std::vector<AllocInfo*>&	dependencies)	{

				int	elementCount	=	arrayDataType->getElementCount();
				if	(	elementCount	==	0	)	{
								void*	ptr	=	*(void**)address;
								AllocInfo*	allocInfo	=	memMgr->getAllocInfoOf(ptr);
								allocInfo->appendDependencies(	memMgr,	dependencies	);
				}	else	if	(	arrayDataType->containsPointers()	){
								const	DataType*	elementType	=	arrayDataType->getSubType();
								int	elementTypeSize	=	elementType->getSize();
								const	ArrayDataType*	memberArrayDataType	=	dynamic_cast<const	ArrayDataType*>(elementType);
								if	(	memberArrayDataType	)	{
												for	(int	ii=0;	ii	<	elementCount;	ii++)	{
																void	*	elementAddress	=	(char*)	address	+	(ii	*	elementTypeSize	);
																appendDependenciesfromArray(	memberArrayDataType,	elementAddress,	memMgr,	dependencies);
												}
								}	else	{
												const	CompositeDataType*	memberCompositeDataType	=	dynamic_cast<const	CompositeDataType*>(elementType);
												if	(	memberCompositeDataType	)	{
																for	(int	ii=0;	ii	<	elementCount;	ii++)	{
																				void	*	elementAddress	=	(char*)	address	+	(ii	*	elementTypeSize	);
																				appendDependenciesfromComposite(	memberCompositeDataType,	elementAddress,	memMgr,	dependencies);
																}
												}
								}
				}
}

//	PUBLIC	MEMBER	FUNCTION
void	AllocInfo::appendDependencies(	MemMgr*	memMgr,	std::vector<AllocInfo*>&	dependencies	)	{

				bool	found	=	false;
				int	n_depends	=	dependencies.size();

				//	Search	through	the	dependencies	to	see	if	"this"	is	in	it.
				for	(int	ii	=	0	;	ii	<	n_depends	;	ii	++)	{
								if	(dependencies[ii]	==	this)	{
												found	=	true;
												return;
								}
				}

				if	(!found)	{
								//	An	allocation	is	dependent	on	itself.
								dependencies.push_back(this);
								const	CompositeDataType*	compositeDataType	=	dynamic_cast<const	CompositeDataType	*>(dataType);
								if	(compositeDataType)	{
												//	Get	the	dependencies	in	the	allocation	of	the	composite	type.
												appendDependenciesfromComposite(	compositeDataType,	start,	memMgr,	dependencies	);
								}	else	{
												const	ArrayDataType*	arrayDataType	=		dynamic_cast<const	ArrayDataType	*>(dataType);
												if	(	arrayDataType	)	{
																appendDependenciesfromArray(	arrayDataType,	start,	memMgr,	dependencies	);
												}
								}
				}
}

//	PUBLIC	MEMBER	FUNCTION
std::string	AllocInfo::toString()	const	{

				std::ostringstream	oss;
				if	(storageClass	==	StorageClass::LOCAL)	{
								oss	<<	"LOCAL	";
				}	else	{
								oss	<<	"EXTERN	";
				}
				oss	<<	"<"	<<	getDataType()->makeDeclaration(	name,	true)	<<	">";
				oss	<<	"	@";
				oss	<<	std::hex	<<	start	<<	".."	;
				oss	<<	std::hex	<<	end			<<	"	"	;
				oss	<<	std::endl;
				return	oss.str();
}

#ifdef	NEWSTUFF
//	PUBLIC	MEMBER	FUNCTION
bool	AllocInfo::getVarAccessInfo(	LexicalAnalyzer*	lexer,	VarAccessInfo&	varAccessInfo	)	{

					bool	errorCondition	=	false;

					if	(dimensions[0]	==	1)	{
									searchType	=	dataType->subType();
					}	else	{
									searchType	=	dataType;
					}

					TypeClass::e	typeClass	=	searchType->getTypeClass();

					nextToken	=	lexer->getToken();
					if	(nextToken	=	Token::Identifier)	{
									std::string	temp	=	lexer->getText();
									if	(temp	!=	name)	{
														errorCondition	=	true;
									}
									nextToken	=	lexer->getToken();
					}

					if	(nextToken	==	Token::EndOfFile)	{
									varAccessInfo.dataType	=	dataType;
									varAccessInfo.address	=	start;
					}	else	if	(	nextToken	==	Token::period)	{
									if	(typeClass	==	TypeClass::COMPOSITE)	{
														errorCondition	|=	((const	CompositeDataType*)searchType)->getMemberInfo(	lexer,	start,	varAccessInfo);
									}	else	{
														errorCondition	=	true;
									}
					}	else	if	(nextToken	==	Token::LeftBracket)	{
									if	(	typeClass	==	TypeClass::ARRAY	)	{
													errorCondition	|=	((const	ArrayDataType*)searchType)->getElementInfo(	lexer,	start,	varAccessInfo);
									}	else	{
													errorCondition	=	true;
									}
					}	else	{
									errorCondition	=	true;
					}
					return	errorCondition;
}
#endif
