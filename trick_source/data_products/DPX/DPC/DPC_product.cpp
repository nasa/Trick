
#include	"DPC/DPC_product.hh"
#include	"DPM/DPM_parse_tree.hh"

#include	<libxml/parser.h>
#include	<libxml/tree.h>

#include	<fcntl.h>		//	for	open()
#include	<unistd.h>	//	for	link()
#include	<stdlib.h>	//	for	getenv()
#include	<string.h>	//	for	strlen()

DPC_product::DPC_product(	DPM_session										*Session,
																										const	char											*ProductFileName
																										)		{

				DPM_time_constraints	*my_time_constraints;
				DPM_time_constraints	*parentTimeConstraints;
				DPM_time_constraints		total_time_constraints;
				DPM_parse_tree	*product_parse_tree;
				DPM_page							*page_spec;
				DPM_table						*table_spec;

				xmlNode	*root_node;

				int	n_pages,	n_tables,	pagix,	tabix,	runix,	n_runs;

				//	###############################################################
				//	Validate	arguements.
				//	###############################################################
				if	(Session->run_list.size()	==	0	)	{
								std::cerr	<<	"No	RUN	Directories	specified."	<<	std::endl;
								std::cerr.flush();
								throw	std::invalid_argument("No	RUN	directories	specified.");
				}

				if	(!	ProductFileName)	{
								std::cerr	<<	"No	ProductFileName	specified."	<<	std::endl;
								std::cerr.flush();
								throw	std::invalid_argument("No	product	file	name	specified.");
				}

				product_parse_tree	=	new	DPM_parse_tree(	ProductFileName);
				root_node	=	product_parse_tree->getRootNode();
				product_spec	=	new	DPM_product(	Session,	root_node);
				delete	product_parse_tree;

				datastream_supplier	=	new	DPC_datastream_supplier(	product_spec	);
				view_data	=	NULL;

				//	###############################################################
				//	Determine	our	time	constraints.
				//	###############################################################

				//	Combine	the	time	constraints	levied	by	the	product	specification
				//	and	those	levied	by	the	parent-page.	The	combined	time	constraints
				//	will	be	levied	on	each	of	the	subordinate	pages.

				parentTimeConstraints	=	Session->time_constraints;
				my_time_constraints	=	product_spec->getTimeConstraints();
				total_time_constraints	=	*my_time_constraints	+	*parentTimeConstraints;

				const	char*	session_mode	=	Session->AttributeValue("mode");

				if	((session_mode	==	NULL)	||	(strcasecmp(	session_mode,	"plot")	==	0))	{

								//	###############################################################
								//	Create	subordinate	pages.
								//	###############################################################
								n_pages	=	product_spec->NumberOfPages();

								for	(pagix	=	0	;	pagix	<	n_pages	;	pagix	++)	{
												DPC_page	*page;

												page_spec	=	product_spec->getPage(pagix);

												try	{
												page	=	new	DPC_page(	datastream_supplier,
																																	&(Session->run_list),
																																	page_spec,
																																	parentTimeConstraints	);
												//	Put	the	new	page	in	the	page	list.
												page_list.push_back(	page	);
												}	catch	(std::invalid_argument&	error)	{
																std::cerr	<<	error.what()	<<	std::endl;
												}

								}
				}

				if	((session_mode	==	NULL)	||	(strcasecmp(	session_mode,	"table")	==	0))	{

								//	###############################################################
								//	Create	subordinate	tables.
								//	###############################################################
								n_tables	=	product_spec->NumberOfTables();
								n_runs	=	(int)Session->run_list.size();

								//	Foreach	of	the	Tables.
								for	(tabix	=	0	;	tabix	<	n_tables	;	tabix++	)	{

												table_spec	=	product_spec->getTable(	tabix	);

												//	Foreach	of	the	RUNS
												for	(runix	=	0	;	runix	<	n_runs	;	runix++	)	{
																DPC_table	*table;

																//	Make	a	plot.
																table	=	new	DPC_table(	datastream_supplier,
																																							Session->run_list[runix],
																																							table_spec,
																																							&total_time_constraints	);

																//	Put	the	new	plot	in	the	plot	list.
																table_list.push_back(	table	);
												}
								}
				}

}
//	DESTRUCTOR
DPC_product::~DPC_product()	{
				DPC_page	*page;
				DPC_table	*table;
				int	n_pages,	pagix;
				int	n_tables,	tabix;

				delete	product_spec;
				delete	datastream_supplier;
				n_pages	=	(int)page_list.size();
				for	(pagix	=	0	;	pagix	<	n_pages	;	pagix	++)	{
								page	=	page_list[pagix];
								delete	page;
				}
				n_tables	=	(int)table_list.size();
				for	(tabix	=	0	;	tabix	<	n_tables	;	tabix	++)	{
								table	=	table_list[tabix];
								delete	table;
				}
}

//	MEMBER	FUNCTION
const	char	*DPC_product::getTitle()	{
				return	(	product_spec->getTitle());
}

//	MEMBER	FUNCTION
int	DPC_product::NumberOfTables()	{
				return	((int)table_list.size());
}

//	MEMBER	FUNCTION
DPC_table	*DPC_product::getTable(	unsigned	int	index)	{
				if	(index	<	table_list.size(	))		{
								return	(	table_list[index]);
				}	else	{
								return	NULL;
				}
}

//	MEMBER	FUNCTION
int	DPC_product::NumberOfPages()	{
				return	(	(int)page_list.size());
}

//	MEMBER	FUNCTION
DPC_page	*DPC_product::getPage(unsigned	int	index)	{

				if	(index	<	page_list.size())	{
								return(	page_list[index]);
				}	else	{
								return	NULL;
				}
}

//	MEMBER	FUNCTION
DPV_pointer	DPC_product::render(	DPV_view	*view	)	{

				if	(view	!=	NULL)	{
								DPC_page		*page;
								DPC_table	*table;
								int	i,	n_pages,	n_tables;

								view_data	=	view->render_product(	this);

								//	Call	the	render	member	function	of	each	of	the	children.
								n_pages	=	(int)page_list.size();
								for	(i=0	;	i<n_pages	;	i++	)	{
												page	=	page_list[i];
												page->render(	view,	view_data);
								}

								//	Call	the	render	member	function	of	each	of	the	children.
								n_tables	=	(int)table_list.size();
								for	(i=0	;	i<n_tables	;	i++	)	{
												table	=	table_list[i];
												table->render(	view,	view_data);
								}

								view->finalize_product_view(	view_data);

				}	else	{
								std::cout	<<	"DPC_product::render:	view	is	NULL."	<<	std::endl;
								view_data	=	NULL;
				}
				return	(view_data);
}

void	DPC_product::notify(	DPV_view	*view,	DPV_message	msg	)	{

				if	(view_data	!=	NULL)	{
								view->notify_product(	view_data,	msg);
				}
}

//	MEMBER	FUNCTION
const	char	*DPC_product::getAttribute(const	char	*key)	{
				return(	product_spec->AttributeValue(key)	);
}

