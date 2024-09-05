#include	<cerrno>
#include	<cstring>
#include	<iostream>

#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<map>
#include	"TrickBinary.hh"
#include	"trick/parameter_types.h"
#include	"trick_byte_order.h"
#include	"trick_byteswap.h"
#include	"trick/units_conv.h"
#include	"trick/map_trick_units_to_udunits.hh"

TrickBinary::TrickBinary(char	*	file_name	,	char	*	param_name	)	{

								const	int	file_type_len	=	10	;
								char	file_type[file_type_len	+	1]	;
								int	my_byte_order	;
								char	*name_ptr	;
								char	*units_ptr	;
								int	len	;
								int	ii	;
								int	type	;
								int	size	;
								std::map<int,	TRICK_TYPE>	seven_to_ten_params		;

								seven_to_ten_params[0]	=	TRICK_CHARACTER	;
								seven_to_ten_params[1]	=	TRICK_UNSIGNED_CHARACTER	;
								seven_to_ten_params[2]	=	TRICK_STRING	;
								seven_to_ten_params[3]	=	TRICK_SHORT	;
								seven_to_ten_params[4]	=	TRICK_UNSIGNED_SHORT	;
								seven_to_ten_params[5]	=	TRICK_INTEGER	;
								seven_to_ten_params[6]	=	TRICK_UNSIGNED_INTEGER	;
								seven_to_ten_params[7]	=	TRICK_LONG	;
								seven_to_ten_params[8]	=	TRICK_UNSIGNED_LONG	;
								seven_to_ten_params[9]	=	TRICK_FLOAT	;
								seven_to_ten_params[10]	=	TRICK_DOUBLE	;
								seven_to_ten_params[11]	=	TRICK_BITFIELD	;
								seven_to_ten_params[12]	=	TRICK_UNSIGNED_BITFIELD	;
								seven_to_ten_params[13]	=	TRICK_LONG_LONG	;
								seven_to_ten_params[14]	=	TRICK_UNSIGNED_LONG_LONG	;
								seven_to_ten_params[15]	=	TRICK_FILE_PTR	;
								seven_to_ten_params[16]	=	TRICK_VOID	;
								seven_to_ten_params[17]	=	TRICK_BOOLEAN	;
								//	18	=	TRICK_COMPLX	,	19	=	TRICK_DBL_COMPLX	,	20	=	TRICK_REF.	These	don't	exist	in	10
								seven_to_ten_params[21]	=		TRICK_WCHAR	;
								seven_to_ten_params[22]	=		TRICK_WSTRING	;
								seven_to_ten_params[99]	=	TRICK_VOID_PTR	;
								seven_to_ten_params[102]	=	TRICK_ENUMERATED	;
								seven_to_ten_params[103]	=	TRICK_STRUCTURED	;

								fileName_	=	file_name	;

								if	((fp_	=	fopen(file_name	,	"r"))	!=	0	)	{
																memset(file_type,	0	,	10	)	;
																fread(file_type	,	file_type_len	,	1	,	fp_	)	;
																file_type[file_type_len]	=	'\0'	;

																if	(	!strncmp(	file_type	,	"Trick-05"	,	8	)	||
																					!strncmp(	file_type	,	"Trick-07"	,	8	)	||
																					!strncmp(	file_type	,	"Trick-10"	,	8	)	)	{

																								TRICK_GET_BYTE_ORDER(my_byte_order)	;
																								switch	(	file_type[file_type_len	-	1]	)	{
																												case	'L':
																																				swap_	=	(	my_byte_order	==	TRICK_LITTLE_ENDIAN	)	?	0	:	1	;
																																				break	;
																												case	'B':
																																				swap_	=	(	my_byte_order	==	TRICK_BIG_ENDIAN	)	?	0	:	1	;
																																				break	;
																								}

																								//	num_params
																								fread(&num_params_	,	4	,	1	,	fp_	)	;
																								if	(	swap_	)	{	num_params_	=	trick_byteswap_int(num_params_)	;	}

																								record_size_	=	0	;
																								for	(	ii	=	0		;	ii	<	num_params_	;	ii++	)	{

																																//	name	length
																																fread(&len	,	4	,	1	,	fp_	)	;
																																if	(	swap_	)	{	len	=	trick_byteswap_int(len)	;	}

																																//	name
																																name_ptr	=	new	char[len	+	1]	;
																																fread(name_ptr	,	len	,	1	,	fp_	)	;
																																name_ptr[len]	=	'\0'	;

																																//	units	length
																																fread(&len	,	4	,	1	,	fp_	)	;
																																if	(	swap_	)	{	len	=	trick_byteswap_int(len)	;	}

																																//	units
																																units_ptr	=	new	char[len	+	1]	;
																																fread(units_ptr	,	len	,	1	,	fp_	)	;
																																units_ptr[len]	=	'\0'	;

																																//	If	this	is	an	05	log	file,	we	need	to	convert	the	units	to	07	units
																																//	(	where	explicit	asterisk	for	multiplication	is	required.	)
																																if	(	!strncmp(	file_type	,	"Trick-05"	,	8	)	)		{
																																								char	new_units_spec[100];
																																								new_units_spec[0]	=	0;
																																								if	(	convert_units_spec	(units_ptr,	new_units_spec)	!=	0	)	{
																																																printf	("	ERROR:	Attempt	to	convert	Trick-05	units	spec	\"%s\"	failed.\n\n",units_ptr);
																																								}
																																								delete	[]	units_ptr;
																																								len	=	strlen(new_units_spec);
																																								units_ptr	=	new	char[len	+	1]	;
																																								strcpy(units_ptr,	new_units_spec);
																																}

																																//	type	of	param
																																fread(&type	,	4	,	1	,	fp_	)	;
																																if	(	swap_	)	{	type	=	trick_byteswap_int(type)	;	}
																																//	adjust	the	recorded	types	for	05	&	07	because	they	are	1	less	than	Trick10	types	(because	of	Penn!)
																																if	(	strncmp(	file_type	,	"Trick-10"	,	8	)	)		{
																																				type	=	(int)seven_to_ten_params[type]	;
																																}

																																//	size	of	param
																																fread(&size	,	4	,	1	,	fp_	)	;
																																if	(	swap_	)	{	size	=	trick_byteswap_int(size)	;	}

																																//	correct	they	"type"	according	to	the	size	recorded
																																switch	(	type	)	{
																																				case	TRICK_LONG:
																																								switch		(	size	)	{
																																												case	4:
																																																type	=	TRICK_INTEGER	;
																																																break	;
																																												case	8:
																																																type	=	TRICK_LONG_LONG	;
																																																break	;
																																												default:
																																																break	;
																																								}
																																				case	TRICK_UNSIGNED_LONG:
																																								switch		(	size	)	{
																																												case	4:
																																																type	=	TRICK_UNSIGNED_INTEGER	;
																																																break	;
																																												case	8:
																																																type	=	TRICK_UNSIGNED_LONG_LONG	;
																																																break	;
																																												default:
																																																break	;
																																								}
																																				default:
																																								break	;
																																}

																																if	(	!	strcmp(	name_ptr	,	"sys.exec.out.time"	))	{
																																								time_size_		=	size	;
																																								unitTimeStr_	=	units_ptr	;
																																}

																																if	(	!	strcmp(	name_ptr	,	param_name	))	{
																																								if	(	!strcmp(units_ptr,"--")	)	{
																																												unitStr_	=	units_ptr	;
																																								}	else	{
																																												unitStr_	=	map_trick_units_to_udunits(units_ptr)	;
																																								}
																																								record_offset_	=		record_size_	;
																																								type_	=		type	;
																																								size_	=		size	;
																																}

																																record_size_	+=	size	;

																																delete[]name_ptr	;
																																delete[]units_ptr	;
																								}

																								data_offset_	=	ftell(fp_)	;
																}

																record_	=	new	char[record_size_]	;
								}
								else	{
												std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
								}
}

TrickBinary::~TrickBinary()
{
								if	(	fp_	)	{
																fclose(fp_);
								}
}

int	TrickBinary::get(	double	*	time	,	double	*	value	)	{

								char	*	cp	;
								unsigned	char	*	ucp	;
								short	*	sp	;
								unsigned	short	*	usp	;
								int	*	ip	;
								unsigned	int	*	uip	;
								long	*	lp	;
								unsigned	long	*	ulp	;
								float	*	fp	;
								double	*	dp	;
								long	long	*	llp	;
								unsigned	long	long	*	ullp	;

								if	(	fread(record_	,	record_size_	,	1	,	fp_	))	{

																if	(	time_size_	==	8	)	{
																								double	*	my_time	=	(double	*)record_	;
																								*time	=	*my_time	;
																								*time	=	swap_	?	trick_byteswap_double(*my_time)	:	*time	;
																}
																else	{
																								float	*my_time	=	(float	*)record_	;
																								*time	=	(double)*my_time	;
																								*time	=	swap_	?	trick_byteswap_double(*my_time)	:	*time	;
																}

																switch	(	type_	)	{
																								case	TRICK_CHARACTER:
																																cp	=	(char	*)(record_	+	record_offset_)	;
																																*value	=	(double)*cp	;
																																break	;
																								case	TRICK_UNSIGNED_CHARACTER:
																																ucp	=	(unsigned	char	*)(record_	+	record_offset_)	;
																																*value	=	(double)*ucp	;
																																break	;
																								case	TRICK_SHORT:
																																sp	=	(short	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	trick_byteswap_short(*sp)	:	*sp)	;
																																break	;
																								case	TRICK_UNSIGNED_SHORT:
																																usp	=	(unsigned	short	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	trick_byteswap_short(*usp)	:	*usp)	;
																																break	;
																								case	TRICK_ENUMERATED:
																								case	TRICK_INTEGER:
																																ip	=	(int	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	trick_byteswap_int(*ip)	:	*ip)	;
																																break	;
																								case	TRICK_UNSIGNED_INTEGER:
																																uip	=	(unsigned	int	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	(unsigned	int)trick_byteswap_int(*uip)	:	*uip)	;
																																break	;
																								case	TRICK_LONG:
																																lp	=	(long	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	trick_byteswap_long(*lp)	:	*lp)	;
																																break	;
																								case	TRICK_UNSIGNED_LONG:
																																ulp	=	(unsigned	long	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	(unsigned	long)trick_byteswap_long(*ulp)	:	*ulp)	;
																																break	;
																								case	TRICK_FLOAT:
																																fp	=	(float	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	trick_byteswap_float(*fp)	:	*fp)	;
																																break	;
																								case	TRICK_DOUBLE:
																																dp	=	(double	*)(record_	+	record_offset_)	;
																																*value	=	swap_	?	trick_byteswap_double(*dp)	:	*dp	;
																																break	;
																								case	TRICK_BITFIELD:
																																switch	(	size_	)	{
																																								case	1	:
																																																cp	=	(char	*)(record_	+	record_offset_)	;
																																																*value	=	(double)*cp	;
																																																break	;
																																								case	2	:
																																																sp	=	(short	*)(record_	+	record_offset_)	;
																																																*value	=	(double)(swap_	?	trick_byteswap_short(*sp)	:	*sp)	;
																																																break	;
																																								case	4	:
																																																ip	=	(int	*)(record_	+	record_offset_)	;
																																																*value	=	(double)(swap_	?	trick_byteswap_int(*ip)	:	*ip)	;
																																																break	;
																																}
																																break	;
																								case	TRICK_UNSIGNED_BITFIELD:
																																switch	(	size_	)	{
																																								case	1	:
																																																ucp	=	(unsigned	char	*)(record_	+	record_offset_)	;
																																																*value	=	(double)*ucp	;
																																																break	;
																																								case	2	:
																																																usp	=	(unsigned	short	*)(record_	+	record_offset_)	;
																																																*value	=	(double)(swap_	?	(unsigned	short)trick_byteswap_short(*usp)	:	*usp)	;
																																																break	;
																																								case	4	:
																																																uip	=	(unsigned	int	*)(record_	+	record_offset_)	;
																																																*value	=	(double)(swap_	?	(unsigned	int)trick_byteswap_int(*uip)	:	*uip)	;
																																																break	;
																																}
																																break	;
																								case	TRICK_LONG_LONG:
																																llp	=	(long	long	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	trick_byteswap_long_long(*llp)	:	*llp)	;
																																break	;
																								case	TRICK_UNSIGNED_LONG_LONG:
																																ullp	=	(unsigned	long	long	*)(record_	+	record_offset_)	;
																																*value	=	(double)(swap_	?	(unsigned	long	long)trick_byteswap_long_long(*ullp)	:	*ullp)	;
																																break	;
																								case	TRICK_BOOLEAN:
																																switch	(	size_	)	{
																																								case	1	:
																																																ucp	=	(unsigned	char	*)(record_	+	record_offset_)	;
																																																*value	=	(double)*ucp	;
																																																break	;
																																								case	4	:
																																																ip	=	(int	*)(record_	+	record_offset_)	;
																																																*value	=	(double)(swap_	?	trick_byteswap_int(*ip)	:	*ip)	;
																																																break	;
																																}
																																break	;
																}
																return(1)	;
								}

								return(0)	;

}

int	TrickBinary::peek(	double	*	time	,	double	*	value	)	{

								long	offset	;
		int	ret	;

								offset	=	ftell(fp_)	;
								ret	=	get(	time	,	value	)	;
								fseek(fp_	,	offset	,	SEEK_SET	)	;

								return(ret)	;
}

void	TrickBinary::begin()	{
								fseek(fp_,	data_offset_	,	SEEK_SET)	;
								return	;
}

int	TrickBinary::end()	{

								long	offset	;
								long	endOffset	;

								offset	=	ftell(fp_)	;
								fseek(fp_,	0,	SEEK_END)	;
								endOffset	=	ftell(fp_)	;
								fseek(fp_	,	offset	,	SEEK_SET	)	;

								if	(	endOffset	-	offset	==	0	)	{
																//	Sitting	past	the	last	data	point
																return(1);
								}

								return(feof(fp_))	;
}

int	TrickBinary::step()	{

								char	*	record	;
								record	=	new	char[record_size_]	;

								if	(	fread(record	,	record_size_	,	1	,	fp_	))	{
																delete[]record	;
																return(1)	;
								}

								delete[]record	;
								return(0)	;
}

int	TrickBinaryReadByteOrder(	FILE*	fp	)	{

								const	int	file_type_len	=	10	;
								char	file_type[file_type_len	+	1]	;
								int	my_byte_order	;
								int	swap	=	0;

								memset(file_type,	0	,	10	)	;
								fread(file_type	,	file_type_len	,	1	,	fp	)	;
								file_type[file_type_len]	=	'\0'	;

								if	(	!strncmp(	file_type	,	"Trick-05"	,	8	)	||
													!strncmp(	file_type	,	"Trick-07"	,	8	)	||
													!strncmp(	file_type	,	"Trick-10"	,	8)	)	{

																TRICK_GET_BYTE_ORDER(my_byte_order)	;
																switch	(	file_type[file_type_len	-	1]	)	{
																				case	'L':
																												swap	=	(	my_byte_order	==	TRICK_LITTLE_ENDIAN	)	?	0	:	1	;
																												break	;
																				case	'B':
																												swap	=	(	my_byte_order	==	TRICK_BIG_ENDIAN	)	?	0	:	1	;
																												break	;
																}
								}	else	{
																return	-1	;
								}

								return	swap	;
}

int	TrickBinaryGetNumVariables(const	char*	file_name)	{

								int	num_params	;
								FILE	*fp	;
								int	swap	;

								if	((fp	=	fopen(file_name	,	"r"))	!=	0	)	{
																swap	=	TrickBinaryReadByteOrder(	fp	)	;
																fread(&num_params	,	4	,	1	,	fp	)	;
																if	(	swap	)	{	num_params	=	trick_byteswap_int(num_params)	;	}
								}	else	{
																std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
																return(0)	;
								}

								fclose(fp);
								return	num_params	;
}

char**	TrickBinaryGetVariableNames(const	char*	file_name)	{

								int	swap	;
								char	*name_ptr	;
								char	*units_ptr	;
								int	len	;
								int	ii	;
								int	type	;
								int	size	;
								int	num_params	;
								FILE	*fp	;

								char**	variable_names	=	0	;

								if	((fp	=	fopen(file_name	,	"r"))	!=	0	)	{

																	swap	=	TrickBinaryReadByteOrder(	fp	)	;

																	//	num_params
																	fread(&num_params	,	4	,	1	,	fp	)	;
																	if	(	swap	)	{	num_params	=	trick_byteswap_int(num_params)	;	}

																	//	Alloc	space	for	return	list
																	variable_names	=	new	char*[num_params]	;

																	for	(	ii	=	0		;	ii	<	num_params	;	ii++	)	{

																									//	name	length
																									fread(&len	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	len	=	trick_byteswap_int(len)	;	}

																									//	name
																									variable_names[ii]	=	new	char[len	+	1]	;
																									name_ptr	=	variable_names[ii]	;
																									fread(name_ptr	,	len	,	1	,	fp	)	;
																									name_ptr[len]	=	'\0'	;

																									//	units	length
																									fread(&len	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	len	=	trick_byteswap_int(len)	;	}

																									//	units
																									units_ptr	=	new	char[len	+	1]	;
																									fread(units_ptr	,	len	,	1	,	fp	)	;
																									units_ptr[len]	=	'\0'	;

																									//	type	of	param
																									fread(&type	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	type	=	trick_byteswap_int(type)	;	}

																									//	size	of	param
																									fread(&size	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	size	=	trick_byteswap_int(size)	;	}

																									delete[]units_ptr	;
																	}
								}
								else	{
																std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
																return(0)	;
								}

								fclose(fp)	;

								return(	variable_names	)	;
}

char**	TrickBinaryGetVariableUnits(const	char*	file_name)	{

								int	swap	;
								char	*name_ptr	;
								char	*units_ptr	;
								int	len	;
								int	ii	;
								int	type	;
								int	size	;
								int	num_params	;
								FILE	*fp	;

								char**	variable_units	=	0	;

								if	((fp	=	fopen(file_name	,	"r"))	!=	0	)	{

																	swap	=	TrickBinaryReadByteOrder(	fp	)	;

																	//	num_params
																	fread(&num_params	,	4	,	1	,	fp	)	;
																	if	(	swap	)	{	num_params	=	trick_byteswap_int(num_params)	;	}

																	//	Alloc	space	for	return	list
																	variable_units	=	new	char*[num_params]	;

																	for	(	ii	=	0		;	ii	<	num_params	;	ii++	)	{

																									//	name	length
																									fread(&len	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	len	=	trick_byteswap_int(len)	;	}

																									//	name
																									name_ptr	=	new	char[len	+	1]	;
																									fread(name_ptr	,	len	,	1	,	fp	)	;
																									name_ptr[len]	=	'\0'	;

																									//	units	length
																									fread(&len	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	len	=	trick_byteswap_int(len)	;	}

																									//	units
																									variable_units[ii]	=	new	char[len	+	1]	;
																									units_ptr	=	variable_units[ii]	;
																									fread(units_ptr	,	len	,	1	,	fp	)	;
																									units_ptr[len]	=	'\0'	;

																									//	type	of	param
																									fread(&type	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	type	=	trick_byteswap_int(type)	;	}

																									//	size	of	param
																									fread(&size	,	4	,	1	,	fp	)	;
																									if	(	swap	)	{	size	=	trick_byteswap_int(size)	;	}

																									delete[]name_ptr	;
																	}
								}
								else	{
																std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
																return(0)	;
								}

								fclose(fp)	;

								return(	variable_units	)	;
}

int	TrickBinaryLocateParam(	const	char	*	file_name	,	const	char	*	param_name	)	{

								int	found	;
								int	num_vars	;
								int	ii	;
								char**	var_names	;

								num_vars	=	TrickBinaryGetNumVariables(	file_name)	;
								if	(	num_vars	==	0	)	{
																return	0	;
								}

								var_names	=	TrickBinaryGetVariableNames(file_name)	;
								if	(	var_names	==	0	)	{
																return	0	;
								}

								found	=	0	;
								for	(	ii	=	0	;	ii	<	num_vars	;	ii++	)	{
																if	(	!strcmp(	var_names[ii],	param_name)	)	{
																								found	=	1	;
																								break	;
																}
								}

								for	(	ii	=	0	;	ii	<	num_vars	;	ii++	)	{
																delete[]	var_names[ii]	;
								}
								delete[]	var_names;
								return(found)	;
}
