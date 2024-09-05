#include	<sstream>
#include	<iomanip>

#include	"trick/MonteVarFixed.hh"

Trick::MonteVarFixed::MonteVarFixed(std::string	in_name,	double	in_value,	std::string	in_unit)	{
				this->name	=	in_name;
				set_value(in_value);
				this->unit	=	in_unit;
}

//	Composite	the	various	properties	of	this	MonteVarFixed.
std::string	Trick::MonteVarFixed::describe_variable()
{
				std::stringstream	ss;

				ss	<<	"#NAME:\t\t"	<<	this->name	<<	"\n"
							<<	"#TYPE:\t\tFIXED\n"	
							<<	"#UNIT:\t\t"	<<	this->unit	<<	"\n"
							<<	"#VALUE:\t\t"	<<	this->value	<<	"\n";

				return	ss.str();
}

void	Trick::MonteVarFixed::set_value(double	in_value)	{
				std::ostringstream	string_stream;
				string_stream	<<	std::setprecision(15)	<<	in_value	;
				this->value	=	string_stream.str();
}

std::string	Trick::MonteVarFixed::get_next_value()	{
				std::ostringstream	string_stream;
				if	(unit.empty())	{
								string_stream	<<	name	<<	"	=	"	<<	value;
				}
				else	{
								string_stream	<<	name	<<	"	=	trick.attach_units(\""	<<	unit	<<	"\",	"	<<	value	<<	")";
				}
				return	string_stream.str();
}
