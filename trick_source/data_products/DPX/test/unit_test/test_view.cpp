#include	<string.h>

#include	"test_view.hh"
#include	"gtest/gtest.h"

std::string	Test_view::getOutput()	{
	std::string	tmp	=	s.str();
	return	tmp;
}

DPV_pointer	Test_view::render_product(	DPC_product*	product){
	const	char	*temp_cstr;

	s	<<	"PRODUCT	VIEW:"	<<	std::endl;
				s	<<	"Attribute	version:	"	<<		product->getAttribute("version")	<<	std::endl;
	if	((temp_cstr	=	product->getTitle())	!=	NULL)	{
				s	<<	"Product	title:	"	<<	product->getTitle()	<<	std::endl;
			}
	if	((temp_cstr	=	product->getAttribute("foreground_color"))	!=	NULL)	{
								s	<<	"Attribute	foreground_color:	"	<<	temp_cstr	<<	std::endl;
				}
				if	((temp_cstr	=	product->getAttribute("background_color"))	!=	NULL)	{
								s	<<	"Attribute	background_color:	"	<<	temp_cstr	<<	std::endl;
				}

	return	(	(DPV_pointer)0x01111111	);
}

DPV_pointer	Test_view::render_page(	DPV_pointer	parent_data,	DPC_page*	page)	{
			const	char	*temp_cstr;
	(void)parent_data;
			s	<<	"PAGE	VIEW:"	<<	std::endl;
			if	((temp_cstr	=	page->getTitle())	!=	NULL)	{
					s	<<	"Page	title:	"	<<	page->getTitle()	<<	std::endl;
			}
	if	((temp_cstr	=	page->getAttribute("foreground_color"))	!=	NULL)	{
								s	<<	"Attribute	foreground_color:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("background_color"))	!=	NULL)	{
								s	<<	"Attribute	background_color:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("hcells"))	!=	NULL)	{
								s	<<	"Attribute	hcells:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("foreground_color"))	!=	NULL)	{
								s	<<	"Attribute	vcells:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("presentation"))	!=	NULL)	{
								s	<<	"Attribute	presentation:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("gnuplot_template"))	!=	NULL)	{
								s	<<	"Attribute	gnuplot_template:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("gnuplot_object"))	!=	NULL)	{
								s	<<	"Attribute	gnuplot_object:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("gnuplot_geom"))	!=	NULL)	{
								s	<<	"Attribute	gnuplot_geom:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("gnuplot_plot_ratio"))	!=	NULL)	{
								s	<<	"Attribute	gnuplot_plot_ratio:	"	<<	temp_cstr	<<	std::endl;
				}
	if	((temp_cstr	=	page->getAttribute("gnuplot_page_orientation"))	!=	NULL)	{
								s	<<	"Attribute	gnuplot_page_orientation:	"	<<	temp_cstr	<<	std::endl;
				}


			return	(	(DPV_pointer)0x02222222	);
}

DPV_pointer	Test_view::render_plot(	DPV_pointer	parent_data,	DPC_plot*	plot)	{
			const	char	*temp_cstr;
	(void)parent_data;
			s	<<	"PLOT	VIEW:"	<<	std::endl;
			if	((temp_cstr	=	plot->getTitle())	!=	NULL)	{
				s	<<	"Plot	title:	"	<<	plot->getTitle()	<<	std::endl;
			}

			s	<<	"Number	of	Curves:	"	<<	plot->getNumCurves()	<<	std::endl;
			s	<<	"X-axis	label:	"	<<	plot->getXLabel()	<<	std::endl;
			s	<<	"Y-axis	label:	"	<<	plot->getYLabel()	<<	std::endl;

			if	((temp_cstr	=	plot->getAttribute("xmin"))	!=	NULL)	{
					s	<<	"Attribute	xmin:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	plot->getAttribute("xmax"))	!=	NULL)	{
					s	<<	"Attribute	xmax:	"	<<	temp_cstr	<<	std::endl;
			}
	if	((temp_cstr	=	plot->getAttribute("x_scale"))	!=	NULL)	{
					s	<<	"Attribute	x_scale:	"	<<	temp_cstr	<<	std::endl;
	}
			if	((temp_cstr	=	plot->getAttribute("ymin"))	!=	NULL)	{
					s	<<	"Attribute	ymin:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	plot->getAttribute("ymax"))	!=	NULL)	{
					s	<<	"Attribute	ymax:	"	<<	temp_cstr	<<	std::endl;
			}
				if	((temp_cstr	=	plot->getAttribute("y_scale"))	!=	NULL)	{
								s	<<	"Attribute	y_scale:	"	<<	temp_cstr	<<	std::endl;
				}
			if	((temp_cstr	=	plot->getAttribute("grid"))	!=	NULL)	{
					s	<<	"Attribute	grid:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	plot->getAttribute("grid_color"))	!=	NULL)	{
					s	<<	"Attribute	grid_color:	"	<<	temp_cstr	<<	std::endl;
			}
		if	((temp_cstr	=	plot->getAttribute("foreground_color"))	!=	NULL)	{
					s	<<	"Attribute	foreground_color:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	plot->getAttribute("background_color"))	!=	NULL)	{
					s	<<	"Attribute	background_color:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	plot->getAttribute("font"))	!=	NULL)	{
					s	<<	"Attribute	font:	"	<<	temp_cstr	<<	std::endl;
			}
			return	(	(DPV_pointer)0x03333333	);
}

DPV_pointer	Test_view::render_table(	DPV_pointer	parent_data,	DPC_table		*table)	{
	const	char	*temp_cstr;
	(void)parent_data;
			int	n_columns,	colix;
			int	i,j;
			double	time;
	double	*	values;

			std::stringstream	begin1;
			std::stringstream	begin2;

			s	<<	"TABLE	VIEW:"	<<	std::endl;

			s	<<	"RUN	Directory:	"	<<	table->getRunDir()	<<	std::endl;
			if	((temp_cstr	=	table->getTitle())	!=	NULL)	{
		s	<<	"Title:	"	<<	temp_cstr	<<	std::endl;
			}

			n_columns	=	table->getNumColumns();
			s	<<	"Number	of	Columns:	"	<<	n_columns	<<	std::endl;

			for	(colix=0;	colix	<	n_columns	;	colix++)	{
					if	((temp_cstr	=	table->getColumnLabel(colix).c_str())[0]	!=	'\0')	{
								s	<<	"Column	Label	["	<<	colix	<<	"]:	"	<<	temp_cstr	<<	std::endl;
					}
					if	((temp_cstr	=	table->getColumnVarName(colix))	!=	NULL)	{
								s	<<	"Variable	Name["	<<	colix	<<	"]:	"	<<	temp_cstr	<<	std::endl;
					}
					if	((temp_cstr	=	table->getColumnUnits(colix))	!=	NULL)	{
								s	<<	"Column	Units	["	<<	colix	<<	"]:	"	<<	temp_cstr	<<	std::endl;
					}
					if	((temp_cstr	=	table->getColumnAttribute(colix,"format"))	!=	NULL)	{
								s	<<	"format	["	<<	colix	<<	"]:	"	<<	temp_cstr	<<	std::endl;
					}
			}

			values	=	new	double[n_columns];

	for	(i=0;	i<5	;	i++)	{
					if	(	table->getRow(&time,	values))	{
									begin1	<<	"time:	"	<<	time	<<	"	values:	";
			s	<<	"time:	"	<<	time	<<	"	values:	";
												for	(j=0;	j<n_columns	;	j++)	{
																	begin1	<<	"	"	<<	values[j];
					s	<<	"	"	<<	values[j];
												}
			begin1	<<	std::endl;
												s	<<	std::endl;
							}
			}

			//	To	test	begin()	Re-initialize	the	values,	begin()	and	doing	it	again.
			//	The	following	lines	should	be	the	same	as	the	previous	lines.
			for	(i=0;	i<n_columns	;	i++)	{	values[i]	=	0.0;	}
			table->begin();
			for	(i=0;	i<5	;	i++)	{
								if	(	table->getRow(&time,	values))	{
			begin2	<<	"time:	"	<<	time	<<	"	values:	";
												for	(j=0;	j<n_columns	;	j++)	{
																	begin2	<<	"	"	<<	values[j];
												}
												begin2	<<	std::endl;
								}
			}
			EXPECT_EQ(strcmp(begin1.str().c_str(),	begin2.str().c_str()),	0);

			delete	[]	values;

			return	(	(DPV_pointer)0x03333333	);
}

DPV_pointer	Test_view::render_curve(	DPV_pointer	parent_data,	DPC_curve*	curve)	{
			int	i;
			const	char	*temp_cstr;
	(void)parent_data;
			s	<<	"CURVE	VIEW:"	<<	std::endl;
			s	<<	"X	Var	Name:	"	<<	curve->getXVarName()	<<	std::endl;
			s	<<	"X	Units:	"	<<	curve->getXUnits()	<<	std::endl;
			s	<<	"Y	Var	Name:	"	<<	curve->getYVarName()	<<	std::endl;
			s	<<	"Y	Units:	"	<<	curve->getYUnits()	<<	std::endl;
			s	<<	"RUN	Directory:	"	<<	curve->getDataSrcLbl()	<<	std::endl;

			if	((temp_cstr	=	curve->getXAttribute("min"))	!=	NULL)	{
					s	<<	"Attribute	x	min:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getXAttribute("max"))	!=	NULL)	{
					s	<<	"Attribute	x	max:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getXAttribute("symbol_style"))	!=	NULL)	{
					s	<<	"Attribute	x	symbol_style:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getXAttribute("symbol_size"))	!=	NULL)	{
					s	<<	"Attribute	x	symbol_size:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getXAttribute("line_style"))	!=	NULL)	{
					s	<<	"Attribute	x	line_style:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getXAttribute("line_color"))	!=	NULL)	{
					s	<<	"Attribute	x	line_color:	"	<<	temp_cstr	<<	std::endl;
			}
	if	((temp_cstr	=	curve->getXAttribute("gnuplot_line_style"))	!=	NULL)	{
								s	<<	"Attribute	x	gnuplot_line_style:	"	<<	temp_cstr	<<	std::endl;
				}

			if	((temp_cstr	=	curve->getYAttribute("min"))	!=	NULL)	{
					s	<<	"Attribute	y	min:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getYAttribute("max"))	!=	NULL)	{
					s	<<	"Attribute	y	max:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getYAttribute("symbol_style"))	!=	NULL)	{
					s	<<	"Attribute	y	symbol_style:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getYAttribute("symbol_size"))	!=	NULL)	{
					s	<<	"Attribute	y	symbol_size:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getYAttribute("line_style"))	!=	NULL)	{
					s	<<	"Attribute	y	line_style:	"	<<	temp_cstr	<<	std::endl;
			}
			if	((temp_cstr	=	curve->getYAttribute("line_color"))	!=	NULL)	{
					s	<<	"Attribute	y	line_color:	"	<<	temp_cstr	<<	std::endl;
			}
	if	((temp_cstr	=	curve->getYAttribute("gnuplot_line_style"))	!=	NULL)	{
								s	<<	"Attribute	y	gnuplot_line_style:	"	<<	temp_cstr	<<	std::endl;
				}

			s	<<	"First	5	points:"	<<	std::endl;
			for	(i=0	;	i	<	5	;	i++)	{
					double	x,	y;
					if	(curve->getXY(&x,	&y)	)	{
								s	<<	"<"	<<	x	<<	","	<<	y	<<	">["	<<	i	<<	"]"	<<	std::endl;
					}
			}

			return	(	(DPV_pointer)0x04444444	);
}

void	Test_view::finalize_product_view(	DPV_pointer	product_view	)	{
			s	<<	"FINALIZE	PRODUCT	VIEW."	<<	std::endl;
	(void)product_view;
}

void	Test_view::finalize_page_view(	DPV_pointer	page_view	)	{
	s	<<	"FINALIZE	PAGE	VIEW."	<<	std::endl;
	(void)page_view;
}

void	Test_view::finalize_plot_view(	DPV_pointer	plot_view	)	{
	s	<<	"FINALIZE	PLOT	VIEW."	<<	std::endl;
	(void)plot_view;
}

void	Test_view::finalize_table_view(	DPV_pointer	table_view	)	{
	s	<<	"FINALIZE	TABLE	VIEW."	<<	std::endl;
	(void)table_view;
}

