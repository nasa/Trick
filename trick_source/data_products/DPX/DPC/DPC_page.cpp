
#include	"DPC/DPC_page.hh"
#include	<stdio.h>

DPC_page::DPC_page(	DPC_datastream_supplier	*DS_Supplier,
																				std::vector	<DPM_run	*>	*RunListp,
																				DPM_page																*PageSpec,
																				DPM_time_constraints				*ParentTimeConstraints	)
{

				DPM_time_constraints	*my_time_constraints;
				DPM_time_constraints	total_time_constraints;
				DPM_relation	*relation;
				const	char	*presentation;
				int	n_relations,	n_runs;
				int	relix,	runix;
				bool	contrast_session	=	false;

				page_spec	=	PageSpec;
				view_data	=	NULL;

				//	Combine	the	time	constraints	levied	by	the	page	specification
				//	and	those	levied	by	the	parent-page.	The	combined	time	constraints
				//	will	be	levied	on	each	of	the	subordinate	plots.

				my_time_constraints	=	page_spec->getTimeConstraints();
				total_time_constraints	=	*my_time_constraints	+	*ParentTimeConstraints;

				//	------------------------
				//	Create	subordinate	plots.
				//	------------------------
				n_runs	=	(int)RunListp->size();

				n_relations	=	PageSpec->NumberOfRelations();

				presentation	=	PageSpec->AttributeValue("presentation");
				if	((presentation	!=	NULL)	&&	(strcasecmp(	presentation,	"CONTRAST")	==	0))	{
								//	for	a	Contrast	session,	do	a	comparison	plot	and	delta	plot	on	same	page
								contrast_session	=	true;
				}

				if	(	((presentation	!=	NULL)	&&	(strcasecmp(	presentation,	"COMPARISON")	==	0))	||
									(contrast_session)	)	{
								//	Foreach	of	the	Variable	Relations.
								for	(relix	=	0	;	relix	<	n_relations	;	relix++	)	{
												relation	=	PageSpec->getRelation(	relix	);
												DPC_plot	*plot;
												try	{
																//	Make	a	plot.
																plot	=	new	DPC_standard_plot(	DS_Supplier,
																																														RunListp,
																																														relation,
																																														&total_time_constraints	);
																if	(contrast_session)	{
																				plot->setTitle("Contrast");
																}
																//	Put	the	new	plot	in	the	plot	list.
																plot_list.push_back(	plot	);
												}	catch	(const	std::invalid_argument&	error)	{
																std::cerr	<<	error.what()	<<	std::endl;
												}
								}
				}
				if	(	((presentation	!=	NULL)	&&	(strcasecmp(	presentation,	"DELTA")	==	0))	||
									(contrast_session)	)	{
								//	Foreach	of	the	Variable	Relations.
								for	(relix	=	0	;	relix	<	n_relations	;	relix++	)	{
												relation	=	PageSpec->getRelation(	relix	);
												DPC_plot	*plot;
												int	i,j;

												//	For	Contrast	session,	put	all	DELTA	curves	for	all	runs	on	one	plot
												if	(contrast_session)	{
																try	{
																				//	Make	a	plot.
																				plot	=	new	DPC_delta_plot(	DS_Supplier,
																																															RunListp,
																																															relation,
																																															&total_time_constraints	);
																				//	Put	the	new	plot	in	the	plot	list.
																				plot->setTitle("Contrast");
																				plot_list.push_back(	plot	);
																}	catch	(const	std::invalid_argument&	error)	{
																				std::cerr	<<	error.what()	<<	std::endl;
																}
												}	else	{
												//	For	Delta	session,	create	a	DELTA	plot	for	each	pairing	of	runs.
																for	(i=0;	i<n_runs-1;	i++)	{
																				for	(j=i+1;	j<n_runs;	j++)	{

																								DPM_run	*Run1	=	(*RunListp)[i];
																								DPM_run	*Run2	=	(*RunListp)[j];
																								try	{
																												//	Make	a	plot.
																												plot	=	new	DPC_delta_plot(	DS_Supplier,
																																																							Run1,
																																																							Run2,
																																																							relation,
																																																							&total_time_constraints	);
																												//	Put	the	new	plot	in	the	plot	list.
																												plot_list.push_back(	plot	);
																								}	catch	(const	std::invalid_argument&	error)	{
																												std::cerr	<<	error.what()	<<	std::endl;
																								}

																				}	//	end	j
																}	//	end	i
												}
								}	//	end	relix
				}

				if	(	plot_list.size()	==	0	)	{
								if	((presentation	==	NULL)	||		(strcasecmp(	presentation,	"SIMPLE")	!=	0))	{
												std::cerr	<<	"WARNING:	Presentation	not	correctly	specified.";
												std::cerr	<<	"	Defaulting	Presentation	to	SIMPLE."	<<	std::endl;
								}
								for	(relix	=	0	;	relix	<	n_relations	;	relix++	)	{
												relation	=	PageSpec->getRelation(	relix	);
												for	(runix	=	0	;	runix	<	n_runs	;	runix++	)	{
																DPC_plot	*plot;
																//	Create	a	list	of	ONE	run_directory.
																std::vector	<DPM_run	*>	*run_list_of_one	=	new	std::vector	<DPM_run	*>	;
																run_list_of_one->push_back(	(*RunListp)[runix]	);	//	FIXME	Does	this	ever	get	deleted?
																try	{
																				//	Make	a	plot.
																				plot	=	new	DPC_standard_plot(	DS_Supplier,
																																																		run_list_of_one,
																																																		relation,
																																																		&total_time_constraints	);
																				//	Put	the	new	plot	in	the	plot	list.
																				plot_list.push_back(	plot	);
																}	catch	(const	std::invalid_argument&	error)	{
																				std::cerr	<<	error.what()	<<	std::endl;
																}
												}
								}
				}

				if	(	plot_list.size()	==	0	)	{
								throw	std::invalid_argument("ERROR:	Page	does	not	contain	any	plots.")	;
				}
}

//	DESTRUCTOR
DPC_page::~DPC_page()	{
				DPC_plot	*plot;
				int	n_plots,	i;

				n_plots	=	(int)plot_list.size();
				for	(i=0	;	i<n_plots	;	i++	)	{
								plot	=	plot_list[i];
								if	(plot)	{	delete	plot;	}
				}
}

//	MEMBER	FUNCTION
DPV_pointer	DPC_page::render(	DPV_view	*view,
																														DPV_pointer	parent_view_data	)	{

				if	(view	!=	NULL)	{
								DPC_plot		*plot;
								int	i,	n_plots;

								view_data	=	view->render_page(	parent_view_data,	this);

								//	Call	the	render	member	function	of	each	of	the	plot	children.
								n_plots	=	(int)plot_list.size();

								for	(i=0	;	i<n_plots	;	i++	)	{
												plot	=	plot_list[i];
												plot->render(	view,	view_data);
								}

								view->finalize_page_view(view_data);

				}	else	{
								view_data	=	NULL;
				}
				return	(view_data);
}

//	MEMBER	FUNCTION
const	char	*DPC_page::getAttribute(const	char	*key)	{
				return(	page_spec->AttributeValue(key)	);
}

//	MEMBER	FUNCTION
void	DPC_page::notify(	DPV_view	*view,	DPV_message	msg)	{

				if	(view_data	!=	NULL)	{
								view->notify_page(	view_data,	msg);
				}
}

//	MEMBER	FUNCTION
const	char	*DPC_page::getTitle()	{
				return(	page_spec->getTitle()	);
}

