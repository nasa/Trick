#include	<cerrno>
#include	<cstring>
#include	<iostream>

#include	<stdlib.h>
#include	<string.h>
#if	__sgi	|	__sun
#include	<strings.h>
#endif

#include	<math.h>
#include	"MatLab.hh"
#include	"trick_byteswap.h"

#include	"trick/parameter_types.h"

//	Add	to	the	parameter	types
#define	MATLAB_ARRAY	21
#define	MATLAB_UNSUPPORTED	22

//	These	come	from	MatLab	documentation
#define	MI_INT8			1
#define	MI_UINT8		2
#define	MI_INT16		3
#define	MI_UINT16	4
#define	MI_INT32		5
#define	MI_UINT32	6
#define	MI_SINGLE	7
#define	MI_DOUBLE	9
#define	MI_INT64		12
#define	MI_UINT64	13
#define	MI_MATRIX	14

//	Supported	array	types

#define	mxSTRUCT_CLASS	2
#define	mxDOUBLE_CLASS	6
/*
	TODO:	support	more	array	types?
				#define	mxCELL_CLASS	1
				#define	mxOBJECT_CLASS	3
				#define	mxCHAR_CLASS	4
				#define	mxSPARSE_CLASS	5
				#define	mxSINGLE_CLASS	7
				#define	mxINT8_CLASS	8
				#define	mxUINT8_CLASS	9
				#define	mxINT16_CLASS	10
*/

MatLab::MatLab(char	*	file_name,	char	*	param_name,	char	*	time_name)	{

				const	int	file_type_len	=	124;
				char	file_type[file_type_len	+	1];
				int	ii;
				char	*	temp_ptr;
				int	temp;
				short	version;
				short	magic;
				int	data_type,	bytes,	field_data_type,	field_bytes;
				int	size	=	0,	type	=	0;
				int	array_type,	array_bytes,	array_flags[2];
				int	array_complex,	array_class	;
				int	dim_type,	dim_bytes,	num_dims;
				int	*	dims	=	NULL;
				int	name_bytes;
				char	*	name,	*	field_name,	*	variable_name;
				int	real_type,	real_bytes;
				int	img_type,	img_bytes;
				bool	param_found,	time_found,	found,	field_found;
				int	offset;

				int	field_index,	field_num;

				fileName_	=	file_name;

				if	((fp_	=	fopen(file_name,	"r"))	==	0)	{
								std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
								return;
				}

				//	we	use	fseek	calls	to	move	around	so	I	need	to	know	the	max	offset	of	the	file
				fseek(fp_,	0,	SEEK_END)	;
				rewind(fp_)	;

				//	TODO:		This	only	handles	single	dimensional	arrays

				//	get	field_num	from	param_name
				temp_ptr	=	rindex(param_name,	'[')	;
				if	(temp_ptr	!=	NULL)	{
								sscanf(&temp_ptr[1],	"%d",	&y_value_.field_num_)	;
				}	else	{
								y_value_.field_num_	=	0;
				}

				//	get	field_num	from	time_name
				temp_ptr	=	rindex(time_name,	'[')	;
				if	(temp_ptr	!=	NULL)	{
								sscanf(&temp_ptr[1],	"%d",	&time_value_.field_num_)	;
				}	else	{
								time_value_.field_num_	=	0;
				}

				fread(file_type,	file_type_len,	1,	fp_)	;
				file_type[file_type_len]	=	'\0';

				if	(	!strncmp(file_type,	"MATL",	4))	{

								fread(&version,	2,	1,	fp_)	;
								fread(&magic,	2,	1,	fp_)	;
								if	(magic	==	0x4d49)	{
												swap_	=	0;
								}	else	if	(magic	==	0x494d)	{
												swap_	=	1;
								}

								param_found	=	time_found	=	false;
								while	(	(!param_found	||	!time_found)	&&	!feof(fp_))	{

												//	Start	to	access	a	new	data	element
												found	=	false;

												//	this	is	either	0	or	num_bytes	if	non_zero
												fread(&temp,	4,	1,	fp_)	;
												if	(swap_)	{
																temp	=	trick_byteswap_int(temp)	;
												}
												if	(	(temp	>>	16	)	==	0)	{
																//	data_type
																data_type	=	temp;

																//	bytes
																fread(&bytes,	4,	1,	fp_)	;
																if	(swap_)	{
																				bytes	=	trick_byteswap_int(bytes)	;
																}
												}	else	{
																bytes	=	temp	>>	16;
																data_type	=	temp	&	0x0000FFFF;
												}

												//	Matricies	are	what	we	want	to	deal	with
												if	(data_type	==	MI_MATRIX)	{
																//	array	type	is	always	miUINT32
																fread(&array_type,	4,	1,	fp_)	;
																//	array	bytes	is	always	8
																fread(&array_bytes,	4,	1,	fp_)	;

																//	array	flags
																fread(array_flags,	8,	1,	fp_)	;
																if	(swap_)	{
																				array_flags[0]	=	trick_byteswap_int(array_flags[0])	;
																}
																array_complex	=	(array_flags[0]	&	0x00000800	)	?	1	:	0;
																array_class	=	array_flags[0]	&	0x000000FF;

																//	dim	type	is	always	miINT32
																fread(&dim_type,	4,	1,	fp_)	;
																if	(swap_)	{
																				dim_type	=	trick_byteswap_int(dim_type)	;
																}

																//	dim	bytes	=	(	num_dim	*	4	)
																fread(&dim_bytes,	4,	1,	fp_)	;
																if	(swap_)	{
																				dim_bytes	=	trick_byteswap_int(dim_bytes)	;
																}

																num_dims	=	dim_bytes	/	4;
																dims	=	new	int[num_dims];
																for	(ii	=	0;	ii	<	num_dims;	ii++)	{
																				fread(&dims[ii],	4,	1,	fp_)	;
																				if	(swap_)	{
																								dims[ii]	=	trick_byteswap_int(dims[ii])	;
																				}
																}

																//	name	field,	could	be	a	short	element	type	if	we	have	a	short	name
																//	this	is	either	0	or	num_bytes	if	non_zero
																fread(&temp,	4,	1,	fp_)	;
																if	(swap_)	{
																				temp	=	trick_byteswap_short(temp)	;
																}
																if	(	(temp	>>	16	)	==	0)	{
																				//	bytes
																				fread(&name_bytes,	4,	1,	fp_)	;
																				if	(swap_)	{
																								name_bytes	=	trick_byteswap_int(name_bytes)	;
																				}
																}	else	{
																				name_bytes	=	temp	>>	16;
																}

																name	=	new	char[name_bytes	+	1];
																fread(name,	name_bytes,	1,	fp_)	;
																name[name_bytes]	=	'\0';

																//	name	pad
																offset	=	ftell(fp_)	;
																if	(offset	%	8)	{
																				fseek(fp_,	8	-	(offset	%	8),	SEEK_CUR)	;
																}

																//	for	mxSTRUCT_CLASS
																if	(array_class	==	mxSTRUCT_CLASS)	{
																				int	field_name_len,	each_field_name_len;
																				fseek(fp_,	4,	SEEK_CUR);
																				fread(&each_field_name_len,	4,	1,	fp_);
																				if	(swap_)	{
																								each_field_name_len	=	trick_byteswap_short(each_field_name_len);
																				}

																				fseek(fp_,	4,	SEEK_CUR);
																				fread(&field_name_len,	4,	1,	fp_);
																				if	(swap_)	{
																								field_name_len	=	trick_byteswap_short(field_name_len);
																				}

																				field_found	=	false;
																				field_num	=	field_name_len	/	each_field_name_len;
																				for	(field_index	=	0;	field_index	<	field_num;	field_index++)	{
																								field_name	=	new	char[each_field_name_len];
																								fread(field_name,	each_field_name_len,	1,	fp_);

																								variable_name	=	new	char[name_bytes+each_field_name_len+1];
																								strcpy(variable_name,	name);

																								strcat(variable_name,	".");
																								strcat(variable_name,	field_name);

																								if	(!strcmp(	variable_name	,	param_name	))	{
																												name	=	new	char[strlen(variable_name)];
																												strcpy(name,	variable_name);
																												field_found	=	true;
																												delete	[]	field_name;
																												delete	[]	variable_name;
																												break;
																								}
																								delete	[]	field_name;
																								delete	[]	variable_name;
																				}

																				if	(field_found)	{
																								fseek(fp_,	(field_num-field_index-1)*each_field_name_len,	SEEK_CUR);
																				}

																				ii	=	0;
																				while	(ii	<	field_num)	{
																								//	Numeric	array	for	each	field
																								//	this	is	either	0	or	num_bytes	if	non_zero
																								fread(&temp,	4,	1,	fp_)	;
																								if	(swap_)	{
																												temp	=	trick_byteswap_int(temp)	;
																								}
																								if	(	(temp	>>	16	)	==	0)	{
																												//	data_type
																												field_data_type	=	temp;

																												//	bytes
																												fread(&field_bytes,	4,	1,	fp_)	;
																												if	(swap_)	{
																																field_bytes	=	trick_byteswap_int(field_bytes)	;
																												}
																								}	else	{
																												field_bytes	=	temp	>>	16;
																												field_data_type	=	temp	&	0x0000FFFF;
																								}

																								if	(ii	==	field_index)	{
																												break;
																								}	else	{
																												fseek(fp_,	field_bytes,	SEEK_CUR);
																												ii++;
																								}
																				}

																				if	(field_found	){
																								if	(field_data_type	==	MI_MATRIX)	{
																												//	array	type	is	always	miUINT32
																												fread(&array_type,	4,	1,	fp_)	;

																												//	array	bytes	is	always	8
																												fread(&array_bytes,	4,	1,	fp_)	;

																												//	array	flags
																												fread(array_flags,	8,	1,	fp_)	;
																												if	(swap_)	{
																																array_flags[0]	=	trick_byteswap_int(array_flags[0])	;
																												}

																												//	dim	type	is	always	miINT32
																												fread(&dim_type,	4,	1,	fp_)	;
																												if	(swap_)	{
																																dim_type	=	trick_byteswap_int(dim_type)	;
																												}

																												fread(&dim_bytes,	4,	1,	fp_)	;
																												if	(swap_)	{
																																dim_bytes	=	trick_byteswap_int(dim_bytes)	;
																												}

																												num_dims	=	dim_bytes	/	4;
																												dims	=	new	int[num_dims];
																												for	(ii	=	0;	ii	<	num_dims;	ii++)	{
																																fread(&dims[ii],	4,	1,	fp_)	;
																																if	(swap_)	{
																																				dims[ii]	=	trick_byteswap_int(dims[ii])	;
																																}
																												}

																												//	each	array	name
																												fread(&temp,	4,	1,	fp_)	;
																												fread(&temp,	4,	1,	fp_);

																												//	field	data,	could	be	a	small	data	element	format	if	we
																												//	have	a	data	element	takes	up	only	1	to	4	bytes.
																												//	if	first	2	bytes	of	the	tag	are	not	0,	the	tag	uses	the	small
																												//	data	element	format.
																												fread(&temp,	4,	1,	fp_)	;
																												if	(swap_)	{
																																temp	=	trick_byteswap_short(temp)	;
																												}

																												if	(	(temp	>>	16	)	==	0)	{
																																//	data_type
																																real_type	=	temp;

																																//	bytes
																																fread(&real_bytes,	4,	1,	fp_)	;
																																if	(swap_)	{
																																				real_bytes	=	trick_byteswap_int(real_bytes)	;
																																}
																												}	else	{
																																real_bytes	=	temp	>>	16;
																																real_type	=	temp	&	0x0000FFFF;

																												}

																												//	TODO:	make	sure	else	part
																								}	else	{
																												//	Everythig	else	we	skip
																												temp_ptr	=	new	char[field_bytes];
																												fread(temp_ptr,	field_bytes,	1,	fp_)	;
																												delete	[]	temp_ptr;
																								}
																				}	else	{
																								real_bytes	=	0;
																								real_type	=	0;
																				}

																}	else	{
																				//	real	data	type
																				fread(&real_type,	4,	1,	fp_)	;
																				if	(swap_)	{
																								real_type	=	trick_byteswap_int(real_type)	;
																				}
																				//	bytes
																				fread(&real_bytes,	4,	1,	fp_)	;
																				if	(swap_)	{
																								real_bytes	=	trick_byteswap_int(real_bytes)	;
																				}
																}

																switch	(real_type)	{
																case	MI_INT8:
																				type	=	TRICK_CHARACTER;
																				size	=	1;
																				break;
																case	MI_UINT8:
																				type	=	TRICK_UNSIGNED_CHARACTER;
																				size	=	1;
																				break;
																case	MI_INT16:
																				type	=	TRICK_SHORT;
																				size	=	2;
																				break;
																case	MI_UINT16:
																				type	=	TRICK_UNSIGNED_SHORT;
																				size	=	2;
																				break;
																case	MI_INT32:
																				type	=	TRICK_INTEGER;
																				size	=	4;
																				break;
																case	MI_UINT32:
																				type	=	TRICK_UNSIGNED_INTEGER;
																				size	=	4;
																				break;
																case	MI_SINGLE:
																				type	=	TRICK_FLOAT;
																				size	=	4;
																				break;
																case	MI_DOUBLE:
																				type	=	TRICK_DOUBLE;
																				size	=	8;
																				break;
																case	MI_INT64:
																				type	=	TRICK_LONG_LONG;
																				size	=	8;
																				break;
																case	MI_UINT64:
																				type	=	TRICK_UNSIGNED_LONG_LONG;
																				size	=	8;
																				break;
																case	15:	//	miCOMPRESSED
																case	16:	//	miUTF8
																case	17:	//	miUTF16
																case	18:	//	miUTF32
																case	0:
																				type	=	MATLAB_UNSUPPORTED;
																				size	=	0;
																				break;
																}

																//	TODO:	Array	bounds	checking
																if	(array_class	==	mxSTRUCT_CLASS)	{

																				if	(!strcmp(name,	param_name))	{
																								y_value_.size_	=	size;
																								y_value_.type_	=	type;
																								y_value_.bytes_	=	real_bytes;
																								y_value_.num_dims_	=	num_dims;
																								y_value_.dims_	=	dims;
																								y_value_.data_offset_	=	ftell(fp_)	;
																								found	=	param_found	=	true;
																				}
																				if	(!strcmp(name,	time_name))	{
																								time_value_.size_	=	size;
																								time_value_.type_	=	type;
																								time_value_.bytes_	=	real_bytes;
																								time_value_.num_dims_	=	num_dims;
																								time_value_.dims_	=	dims;
																								time_value_.data_offset_	=	ftell(fp_)	;
																								found	=	time_found	=	true;
																				}
																}	else	{
																				if	(!strcmp(name,	param_name))	{
																								y_value_.size_	=	size;
																								y_value_.type_	=	type;
																								y_value_.bytes_	=	real_bytes;
																								y_value_.num_dims_	=	num_dims;
																								y_value_.dims_	=	dims;
																								y_value_.data_offset_	=	ftell(fp_)	;
																								found	=	param_found	=	true;
																				}

																				if	(	!strcmp(name,	time_name))	{
																								time_value_.size_	=	size;
																								time_value_.type_	=	type;
																								time_value_.bytes_	=	real_bytes;
																								time_value_.num_dims_	=	num_dims;
																								time_value_.dims_	=	dims;
																								time_value_.data_offset_	=	ftell(fp_)	;
																								found	=	time_found	=	true;
																				}
																}

																//	skip	past	this	array
																fseek(fp_,	real_bytes,	SEEK_CUR)	;
																if	(array_class	!=	mxSTRUCT_CLASS)	{

																				//	make	sure	that	we	are	on	a	64	bit	boundary
																				offset	=	ftell(fp_)	;
																				if	(offset	%	8)	{
																								fseek(fp_,	8	-	(offset	%	8),	SEEK_CUR)	;
																				}

																				//	skip	over	the	imaginary	part	if	any	right	now
																				if	(array_complex)	{
																								//	real	data	type
																								fread(&img_type,	4,	1,	fp_)	;
																								//	bytes
																								fread(&img_bytes,	4,	1,	fp_)	;
																								if	(swap_)	{
																												img_bytes	=	trick_byteswap_int(img_bytes)	;
																								}

																								fseek(fp_,	img_bytes,	SEEK_CUR)	;

																								//	make	sure	that	we	are	on	a	64	bit	boundary
																								offset	=	ftell(fp_)	;
																								if	(offset	%	8)	{
																												fseek(fp_,	8	-	(offset	%	8),	SEEK_CUR)	;
																								}

																				}
																}	else	{
																				//	for	mxSTRUCT_CLASS,	skip	pass	all	the	other	fields
																				while	(field_index	<	field_num-1)	{
																								//	Numeric	array	for	each	field
																								//	this	is	either	0	or	num_bytes	if	non_zero
																								fread(&temp,	4,	1,	fp_)	;
																								if	(swap_)	{
																												temp	=	trick_byteswap_int(temp)	;
																								}
																								if	(	(temp	>>	16	)	==	0)	{
																												//	bytes
																												fread(&bytes,	4,	1,	fp_)	;
																												if	(swap_)	{
																																bytes	=	trick_byteswap_int(bytes)	;
																												}
																								}	else	{
																												bytes	=	temp	>>	16;
																								}
																								fseek(fp_,	bytes,	SEEK_CUR);
																								field_index++;
																				}

																				//	make	sure	that	we	are	on	a	64	bit	boundary
																				offset	=	ftell(fp_)	;
																				if	(offset	%	8)	{
																								fseek(fp_,	8	-	(offset	%	8),	SEEK_CUR)	;
																				}

																}

																if	(	!found)	{
																				delete	[]	dims;
																				dims	=	NULL;
																}
																delete	[]	name;

												}	else	{
																//	Everythig	else	we	skip
																temp_ptr	=	new	char[bytes];
																fread(temp_ptr,	bytes,	1,	fp_)	;
																delete	[]	temp_ptr;
												}
								}
				}

				curr_row_	=	0;
}

double	MatLab::getvalue(struct	MatLabVar	*	mv)	{
				char	c;
				short	s;
				unsigned	short	us;
				int	i;
				float	f;
				double	d;

				switch	(mv->type_)	{
				case	TRICK_CHARACTER:
								fread(	&c,	mv->size_,	1,	fp_)	;
								return	((double)c);
				case	TRICK_SHORT:
								fread(	&s,	mv->size_,	1,	fp_)	;
								if	(swap_)	{
												s	=	trick_byteswap_short(s)	;
								}
								return	((double)s);
				case	TRICK_UNSIGNED_SHORT:
								fread(	&us,	mv->size_,	1,	fp_)	;
								if	(swap_)	{
												us	=	(unsigned	short)trick_byteswap_short(us)	;
								}
								return	((double)us);
				case	TRICK_INTEGER:
								fread(	&i,	mv->size_,	1,	fp_)	;
								if	(swap_)	{
												i	=	trick_byteswap_int(i)	;
								}
								return	((double)i);
				case	TRICK_FLOAT:
								fread(	&f,	mv->size_,	1,	fp_)	;
								if	(swap_)	{
												f	=	trick_byteswap_float(f)	;
								}
								return	((double)f);
				case	TRICK_DOUBLE:
								fread(	&d,	mv->size_,	1,	fp_)	;
								if	(swap_)	{
												d	=	trick_byteswap_double(d)	;
								}
								return	(d);
				}

				return	(0);
}

int	MatLab::get(double	*	time,	double	*	value)	{

				long	offset;


				if	(curr_row_	<	y_value_.dims_[0]	&&	curr_row_	<	time_value_.dims_[0])	{
								offset	=	time_value_.data_offset_	+	time_value_.dims_[0]
																*	time_value_.field_num_	*	time_value_.size_	+	curr_row_
																*	time_value_.size_;
								fseek(fp_,	offset,	SEEK_SET)	;
								*time	=	getvalue(&time_value_)	;

								offset	=	y_value_.data_offset_	+	y_value_.dims_[0]
																*	y_value_.field_num_	*	y_value_.size_	+	curr_row_
																*	y_value_.size_;
								fseek(fp_,	offset,	SEEK_SET)	;
								*value	=	getvalue(&y_value_)	;
								curr_row_++;

								return	(	1	);
				}

				return	(	0	);
}

int	MatLab::peek(double	*	time,	double	*	value)	{

				long	save_row;
	int	ret;

				save_row	=	curr_row_;
				ret	=	get(time,	value)	;
				curr_row_	=	save_row;

				return	(ret);
}

void	MatLab::begin()	{
				curr_row_	=	0;
				return;
}

int	MatLab::end()	{
				return	(curr_row_	>=	y_value_.dims_[0]	||	curr_row_	>=	time_value_.dims_[0]	);
}

int	MatLab::step()	{

				if	(curr_row_	<	y_value_.dims_[0]	&&	curr_row_	<	time_value_.dims_[0])	{
								curr_row_++;
								return	(	1	);
				}
				return	(0);

}

int	MatLabLocateParam(char	*	file_name,	char	*	param_name,	char	*	time_name)	{
				const	int	file_type_len	=	124;
				char	file_type[file_type_len	+	1];
				int	ii;
				char	*	temp_ptr;
				int	temp;
				short	version;
				short	magic;
				int	data_type,	bytes;
				int	array_type,	array_bytes,	array_flags[2];
				int	array_complex,	array_class;
				int	dim_type,	dim_bytes,	num_dims;
				int	*	dims	=	NULL;
				int	name_bytes;
				char	*	name,	*	field_name,	*	variable_name;
				int	real_type,	real_bytes;
				int	img_type,	img_bytes;
				bool	param_found,	time_found,	field_found;
				int	offset	=	0;
				int	swap;
				int	y_field_num,	time_field_num;
				FILE	*fp;
				int	field_index,	field_num;

				if	((fp	=	fopen(file_name,	"r"))	==	0)	{
								std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
								return	(0);
				}

				//	we	use	fseek	calls	to	move	around	so	I	need	to	know	the	max	offset	of	the	file
				fseek(fp,	0,	SEEK_END)	;
				rewind(fp)	;

				//	TODO:		This	only	handles	single	dimensional	arrays

				//	get	field_num	from	param_name
				temp_ptr	=	rindex(param_name,	'[')	;
				if	(temp_ptr	!=	NULL)	{
								sscanf(&temp_ptr[1],	"%d",	&y_field_num)	;
				}	else	{
								y_field_num	=	0;
				}

				//	get	field_num	from	time_name
				temp_ptr	=	rindex(time_name,	'[')	;
				if	(temp_ptr	!=	NULL)	{
								sscanf(&temp_ptr[1],	"%d",	&time_field_num)	;
				}	else	{
								time_field_num	=	0;
				}

				fread(file_type,	file_type_len,	1,	fp)	;
				file_type[file_type_len]	=	'\0';

				if	(	!strncmp(file_type,	"MATL",	4))	{

								fread(&version,	2,	1,	fp)	;
								fread(&magic,	2,	1,	fp)	;
								if	(magic	==	0x4d49)	{
												swap	=	0;
								}	else	if	(magic	==	0x494d)	{
												swap	=	1;
								}	else	{
												std::cerr	<<	"ERROR:	MatLab.cpp	swap	magic	is	invalid,	default	to	byte	swap	==	false"	<<	std::endl;
												swap	=	0;
								}

								param_found	=	time_found	=	false;
								while	(	(!param_found	||	!time_found)	&&	!feof(fp))	{

												//	this	is	either	0	or	num_bytes	if	non_zero
												fread(&temp,	4,	1,	fp)	;
												if	(swap)	{
																temp	=	trick_byteswap_int(temp)	;
												}
												if	(	(temp	>>	16	)	==	0)	{
																//	data_type
																data_type	=	temp;

																//	bytes
																fread(&bytes,	4,	1,	fp)	;
																if	(swap)	{
																				bytes	=	trick_byteswap_int(bytes)	;
																}
												}	else	{
																bytes	=	temp	>>	16;
																data_type	=	temp	&	0x0000FFFF;
												}

												//	Matricies	are	what	we	want	to	deal	with
												if	(data_type	==	MI_MATRIX)	{
																//	array	type	is	always	miUINT32
																fread(&array_type,	4,	1,	fp)	;
																//	array	bytes	is	always	8
																fread(&array_bytes,	4,	1,	fp)	;

																//	array	flags
																fread(array_flags,	8,	1,	fp)	;
																if	(swap)	{
																				array_flags[0]	=	trick_byteswap_int(array_flags[0])	;
																}
																array_complex	=	(array_flags[0]	&	0x00000800	)	?	1	:	0;
																array_class	=	array_flags[0]	&	0x000000FF;

																//	dim	type	is	always	miINT32
																fread(&dim_type,	4,	1,	fp)	;
																if	(swap)	{
																				dim_type	=	trick_byteswap_int(dim_type)	;
																}

																//	dim	bytes	=	(	num_dim	*	4	)
																fread(&dim_bytes,	4,	1,	fp)	;
																if	(swap)	{
																				dim_bytes	=	trick_byteswap_int(dim_bytes)	;
																}

																num_dims	=	dim_bytes	/	4;
																dims	=	new	int[num_dims];
																for	(ii	=	0;	ii	<	num_dims;	ii++)	{
																				fread(&dims[ii],	4,	1,	fp)	;
																				if	(swap)	{
																								dims[ii]	=	trick_byteswap_int(dims[ii])	;
																				}
																}

																//	name	field,	could	be	a	short	element	type	if	we	have	a	short	name
																//	this	is	either	0	or	num_bytes	if	non_zero
																fread(&temp,	4,	1,	fp)	;
																if	(swap)	{
																				temp	=	trick_byteswap_short(temp)	;
																}
																if	(	(temp	>>	16	)	==	0)	{
																				//	bytes
																				fread(&name_bytes,	4,	1,	fp)	;
																				if	(swap)	{
																								name_bytes	=	trick_byteswap_int(name_bytes)	;
																				}
																}	else	{
																				name_bytes	=	temp	>>	16;
																}

																name	=	new	char[name_bytes	+	1];
																fread(name,	name_bytes,	1,	fp)	;
																name[name_bytes]	=	'\0';

																//	name	pad
																offset	=	ftell(fp)	;
																if	(offset	%	8)	{
																				fseek(fp,	8	-	(offset	%	8),	SEEK_CUR)	;
																}

																//	for	mxSTRUCT_CLASS
																if	(array_class	==	mxSTRUCT_CLASS)	{
																				int	field_name_len,	each_field_name_len;
																				fseek(fp,	4,	SEEK_CUR);
																				fread(&each_field_name_len,	4,	1,	fp);
																				if	(swap)	{
																								each_field_name_len	=	trick_byteswap_short(each_field_name_len);
																				}

																				fseek(fp,	4,	SEEK_CUR);
																				fread(&field_name_len,	4,	1,	fp);
																				if	(swap)	{
																								field_name_len	=	trick_byteswap_short(field_name_len);
																				}

																				field_num	=	field_name_len	/	each_field_name_len;

																				field_found	=	false;
																				for	(field_index	=	0;	field_index	<	field_num;	field_index++)	{
																								field_name	=	new	char[each_field_name_len];
																								fread(field_name,	each_field_name_len,	1,	fp);
																								variable_name	=	new	char[name_bytes+each_field_name_len+1];
																								strcpy(variable_name,	name);
																								strcat(variable_name,	".");
																								strcat(variable_name,	field_name);

																								if	(!strcmp(	variable_name	,	param_name	)	)	{
																												name	=	new	char[strlen(variable_name)];
																												strcpy(name,	variable_name);
																												field_found	=	true;
																												delete	[]	field_name;
																												delete	[]	variable_name;
																												break;
																								}
																								delete	[]	field_name;
																								delete	[]	variable_name;
																				}

																				if	(field_found)	{
																								fseek(fp,	(field_num-field_index-1)*each_field_name_len,	SEEK_CUR);
																				}
																				ii	=	0;
																				while	(ii	<	field_num)	{
																								//	Numeric	array	for	each	field
																								//	this	is	either	0	or	num_bytes	if	non_zero
																								fread(&temp,	4,	1,	fp)	;
																								if	(swap)	{
																												temp	=	trick_byteswap_int(temp)	;
																								}
																								if	(	(temp	>>	16	)	==	0)	{

																												//	bytes
																												fread(&bytes,	4,	1,	fp)	;
																												if	(swap)	{
																																bytes	=	trick_byteswap_int(bytes)	;
																												}
																								}	else	{
																												bytes	=	temp	>>	16;
																								}
																								fseek(fp,	bytes,	SEEK_CUR);
																								ii++;
																				}
																}	else	{
																				//	real	data	type
																				fread(&real_type,	4,	1,	fp)	;
																				if	(swap)	{
																								real_type	=	trick_byteswap_int(real_type)	;
																				}
																				//	bytes
																				fread(&real_bytes,	4,	1,	fp)	;
																				if	(swap)	{
																								real_bytes	=	trick_byteswap_int(real_bytes)	;
																				}
																}

																//	TODO:	Array	bounds	checking
																if	(array_class	==	mxSTRUCT_CLASS)	{

																				if	(!strcmp(name,	param_name))	{
																								param_found	=	true;
																				}

																				if	(!strcmp(name,	time_name))	{

																							time_found	=	true;
																				}
																}	else	{

																				if	(!strncmp(name,	param_name,	name_bytes))	{
																								param_found	=	true;
																				}
																				if	(!strncmp(name,	time_name,	name_bytes))	{
																								time_found	=	true;
																				}

																}
																delete	[]	name;
																//	delete	[]	dims;
																dims	=	NULL;

																if	(param_found	&&	time_found)	{
																				fclose(fp);
																				return(1);
																}
																if	(array_class	!=	mxSTRUCT_CLASS)	{
																				//	skip	past	this	array
																				fseek(fp,	real_bytes,	SEEK_CUR)	;

																				//	make	sure	that	we	are	on	a	64	bit	boundary
																				offset	=	ftell(fp)	;
																				if	(offset	%	8)	{
																								fseek(fp,	8	-	(offset	%	8),	SEEK_CUR)	;
																				}

																				//	skip	over	the	imaginary	part	if	any	right	now
																				if	(array_complex)	{
																								//	real	data	type
																								fread(&img_type,	4,	1,	fp)	;
																								//	bytes
																								fread(&img_bytes,	4,	1,	fp)	;
																								if	(swap)	{
																												img_bytes	=	trick_byteswap_int(img_bytes)	;
																								}

																								fseek(fp,	img_bytes,	SEEK_CUR)	;

																								//	make	sure	that	we	are	on	a	64	bit	boundary
																								offset	=	ftell(fp)	;
																								if	(offset	%	8)	{
																												fseek(fp,	8	-	(offset	%	8),	SEEK_CUR)	;
																								}

																				}
																}
												}	else	{
																//	Everythig	else	we	skip
																temp_ptr	=	new	char[bytes];
																fread(temp_ptr,	bytes,	1,	fp)	;
																delete	[]	temp_ptr;
												}
								}
				}
				fclose(fp)	;
				return	(0);

}

