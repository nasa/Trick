
#include	<iostream>

#include	"EnumValues.hh"

EnumValues::EnumValues()	:	has_definition(true)	{}

void	EnumValues::addEnum(	std::string	in_name	,	long	long	in_val	)	{
				std::pair<	std::string	,	long	long	>	new_enum(in_name,	in_val)	;
				enum_values.push_back(new_enum)	;
}

void	EnumValues::addFullyQualifiedEnum(	std::string	in_name	,	long	long	in_val	)	{
				std::pair<	std::string	,	long	long	>	new_enum(in_name,	in_val)	;
				fully_qualified_enum_values.push_back(new_enum)	;
}


void	EnumValues::setHasDefinition(	bool	in	)	{
				has_definition	=	in	;
}

bool	EnumValues::getHasDefinition()	{
				return	has_definition	;
}

std::ostream	&	operator	<<	(std::ostream	&	ostream	,	EnumValues	&	ev	)	{
				ostream	<<	"				name	=	"	<<	ev.name	<<	std::endl	;
				ostream	<<	"				file_name	=	"	<<	ev.file_name	<<	std::endl	;
				ostream	<<	"				namespaces	="	;
				ev.printNamespaces(ostream)	;
				ostream	<<	std::endl	;
				ostream	<<	"				parent	classes	="	;
				ev.printContainerClasses(ostream)	;
				ostream	<<	std::endl	;

				for	(auto&	pair	:	ev.getPairs())	{
								ostream	<<	"						"	<<	pair.first	<<	"	"	<<	pair.second	<<	std::endl	;
				}

				return	ostream	;
}
