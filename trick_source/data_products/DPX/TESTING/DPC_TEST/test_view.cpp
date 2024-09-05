#include	"test_view.hh"



DPV_pointer	test_view::create_product_view(	DPC_product*	product){
		cout	<<	"======================="	<<	endl;
		cout	<<	"PRODUCT	VIEW."	<<	endl;
		cout	<<	"======================="	<<	endl;
		cout.flush();
		cout	<<	"Product	title:	"	<<	product->getTitle()	<<	endl;
		cout.flush();

		return	(	(DPV_pointer)0x01111111	);
}

DPV_pointer	test_view::create_page_view(	DPV_pointer	parent_data,
							DPC_page*	page)	{
		long	pdata	=	(long)parent_data;
		cout	<<	"======================="	<<	endl;
		cout	<<	"PAGE	VIEW."	<<	endl;
		cout	<<	"======================="	<<	endl;
		cout	<<	"parent	data:	"	<<	pdata	<<	endl;
		cout.flush();
		cout	<<	"Page	title:	"	<<	page->getTitle()	<<	endl;
		cout.flush();

		return	(	(DPV_pointer)0x02222222	);
}

DPV_pointer	test_view::create_plot_view(	DPV_pointer	parent_data,
							DPC_plot*	plot)	{
		long	pdata	=	(long)parent_data;
		const	char	*temp_cstr;

		cout	<<	"====================="	<<	endl;
		cout	<<	"PLOT	VIEW."	<<	endl;
		cout	<<	"====================="	<<	endl;
		cout	<<	"parent	data:	"	<<	pdata	<<	endl;
		cout	<<	"Plot	title:	"	<<	plot->getTitle()	<<	endl;
		cout.flush();
		cout	<<	"Number	of	Curves:	"	<<	plot->getNumCurves()	<<	endl;
		cout.flush();
		cout	<<	"X-axis	label:	"	<<	plot->getXLabel()	<<	endl;
		cout.flush();
		cout	<<	"Y-axis	label:	"	<<	plot->getYLabel()	<<	endl;
		cout.flush();

		if	((temp_cstr	=	plot->getAttribute("xmin"))	!=	NULL)	{
				cout	<<	"Attribute	xmin:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("xmax"))	!=	NULL)	{
				cout	<<	"Attribute	xmax:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("ymin"))	!=	NULL)	{
				cout	<<	"Attribute	ymin:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("ymax"))	!=	NULL)	{
				cout	<<	"Attribute	ymax:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("grid"))	!=	NULL)	{
				cout	<<	"Attribute	grid:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("grid_color"))	!=	NULL)	{
				cout	<<	"Attribute	grid_color:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("foreground_color"))	!=	NULL)	{
				cout	<<	"Attribute	foreground_color:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("background_color"))	!=	NULL)	{
				cout	<<	"Attribute	background_color:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	plot->getAttribute("font"))	!=	NULL)	{
				cout	<<	"Attribute	font:	"	<<	temp_cstr	<<	endl;
		}
		return	(	(DPV_pointer)0x03333333	);
}

DPV_pointer	test_view::create_table_view(	DPV_pointer	parent_data,
																																										DPC_table		*table)	{
		int	n_columns,	colix;
		int	i,j;
		double	*	values;
		double	time;

		cout	<<	"====================="	<<	endl;
		cout	<<	"TABLE	VIEW."	<<	endl;
		cout	<<	"====================="	<<	endl;

		cout	<<	"RUN	Directory	:	"	<<	table->getRunDir()	<<	endl;
		cout	<<	"Title	:	"	<<	table->getTitle()	<<	endl;

		n_columns	=	table->getNumColumns();
		cout	<<	"Number	of	Columns	:	"	<<	n_columns	<<	endl;

		for	(colix=0;	colix	<	n_columns	;	colix++)	{
				const	char	*col_label	=	table->getColumnLabel(	colix).c_str();
				const	char	*var_name	=	table->getColumnVarName(	colix);
				const	char	*col_units	=	table->getColumnUnits(	colix);
				const	char	*format	=	table->getColumnAttribute(	colix,"format");

				if	(col_label	!=	NULL)	{
						cout	<<	"Column	Label["	<<	colix	<<	"]:	"	<<	col_label	<<	endl;
				}
				if	(var_name	!=	NULL)	{
						cout	<<	"Variable	Name["	<<	colix	<<	"]:	"	<<	var_name	<<	endl;
				}
				if	(col_units	!=	NULL)	{
						cout	<<	"Column	Units["	<<	colix	<<	"]:	"	<<	col_units	<<	endl;
				}
				if	(format	!=	NULL)	{
						cout	<<	"format	["	<<	colix	<<	"]:	"	<<	format	<<	endl;
				}
		}

		values	=	new	double[n_columns];

		for	(i=0;	i<5	;	i++)	{
							if	(	table->getRow(&time,	values))	{
												cout	<<	"time:	"	<<	time	;
												for	(j=0;	j<n_columns	;	j++)	{
																	cout	<<	"	"	<<	values[j];
												}
												cout	<<	endl;
							}
		}

//	To	test	begin()	Re-initialize	the	values,	begin()	and	doing	it	again.
		cout	<<	"The	following	5	lines	should	be	the	same	as	the	previous	5	lines."	<<	endl;
		for	(i=0;	i<5	;	i++)	{	values[i]	=	0.0;	}
		table->begin();
		for	(i=0;	i<5	;	i++)	{
							if	(	table->getRow(&time,	values))	{
												cout	<<	"time:	"	<<	time	;
												for	(j=0;	j<n_columns	;	j++)	{
																	cout	<<	"	"	<<	values[j];
												}
												cout	<<	endl;
							}
		}

		return	(	(DPV_pointer)0x03333333	);
}

DPV_pointer	test_view::create_curve_view(	DPV_pointer	parent_data,
								DPC_curve*	curve)	{
		int	i;
		long	pdata	=	(long)parent_data;
		const	char	*temp_cstr;

		cout	<<	"====================="	<<	endl;
		cout	<<	"CURVE	VIEW."	<<	endl;
		cout	<<	"====================="	<<	endl;
		cout	<<	"parent	data:	"	<<	pdata	<<	endl;
		cout	<<	"X	Var	Name:	"	<<	curve->getXVarName()	<<	endl;
		cout.flush();
		cout	<<	"Y	Var	Name:	"	<<	curve->getYVarName()	<<	endl;
		cout.flush();
		cout	<<	"X	Units:	"	<<	curve->getXUnits()	<<	endl;
		cout.flush();
		cout	<<	"Y	Units:	"	<<	curve->getYUnits()	<<	endl;
		cout.flush();
		cout	<<	"RUN	Directory:	"	<<	curve->getDataSrcLbl()	<<	endl;
		cout.flush();

		if	((temp_cstr	=	curve->getXAttribute("min"))	!=	NULL)	{
				cout	<<	"Attribute	min:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getXAttribute("max"))	!=	NULL)	{
				cout	<<	"Attribute	max:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getXAttribute("symbol_style"))	!=	NULL)	{
				cout	<<	"Attribute	symbol_style:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getXAttribute("symbol_size"))	!=	NULL)	{
				cout	<<	"Attribute	symbol_size:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getXAttribute("line_style"))	!=	NULL)	{
				cout	<<	"Attribute	line_style:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getXAttribute("line_color"))	!=	NULL)	{
				cout	<<	"Attribute	line_color:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getYAttribute("min"))	!=	NULL)	{
				cout	<<	"Attribute	min:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getYAttribute("max"))	!=	NULL)	{
				cout	<<	"Attribute	max:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getYAttribute("symbol_style"))	!=	NULL)	{
				cout	<<	"Attribute	symbol_style:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getYAttribute("symbol_size"))	!=	NULL)	{
				cout	<<	"Attribute	symbol_size:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getYAttribute("line_style"))	!=	NULL)	{
				cout	<<	"Attribute	line_style:	"	<<	temp_cstr	<<	endl;
		}
		if	((temp_cstr	=	curve->getYAttribute("line_color"))	!=	NULL)	{
				cout	<<	"Attribute	line_color:	"	<<	temp_cstr	<<	endl;
		}

		cout	<<	"First	10	points:"	<<	endl;
		cout.flush();
		for	(i=0	;	i	<	10	;	i++)	{
				double	x,	y;
				if	(curve->getXY(&x,	&y)	)	{
						cout	<<	"<"	<<	x	<<	","	<<	y	<<	">["	<<	i	<<	"]"	<<	endl;
						cout.flush();
				}
		}

		return	(	(DPV_pointer)0x04444444	);
}

void	test_view::finalize_product_view(	DPV_pointer	product_view	)	{
		cout	<<	"====================="	<<	endl;
		cout	<<	"FINALIZE	PRODUCT	VIEW."	<<	endl;
		cout	<<	"====================="	<<	endl;
		cout.flush();

}

void	test_view::finalize_page_view(	DPV_pointer	page_view	)	{
		cout	<<	"====================="	<<	endl;
		cout	<<	"FINALIZE	PAGE	VIEW."	<<	endl;
		cout	<<	"====================="	<<	endl;
		cout.flush();
}

void	test_view::finalize_plot_view(	DPV_pointer	plot_view	)	{
		cout	<<	"====================="	<<	endl;
		cout	<<	"FINALIZE	PLOT	VIEW."	<<	endl;
		cout	<<	"====================="	<<	endl;
		cout.flush();
}
