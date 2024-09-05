
#include	<iostream>
#include	<string>

#include	<Xm/Xm.h>

extern	"C"{
#include	"XY.h"
}

#include	<stdio.h>				//	for	snprintf
#include	<stdlib.h>			//	for	atoi
#include	<stdarg.h>			//	for	varargs	stuff
#include	<strings.h>

#include	"fermi_view.hh"
#include	"parse_format.h"

//	CONSTRUCTOR
fermi_view::fermi_view(Widget	TopLevel)	{
				char	applresdir[1024];
				char	*trick_home;

				toplevel	=	TopLevel;
				display		=	XtDisplay(toplevel);
				screen			=	XtScreen(toplevel);
				colormap	=	DefaultColormapOfScreen(screen);
				xpos			=	400;
				ypos			=	50;
				width		=	600;
				height	=	700;

				trick_home	=	getenv("TRICK_HOME");
				if	(trick_home	!=	NULL)	{
									snprintf(	applresdir,sizeof(applresdir),	"%s/trick_source/data_products/DPX/APPS/FXPLOT",trick_home);
									setenv("XAPPLRESDIR",	applresdir,	0);
				}	else	{
									std::cerr	<<	"ERROR:	$TRICK_HOME	is	not	set."	<<	std::endl;
				}
}

//	DESTRUCTOR
fermi_view::~fermi_view()	{
				ProductViewNode	*pvnode;
				int	n_pvnode,	i;

				n_pvnode	=	(int)product_node_list.size();
				for	(i=0	;	i<n_pvnode	;	i++	)	{
								pvnode	=	product_node_list[i];
								if	(pvnode)	{	delete	pvnode;	}
				}
}

//	MEMBER	FUNCTION
DPV_pointer	fermi_view::render_product(	DPC_product*	product)	{

				ProductViewNode	*product_view_node	=	new	ProductViewNode(toplevel,	product);

				product_node_list.push_back(	product_view_node);

				return	(	(DPV_pointer)product_view_node);
}


//	MEMBER	FUNCTION
DPV_pointer	fermi_view::render_page(	DPV_pointer	parent_data,
																																					DPC_page*	page)	{
				PageViewNode	*page_view_node;
				ProductViewNode	*product_view_node	=	(ProductViewNode	*)parent_data;


				if	((xpos	+	width)	>=	WidthOfScreen(	XtScreen(toplevel)))	{
						xpos	=	100;
				}
				if	((ypos	+	height)	>=	HeightOfScreen(	XtScreen(toplevel)))	{
						ypos	=	100;
				}

				page_view_node	=	new	PageViewNode(toplevel,	page,	xpos,	ypos,	width,	height);

				xpos	+=	20;
				ypos	+=	20;

				product_view_node->page_node_list.push_back(page_view_node);

				return(	(DPV_pointer)page_view_node	);
}


//	MEMBER	FUNCTION
DPV_pointer	fermi_view::render_table(	DPV_pointer	parent_data,
																																						DPC_table*	table)	{

				TableViewNode	*table_view_node;

				ProductViewNode	*product_view_node	=	(ProductViewNode*)parent_data;

				if	((xpos	+	width)	>=	WidthOfScreen(	XtScreen(toplevel)))	{
						xpos	=	100;
				}
				if	((ypos	+	height)	>=	HeightOfScreen(	XtScreen(toplevel)))	{
						ypos	=	100;
				}

				table_view_node	=	new	TableViewNode(toplevel,	table,	xpos,	ypos,	width,	height);

				xpos	+=	20;
				ypos	+=	20;

				product_view_node->table_node_list.push_back(	table_view_node	);

				return(	(DPV_pointer)table_view_node);
}

//	MEMBER	FUNCTION
DPV_pointer	fermi_view::render_plot(	DPV_pointer	parent_data,
																																					DPC_plot*	plot)	{
				PageViewNode	*page_view_node;
				PlotViewNode	*plot_view_node;

				page_view_node	=	(PageViewNode*)parent_data;

				plot_view_node	=	new	PlotViewNode(	toplevel,	page_view_node->plots_form_widget,	plot);

				page_view_node->plot_node_list.push_back(	plot_view_node	);

				return(	(DPV_pointer)plot_view_node);
}

DPV_pointer	fermi_view::update_plot(	DPV_pointer	view_data,
																																					DPC_plot*	plot	)	{

				PlotViewNode	*plot_view_node	=	(PlotViewNode*)	view_data;

				//	reset	the	curve	counter	to	0,	so	we	can	step	through	the	curves	again
				plot_view_node->current	=	0;

				return	((DPV_pointer)NULL)	;
}


//	MEMBER	FUNCTION
DPV_pointer	fermi_view::render_curve(	DPV_pointer	parent_data,
																																						DPC_curve*	abstract_curve)	{

				CurveViewNode	*curve_view_node;
				XYCurve	*	fermi_curve;

				//	--------------------------------------------------------------------
				//	Cast	our	parent	data	pointer	to	what	it	really	is:	a	PlotViewNode.
				//	--------------------------------------------------------------------
				PlotViewNode	*plot_view_node	=	(PlotViewNode*)	parent_data;

				//	--------------------------------------------------------
				//	Get	a	reference	to	the	particular	XYCurve	structure	for
				//	this	curve	so	we	can	populate	it.
				//	--------------------------------------------------------
				fermi_curve	=	&(	plot_view_node->curves[plot_view_node->current]	);

				//	--------------------------------------------------------------------
				//	Save	our	reference	to	the	DPC_curve	object,	so	we	can	handle
				//	DATA_UPDATE	notifications.
				//	--------------------------------------------------------------------
				curve_view_node	=	new	CurveViewNode(toplevel,	fermi_curve,	abstract_curve);
				plot_view_node->curve_node_list.push_back(	curve_view_node);

				//------------------------------------------------------------------
				//	Now	that	we've	populated	the	XYCurve	element	for	this	curve,	update
				//	the	CurrentCurve	counter	for	the	next	one.
				//------------------------------------------------------------------
				plot_view_node->current	++;

				return	((DPV_pointer)	NULL);
}

//	MEMBER	FUNCTION
int	fermi_view::update_curve(	DPV_pointer	parent_data,
																														DPC_curve*	curve)	{

				XYCurve	*	fermi_curve;

				double	x,	y;

				//	--------------------------------------------------------------------
				//	Cast	our	parent	data	pointer	to	what	it	really	is:	a	PlotViewNode.
				//	--------------------------------------------------------------------
				PlotViewNode	*plot_view_node	=	(PlotViewNode*)	parent_data;

				//	--------------------------------------------------------
				//	Get	a	reference	to	the	particular	XYCurve	structure	for
				//	this	curve	so	we	can	populate	it.
				//	--------------------------------------------------------
				fermi_curve	=	&(	plot_view_node->curves[plot_view_node->current]	);

				//------------------------------------------------------------------
				//	Now	that	we've	populated	the	XYCurve	element	for	this	curve,	update
				//	the	CurrentCurve	counter	for	the	next	one.
				//------------------------------------------------------------------
				plot_view_node->current	++;

				if	(curve->getXY(&x,	&y))	{

								if	(	x	!=	fermi_curve->points[	fermi_curve->nPoints	-	1	].x	)	{
												fermi_curve->points[	fermi_curve->nPoints	].x	=	x;
												fermi_curve->points[	fermi_curve->nPoints	].y	=	y;
												fermi_curve->nPoints	++;

												if	(	x	-	fermi_curve->points[0].x	>	30.0	)	{
																//FIXME:	terribly	inefficient!!!
																memmove(&fermi_curve->points[0]	,	&fermi_curve->points[1]	,	(fermi_curve->nPoints	-	1)	*	sizeof(XYPoint))	;
																fermi_curve->nPoints	--;
												}
								}

								return(1)	;
				}

				return	(0);
}

//	MEMBER	FUNCTION
void	fermi_view::finalize_page_view(	DPV_pointer	page_view	)	{

				PageViewNode*	page_view_node	=	(PageViewNode	*)page_view;

				page_view_node->finalize();

}

//	MEMBER	FUNCTION
void	fermi_view::finalize_table_view(	DPV_pointer	table_view	)	{

				TableViewNode*	table_view_node	=	(TableViewNode	*)table_view;

				table_view_node->finalize();

}

//	MEMBER	FUNCTION
void	fermi_view::finalize_plot_view(	DPV_pointer	plot_view	)	{

				PlotViewNode	*plot_view_node	=	(PlotViewNode	*)plot_view;

				plot_view_node->finalize();

}

//	MEMBER	FUNCTION
void	fermi_view::update_finalize_plot_view(	DPV_pointer	plot_view	)	{

				PlotViewNode	*plot_view_node;

				plot_view_node	=	(PlotViewNode	*)plot_view;

				XYSetContents(	plot_view_node->xy_widget,
																			plot_view_node->curves,
																			plot_view_node->n_curves,
																			XY_RESCALE);
}

//	MEMBER	FUNCTION
void	fermi_view::notify_product(	DPV_pointer	product_view,	DPV_message	msg	)	{

				PageViewNode			*page_view_node;
				TableViewNode		*table_view_node;
				int	i,	n_pages,	n_tables;

				ProductViewNode	*product_view_node	=	(ProductViewNode	*)product_view;

				n_pages	=	(int)product_view_node->page_node_list.size();
				for	(i=0	;	i<n_pages	;	i++	)	{
								page_view_node	=	product_view_node->page_node_list[i];
								notify_page(	(DPV_pointer)page_view_node,	msg	);
				}

				n_tables	=	(int)product_view_node->table_node_list.size();
				for	(i=0	;	i<n_tables	;	i++	)	{
								table_view_node	=	product_view_node->table_node_list[i];
								notify_table(	(DPV_pointer)table_view_node,	msg	);
				}

}

//	MEMBER	FUNCTION
void	fermi_view::notify_page(	DPV_pointer	page_view,	DPV_message	msg	)	{

				Widget	page_widget;
				PageViewNode	*page_view_node	=	(PageViewNode	*)page_view;

				if	(msg	==	SHOW_WINDOW)	{
								page_widget	=	page_view_node->dialog_shell_widget;
								if	(page_widget	!=	NULL	)	{
												XtPopup(page_widget,	XtGrabNone);
								}
				}	else	if	(msg	==	RAISE_WINDOW)	{
								page_widget	=	page_view_node->dialog_shell_widget;
								if	(page_widget	!=	NULL	)	{
												XRaiseWindow(	XtDisplay(page_widget),	XtWindow(page_widget));
								}
				}	else	{
								int	i,	n_plots;
								//	Propagate	the	notification	to	subordinate	plots.
								n_plots	=	(int)page_view_node->plot_node_list.size();
								for	(i=0	;	i<n_plots	;	i++	)	{
												PlotViewNode	*plot_view_node;
												plot_view_node	=	page_view_node->plot_node_list[i];
												notify_plot(	(DPV_pointer)plot_view_node,	msg	);
								}
				}
}

//	MEMBER	FUNCTION
void	fermi_view::notify_table(	DPV_pointer	table_view,	DPV_message	msg	)	{

				Widget	table_widget;
				TableViewNode	*table_view_node	=	(TableViewNode	*)table_view;

				if	(msg	==	SHOW_WINDOW)	{
								table_widget	=	table_view_node->dialog_shell_widget;
								if	(table_widget	!=	NULL	)	{
												XtPopup(table_widget,	XtGrabNone);
								}
				}	else	if	(msg	==	RAISE_WINDOW)	{
								table_widget	=	table_view_node->dialog_shell_widget;
								if	(table_widget	!=	NULL	)	{
												XRaiseWindow(	XtDisplay(table_widget),	XtWindow(table_widget));
								}
				}
}

//	MEMBER	FUNCTION
void	fermi_view::notify_plot(	DPV_pointer	plot_view,	DPV_message	msg	)	{
				PlotViewNode	*plot_view_node	=	(PlotViewNode*)	plot_view;

				if	(msg	==	DATA_UPDATE	)	{
								int	i,	n_curves;
								//	reset	the	curve	counter	to	0,	so	we	can	step	through	the	curves	again
								plot_view_node->current	=	0;
								n_curves	=	(int)plot_view_node->curve_node_list.size();
								for	(i=0;	i<n_curves;	i++)	{
												CurveViewNode	*curve_view_node;
												curve_view_node	=	plot_view_node->curve_node_list[i];
												update_curve(	plot_view,	curve_view_node->dpc_curve);
								}
								update_finalize_plot_view(	plot_view	);
				}
}

//	MEMBER	FUNCTION
void	fermi_view::notify_curve(	DPV_pointer	curve_view,	DPV_message	msg	)	{
				//	Nothing	to	do.
}

