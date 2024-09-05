
#include	<udunits2.h>
#include	"DPC/DPC_table.hh"
#include	"DPM/DPM_var.hh"
#include	"DPM/DPM_column.hh"

extern	ut_system	*	u_system	;

static	const	char	*default_time_units	=	"s";

//	CONSTRUCTOR
DPC_table::DPC_table(	DPC_datastream_supplier	*DS_Supplier,
																						DPM_run																	*Run,
																						DPM_table															*Table_spec,
																						DPM_time_constraints				*ParentTimeConstraints	)
					{

				DPM_time_constraints	*my_time_constraints;
				DPM_time_constraints	total_time_constraints;
				DPM_var	*column_var;

				//const	char		*actual_units;
				int	n_columns,	n_datastreams;
				int	colix;

				//	###############################################################
				//	Initialize	this	table	object.
				//	###############################################################
				if	(	!DS_Supplier	)	{	throw	std::invalid_argument("DS_Supplier	is	NULL");	}
				if	(	!Run									)	{	throw	std::invalid_argument("DPM_run	is	NULL");	}
				if	(	!Table_spec		)	{	throw	std::invalid_argument("DPM_table	is	NULL");	}

				table_spec	=	Table_spec;
				view_data	=	NULL;
				run	=	Run;

				//	###############################################################
				//	Determine	our	time	constraints.
				//	###############################################################

				//	Combine	the	time	constraints	levied	by	the	table	specification
				//	and	those	levied	by	the	parent-page.	The	combined	time	constraints
				//	will	be	levied	on	each	of	the	subordinate	columns.

				my_time_constraints	=	table_spec->getTimeConstraints();
				if	(ParentTimeConstraints	!=	NULL)	{
								total_time_constraints	=	*my_time_constraints	+	*ParentTimeConstraints;
				}	else	{
								total_time_constraints	=	*my_time_constraints;
				}

				//	#######################################################
				//	Create	the	necessary	DataStreams,	one	for	each	column	.
				//	#######################################################

				//	Get	information	about	the	variable(s)	for	this	table.

				n_columns	=	Table_spec->NumberOfColumns();

				for	(	colix	=	0	;	colix	<	n_columns	;	colix	++	)	{
								DataStream	*	ds;
								DPC_column_info	*	column_info;
								DPM_column	*	column	=	Table_spec->getColumn(	colix	);
		const	char	*	actual_units;

								column_var	=	column->getVar();
								const	char	*	column_var_name	=	column_var->getName();
								const	char	*	column_var_units	=	column_var->AttributeValue("units");
								const	char	*	column_var_from_units	=	column_var->AttributeValue("from_units");

								cv_converter	*time_conversion	=	NULL;
								int	dsix	=	-1;

								if	(	strcmp(	column_var_name,	run->getTimeName()	)	==	0	)	{
												if	((column_var_units	!=	NULL)	&&	(strcmp(	column_var_units,	default_time_units)	!=	0))	{
																//	Convert	seconds	to	whatever	the	user	requested.
																ut_unit	*to;
																ut_unit	*from;

																to	=	ut_parse(u_system,	column_var_units,	UT_ASCII)	;
																if	(	!	to	)	{
																				std::cerr	<<	"ERROR:	Unable	to	convert	values	to	invalid	units:	\""
																									<<	column_var_units	<<	"\"."	<<	std::endl;
																}
																from	=	ut_parse(u_system,	default_time_units,	UT_ASCII)	;

																if	(to	&&	from)	{
																				actual_units	=	column_var_units;
																				time_conversion	=	ut_get_converter(from,to)	;
																				if	(	time_conversion	==	NULL	)	{
																								std::cerr	<<	"ERROR:	Unable	to	convert	from	\""	<<	default_time_units	<<	"\"	to	\""
																												<<	column_var_units	<<	"\"	because	they	are	incompatible."	<<	std::endl;
																								time_conversion	=	cv_get_trivial()	;
																								actual_units	=	default_time_units;
																				}
																				ut_free(to)	;
																				ut_free(from)	;
																}	else	{
																				time_conversion	=	cv_get_trivial()	;
																				actual_units	=	default_time_units;
																}
												}	else	{
																time_conversion	=	cv_get_trivial()	;
																actual_units	=	default_time_units;
												}

								}	else	{

												ds	=	DS_Supplier->getDataStream(	column_var_name,
																																													column_var_units,
																																													column_var_from_units,
																																													Run,
																																													&total_time_constraints	);
												if	(ds	==	NULL)	{
													std::cerr	<<	"ERROR:	Unable	to	create	a	DataStream	for	:"	<<	std::endl	<<
																				"			Var	name	=	\""						<<	column_var_name	<<	"\""	<<	std::endl	<<
																				"			Run	Directory	=	\""	<<	run->getDir()	<<	"\""	<<	std::endl;
																std::cerr.flush();
																throw	std::invalid_argument("ERROR:	Bogus	Table.");
												}
												datastream_list.push_back(	ds	);
												actual_units	=	ds->getUnit().c_str();
			dsix	=	datastream_list.size()	-	1;
								}
								column_info	=	new	DPC_column_info(	column_var,	actual_units,	dsix,	time_conversion);
		column_info_list.push_back(	column_info	);
				}

				n_datastreams	=	(int)datastream_list.size();

				if	(	n_datastreams	==	0	)	{
								throw	(std::logic_error("DataStreamList	is	empty."));
				}

				dsData	=	new	DPC_datum[n_datastreams];
				end_of_stream	=	new	bool[n_datastreams];
				synchronized_values	=	new	double[n_datastreams];

				begin();
}

//	DESTRUCTOR
DPC_table::~DPC_table()	{

				DataStream	*ds;
				int	n_ds,	ds_ix;

				//	This	class	didn't	create	the	objects	referenced	by:
				//	(	table_spec,	view_data	and	run)
				//	so	it's	not	allowed	to	delete	them:

				n_ds	=	(int)datastream_list.size();

				for	(	ds_ix	=	0;	ds_ix	<n_ds	;	ds_ix++	)	{
								ds	=	datastream_list[ds_ix];
								if	(ds)	{	delete	ds;	}
				}

				if	(dsData)	{	delete	dsData;	}
}

//	MEMBER	FUNCTION
DPV_pointer	DPC_table::render(	DPV_view	*view,
																															DPV_pointer	parent_view_data	)	{

				if	(view	!=	NULL)	{
								view_data	=	view->render_table(	parent_view_data,	this);
								view->finalize_table_view(view_data);
				}	else	{
								view_data	=	NULL;
				}
				return	(view_data);
}

void	DPC_table::notify(	DPV_view	*view,	DPV_message	msg	)	{
				if	(view_data	!=	NULL)	{
								view->notify_table(	view_data,	msg);
				}
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getRunDir()	{
				return(	run->getDir());
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getTitle()	{
				return(	table_spec->getTitle());
}

//	MEMBER	FUNCTION
int	DPC_table::getNumColumns()	{
				return(	table_spec->NumberOfColumns());
}

//	MEMBER	FUNCTION
std::string	DPC_table::getColumnLabel(unsigned	int	index)	{
				DPM_column	*column	=	table_spec->getColumn(	index	);
				if	(	column->getLabel()	!=	NULL)	{
								return(	column->getLabel());
				}	else	{
								return(	column->getVar()->getShortName()	);
				}
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getColumnVarName(unsigned	int	index)	{

				return(	table_spec->getColumn(	index)->getVar()->getName());
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getColumnUnits(unsigned	int	index)	{
				if	(	index	<	column_info_list.size())	{
		//std::cout	<<	column_info_list[index]->units	<<	std::endl;
								return	(	column_info_list[index]->units);
				}
				return	(NULL);
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getColumnAttribute(unsigned	int	index,
																																										const	char	*key)	{

				return(	table_spec->getColumn(	index)->AttributeValue(	key));
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getVarAttribute(unsigned	int	index,	const	char	*key)	{
				return(	table_spec->getColumn(index)->getVar()->AttributeValue(key)	);
}

//	MEMBER	FUNCTION
int	DPC_table::getRow(double	*time,	double	*row_values)	{

				int	ds_ix,	col_ix;
				int	n_columns	=	table_spec->NumberOfColumns();
				int	n_datastreams	=	(int)datastream_list.size();
				int	min_time_ds_ix,	max_time_ds_ix;

				bool	end_of_table	=	true;
				for	(ds_ix	=	0;	ds_ix	<	n_datastreams	;	ds_ix	++)	{
								if	(	end_of_stream[ds_ix]	==	false	)	{
												if	(	end_of_table	)	{
																min_time_ds_ix	=	max_time_ds_ix	=	ds_ix;
																end_of_table	=	false;
												}	else	{
																if	(dsData[ds_ix].time		<	dsData[min_time_ds_ix].time	){
																				min_time_ds_ix	=	ds_ix;
																}	else	if	(dsData[ds_ix].time		>	dsData[max_time_ds_ix].time	){
																				max_time_ds_ix	=	ds_ix;
																}
												}
								}
				}

				if	(	end_of_table	==	false	)	{

								*time	=	dsData[min_time_ds_ix].time;

								if	(	(dsData[max_time_ds_ix].time	-	dsData[min_time_ds_ix].time)	<=	T_TOLERANCE	)	{
												//	The	times	for	each	datastream	are	the	same	(within	a	tolerance)
												//	so	output	all	of	the	current	values,	and	get	new	ones.

												for	(ds_ix	=	0	;ds_ix	<	n_datastreams	;	ds_ix++	)	{
																synchronized_values[ds_ix]	=	dsData[ds_ix].value;
																end_of_stream[ds_ix]	=	!datastream_list[ds_ix]->get(	&dsData[ds_ix].time,	&dsData[ds_ix].value);
												}

								}	else	{
												//	The	times	for	each	column	are	not	the	same	so	output/update
												//	only	the	values	with	the	min	time	and	those	values	within
												//	TOLERANCE	of	the	min	time.	For	those	values	output,	get	new
												//	values	from	the	datastream.

												synchronized_values[min_time_ds_ix]	=	dsData[min_time_ds_ix].value;

												for	(ds_ix	=	0	;ds_ix	<	n_datastreams	;	ds_ix++	)	{
																if	(	ds_ix	!=	min_time_ds_ix)	{
																				if	((dsData[ds_ix].time	-	dsData[min_time_ds_ix].time)	<=	T_TOLERANCE	)	{
																								synchronized_values[ds_ix]	=	dsData[ds_ix].value;
																								end_of_stream[ds_ix]	=	!datastream_list[ds_ix]->get(	&dsData[ds_ix].time,	&dsData[ds_ix].value);
																				}
																}
												}
								}

								for	(col_ix	=	0	;	col_ix	<	n_columns	;	col_ix++	)	{
												const	char		*column_var_name;
												column_var_name	=	column_info_list[col_ix]->var->getName();
												if	(	strcmp(	column_var_name,	run->getTimeName())	==	0)	{
															row_values[col_ix]	=	cv_convert_double(column_info_list[col_ix]->time_conversion,	*time);
												}	else	{
															row_values[col_ix]	=	synchronized_values[	column_info_list[col_ix]->ds_ix	];
												}
								}
				}

				return	(!end_of_table);
}

//	MEMBER	FUNCTION
void	DPC_table::begin()	{
				int	ds_ix;
				int	n_ds	=	(int)datastream_list.size();

				for	(	ds_ix	=	0;	ds_ix	<n_ds	;	ds_ix++	)	{
								datastream_list[ds_ix]->begin();
				}

				for	(ds_ix	=	0	;ds_ix	<	n_ds	;	ds_ix++	)	{
								end_of_stream[ds_ix]	=	!datastream_list[ds_ix]->get(	&dsData[ds_ix].time,	&dsData[ds_ix].value);
				}
}

//	MEMBER	FUNCTION
const	char	*DPC_table::getAttribute(const	char	*key)	{
				return(	table_spec->AttributeValue(key)	);
}
