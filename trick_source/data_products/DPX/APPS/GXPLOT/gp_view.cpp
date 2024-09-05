
#include	<stdio.h>				//	for	snprintf
#include	<stdlib.h>			//	for	atoi
#include	<stdarg.h>			//	for	varargs	stuff
#include	<strings.h>

#include	"gp_view.hh"
#include	"parse_format.h"

//	CONSTRUCTOR
gnuplot_view::gnuplot_view(Widget	TopLevel)	{

				toplevel	=	TopLevel;
				display		=	XtDisplay(toplevel);
				screen			=	XtScreen(toplevel);
				colormap	=	DefaultColormapOfScreen(screen);
				xpos			=	200;
				ypos			=	50;
				width		=	500;
				height	=	500;

}

//	DESTRUCTOR
gnuplot_view::~gnuplot_view()	{

				GPViewProductNode	*pvnode;
				int	n_pvnode,	i;

				n_pvnode	=	(int)product_node_list.size();
				for	(i=0	;	i<n_pvnode	;	i++	)	{
								pvnode	=	product_node_list[i];
								if	(pvnode)	{	delete	pvnode;	}
				}
}

//	MEMBER	FUNCTION
DPV_pointer	gnuplot_view::render_product(	DPC_product*	product)	{


				GPViewProductNode	*product_view_node	=	new	GPViewProductNode(toplevel,	product);

				product_node_list.push_back(	product_view_node);

				return	(	(DPV_pointer)product_view_node);
}


//	MEMBER	FUNCTION
DPV_pointer	gnuplot_view::render_page(	DPV_pointer	parent_data,
																																							DPC_page*	page)	{
				GPViewPageNode	*page_view_node;
				//int													page_number;

				GPViewProductNode	*product_view_node	=	(GPViewProductNode	*)parent_data;


				if	((xpos	+	width)	>=	WidthOfScreen(	XtScreen(toplevel)))	{
						xpos	=	100;
				}
				if	((ypos	+	height)	>=	HeightOfScreen(	XtScreen(toplevel)))	{
						ypos	=	100;
				}

				page_view_node	=	new	GPViewPageNode(toplevel,	page,	xpos,	ypos,	width,	height);

				xpos	+=	20;
				ypos	+=	20;

				product_view_node->page_node_list.push_back(	page_view_node	);

				return(	(DPV_pointer)page_view_node	);
}


//	MEMBER	FUNCTION
DPV_pointer	gnuplot_view::render_table(	DPV_pointer	parent_data,
																																								DPC_table*	table)	{
				return	NULL;
}

//	MEMBER	FUNCTION
DPV_pointer	gnuplot_view::render_plot(	DPV_pointer	parent_data,
																																							DPC_plot*	plot)	{
				GPViewPlotNode	*plot_view_node;
				int													plot_number;

				GPViewPageNode	*page_view_node	=	(GPViewPageNode*)parent_data;

				plot_number	=	(int)page_view_node->plot_node_list.size();

				plot_view_node	=	new	GPViewPlotNode(	toplevel,
																																								&page_view_node->textbuf,
																																									plot,
																																									plot_number);

				page_view_node->plot_node_list.push_back(	plot_view_node	);

				return(	(DPV_pointer)plot_view_node);
}

DPV_pointer	gnuplot_view::update_plot(	DPV_pointer	view_data,
																																							DPC_plot*	plot	)	{

				GPViewPlotNode	*plot_view_node	=	(GPViewPlotNode*)	view_data;

				//	reset	the	curve	counter	to	0,	so	we	can	step	through	the	curves	again
				plot_view_node->current	=	0;

				return	((DPV_pointer)NULL)	;
}


//	MEMBER	FUNCTION
DPV_pointer	gnuplot_view::render_curve(	DPV_pointer	parent_data,
																																								DPC_curve*	abstract_curve)	{

				GPViewCurveNode	*curve_view_node;
				int														curve_number;

				//	--------------------------------------------------------------------
				//	Cast	our	parent	data	pointer	to	what	it	really	is:	a	GPPlotViewNode.
				//	--------------------------------------------------------------------
				GPViewPlotNode		*plot_view_node	=	(GPViewPlotNode*)	parent_data;

				//	--------------------------------------------------------------------
				//	Save	our	reference	to	the	DPC_curve	object,	so	we	can	handle
				//	DATA_UPDATE	notifications.
				//	--------------------------------------------------------------------
				curve_number	=	(int)plot_view_node->curve_node_list.size();
				curve_view_node	=	new	GPViewCurveNode(	toplevel,
																																											plot_view_node->textbuf,
																																											abstract_curve,
																																											curve_number);
				plot_view_node->curve_node_list.push_back(	curve_view_node);

				//------------------------------------------------------------------
				//	Update	the	CurrentCurve	counter	for	the	next	one.
				//------------------------------------------------------------------
				plot_view_node->current	++;

				return	((DPV_pointer)	NULL);
}

//	MEMBER	FUNCTION
//	FIXME:	(Alex)	What	does	this	function	do?		It	doesn't	do	anything	right	now.	(5/4/11)
int	gnuplot_view::update_curve(	DPV_pointer	parent_data,
																																DPC_curve*	curve)	{

//	(Alex,	9/24/13)	commented	out	everything	to	suppress	warnings	about	unused	variables.
#if	0
				int				curve_number;

				//	--------------------------------------------------------------------
				//	Cast	our	parent	data	pointer	to	what	it	really	is:	a	GPViewPlotNode.
				//	--------------------------------------------------------------------
				GPViewPlotNode	*plot_view_node	=	(GPViewPlotNode*)	parent_data;

				curve_number	=	(int)plot_view_node->curve_node_list.size();
#endif

				return	(0);
}

//	MEMBER	FUNCTION
void	gnuplot_view::finalize_page_view(	DPV_pointer	page_view	)	{

				GPViewPageNode*	page_view_node	=	(GPViewPageNode	*)page_view;

				page_view_node->finalize();

}

//	MEMBER	FUNCTION
void	gnuplot_view::finalize_table_view(	DPV_pointer	table_view	)	{	}

//	MEMBER	FUNCTION
void	gnuplot_view::finalize_plot_view(	DPV_pointer	plot_view	)	{

				GPViewPlotNode	*plot_view_node	=	(GPViewPlotNode	*)plot_view;

				plot_view_node->finalize();

}
//	MEMBER	FUNCTION
//	FIXME:	(Alex)	What	does	this	function	do?		It	doesn't	do	anything	right	now.	(5/4/11)
void	gnuplot_view::update_finalize_plot_view(	DPV_pointer	plot_view	)	{

//	(Alex,	9/24/13)	commented	out	everything	to	suppress	warnings	about	unused	variables.
#if	0
				GPViewPlotNode	*plot_view_node;

				plot_view_node	=	(GPViewPlotNode	*)plot_view;
#endif

}

//	MEMBER	FUNCTION
void	gnuplot_view::notify_product(	DPV_pointer	product_view,	DPV_message	msg	)	{

				GPViewPageNode	*page_view_node;
				int	i,	n_pages;

				GPViewProductNode	*product_view_node	=	(GPViewProductNode	*)product_view;

				n_pages	=	(int)product_view_node->page_node_list.size();
				for	(i=0	;	i<n_pages	;	i++	)	{
								page_view_node	=	product_view_node->page_node_list[i];
								notify_page(	(DPV_pointer)page_view_node,	msg	);
				}
}

//	MEMBER	FUNCTION
void	gnuplot_view::notify_page(	DPV_pointer	page_view,	DPV_message	msg	)	{

				Widget	page_widget;
				GPViewPageNode	*page_view_node	=	(GPViewPageNode	*)page_view;

				if	(msg	==	RAISE_WINDOW)	{
								page_widget	=	page_view_node->dialog_shell_widget;
								if	(page_widget	!=	NULL	)	{
												XRaiseWindow(	XtDisplay(page_widget),	XtWindow(page_widget));
								}
				}	else	{
								int	i,	n_plots;
								//	Propagate	the	notification	to	subordinate	plots.
								n_plots	=	(int)page_view_node->plot_node_list.size();
								for	(i=0	;	i<n_plots	;	i++	)	{
												GPViewPlotNode	*plot_view_node;
												plot_view_node	=	page_view_node->plot_node_list[i];
												notify_plot(	(DPV_pointer)plot_view_node,	msg	);
								}
				}

}

//	MEMBER	FUNCTION
void	gnuplot_view::notify_table(	DPV_pointer	table_view,	DPV_message	msg	)	{	}

//	MEMBER	FUNCTION
void	gnuplot_view::notify_plot(	DPV_pointer	plot_view,	DPV_message	msg	)	{

				GPViewPlotNode	*plot_view_node	=	(GPViewPlotNode*)	plot_view;

				if	(msg	==	DATA_UPDATE	)	{
								int	i,	n_curves;
								//	reset	the	curve	counter	to	0,	so	we	can	step	through	the	curves	again
								plot_view_node->current	=	0;
								n_curves	=	(int)plot_view_node->curve_node_list.size();
								for	(i=0;	i<n_curves;	i++)	{
												GPViewCurveNode	*curve_view_node;
												curve_view_node	=	plot_view_node->curve_node_list[i];
												update_curve(	plot_view,	curve_view_node->dpc_curve);
								}
								update_finalize_plot_view(	plot_view	);
				}

}

//	MEMBER	FUNCTION
void	gnuplot_view::notify_curve(	DPV_pointer	curve_view,	DPV_message	msg	)	{
				//	Nothing	to	do.
}

