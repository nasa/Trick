
#include	"DPC/DPC_delta_plot.hh"
#include	"DPC/DPC_delta_curve.hh"
#include	"DPM/DPM_var.hh"
#include	"DPM/DPM_axis.hh"
#include	"DPM/DPM_run.hh"

//	CONSTRUCTOR	for	Delta	session
//	One	delta	curve	per	plot
DPC_delta_plot::DPC_delta_plot(	DPC_datastream_supplier	*DS_Supplier,
																																DPM_run	*Run1,
																																DPM_run	*Run2,
																																DPM_relation												*Relation,
																																DPM_time_constraints				*ParentTimeConstraints	)
				:	DPC_plot(	Relation)
{

				DPM_time_constraints	*my_time_constraints;
				DPM_time_constraints	total_time_constraints;
				int	n_curves;
				int	cix;

				//	###############################################################
				//	Initialize	this	plot	object.
				//	###############################################################
				if	(	!DS_Supplier	)											{	throw	std::invalid_argument("DS_Supplier	is	NULL.");	}
				if	(	!Run1	)																		{	throw	std::invalid_argument("Run1	is	NULL.");	}
				if	(	!Run2	)																		{	throw	std::invalid_argument("Run2	is	NULL.");	}
				if	(	!Relation	)														{	throw	std::invalid_argument("Relation	is	NULL.");	}
				if	(	!ParentTimeConstraints	)	{	throw	std::invalid_argument("ParentTimeConstraints	is	NULL.");	}

				//	###############################################################
				//	Determine	our	time	constraints.
				//	###############################################################

				//	Combine	the	time	constraints	levied	by	the	plot	specification
				//	and	those	levied	by	the	parent-page.	The	combined	time	constraints
				//	will	be	levied	on	each	of	the	subordinate	curves.

				my_time_constraints	=	Relation->getTimeConstraints();
				total_time_constraints	=	*my_time_constraints	+	*ParentTimeConstraints;

				//	###############################################################
				//	Create	subordinate	curves.
				//	###############################################################

				n_curves	=	Relation->NumberOfCurves();

				//	foreach	of	the	Y	variables
				for	(cix	=	0	;	cix	<	n_curves	;	cix	++	)	{
								DPC_curve	*curve;
								DPM_curve	*curve_spec	=	Relation->getCurve(cix);
								try	{
												//	Make	a	curve.
												curve	=	new	DPC_delta_curve(	curve_spec,
																																									Run1,
																																									Run2,
																																									DS_Supplier,
																																									&total_time_constraints	);
								}	catch	(const	std::logic_error&	error)	{
												curve	=	NULL;
												std::cerr	<<	error.what()	<<	std::endl;
												std::cerr	<<	"DPC_delta_plot:	Alas!	The	DPC_delta_curve	has	poofed!."	<<	std::endl;
								}
								//	Put	that	new	curve	object	into	the	curve	list.
								if	(	curve)	{
												if	(	add_curve(	curve)	<	0	)	{
																std::cerr	<<	"ERROR:	Rejecting	curve."	<<	std::endl;
																delete	curve;
												}
								}
				}

				if	(	getNumCurves()	==	0	)	{
								throw	std::invalid_argument("DPC_delta_plot:	Ack!!	I	dont	have	any	curves	to	plot!	I	have	no	reason	to	exist!	*POOF!*");
				}
}

//	CONSTRUCTOR
DPC_delta_plot::DPC_delta_plot(	DPC_datastream_supplier	*DS_Supplier,
																																std::vector	<DPM_run	*>	*RunListp,
																																DPM_relation												*Relation,
																																DPM_time_constraints				*ParentTimeConstraints	)
				:	DPC_plot(	Relation)
{

				DPM_time_constraints	*my_time_constraints;
				DPM_time_constraints	total_time_constraints;
				int	n_curves,	n_runs;
				int	cix,	i,	j;

				//	Validate	Arguments
				if	(	!DS_Supplier	)											{	throw	std::invalid_argument("DS_Supplier	is	NULL.");	}
				n_runs	=	(int)RunListp->size();
				if	(	n_runs==0)															{	throw	std::invalid_argument("Run	List	is	NULL.");	}
				if	(	!Relation	)														{	throw	std::invalid_argument("Relation	is	NULL.");	}
				if	(	!ParentTimeConstraints	)	{	throw	std::invalid_argument("ParentTimeConstraints	is	NULL.");	}

				//	Determine	our	time	constraints.
				//	Combine	the	time	constraints	levied	by	the	plot	specification
				//	and	those	levied	by	the	parent-page.	The	combined	time	constraints
				//	will	be	levied	on	each	of	the	subordinate	curves.

				my_time_constraints	=	Relation->getTimeConstraints();
				total_time_constraints	=	*my_time_constraints	+	*ParentTimeConstraints;

				//	Create	subordinate	curves.

				n_curves	=	Relation->NumberOfCurves();

				//	foreach	of	the	Y	variables
				for	(cix	=	0	;	cix	<	n_curves	;	cix	++	)	{
								DPC_curve	*curve;
								DPM_curve	*curve_spec	=	Relation->getCurve(cix);
								//	Create	a	DELTA	curve	for	each	pairing	of	runs.
								for	(i=0;	i<n_runs-1;	i++)	{
												for	(j=i+1;	j<n_runs;	j++)	{

																DPM_run	*Run1	=	(*RunListp)[i];
																DPM_run	*Run2	=	(*RunListp)[j];
																try	{
																				//	Make	a	curve.
																				curve	=	new	DPC_delta_curve(	curve_spec,
																																																	Run1,
																																																	Run2,
																																																	DS_Supplier,
																																																	&total_time_constraints	);
																}	catch	(const	std::logic_error&	error)	{
																				curve	=	NULL;
																				std::cerr	<<	error.what()	<<	std::endl;
																				std::cerr	<<	"DPC_delta_plot:	Alas!	The	DPC_delta_curve	has	poofed!."	<<	std::endl;
																}
																//	Put	that	new	curve	object	into	the	curve	list.
																if	(	curve)	{
																				if	(	add_curve(	curve)	<	0	)	{
																								std::cerr	<<	"ERROR:	Rejecting	curve."	<<	std::endl;
																								delete	curve;
																				}
																}
												}	//	end	j
								}	//	end	i
				}	//	end	cix

				if	(	getNumCurves()	==	0	)	{
								throw	std::invalid_argument("DPC_delta_plot:	Ack!!	I	dont	have	any	curves	to	plot!	I	have	no	reason	to	exist!	*POOF!*");
				}
}

