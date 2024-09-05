
#include	<udunits2.h>
#include	<string.h>
#include	"DPM_column.hh"

extern	ut_system	*	u_system	;

int	DPM_column::Initialize(xmlNode	*base_node)	{

				xmlNode	*current_node;

				label					=	NULL;
				unitspec		=	NULL;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_column::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}

				current_node	=	base_node->children;
				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												if	(	strcmp(	(const	char	*)current_node->name,	"label")	==	0)	{
																//	label	is	optional.
																xmlNode	*text_node	=	current_node->children;
																label	=	strdup(	(const	char	*)text_node->content);
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"units")	==	0)	{
																//	units	is	optional.
																xmlNode	*text_node	=	current_node->children;
																unitspec	=	strdup(	(const	char	*)text_node->content);
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"var")	==	0)	{
																DPM_var	*var	=	new	DPM_var(this,	current_node);
																if	(var	!=	NULL)	{
																			column_var	=	var;
																}
												}	else	{
																std::cerr	<<	"WARNING:	<column>	specification	contains	an	unknown	element	\"<"
																					<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}

				//	Validation	Criteria:
				//	1.	If	units	are	supplied,	they	must	be	valid.

				if	(unitspec	!=	NULL)	{
								ut_unit	*	validation_unit	=	ut_parse(u_system,	unitspec,	UT_ASCII)	;
								if(	validation_unit	==	NULL	)	{
												std::cerr	<<	"ERROR:	<column>	specifies	invalid	units:	\""	<<	unitspec	<<	"\"."	<<	std::endl;
												return	-1;
								}
								ut_free(validation_unit)	;
				}

				return	0;
}

//	XML	CONSTRUCTOR
DPM_column::DPM_column(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(Base_node)	<	0)	{
						throw	std::invalid_argument("DPM_column	specification	is	un-usable.");
		}
}

//	DESTRUCTOR
DPM_column::~DPM_column()	{
		if	(unitspec)	{	free	(unitspec);	}
		if	(label)	{	free	(label);	}
		delete	column_var	;
}

//	MEMBER	FUNCTION
const	char	*DPM_column::getLabel()	{
		if	(label	!=	NULL)	{
						return	((const	char*)label);
		}

return	NULL;
}

//	MEMBER	FUNCTION
const	char	*DPM_column::getUnits()	{
		return	(	(const	char*)unitspec	);
}

DPM_var	*DPM_column::getVar()	{
		return	(	column_var	);
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_column	*column)	{
		int	i;
		int	n_attrs	=	(int)column->attribute_list.size();

		s	<<	"<column";
		//	Output	the	column	element	attributes.
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	column->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;
		//	Output	the	optional	label	(if	it	exists).
		//if	(strcmp(column->label,""))	{
		if	(column->label)	{
				s	<<	"<label>"	<<	column->label	<<	"</label>"	<<	std::endl;
		}
		//	Output	the	unitspec.
		if	(column->unitspec)	{
				s	<<	"<units>"	<<	column->unitspec	<<	"</units>"	<<	std::endl;
		}

		s	<<	column->column_var;


		s	<<	"</column>";
		s	<<	std::endl;
		return	s;
}

