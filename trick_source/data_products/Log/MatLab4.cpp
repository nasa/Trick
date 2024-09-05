#include	<cerrno>
#include	<cstring>
#include	<iostream>

#include	<stdlib.h>
#include	<string.h>
#if	__sgi	|	__sun
#include	<strings.h>
#endif

#include	<math.h>
#include	"MatLab4.hh"
#include	"trick/parameter_types.h"
#include	"trick_byteswap.h"
#include	"trick_byte_order.h"

MatLab4::MatLab4(char	*	file_name	,	char	*	param_name	,	char	*	time_name	)	{

								int	len	;
								int	temp	;
								int	endian	;
								int	mat_size	;
								int	type	=	0,	size	=	0;
								int	mat_type	;
								int	my_byte_order	;
								div_t	div_result	;
								int	row	,	column	,	imaginary	;
								char	*	temp_ptr	;
								bool	param_found	,	time_found	;
								int	max_offset	,	offset	=	0	;

								fileName_	=	file_name	;
								TRICK_GET_BYTE_ORDER(my_byte_order)	;

								if	((fp_	=	fopen(file_name	,	"r"))	==	0	)	{
											std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
											exit(-1)	;
								}

								//	we	use	fseek	calls	to	move	around	so	I	need	to	know	the	max	offset	of	the	file
								fseek(	fp_	,	0	,	SEEK_END	)	;
								max_offset	=	ftell(fp_)	;
								rewind(	fp_	)	;

								//	get	field_num	from	param_name
								temp_ptr	=	rindex(param_name,	'[')	;
								if	(	temp_ptr	!=	NULL	)	{
																sscanf(&temp_ptr[1]	,	"%d"	,	&y_value_.field_num_	)	;
								}
								else	{
																y_value_.field_num_	=	0	;
								}

								//	get	field_num	from	time_name
								temp_ptr	=	rindex(param_name,	'[')	;
								if	(	temp_ptr	!=	NULL	)	{
																sscanf(&temp_ptr[1]	,	"%d"	,	&time_value_.field_num_	)	;
								}
								else	{
																time_value_.field_num_	=	0	;
								}

								param_found	=	time_found	=	false	;
								while	(	(!param_found	||	!time_found)	&&	(offset	<	max_offset))	{

																//	read	the	type	in
																fread(	&temp	,	4	,	1	,	fp_	)	;
																if	(	!	strncmp(	(char	*)&temp	,	"MATL"	,	4	))	{
																								fclose(fp_)	;
																								return	;
																}
																//	byte	swap	it	if	it	looks	like	we	need	to
																if	(	temp	&	0xFFFF0000	)	{
																								temp	=	trick_byteswap_int(temp)	;
																}

																div_result	=	div(temp	,	1000)	;
																endian	=	div_result.quot	;
																temp	=	div_result.rem	;

																switch	(	endian	)	{
																								case	0:
																																swap_	=	(	my_byte_order	==	TRICK_LITTLE_ENDIAN	)	?	0	:	1	;
																																break	;
																								case	1:
																																swap_	=	(	my_byte_order	==	TRICK_BIG_ENDIAN	)	?	0	:	1	;
																																break	;
																								default:
																																fprintf(stderr,"Unknown	endianness!\n")	;
																																fclose(fp_)	;
																																return	;
																																break	;
																}

																div_result	=	div(	temp	,	10	)	;
																mat_size	=	div_result.quot	;
																mat_type	=	div_result.rem	;
																switch	(	mat_size	)	{
																								case	0:
																																type	=	TRICK_DOUBLE	;
																																size	=	8	;
																																break	;
																								case	1:
																																type	=	TRICK_FLOAT	;
																																size	=	4	;
																																break	;
																								case	2:
																																type	=	TRICK_INTEGER	;
																																size	=	4	;
																																break	;
																								case	3:
																																type	=	TRICK_SHORT	;
																																size	=	2	;
																																break	;
																								case	4:
																																type	=	TRICK_UNSIGNED_SHORT	;
																																size	=	2	;
																																break	;
																								case	5:
																																type	=	TRICK_CHARACTER	;
																																size	=	1	;
																																break	;
																}

																if	(	mat_type	!=	0	)	{
																								fprintf(stderr,"Unknown	matrix	type!\n")	;
																								fclose(fp_)	;
																								return	;
																}

																fread(	&row	,	4	,	1	,	fp_	)	;
																if	(	swap_	)	{	row	=	trick_byteswap_int(row)	;	}

																fread(	&column	,	4	,	1	,	fp_	)	;
																if	(	swap_	)	{	column	=	trick_byteswap_int(column)	;	}

																fread(	&imaginary	,	4	,	1	,	fp_	)	;
																if	(	swap_	)	{	imaginary	=	trick_byteswap_int(imaginary)	;	}

																fread(	&len	,	4	,	1	,	fp_	)	;
																if	(	swap_	)	{	len	=	trick_byteswap_int(len)	;	}

																temp_ptr	=	new	char[len	+1]	;
																fread(	temp_ptr	,	len	,	1	,	fp_	)	;

																if	(	!	strncmp(	temp_ptr	,	param_name	,	len	)	&&	y_value_.field_num_	<	column	)	{
																								y_value_.size_	=	size	;
																								y_value_.type_	=	type	;
																								y_value_.rows_	=	row	;
																								y_value_.columns_	=	column	;
																								y_value_.imaginary_	=	imaginary	;
																								y_value_.data_offset_	=	ftell(fp_)	;
																								param_found	=	true	;
																}
																if	(	!	strncmp(	temp_ptr	,	time_name	,	len	)	&&	time_value_.field_num_	<	column	)	{
																								time_value_.size_	=	size	;
																								time_value_.type_	=	type	;
																								time_value_.rows_	=	row	;
																								time_value_.columns_	=	column	;
																								time_value_.imaginary_	=	imaginary	;
																								time_value_.data_offset_	=	ftell(fp_)	;
																								time_found	=	true	;
																}

																delete	[]	temp_ptr	;

																//	skip	to	next	parameter
																fseek(	fp_	,		row	*	column	*	size	*	(	1	+	imaginary	)	,	SEEK_CUR	)	;
																offset	=	ftell(fp_)	;
								}

								curr_row_	=	0	;
}

double	MatLab4::getvalue(struct	MatLab4Var	*	mv	)	{
								char	c	;
								short	s	;
								unsigned	short	us	;
								int	i	;
								float	f	;
								double	d	;

								switch	(	mv->type_	)	{
																case	TRICK_CHARACTER:
																								fread(	&c	,	mv->size_	,	1	,	fp_	)	;
																								return((double)c)	;
																case	TRICK_SHORT:
																								fread(	&s	,	mv->size_	,	1	,	fp_	)	;
																								if	(	swap_	)	{	s	=	trick_byteswap_short(s)	;	}
																								return((double)s)	;
																case	TRICK_UNSIGNED_SHORT:
																								fread(	&us	,	mv->size_	,	1	,	fp_	)	;
																								if	(	swap_	)	{	us	=	(unsigned	short)trick_byteswap_short(us)	;	}
																								return((double)us)	;
																case	TRICK_INTEGER:
																								fread(	&i	,	mv->size_	,	1	,	fp_	)	;
																								if	(	swap_	)	{	i	=	trick_byteswap_int(i)	;	}
																								return((double)i)	;
																case	TRICK_FLOAT:
																								fread(	&f	,	mv->size_	,	1	,	fp_	)	;
																								if	(	swap_	)	{	f	=	trick_byteswap_float(f)	;	}
																								return((double)f)	;
																case	TRICK_DOUBLE:
																								fread(	&d	,	mv->size_	,	1	,	fp_	)	;
																								if	(	swap_	)	{	d	=	trick_byteswap_double(d)	;	}
																								return(d)	;
								}
								return(0.0)	;
}

int	MatLab4::get(	double	*	time	,	double	*	value	)	{

								long	offset	;

								if	(	curr_row_	<	y_value_.rows_	&&	curr_row_	<	time_value_.rows_	)	{
																offset	=	time_value_.data_offset_	+	time_value_.rows_	*	time_value_.field_num_	*	time_value_.size_	+
																									curr_row_	*	time_value_.size_	;
																fseek(	fp_	,	offset	,	SEEK_SET)	;
																*time	=	getvalue(&time_value_)	;

																offset	=	y_value_.data_offset_	+	y_value_.rows_	*	y_value_.field_num_	*	y_value_.size_	+
																									curr_row_	*	y_value_.size_	;
																fseek(	fp_	,	offset	,	SEEK_SET)	;
																*value	=	getvalue(&y_value_)	;

																curr_row_++	;
																return(	1	)	;
								}

								return(	0	)	;
}

int	MatLab4::peek(	double	*	time	,	double	*	value	)	{

								long	save_row	;
		int	ret;

								save_row	=	curr_row_	;
								ret	=	get(	time	,	value	)	;
								curr_row_	=	save_row	;

								return(ret)	;
}

void	MatLab4::begin()	{

								curr_row_	=	0	;
								return	;
}

int	MatLab4::end()	{
								return(curr_row_	>=	y_value_.rows_	||	curr_row_	>=	time_value_.rows_	)	;
}

int	MatLab4::step()	{

								if	(	curr_row_	<	y_value_.rows_	&&	curr_row_	<	time_value_.rows_	)	{
																curr_row_++	;
																return(	1	)	;
								}
								return(0)	;

}

int	MatLab4LocateParam(	char	*file_name	,	char	*param_name	,	char	*time_name	)	{

								int	len	;
								int	temp	;
								int	endian	;
								int	mat_size	;
								int	mat_type	;
								int	my_byte_order	;
								div_t	div_result	;
								int	row	,	column	,	imaginary	;
								char	*	temp_ptr	;
								bool	param_found	,	time_found	;
								FILE	*fp	;
								bool	swap	=	false	;
								int	size	;
								int	y_field_num	,	time_field_num	;
								int	max_offset	,	offset	=	0	;

								TRICK_GET_BYTE_ORDER(my_byte_order)	;

								if	((fp	=	fopen(file_name	,	"r"))	==	0	)	{
											std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
											exit(-1)	;
								}

								//	we	use	fseek	calls	to	move	around	so	I	need	to	know	the	max	offset	of	the	file
								fseek(	fp	,	0	,	SEEK_END	)	;
								max_offset	=	ftell(fp)	;
								rewind(	fp	)	;

								//	get	field_num	from	param_name
								temp_ptr	=	rindex(param_name,	'[')	;
								if	(	temp_ptr	!=	NULL	)	{
																sscanf(&temp_ptr[1]	,	"%d"	,	&y_field_num	)	;
								}
								else	{
																y_field_num	=	0	;
								}

								//	get	field_num	from	time_name
								temp_ptr	=	rindex(param_name,	'[')	;
								if	(	temp_ptr	!=	NULL	)	{
																sscanf(&temp_ptr[1]	,	"%d"	,	&time_field_num	)	;
								}
								else	{
																time_field_num	=	0	;
								}

								param_found	=	time_found	=	false	;
								while	(	offset	<	max_offset	)	{

																//	read	the	type	in
																fread(	&temp	,	4	,	1	,	fp	)	;
																if	(	!strncmp(	(char	*)&temp	,	"MATL"	,	4	))	{
																								return(0)	;
																}

																//	byte	swap	it	if	it	looks	like	we	need	to
																if	(	temp	&	0xFFFF0000	)	{
																								temp	=	trick_byteswap_int(temp)	;
																}

																div_result	=	div(temp	,	1000)	;
																endian	=	div_result.quot	;
																temp	=	div_result.rem	;

																switch	(	endian	)	{
																								case	0:
																																swap	=	(	my_byte_order	==	TRICK_LITTLE_ENDIAN	)	?	0	:	1	;
																																break	;
																								case	1:
																																swap	=	(	my_byte_order	==	TRICK_BIG_ENDIAN	)	?	0	:	1	;
																																break	;
																								default:
																																fprintf(stderr,"Unknown	endianness!\n")	;
																																fclose(fp)	;
																																return(0)	;
																																break	;
																}

																div_result	=	div(	temp	,	10	)	;
																mat_size	=	div_result.quot	;
																mat_type	=	div_result.rem	;
																switch	(	mat_size	)	{
																								case	0:
																																size	=	8	;
																																break	;
																								case	1:
																																size	=	4	;
																																break	;
																								case	2:
																																size	=	4	;
																																break	;
																								case	3:
																																size	=	2	;
																																break	;
																								case	4:
																																size	=	2	;
																																break	;
																								case	5:
																																size	=	1	;
																																break	;
																								default	:
																																std::cerr	<<	"Matlab4	"	<<	__FUNCTION__	<<	"ERROR:	invalid	matrix	size"	<<	std::endl;
																																size	=	-1	;	
																																break	;
																}

																fread(	&row	,	4	,	1	,	fp	)	;
																if	(	swap	)	{	row	=	trick_byteswap_int(row)	;	}

																fread(	&column	,	4	,	1	,	fp	)	;
																if	(	swap	)	{	column	=	trick_byteswap_int(column)	;	}

																fread(	&imaginary	,	4	,	1	,	fp	)	;
																if	(	swap	)	{	imaginary	=	trick_byteswap_int(imaginary)	;	}

																fread(	&len	,	4	,	1	,	fp	)	;
																if	(	swap	)	{	len	=	trick_byteswap_int(len)	;	}

																temp_ptr	=	new	char[len	+1]	;
																fread(	temp_ptr	,	len	,	1	,	fp	)	;

																if	(	!	strncmp(	temp_ptr	,	param_name	,	len	)	&&	y_field_num	<	column	)	{
																								param_found	=	true	;
																}

																if	(	!	strncmp(	temp_ptr	,	time_name	,	len	)	&&	time_field_num	<	column	)	{
																								time_found	=	true	;
																}

																if	(	mat_type	!=	0	)	{
																								fprintf(stderr,"Unknown	matrix	type	for	%s\n",	temp_ptr)	;
																}

																if	(	param_found	&&	time_found	)	{
																								fclose(fp)	;
																								delete[]	temp_ptr	;
																								return(1)	;
																}

																delete[]	temp_ptr	;

																//	skip	to	next	parameter
																fseek(	fp	,		row	*	column	*	size	*	(	1	+	imaginary	)	,	SEEK_CUR	)	;
																offset	=	ftell(fp)	;

								}

								fclose(fp)	;
								return(0)	;
}

