#include	"MemMgr.hh"
#include	"ClassicChkPtAgent.hh"
#include	"AllocInfo.hh"
#include	"TypeDictionary.hh"
#include	"ArrayDataType.hh"
#include	"ParsedDeclaration.hh"
#include	<iostream>
#include	<fstream>

//	Constructor
MemMgr::MemMgr()	{

			debugLevel	=	0;
			reducedCheckpoint	=	true;
			hexfloatCheckpoint	=	false;
			compactArraysCheckpoint	=	true;
			
			defaultCheckPointAgent	=	new	ClassicChkPtAgent(	std::cout	);
			defaultCheckPointAgent->setDebugLevel(	debugLevel);
			defaultCheckPointAgent->setReducedCheckpoint(	reducedCheckpoint);
			defaultCheckPointAgent->setHexfloatCheckpoint(	hexfloatCheckpoint);
			defaultCheckPointAgent->setMakeCompactArrays(	compactArraysCheckpoint	);
			
			currentCheckPointAgent	=	defaultCheckPointAgent;

			typeDictionary	=	new	TypeDictionary();
}

//	MEMBER	FUNCTION
void*	MemMgr::declare_var(	const	std::string&	typeSpecifier,
																											const	std::string&	variableName,
																											unsigned	int							dimensionsCount,
																											int*															dimensions,
																											void*														suppliedAllocation	)	{

				if	(	var_exists(	variableName	))	{
								std::cerr	<<	"ERROR:	Variable	"	<<	variableName	<<	"	already	declared."	<<	std::endl;
								return	((void*)NULL);
				}

				AllocInfo*	newAllocInfo;
				void*	actualAllocation;
				try	{
								//	FIXME:	MUTEX	LOCK	to	protect	AllocInfo	static	variables
								newAllocInfo	=	new	AllocInfo(	variableName,
																																						typeSpecifier,
																																						dimensionsCount,
																																						dimensions,
																																						typeDictionary,
																																						suppliedAllocation	);
								actualAllocation	=	newAllocInfo->getStart();
								allocInfoByAddressMap[actualAllocation]	=	newAllocInfo;
								allocInfoByNameMap[variableName]	=	newAllocInfo;
				}	catch	(	std::logic_error	e	)	{
								//	FIXME:	MUTEX	UNLOCK
								std::cerr	<<	e.what();
								newAllocInfo	=	(AllocInfo*)NULL;
								actualAllocation	=	(void*)NULL;
				}
				if	(	debugLevel	)	{
								if	(	newAllocInfo	==	(AllocInfo*)NULL	)	{
												std::cout	<<	"Allocation	Failed."	<<	std::endl;
								}	else	{
												std::cout	<<	newAllocInfo->toString()	<<	std::endl;
								}
				}
				return(	actualAllocation	);
}

//	MEMBER	FUNCTION
void*	MemMgr::declare_var(	const	std::string&	declaration,
																											void*														suppliedAllocation	)	{

				ParsedDeclaration*	typeName	=	new	ParsedDeclaration(	declaration);
				std::string	typeSpecifier	=	typeName->getTypeSpecifier();
				std::string	variableName	=	typeName->getVariableName();

				unsigned	int	dimensionsCount	=	typeName->getNumberOfDimensions();
				int	dimensions[16];
				for	(int	ii=0	;	ii	<	dimensionsCount;	ii++)	{
								dimensions[ii]	=	typeName->getDimensionSize(ii);
				}
				void*	actualAllocation	=	declare_var(	typeSpecifier,	variableName,	dimensionsCount,	dimensions,	suppliedAllocation	);
				return	actualAllocation;
}

//	MEMBER	FUNCTION
void*	MemMgr::declare_var(	const	std::string&	typeSpecName,
																											const	std::string&	variableName,
																											int																n,
																											void*														suppliedAllocation)	{

				void*	actualAllocation	=	declare_var(typeSpecName,	variableName,	1,	&n,	suppliedAllocation	);
				return	actualAllocation;
}

//	MEMBER	FUNCTION
void*	MemMgr::declare_var(	const	std::string&	typeSpecName,
																											int																n,
																											void*														suppliedAllocation	)	{
				void*	actualAllocation	=	declare_var(typeSpecName,	"",	1,	&n,	suppliedAllocation);
				return	actualAllocation;
}

//	MEMBER	FUNCTION
void*	MemMgr::resize_var(	void*	address,	size_t	newElementCount)	{
				void*	newAddress;
				AllocInfo*	allocInfo	=	getAllocInfoOf(address);
				if	(allocInfo	!=	NULL)	{
								newAddress	=	allocInfo->resize(	newElementCount	);
				}	else	{
								std::cerr	<<	__FUNCTION__	<<	"	failed.	Address	("	<<	address	<<	")	not	in	Trick	managed	memory."	<<	std::endl;
								newAddress	=	NULL;
				}
				return	newAddress;
}

//	MEMBER	FUNCTION
void*	MemMgr::resize_var(	const	std::string&	name,	size_t	newElementCount)	{
			void*	newAddress;
			AllocInfo*	allocInfo	=	getAllocInfoNamed(name);
				if	(allocInfo	!=	NULL)	{
								newAddress	=	allocInfo->resize(	newElementCount	);
				}	else	{
								std::cerr	<<	__FUNCTION__	<<	"	failed.	Name	\""	<<	name	<<	"\"	not	in	Trick	managed	memory."	<<	std::endl;
								newAddress	=	NULL;
				}
				return	newAddress;
}

//	MEMBER	FUNCTION
//				void*	resize_var(	const	char*	name,	int	num);


//	MEMBER	FUNCTION
bool	MemMgr::var_exists(	const	std::string&	variableName)	{

				if	(	!variableName.empty())	{
								AllocInfo*	allocInfo	=	getAllocInfoNamed(	variableName	);
								if	(	allocInfo	!=	(AllocInfo*)NULL	)	{
												return	true;
								}
				}
				return	false;
}

//	MEMBER	FUNCTION
void	MemMgr::clear_var(	void*	address)	{

				AllocInfo*	allocInfo;
				if	((allocInfo	=	getAllocInfoOf(	address))	!=	NULL)	{
								allocInfo->clear();
				}	else	{
								std::stringstream	ss;
								ss	<<	"Cannot	clear	the	variable	at	address	";
								ss	<<	address;
								ss	<<	"	because	memory	manager	knows	nothing	about	it.";
								ss	<<	std::endl;
								std::cerr	<<	ss.str();
				}
}

//	MEMBER	FUNCTION
void	MemMgr::clear_var(	const	std::string&	variableName)	{

				AllocInfo*	allocInfo;
				if	((allocInfo	=	getAllocInfoNamed(	variableName	))	!=	NULL)	{
								allocInfo->clear();
				}	else	{
								std::stringstream	ss;
								ss	<<	"Cannot	clear	the	variable	named	";
								ss	<<	variableName;
								ss	<<	"	because	memory	manager	knows	nothing	about	it.";
								ss	<<	std::endl;
								std::cerr	<<	ss.str();
				}
}

//	MEMBER	FUNCTION
void	MemMgr::clear_all_vars()	{
}

//	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(	std::ostream&	out_s)	{

				AllocInfo*	allocInfo;
				std::vector<AllocInfo*>	allocInfoList;
				std::map<void*,	AllocInfo*>::iterator	pos;

				pthread_mutex_lock(&allocInfoMapMutex);

				for	(	pos	=	allocInfoByAddressMap.begin()	;	pos	!=	allocInfoByAddressMap.end()	;	pos++	)	{
								allocInfo	=	pos->second;
								allocInfoList.push_back(allocInfo);
				}
				write_checkpoint(	out_s,	allocInfoList);

				pthread_mutex_unlock(&allocInfoMapMutex);
}

//	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(	const	std::string&	filename)	{

				std::ofstream	out_s(	filename.c_str(),	std::ios::out);
				if	(out_s.is_open())	{
								write_checkpoint(	out_s	);
				}	else	{
								std::cerr	<<	"ERROR:	Couldn't	open	\""<<	filename	<<"\"."	<<	std::endl;
								std::cerr.flush();
				}
}

//	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(	std::ostream&	out_s,	const	std::string&	variableName)	{

				std::vector<AllocInfo*>	dependencies;
				pthread_mutex_lock(&allocInfoMapMutex);
				AllocInfo	*	allocInfo	=	getAllocInfoNamed(	variableName	);
				if	(	allocInfo	!=	NULL	)	{
								allocInfo->appendDependencies(	this,	dependencies	);
				}
				pthread_mutex_unlock(&allocInfoMapMutex);
				write_checkpoint(	out_s,	dependencies);
}

//	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(const	std::string&	filename,	const	std::string&	variableName)	{

				std::ofstream	out_s(	filename.c_str(),	std::ios::out);
				if	(out_s.is_open())	{
								write_checkpoint(	out_s,	variableName);
				}	else	{
								std::cerr	<<	"ERROR:	Couldn't	open	\""<<	filename	<<"\"."	<<	std::endl;
								std::cerr.flush();
				}
}

//	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(	std::ostream&	out_s,	std::vector<const	char*>&	variableNameList)	{

				std::vector<AllocInfo*>	dependencies;
				const	char*	variableName;
				int	nameCount;
				nameCount	=	variableNameList.size();
				for	(int	ii=0;	ii<	nameCount;	ii++)	{
								variableName	=	variableNameList[ii];
								pthread_mutex_lock(&allocInfoMapMutex);
								AllocInfo*	allocInfo	=	getAllocInfoNamed(	variableName	);
								if	(	allocInfo	!=	NULL	)	{
												allocInfo->appendDependencies(	this,	dependencies	);
								}
								pthread_mutex_unlock(&allocInfoMapMutex);
				}
				write_checkpoint(	out_s,	dependencies);
}

//	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(const	std::string&	filename,	std::vector<const	char*>&	variableNameList)	{

				std::ofstream	out_s(	filename.c_str(),	std::ios::out);
				if	(out_s.is_open())	{
								write_checkpoint(	out_s,	variableNameList	);
				}	else	{
								std::cerr	<<	"ERROR:	Couldn't	open	\""<<	filename	<<"\"."	<<	std::endl;
								std::cerr.flush();
				}
}

//	STATIC	FUNCTION
static	bool	allocInfoCompare(	AllocInfo*	lhs,	AllocInfo*	rhs	)	{
				bool	result;
				StorageClass::e	lhsStorageClass	=	lhs->getStorageClass();
				StorageClass::e	rhsStorageClass	=	rhs->getStorageClass();
				unsigned	int	lhsSerialNumber	=	lhs->getSerialNumber();
				unsigned	int	rhsSerialNumber	=	rhs->getSerialNumber();

				if	(	lhsStorageClass	==	rhsStorageClass	)	{
								result	=	(	lhsSerialNumber	<	rhsSerialNumber	);
				}	else	if	(	lhsStorageClass	==	StorageClass::	EXTERN	)	{
								result	=	true;
				}	else	{
								result	=	false;
				}
				return	result;
}

//	PRIVATE	MEMBER	FUNCTION
void	MemMgr::write_checkpoint(std::ostream&	outStream,	std::vector<AllocInfo*>&	allocInfoList)	{

				std::sort(	allocInfoList.begin(),	allocInfoList.end(),	allocInfoCompare	)	;

				//	2)	Generate	declaration	statements	for	each	the	local	allocations.
				outStream	<<	"//	Variable	Declarations."	<<	std::endl;
				outStream.flush();

				int	itemCount	=	allocInfoList.size();

				for	(int	ii	=	0	;	ii	<	itemCount	;	ii	++)	{
								AllocInfo	*	allocInfo	=	allocInfoList[ii];
								StorageClass::e	storageClass	=	allocInfo->getStorageClass();
								if	(	storageClass	==	StorageClass::LOCAL)	{
																std::string	name	=	allocInfo->getName();
												if	(	!name.empty()	)	{
																currentCheckPointAgent->writeDeclaration(	outStream,	name,	allocInfo->getDataType()	);
												}	else	{
																currentCheckPointAgent->writeDeclaration(	outStream,	allocInfo->getSerialName(),	allocInfo->getDataType()	);
												}
								}
				}

				//	3)	Write	a	"clear_all_vars"	command.
				outStream	<<	std::endl	<<	std::endl	<<	"//	Clear	all	allocations	to	0."	<<	std::endl;
				outStream	<<	"clear_all_vars();"	<<	std::endl;

				//	4)	Dump	the	contents	of	each	of	the	local	and	extern	allocations.
				outStream	<<	std::endl	<<	std::endl	<<	"//	Variable	Assignments."	<<	std::endl;
				outStream.flush();

				for	(int	ii	=	0	;	ii	<	itemCount	;	ii	++)	{
								AllocInfo	*	allocInfo	=	allocInfoList[ii];

								std::string	name	=	allocInfo->getName();
								if	(	!name.empty()	)	{
												currentCheckPointAgent->pushName(	name	);
												currentCheckPointAgent->writeVariable(	outStream,	this,	allocInfo->getStart(),	allocInfo->getDataType()	);
								}	else	{
												currentCheckPointAgent->pushName(	allocInfo->getSerialName()	);
												currentCheckPointAgent->writeVariable(	outStream,	this,	allocInfo->getStart(),	allocInfo->getDataType()	);
								}
								currentCheckPointAgent->popNameElement();
								outStream	<<	std::endl;
				}
}

//	MEMBER	FUNCTION
AllocInfo*	MemMgr::getAllocInfoOf(	void*	address	)	{
				AllocInfo*	allocInfo;
				std::map<void*,	AllocInfo*>::iterator	pos;
				for	(	pos	=	allocInfoByAddressMap.begin()	;	pos	!=	allocInfoByAddressMap.end()	;	pos++	)	{
								allocInfo	=	pos->second;
								if	(	allocInfo->contains(	address	))	{
												return	allocInfo;
								}
				}
				return	NULL;
}

//	MEMBER	FUNCTION
AllocInfo*	MemMgr::getAllocInfoAt(	void*	address	)	{
				std::map<void*,	AllocInfo*>::iterator	pos;
				pos	=	allocInfoByAddressMap.find(	address	);
				if	(	pos	==	allocInfoByAddressMap.end())	{
								return	NULL;
				}	else	{
								return	pos->second;
				}
}

//	MEMBER	FUNCTION
AllocInfo*	MemMgr::getAllocInfoNamed(	const	std::string&	name	)	{
				std::map<std::string,	AllocInfo*>::iterator	pos;
				pos	=	allocInfoByNameMap.find(name);
				if	(	pos	==	allocInfoByNameMap.end())	{
								return	NULL;
				}	else	{
								return	pos->second;
				}
}


//	MEMBER	FUNCTION
const	DataType*	MemMgr::getDataType(	std::string	typeName	)	{
				return	typeDictionary->getDataType(typeName);
}

#if	NEWSTUFF
void	*	MemMgr::getVarAccessInfo(	const	std::string&	variableName,	VarAccessInfo&	varAccessInfo	)	{

				std::string	varName	=	variableName;
				//	Get	identifier	at	beginnning	of	the	string	and	the	remiander.
				//	Find	the	AllocInfoByName	(	identifier	);

				AllocInfo*	allocInfo	=	getAllocInfoNamed	(variableName);
				if	(allocInfo	!=	NULL)	{
				}
				returnAddress	=	alloc->getVarAccessInfo(	varName,	varAccessInfo	);

				return	returnAddress;
}
#endif
