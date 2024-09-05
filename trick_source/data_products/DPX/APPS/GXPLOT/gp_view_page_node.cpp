
#include	<stdio.h>				//	for	snprintf
#include	<string>
#include	<iostream>
#include	<sstream>
#include	<stdlib.h>

#include	"gp_view_page_node.hh"
#include	"gp_version.hh"
#include	"gp_colors.hh"
#include	"gp_utilities.hh"

//#define	DEBUG	1
//	CLASS	VARIABLE	INITIALIZATION
int	GPViewPageNode::instance_count	=	0;


//	DESTRUCTOR
GPViewPageNode::~GPViewPageNode()	{

				GPViewPlotNode	*gp_view_plot_node;
				int	n,i;

				#ifdef	DEBUG
				cout	<<	"GPViewPageNode	DESTRUCTOR."	<<	endl;
				#endif

				n	=	(int)plot_node_list.size();
				for	(i=0	;	i<n;	i++	)	{
								gp_view_plot_node	=	plot_node_list[i];
								if	(gp_view_plot_node)	{	delete	gp_view_plot_node;	}
				}

}

//	CONSTRUCTOR
GPViewPageNode::GPViewPageNode(Widget	Toplevel,	DPC_page*	Page,	int	Xpos,	int	Ypos,	int	Width,	int	Height)	{

				const	char	*separator	=	"#------------------------------------------------\n";
				const	char	*new_section	=	"\n\n";
				const	char	*ncells_s;

				page	=	Page;
				toplevel	=	Toplevel;
				device	=	NOT_SET;
				#ifdef	DEBUG
				save_tmp_files	=	true;
				#endif

				if	((ncells_s	=	Page->getAttribute("hcells"))	!=	NULL)	{
								n_hcells	=	atoi(ncells_s);
								if	((n_hcells	<	0)||(n_hcells	>	10))	{
												std::cerr	<<	"Value	for	hcells	attribute	is	out	of	range.\n"
													<<	"Valid	range	for	hcells	is	1	..	10.\n"
													<<	"Ignoring	hcells	value."	<<	std::endl	;
												n_hcells	=	1;
								}
				}	else	{
								n_hcells	=	1;
				}

				if	((ncells_s	=	Page->getAttribute("vcells"))	!=	NULL)	{
								n_vcells	=	atoi(ncells_s);
								if	((n_vcells	<	0)||(n_vcells	>	10))	{
											std::cerr	<<	"Value	for	vcells	attribute	is	out	of	range.\n"
												<<	"Valid	range	for	vcells	is	1	..	10.\n"
												<<	"Ignoring	vcells	value.\n"	<<	std::endl	;
												n_vcells	=	1;
								}
				}	else	{
								n_vcells	=	1;
				}


				//!	Build	the	page	part	of	the	template.
				textbuf.print("%s",	separator);
				textbuf.print("#	(generated	for	GNUPLOT	Version	<GNUPLOT_VERSION>)\n");
				textbuf.print("#	To	execute,	run	the	following	command:\n");
				textbuf.print("#	%%	gnuplot	-persist	file_name	[ENTER]\n");
				textbuf.print("#\n");
				textbuf.print("#	Template:\n");
				textbuf.print("%s",	separator);
				textbuf.print("set	title	\"<PAGE_TITLE>\"\n");

				//!	Terminal	spec	line
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#		Set	the	output	device\n");
				textbuf.print("%s",	separator);
				if	(	GnuplotVersion()	>=	4.2	)	{
								textbuf.print("set	term	<TERM_TYPE>	<CANVAS_SIZE>	<TERM_OPTIONS>\n");
				}	else	if	(	GnuplotVersion()	>=	4.0	)	{
								textbuf.print("set	term	<TERM_TYPE>	<TERM_OPTIONS>\n");
				}	else	{
								textbuf.print("set	term	<TERM_TYPE>\n");
				}

				//!	Page	size	for	whole	page
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#		Set	\"page\"	size	for	whole	plot\n");
				textbuf.print("%s",	separator);
				textbuf.print("set	size	1.0,1.0\n");
				textbuf.print("set	origin	0.0,0.0\n");

				//!	Default	option	for	all	plots
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#		Set	default	options	for	all	plots\n");
				textbuf.print("%s",	separator);
				textbuf.print("<PAGE_GRID_SPEC>	<PAGE_GRID_COLOR_SPEC>\n");
				textbuf.print("set	nolabel\n");
				if	(	GnuplotVersion()	>=	4.0	)	{
								textbuf.print("set	mouse\n");
								textbuf.print("set	style	data	<STYLE_DATA_SPEC>\n");
								textbuf.print("set	style	function	<STYLE_FUNC_SPEC>\n");
				}

				/*!
					*	OPTIONAL	PAGE	SETTINGS
					*/
				if	(	GnuplotVersion()	>=	4.0	)	{
								textbuf.print("<PAGE_X_AXIS_SCALING_SPEC>");
								textbuf.print("<PAGE_Y_AXIS_SCALING_SPEC>");
				}
				textbuf.print("<PAGE_Y_AXIS_FORMAT_SPEC>");
				textbuf.print("<PAGE_FOREGROUND_SPEC>");
				textbuf.print("<PAGE_BACKGROUND_SPEC>");
				textbuf.print("<PAGE_KEY_SPEC>");	//	key	is	the	legend

				//!	Linestyles
				char	version_line_style[32]	;
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#		Set	linestyles\n");
				textbuf.print("#		color	scheme	may	differ	if	\"-xrm\"	option	was	used\n");
				textbuf.print("%s",	separator);
				textbuf.print("line_type_1		=	<LINE_TYPE_1>\n");
				textbuf.print("line_type_2		=	<LINE_TYPE_2>\n");
				textbuf.print("line_type_3		=	<LINE_TYPE_3>\n");
				textbuf.print("line_type_4		=	<LINE_TYPE_4>\n");
				textbuf.print("line_type_5		=	<LINE_TYPE_5>\n");
				textbuf.print("line_type_6		=	<LINE_TYPE_6>\n");
				textbuf.print("line_type_7		=	<LINE_TYPE_7>\n");
				textbuf.print("line_type_8		=	<LINE_TYPE_8>\n");
				textbuf.print("line_type_9		=	<LINE_TYPE_9>\n");
				textbuf.print("line_type_10	=	<LINE_TYPE_10>\n");
				textbuf.print("line_type_11	=	<LINE_TYPE_11>\n");
				textbuf.print("line_type_12	=	<LINE_TYPE_12>\n");
				textbuf.print("line_type_13	=	<LINE_TYPE_13>\n");

				textbuf.print("\n");
				textbuf.print("point_shape_1		=	<POINT_SHAPE_1>\n");
				textbuf.print("point_shape_2		=	<POINT_SHAPE_2>\n");
				textbuf.print("point_shape_3		=	<POINT_SHAPE_3>\n");
				textbuf.print("point_shape_4		=	<POINT_SHAPE_4>\n");
				textbuf.print("point_shape_5		=	<POINT_SHAPE_5>\n");
				textbuf.print("point_shape_6		=	<POINT_SHAPE_6>\n");
				textbuf.print("point_shape_7		=	<POINT_SHAPE_7>\n");
				textbuf.print("point_shape_8		=	<POINT_SHAPE_8>\n");
				textbuf.print("point_shape_9		=	<POINT_SHAPE_9>\n");
				textbuf.print("point_shape_10	=	<POINT_SHAPE_10>\n");
				textbuf.print("point_shape_11	=	<POINT_SHAPE_11>\n");
				textbuf.print("point_shape_12	=	<POINT_SHAPE_12>\n");
				textbuf.print("point_shape_13	=	<POINT_SHAPE_13>\n");

				//!Map	Symbol	Style	pulldown	menu	to	internal	gnuplot	shape	indexes
				textbuf.print("\n");
				textbuf.print("None										=	-1\n");
				textbuf.print("Square								=	4\n");
				textbuf.print("Circle								=	6\n");
				textbuf.print("Star										=	3\n");
				textbuf.print("XX												=	2\n");
				textbuf.print("Triangle						=	8\n");
				textbuf.print("Solid_Square		=	5\n");
				textbuf.print("Solid_Circle		=	7\n");
				textbuf.print("Thick_Square		=	5\n");

				//!	Define	a	numerical	value	for	the	Symbol	Size	pulldown	menu
				textbuf.print("\n");
				textbuf.print("Tiny			=	0.4\n");
				textbuf.print("Small		=	0.8\n");
				textbuf.print("Medium	=	1.4\n");
				textbuf.print("Large		=	3.0\n");

				textbuf.print("\n");
				if	(	GnuplotVersion()	>=	4.0	)	{
												strcpy(version_line_style,	"style	line")	;
				}	else	{
												strcpy(version_line_style,	"linestyle")	;
				}
				textbuf.print("set	%s		1		<LINE_SPEC_1>		<LINE_COLOR_1>\n",	version_line_style);
				textbuf.print("set	%s		2		<LINE_SPEC_2>		<LINE_COLOR_2>\n",	version_line_style);
				textbuf.print("set	%s		3		<LINE_SPEC_3>		<LINE_COLOR_3>\n",	version_line_style);
				textbuf.print("set	%s		4		<LINE_SPEC_4>		<LINE_COLOR_4>\n",	version_line_style);
				textbuf.print("set	%s		5		<LINE_SPEC_5>		<LINE_COLOR_5>\n",	version_line_style);
				textbuf.print("set	%s		6		<LINE_SPEC_6>		<LINE_COLOR_6>\n",	version_line_style);
				textbuf.print("set	%s		7		<LINE_SPEC_7>		<LINE_COLOR_7>\n",	version_line_style);
				textbuf.print("set	%s		8		<LINE_SPEC_8>		<LINE_COLOR_8>\n",	version_line_style);
				textbuf.print("set	%s		9		<LINE_SPEC_9>		<LINE_COLOR_9>\n",	version_line_style);
				textbuf.print("set	%s	10		<LINE_SPEC_10>		<LINE_COLOR_10>\n",	version_line_style);
				textbuf.print("set	%s	11		<LINE_SPEC_11>		<LINE_COLOR_11>\n",	version_line_style);
				textbuf.print("set	%s	12		<LINE_SPEC_12>		<LINE_COLOR_12>\n",	version_line_style);
				textbuf.print("set	%s	13		<LINE_SPEC_13>		<LINE_COLOR_13>\n",	version_line_style);

				//!	Y	Axes	alignment	command
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#		Force	y-axes	to	align\n");
				textbuf.print("%s",	separator);
				textbuf.print("set	lmargin	7\n");

				//!	Titles	for	whole	page
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#	Add	title(s)	for	the	whole	page\n");
				textbuf.print("%s",	separator);


				//!	Page	Title
				textbuf.print("set	label	<LABEL_1_TAG>	\\\n");
				textbuf.print("				\"<LABEL_1_NAME>\"	\\\n");
				textbuf.print("				at	screen	<LABEL_1_XPOS>,\\\n");
				textbuf.print("							screen	<LABEL_1_YPOS>,\\\n");
				textbuf.print("							screen	0.0	<LABEL_1_JUST>	\\\n");
				textbuf.print("							<PAGE_TEXT_COLOR_SPEC>	\\\n");
				textbuf.print("							font	<LABEL_1_FONT>\n\n");

				textbuf.print("set	label	<LABEL_2_TAG>	\\\n");
				textbuf.print("				\"<LABEL_2_NAME>\"	\\\n");
				textbuf.print("				at	screen	<LABEL_2_XPOS>,\\\n");
				textbuf.print("							screen	<LABEL_2_YPOS>,\\\n");
				textbuf.print("							screen	0.0	<LABEL_2_JUST>	\\\n");
				textbuf.print("							<PAGE_TEXT_COLOR_SPEC>	\\\n");
				textbuf.print("							font	<LABEL_2_FONT>\n\n");

				//!	Another	title	with	Simname/Runname
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#	Add	label(s)	for	the	sim	and	run	names\n");
				textbuf.print("%s",	separator);

				textbuf.print("set	label	<LABEL_3_TAG>	\\\n");
				textbuf.print("				\"<LABEL_3_NAME>\"	\\\n");
				textbuf.print("				at	screen	<LABEL_3_XPOS>,	\\\n");
				textbuf.print("							screen	<LABEL_3_YPOS>,	\\\n");
				textbuf.print("							screen	0.0	<LABEL_3_JUST>	\\\n");
				textbuf.print("							<PAGE_TEXT_COLOR_SPEC>	\\\n");
				textbuf.print("							font	<LABEL_3_FONT>\n\n");

				//!	Date	(i.e.,	4/2/2009)
				textbuf.print("set	label	<LABEL_4_TAG>	\\\n");
				textbuf.print("				\"<LABEL_4_NAME>\"	\\\n");
				textbuf.print("				at	screen	<LABEL_4_XPOS>,	\\\n");
				textbuf.print("							screen	<LABEL_4_YPOS>,	\\\n");
				textbuf.print("							screen	0.0	<LABEL_4_JUST>	\\\n");
				textbuf.print("							<PAGE_TEXT_COLOR_SPEC>	\\\n");
				textbuf.print("							font	<LABEL_4_FONT>\n\n");

				//!	Multiplot	command
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#	Multiplot	mode\n");
				textbuf.print("%s",	separator);
				textbuf.print("set	multiplot\n");

				//!	Plot	sections
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#	Set	defs	for	plot	sizes	and	locations\n");
				textbuf.print("%s",	separator);
				textbuf.print("plot_width		=	<PLOT_WIDTH>\n");
				textbuf.print("plot_height	=	<PLOT_HEIGHT>\n");
				textbuf.print("plot_title_xpos	=	<PLOT_TITLE_XPOS>\n");
				textbuf.print("plot_title_ypos	=	<PLOT_TITLE_YPOS>\n");

				textbuf.print("\n");
				textbuf.print("x_axis_label_xpos	=	<X_AXIS_LABEL_XPOS_DECL>\n");
				textbuf.print("x_axis_label_ypos	=	<X_AXIS_LABEL_YPOS_DECL>\n");
				textbuf.print("y_axis_label_xpos	=	<Y_AXIS_LABEL_XPOS_DECL>\n");
				textbuf.print("y_axis_label_ypos	=	<Y_AXIS_LABEL_YPOS_DECL>\n");

				//	-----------------
				instance_count	++;
}

static	void	layout_page	(GPViewPageNode*	gp_view_page_node	)	{

				int	n_horizontal_cells,	n_vertical_cells;
				double	cell_width,	cell_height;
				int	number_of_plots;
				int	plot_ix;
				char	value_text[10];

				number_of_plots	=	(int)gp_view_page_node->plot_node_list.size();

				n_horizontal_cells	=	gp_view_page_node->n_hcells;
				n_vertical_cells			=	gp_view_page_node->n_vcells;

				while((n_horizontal_cells	*	n_vertical_cells)	<	number_of_plots	)	{
							if	(n_horizontal_cells	<=	n_vertical_cells)	{
											n_horizontal_cells++;
							}	else	{
											n_vertical_cells++;
							}
				}

				//	CALCULATE	PLOT	WIDTH
				cell_width		=	1.0	/	n_horizontal_cells;
				snprintf(value_text,	sizeof(value_text),	"%5.3f",	cell_width);
				gp_view_page_node->textbuf.subst("<PLOT_WIDTH>",	value_text);

				//	CALCULATE	PLOT	HEIGHT
				cell_height	=	1.0	/	n_vertical_cells;
				snprintf(value_text,	sizeof(value_text),	"%5.3f",	cell_height);
				gp_view_page_node->textbuf.subst("<PLOT_HEIGHT>",	value_text);

				for	(plot_ix=0;	plot_ix<	number_of_plots;	plot_ix++)	{
								float	x,y;
								char	key_text[20];

								snprintf(key_text,	sizeof(key_text),	"<PLOT_ORIGIN_X_%d>",	plot_ix);
								x	=	(float)((plot_ix	%	n_horizontal_cells)	*	cell_width);
								snprintf(value_text,	sizeof(value_text),	"%5.3f",	x);
								gp_view_page_node->textbuf.subst(key_text,	value_text);


								snprintf(key_text,	sizeof(key_text),	"<PLOT_ORIGIN_Y_%d>",	plot_ix);
								y	=	(float)(1.0	-	((plot_ix	/	n_horizontal_cells)	*	cell_height	+	cell_height));
								snprintf(value_text,	sizeof(value_text),	"%5.3f",	y);
								gp_view_page_node->textbuf.subst(key_text,	value_text);
				}
}

void	GPViewPageNode::finalize()	{

				int	ii;
				const	char	*separator	=	"#------------------------------------------------\n";
				const	char	*new_section	=	"\n\n";
				string							page_title;
				string							page_legend;
				string							page_fg_color;
				string							page_bg_color;
				string							page_font;	/*	X11	font	naming	convention	*/
				char	gp_template_file_name[512];
				char	outFileName[512];
				string	tmp_string;
				stringstream	tmp_stream;
				stringstream	cmd;
				string	extension;
				string::size_type	idx;
				int	xx_geom	=	850;		//default	gnuplot	window	width
				int	yy_geom	=	657;		//default	gnuplot	window	height
				int	max_num_curves	=	0;

				/*!
					*	Gnuplot	knows	a	limited	number	of	color	names.
					*	To	see	the	list	of	known	color	names,	run...
					*	%	gnuplot	-e	'show	palette	colornames'
					*/
				string	plotColors[]=	{	"black",	"red",	"blue",	"green",	"magenta",
																											"yellow",	"cyan",	"pink",	"dark-green",
																											"light-blue",		"purple",	"orange",	"brown"	};
				int	n_CustomColors	=	sizeof(plotColors)/sizeof(plotColors[0]);

				/**!
					*	WARNING:	getAttribute()	returns	a	const	char	*,	which	may	be	null.
					*										Assigning	a	null	pointer	to	a	string	will	crash	the	plot.
					*										So,	check	the	return	value	before	making	the	assignment.
					*/
				page_title							=	(page->getTitle())	?		page->getTitle()	:	"Page";
				page_legend						=	(page->getAttribute("legend"))	?		page->getAttribute("legend")	:	"";
				page_bg_color				=	(page->getAttribute("background_color"))	?		page->getAttribute("background_color")	:	"";
				page_fg_color				=	(page->getAttribute("foreground_color"))	?		page->getAttribute("foreground_color")	:	"";
				page_font								=	(page->getAttribute("font"))	?		page->getAttribute("font")	:	"";


				//!	Nomultiplot	command
				textbuf.print("\n");
				textbuf.print("%s",	new_section);
				textbuf.print("%s",	separator);
				textbuf.print("#	For	some	terminals,	plots	are	not	displayed\n");
				textbuf.print("#	until	the	command	`unset	multiplot`	is	given.\n");
				textbuf.print("#	Return	to	single	plot	mode	to	switch	device\n");
				textbuf.print("%s",	separator);
				textbuf.print("unset	multiplot\n");


				//!	Get	Gnuplot	Terminal	setting
				terminalType_txt	=	(page->getAttribute("gnuplot_terminal"))	?		page->getAttribute("gnuplot_terminal")	:	"";
				if	(	strcasecmp(terminalType_txt.c_str(),	"X11")	==	0	)	{

								terminalType	=	GPViewPageNode::X11;
								terminalType_txt	=	"x11";

				}	else	if	(	strcasecmp(terminalType_txt.c_str(),	"PS")	==	0	)	{

								terminalType	=	GPViewPageNode::PS;
								terminalType_txt	=	"postscript";
								extension	=	".ps";

				}	else	if	(	strcasecmp(terminalType_txt.c_str(),	"PS_COLOR")	==	0	)	{

								terminalType	=	GPViewPageNode::PS_COLOR;
								terminalType_txt	=	"postscript";
								extension	=	".ps";

				}	else	if	(	strcasecmp(terminalType_txt.c_str(),	"PNG")	==	0	)	{

								terminalType	=	GPViewPageNode::PNG;
								terminalType_txt	=	"png";
								extension	=	".png";

				}	else	if	(	strcasecmp(terminalType_txt.c_str(),	"EPS")	==	0	)	{

								terminalType	=	GPViewPageNode::EPS;
								terminalType_txt	=	"eps";
								extension	=	".eps";

				}	else	if	(	strcasecmp(terminalType_txt.c_str(),	"AQUA")	==	0	)	{

								terminalType	=	GPViewPageNode::AQUA;
								terminalType_txt	=	"x11";

				}		else	{
								cerr	<<	"ERROR:	Bad	terminal-type	spec:	\""	<<	tmp_string	<<	"\""	<<	endl;
								cerr	<<	"Defaulting	terminal-type	to	\"x11.\""	<<	endl;
								terminalType	=	GPViewPageNode::X11	;
								terminalType_txt	=	"x11";
				}


				//!	Get	Destination	setting
				device_txt	=	(page->getAttribute("device"))	?		page->getAttribute("device")	:	"";
				if	(	strcasecmp(device_txt.c_str(),	"TERMINAL")	==	0	)	{

								device	=	GPViewPageNode::TERMINAL;

				}	else	if	(	strncasecmp(device_txt.c_str(),	"FILE",	4)	==	0	)	{

								device	=	GPViewPageNode::DISK	;
								//!	copy	from	first	space	character	to	end	of	string
								diskFileName	=	device_txt.substr(	device_txt.find("	")	);

								//!	output	may	not	redirect	to	file	format	with	some	terminal	types
								switch	(	terminalType	)	{
												case	GPViewPageNode::X11:
												case	GPViewPageNode::AQUA:
																//!	Force	to	a	different	format	before	going	to	file.
																terminalType_txt	=	"postscript";
																extension	=	".ps";
																break;
												default:
																break;
								}

				}	else	if	(	strcasecmp(device_txt.c_str(),	"PRINTER")	==	0	)	{

								device	=	GPViewPageNode::PRINTER;
								terminalType_txt	=	"postscript";

				}		else	{
								cerr	<<	"ERROR:	Bad	device	spec:	\""	<<	device_txt	<<	"\""	<<	endl;
								cerr	<<	"Defaulting	device	to	\"Terminal.\""	<<	endl;
								device	=	GPViewPageNode::TERMINAL;
				}


				//	==================================================
				//																POPULATE	THE	TEMPLATE
				//	==================================================
				tmp_stream.str("");
				tmp_stream	<<	GnuplotVersion()	;
				textbuf.subst_g("<GNUPLOT_VERSION>",	tmp_stream.str().c_str()	);
				textbuf.subst_g("<PAGE_TITLE>",	page_title.c_str());
				textbuf.subst_g("<TERM_TYPE>",	terminalType_txt.c_str());
				if	(	(plot_node_list.size()%5	==	2		||		plot_node_list.size()%5	==	3)		&&
										plot_node_list.size()	>	2	)	{
								xx_geom	=	600;
								yy_geom	=	720;
				}
				tmp_stream.str("");
				tmp_stream	<<	"size	"	<<	xx_geom	<<	","	<<	yy_geom	;
				textbuf.subst_g("<CANVAS_SIZE>",	tmp_stream.str().c_str()	);
				if	(	strcasecmp(terminalType_txt.c_str(),	"x11")	==	0	)	{
								if	(	GnuplotVersion()	>=	4.0	)	{
												textbuf.subst_g("<TERM_OPTIONS>",	"persist");
								}
				}	else	{
								textbuf.subst_g("<TERM_OPTIONS>",	"");
				}

				//!	Enable/Disable	Grid
				textbuf.subst_g("<PAGE_GRID_SPEC>",	"set	grid");
				if	(	GnuplotVersion()	>=	4.2		&&
												!	page_fg_color.empty()		&&		color_is_valid(&page_fg_color)	)	{
								tmp_stream.str("");
								tmp_stream	<<	"linecolor	rgb	\""	<<	page_fg_color	<<	"\""	;
								//!	Also	change	border	color	to	match	grid	color
								tmp_stream	<<	"\nset	border	linecolor	rgb	\""	<<	page_fg_color	<<	"\""	;
								textbuf.subst_g(	"<PAGE_GRID_COLOR_SPEC>",	tmp_stream.str().c_str()	);
				}	else	{
								textbuf.subst_g(	"<PAGE_GRID_COLOR_SPEC>",	""	);
				}

				//!	plotting	style	for	Data	plots
				textbuf.subst_g("<STYLE_DATA_SPEC>",	"lines");
				//!	plotting	style	for	Function	plots
				textbuf.subst_g("<STYLE_FUNC_SPEC>",	"lines");

				textbuf.subst_g("<PAGE_X_AXIS_SCALING_SPEC>",	"unset	logscale	x\n");
				textbuf.subst_g("<PAGE_Y_AXIS_FORMAT_SPEC>",	"set	format	y\n");
				textbuf.subst_g("<PAGE_Y_AXIS_SCALING_SPEC>",	"unset	logscale	y\n");
				textbuf.subst_g("<PAGE_FOREGROUND_SPEC>",	"");
				textbuf.subst_g("<PAGE_BACKGROUND_SPEC>",	"");

				//!	Enable/Disable	key	(legend).
				if	(	page_legend.compare("No")	==	0	)	{
								if	(	GnuplotVersion()	>=	4.0	)	{
																textbuf.subst_g("<PAGE_KEY_SPEC>",	"set	key	off\n");
								}	else	{
																textbuf.subst_g("<PAGE_KEY_SPEC>",	"set	nokey\n");
								}
				}	else	{
								string	vert_spacing_	=	"";
								if	(	GnuplotVersion()	<	4.2	)	{
												vert_spacing_	=	"	spacing	0.5";
								}
								tmp_stream.str("");
								tmp_stream	<<	"set	key	below	Left	reverse"	<<	vert_spacing_	<<	"\n"	;
								textbuf.subst_g(	"<PAGE_KEY_SPEC>",	tmp_stream.str().c_str()	);

								/*	Increase	window	size	so	key	doesn't	cause	graph	to	shrink	*/
								for	(ii	=	0;		ii	<	(int)plot_node_list.size();		ii++)	{
												if	((int)plot_node_list[ii]->curve_node_list.size()	>	max_num_curves)	{
																max_num_curves	=	plot_node_list[ii]->curve_node_list.size();
												}
								}
								yy_geom	+=	max_num_curves*10;		//	stretch	10	pixels	per	Y-var	label
				}

				//!	Set	Font
				if	(		!	page_font.empty()	)	{
								tmp_stream.str("");
								//!	insert	a	backslash(\),	newline	&	indent	4	spaces
								tmp_stream	<<	"\\\n				font	\""	<<	page_font	<<	"\""	;
								textbuf.subst_g(	"<PAGE_FONT_SPEC>",	tmp_stream.str().c_str()	);
				}	else	{
								textbuf.subst_g("<PAGE_FONT_SPEC>",	"\\\n				font	<DEFAULT_FONT>");
				}

				textbuf.subst_g("<LINE_TYPE_1>",		"-1	#black");
				textbuf.subst_g("<LINE_TYPE_2>",		"	1	#red");
				textbuf.subst_g("<LINE_TYPE_3>",		"	3	#blue");
				textbuf.subst_g("<LINE_TYPE_4>",		"	2	#green");
				textbuf.subst_g("<LINE_TYPE_5>",		"	0	#magenta");
				textbuf.subst_g("<LINE_TYPE_6>",		"	7	#yellow");
				textbuf.subst_g("<LINE_TYPE_7>",		"	5	#cyan");
				textbuf.subst_g("<LINE_TYPE_8>",		"	0	#pink");
				textbuf.subst_g("<LINE_TYPE_9>",		"	0	#dark-green");
				textbuf.subst_g("<LINE_TYPE_10>",	"	0	#light-blue");
				textbuf.subst_g("<LINE_TYPE_11>",	"	4	#purple");
				textbuf.subst_g("<LINE_TYPE_12>",	"	8	#orange");
				textbuf.subst_g("<LINE_TYPE_13>",	"	6	#brown");

				textbuf.subst_g("<POINT_SHAPE_1>",		"	1	#plus");
				textbuf.subst_g("<POINT_SHAPE_2>",		"	2	#cross");
				textbuf.subst_g("<POINT_SHAPE_3>",		"	3	#astrisk");
				textbuf.subst_g("<POINT_SHAPE_4>",		"	4	#open_square");
				textbuf.subst_g("<POINT_SHAPE_5>",		"	5	#filled_square");
				textbuf.subst_g("<POINT_SHAPE_6>",		"	6	#open_circle");
				textbuf.subst_g("<POINT_SHAPE_7>",		"	7	#filled_circle");
				textbuf.subst_g("<POINT_SHAPE_8>",		"	8	#open_triangle");
				textbuf.subst_g("<POINT_SHAPE_9>",		"	9	#filled_triangle");
				textbuf.subst_g("<POINT_SHAPE_10>",	"10	#open_inverse-triangle");
				textbuf.subst_g("<POINT_SHAPE_11>",	"11	#filled_inverse-triangle");
				textbuf.subst_g("<POINT_SHAPE_12>",	"12	#open_diamond");
				textbuf.subst_g("<POINT_SHAPE_13>",	"13	#filled_diamond");

				textbuf.subst_g("<LINE_SPEC_1>",	"lw	1.0		pt	point_shape_1			ps	Small");
				textbuf.subst_g("<LINE_SPEC_2>",	"lw	1.0		pt	point_shape_2			ps	Small");
				textbuf.subst_g("<LINE_SPEC_3>",	"lw	1.0		pt	point_shape_3			ps	Small");
				textbuf.subst_g("<LINE_SPEC_4>",	"lw	1.0		pt	point_shape_4			ps	Small");
				textbuf.subst_g("<LINE_SPEC_5>",	"lw	1.0		pt	point_shape_5			ps	Small");
				textbuf.subst_g("<LINE_SPEC_6>",	"lw	1.0		pt	point_shape_6			ps	Small");
				textbuf.subst_g("<LINE_SPEC_7>",	"lw	1.0		pt	point_shape_7			ps	Small");
				textbuf.subst_g("<LINE_SPEC_8>",	"lw	1.0		pt	point_shape_8			ps	Small");
				textbuf.subst_g("<LINE_SPEC_9>",	"lw	1.0		pt	point_shape_9			ps	Small");
				textbuf.subst_g("<LINE_SPEC_10>","lw	1.0		pt	point_shape_10		ps	Small");
				textbuf.subst_g("<LINE_SPEC_11>","lw	1.0		pt	point_shape_11		ps	Small");
				textbuf.subst_g("<LINE_SPEC_12>","lw	1.0		pt	point_shape_12		ps	Small");
				textbuf.subst_g("<LINE_SPEC_13>","lw	1.0		pt	point_shape_13		ps	Small");

				if	(	GnuplotVersion()	>=	4.2	)	{
								for	(ii	=	0;		ii	<	n_CustomColors;			ii++)	{
												//!	build	template	(e.g.	"<LINE_COLOR_1>")
												tmp_stream.str("");
												tmp_stream	<<	"<LINE_COLOR_"	<<	ii+1	<<	">"	;
												tmp_string	=	tmp_stream.str();
												//!	build	string	(e.g.	"linecolor	rgb	\"black\"")
												tmp_stream.str("");
												tmp_stream	<<	"linecolor	rgb	\""	<<	plotColors[ii]	<<	"\""	;
												//!	Replace	template	with	string
												textbuf.subst_g(	tmp_string.c_str(),	tmp_stream.str().c_str()	);
								}
				}	else	{
								/*!
									*	Change	gnuplot's	default	line	colors	(1=red	2=green	3=blue
									*	4=magenta	5=cyan	6=sienna	7=orange	8=coral)	for	plot.
									*	Note:	This	is	only	used	when	co-plotting	multiple	RUN_	dirs.
									*/
								for	(ii	=	0;		ii	<	n_CustomColors;			ii++)	{
												tmp_stream.str("");
												tmp_stream	<<	"<LINE_COLOR_"	<<	ii+1	<<	">"	;
												//!	Clear	template;	use	new	line#Color	definitions	from	"-xrm"	options
												textbuf.subst_g(	tmp_stream.str().c_str(),	""	);
								}
				}

				tmp_stream.str("");
				if	(	GnuplotVersion()	<	4.2		||		page_fg_color.empty()		||
									!page_fg_color.empty()	)	{
								tmp_stream	<<	"#"	;		/*	comment	out	the	textcolor	command	*/
				}
				tmp_stream	<<	"textcolor	rgb	\""	<<	page_fg_color	<<	"\""	;
				textbuf.subst_g("<PAGE_TEXT_COLOR_SPEC>",	tmp_stream.str().c_str());

				textbuf.subst_g("<LABEL_1_TAG>",	"1");
				textbuf.subst_g("<LABEL_1_NAME>",	page_title.c_str());
				textbuf.subst_g("<LABEL_1_XPOS>",	"0.51");
				textbuf.subst_g("<LABEL_1_YPOS>",	"0.97");
				textbuf.subst_g("<LABEL_1_JUST>",	"center");
				textbuf.subst_g("<LABEL_1_FONT>",	"\"Helvetica,14\"");

				textbuf.subst_g("<LABEL_2_TAG>",	"2");
				textbuf.subst_g("<LABEL_2_NAME>",	"");
				textbuf.subst_g("<LABEL_2_XPOS>",	"0.51");
				textbuf.subst_g("<LABEL_2_YPOS>",	"0.95");
				textbuf.subst_g("<LABEL_2_JUST>",	"center");
				textbuf.subst_g("<LABEL_2_FONT>",	"\"Helvetica,10\"");

				textbuf.subst_g("<LABEL_3_TAG>",	"3");
				textbuf.subst_g("<LABEL_3_NAME>",	"`whoami`");
				textbuf.subst_g("<LABEL_3_XPOS>",	"0.99");
				textbuf.subst_g("<LABEL_3_YPOS>",	"0.99");
				textbuf.subst_g("<LABEL_3_JUST>",	"right");
				textbuf.subst_g("<LABEL_3_FONT>",	"\"Helvetica,8\"");

				textbuf.subst_g("<LABEL_4_TAG>",	"4");
				textbuf.subst_g("<LABEL_4_NAME>",	"`date	+%m/%d/%Y`");
				textbuf.subst_g("<LABEL_4_XPOS>",	"0.99");
				textbuf.subst_g("<LABEL_4_YPOS>",	"0.957");
				textbuf.subst_g("<LABEL_4_JUST>",	"right");
				textbuf.subst_g("<LABEL_4_FONT>",	"\"Helvetica,8\"");

				textbuf.subst_g("<PLOT_TITLE_XPOS>",	"0.0");
				textbuf.subst_g("<PLOT_TITLE_YPOS>",	"0.2");
				textbuf.subst_g("<X_AXIS_LABEL_XPOS_DECL>",	"0.0");
				textbuf.subst_g("<X_AXIS_LABEL_YPOS_DECL>",	"0.6");
				if	(	GnuplotVersion()	<	4.2	)	{
								textbuf.subst_g("<Y_AXIS_LABEL_XPOS_DECL>",	"0.6");
				}	else	{
								textbuf.subst_g("<Y_AXIS_LABEL_XPOS_DECL>",	"1.4");
				}
				textbuf.subst_g("<Y_AXIS_LABEL_YPOS_DECL>",	"0.0");

				textbuf.subst_g("<DEFAULT_FONT>",		"\"Helvetica,12\"");

				layout_page	(	this	);

				//!	Figure	out	where	we	can	create	a	temporary	file.
				if	(!access("/tmp",	W_OK))	{
								strncpy(gp_template_file_name,
																"/tmp/dpx_gp_page_XXXXXX",	sizeof(gp_template_file_name));

				}	else	if	(!access("/var/tmp",	W_OK))	{
								strncpy(gp_template_file_name,
																"/var/tmp/dpx_gp_page_XXXXXX",	sizeof(gp_template_file_name));

				}	else	{
								std::cerr	<<	"Unable	to	access	/tmp	or	/var/tmp,	where	a	temporary\n"
												<<	"file	(that	represents	gnuplot	commands)	needs\n"
												<<	"to	be	created.	Please	check	your	permissions."	<<	std::endl	;
								return;
				}

				//!	Create	a	name	for	our	temporary	gnuplot	command	file.
				if	(mkstemp(	gp_template_file_name)	<	0)	{
								std::cerr	<<	"Unable	to	generate	a	temporary	file"
												<<	"name	for	some	mind	boggling	reason."	<<	std::endl	;
								return;
				}


				if	(	save_tmp_files	)	{

								cout	<<	"Keeping	data	associated	with	this	gnuplot	file	:	"	<<	gp_template_file_name	<<	endl;

								//!	Comment	out	all	commands	that	delete	temp	data	files
								textbuf.subst_g("<SAVE_TMP_FILE_SPEC>",	"#");

				}	else	{

								//!	Allow	all	temp	data	files	to	be	deleted
								textbuf.subst_g("<SAVE_TMP_FILE_SPEC>",	"");

				}


				//!	If	user	would	like	to	see	this	template,	print	it	to	stdout
				//if	(args.generate_template)	{
				//				cout	<<	textbuf.getText()	<<	endl;
				//}

				/*!
					*	SAVE	buffer	TO	FILE	with	given	filename.
					*	Note:	must	save	to	file	prior	to	sending	to	PRINTER	device.
					*/
				textbuf.writeFile(	(const	char*)gp_template_file_name	);


				cmd	<<	"gnuplot";
				if	(	!	page_fg_color.empty()		&&		color_is_valid(&page_fg_color)	)	{
								cmd	<<	"	-xrm	'gnuplot*textColor:"	<<	page_fg_color	<<	"'"	;
								cmd	<<	"	-xrm	'gnuplot*borderColor:"	<<	page_fg_color	<<	"'"	;
								cmd	<<	"	-xrm	'gnuplot*axisColor:"	<<	page_fg_color	<<	"'"	;
				}
				if	(	!	page_bg_color.empty()		&&		color_is_valid(&page_bg_color)	)	{
								cmd	<<	"	-xrm	'gnuplot*background:"	<<	page_bg_color	<<	"'"	;
				}
				/*!
					*	Set	X11	line	colors.
					*	Line	colors	are	global	per	page	in	gnuplot.
					*	And	there	are	at	max	8	default	colors.	(%man	gnuplot)
					*/
				for	(ii	=	0;		ii	<	n_CustomColors;			ii++)	{
								cmd	<<	"	-xrm	'gnuplot*line"	<<	ii+1	<<	"Color:"	<<	plotColors[ii]	<<	"'"	;
				}

				if	(	device	>	GPViewPageNode::NOT_SET	)	{
												cerr				<<	"ERROR:	GPViewPageNode::finalize()	called	with	invalid	device.\n"
																				<<	"Example	devices	are	TERMINAL,	DISK	and	PRINTER.\n"
																				<<	"It	is	an	enumerated	type	in	the	GPViewPageNode	class.\n"	;
												exit(-1);
				}

				switch	(	device	)	{

								case	GPViewPageNode::DISK:

												if	(	diskFileName.empty()	)	{
																cerr	<<	"ERROR:	GPViewPageNode::finalize()	called	without	"
																					<<	"setting	output	file.		When	you	are	sending	data	"
																					<<	"to	disk,	you	must	set	output	file	name."
																					<<	endl	;
																exit(-1);
												}

												//!	Remove	any	quotes	from	string
												while	(	1	)	{
																idx	=	diskFileName.find('"');
																if	(	idx	!=	string::npos	)	{
																				diskFileName.erase(idx,1);
																}	else	{
																				break	;
																}
												}

												/*!
													*	The	weird	rfind(),	substr()	and	erase()	are	used
													*	to	make	sure	that	if	the	outfile	name	already
													*	has	an	extension	that	we	don't	duplicate	it
													*/
												idx	=	diskFileName.rfind('.');
												if	(	idx	!=	string::npos	)	{
																if	(diskFileName.substr(idx)	==	extension)	{
																				diskFileName.erase(idx);
																}
												}

												tmp_stream.str("");
												tmp_stream	<<	instance_count	;
												diskFileName.append(	tmp_stream.str()	);
												diskFileName.append(	extension	);

												//!	Complete	command	by	generating	an	output	file	from	the	gnuplot	file.
												cmd	<<	"	"	<<	gp_template_file_name	<<	"	>&	"	<<	diskFileName;
												cout	<<	"Generating	"	<<	diskFileName	<<	"..."	<<	endl;
												cout.flush();
												if	(	system(cmd.str().c_str())	>	-1	)	{
																cout	<<	"Done."	<<	endl;
												}

												//!	Run	postscript	file	through	a	filter
												if	(	terminalType	==	PS	)	{
																filterPostScriptBW(	diskFileName.c_str()	);
												}	else	if	(	terminalType	==	PS_COLOR	||	terminalType	==	EPS)	{
																filterPostScriptColor(	diskFileName.c_str(),	this	);
												}

												break;

								case	GPViewPageNode::PRINTER:

												//!	Figure	out	where	we	can	create	a	temporary	file.
												if	(!access("/tmp",	W_OK))	{
																strncpy(outFileName,
																								"/tmp/dpx_gxplot_XXXXXX",	sizeof(outFileName));

												}	else	if	(!access("/var/tmp",	W_OK))	{
																strncpy(outFileName,
																								"/var/tmp/dpx_gxplot_XXXXXX",	sizeof(outFileName));

												}	else	{
																std::cerr	<<	"Unable	to	access	/tmp	or	/var/tmp,	where	a	temporary\n"
																				<<	"file	for	printing	needs	to	be	created.\n"
																				<<	"Please	check	your	permissions."	<<	std::endl	;
																return;
												}

												//!	Create	a	name	for	our	temporary	gnuplot	command	file.
												if	(mkstemp(outFileName)	<	0)	{
																std::cerr	<<	"Unable	to	generate	a	temporary	file"
																				<<	"name	for	some	mind	boggling	reason."	<<	std::endl	;
																return;
												}

												//!	Complete	command	by	generating	an	output	file	from	the	gnuplot	file.
												cmd	<<	"	"	<<	gp_template_file_name	<<	"	>&	"	<<	outFileName;
												cout	<<	"Generating	output	file	:	"	<<	outFileName	<<	"..."	<<	endl;
												if	(	system(cmd.str().c_str())	>	-1	)	{
																cout	<<	"Done."	<<	endl;
												}

												//!	Run	postscript	file	through	a	filter
												filterPostScriptBW(outFileName);

												//!	Print	the	file
												if	(	printPS((const	char*)outFileName)	>	-1	)	{
																cout	<<	"Complete."	<<	endl;
																cout.flush();
												}

												if	(	!save_tmp_files	)	{
																//!	Clean	up	output	file.
																tmp_stream.str("");
																tmp_stream	<<	"rm	-f	"	<<	outFileName	;
																tmp_stream	<<	";	rm	-f	"	<<	gp_template_file_name;
																system(	tmp_stream.str().c_str()	);
												}

												break;

								case	GPViewPageNode::NOT_SET:
												break;

								case	GPViewPageNode::TERMINAL:
								default:

												cmd	<<	"	-persist";
												cmd	<<	"	-geometry	"	<<	xx_geom	<<	"x"	<<	yy_geom	;	//e.g.	850x657

												//!	Complete	the	command	by	providing	the	gnuplot	file	name.
												cmd	<<	"	"	<<	gp_template_file_name	;

												if	(	!save_tmp_files	)	{
																/*!
																*	After	the	plot	is	drawn,	add	another	command	to	delete
																*	the	temporary	gnuplot	file:	'/tmp/dpx_gp_page_******'.
																*/
																cmd	<<	";	rm	-f	"	<<	gp_template_file_name;
												}

												/*!
													*	Run	the	system	command	to	create	the	plot.
													*	Example:
													*				%	"gnuplot	-persist	-geometry	850x657	\
													*							-xrm	'gnuplot*textColor:black'	/tmp/dpx_gp_page_******"
													*/
												system(	cmd.str().c_str()	);

												break;

				}

}


