/*
PURPOSE:
				(Data	record	to	disk	in	binary	format.)
PROGRAMMERS:
					(((Robert	W.	Bailey)	(LinCom	Corp)	(3/96)	(SES	upgrades)
					((Alex	Lin)	(NASA)	(April	2009)	(--)	(c++	port)))
*/

#include	<iostream>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<unistd.h>

#include	"trick/DRBinary.hh"
#include	"trick/command_line_protos.h"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/bitfield_proto.h"

/*
			Other	classes	inherit	from	DRBinary.	In	these	cases,	we	don't	want	to	register	the	memory	as	DRBinary,
			so	register_group	will	be	set	to	false.
*/
Trick::DRBinary::DRBinary(	std::string	in_name	,	bool	register_group	)	:	Trick::DataRecordGroup(in_name)	{
				if	(	register_group	)	{
								register_group_with_mm(this,	"Trick::DRBinary")	;
				}
}

int	Trick::DRBinary::format_specific_header(	std::fstream	&	out_stream	)	{
				out_stream	<<	"	byte_order	is	"	<<	byte_order	<<	std::endl	;
				return(0)	;
}

/**
@details
-#	Set	the	file	extension	to	".trk"
-#	Allocate	enough	memory	to	hold	#record_size	of	records	in	memory
-#	Open	the	log	file
			-#	Return	an	error	if	the	open	failed
-#	Write	out	the	magic	Trick-07-[LB]	keyword,	L	for	little	endian,	B	for	big.
-#	Write	out	the	number	of	variables	recorded
-#	For	each	variable	to	be	recorded
			-#	Write	out	the	name
			-#	Write	out	the	units
			-#	Write	out	the	type
			-#	Write	out	the	size
-#	Declare	the	recording	group	to	the	memory	manager	so	that	the	group	can	be	checkpointed
			and	restored
*/
int	Trick::DRBinary::format_specific_init()	{

				unsigned	int	jj	;
				int	write_value	;
				/*	number	of	bytes	written	to	data	record	*/
				int	bytes	=	0	;

				union	{
								long	l;
								char	c[sizeof(long)];
				}	byte_order_union;

				file_name.append(".trk");

				/*	Calculate	a	"worst	case"	for	space	used	for	1	record.	*/
				writer_buff	=	(char	*)calloc(1	,	record_size	*	rec_buffer.size())	;

				/*	This	loop	touches	all	of	the	memory	locations	in	the	allocation	forcing	the
							system	to	actually	do	the	allocation	*/
				for	(	jj=	0	;	jj	<	record_size	*	rec_buffer.size()	;	jj	+=	1024	)	{
								writer_buff[jj]	=	1	;
				}
				writer_buff[record_size	*	rec_buffer.size()	-	1]	=	1	;

				/*	start	header	information	in	trk	file	*/
				if	((fd	=	creat(file_name.c_str(),	S_IRUSR	|	S_IWUSR	|	S_IRGRP	|	S_IWGRP	|	S_IROTH	|	S_IWOTH))	==	-1)	{
								record	=	false	;
								return	(-1)	;
				}


				/*	Check	to	see	if	data	is	being	recorded	in	little	endian
					*	byte	order,	and	add	little	endian	line	if	so.
					*/
				byte_order_union.l	=	1	;
				if	(byte_order_union.c[sizeof(long)-1]	!=	1)	{
								bytes	+=	write(	fd	,	"Trick-10-L",	(size_t)10	)	;
								
				}	else	{
								bytes	+=	write(	fd	,	"Trick-10-B",	(size_t)10	)	;
				}
				write_value	=	rec_buffer.size()	;
				bytes	+=	write(	fd	,	&write_value	,	sizeof(int)	)	;

				for	(jj	=	0;	jj	<	rec_buffer.size();	jj++)	{
								/*	name	*/
								write_value	=	strlen(rec_buffer[jj]->ref->reference)	;
								bytes	+=	write(	fd	,	&write_value	,	sizeof(int))	;
								bytes	+=	write(	fd	,	rec_buffer[jj]->ref->reference	,	write_value	)	;

								/*	units	*/
								if	(	rec_buffer[jj]->ref->attr->mods	&	TRICK_MODS_UNITSDASHDASH	)	{
												write_value	=	strlen("--")	;
												bytes	+=	write(	fd	,	&write_value	,	sizeof(int))	;
												bytes	+=	write(	fd	,	"--"	,	write_value	)	;
								}	else	{
												write_value	=	strlen(rec_buffer[jj]->ref->attr->units)	;
												bytes	+=	write(	fd	,	&write_value	,	sizeof(int))	;
												bytes	+=	write(	fd	,	rec_buffer[jj]->ref->attr->units	,	write_value	)	;
								}

								write_value	=	rec_buffer[jj]->ref->attr->type	;
								bytes	+=	write(	fd	,	&write_value	,	sizeof(int))	;

								bytes	+=	write(	fd	,	&rec_buffer[jj]->ref->attr->size	,	sizeof(int))	;
				}
				total_bytes_written	+=	bytes;
				return(0)	;
}

/**
@details
-#	While	there	is	data	in	memory	that	has	not	been	written	to	disk
			-#	Write	out	each	of	the	other	parameter	values	to	the	temporary	#writer_buff
			-#	Write	#writer_buff	to	the	output	file
-#	return	the	number	of	bytes	written
*/
int	Trick::DRBinary::format_specific_write_data(unsigned	int	writer_offset)	{

	unsigned	long	bf;
	int	sbf;
				unsigned	int	ii	;
				unsigned	int	len	=	0	;
				char	*address	=	0	;

				/*	Write	out	all	parameters	*/
				for	(ii	=	0;	ii	<	rec_buffer.size()	;	ii++)	{

								address	=	rec_buffer[ii]->buffer	+	(	writer_offset	*	rec_buffer[ii]->ref->attr->size	)	;

								switch	(rec_buffer[ii]->ref->attr->type)	{
												case	TRICK_CHARACTER:
												case	TRICK_UNSIGNED_CHARACTER:
												case	TRICK_SHORT:
												case	TRICK_UNSIGNED_SHORT:
												case	TRICK_BOOLEAN:
												case	TRICK_ENUMERATED:
												case	TRICK_INTEGER:
												case	TRICK_UNSIGNED_INTEGER:
												case	TRICK_FLOAT:
												case	TRICK_LONG:
												case	TRICK_UNSIGNED_LONG:
												case	TRICK_LONG_LONG:
												case	TRICK_UNSIGNED_LONG_LONG:
												case	TRICK_STRUCTURED:
												case	TRICK_DOUBLE:
																memcpy(writer_buff	+	len,	address,	(size_t)rec_buffer[ii]->ref->attr->size);
																break;

												case	TRICK_BITFIELD:
																sbf	=	GET_BITFIELD(address,	rec_buffer[ii]->ref->attr->size,
																	rec_buffer[ii]->ref->attr->index[0].start,	rec_buffer[ii]->ref->attr->index[0].size);
																memcpy(writer_buff	+	len,	&sbf,	(size_t)rec_buffer[ii]->ref->attr->size);
																break;

												case	TRICK_UNSIGNED_BITFIELD:
																bf	=	GET_UNSIGNED_BITFIELD(address,	rec_buffer[ii]->ref->attr->size,
																	rec_buffer[ii]->ref->attr->index[0].start,	rec_buffer[ii]->ref->attr->index[0].size);
																memcpy(writer_buff	+	len,	&bf,	(size_t)rec_buffer[ii]->ref->attr->size);
																break;

												default:
																break;
								}
								len	+=	rec_buffer[ii]->ref->attr->size	;

				}

				return	write(	fd	,	writer_buff	,	len)	;
}

/**
@details
-#	Close	the	output	file	stream
*/
int	Trick::DRBinary::format_specific_shutdown()	{

				if	(	inited	)	{
								close(fd)	;
				}
				return(0)	;
}