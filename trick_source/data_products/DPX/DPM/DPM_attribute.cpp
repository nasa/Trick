
#include	"DPM_attribute.hh"
#include	<strings.h>
#include	<string.h>

//	COPY	CONSTRUCTOR
DPM_attribute::DPM_attribute(const	DPM_attribute	*attr)	{
		key			=	attr->key;
		value	=	attr->value;
}

//	MEMBER	FUNCTION
int	DPM_attribute::key_is	(const	char	*name)	{
		return	(strcmp(name,	this->key.c_str())	==	0);
}

//	MEMBER	FUNCTION
std::string	*DPM_attribute::value_of()	{
		return	(new	std::string(this->value));
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_attribute	*attr)	{
		s	<<	attr->key	<<	"=\""	<<	attr->value	<<	"\"";
		return	s;
}
