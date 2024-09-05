
#include	<string>
#include	<iostream>
#include	<sstream>
#include	<fstream>
#include	<unistd.h>

#include	"gp_view_curve_node.hh"
#include	"gp_version.hh"
#include	"gp_colors.hh"

//	CLASS	VARIABLE	INITIALIZATION
int	GPViewCurveNode::instance_count	=	0;

//	DESTRUCTOR
GPViewCurveNode::~GPViewCurveNode()	{

}

//	CONSTRUCTOR
GPViewCurveNode::GPViewCurveNode(	Widget										Toplevel,
																																		DPV_TextBuffer	*Textbuf,
																																		DPC_curve						*Curve,
																																		int													Curve_number
																																)	{

				const	char	*	presentation;
				double				x,	y;
				std::string							curve_x_label;
				std::string							curve_x_min_rng;
				std::string							curve_x_max_rng;
				std::string							curve_x_scale_factor;
				std::string							curve_x_bias;
				std::string							curve_y_label;
				std::string							curve_y_min_rng;
				std::string							curve_y_max_rng;
				std::string							curve_y_scale_factor;
				std::string							curve_y_bias;
				std::string							curve_line_color;
				std::string							curve_line_style;
				std::string							curve_gp_line_style;
				std::string							curve_symbol_style;
				std::string							curve_symbol_size;
				std::string							tmp_string;

				toplevel	=	Toplevel;
				dpc_curve	=	Curve;

				data_src_label	=	(Curve->getDataSrcLbl())	?	Curve->getDataSrcLbl()	:	""	;

				/*
					If	present,	the	curves	label	is	THE	label	of	the	curve.	It	represents	what	the
					user	wants	to	see	and	should	not	be	altered.	If	it	is	not	present,	then	a	label
					is	generated	from	the	variable	name	and	the	units.
					*/
				if	(Curve->getXAttribute("label"))	{
								curve_x_label	=	Curve->getXAttribute("label");
				}	else	{
								std::string	curve_x_var_name	=	(dpc_curve->getXVarName())	?	dpc_curve->getXVarName()	:	""	;
								std::string	curve_x_units				=	(dpc_curve->getXAttribute("units"))	?		dpc_curve->getXAttribute("units")	:	"1";
								curve_x_label				=	curve_x_var_name	+	"("	+	curve_x_units	+	")";
				}

				std::string	curve_y_var_name	=	(dpc_curve->getYVarName())	?	dpc_curve->getYVarName()	:	""	;
				if	(Curve->getYAttribute("label"))	{
								curve_y_label	=	Curve->getYAttribute("label");
				}	else	{
								std::string	curve_y_units				=	(dpc_curve->getYAttribute("units"))	?		dpc_curve->getYAttribute("units")	:	"1";
								curve_y_label				=	curve_y_var_name	+	"("	+	curve_y_units	+	")";
				}

				std::string	curve_title	=	curve_y_label	+	"	VS.	"	+	curve_x_label	+	"	["+data_src_label+"]";

				/**!
					*	WARNING:	getAttribute()	returns	a	const	char	*,	which	may	be	null.
					*										Assigning	a	null	pointer	to	a	string	will	crash	the	plot.
					*										So,	check	the	return	value	before	making	the	assignment.
					*/
				curve_x_min_rng						=	(dpc_curve->getXAttribute("min"))	?		dpc_curve->getXAttribute("min")	:	"";
				curve_x_max_rng						=	(dpc_curve->getXAttribute("max"))	?		dpc_curve->getXAttribute("max")	:	"";
				curve_x_scale_factor	=	(dpc_curve->getXAttribute("scale"))	?		dpc_curve->getXAttribute("scale")	:	"1.0";
				curve_x_bias									=	(dpc_curve->getXAttribute("bias"))	?		dpc_curve->getXAttribute("bias")	:	"0.0";
				curve_y_min_rng						=	(dpc_curve->getYAttribute("min"))	?		dpc_curve->getYAttribute("min")	:	"";
				curve_y_max_rng						=	(dpc_curve->getYAttribute("max"))	?		dpc_curve->getYAttribute("max")	:	"";
				curve_y_scale_factor	=	(dpc_curve->getYAttribute("scale"))	?		dpc_curve->getYAttribute("scale")	:	"1.0";
				curve_y_bias									=	(dpc_curve->getYAttribute("bias"))	?		dpc_curve->getYAttribute("bias")	:	"0.0";
				curve_line_color					=	(dpc_curve->getYAttribute("line_color"))	?		dpc_curve->getYAttribute("line_color")	:	"";
				curve_line_style					=	(dpc_curve->getYAttribute("line_style"))	?		dpc_curve->getYAttribute("line_style")	:	"";
				curve_gp_line_style		=	(dpc_curve->getYAttribute("gnuplot_line_style"))	?		dpc_curve->getYAttribute("gnuplot_line_style")	:	"lines";
				curve_symbol_style			=	(dpc_curve->getYAttribute("symbol_style"))	?		dpc_curve->getYAttribute("symbol_style")	:	"";
				curve_symbol_size				=	(dpc_curve->getYAttribute("symbol_size"))	?		dpc_curve->getYAttribute("symbol_size")	:	"Small";

				std::string	save_template	=	"<SAVE_TMP_FILE_SPEC>";
				std::string	prev_TmpFileSpec	=	"";

				//------------------------------------------------------------------
				//	Set	the	XYCurve's	data	points	(points)	and	number	of	points	(nPoints)
				//------------------------------------------------------------------

				presentation	=	Curve->getYAttribute("presentation")	;

				if	(	(presentation	!=	NULL)		&&		strcasecmp(presentation,	"stream")	!=	0	)	{

								char	curve_data_file_name[512];
								FILE	*curve_data_fp;

								//!	Figure	out	where	we	can	create	a	temporary	file.
								if	(!access("/tmp",	W_OK))	{

												strncpy(curve_data_file_name,
																				"/tmp/dpx_gp_curve_XXXXXX",	sizeof(curve_data_file_name));

								}	else	if	(!access("/var/tmp",	W_OK))	{
												strncpy(curve_data_file_name,
																				"/var/tmp/dpx_gp_curve_XXXXXX",	sizeof(curve_data_file_name));

								}	else	{
												std::cerr	<<	"Unable	to	access	/tmp	or	/var/tmp,	where	a	temporary\n"
																<<	"file	(that	represents	gnuplot	curve	data)	needs\n"
																<<	"to	be	created.	Please	check	your	permissions."	<<	std::endl	;
												return;
								}

								//!	Create	a	name	for	our	temporary	data	file.
								if	(mkstemp(	curve_data_file_name)	<	0)	{
												std::cerr	<<	"Unable	to	generate	a	temporary	file"
																<<	"name	for	some	mind	boggling	reason."	<<	std::endl	;
												return;
								}

								//!	Open	the	temporary	data	file.
								if	((curve_data_fp	=	fopen(	curve_data_file_name,	"w"))	==	NULL)	{
												std::cerr	<<	"Unable	to	open	a	temporary	file."	<<	std::endl	;
												return;
								}

								//!	Get	the	data	for	the	curve.
								while	(	Curve->getXY(&x,	&y))	{
												fprintf(	curve_data_fp,"%e\t%e\n",
														(	x*atof(curve_x_scale_factor.c_str())	+	atof(curve_x_bias.c_str())	),
														(	y*atof(curve_y_scale_factor.c_str())	+	atof(curve_y_bias.c_str())	)	);
								}
								//	Print	the	final	point	from	getXY
								fprintf(	curve_data_fp,"%e\t%e\n",
											(	x*atof(curve_x_scale_factor.c_str())	+	atof(curve_x_bias.c_str())	),
											(	y*atof(curve_y_scale_factor.c_str())	+	atof(curve_y_bias.c_str())	)	);
								fclose(curve_data_fp);

								if	(	Curve_number	==	0	)	{
												/*!
													*	Set	ranges	as	part	of	the	plot	command.	A	range	given	on	the	plot
													*	command	will	be	used	for	all	curves	on	that	single	graph	only,	&
													*	not	for	all	subsequent	graphs(e.g.	plot	[xmin:xmax]	[ymin:ymax]	)
													*/
												//!	Set	each	curve's	X	Var	MIN	RANGE.
												if	(	!	curve_x_min_rng.empty()	)	{
																Textbuf->print("[%d:",	curve_x_min_rng.c_str());
												}	else	{
																Textbuf->print("[<PLOT_X_MIN_RNG>:");
												}
												//!	Set	each	curve's	X	Var	MAX	RANGE.
												if	(	!	curve_x_max_rng.empty()	)	{
																Textbuf->print("%d]",	curve_x_max_rng.c_str());
												}	else	{
																Textbuf->print("<PLOT_X_MAX_RNG>]");
												}
												//!	Set	each	curve's	Y	Var	MIN	RANGE.
												if	(	!	curve_y_min_rng.empty()	)	{
																Textbuf->print("[%d:",	curve_y_min_rng.c_str());
												}	else	{
																Textbuf->print("[<PLOT_Y_MIN_RNG>:");
												}
												//!	Set	each	curve's	Y	Var	MAX	RANGE.
												if	(	!	curve_y_max_rng.empty()	)	{
																Textbuf->print("%d]",	curve_y_max_rng.c_str());
												}	else	{
																Textbuf->print("<PLOT_Y_MAX_RNG>]");
												}

												Textbuf->print("	\\\n");

								}	else	if	(	Curve_number	>	0	)	{
												/*	At	this	point,	the	`plot`	command	is	drawing	multiple
													*	lines	(curves)	onto	the	same	graph.
													*	The	`plot`	command	will	not	work	if	other	non-plot	text
													*	(i.e.	system	commands)	exist	within	the	curve	info	list.
													*	We	need	to	save	off	all	system	commands	from	the	buffer
													*	that	delete	the	previous	curves'	temporary	datafile(s).
													*	We	will	re-insert	later	once	this	curve's	info	is	written.
													*/
												tmp_string	=	Textbuf->getText();
												int	save_startPos	=	(int)tmp_string.find("\n"+save_template,	0);

												prev_TmpFileSpec	=	tmp_string.substr(	save_startPos,	tmp_string.size()	);

												/*	Remove	previous	system	commands	*/
												Textbuf->subst_g(prev_TmpFileSpec.c_str(),	"");

												//!	insert	a	comma,	space,	backslash(\)	&	newline	prior	to	next	curve	info
												Textbuf->print(",	\\\n",	"	");
								}

								//!	Indent	4	spaces	then	Populate	the	datafile	name.
								Textbuf->print("%4s\"%s\"	using	1:2",		"	",	curve_data_file_name);

								//!	Set	each	curve's	TITLE.
								Textbuf->print("		t	\"%s\"",	curve_title.c_str());

								//!	Set	each	curve's	GP	LINE	STYLE.
								Textbuf->print("	w	%s",	curve_gp_line_style.c_str());

								if	(	GnuplotVersion()	>=	4.2	)	{
												//!	Default	line	definitions.
												Textbuf->print("	lt	line_type_%i",	(Curve_number+1));
												Textbuf->print("	lw	1.0");

												//!	Set	each	curve's	LINE	COLOR.
												if	(	!	curve_line_color.empty()	)	{
																if	(	color_is_valid(&curve_line_color)	)	{
																				Textbuf->print("	lc	rgb	\"%s\"",	curve_line_color.c_str());
																}
												}

												//!	POINT	settings	are	applicable	to	points	&	linepoints	only.
												if	(	curve_gp_line_style.find("point")	!=	std::string::npos	)	{
																//!	Set	each	curve's	POINT	TYPE.
																if	(	!	curve_symbol_style.empty()	)	{
																				Textbuf->print("	pt	%s",	curve_symbol_style.c_str());
																}	else	{
																				Textbuf->print("	pt	point_shape_%i",	(Curve_number+1));
																}
																//!	Set	each	curve's	POINT	SIZE.
																Textbuf->print("	ps	%s",	curve_symbol_size.c_str());
												}
								}	else	{
												/*!
													*	Setting	line	color	in	gnuplot	version	4.0	or	older	cannot	be	done
													*	on	the	fly.	It	must	be	set	using	a	limited	number	of	line	styles
													*	or	at	startup	(e.g.	-xrm	'gnuplot*line1Color:black'...).
													*/
												Textbuf->print("	linestyle	%i",	(Curve_number+1)	);
												if	(!curve_line_color.empty())	{
																std::cout	<<	"note:	Ignoring	line	color;	"	;
																std::cout	<<	"feature	only	available	in	Gnuplot	version	4.2	(or	later)."	<<	std::endl;
												}
								}

								/*	Re-insert	previous	system	commands	*/
								Textbuf->print("%s\n",	prev_TmpFileSpec.c_str());
								/*!
									*	After	the	command	to	plot	the	curve	is	processed,	add
									*	a	system	command	to	delete	the	associated	data	file.
									*	Run	the	delete	command	in	the	background.
									*/
								Textbuf->print("%ssystem	\"rm	-f	%s	&\"",	save_template.c_str(),	curve_data_file_name);
				}

				instance_count	++;
}
