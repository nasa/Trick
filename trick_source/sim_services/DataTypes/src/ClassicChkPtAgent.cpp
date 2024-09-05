#include	"ClassicChkPtAgent.hh"

#include	"DataType.hh"

#include	"AllocInfo.hh"

#include	"PrimitiveDataType.hh"
#include	"ArrayDataType.hh"
#include	"PointerDataType.hh"
#include	"CompositeDataType.hh"
#include	"EnumDataType.hh"

#include	"StructMember.hh"
#include	"NormalStructMember.hh"
#include	"StaticStructMember.hh"
#include	"BitfieldStructMember.hh"

#include	"MemMgr.hh"

//	STATIC	FUNCTION
static	void	writeStringLiteral(	std::ostream&	os,	const	char*	s)	{
				int	ii;
				int	len	=	strlen(s);
				os	<<	"\""	;
				for	(ii=0	;	ii<len	;	ii++)	{
								switch	((s)[ii])	{
								case	'\n':	os	<<	"\\n";	break;
								case	'\t':	os	<<	"\\t";	break;
								case	'\b':	os	<<	"\\b";	break;
								case	'\"':	os	<<	"\\\"";	break;
								default		:	os	<<	s[ii]	;	break;
								}
				}
				os	<<	"\""	;
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	pushName(	const	std::string&	name	)	{

				std::stringstream	nameStream;
				if	(leftSideNameStack.size()	==	0)	{
								nameStream	<<	name;
				}	else	{
								nameStream	<<		'.'	<<	name	;
				}
				leftSideNameStack.push_back(	nameStream.str()	);
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	pushIndex(	int	index)	{

				std::stringstream	nameStream;
				nameStream	<<	'['	<<	index	<<	']';
				leftSideNameStack.push_back(	nameStream.str()	);
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	popNameElement()	{

				int	numElements	=	leftSideNameStack.size();
				if	(numElements	>	0)	{
								leftSideNameStack.pop_back();
				}	else	{
								errorStream	<<	"WARNING:	ClassicChkPtAgent::popNameElement	called	with	an	empty	name	stack."	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
std::string	ClassicChkPtAgent::getLeftSideName()	{

				std::stringstream	nameStream;
				int	numElements	=	leftSideNameStack.size();
				for	(int	ii	=	0;	ii	<	numElements;	ii	++	)	{
								std::string	element	=	leftSideNameStack[ii];
								nameStream	<<	element;
				}
				return	nameStream.str();
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::	writeDeclaration(	std::ostream&	out_s,	std::string	name,	const	DataType	*	dataType)	{
				bool	errorCondition	=	false;
				if	(dataType	!=	NULL)	{
								out_s	<<	dataType->makeDeclaration(name,	true);
								out_s	<<	";\n";
				}	else	{
								errorCondition	=	true;
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::writeVariable(	std::ostream&	out_s,	MemMgr*	memMgr,	void*	address,	const	DataType*	dataType	)	{

				bool	errorCondition	=	false;
				TypeClass::e	typeClass	=	dataType->getTypeClass();
				if	(	(	typeClass	==	TypeClass::PRIMITIVE	)	||	(	typeClass	==	TypeClass::ENUMERATION	)	)	{
								std::stringstream	rightHandSide;
								dataType->printValue(	rightHandSide,	address	);
								writeAssignment(	out_s,	rightHandSide.str()	);
								out_s	<<	"\n";
				}	else	if	(	typeClass	==	TypeClass::POINTER	)	{
								std::stringstream	pointerName;
								errorCondition	|=	writePointerValue(	pointerName,	memMgr,	address,	(const	PointerDataType*)	dataType);
								writeAssignment(out_s,	pointerName.str());
								out_s	<<	"\n";
				}	else	if	(	typeClass	==	TypeClass::COMPOSITE	)	{
								errorCondition	|=	writeCompositeAssignments(	out_s,	memMgr,	address,	(const	CompositeDataType*)dataType);
				}	else	if	(	typeClass	==	TypeClass::ARRAY	)	{
								errorCondition	|=	writeArrayAssignments(		out_s,	memMgr,	address,	(const	ArrayDataType*)dataType);
				}	else	{
								errorCondition	=	true;
								errorStream	<<	"ERROR:	Unknown	TypeClass	in	ClassicChkPtAgent::writeVariable."	<<	std::endl;
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::writePointerValue(	std::ostream&	out_s,	MemMgr*	memMgr,	void*	address,	const	PointerDataType*	pointerDataType	)	{

				bool	errorCondition	=	false;
				void*	pointerValue	=	*(void**)address;
				const	DataType*	subType	=	pointerDataType->getSubType();	//	FIXME:	this	can	return	NULL
				const	AllocInfo*	allocInfo	=	memMgr->getAllocInfoOf(	pointerValue	);

				if	(allocInfo	!=	NULL)	{
								out_s	<<	"&"	<<	allocInfo->getName();
								errorCondition	|=	getSubName(	out_s,	pointerValue,	subType,	allocInfo->getStart(),	allocInfo->getDataType()	);
				}	else	if	(	subType	==	memMgr->getDataType(	"char"	))	{
								writeStringLiteral(	out_s,	(char*)pointerValue	);
				}	else	{
								errorCondition	=	true;
								errorStream	<<	"ERROR:	The	variable	\""	<<	getLeftSideName()	<<	"\"	contains	";
								errorStream	<<	"a	pointer	to	memory	that	isnt	managed	by	Trick	";
								errorStream	<<	"and	it	isn't	a	char	pointer."	<<	std::endl;
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::	writeArrayAssignments(	std::ostream&	out_s,	MemMgr*	memMgr,	void*	address,	const	ArrayDataType*	arrayDataType	)	{

				bool	errorCondition	=	false;
				int														elementCount					=	arrayDataType->getElementCount();
				const	DataType	*	elementType						=	arrayDataType->getSubType();	//	FIXME:	this	can	return	NULL
				size_t											elementTypeSize		=	elementType->getSize();

				TypeClass::e	elementTypeClass	=	elementType->getTypeClass();

				if	(	makeCompactArrays	&&
									((	elementTypeClass	==	TypeClass::PRIMITIVE)	||
										(	elementTypeClass	==	TypeClass::ENUMERATION)	||
										(	elementTypeClass	==	TypeClass::POINTER)))	{

								std::stringstream	rightHandSide;
								rightHandSide	<<	"{"	;
								for	(int	ii=0;	ii	<	elementCount;	ii++)	{
												void	*	elementAddress	=	(char*)	address	+	(ii	*	elementTypeSize	);
												if	(ii	!=	0)	{
																rightHandSide	<<	",";
												}
												if	(	elementTypeClass	==	TypeClass::POINTER	)	{
																errorCondition	|=	writePointerValue(	rightHandSide,	memMgr,	elementAddress,	(const	PointerDataType*)	elementType);
												}	else	{
																elementType->printValue(	rightHandSide,	elementAddress	);
												}
								}
								rightHandSide	<<	"}";
								writeAssignment(	out_s,	rightHandSide.str()	);
								out_s	<<	"\n";
				}	else	{
								for	(int	ii=0;	ii	<	elementCount;	ii++)	{
												void	*	elementAddress	=	(char*)	address	+	(ii	*	elementTypeSize	);
												pushIndex(ii);
												errorCondition	|=	writeVariable(	out_s,	memMgr,	elementAddress,	elementType	);
												popNameElement();
								}
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::writeCompositeAssignments(	std::ostream&	out_s,	MemMgr*	memMgr,	void*	address,	const	CompositeDataType*	compDataType)	{

				bool	errorCondition	=	false;

				int	memberCount	=	compDataType->getMemberCount();

				for	(int	ii	=	0;	ii	<	memberCount;	ii++)	{

								//int	ioControl	=	structMember->getIoControl();
								int	ioControl	=	1;	//	FIXME:	implement	Trick's	IOControl.

								if	(	ioControl	==	1	||	ioControl	==	3)	{

												StructMember*	structMember	=	compDataType->getStructMember(ii);
												MemberClass::e	memberClass	=	structMember->getMemberClass();

												if	(	memberClass	==	MemberClass::BITFIELD	)	{
																std::stringstream	rightHandSide;
																structMember->printValue(	rightHandSide,	address	);
																writeAssignment(	out_s,	rightHandSide.str()	);
																out_s	<<	"\n";
												}	else	{
																char*	memberAddress;
																const	DataType*	memberDataType;
																if	(	memberClass	==	MemberClass::NORMAL	)	{
																			NormalStructMember*	normalStructMember	=	(NormalStructMember*)structMember;
																			memberAddress	=	(char*)address	+	normalStructMember->getOffset();
																			memberDataType	=	normalStructMember->getDataType();
																}	else	if	(	memberClass	==	MemberClass::STATIC	)	{
																				StaticStructMember*	staticStructMember	=	(StaticStructMember*)structMember;
																				memberAddress	=		(char*)staticStructMember->getAddress();
																				memberDataType	=	staticStructMember->getDataType();
																}	else	{
																				errorCondition	=	true;
																				errorStream	<<	"ERROR:	"	<<	std::endl;
																				return	errorCondition;
																}
																std::string	memberName	=	structMember->getName();

																pushName(memberName.c_str());
																errorCondition	|=	writeVariable(	out_s,	memMgr,	memberAddress,	memberDataType);
																popNameElement();
												}
								}
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	writeAssignment(	std::ostream&	out_s,	std::string	rightHandSide	)	{
				out_s	<<	getLeftSideName()	<<	"	=	"	<<	rightHandSide	<<	";"	;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::	getArraySubName(	std::ostream&	nameStream,
																																										void	*	searchAddress,
																																										const	DataType	*	searchType,
																																										void	*	baseAddress,
																																										const	ArrayDataType	*	baseType	)	{
				bool			errorCondition	=	false;
				size_t	baseTypeSize	=		baseType->getSize();

				if	((	searchAddress	>=	(char	*)baseAddress	)	&&
								(	searchAddress	<		(char	*)baseAddress	+	baseTypeSize))	{

								const	DataType	*	elementType						=	baseType->getSubType();	//	FIXME:	this	can	return	NULL
								size_t											elementTypeSize		=	elementType->getSize();
								long													elementIndex					=	((long)searchAddress	-	(long)baseAddress)	/	elementTypeSize;
								void	*											elementAddress			=	(char*)	baseAddress	+	(	elementIndex	*	elementTypeSize	);

								nameStream	<<	"["	<<	elementIndex	<<	"]";

								errorCondition	=	getSubName(	nameStream,	searchAddress,	searchType,	elementAddress,	elementType);

				}	else	{
								errorCondition	=	true;
								errorStream	<<	"ERROR:	Bad	call	to	ClassicChkPtAgent::getArraySubName().	"
																				<<	"The	searchAddress	("	<<	searchAddress	<<	")	"
																				<<	"is	not	within	the	bounds	of	the	base-object	"
																				<<	"("	<<	baseAddress	<<	".."
																				<<	(void*)((char	*)baseAddress	+	baseTypeSize)	<<	")"	<<	std::endl;
				}

				return	errorCondition;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::getCompositeSubName(	std::ostream&	nameStream,
																																														void	*	searchAddress,
																																														const	DataType	*	searchType,
																																														void	*	baseAddress,
																																														const	CompositeDataType	*	baseType	)	{
				bool			errorCondition	=	false;
				size_t	baseTypeSize	=	baseType->getSize();

				if	((	searchAddress	>=	(char	*)baseAddress	)	&&
								(	searchAddress	<		(char	*)baseAddress	+	baseTypeSize))	{

								int		memberCount	=	baseType->getMemberCount();
								int		memberIndex	=	0;
								bool	memberFound	=	false;

								while	((memberIndex	<	memberCount)	&&	(memberFound	==	false))	{

												StructMember*	structMember	=	baseType->getStructMember(memberIndex);
												MemberClass::e	memberClass	=	structMember->getMemberClass();

												/*
												We	don't	consider	BITFIELD	members	because	pointers	to	bitfields	are	meaningless.
												Also	note	that	we	don't	consider	STATIC	class	members.	This	is	because	static
												members	aren't	really	class	object	members,	rather	they	are	global	variables.
												*/

												//	FIXME:	Static	class	variables	are	dealt	with	else	where.

												if	(	memberClass	==	MemberClass::NORMAL	)	{

																NormalStructMember*	normalStructMember	=	(NormalStructMember	*)structMember;

																const	DataType	*	memberDataType		=	normalStructMember->getDataType();	//	FIXME:	this	can	return	NULL
																size_t											memberTypeSize		=	memberDataType->getSize();
																int														memberOffset				=	normalStructMember->getOffset();
																char	*											memberAddress			=	(char*)baseAddress	+	memberOffset;

																if	(	(	searchAddress	>=	(char	*)memberAddress	)	&&
																					(	searchAddress	<	(char	*)memberAddress	+	memberTypeSize	))	{

																				memberFound	=	true;
																				nameStream	<<	"."	<<	structMember->getName();
																				errorCondition	=	getSubName(	nameStream,	searchAddress,	searchType,	memberAddress,	memberDataType);

																}
												}
												memberIndex	++;
								}

								if	(!memberFound)	{
												errorCondition	=	true;
												errorStream	<<	"ERROR:	Bad	call	to	ClassicChkPtAgent::getCompositeSubName().	"
																								<<	"The	searchAddress	("	<<	searchAddress	<<	")	"
																								<<	"is	not	within	the	bounds	of	the	base-object	"
																								<<	"("	<<	baseAddress	<<	".."
																								<<	(void*)((char	*)baseAddress	+	baseTypeSize)	<<	")"	<<	std::endl;
								}

				}	else	{
								errorCondition	=	true;
								errorStream	<<	"ERROR:	Bad	call	to	ClassicChkPtAgent::getCompositeSubName().	The	address	in	"
																				<<	"the	parameter	<searchAddress>	is	not	within	the	bounds	of	the	object	"
																				<<	"at	<baseAddress>."	<<	std::endl;
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
bool	ClassicChkPtAgent::	getSubName(	std::ostream&	nameStream,
																																					void	*	searchAddress,
																																					const	DataType	*	searchType,
																																					void	*	baseAddress,
																																					const	DataType	*	baseType	)	{
				bool			errorCondition	=	false;
				size_t	baseTypeSize	=		baseType->getSize();

				if	(	(	searchAddress	>=	(char	*)baseAddress	)	&&
									(	searchAddress	<	(char	*)baseAddress	+	baseTypeSize))	{

								if	(	searchType	==	baseType	)	{
												if	(	searchAddress	!=	baseAddress	)	{
																errorCondition	=	true;
																errorStream	<<	"ERROR:	Something	is	messed	up.	Misaligned	pointer."	<<	std::endl;
																nameStream	<<	"	+	"	<<	((long)searchAddress	-	(long)baseAddress);
												}
								}	else	{
												TypeClass::e	typeClass	=	baseType->getTypeClass();

												if	(	typeClass	==	TypeClass::COMPOSITE	)	{
																errorCondition	=	getCompositeSubName(	nameStream,	searchAddress,	searchType,	baseAddress,	(const	CompositeDataType	*)baseType	);
												}	else	if	(	typeClass	==	TypeClass::ARRAY	)	{
																errorCondition	=	getArraySubName(	nameStream,	searchAddress,	searchType,	baseAddress,	(const	ArrayDataType	*)baseType	);
												}	else	{
																errorCondition	=	true;
																errorStream	<<	"ERROR:	The	data-type	of	\""	<<	getLeftSideName()	<<	"	doesnt	match	what	it's	pointing	to."	<<	std::endl;
												}
								}
				}	else	{
								errorCondition	=	true;
								errorStream	<<	"ERROR:	Bad	call	to	ClassicChkPtAgent::getSubName().	"
																				<<	"The	searchAddress	("	<<	searchAddress	<<	")	"
																				<<	"is	not	within	the	bounds	of	the	base-object	"
																				<<	"("	<<	baseAddress	<<	".."
																				<<	(void*)((char	*)baseAddress	+	baseTypeSize)	<<	")"	<<	std::endl;
				}
				return	errorCondition;
}

//	MEMBER	FUNCTION
int	ClassicChkPtAgent::	restore(	std::istream*	checkpoint_stream)	{	return(0);	}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	setReducedCheckpoint(bool	flag)	{
				reducedCheckpoint	=	flag;
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	setHexfloatCheckpoint(bool	flag)	{
				hexfloatCheckpoint	=	flag;
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	setMakeCompactArrays(bool	flag)	{
				makeCompactArrays	=	flag;
}

//	MEMBER	FUNCTION
void	ClassicChkPtAgent::	setDebugLevel(int	level)	{	}
