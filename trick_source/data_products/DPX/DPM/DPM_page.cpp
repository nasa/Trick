
#include	<string.h>

#include	"DPM_page.hh"
#include	"float.h"

int	DPM_page::Initialize(	xmlNode	*base_node)	{

				double	tstart				=	0.0;
				double	tstop					=	DBL_MAX;
				double	frequency	=	DBL_MAX;
				title	=	NULL;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_page::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"page")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<page>	specification	but	alas	it	wasn\'t	found."	<<	std::endl;
								return	-1;
				}

				int	n_relations;
				/*	Process	children	*/
				xmlNode	*current_node	=	base_node->children;
				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												if	(	strcmp(	(const	char	*)current_node->name,	"title")	==	0)	{
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
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"plot")	==	0)	{
																DPM_relation	*relation;
																try	{
																				relation	=	new	DPM_relation(this,	current_node);
																}	catch	(std::invalid_argument)	{
																				relation	=	NULL;
																				std::cerr	<<	"ERROR:	<page>	specification	contains	an	invalid	<plot>	specification."	<<	std::endl;
																}
																if	(relation	!=	NULL)	{
																				relation_list.push_back(relation);
																}
												}	else	{
																std::cerr	<<	"WARNING:	<page>	specification	contains	an	unknown	element	\"<"	<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}

				time_constraints	=	new	DPM_time_constraints(tstart,	tstop,	frequency);

				//	Validation	criteria:
				//	1.	Page	requires	at	least	one	relation	(plot).
				n_relations	=	(int)relation_list.size();
				if	(n_relations	<	1)	{
								std::cerr	<<	"ERROR:	<page>	specification	requires	at	least	one	valid	<plot>	specification.	"	<<	std::endl;
								return	-1;
				}
				return	0;
}

//	CONSTRUCTOR
DPM_page::DPM_page(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(Initialize(Base_node)	<	0	)	{
						throw	std::invalid_argument("<page>	specification	is	invalid	and	un-usable.");
		}
}

//	MEMBER	FUNCTION
DPM_page::~DPM_page()	{
				//	Destroy	all	of	the	DPM_relation	objects	in	the	relation_list.
				int	i,n;
				n	=	(int)relation_list.size();
				for	(i	=	0;	i	<	n	;	i	++)	{
								DPM_relation	*relation;
								relation	=	relation_list[i];
								if	(relation)	{	delete	relation;	}
				}
}

//	MEMBER	FUNCTION
const	char	*DPM_page::getTitle()	{
	return(title);
}

//	MEMBER	FUNCTION
DPM_time_constraints	*DPM_page::getTimeConstraints()	{
		return	(time_constraints);
}

//	MEMBER	FUNCTION
int	DPM_page::NumberOfRelations()	{
		return	((int)relation_list.size());
}

//	MEMBER	FUNCTION
DPM_relation	*DPM_page::getRelation(	unsigned	int	index)	{
		if	(index	<	relation_list.size(	))		{
				return	(	relation_list[index]);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_page	*page)	{
		int	i;
		int	n_attrs	=	(int)page->attribute_list.size();
		int	n_relations	=	(int)page->relation_list.size();

		s	<<	"<page";
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	page->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;
		if	(page->title)	{
			s	<<	"<title>"		<<	page->title		<<	"</title>"		<<	std::endl;
		}
		s	<<	"<tstart>"	<<	page->time_constraints->getStart()	<<	"</tstart>"	<<	std::endl;
		s	<<	"<tstop>"		<<	page->time_constraints->getStop()		<<	"</tstop>"		<<	std::endl;
		s	<<	"<frequency>"		<<	page->time_constraints->getFrequency()		<<	"</frequency>"		<<	std::endl;

		for	(i=0	;	i<n_relations	;	i++)	{
				s	<<	page->relation_list[i];
		}

		s	<<	"</page>"	<<	std::endl;
		return	s;
}

