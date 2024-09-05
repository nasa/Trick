#include	"StringValue.hh"

StringValue::StringValue(const	char*	value)	{
				this->value	=	value;
};

void	StringValue::print(std::ostream	&s)	{
				s	<<	value;
}

std::string	StringValue::toString()	{
				return	value;
}
