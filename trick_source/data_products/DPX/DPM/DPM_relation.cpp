
#include	<string.h>

#include	"DPM_relation.hh"
#include	"float.h"

int	DPM_relation::Initialize(xmlNode	*base_node)	{

				double	tstart				=	0.0;
				double	tstop					=	DBL_MAX;
				double	frequency	=	DBL_MAX;
				title	=	NULL;
				xaxis	=	NULL;
				yaxis	=	NULL;
				zaxis	=	NULL;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_relation::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"plot")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<plot>	specification	but	alas	didnt	find	it."	<<	std::endl;
								return	-1;
				}
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
																tstop	=	strtod((const	char	*)	text_node->content,(char**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"frequency")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																frequency	=	strtod((const	char	*)text_node->content,(char**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"xaxis")	==	0)	{
																try	{
																				xaxis	=	new	DPM_axis(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				xaxis	=	NULL;
																				std::cerr	<<	"ERROR:	<plot>	specification	contains	an	invalid	<xaxis>	specification."	<<	std::endl;
																}
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"yaxis")	==	0)	{
																try	{
																				yaxis	=	new	DPM_axis(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				yaxis	=	NULL;
																				std::cerr	<<	"ERROR:	<plot>	specification	contains	an	invalid	<yaxis>	specification."	<<	std::endl;
																}
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"zaxis")	==	0)	{
																try	{
																				zaxis	=	new	DPM_axis(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				zaxis	=	NULL;
																				std::cerr	<<	"ERROR:	<plot>	specification	contains	an	invalid	<zaxis>	specification."	<<	std::endl;
																}
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"curve")	==	0)	{
																DPM_curve	*curve;
																try	{
																				curve	=	new	DPM_curve(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				curve	=	NULL;
																				std::cerr	<<	"ERROR:	"	<<	std::endl;
																}
																if	(curve	!=	NULL)	{
																				curve_list.push_back(	curve);
																}
													}	else	{
																std::cerr	<<	"WARNING:	<plot>	specification	contains	an	unknown	element	\"<"	<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}

				time_constraints	=	new	DPM_time_constraints(tstart,	tstop,	frequency);

				//	Validation	criteria:
				//	1.	Relation	requires	at	least	one	curve.

				return	0;
}

//	XML	CONSTRUCTOR
DPM_relation::DPM_relation(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(	Base_node)	<	0)	{
						throw	std::invalid_argument("DPM_relation	specification	is	un-usable.");
		}
}

//	MEMBER	FUNCTION
DPM_relation::~DPM_relation()	{

		delete	xaxis;
		delete	yaxis;
		delete	zaxis;
		if	(title)	{	free(	title);	}
		//	destroy	each	of	the	curves	in	the	curves	list
		int	i;
		int	n	=	(int)this->curve_list.size();
		for	(i	=	0;	i	<	n	;	i	++)	{
						delete	this->curve_list[i];
		}
}

//	MEMBER	FUNCTION
int	DPM_relation::setTitle(const	char	*in_title)	{
		title	=	strdup(in_title);
		return	(0);
}

//	MEMBER	FUNCTION
const	char	*DPM_relation::getTitle()	{
		return	(	(const	char	*)title	);
}

//	MEMBER	FUNCTION
DPM_time_constraints	*DPM_relation::getTimeConstraints()	{
		return	(time_constraints);
}

//	MEMBER	FUNCTION
int	DPM_relation::NumberOfAxes()	{
				int	n_curves,n_axes,n,i;
				n_curves	=	(int)curve_list.size();
				if	(n_curves	<=	0	)	return	(0);

				n_axes	=	curve_list[0]->NumberOfAxes();
				if	(n_curves	>	1)	{
								for	(i=1;	i<n_curves;	i++)	{
												n	=	curve_list[i]->NumberOfAxes();
												if	(n	<	n_axes)	n_axes	=	n;
								}
				}
				return	(n_axes);
}

//	MEMBER	FUNCTION
std::string	DPM_relation::getXAxisLabel()	{
		std::string	candidate_label;
		
		int	n_curves,	i;

		if	(xaxis)	{
						candidate_label	=	xaxis->getLabel();
		}	else	{
						candidate_label	=	"";
		}
		//	If	an	Y-Axis	label	wasn't	supplied,	see	if	there	is	a	common
		//	variable	name	that	will	serve	as	a	label.
		if	(candidate_label	==	"")	{
						if	((	candidate_label	=	curve_list[0]->getXCommonName()	)	==	"")	{
										return	("");
						}
						n_curves	=	(int)curve_list.size();
						for	(i=1;	i<n_curves;	i++)	{
										if	(	curve_list[i]->getXCommonName().empty()	||	candidate_label	!=	curve_list[i]->getXCommonName())	{
															return	("");
										}	
						}
		}
		return	(candidate_label);
}

//	MEMBER	FUNCTION
std::string	DPM_relation::getYAxisLabel()	{
		std::string	candidate_label;
		
		int	n_curves,	i;

		if	(yaxis)	{
						candidate_label	=	yaxis->getLabel();
		}	else	{
						candidate_label	=	"";
		}
		//	If	an	Y-Axis	label	wasn't	supplied,	see	if	there	is	a	common
		//	variable	name	that	will	serve	as	a	label.
		if	(candidate_label	==	"")	{
						if	((	candidate_label	=	curve_list[0]->getYCommonName()	)	==	"")	{
										return	("");
						}
						n_curves	=	(int)curve_list.size();
						for	(i=1;	i<n_curves;	i++)	{
										if	(curve_list[i]->getYCommonName().empty()	||	candidate_label	!=	curve_list[i]->getYCommonName())	{
															return	("");
										}	
						}
		}
		return	(candidate_label);
}

//	MEMBER	FUNCTION
std::string	DPM_relation::getZAxisLabel()	{
		std::string	candidate_label;
		
		int	n_curves,	i;

		if	(zaxis)	{
						candidate_label	=	zaxis->getLabel();
		}	else	{
						candidate_label	=	"";
		}
		//	If	an	Z-Axis	label	wasn't	supplied,	see	if	there	is	a	common
		//	variable	name	that	will	serve	as	a	label.
		if	(candidate_label	==	"")	{
						if	((	candidate_label	=	curve_list[0]->getZCommonName()	)	==	"")	{
										return	("");
						}
						n_curves	=	(int)curve_list.size();
						for	(i=1;	i<n_curves;	i++)	{
										if	(curve_list[i]->getZCommonName().empty()	||	candidate_label	!=	curve_list[i]->getZCommonName())	{
															return	("");
										}	
						}
		}
		return	(candidate_label);
}

//	MEMBER	FUNCTION
		const	char	*	DPM_relation::getXAxisUnits()	{
		if	(xaxis)	{
						return(	xaxis->getUnits());
		}
		return	NULL;
}

//	MEMBER	FUNCTION
		const	char	*	DPM_relation::getYAxisUnits()	{
		if	(yaxis)	{
						return(	yaxis->getUnits());
		}
		return	NULL;
}

//	MEMBER	FUNCTION
		const	char	*	DPM_relation::getZAxisUnits()	{
		if	(zaxis)	{
						return(	zaxis->getUnits());
		}
		return	NULL;
}

		const	char	*	DPM_relation::getYAxisFormat()	{
						if	(yaxis)	{
										return	yaxis->AttributeValue("format");
						}	else	{
										return	NULL;
						}
		}

//	MEMBER	FUNCTION
int	DPM_relation::NumberOfCurves()	{
				return	((int)curve_list.size());
}

//	MEMBER	FUNCTION
DPM_curve	*DPM_relation::getCurve(unsigned	int	index)	{
		if	(index	<	curve_list.size(	))		{
				return	(	curve_list[index]);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_relation	*relation)	{
		int	i;
		int	n_attr	=	(int)relation->attribute_list.size();
		int	n_curve	=	(int)relation->curve_list.size();

		s	<<	"<plot";
		for	(i=0	;	i<n_attr	;	i++)	{
				s	<<	"	"	<<	relation->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;
		if	(relation->title)	{
				s	<<	"<title>"		<<	relation->title		<<	"</title>"		<<	std::endl;
		}
		s	<<	"<tstart>"	<<	relation->time_constraints->getStart()	<<	"</tstart>"	<<	std::endl;
		s	<<	"<tstop>"		<<	relation->time_constraints->getStop()		<<	"</tstop>"		<<	std::endl;
		s	<<	"<frequency>"		<<	relation->time_constraints->getFrequency()		<<	"</frequency>"		<<	std::endl;

		if	(relation->xaxis)	{	s	<<	relation->xaxis;	}
		if	(relation->yaxis)	{	s	<<	relation->yaxis;	}
		if	(relation->zaxis)	{	s	<<	relation->zaxis;	}

		for	(i=0	;	i<n_curve	;	i++)	{
				s	<<	relation->curve_list[i];
		}

		s	<<	"</plot>";
		s	<<	std::endl;
		return	s;
}

