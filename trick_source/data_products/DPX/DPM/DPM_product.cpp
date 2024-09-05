
#include	<string.h>

#include	"DPM_product.hh"
#include	"float.h"
#include	<stdio.h>

int	DPM_product::Initialize(	xmlNode	*base_node)	{

				double	tstart				=	0.0;
				double	tstop					=	DBL_MAX;
				double	frequency	=	DBL_MAX;
				title			=	(char	*)NULL;

				int	n_pages,	n_tables;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_product::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(strcmp((char*)	base_node->name,	"product")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<product>	specification	but	alas	didnt	find	it."	<<	std::endl;
								return	-1;
				}
				/*	Process	children	*/
				xmlNode	*current_node	=	base_node->children;

				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												DPM_page		*page;
												DPM_extfn	*ext_prog;
												DPM_table	*table;
												if	(	strcmp(	(char	*)current_node->name,	"page")	==	0)	{
																try	{
																				page	=	new	DPM_page(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				page	=	NULL;
																				std::cerr	<<	"ERROR:	<product>	specification	contains	an	invalid	<page>	specification."	<<	std::endl;
																}
																if	(page	!=	NULL)	{
																				page_list.push_back(	page);
																}
												}	else	if	(	strcmp(	(char*)current_node->name,	"table")	==	0)	{
																try	{
																				table	=	new	DPM_table(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				table	=	NULL;
																				std::cerr	<<	"ERROR:	<product>	specification	contains	an	invalid	<table>	specification."	<<	std::endl;
																}
																if	(table	!=	NULL)	{
																				table_list.push_back(	table	);
																}
												}	else	if	(	strcmp(	(char*)current_node->name,	"extfn")	==	0)	{
																try	{
																				ext_prog	=	new	DPM_extfn(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				ext_prog	=	NULL;
																				std::cerr	<<	"ERROR:	<product>	specification	contains	an	invalid	<extfn>	specification."	<<	std::endl;
																}
																if	(ext_prog	!=	NULL)	{
																				extfn_list.push_back(	ext_prog	);
																}
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"title")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																title	=	strdup(	(const	char	*)text_node->content);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"tstart")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																tstart	=	strtod((const	char	*)text_node->content,(char	**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"tstop")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																tstop	=	strtod((const	char	*)text_node->content,(char	**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"frequency")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																frequency	=	strtod((const	char	*)text_node->content,(char**)NULL);

												}	else	{
																std::cerr	<<	"WARNING:	<product>	specification	contains	an	unknown	element	\"<"	<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}
				time_constraints	=	new	DPM_time_constraints(tstart,	tstop,	frequency);

				//	Validation	criteria:
				//	1.	Product	requires	at	least	on	page	or	table.
				n_pages	=	(int)page_list.size();
				n_tables	=	(int)table_list.size();
				if	((n_pages	+	n_tables)	<	1	)	{
							std::cerr	<<	"ERROR:	<product>	specification	requires	at	least	one	<page>	or	<table>	specification."	<<	std::endl;
							return	-1;
				}

				xmlCleanupParser();
				return	0;
}

//	XML	CONSTRUCTOR
DPM_product::DPM_product(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(	Base_node)	<	0)	{
						throw	std::invalid_argument("<product>	specification	is	invalid	and	un-usable.");
		}
}

//	DESTRUCTOR
DPM_product::~DPM_product()	{
		//	destroy	each	of	the	allocated	object	elements.
}

//	MEMBER	FUNCTION
const	char	*DPM_product::getTitle()	{
		return(	title);
}

//	MEMBER	FUNCTION
DPM_time_constraints	*DPM_product::getTimeConstraints()	{
		return	(	time_constraints);
}

//	MEMBER	FUNCTION
int	DPM_product::NumberOfTables()	{
		return	((int)table_list.size());
}

//	MEMBER	FUNCTION
DPM_table	*DPM_product::getTable(	unsigned	int	index)	{
		if	(index	<	table_list.size(	))		{
				return	(	table_list[index]);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
int	DPM_product::NumberOfPages()	{
		return	(	(int)page_list.size());
}

//	MEMBER	FUNCTION
DPM_page	*DPM_product::getPage(unsigned	int	index)	{

		if	(index	<	page_list.size())	{
				return(	page_list[index]);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
int	DPM_product::NumberOfExtFns()	{
		return	(	(int)extfn_list.size());
}

//	MEMBER	FUNCTION
DPM_extfn	*DPM_product::getExtFn(unsigned	int	index)	{

		if	(index	<	extfn_list.size())	{
				return(	extfn_list[index]);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_product	*product)	{
		int	i;
		int	n_attrs	=	(int)product->attribute_list.size();
		int	n_pages	=	(int)product->page_list.size();
		int	n_tables	=	(int)product->table_list.size();
		int	n_extfns	=	(int)product->extfn_list.size();

		s	<<	"<product";
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	product->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;

		if	(product->title)	{
				s	<<	"<title>"	<<	product->title	<<	"</title>"	<<	std::endl;
		}

		for	(i=0	;	i<n_pages	;	i++)	{
				s	<<	product->page_list[i];
		}

		for	(i=0	;	i<n_tables	;	i++)	{
				s	<<	product->table_list[i];
		}

		for	(i=0	;	i<n_extfns	;	i++)	{
				s	<<	product->extfn_list[i];
		}
		s	<<	"</product>"	<<	std::endl;
		return	s;
}

