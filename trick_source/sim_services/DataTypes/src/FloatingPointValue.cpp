#include	"FloatingPointValue.hh"
#include	"IntegerValue.hh"
#include	<sstream>

//	CONSTRUCTOR
FloatingPointValue::FloatingPointValue(double	value)	{
				this->value	=	value;
};

//	MEMBER	FUNCTION
void	FloatingPointValue::print(std::ostream	&s)	{
				s	<<	value;
}

//	MEMBER	FUNCTION
std::string	FloatingPointValue::toString()	{
				std::ostringstream	oss;
				oss	<<	value;
				return	oss.str();
}

//	MEMBER	FUNCTION
double	FloatingPointValue::getFloatingPointValue()	{
				return	(double)value;
}

//	MEMBER	FUNCTION
long	long	FloatingPointValue::getIntegerValue()	{
				return	(long	long)value;
}

