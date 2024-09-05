#include	"IntegerValue.hh"
#include	"FloatingPointValue.hh"
#include	<sstream>

//	CONSTRUCTOR
IntegerValue::IntegerValue(long	long	value)	{
				this->value	=	value;
};

//	MEMBER	FUNCTION
void	IntegerValue::print(std::ostream	&s)	{
				s	<<	value;
}

//	MEMBER	FUNCTION
std::string	IntegerValue::toString()	{
				std::ostringstream	oss;
				oss	<<	value;
				return	oss.str();
}

//	MEMBER	FUNCTION
double	IntegerValue::getFloatingPointValue()	{
				return	(double)value;
}

//	MEMBER	FUNCTION
long	long	IntegerValue::getIntegerValue()	{
				return	(long	long)value;
}

