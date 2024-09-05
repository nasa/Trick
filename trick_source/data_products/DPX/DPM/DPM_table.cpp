
#include	<string.h>

#include	"DPM_table.hh"
#include	"float.h"

int	DPM_table::Initialize(	xmlNode	*base_node)	{

				double	tstart				=	0.0;
				double	tstop					=	DBL_MAX;
				double	frequency	=	DBL_MAX;
				title	=	NULL;

				int	n_columns;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_table::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"table")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<table>	but	didnt	find	it."	<<	std::endl;
								return	-1;
				}
				/*	Process	children	*/
				xmlNode	*current_node	=	base_node->children;

				while	(current_node	!=	NULL)	{

								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												DPM_column	*column;

												if	(	strcmp(	(const	char	*)current_node->name,	"title")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																title	=	strdup(	(const	char	*)text_node->content);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"tstart")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																tstart	=	strtod((const	char	*)text_node->content,(char	**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"tstop")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																tstop	=	strtod((const	char	*)	text_node->content,(char**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"frequency")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																frequency	=	strtod((const	char	*)text_node->content,(char**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"column")	==	0)	{
																try	{
																				column	=	new	DPM_column(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				column	=	NULL;
																				std::cerr	<<	"ERROR:	<table>	will	be	incomplete	because	one	of	it\'s	<column>s	is	invalid."	<<	std::endl;
																}
																if	(column	!=	NULL)	{
																				column_list.push_back(	column);
																}

												}	else	{
																std::cerr	<<	"WARNING:	Unknown	element	\""	<<	current_node->name	<<	"\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}

				time_constraints	=	new	DPM_time_constraints(tstart,	tstop,	frequency);

				//	Validation	criteria:
				//	1.	Table	requires	at	least	one	column	(DPM_column)	in	the	column_list.

				n_columns	=	(int)column_list.size();
				if	(n_columns	<	1)	{
								std::cerr	<<	"ERROR:	<table>	requires	at	least	one	valid	<column>."	<<	std::endl;
								return	-1;
				}
				return	0;
}

//	XML	CONSTRUCTOR
DPM_table::DPM_table(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(	Base_node)	<	0	)	{
						throw	std::invalid_argument("DPM_table	specification	is	un-usable.");
		}
}

//	MEMBER	FUNCTION
DPM_table::~DPM_table()	{
				int	i,	n_columns	;

				n_columns	=	(int)column_list.size();
				for	(i	=	0;	i	<	n_columns	;	i	++)	{
								DPM_column	*column;
								column	=		column_list[i];
								if	(column)	{	delete	column;	}
				}
				if	(title	)	{
								free(	title);
				}
}

//	MEMBER	FUNCTION
const	char	*DPM_table::getTitle()	{
		return	(	title);
}

//	MEMBER	FUNCTION
DPM_time_constraints	*DPM_table::getTimeConstraints()	{
		return	(time_constraints);
}

//	MEMBER	FUNCTION
int	DPM_table::NumberOfColumns()	{
		return	((int)column_list.size());
}

//	MEMBER	FUNCTION
DPM_column	*DPM_table::getColumn(	unsigned	int	index)	{
		if	(index	<	column_list.size(	))		{
				return	(	column_list[index]);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_table	*table)	{
		int	i;
		int	n_attrs	=	(int)table->attribute_list.size();
		int	n_columns	=	(int)table->column_list.size();

		s	<<	"<table";
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	table->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;
		if	(table->title	!=	NULL)	{
				s	<<	"<title>"		<<	table->title		<<	"</title>"		<<	std::endl;
		}
		s	<<	"<tstart>"	<<	table->time_constraints->getStart()	<<	"</tstart>"	<<	std::endl;
		s	<<	"<tstop>"		<<	table->time_constraints->getStop()		<<	"</tstop>"		<<	std::endl;
		s	<<	"<frequency>"		<<	table->time_constraints->getFrequency()		<<	"</frequency>"		<<	std::endl;

		for	(i=0	;	i<n_columns	;	i++)	{
				s	<<	table->column_list[i];
		}

		s	<<	"</table>";
		s	<<	std::endl;
		s.flush();

		return	s;
}

