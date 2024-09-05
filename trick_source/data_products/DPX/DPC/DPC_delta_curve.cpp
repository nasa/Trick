
#include	"DPC/DPC_delta_curve.hh"
#include	"math.h"
#include	"string.h"
#include	<sstream>

//	CONSTRUCTOR
DPC_delta_curve::DPC_delta_curve(	DPM_curve*															Curve_spec,
																																		DPM_run*																	RUN1,
																																		DPM_run*																	RUN2,
																																		DPC_datastream_supplier	*ds_supplier,
																																		DPM_time_constraints				*Time_constraints	)
				:	DPC_curve()
{

				char	work[255];
				char	*temp_s;

				std::string	run1_ds_units;
				std::string	run2_ds_units;

				int	RUN1_case_index;
				int	RUN2_case_index;
				unsigned	int	case_index;
				unsigned	int	number_of_cases	=	Curve_spec->NumberOfVarCases();

				DPM_var	*	run1_x_var;
				DPM_var	*	run1_y_var;
				DPM_var	*	run2_x_var;
				DPM_var	*	run2_y_var;

				time_conversion	=	NULL;

				run_dir1	=	strdup(	RUN1->getDir());
				run_dir2	=	strdup(	RUN2->getDir());

				//	------------------------------------------------------------
				//	The	X-axis	represents	time,	so	this	curve	is	of	type:
				//
				//																						<X,Y>	=	<t,	|A(t)	-	A(t)|	>
				//																																			Run1			Run2
				//	------------------------------------------------------------

				//	For	Delta	Curves,	the	X-variable	must	be	time.
				//	For	RUN1,	determine	which	of	the	X-varcase	variable	names	matches	RUN1's	"time	name"
				//	with	its	related	Y-variable	also	being	found	in	RUN1.
				case_index	=	0;
				RUN1_case_index	=	-1;
				while	((RUN1_case_index	<	0)	&&	(case_index	<	number_of_cases))	{
								run1_x_var	=	Curve_spec->getXVar(case_index);
								if	(	strcmp(	run1_x_var->getName(),	RUN1->getTimeName())	==	0)	{
												run1_y_var	=	Curve_spec->getYVar(case_index);

												//	Create	DataStream	#1	if	Y-variable	is	also	found
												ds[0]	=	ds_supplier->getDataStream(	run1_y_var->getName(),
																																																run1_y_var->AttributeValue("units"),
																																																run1_y_var->AttributeValue("from_units"),
																																																RUN1,
																																																Time_constraints	);
												if	(ds[0]	!=	NULL)	{
																RUN1_case_index	=	case_index;
												}
								}
								case_index++;
				}

				//	If	we	did	not	find	an	X-variable	in	the	varcase	list	that	matches	the
				//	time_name	the	RUN1	or	the	related	Y-variable	then	we	have	an	error	condition.
				if	(	RUN1_case_index	<	0	)	{
								std::cerr	<<	"ERROR:	For	RUN1,	the	X	variable	either	does	not	represent	time,	a	requirement	for	delta	plots\n"
																		<<	"							or	the	Y	variable	could	not	be	found	in	RUN1."
																		<<	std::endl;
								std::cerr.flush();
								throw(std::logic_error("Unable	to	create	DPC_delta_curve."));
				}

				//	For	RUN2,	determine	which	of	the	X-varcase	variable	names	matches	RUN2's	"time	name"
				//	with	its	related	Y-variable	also	being	found	in	RUN2.
				case_index	=	0;
				RUN2_case_index	=	-1;
				while	(	(RUN2_case_index	<	0)	&&	(case_index	<	number_of_cases))	{
								run2_x_var	=	Curve_spec->getXVar(case_index);
								if	(	strcmp(	run2_x_var->getName(),	RUN2->getTimeName())	==	0)	{
												run2_y_var	=	Curve_spec->getYVar(case_index);

												//	Create	DataStream	#1	if	Y-variable	is	also	found
												ds[1]	=	ds_supplier->getDataStream(	run2_y_var->getName(),
																																																run2_y_var->AttributeValue("units"),
																																																run2_y_var->AttributeValue("from_units"),
																																																RUN2,
																																																Time_constraints	);
												if	(ds[1]	!=	NULL)	{
																RUN2_case_index	=	case_index;
												}
								}
								case_index++;
				}

				//	If	we	did	not	find	an	X-variable	in	the	varcase	list	that	matches	the
				//	time_name	the	RUN2	or	the	related	Y-variable	then	we	have	an	error	condition.
				if	(	RUN2_case_index	<	0	)	{
								std::cerr	<<	"ERROR:	For	RUN2,	the	X	variable	does	not	represent	time,	a	requirement	for	delta	plots\n"
																		<<	"							or	the	Y	variable	could	not	be	found	in	RUN2."
																		<<	std::endl;
								std::cerr.flush();
								throw(std::logic_error("Unable	to	create	DPC_delta_curve."));
				}

				//	Create	the	X	DPM_Var
				if	(strcmp(	run1_x_var->getName(),	run2_x_var->getName())	==	0)	{
								delta_x_var	=		new	DPM_var(	run1_x_var->getName());
				}	else	{
								delta_x_var	=		new	DPM_var("time");
				}
				delta_x_var->add_attribute("units",	"s");
				x_var	=	delta_x_var;

				//	Create	the	Y	DPM_Var
				std::string	s	=	"DELTA(";
				s	+=	run1_y_var->getName()	;
				s	+=	")";
				delta_y_var	=		new	DPM_var(	s.c_str());
				y_var	=	delta_y_var;

				time_conversion	=	cv_get_trivial()	;
				x_actual_units	=	strdup("s");

				//	Make	sure	we	know	what	units	we're	actually	getting	from	DataStream	#1.
				run1_ds_units	=	ds[0]->getUnit().c_str();

				//	Make	sure	we	know	what	units	we're	actually	getting	from	DataStream	#2.
				run2_ds_units	=	ds[1]->getUnit().c_str();

				//	ds1	and	ds2	had	better	be	supplying	the	same	units.
				if	(	run1_ds_units	!=	run2_ds_units	)	{
								std::cerr	<<	"ERROR:	The	two	data	streams	of	a	delta	plot	have	different	units."	<<	std::endl;
								std::cerr.flush();
								throw(std::logic_error("Failed	to	create	DataStream"));
				}

				y_actual_units	=	strdup(	run1_ds_units.c_str());

				//	-------------------------------------------------------------
				//	Tell	our	DataStreams	to	start	at	the	beginning	just	because
				//	it	makes	me	feel	all	warm	and	fuzzy.
				//	-------------------------------------------------------------
				ds[0]->begin();
				ds[1]->begin();

				//	------------------------
				//	Save	the	RUN	directorys.
				//	------------------------
				work[0]	='\0';
				if	((temp_s	=	strrchr(run_dir1,'/'))	!=	NULL)	{
								strcpy(work,	temp_s+1);
				}	else	{
								strcpy(work,	run_dir1);
				}
				strcat(work,	"	-	");
				if	((temp_s	=	strrchr(run_dir2,'/'))	!=	NULL)	{
								strcat(work,	temp_s+1);
				}	else	{
								strcat(work,	run_dir2);
				}
				data_src_label	=	strdup(work);
}

//	DESTRUCTOR
DPC_delta_curve::~DPC_delta_curve()	{

				//	We	ONLY	delete	what	we	create.
				delete	delta_x_var;
				delete	delta_y_var;
				delete	ds[0];
				delete	ds[1];
				if	(	x_actual_units	)	{	free(	x_actual_units);	}
				if	(	y_actual_units	)	{	free(	y_actual_units);	}
				if	(	run_dir1	)	{	free(	run_dir1);	}
				if	(	run_dir2	)	{	free(	run_dir2);	}
				if	(	data_src_label	)	{	free(	data_src_label);	}
				if	(	time_conversion	)	{	cv_free(time_conversion);	}
}

//	MEMBER	FUNCTION
const	char*	DPC_delta_curve::getXUnits()	{
				return	(	x_actual_units);
}

//	MEMBER	FUNCTION
const	char*	DPC_delta_curve::getYUnits()	{
				return	(	y_actual_units);
}

//	MEMBER	FUNCTION
const	char	*DPC_delta_curve::getDataSrcLbl()	{
				return(	data_src_label);
}

//	MEMBER	FUNCTION
int	DPC_delta_curve::getXY(double	*X_value,	double	*Y_value)	{

				double	t1,t2,v1,v2;
				int	eos;
				int	e0,	e1;


				e0	=	ds[0]->get(	&t1,	&v1);
				e1	=	ds[1]->get(	&t2,	&v2);
				eos	=	!(e0	&&	e1);

				while	(!eos	&&	(	fabs(	t1	-	t2)	>	T_TOLERANCE	))	{
								if	(t1	<	t2)	{
												eos	=	!	ds[0]->get(	&t1,	&v1);
								}	else	{
												eos	=	!	ds[1]->get(	&t2,	&v2);
								}
				}
				if	(!eos)	{
								*X_value	=	cv_convert_double(time_conversion,t1);
								*Y_value	=	v1	-	v2;
								return(1);
				}

				return(0);
}

//	MEMBER	FUNCTION
void	DPC_delta_curve::begin()	{
				if	(ds[0])	ds[0]->begin();
				if	(ds[1])	ds[1]->begin();
}

