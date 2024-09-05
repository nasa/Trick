#include	<iostream>
#include	<sstream>
#include	"EnumDictionary.hh"

int	EnumDictionary::getValue(std::string	name	)		{

				enumDictionaryIterator	=	enumDictionary.find(name);
				if	(enumDictionaryIterator	==	enumDictionary.end())	{
								std::stringstream	error_stream	;
								error_stream	<<	"ERROR:	Enumerator	\""	<<	name	<<	"\"	not	defined."	<<	std::endl;
								throw	std::logic_error(	error_stream.str());
				}	else	{
								return(	enumDictionaryIterator->second	);
				}
}

void	EnumDictionary::addEnumerator(std::string	name,	int	value)		{

				enumDictionaryIterator	=	enumDictionary.find(name);
				if	(enumDictionaryIterator	==	enumDictionary.end())	{
								enumDictionary[name]	=	value;
				}	else	{
								std::stringstream	error_stream	;
								error_stream	<<	"ERROR:	Attempt	to	re-define	enumerator	\""	<<	name	<<	"\"."	<<	std::endl;
								throw	std::logic_error(	error_stream.str());
				}
}

//	MEMBER	FUNCTION
std::string	EnumDictionary::toString()	{
				std::ostringstream	oss;

				for	(	enumDictionaryIterator		=	enumDictionary.begin();
										enumDictionaryIterator	!=	enumDictionary.end();
										enumDictionaryIterator++	)	{

								oss	<<	enumDictionaryIterator->first	<<	"	=	"	<<	enumDictionaryIterator->second	<<	std::endl;
				}
				return	oss.str();
}

