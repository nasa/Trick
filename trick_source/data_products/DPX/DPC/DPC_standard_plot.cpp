
#include	"DPC/DPC_standard_plot.hh"
#include	"DPC/DPC_std_curve.hh"
#include	"DPM/DPM_var.hh"
#include	"DPM/DPM_axis.hh"

//	CONSTRUCTOR
DPC_standard_plot::DPC_standard_plot(
																																					DPC_datastream_supplier	*DS_Supplier,
																																					std::vector	<DPM_run	*>	*RunListp,
																																					DPM_relation												*Relation,
																																					DPM_time_constraints				*ParentTimeConstraints	)
				:	DPC_plot	(Relation)	{

				DPM_time_constraints	*my_time_constraints;
				DPM_time_constraints	total_time_constraints;
				int	n_runs,	n_curves;
				int	cix,	runix;

				if	(	!DS_Supplier	)											{	throw	std::invalid_argument("DS_Supplier	is	NULL.");	}
				if	(	!RunListp	)														{	throw	std::invalid_argument("RunListp	is	NULL.");	}
				if	(	!Relation	)														{	throw	std::invalid_argument("Relation	is	NULL.");	}
				if	(	!ParentTimeConstraints	)	{	throw	std::invalid_argument("ParentTimeConstraints	is	NULL.");	}

				//	Combine	the	time	constraints	levied	by	the	plot	specification
				//	and	those	levied	by	the	parent-page.	The	combined	time	constraints
				//	will	be	levied	on	each	of	the	subordinate	curves.

				my_time_constraints	=	Relation->getTimeConstraints();
				total_time_constraints	=	*my_time_constraints	+	*ParentTimeConstraints;

				n_runs	=	(int)RunListp->size();

				n_curves	=	Relation->NumberOfCurves();

				//	for	each	of	the	RUNs
				for	(runix	=	0	;	runix	<	n_runs	;	runix++)	{
								//	for	each	of	the	curves
								for	(cix	=	0	;	cix	<	n_curves	;	cix	++	)	{
												DPC_curve	*curve;
												DPM_curve	*curve_spec	=	Relation->getCurve(cix);
												try	{
																//	Make	a	curve.
																curve	=	new	DPC_std_curve(	curve_spec,
																																											(*RunListp)[runix],
																																											DS_Supplier,
																																											&total_time_constraints	);
												}	catch	(const	std::logic_error&	error)	{
																curve	=	NULL;
																std::cerr	<<	error.what()	<<	std::endl;
												}

												//	Put	that	new	curve	object	into	the	curve	list.
												if	(	curve)	{
																if	(	add_curve(	curve)	<	0	)	{
																				std::cerr	<<	"ERROR:	Rejecting	curve."	<<	std::endl;
																				delete	curve;
																}
												}
								}
				}

				//	VALIDATION
				//	1.	We	must	have	added	at	least	1	valid	curve	to	the	list.
				//	2.	If	we	have	more	than	one	curve,	all	of	the	ACTUAL	units	must	match.
				if	(	getNumCurves()	<=	0)	{
								throw	std::invalid_argument("ERROR:	DPC_plot	does	not	contain	any	valid	curves.");
				}

}

