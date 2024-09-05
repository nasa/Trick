
#include	<stdlib.h>
#include	<string.h>
#include	<strings.h>
#include	<math.h>
#include	"TrickHDF5.hh"
#include	"trick/map_trick_units_to_udunits.hh"

TrickHDF5::TrickHDF5(char	*file_name	,	char	*parameter_name	,	char	*time_name)	{

				packet_index	=	0;

				hid_t	header_group,	parameter_names,	parameter_units;
				hsize_t	header_packet_index;
				hsize_t	header_num_packets;

				fileName_	=	file_name	;
				unitTimeStr_	=	time_name	;

				/*!
					*	Open	the	HDF5	file.
					*/
				if	(	(file	=	H5Fopen(	file_name,	H5F_ACC_RDONLY,	H5P_DEFAULT	))	<	0	)	{
								cerr	<<	"ERROR:		Couldn't	open	file	\""	<<	file_name	<<	"\""	<<	endl;
								exit(-1);
				}

				/*!
					*	Open	all	dataset/packet-table	groups.
					*	Every	HDF5	file	has	at	least	a	root	"/"	group.
					*/
				root_group	=	H5Gopen(	file,	"/",	H5P_DEFAULT	);
				if	(	root_group	<	0	)	{
								cerr	<<	"ERROR:		Couldn't	open	group	\""	<<		"/"	<<	"\""	<<	endl;
								exit(-1);
				}

				//	Open	"/header"	group	for	the	purpose	of	getting	the	units	of	the	parameter
				header_group	=	H5Gopen(	file,	"/header",	H5P_DEFAULT	);

				//	Find	the	units	for	specified	parameter_name	in	recorded	data
				if	(	header_group	>=	0	)	{
								parameter_names	=	H5PTopen(	header_group,	"param_names"	);
								parameter_units	=	H5PTopen(	header_group,	"param_units"	);

								if	(H5PTis_valid(parameter_names)	>=	0	&&		H5PTis_valid(parameter_units)	>=	0)	{
												H5PTget_num_packets(parameter_names,	&header_num_packets);

												char	name_buf[1024];
												char	units_buf[1024];

												for	(header_packet_index	=	0;	header_packet_index	<	header_num_packets;	header_packet_index++)	{
																H5PTget_next(parameter_names,	1,	name_buf);
																H5PTget_next(parameter_units,	1,	units_buf);
																//	the	specified	parameter_name	is	found,	set	the	units
																if	(strcmp(parameter_name,	name_buf)	==	0)	{
																				if	(	!strcmp(units_buf,"--")	)	{
																								unitStr_	=	strdup(units_buf)	;
																				}	else	{
																								unitStr_	=	map_trick_units_to_udunits(units_buf)	;
																				}
																				break;
																}
												}

												H5PTclose(parameter_names);
												H5PTclose(parameter_units);
								}

								H5Gclose(header_group);
				}

				/*!
					*	Open	datasets(D)/packet-tables(PT).
					*	"parameter_dataset"	is	the	recorded	data	for	the	specified	parameter.
					*	"time_dataset"	is	the	actual	recorded	data	for	"sys.exec.out.time".
					*/
				parameter_dataset	=	H5PTopen(	root_group,	parameter_name	);
				if	(	H5PTis_valid(parameter_dataset)	<	0	)	{
								cerr	<<	"ERROR:		Couldn't	open	packet	table	\""
													<<	parameter_name	<<	"\""	<<	endl;
								exit(-1);
				}
				time_dataset	=	H5PTopen(	root_group,	time_name	);
				if	(	H5PTis_valid(time_dataset)	<	0	)	{
								cerr	<<	"ERROR:		Couldn't	open	packet	table	\""
													<<	time_name	<<	"\""	<<	endl;
								exit(-1);
				}

}

TrickHDF5::~TrickHDF5()	{
				//!	End	access	to	all	open	packet	tables.
				H5PTclose(time_dataset);
				H5PTclose(parameter_dataset);
				//!	End	access	to	all	dataset/packet-table	groups.
				H5Gclose(root_group);
				//!	Terminate	access	to	the	HDF5	file.
				H5Fclose(file);
}

int	TrickHDF5::get(double	*time,	double	*value)	{

				int	ret;

				if	(	packet_index	<	num_packets	)	{
								/*!	Retrieve	a	param	value	(plus	corresponding	time)
									*		from	the	current	packet	index	position.	*/
								H5PTget_next(parameter_dataset,	1,	value);
								H5PTget_next(time_dataset,						1,	time);

								//!	Advance	the	packet	index.
								ret	=	this->step();

								//!	Is	there	more	data	to	read?
								return	(ret);
				}	else	{
								//!	No	more	data	to	read
								return	(0);
				}

}

int	TrickHDF5::peek(	double	*	time	,	double	*	value	)	{

				int	ret	;

				ret	=	get(	time	,	value	)	;

				return(ret)	;
}

void	TrickHDF5::begin()	{

				//!	Reset	the	dataset	if	another	data	pass	is	needed.
				packet_index	=	0;

				/*!	See	how	many	packets	were	logged	for	this	parameter.
					*		Each	recorded	value	is	represented	by	one	packet.	*/
				H5PTget_num_packets(	parameter_dataset,	&num_packets	);

				/*!	Before	beginning	to	read	values	(packets)	from	a	packet	table,
					*		reset	the	packet	table's	index	to	point	to	the	first	packet.
					*		(Note:	the	packets	are	zero-indexed.)
					*/
				H5PTcreate_index(	time_dataset	);
				H5PTcreate_index(	parameter_dataset	);

				return	;
}

int	TrickHDF5::end()	{

				//!	Move	packet	index	to	the	end	of	the	packet	table.
				H5PTset_index(	time_dataset,	num_packets	);
				H5PTset_index(	parameter_dataset,	num_packets	);
				packet_index	=	num_packets;

				return	(1);
}

int	TrickHDF5::step()	{

				if	(	packet_index	<	num_packets	)	{
								//!	Increment	the	packet	table's	index.
								packet_index++;
								/*!	Set	the	packet	table's	index.		Each	packet	table	keeps	an	index
									*		of	its	"current"	packet	so	that	get_next	can	iterate	through
									*		the	packets	in	order.	*/
								H5PTset_index(	time_dataset,	packet_index	);
								H5PTset_index(	parameter_dataset,	packet_index	);

								return	(1);

				}	else	{
								//!	No	more	data	to	read
								return	(0);
				}
}


int	HDF5LocateParam(	const	char	*	file_name,	const	char	*	parameter_name	)	{

				//!	Open	an	existing	HDF5	file	and	get	a	file	identifier.
				hid_t	file	=	H5Fopen(file_name,	H5F_ACC_RDONLY,	H5P_DEFAULT);
				if	(file	<	0)	{
								cerr	<<	"ERROR:		Couldn't	open	file	\""	<<	file_name	<<	"\""	<<	endl;
								return	0;
				}

				//!	Open	group	containing	data.
				hid_t	group	=	H5Gopen(file,	"/",	H5P_DEFAULT);
				if	(group	<	0)	{
								cerr	<<	"ERROR:		Couldn't	open	group	\""	<<		"/"	<<	"\""	<<	endl;
								return	0;
				}

				/*!	Open	an	existing	HDF5	packet	table.
					*	PARAMETERS:
					*					IN:	Identifier	of	the	file/group	which	the	packet	table	can	be	found.
					*					IN:	The	name	of	the	group/packet-table	to	open.
					*	RETURN:
					*					Returns	an	identifier	for	the	group/packet-table,	or	H5I_BADID	on	error.
					*/
				hid_t	packet_table	=	H5PTopen(group,	parameter_name);
				if	(	H5PTis_valid(packet_table)	<	0	)	{
								cerr	<<	"ERROR:		Couldn't	open	packet	table	\""
													<<	parameter_name	<<	"\""	<<	endl;
								return	0;
				}

				//!	Close	and	end	access	to	packet	table,	group,	and	file
				H5PTclose(packet_table);
				H5Gclose(group);
				H5Fclose(file);

				return	1;

}
/*!	Overloaded	function	*/
int	HDF5LocateParam(	string	file_name	,	string	param_name	)	{
				return(	HDF5LocateParam(file_name.c_str(),	param_name.c_str())	)	;
}
