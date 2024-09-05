#include	"trick/MemoryManager.hh"
#include	<sstream>

int	Trick::MemoryManager::get_type_attributes(	TRICK_TYPE&	type,
																																															std::string	user_type_name,
																																															int									n_stars,
																																															ATTRIBUTES*&	sub_attr,
																																															int&								size)	{

				sub_attr	=	NULL;

				switch	(type)	{

								case	TRICK_CHARACTER:	{
											size	=	(n_stars	==	0)	?	sizeof(char)	:	sizeof(void*)	;
								}	break;
								case	TRICK_SHORT:	{
											size	=	(n_stars	==	0)	?	sizeof(short)	:	sizeof(void*)	;
								}	break;
								case	TRICK_INTEGER:	{
											size	=	(n_stars	==	0)	?	sizeof(int)	:	sizeof(void*)	;
								}	break;
								case	TRICK_LONG:	{
											size	=	(n_stars	==	0)	?	sizeof(long)	:	sizeof(void*)	;
								}	break;
								case	TRICK_FLOAT:	{
											size	=	(n_stars	==	0)	?	sizeof(float)	:	sizeof(void*)	;
								}	break;
								case	TRICK_DOUBLE:	{
											size	=	(n_stars	==	0)	?	sizeof(double)	:	sizeof(void*)	;
								}	break;
								case	TRICK_BOOLEAN:	{
											size	=	(n_stars	==	0)	?	sizeof(bool)	:	sizeof(void*)	;
								}	break;
								case	TRICK_LONG_LONG:	{
											size	=	(n_stars	==	0)	?	sizeof(long	long)	:	sizeof(void*)	;
								}	break;
								case	TRICK_WCHAR:	{
											size	=	(n_stars	==	0)	?	sizeof(wchar_t)	:	sizeof(void*)	;
								}	break;
								case	TRICK_WSTRING:	{
											size	=	sizeof(void*)	;
								}	break;
								case	TRICK_UNSIGNED_CHARACTER:	{
											size	=	(n_stars	==	0)	?	sizeof(unsigned	char)	:	sizeof(void*)	;
								}	break;
								case	TRICK_UNSIGNED_SHORT:	{
											size	=	(n_stars	==	0)	?	sizeof(unsigned	short)	:	sizeof(void*)	;
								}	break;
								case	TRICK_UNSIGNED_INTEGER:	{
											size	=	(n_stars	==	0)	?	sizeof(unsigned	int)	:	sizeof(void*)	;
								}	break;
								case	TRICK_UNSIGNED_LONG:	{
											size	=	(n_stars	==	0)	?	sizeof(unsigned	long)	:	sizeof(void*)	;
								}	break;
								case	TRICK_UNSIGNED_LONG_LONG:	{
											size	=	(n_stars	==	0)	?	sizeof(unsigned	long	long)	:	sizeof(void*)	;
								}	break;
								case	TRICK_STRING:	{
											size	=	(n_stars	==	0)	?	sizeof(std::string)	:	sizeof(void*)	;
								}	break;
								case	TRICK_OPAQUE_TYPE:
								case	TRICK_ENUMERATED:
								case	TRICK_STRUCTURED:	{

												if	(n_stars	!=	0)	{
																size	=	sizeof(void*);
												}	else	{
																if	((size	=	io_src_sizeof_user_type(	user_type_name.c_str()))	==	0)	{
																				std::stringstream	message;
																				message	<<	"size	of	type	\""	<<	user_type_name.c_str()	<<"\"	not	found.\n";
																				emitError(message.str());
																				return	(1);
																}
												}

												//	Find	the	attribute	info	for	<user_type_name>	and	put	it	in	temp_attr.
												ATTRIBUTES	temp_attr;
												int	status	=	add_attr_info(	user_type_name,	&temp_attr);

												if	(	status	==	0	)	{
																type					=	temp_attr.type;
																sub_attr	=	(ATTRIBUTES*)(temp_attr.attr);
												}	else	{
																return	(1);
												}
								}	break;

								default:	{
												std::stringstream	message;
												message	<<	"Unknown	data-type	\""	<<	type	<<	"\"."	;
												emitError(message.str());
												return	(1);
								}	break;
				}
				return	(0);
}
