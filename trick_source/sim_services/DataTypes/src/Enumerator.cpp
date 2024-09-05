#include	"Enumerator.hh"
#include	"Enumerator.hh"
#include	<sstream>

Enumerator::Enumerator(	std::string	name,	int	value)	{
				this->name	=	name;
				this->value	=	value;
}

Enumerator::Enumerator(	const	Enumerator&	original)	{
				this->name	=	original.name;
				this->value	=	original.value;
}

std::string	Enumerator::getName()	{
				return	name;
}

int	Enumerator::getValue()	{
				return	value;
}

//	MEMBER	FUNCTION
std::string	Enumerator::toString()	{
				std::ostringstream	oss;
				oss	<<	getName();
				oss	<<	"	=	";
				oss	<<	getValue();
				return	oss.str();
}

