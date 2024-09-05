
#include	<iostream>
#include	<string>

#include	"curve_view_node.hh"

//	used	by	render_curve
const	char*	mark_style_name[XY_N_MARK_STYLES]	=	{
				"none",										//	XY_NO_MARK
				"square",								//	XY_SQUARE_MARK
				"circle",								//	XY_CIRCLE_MARK
				"star",										//	XY_STAR_MARK
				"x",													//	XY_X_MARK
				"triangle",						//	XY_TRIANGLE_MARK
				"solid_square",		//	XY_SOLID_SQUARE_MARK
				"solid_circle",		//	XY_SOLID_CIRCLE_MARK
				"thick_square",		//	XY_THICK_SQUARE_MARK
				"thick_circle"			//	XY_THICK_CIRCLE_MARK
};

//	used	by	render_curve
static	const	char*	mark_size_name[XY_N_MARK_SIZES]	=	{
				"tiny",										//	XY_TINY
				"small",									//	XY_SMALL
				"medium",								//	XY_MEDIUM
				"large",									//	XY_LARGE
};

//	used	by	render_curve
static	const	char*		line_style_name[XY_N_LINE_STYLES]	=	{
				"no_line",							//	XY_NO_LINE
				"plain",									//	XY_PLAIN_LINE
				"fine_dash",					//	XY_FINE_DASH
				"med_fine_dash",	//	XY_MED_FINE_DASH
				"dash",										//	XY_DASH
				"long_dash",					//	XY_LONG_DASH
				"x_long_dash",			//	XY_X_LONG_DASH
				"dot_dash",						//	XY_1_DOT_DASH
				"2_dot_dash",				//	XY_2_DOT_DASH
				"3_dot_dash",				//	XY_3_DOT_DASH
				"4_dot_dash",				//	XY_4_DOT_DASH
				"thick_line",				//	XY_THICK_LINE
				"x_thick_line",		//	XY_X_THICK_LINE
};

//	CLASS	VARIABLE	INITIALIZATION
int	CurveViewNode::instance_count	=	0;

//	DESTRUCTOR
CurveViewNode::~CurveViewNode()	{

				std::cout	<<	"CurveViewNode	destructor."	<<	std::endl;

}

//	CONSTRUCTOR
CurveViewNode::CurveViewNode(	Widget								Toplevel,
																														XYCurve						*Fermi_curve,
																														DPC_curve				*Curve)	{

				char	x_label_work[256];
				char	y_label_work[256];

				char	curve_label[256];
				const	char	*	x_label;
				const	char	*	y_label;

				const	char	*	marker_style;
				const	char	*	marker_size;
				const	char	*	line_style;
				const	char	*	line_color;
				const	char	*	X_scale,	*	X_bias;
				const	char	*	Y_scale,	*	Y_bias;
				double	x_scale_val,	x_bias_val;
				double	y_scale_val,	y_bias_val;

				const	int	NumberOfPointsPerMalloc	=	1024;
				int							n_allocated_points;
				double				x,	y;

				toplevel	=	Toplevel;
				dpc_curve	=	Curve;
				fermi_curve	=	Fermi_curve;

				//-------------------------------------------------------------------
				//	Build	the	label	for	this	curve	that	will	go	into	the	plot's	legend.
				//-------------------------------------------------------------------
				data_src_label	=	Curve->getDataSrcLbl();

				if	((x_label	=	Curve->getXAttribute("label"))	==	NULL)	{
								const	char	*	x_var_name	=	Curve->getXVarName();
								const	char	*	x_unitspec	=	Curve->getXUnits();
								snprintf(x_label_work,	sizeof(x_label_work)-1,	"%s	(%s)",	x_var_name,	x_unitspec);
								x_label	=	x_label_work;
				}

				const	char	*	y_var_name	=	Curve->getYVarName();
				if	((y_label	=	Curve->getYAttribute("label"))	==	NULL)	{
								const	char	*	y_unitspec	=	Curve->getYUnits();
								snprintf(y_label_work,	sizeof(y_label_work)-1,	"%s	(%s)",	y_var_name,	y_unitspec);
								y_label	=	y_label_work;
				}

				snprintf(curve_label,	sizeof(curve_label)-1,	"%s	VS.	%s	[%s]",	y_label,	x_label,	data_src_label);

				if	(data_src_label	!=	NULL)	{
					fermi_curve->curve_id						=	XmStringCreateLocalized((char*)data_src_label);
				}	else	{
					fermi_curve->curve_id						=	XmStringCreateLocalized((char*)y_var_name);
				}

				fermi_curve->name										=	XmStringCreateLocalized(curve_label);

				//------------------------------------------
				//	Set	the	XYCurve's	markerStyle.
				//------------------------------------------
				fermi_curve->markerStyle			=	XY_NO_MARK;	//	Default	marker	style
				if	((marker_style	=	Curve->getYAttribute("symbol_style"))	!=	NULL)	{
								int	i;
								for	(i	=	0	;	i	<	XY_N_MARK_STYLES	;	i++)	{
												if	(!strcasecmp(marker_style,	mark_style_name[i])){
																fermi_curve->markerStyle	=	(char)i;
																break;
												}
								}
				}

				//------------------------------------------
				//	Set	the	XYCurve's	markerSize.
				//------------------------------------------
				fermi_curve->markerSize	=	XY_MEDIUM;	//	Default	marker	size
				if	((marker_size	=	Curve->getYAttribute("symbol_size"))	!=	NULL)	{
								int	i;
								for	(i=0	;	i<XY_N_MARK_SIZES	;	i++)	{
												if	(!strcasecmp(marker_size,	mark_size_name[i])){
																fermi_curve->markerSize	=	(char)i;
																break;
												}
								}
				}

				//------------------------------------------
				//	Set	the	XYCurve's	lineStyle.
				//------------------------------------------
				fermi_curve->lineStyle					=	XY_PLAIN_LINE;	//	Default	line	style
				if	((line_style	=	Curve->getYAttribute("line_style"))	!=	NULL)	{
								int	i;
								for	(i=0	;	i<XY_N_LINE_STYLES	;	i++)	{
												if	(!strcasecmp(line_style,	line_style_name[i])){
																fermi_curve->lineStyle	=	(char)i;
																break;
												}
								}
				}

				//------------------------------------------------------------------
				//	Set	the	XYCurve's	linePixel	and	markerPixel	(i.e.,	their	color's).
				//------------------------------------------------------------------
				if	((line_color	=	Curve->getYAttribute("line_color"))	==	NULL)	{
				//	use	black	when	user	does	not	specify	a	color
								fermi_curve->linePixel	=	BlackPixel(XtDisplay(toplevel),0);	//	Default
				}	else	{
				//	user	specified	color
								XColor	color_def;
								if	(XParseColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	line_color,	&color_def	))	{
												if	(	XAllocColor(XtDisplay(toplevel),	DefaultColormapOfScreen(XtScreen(toplevel)),	&color_def	))	{
																fermi_curve->linePixel	=	color_def.pixel;
												}
								}
				}
				fermi_curve->markerPixel			=	fermi_curve->linePixel;

				//------------------------------------------------------------------
				//	Set	the	XYCurve's	scale	and	bias	factors,	if	specified
				//------------------------------------------------------------------
				x_scale_val	=	y_scale_val	=	1.0;
				x_bias_val	=	y_bias_val	=	0.0;
				if	((X_scale	=	Curve->getXAttribute("scale"))	!=	NULL)	{
								x_scale_val	=	strtod(X_scale,	NULL);
				}
				if	((X_bias	=	Curve->getXAttribute("bias"))	!=	NULL)	{
								x_bias_val	=	strtod(X_bias,	NULL);
				}
				if	((Y_scale	=	Curve->getYAttribute("scale"))	!=	NULL)	{
								y_scale_val	=	strtod(Y_scale,	NULL);
				}
				if	((Y_bias	=	Curve->getYAttribute("bias"))	!=	NULL)	{
								y_bias_val	=	strtod(Y_bias,	NULL);
				}

				//------------------------------------------------------------------
				//	Set	the	XYCurve's	data	points	(points)	and	number	of	points	(nPoints)
				//------------------------------------------------------------------
				n_allocated_points	=	NumberOfPointsPerMalloc;

				if	((fermi_curve->points	=
									(XYPoint	*)malloc(	n_allocated_points	*	sizeof(XYPoint)))	==	NULL)	{
								std::cerr	<<	"OUT_OF_MEMORY	in	fermi_view::render_curve."	<<	std::endl;
				}

				fermi_curve->nPoints	=	0;

				while	(	Curve->getXY(&x,	&y))	{
								fermi_curve->points[	fermi_curve->nPoints	].x	=	(x	*	x_scale_val)	+	x_bias_val;
								fermi_curve->points[	fermi_curve->nPoints	].y	=	(y	*	y_scale_val)	+	y_bias_val;

								fermi_curve->nPoints	++;

								//	If	we've	run	out	of	space	to	store	our	points,	allocate	more.
								if	(	fermi_curve->nPoints	%	NumberOfPointsPerMalloc	==	0)	{
												n_allocated_points	+=	NumberOfPointsPerMalloc;
												if	((fermi_curve->points	=
																	(XYPoint	*)realloc(	fermi_curve->points,	n_allocated_points	*	sizeof(XYPoint)))	==	NULL)	{
																std::cerr	<<	"OUT_OF_MEMORY	in	fermi_view::render_curve."	<<	std::endl;
												}
								}
				}

				//	Save	the	final	point	from	getXY
				fermi_curve->points[	fermi_curve->nPoints	].x	=	(x	*	x_scale_val)	+	x_bias_val;
				fermi_curve->points[	fermi_curve->nPoints	].y	=	(y	*	y_scale_val)	+	y_bias_val;
				fermi_curve->nPoints	++;

				//------------------------------------------------------------------
				//	Set	the	XYCurve's	remaining	elements.
				//------------------------------------------------------------------
				fermi_curve->horizBars					=	NULL;
				fermi_curve->vertBars						=	NULL;
				fermi_curve->flatline						=	0;
				fermi_curve->flatlineValue	=	0.0;

				instance_count	++;
}
