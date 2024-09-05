#include	"CompositeValue.hh"
#include	<sstream>

//	DESTRUCTOR
CompositeValue::~CompositeValue()	{
				int	n	=	member_list.size();
				for	(int	ii=	0	;	ii	<	n;	ii++)	{
								delete(	member_list[ii]	);
				}
}

//	MEMBER	FUNCTION
void	CompositeValue::addMember(Value*	value)	{
				member_list.push_back(value);
}

//	MEMBER	FUNCTION
int	CompositeValue::getMemberCount()	{
				return	(	(int)member_list.size()	);
}

//	MEMBER	FUNCTION
Value*&		CompositeValue::operator[]	(const	int	index)	{
				return	(	member_list[index]	);
}

//	MEMBER	FUNCTION
void		CompositeValue::print(std::ostream	&s)	{
			int	n	=	member_list.size();
				s	<<	"{";
				for	(int	ii=	0	;	ii	<	n;	ii++)	{
								if	(ii	!=	0)	{
												s	<<	",";
								}
								member_list[ii]->print(s);
				}
				s	<<	"}";
}

//	MEMBER	FUNCTION
std::string		CompositeValue::toString()	{
			std::ostringstream	oss;

			int	n	=	member_list.size();
				oss	<<	"{";
				for	(int	ii=	0	;	ii	<	n;	ii++)	{
								if	(ii	!=	0)	{
												oss	<<	",";
								}
								oss	<<	member_list[ii]->toString();
				}
				oss	<<	"}";
				return	oss.str();
}

