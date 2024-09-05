#include	"DataType.hh"
#include	"TypeDictionary.hh"
#include	"PrimitiveDataType.hh"
#include	<sstream>
#include	<iostream>

//	MEMBER	FUNCTION
TypeDictionary::TypeDictionary()	{

				//	Add	builtin	types.
				addTypeDefinition("void",	new	PrimitiveDataType<void>()	);
				addTypeDefinition("char",	new	PrimitiveDataType<char>()	);
				addTypeDefinition("short",	new	PrimitiveDataType<short>()	);
				addTypeDefinition("int",	new	PrimitiveDataType<int>()	);
				addTypeDefinition("long",	new	PrimitiveDataType<long>()	);
				addTypeDefinition("wchar_t",	new	PrimitiveDataType<wchar_t>()	);
				addTypeDefinition("long	long",	new	PrimitiveDataType<long	long>()	);
				addTypeDefinition("unsigned	char",	new	PrimitiveDataType<unsigned	char>()	);
				addTypeDefinition("unsigned	short",	new	PrimitiveDataType<unsigned	short>()	);
				addTypeDefinition("unsigned	int",	new	PrimitiveDataType<unsigned	int>()	);
				addTypeDefinition("unsigned	long",	new	PrimitiveDataType<unsigned	long>()	);
				addTypeDefinition("unsigned	long	long",	new	PrimitiveDataType<unsigned	long	long>()	);
				addTypeDefinition("float",	new	PrimitiveDataType<float>()	);
				addTypeDefinition("double",	new	PrimitiveDataType<double>()	);
				//	FIXME:	add	the	other	goofy	types	(e.g.,	uint8	)	that	were	added	via	the	old	lexical	analyzer.

}

//	MEMBER	FUNCTION
const	DataType*	TypeDictionary::getDataType(std::string	name	)	{

				typeDictionaryIterator	=	typeDictionary.find(name);
				if	(typeDictionaryIterator	==	typeDictionary.end())	{
								return	NULL;
				}	else	{
								return(	typeDictionaryIterator->second	);
				}
}

//	MEMBER	FUNCTION
void	TypeDictionary::addTypeDefinition(std::string	name,	DataType	*	typeSpec)		{

				const	DataType*	preExistingDataType	=	getDataType(name);

				if	(	preExistingDataType	==	NULL	)	{
								typeDictionary[name]	=	typeSpec;
				}	else	{
								std::stringstream	error_stream	;
								error_stream	<<	"ERROR:	Attempt	to	re-define	type	\""	<<	name	<<	"\""	<<	std::endl;
								throw	std::logic_error(	error_stream.str());
				}
}

//	MEMBER	FUNCTION
bool	TypeDictionary::validate()	{

				is_valid	=	true;
				for	(	typeDictionaryIterator		=	typeDictionary.begin();
										typeDictionaryIterator	!=	typeDictionary.end();
										typeDictionaryIterator++	)	{

									DataType*	dataType	=	typeDictionaryIterator->second;
									is_valid	=	is_valid	&&	dataType->validate();
				}
				if	(!is_valid)	{
								std::cerr	<<	"TypeDictionary	contains	invalidate	data-types.";
				}
				return	is_valid;
}

//	MEMBER	FUNCTION
std::string	TypeDictionary::toString()	{
				std::ostringstream	oss;

				for	(	typeDictionaryIterator		=	typeDictionary.begin();
										typeDictionaryIterator	!=	typeDictionary.end();
										typeDictionaryIterator++	)	{

								oss	<<	typeDictionaryIterator->first	<<	"	=	"	<<	typeDictionaryIterator->second	<<	std::endl;
				}

				return	oss.str();
}

//	MEMBER	FUNCTION
TypeDictionary::~TypeDictionary()	{

				//	Delete	all	DataTypes	in	the	map.
				for	(	typeDictionaryIterator		=	typeDictionary.begin();
										typeDictionaryIterator	!=	typeDictionary.end();
										typeDictionaryIterator++	)	{

								delete	typeDictionaryIterator->second;
				}
}








