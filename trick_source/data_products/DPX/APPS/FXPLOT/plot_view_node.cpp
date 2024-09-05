
#include	<Xm/Xm.h>
#include	<Xm/Form.h>
#include	<Xm/PushB.h>
#include	<Xm/ToggleBG.h>
#include	<math.h>

#include	<iostream>
#include	<string>

#include	<stdio.h>				//	for	snprintf
#include	<string.h>			//	for	strncpy,	strcmp,	strlen
#include	<stdlib.h>
#include	<unistd.h>

#include	"plot_view_node.hh"
#include	"post_dialog.h"

#ifndef	PI
#define	PI	3.1415926535
#endif

//	our	line	colors	for	multiple	curves	on	same	plot
#define	NUM_COLORS	16
static	const	char*	colors[NUM_COLORS]	=	{
				"#000000",							//	black
				"#ff0000",							//	red
				"#0000ff",							//	blue
				"#00ff00",							//	green
				"#ff00ff",							//	magenta
				"#ffff00",							//	yellow
				"#00ffff",							//	cyan
				"#cc3300",							//	burnt	orange
				"#999999",							//	gray
				"#ff99cc",							//	pink
				"#6699ff",							//	lt	blue
				"#669966",							//	olive	green
				"#660066",							//	purple
				"#ffcc00",							//	gold
				"#ccff00",							//	limegreen
				"#996633",							//	brown
};

static	double	dMin(double	d1,	double	d2);
static	double	dMax(double	d1,	double	d2);

//	CALLBACK
static	void	toggle_sel_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				Boolean	val	;

				PlotViewNode*	plot_view_node	=	(PlotViewNode	*)client_data;
				XmToggleButtonCallbackStruct	*toggle_data	=	(XmToggleButtonCallbackStruct	*)call_data;

				val	=	(toggle_data->set	!=	0);
				plot_view_node->selected	=	val;
}

//	CALLBACK
static	void	toggle_grid_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				Boolean	val	;

				PlotViewNode*	plot_view_node	=	(PlotViewNode	*)client_data;
				XmToggleButtonCallbackStruct	*toggle_data	=	(XmToggleButtonCallbackStruct	*)call_data;

				val	=	(toggle_data->set	!=	0);
				XtVaSetValues(	plot_view_node->xy_widget,	XmNshowGrid,	val,	NULL);
}

//	CALLBACK
static	void	toggle_value_display_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				Boolean	val	;
				PlotViewNode*	plot_view_node	=	(PlotViewNode	*)client_data;
				XmToggleButtonCallbackStruct	*toggle_data	=	(XmToggleButtonCallbackStruct	*)call_data;

				val	=	(toggle_data->set	!=	0);
				XtVaSetValues(	plot_view_node->xy_widget,	XmNValueDisplay,	val,	NULL);
}

//	CALLBACK
static	void	toggle_crosshairs_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				Boolean	val	;
				PlotViewNode*	plot_view_node	=	(PlotViewNode	*)client_data;
				XmToggleButtonCallbackStruct	*toggle_data	=	(XmToggleButtonCallbackStruct	*)call_data;

				val	=	(toggle_data->set	!=	0);
				XtVaSetValues(	plot_view_node->xy_widget,	XmNenableCrosshairs,	val,	NULL);
}

//	CALLBACK
static	void	print_plot_cb(Widget	w,	XtPointer	client_data,	XtPointer	call_data)	{
				char	ps_file_name[256];
				char	system_cmd[256];
				char	*trick_print_cmd;
				char	*trick_printer_name;
				int	sys_ret;

				PlotViewNode*	plot_view_node	=	(PlotViewNode	*)client_data;

				if	((trick_print_cmd	=	getenv("TRICK_PRINT_CMD"))	==	NULL)	{
								trick_print_cmd	=	(char	*)"lpr";
				}
				trick_printer_name	=	getenv("TRICK_PRINTER_NAME");

				if	(!access("/tmp",	W_OK))	{
								strncpy(ps_file_name,	"/tmp/dpx_postscript_XXXXXX",	sizeof(ps_file_name));
				}	else	if	(!access("/var/tmp",	W_OK))	{
								strncpy(ps_file_name,	"/var/tmp/dpx_postscript_XXXXXX",	sizeof(ps_file_name));
				}	else	{

								post_dialog(	plot_view_node->toplevel,	XmDIALOG_ERROR,
												"Unable	to	access	/tmp	or	/var/tmp,\n"
												"where	a	temporary	Postscript	file\n"
												"(that	represents	your	print	out)\n"
												"needs	to	be	generated.	Please	check\n"
												"your	file	permissions.");

								return;
				}

				if	(mkstemp(	ps_file_name)	<	0)	{
								post_dialog(	plot_view_node->toplevel,	XmDIALOG_ERROR,
												"Unable	to	generate	a	temporary	file\n"
												"name	for	some	mind	boggling	reason."	);

								return;
				}

				XYPrintContents(	plot_view_node->xy_widget,	ps_file_name);

				if	(!strcmp(trick_print_cmd,	"lpr"))	{
								if	((trick_printer_name	==	NULL)	||	(strlen(trick_printer_name)	==	0))	{
												snprintf(system_cmd,	sizeof(system_cmd),	"%s	%s",	trick_print_cmd,	ps_file_name);
								}	else	{
												snprintf(system_cmd,	sizeof(system_cmd),	"%s	-P	%s	%s",	trick_print_cmd,	trick_printer_name,	ps_file_name);
								}
				}	else	{
								snprintf(system_cmd,	sizeof(system_cmd),	"%s	%s",	trick_print_cmd,	ps_file_name);
				}

				std::cout	<<	"Printing	with	command:	"	<<	system_cmd	<<	std::endl;
				sys_ret	=	system(	system_cmd);
				std::cout	<<	"	System	call	returned:	"	<<	sys_ret	<<	std::endl;
				snprintf(system_cmd,	sizeof(system_cmd),	"rm	%s",	ps_file_name);
				system(	system_cmd);
}


//	CLASS	VARIABLE	INITIALIZATION
int	PlotViewNode::instance_count	=	0;


//	DESTRUCTOR
PlotViewNode::~PlotViewNode()	{

				#ifdef	DEBUG
				std::cout	<<	"PlotViewNode	DESTRUCTOR."	<<	std::endl;
				#endif

				delete	curves;
				XtDestroyWidget(xy_widget);
				XtDestroyWidget(print_button);
				XtDestroyWidget(toggle4);
				XtDestroyWidget(toggle3);
				XtDestroyWidget(toggle2);
				XtDestroyWidget(toggle1);
				XtDestroyWidget(ctrls_form_widget);
				XtDestroyWidget(main_form_widget);
}


//	CONSTRUCTOR
PlotViewNode::PlotViewNode(	Widget	Toplevel,	Widget	Parent_form,	DPC_plot*	Plot	)	{

				XmString	X_label,	Y_label;
				Pixel	fg,bg;
				Pixmap	pixmap;

				const	char	*	show_grid;
				const	char	*	grid_color;
				const	char	*	foreground_color;
				const	char	*	background_color;
				const	char	*	font;

				const	char	*	const_temp_str;
				char	charbuf[256];

				const	char	*	X_min,	*	X_max;
				const	char	*	Y_min,	*	Y_max;
				double	x_min_val,	x_max_val;
				double	y_min_val,	y_max_val;

				plot	=	Plot;

				toplevel	=	Toplevel;

				current		=	0;
				selected	=	0;

				n_curves	=	plot->getNumCurves();
				curves	=	new	XYCurve[n_curves];

				//	X	Axis	label.
				snprintf(	charbuf,	sizeof(charbuf),	"%s",	const_temp_str	);
				X_label	=	XmStringCreateLocalized(	charbuf);

				//	Y	Axis	label.
				snprintf(	charbuf,	sizeof(charbuf),	"%s",	const_temp_str	);
				Y_label	=	XmStringCreateLocalized(	charbuf);

				//	---------------------------
				//	Create	the	plot	widget	name.
				//	---------------------------
				snprintf(	charbuf,	sizeof(charbuf),	"Plot_%d",	instance_count);

				//	-----------------------
				//	Create	the	plot	widget.
				//	-----------------------
				main_form_widget	=	XtVaCreateWidget(	"PlotForm",
																																									xmFormWidgetClass,
																																									Parent_form,
																																									XmNresizePolicy,	XmRESIZE_GROW,
																																									XmNfractionBase,	10,
																																									NULL);

				ctrls_form_widget	=	XtVaCreateManagedWidget(	"CtrlsForm",
																																												xmFormWidgetClass,
																																												main_form_widget,
																																												XmNresizePolicy,					XmRESIZE_GROW,
																																												XmNfractionBase,					10,
																																												XmNtopAttachment,				XmATTACH_NONE,
																																												XmNleftAttachment,			XmATTACH_FORM,
																																												XmNrightAttachment,		XmATTACH_FORM,
																																												XmNbottomAttachment,	XmATTACH_FORM,
																																												NULL);

				XtVaGetValues	(ctrls_form_widget,	XmNforeground,	&fg,	XmNbackground,	&bg,	NULL);

				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"toggle_xhairs.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								toggle1	=	XtVaCreateManagedWidget(	"X",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_FORM,
																																											NULL);
				}	else	{
								toggle1	=	XtVaCreateManagedWidget(	"X",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_FORM,
																																											XmNlabelType,								XmPIXMAP,
																																											XmNlabelPixmap,						pixmap,
																																											NULL);
				}

				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"toggle_values.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								toggle2	=	XtVaCreateManagedWidget(	"V",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_WIDGET,
																																											XmNleftWidget,							toggle1,
																																											NULL);
				}	else	{
								toggle2	=	XtVaCreateManagedWidget(	"V",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_WIDGET,
																																											XmNleftWidget,							toggle1,
																																											XmNlabelType,								XmPIXMAP,
																																											XmNlabelPixmap,						pixmap,
																																											NULL);
				}

				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"toggle_grid.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								toggle3	=	XtVaCreateManagedWidget(	"G",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_WIDGET,
																																											XmNleftWidget,							toggle2,
																																											NULL);
				}	else	{
								toggle3	=	XtVaCreateManagedWidget(	"G",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_WIDGET,
																																											XmNleftWidget,							toggle2,
																																											XmNlabelType,								XmPIXMAP,
																																											XmNlabelPixmap,						pixmap,
																																											NULL);
				}

				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"select.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								toggle4	=	XtVaCreateManagedWidget(	"Sel",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_WIDGET,
																																											XmNleftWidget,							toggle3,
																																											NULL);
				}	else	{
								toggle4	=	XtVaCreateManagedWidget(	"Sel",	xmToggleButtonGadgetClass,	ctrls_form_widget,
																																											XmNleftAttachment,			XmATTACH_WIDGET,
																																											XmNleftWidget,							toggle3,
																																											XmNlabelType,								XmPIXMAP,
																																											XmNlabelPixmap,						pixmap,
																																											NULL);
				}
				pixmap	=	XmGetPixmap(	XtScreen(toplevel),	(char	*)"print_plot.xpm",	fg,	bg);
				if	(pixmap	==	XmUNSPECIFIED_PIXMAP)	{
								print_button	=	XtVaCreateManagedWidget(	"PrintPlot",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																XmNrightAttachment,			XmATTACH_FORM,
																																																NULL);
				}	else	{
								print_button	=	XtVaCreateManagedWidget(	"PrintPlot",	xmPushButtonWidgetClass,	ctrls_form_widget,
																																																XmNleftAttachment,			XmATTACH_WIDGET,
																																																XmNleftWidget,							toggle4,
																																																XmNlabelType,								XmPIXMAP,
																																																XmNlabelPixmap,						pixmap,
																																																NULL);
				}

				XtManageChild(ctrls_form_widget);

				xy_widget	=	XtVaCreateManagedWidget(	charbuf,
																																									xyWidgetClass,
																																									main_form_widget,
																																									XmNxAxisLabel,	X_label,
																																									XmNyAxisLabel,	Y_label,
																																									XmNshowLegend,	1,
																																									XmNdoubleBuffer,	1,
																																									XmNtopAttachment,				XmATTACH_FORM,
																																									XmNleftAttachment,			XmATTACH_FORM,
																																									XmNrightAttachment,		XmATTACH_FORM,
																																									XmNbottomAttachment,	XmATTACH_WIDGET,
																																									XmNbottomWidget,					ctrls_form_widget,
																																									NULL);

				XmStringFree(X_label);
				XmStringFree(Y_label);

				//	-------------------------------------------------------
				//	Set	Y	axis	format	if	specified.
				//	-------------------------------------------------------
				if	(plot->getYFormat()	!=	NULL	&&	strcmp(plot->getYFormat(),	""))	{
								XtVaSetValues(	xy_widget,	XmNyAxisFormat,	XmStringCreateLocalized((char	*)plot->getYFormat()),	NULL	)	;
				}

				//	-------------------------------------------------------
				//	Set	X	axis	scaling	if	specified.
				//	-------------------------------------------------------
				if	(plot->getAttribute("x_scale")	!=	NULL	&&	!strcmp(plot->getAttribute("x_scale"),	"Log"))	{
								Boolean	log	=	True	;
								XtVaSetValues(xy_widget,	XmNxLogScaling,	log,	NULL)	;
				}

				//	-------------------------------------------------------
				//	Set	Y	axis	scaling	if	specified.
				//	-------------------------------------------------------
				if	(plot->getAttribute("y_scale")	!=	NULL	&&	!strcmp(plot->getAttribute("y_scale"),	"Log"))	{
								Boolean	log	=	True	;
								XtVaSetValues(xy_widget,	XmNyLogScaling,	log,	NULL)	;
				}

				//	-------------------------------------------------------
				//	Enable	the	plot	grid	if	specified.
				//	-------------------------------------------------------
				if	((show_grid	=	plot->getAttribute("grid"))	!=	NULL)	{
							XmToggleButtonGadgetSetState(	toggle3,	True,	False);
							XtVaSetValues(	xy_widget,	XmNshowGrid,	1,	NULL);
				}	else	{
							XtVaSetValues(	xy_widget,	XmNshowGrid,	0,	NULL);
				}

				//	-------------------------------------------------------
				//	Set	the	plot's	grid	color.
				//	-------------------------------------------------------
				//XtVaSetValues(	xy_widget,	XmNgridColor,	BlackPixel(XtDisplay(toplevel),0),	NULL);
				if	((grid_color	=	plot->getAttribute("grid_color"))	!=	NULL)	{
								XColor	color_def;
								if	(	XParseColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	grid_color,	&color_def	))	{
												if	(	XAllocColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																XtVaSetValues(	xy_widget,	XmNgridColor,	color_def.pixel,	NULL);
												}
								}
				}

				//	-------------------------------------------------------
				//	Set	the	plot's	foreground	color.
				//	-------------------------------------------------------
				//XtVaSetValues(	xy_widget,	XmNforeground,	BlackPixel(XtDisplay(toplevel),0),	NULL);
				if	((foreground_color	=	plot->getAttribute("foreground_color"))	!=	NULL)	{
								XColor	color_def;
								if	(	XParseColor(	XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	foreground_color,	&color_def	))	{
												if	(	XAllocColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																XtVaSetValues(	xy_widget,	XmNforeground,	color_def.pixel,	NULL);
												}
								}
				}

				//	-------------------------------------------------------
				//	Set	the	plot's	background	color.
				//	-------------------------------------------------------
				//XtVaSetValues(	xy_widget,	XmNbackground,	WhitePixel(	XtDisplay(toplevel),0),	NULL);
				if	((background_color	=	plot->getAttribute("background_color"))	!=	NULL)	{
								XColor	color_def;
								if	(	XParseColor(	XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	background_color,	&color_def	))	{
												if	(	XAllocColor(	XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																XtVaSetValues(	xy_widget,	XmNbackground,	color_def.pixel,	NULL);
												}
								}
				}

				//	-------------------------------------------------------
				//	Set	the	plot's	font.
				//	-------------------------------------------------------
				if	((font	=	plot->getAttribute("font"))	!=	NULL)	{
								XmRendition	rendition;
								XmRenderTable	new_render_table;
								Arg	wargs[3];
								int	nargs	=	0;
								XtSetArg(	wargs[nargs],	XmNfontName,	font);														nargs	++;
								XtSetArg(	wargs[nargs],	XmNfontType,	XmFONT_IS_FONT);				nargs	++;
								XtSetArg(	wargs[nargs],	XmNloadModel,	XmLOAD_IMMEDIATE);	nargs	++;

								rendition	=	XmRenditionCreate(	xy_widget,	XmFONTLIST_DEFAULT_TAG,	wargs,	nargs);
								new_render_table	=	XmRenderTableAddRenditions(	NULL,	&rendition,	1,	XmMERGE_REPLACE);
								XtVaSetValues(	xy_widget,	XmNrenderTable,	new_render_table,	NULL);
				}

				//	-------------------------------------------------------
				//	Set	the	plot's	xmin,	xmax,	ymin,	and	ymax
				//	-------------------------------------------------------
				if	((X_min	=	plot->getAttribute("xmin"))	!=	NULL)	{
								x_min_val	=	strtod(X_min,	NULL);
								plot->setXMin(x_min_val);
				}
				if	((X_max	=	plot->getAttribute("xmax"))	!=	NULL)	{
								x_max_val	=	strtod(X_max,	NULL);
								plot->setXMax(x_max_val);
				}
				if	((Y_min	=	plot->getAttribute("ymin"))	!=	NULL)	{
								y_min_val	=	strtod(Y_min,	NULL);
								plot->setYMin(y_min_val);
				}
				if	((Y_max	=	plot->getAttribute("ymax"))	!=	NULL)	{
								y_max_val	=	strtod(Y_max,	NULL);
								plot->setYMax(y_max_val);
				}

				//
				//	----------------------------------------------------------------------
				//	----------------------------------------------------------------------

				XtAddCallback(	toggle1,	XmNvalueChangedCallback,	toggle_crosshairs_cb,	(XtPointer)this);
				XtAddCallback(	toggle2,	XmNvalueChangedCallback,	toggle_value_display_cb,	(XtPointer)this);
				XtAddCallback(	toggle3,	XmNvalueChangedCallback,	toggle_grid_cb,	(XtPointer)this);
				XtAddCallback(	toggle4,	XmNvalueChangedCallback,	toggle_sel_cb,	(XtPointer)this);
				XtAddCallback(	print_button,	XmNactivateCallback,	print_plot_cb,	(XtPointer)this);

				instance_count	++;
}


void	PlotViewNode::finalize()	{

				XmString	plot_title;
				char	charbuf[256];
				const	char	*data_src_label;
				const	char	*title;
				double	cxmin,	cxmax,	cymin,	cymax;

				if	(n_curves	==	1)	{
								data_src_label	=	curve_node_list[0]->data_src_label;
				}	else	{
								data_src_label	=	NULL;
				}

				title	=	plot->getTitle();

				//	Construct	the	Plot	title.
				if	(	title	!=	NULL)	{
								if	(strcmp(title,	"Contrast")==0)	{
												//	special	case	for	Contrast	session:	1st	plot	is	Compare,	2nd	plot	is	Difference
												if	(instance_count==1)	{
																snprintf(	charbuf,	sizeof(charbuf),	"Plot\n(Compare)");
												}	else		{
																snprintf(	charbuf,	sizeof(charbuf),	"Plot\n(Difference)");
												}
								}	else	if	(	data_src_label	!=	NULL)	{
												snprintf(	charbuf,	sizeof(charbuf),	"%s\n(%s)",	title,	data_src_label);
								}	else	{
												snprintf(	charbuf,	sizeof(charbuf),	"%s",	title);
								}
				}	else	{
								if	(	data_src_label	!=	NULL)	{
												snprintf(	charbuf,	sizeof(charbuf),	"(%s)",	data_src_label);
								}	else	{
												charbuf[0]	=	'\0';
								}
				}

				plot_title	=	XmStringCreateLocalized(	charbuf);
				XtVaSetValues(	xy_widget,	XmNplotTitle,	plot_title,	NULL);
				XmStringFree(plot_title);

				//	Turn	off	the	legend	when	it	gets	too	big	because	of	multiple	curves
				//	TODO:	should	this	be	a	user	settable	variable?
				if	(n_curves	>	20)	{
								XtVaSetValues(	xy_widget,	XmNshowLegend,	False,	NULL);
				}

				//if	((n_curves	>	1)	&&	(XmStringCompare(curves[0].curve_id,	curves[1].curve_id)))	{
				if	(n_curves	>	1)	{
								//	Color	each	curve	when	plotting	multiple	curves
								XColor	color_def;
								int	curv_ix;
								for	(curv_ix=0	;	curv_ix<	n_curves	;	curv_ix++)	{
												const	char	*	line_color	=	colors[curv_ix	%	NUM_COLORS];
												if	(XParseColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	line_color,	&color_def	))	{
																//	only	use	our	color	if	the	curve's	line_color	was	not	set	by	user
																if	(	(curves[curv_ix].linePixel==0)	&&	XAllocColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																				curves[curv_ix].linePixel	=	color_def.pixel;
																}
												}
								}
				}

				XYSetContents(	xy_widget,
																			curves,
																			n_curves,
																			XY_RESCALE);

				XYGetVisibleRange(xy_widget,	&cxmin,	&cymin,	&cxmax,	&cymax);

				XYSetVisibleRange(xy_widget,
																dMax(cxmin,	plot->getXMin()),
																dMax(cymin,	plot->getYMin()),
																dMin(cxmax,	plot->getXMax()),
																dMin(cymax,	plot->getYMax()));
}

//	return	the	minimum	number
static	double	dMin(double	d1,	double	d2)	{
				if	(d2	<	d1)
								return	d2;
				else
								return	d1;
}

//	return	the	maximum	number
static	double	dMax(double	d1,	double	d2)	{
				if	(d2	>	d1)
								return	d2;
				else
								return	d1;
}
