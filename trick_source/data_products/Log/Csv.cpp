#include	<cerrno>
#include	<cstring>
#include	<iostream>
#include	<stdlib.h>
#include	<string.h>
#include	<strings.h>
#include	<math.h>
#include	"Csv.hh"
#include	"trick/map_trick_units_to_udunits.hh"

Csv::Csv(char	*	file_name	,	char	*	param_name	)	{

								char	*	header	;
								char	*	next_field	;
								int	len	;

								fileName_	=	file_name	;
								len	=	strlen(param_name)	;

								if	((fp_	=	fopen(file_name	,	"r"))	==	0	)	{
											std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
											exit(-1)	;
								}

								line_	=	new	char[20480]	;
								header	=	new	char[20480]	;
								fgets(	header	,	20480	,	fp_	);

								//	remove	trailing	newline
								header[strlen(header)-1]	=	'\0'	;

								field_num_	=	0	;
								next_field	=	strtok(	header	,	","	)	;

								if	(	strncmp(next_field	,	param_name,	len)	)	{
																field_num_++	;
																while	((next_field	=	strtok(	NULL	,	","	)))	{
																								char	*space_loc	=	strchr(next_field,	'	');
																								int	actual_name_len	=	space_loc	-	next_field;
																								//	need	to	make	sure	if	the	length	of	the	parameter	and	the	length
																								//	of	the	parameter	found	in	the	next_field	are	actually	the	same,
																								//	since	strncmp(a.b.c_more,	a.b.c,	5)	results	a.b.c_more	equals	a.b.c.
																								if	(	strncmp(next_field	,	param_name,	len)	||	actual_name_len	!=	len)	{
																																field_num_++	;
																								}
																								else	{
																																/*	found	the	parameter,	get	the	units	if	there	are	any	*/
																																char	*	start_unit	,		*	end_unit	;
																																if	((start_unit	=	index(	next_field	,	'{'	)))	{
																																								end_unit	=	index(	next_field	,	'}'	)	;
																																								end_unit[0]	=	'\0'	;
																																								if	(	!	strcmp(	param_name	,	"sys.exec.out.time"	))	{
																																																unitTimeStr_	=	start_unit	+	1	;
																																								}
																																								else	{
																																																if	(	!strcmp(start_unit	+	1,"--")	)	{
																																																				unitStr_	=	start_unit	+	1;
																																																}	else	{
																																																				unitStr_	=	map_trick_units_to_udunits(start_unit	+	1)	;
																																																}
																																								}
																																}
																																break	;
																								}
																}
								}

								data_offset_	=	ftell(fp_)	;

								delete[]header	;

}

int	Csv::get(	double	*	time	,	double	*	value	)	{

								int	ii	;
								char	*	field	;
								int	ret	;

								/*	loop	until	we	get	a	good	line	or	we	finish	the	file	*/
								while	(	fgets(	line_	,	20480	,	fp_	)	)	{

																field	=	strtok(	line_	,	","	)	;
																/*	handle	numbers	in	parenthesis	as	negative	numbers	*/
																if	(	(ret	=	sscanf(	field	,	"%*[	\t](%lf)"	,	time	))	==	1	||	(ret	=	sscanf(	field	,	"(%lf)"	,	time	))	==	1		)	{
																								*time	=	-(*time)	;
																}
																else	{
																								ret	=	sscanf(	field	,	"%lf"	,	time	)	;
																}

																if	(	ret	==	1	)	{

																								for	(	ii	=	0	;	ii	<	field_num_	;	ii++	)	{
																																field	=	strtok(NULL	,	",")	;
																								}

																								if	(	field	!=	NULL	)	{
																																/*	handle	numbers	in	parenthesis	as	negative	numbers	*/
																																if	(	sscanf(	field	,	"%*[	\t](%lf)"	,	value	)	==	1	||	sscanf(	field	,	"(%lf)"	,	value	)	==	1	)	{
																																								*value	=	-(*value)	;
																																								/*	successfully	read	both	time	and	value	*/
																																								return(	1	)	;
																																}
																																else	{
																																								ret	=	sscanf(	field	,	"%lf"	,		value	)	;
																																								if	(	ret	==	1	)	{
																																																/*	successfully	read	both	time	and	value	*/
																																																return(	1	)	;
																																								}
																																}
																								}
																}
								}

								return	(	0	)	;

}

int	Csv::peek(	double	*	time	,	double	*	value	)	{

								long	offset	;
								int	ret	;

								offset	=	ftell(fp_)	;
								ret	=	get(	time	,	value	)	;
								fseek(fp_	,	offset	,	SEEK_SET	)	;

								return(ret)	;
}

void	Csv::begin()	{
								fseek(fp_	,	data_offset_	,	SEEK_SET	)	;
								return	;
}

int	Csv::end()	{
								return(feof(fp_))	;
}

int	Csv::step()	{

								if	(	fgets(	line_	,	20480	,	fp_	)	)	{
																return(1)	;
								}

								return(0)	;
}

int	CsvLocateParam(	char	*	file_name	,	char	*	param_name	)	{
								char	*	header	;
								char	*	next_field	;
								FILE	*	fp	;
								int	len	;

								len	=	strlen(param_name)	;

								if	((fp	=	fopen(file_name	,	"r"))	!=	0	)	{
																header	=	new	char[20480]	;
																fgets(	header	,	20480	,	fp	);

																//	remove	trailing	newline
																header[strlen(header)-1]	=	'\0'	;
																next_field	=	strtok(	header	,	","	)	;
																do	{
																								if	(	!	strncmp(next_field	,	param_name,	len)	)	{
																																fclose(fp)	;
																																delete[]header	;
																																return(1)	;
																								}
																}	while	((next_field	=	strtok(	NULL	,	","	)))	;

																delete[]header	;
																fclose(fp)	;

								}
								else	{
											std::cerr	<<	"ERROR:		Couldn't	open	\""	<<	file_name	<<	"\":	"	<<	std::strerror(errno)	<<	std::endl;
								}

								return(0)	;
}


