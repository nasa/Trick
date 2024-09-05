
#include	<string.h>
#include	"trick/swig/PrimitiveAttributesMap.hh"

//	Global	instance.		This	isn't	a	singleton,	but	the	only	one	that	works	with	attributes_map.
Trick::PrimitiveAttributesMap	attr_map	;

Trick::PrimitiveAttributesMap	*	Trick::PrimitiveAttributesMap::attributes_map()	{
				return(&attr_map)	;
}	;

Trick::PrimitiveAttributesMap::PrimitiveAttributesMap()	{
				add_type("char",	TRICK_CHARACTER	,	sizeof(char))	;
				add_type("unsigned	char",	TRICK_UNSIGNED_CHARACTER	,	sizeof(unsigned	char))	;
				add_type("short",	TRICK_SHORT	,	sizeof(short))	;
				add_type("unsigned	short",	TRICK_UNSIGNED_SHORT	,	sizeof(unsigned	short))	;
				add_type("int",	TRICK_INTEGER	,	sizeof(int))	;
				add_type("unsigned	int",	TRICK_UNSIGNED_INTEGER	,	sizeof(unsigned	int))	;
				add_type("long",	TRICK_LONG	,	sizeof(long))	;
				add_type("unsigned	long",	TRICK_UNSIGNED_LONG	,	sizeof(unsigned	long))	;
				add_type("long	long",	TRICK_LONG_LONG	,	sizeof(long	long))	;
				add_type("unsigned	long	long",	TRICK_UNSIGNED_LONG_LONG	,	sizeof(unsigned	long	long))	;
				add_type("bool",	TRICK_BOOLEAN	,	sizeof(bool))	;
				add_type("double",	TRICK_DOUBLE	,	sizeof(double))	;
				add_type("float",	TRICK_FLOAT	,	sizeof(float))	;
}

Trick::PrimitiveAttributesMap::~PrimitiveAttributesMap()	{
				std::map<std::string,	ATTRIBUTES	*	>::iterator	pit	;
				for	(	pit	=	param_attr.begin()	;	pit	!=	param_attr.end()	;	++pit	)	{
								delete(pit->second)	;
				}
				param_attr.clear()	;
}

int	Trick::PrimitiveAttributesMap::add_type(std::string	param	,	TRICK_TYPE	in_type,	int	in_size)	{
				ATTRIBUTES	*	new_attr	;
				new_attr	=	new	ATTRIBUTES()	;
				new_attr->type	=	in_type	;
				new_attr->size	=	in_size	;
				new_attr->io	=	TRICK_VAR_OUTPUT	|	TRICK_VAR_INPUT	|	TRICK_CHKPNT_OUTPUT	|	TRICK_CHKPNT_INPUT	;
				param_attr[param]	=	new_attr	;
				return	0	;
}

ATTRIBUTES	*	Trick::PrimitiveAttributesMap::get_attr(	std::string	param	)	{
				if	(	param_attr.find(param)	!=	param_attr.end()	)	{
								ATTRIBUTES	*	new_attr	=	new	ATTRIBUTES()	;
								*new_attr	=	*(param_attr[param])	;
								return	new_attr	;
								//return	param_attr[param]	;
				}	else	{
								return	NULL	;
				}
}
