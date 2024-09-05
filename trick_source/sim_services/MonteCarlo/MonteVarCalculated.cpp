#include	<iostream>
#include	<sstream>

#include	"trick/MonteVarCalculated.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

Trick::MonteVarCalculated::MonteVarCalculated(std::string	in_name,	std::string	in_unit)	{
				this->name	=	in_name;
				this->unit	=	in_unit;

				ref2	=	ref_attributes(name.c_str());
				if	(ref2	==	NULL)	{
								message_publish(MSG_ERROR,	"Monte	:	MonteVarCalculated	could	not	find	parameter	%s.\n",	name.c_str())	;
				}
}

//	Composite	the	various	properties	of	this	MonteVarCalculated.
std::string	Trick::MonteVarCalculated::describe_variable()
{
				std::stringstream	ss;

				ss	<<	"#NAME:\t"	<<	this->name	<<	"\n"
							<<	"#TYPE:\tCALCULATED\n"	
							<<	"#UNIT:\t"	<<	this->unit	<<	"\n";

				return	ss.str();
}

std::string	Trick::MonteVarCalculated::get_next_value()	{
				char	buffer[128];
				if	(ref2	!=	NULL)	{
								switch	(ref2->attr->type)	{
												case	TRICK_CHARACTER:
												case	TRICK_UNSIGNED_CHARACTER:
																snprintf(buffer,	sizeof(buffer),	"%d",	*(char	*)ref2->address);
																value	=	buffer;
																break;
												case	TRICK_SHORT:
												case	TRICK_UNSIGNED_SHORT:
																snprintf(buffer,	sizeof(buffer),	"%d",	*(short	*)ref2->address);
																value	=	buffer;
																break;
												case	TRICK_INTEGER:
												case	TRICK_UNSIGNED_INTEGER:
																snprintf(buffer,	sizeof(buffer),	"%d",	*(int	*)ref2->address);
																value	=	buffer;
																break;
												case	TRICK_LONG:
												case	TRICK_UNSIGNED_LONG:
																snprintf(buffer,	sizeof(buffer),	"%ld",	*(long	*)ref2->address);
																value	=	buffer;
																break;
												case	TRICK_LONG_LONG:
												case	TRICK_UNSIGNED_LONG_LONG:
																snprintf(buffer,	sizeof(buffer),	"%lld",	*(long	long	*)ref2->address);
																value	=	buffer;
																break;
												case	TRICK_FLOAT:
																snprintf(buffer,	sizeof(buffer),	"%.15g",	*(float	*)ref2->address);
																value	=	buffer;
																break;
												case	TRICK_DOUBLE:
																snprintf(buffer,	sizeof(buffer),	"%.15g",	*(double	*)ref2->address);
																value	=	buffer;
																break;
												default:
																snprintf(buffer,	sizeof(buffer),	"#Unsupported	value	type	%d",	ref2->attr->type)	;
																break	;
								}

								if	(unit.empty())	{
												return	name	+	std::string("	=	")	+	value;
								}	else	{
												return	name	+	std::string("	=	trick.attach_units(\"")	+	unit	+	std::string("\",	")	+	value	+	std::string(")");
								}
				}
				return	NULL;
}
