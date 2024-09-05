
#include	<math.h>
#include	<stdio.h>				//	for	snprintf
#include	<string.h>			//	for	strncpy,	strcmp,	strlen
#include	<string>
#include	<iostream>
#include	<sstream>

#include	"gp_view_plot_node.hh"
#include	"gp_version.hh"
#include	"gp_colors.hh"

//	CLASS	VARIABLE	INITIALIZATION
int	GPViewPlotNode::instance_count	=	0;


//	DESTRUCTOR
GPViewPlotNode::~GPViewPlotNode()	{

}


//	CONSTRUCTOR
GPViewPlotNode::GPViewPlotNode(	Widget											Toplevel,
																																DPV_TextBuffer	*	Textbuf,
																																DPC_plot							*	Plot,
																																int														Plot_number	)	{

				const	char	*separator	=	"#------------------------------------------------\n";
				const	char	*new_section	=	"\n\n";

				//!	Plot	info	is	taken	from	file:	'/tmp/DP_*.xml'
				plot	=	Plot;
				toplevel	=	Toplevel;
				textbuf	=	Textbuf;

				/*!
					*	Append	to	the	gnuplot	file	with	Plot	setup	and	option	commands
					*/
				textbuf->print("\n");
				textbuf->print("%s",	new_section);
				textbuf->print("%s",	separator);
				textbuf->print("#	Plot	%d\n",	instance_count+1);
				textbuf->print("%s",	separator);

				//!	Plot	origins.	These	will	be	populated	by	the	Page's	finalize	method.
				textbuf->print("plot_origin_x_%d	=	<PLOT_ORIGIN_X_%d>	\n",
																Plot_number,	Plot_number);
				textbuf->print("plot_origin_y_%d	=	<PLOT_ORIGIN_Y_%d>	\n",
																Plot_number,	Plot_number);
				textbuf->print("set	origin	plot_origin_x_%d,	plot_origin_y_%d\n",
																Plot_number,	Plot_number);

				textbuf->print("set	size	<PLOT_SIZE_SPEC>	\n");

				textbuf->print("<PLOT_KEY_SPEC>");	//	key	is	the	legend

				if	(	GnuplotVersion()	>=	4.2	)	{
								textbuf->print("set	title	\"<PLOT_TITLE>\"	offset	plot_title_xpos,	plot_title_ypos	<PLOT_FONT_SPEC>	<PLOT_TEXT_COLOR_SPEC>\n");
								textbuf->print("set	xlabel	\"<PLOT_X_AXIS_LABEL>\"	offset	x_axis_label_xpos,	x_axis_label_ypos	<PLOT_FONT_SPEC>	<PLOT_TEXT_COLOR_SPEC>\n");
								textbuf->print("set	ylabel	\"<PLOT_Y_AXIS_LABEL>\"	offset	y_axis_label_xpos,	y_axis_label_ypos	<PLOT_FONT_SPEC>	<PLOT_TEXT_COLOR_SPEC>\n");
				}	else	{
								textbuf->print("set	title	\"<PLOT_TITLE>\"	plot_title_xpos,	plot_title_ypos	<PLOT_FONT_SPEC>	\n");
								textbuf->print("set	xlabel	\"<PLOT_X_AXIS_LABEL>\"	x_axis_label_xpos,	x_axis_label_ypos	<PLOT_FONT_SPEC>	\n");
								textbuf->print("set	ylabel	\"<PLOT_Y_AXIS_LABEL>\"	y_axis_label_xpos,	y_axis_label_ypos	<PLOT_FONT_SPEC>	\n");
				}

				/*!
					*	OPTIONAL	PLOT	SETTINGS
					*/
				if	(	GnuplotVersion()	>=	4.0	)	{
								textbuf->print("<PLOT_X_AXIS_SCALING_SPEC>");
								textbuf->print("<PLOT_Y_AXIS_SCALING_SPEC>");
				}
				textbuf->print("<PLOT_Y_AXIS_FORMAT_SPEC>");
				textbuf->print("<PLOT_FOREGROUND_SPEC>");
				textbuf->print("<PLOT_BACKGROUND_SPEC>");
				textbuf->print("<PLOT_GRID_SPEC>	<PLOT_GRID_COLOR_SPEC>\n");

				/*!
					*	PLOTTING:
					*	There	are	three	`gnuplot`	commands	which	actually	create	a	plot:	`plot`,
					*	`splot`	and	`replot`.		`plot`	generates	2-d	plots,	`splot`	generates	3-d
					*	plots	(actually	2-d	projections,	of	course),	and	`replot`	appends	its
					*	arguments	to	the	previous	`plot`	or	`splot`	and	executes	the	modified
					*	command.
					*/
				textbuf->print("plot	");

				instance_count	++;
}

void	GPViewPlotNode::finalize()	{

				std::string							plot_data_src_label;
				std::string							plot_title;
				std::string							plot_x_label;
				std::string							plot_x_scale;
				std::string							plot_x_min_rng;
				std::string							plot_x_max_rng;
				std::string							plot_y_label;
				std::string							plot_y_units;
				std::string							plot_y_format;
				std::string							plot_y_scale;
				std::string							plot_y_min_rng;
				std::string							plot_y_max_rng;
				std::string							plot_fg_color;
				std::string							plot_bg_color;
				std::string							show_grid;
				std::string							grid_color;
				std::string							plot_font;
				int										n_curves;
				std::stringstream	tmp_stream;

				//!	Plot	info	is	taken	from	file:	'/tmp/DP_*.xml'
				tmp_stream.str("");
				tmp_stream	<<	"Plot	"	<<	instance_count+1	;

				n_curves	=	plot->getNumCurves();

				/**!
					*	WARNING:	getAttribute()	returns	a	const	char	*,	which	may	be	null.
					*										Assigning	a	null	pointer	to	a	string	will	crash	the	plot.
					*										So,	check	the	return	value	before	making	the	assignment.
					*/
				plot_title						=	(plot->getTitle())	?		plot->getTitle()	:	tmp_stream.str();
				plot_x_label				=	plot->getXLabel();
				plot_x_scale				=	(plot->getAttribute("x_scale"))	?		plot->getAttribute("x_scale")	:	"";
				plot_x_min_rng		=	(plot->getAttribute("xmin"))	?		plot->getAttribute("xmin")	:	"";
				plot_x_max_rng		=	(plot->getAttribute("xmax"))	?		plot->getAttribute("xmax")	:	"";
				plot_y_label				=	plot->getYLabel();
				plot_y_format			=	(plot->getAttribute("format"))	?		plot->getAttribute("format")	:	"";
				plot_y_scale				=	(plot->getAttribute("y_scale"))	?		plot->getAttribute("y_scale")	:	"";
				plot_y_min_rng		=	(plot->getAttribute("ymin"))	?		plot->getAttribute("ymin")	:	"";
				plot_y_max_rng		=	(plot->getAttribute("ymax"))	?		plot->getAttribute("ymax")	:	"";
				plot_fg_color			=	(plot->getAttribute("foreground_color"))	?		plot->getAttribute("foreground_color")	:	"";
				plot_bg_color			=	(plot->getAttribute("background_color"))	?		plot->getAttribute("background_color")	:	"";
				show_grid							=	(plot->getAttribute("grid"))	?		plot->getAttribute("grid")	:	"";
				grid_color						=	(plot->getAttribute("grid_color"))	?		plot->getAttribute("grid_color")	:	"";
				plot_font							=	(plot->getAttribute("font"))	?		plot->getAttribute("font")	:	"";

				if	(n_curves	==	1)	{
								plot_data_src_label	=	curve_node_list[0]->data_src_label;
								if	(!plot_data_src_label.empty())	{
												/*	Append	SIM/RUN_	name	to	plot	title	*/
												plot_title.append("\\n"	);		/*	add	a	newline	after	Plot	title	*/
												plot_title.append("("	+	plot_data_src_label	+	")");
								}
				}

				//!	plot_width	and	plot_height	values	are	assigned	in	the	Page	constructor.
				textbuf->subst_g(	"<PLOT_SIZE_SPEC>",	"plot_width,	plot_height")	;

				if	(n_curves	>	10)	{
								//	too	many	curves	to	have	a	legend
								textbuf->subst_g(	"<PLOT_KEY_SPEC>",	"set	key	off	\n"	);
				}	else	{
								//	use	default	page	setting
								textbuf->subst_g(	"<PLOT_KEY_SPEC>",	"<PAGE_KEY_SPEC>"	);
				}

				//!	Set	the	plot's	TITLE.
				plot_title.insert(	0,	"\\n"	);		/*	add	a	newline	between	Page	title	*/
				textbuf->subst_g(	"<PLOT_TITLE>",	plot_title.c_str()	);

				//!	Set	the	plot's	X	AXIS	LABEL.
				if	(	!	plot_x_label.empty()	)	{
								textbuf->subst_g(	"<PLOT_X_AXIS_LABEL>",	plot_x_label.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_X_AXIS_LABEL>",	""	);
				}

				//!	Set	the	plot's	X	AXIS	SCALING.
				if	(	plot_x_scale.compare("Log")	==	0	)	{
								textbuf->subst_g(	"<PLOT_X_AXIS_SCALING_SPEC>",	"set	logscale	x\n"	);
				}	else	{
								textbuf->subst_g(	"<PLOT_X_AXIS_SCALING_SPEC>",	"<PAGE_X_AXIS_SCALING_SPEC>"	);
				}

				//!	Set	the	plot's	X	MIN	RANGE.
				if	(	!	plot_x_min_rng.empty()	)	{
								textbuf->subst_g(	"<PLOT_X_MIN_RNG>",	plot_x_min_rng.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_X_MIN_RNG>",	""	);		/*	keep	current	range	*/
				}

				//!	Set	the	plot's	X	MAX	RANGE.
				if	(	!	plot_x_max_rng.empty()	)	{
								textbuf->subst_g(	"<PLOT_X_MAX_RNG>",	plot_x_max_rng.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_X_MAX_RNG>",	""	);		/*	keep	current	range	*/
				}

				//!	Set	the	plot's	Y	AXIS	LABEL.
				if	(	!	plot_y_label.empty()	)	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_LABEL>",	plot_y_label.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_LABEL>",	""	);
				}

				//!	Set	the	plot's	Y	AXIS	UNITS.
				if	(	!	plot_y_units.empty()	)	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_UNITS>",	plot_y_units.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_UNITS>",	""	);
				}

				//!	Set	the	plot's	Y	AXIS	FORMAT.
				if	(	!	plot_y_format.empty()	)	{
								tmp_stream.str("");
								tmp_stream	<<	"set	format	y	\'"	<<	plot_y_format	<<	"\'\n"	;
								textbuf->subst_g(	"<PLOT_Y_AXIS_FORMAT_SPEC>",	tmp_stream.str().c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_FORMAT_SPEC>",	"<PAGE_Y_AXIS_FORMAT_SPEC>"	);
				}

				//!	Set	the	plot's	Y	AXIS	SCALING.
				if	(	plot_y_scale.compare("Log")	==	0	)	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_SCALING_SPEC>",	"set	logscale	y\n"	);
				}	else	{
								textbuf->subst_g(	"<PLOT_Y_AXIS_SCALING_SPEC>",	"<PAGE_Y_AXIS_SCALING_SPEC>"	);
				}

				//!	Set	the	plot's	Y	MIN	RANGE.
				if	(	!	plot_y_min_rng.empty()	)	{
								textbuf->subst_g(	"<PLOT_Y_MIN_RNG>",	plot_y_min_rng.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_Y_MIN_RNG>",	""	);		/*	keep	current	range	*/
				}

				//!	Set	the	plot's	Y	MAX	RANGE.
				if	(	!	plot_y_max_rng.empty()	)	{
								textbuf->subst_g(	"<PLOT_Y_MAX_RNG>",	plot_y_max_rng.c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_Y_MAX_RNG>",	""	);		/*	keep	current	range	*/
				}

				/*!
					*	Set	the	plot's	FOREGROUND	COLOR.
					*	(Feature	only	available	in	Gnuplot	version	4.2,	or	later.)
					*/
				tmp_stream.str("");
				if	(	GnuplotVersion()	<	4.2		||		plot_fg_color.empty()		||
									!color_is_valid(&plot_fg_color)	)	{
								tmp_stream	<<	"#"	;	/*	comment	out	the	textcolor	command	*/
				}
				tmp_stream	<<	"textcolor	rgb	\""	<<	plot_fg_color	<<	"\""	;
				textbuf->subst_g(	"<PLOT_TEXT_COLOR_SPEC>",	tmp_stream.str().c_str()	);
				textbuf->subst_g(	"<PLOT_FOREGROUND_SPEC>",	"<PAGE_FOREGROUND_SPEC>"	);

				/*!
					*	Set	the	plot's	BACKGROUND	COLOR.
					*	(Feature	only	available	in	Gnuplot	version	4.2,	or	later.)
					*/
				if	(	GnuplotVersion()	>=	4.2	)	{
								tmp_stream.str("");
								tmp_stream	<<	"set	object	1	rect	default;	";
								tmp_stream	<<	"unset	object	1	#Clear	previous	plot	background	color"	<<	std::endl;
								if	(	!	plot_bg_color.empty()		&&		color_is_valid(&plot_bg_color)	)	{
												tmp_stream	<<	"set	object	1	rectangle";
												tmp_stream	<<	"	from	graph	0,	graph	0	to	graph	1,	graph	1	behind"	<<	std::endl;
												tmp_stream	<<	"set	object	1	rectangle";
												tmp_stream	<<	"	fc	rgb	\""	<<	plot_bg_color	<<	"\""	<<	std::endl;
												textbuf->subst_g(	"<PLOT_BACKGROUND_SPEC>",	tmp_stream.str().c_str()	);
								}	else	{
												textbuf->subst_g(	"<PLOT_BACKGROUND_SPEC>",	tmp_stream.str().c_str()	);
								}
				}	else	{
												textbuf->subst_g(	"<PLOT_BACKGROUND_SPEC>",	"<PAGE_BACKGROUND_SPEC>"	);
				}

				//!	Enable/Disable	Grid.
				if	(	show_grid.compare("No")	==	0	)	{
								textbuf->subst_g(	"<PLOT_GRID_SPEC>",	"unset	grid"	);
								textbuf->subst_g(	"<PLOT_GRID_COLOR_SPEC>",	""	);
				}	else	{
								textbuf->subst_g(	"<PLOT_GRID_SPEC>",	"<PAGE_GRID_SPEC>"	);

								/*!
									*	Set	the	plot's	GRID	COLOR.
									*	(Feature	only	available	in	Gnuplot	version	4.2,	or	later.)
									*/
								if	(	GnuplotVersion()	>=	4.2		&&
													!	grid_color.empty()		&&		color_is_valid(&grid_color)	)	{
												tmp_stream.str("");
												tmp_stream	<<	"linecolor	rgb	\""	<<	grid_color	<<	"\""	;
												//!	Also	change	border	color	to	match	grid	color
												tmp_stream	<<	"\nset	border	linecolor	rgb	\""	<<	grid_color	<<	"\""	;
												textbuf->subst_g(	"<PLOT_GRID_COLOR_SPEC>",	tmp_stream.str().c_str()	);
								}	else	{
												textbuf->subst_g(	"<PLOT_GRID_COLOR_SPEC>",	"<PAGE_GRID_COLOR_SPEC>"	);
								}
				}

				//!	Set	the	plot's	FONT.
				if	(	!	plot_font.empty()	)	{
								tmp_stream.str("");
								//!	insert	a	backslash(\),	newline	&	indent	4	spaces
								tmp_stream	<<	"\\\n				font	\""	<<	plot_font	<<	"\""	;
								textbuf->subst_g(	"<PLOT_FONT_SPEC>",	tmp_stream.str().c_str()	);
				}	else	{
								textbuf->subst_g(	"<PLOT_FONT_SPEC>",	"<PAGE_FONT_SPEC>"	);
				}

}

