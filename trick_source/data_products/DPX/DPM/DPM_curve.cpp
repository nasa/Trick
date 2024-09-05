
#include	<string.h>

#include	"DPM_curve.hh"
#include	"float.h"

int	DPM_curve::Initialize(xmlNode	*base_node)	{

				int	axis_index	=	0;
				int	number_of_axes;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_curve::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"curve")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<curve>	specification	but	alas	didnt	find	it."	<<	std::endl;
								return	-1;
				}
				/*	Process	children	*/
				xmlNode	*current_node	=	base_node->children;

				/*	Get	the	line	number	of	this	curve.	*/
				line_number	=	current_node->line;


			while(	(current_node	!=	NULL)	&&
										(current_node->type	!=	XML_ELEMENT_NODE)	)	{
								current_node	=	current_node->next;
			}

			if	(current_node	==	NULL)	{
								std::cerr	<<	"ERROR:	Expected	<var>	or	<varcase>	specification	but	alas	didnt	find	it."	<<	std::endl;
								return	-1;
			}

			if	(strcmp(	(const	char	*)current_node->name,	"var")	==	0)	{
							while	(current_node	!=	NULL)	{
											if	(current_node->type	==	XML_ELEMENT_NODE)	{
															if	(	strcmp(	(const	char	*)current_node->name,	"var")	==	0)	{
																			DPM_var	*var	=	new	DPM_var(this,	current_node);
																			if	(var	!=	NULL)	{
																							switch	(axis_index)	{
																							case	0	:
																										x_varcase_list.push_back(var);
																							break;
																							case	1	:
																										y_varcase_list.push_back(var);
																							break;
																							case	2	:
																										z_varcase_list.push_back(var);
																							break;
																							default:
																											std::cerr	<<	"ERROR:	Too	many	<var>	elements	in	<curve>."			<<	std::endl;
																											std::cerr	<<	"							at	line	"	<<	line_number	<<	"."								<<	std::endl;
																											std::cerr	<<	"							There	can	be	no	more	than	three	<var>"	<<	std::endl;
																											std::cerr	<<	"							elements	in	a	<curve>."																<<	std::endl;
																											return	-1;
																							break;
																							}
																			}
																			axis_index++;
															}	else	{
																			std::cerr	<<	"WARNING:	<curve>	specification	contains	an	unknown	element	\"<"
																								<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
															}
											}
											current_node	=	current_node->next;
							}
				}		else	if	(strcmp(	(const	char	*)current_node->name,	"varcase")	==	0)	{
										while	(current_node	!=	NULL)	{
														if	(current_node->type	==	XML_ELEMENT_NODE)	{
																		if	(	strcmp(	(const	char	*)current_node->name,	"varcase")	==	0)	{
																						xmlNode	*child_current_node	=	current_node->children;
																						axis_index	=	0;
																						while	(child_current_node	!=	NULL)	{
																										if	(child_current_node->type	==	XML_ELEMENT_NODE)	{
																														if	(	strcmp(	(const	char	*)child_current_node->name,	"var")	==	0)	{
																																		DPM_var	*var	=	new	DPM_var(this,	child_current_node);
																																		if	(var	!=	NULL)	{
																																						switch	(axis_index)	{
																																										case	0	:
																																													x_varcase_list.push_back(var);
																																										break;
																																										case	1	:
																																													y_varcase_list.push_back(var);
																																										break;
																																										case	2	:
																																													z_varcase_list.push_back(var);
																																										break;
																																										default:
																																														std::cerr	<<	"ERROR:	Too	many	<var>	elements	in	<varcase>."	<<	std::endl;
																																														std::cerr	<<	"							at	line	"	<<	line_number	<<	"."								<<	std::endl;
																																														std::cerr	<<	"							There	can	be	no	more	than	three	<var>"	<<	std::endl;
																																														std::cerr	<<	"							elements	in	a	<varcase>."														<<	std::endl;
																																														return	-1;
																																										break;
																																						}
																																		}
																																		axis_index++;
																														}	else	{
																																		std::cerr	<<	"WARNING:	<varcase>	specification	contains	an	unknown	element	\"<"
																																		<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
																														}
																										}
																										child_current_node	=	child_current_node->next;
																						}
																		}	else	{
																						std::cerr	<<	"WARNING:	<curve>	specification	contains	an	unknown	element	\"<"
																											<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
																		}
														}
														current_node	=	current_node->next;
										}
			}	else	{
								std::cerr	<<	"ERROR:	Expected	<var>	or	<varcase>	specification	but	alas	didnt	find	it."	<<	std::endl;
								return	-1;
			}

				//	Validation	criteria:
				number_of_axes	=	this->NumberOfAxes();

				if	(	!(number_of_axes	==	2)	&&	!(number_of_axes	==	3))	{
								return	-1;
				}

				return	0;
}

//	XML	CONSTRUCTOR
DPM_curve::DPM_curve(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(	Base_node)	<	0)	{
						throw	std::invalid_argument("DPM_table	specification	is	un-usable.");
		}
}

//	MEMBER	FUNCTION
DPM_curve::~DPM_curve()	{

//	FIXME	delete	all	of	the	elements	of	the	varcase	lists.
		if	(x_varcase_list.size())	{	x_varcase_list.clear();	}
		if	(y_varcase_list.size())	{	y_varcase_list.clear();	}
		if	(z_varcase_list.size())	{	z_varcase_list.clear();	}
}

//	MEMBER	FUNCTION
		int	DPM_curve::NumberOfAxes()	{
				int	number_of_axes;
				int	x_case_count	=	(int)x_varcase_list.size();
				int	y_case_count	=	(int)y_varcase_list.size();
				int	z_case_count	=	(int)z_varcase_list.size();

				if	(x_case_count	==	y_case_count)	{
								if	(z_case_count	==	0)	{
												number_of_axes	=	2;
								}	else	if	(z_case_count	==	x_case_count)	{
												number_of_axes	=	3;
								}	else	{
												std::cerr	<<	"INTERNAL	ERROR:	Variables	lists	are	messed	up.(1)"	<<	std::endl;
												return	0;
								}
				}	else	{
								std::cerr	<<	"INTERNAL	ERROR:	Variables	lists	are	messed	up.(2)"	<<	std::endl;
								std::cerr	<<	"x_case_count	="	<<	x_case_count	<<	std::endl;
								std::cerr	<<	"y_case_count	="	<<	y_case_count	<<	std::endl;
								std::cerr	<<	"z_case_count	="	<<	z_case_count	<<	std::endl;
								return	0;
				}
				return	(	number_of_axes);
}

//	MEMBER	FUNCTION
		int	DPM_curve::NumberOfVarCases()	{
		return	(	(int)x_varcase_list.size());
}

//	MEMBER	FUNCTION
DPM_var	*DPM_curve::getXVar(unsigned	int	index)	{
		if	(index	<	x_varcase_list.size(	))		{
				return	(	x_varcase_list[index]	);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
DPM_var	*DPM_curve::getYVar(unsigned	int	index)	{
		if	(index	<	y_varcase_list.size(	))		{
				return	(	y_varcase_list[index]	);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
DPM_var	*DPM_curve::getZVar(unsigned	int	index)	{
		if	(index	<	z_varcase_list.size(	))		{
				return	(	z_varcase_list[index]	);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
const	char	*	DPM_curve::getXVarName(unsigned	int	case_index)	{
		if	(case_index	<	x_varcase_list.size())		{
				return	(	x_varcase_list[case_index]->getName());
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
const	char	*	DPM_curve::getYVarName(unsigned	int	case_index)	{
		if	(case_index	<	y_varcase_list.size())		{
				return	(	y_varcase_list[case_index]->getName());
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
const	char	*	DPM_curve::getZVarName(unsigned	int	case_index)	{
		if	(case_index	<	z_varcase_list.size())		{
				return	(	z_varcase_list[case_index]->getName());
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::string	DPM_curve::getXCommonName()	{

		std::string	candidate_label;
		
		int	n_vars,	i;

		candidate_label	=	x_varcase_list[0]->getShortName();
		n_vars	=	(int)x_varcase_list.size();
		for	(i=1;	i<n_vars;	i++)	{
					if	(candidate_label	!=	x_varcase_list[i]->getShortName())	{
								return	("");
					}
		}
		return	(candidate_label);
}

//	MEMBER	FUNCTION
std::string	DPM_curve::getYCommonName()	{

		std::string	candidate_label;
		
		int	n_vars,	i;

		candidate_label	=	y_varcase_list[0]->getShortName();
		n_vars	=	(int)y_varcase_list.size();
		for	(i=1;	i<n_vars;	i++)	{
					if	(candidate_label	!=	y_varcase_list[i]->getShortName())	{
								return	("");
					}
		}
		return	(	candidate_label);
}

//	MEMBER	FUNCTION
std::string	DPM_curve::getZCommonName()	{

		std::string	candidate_label;
		
		int	n_vars,	i;

		candidate_label	=	z_varcase_list[0]->getShortName();
		n_vars	=	(int)z_varcase_list.size();
		for	(i=1;	i<n_vars;	i++)	{
					if	(candidate_label	!=	z_varcase_list[i]->getShortName())	{
								return	("");
					}
		}
		return	(candidate_label);
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_curve	*curve)	{
		int	i;
		//int	n	=	(int)curve->attribute_list.size();
		int	n_x		=	curve->x_varcase_list.size();
		int	n_y		=	curve->y_varcase_list.size();
		int	n_z		=	curve->z_varcase_list.size();

		s	<<	"<curve>"	<<	std::endl;
		//s	<<	"<curve";
		//for	(i=0	;	i<n	;	i++)	{
		//		s	<<	"	"	<<	curve->attribute_list[i];
		//}
		//s	<<	">"	<<	s	<<	std::endl;

		//	Print	out	the	variables.
		for	(i=0;	i<n_x;	i++)	{
				s	<<	curve->x_varcase_list[i];
		}

		for	(i=0;	i<n_y;	i++)	{
				s	<<	curve->y_varcase_list[i];
		}

		for	(i=0;	i<n_z;	i++)	{
				s	<<	curve->z_varcase_list[i];
		}

		s	<<	"</curve>";
		s	<<	std::endl;
		return	s;
}

