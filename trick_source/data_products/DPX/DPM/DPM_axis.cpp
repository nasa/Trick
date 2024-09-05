
#include	<udunits2.h>
#include	<string.h>
#include	"DPM_axis.hh"

extern	ut_system	*	u_system	;

int	DPM_axis::Initialize(xmlNode	*base_node)	{

				xmlNode	*current_node;

				label					=	NULL;
				unitspec		=	NULL;
				axis_type	=	DPM_AXIS_NONE;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_axis::Initialize:	Bad	parameters."	<<	std::endl;
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
												}	else	{
																std::cerr	<<	"WARNING:	<axis>	specification	contains	an	unknown	element	\"<"
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
												std::cerr	<<	"ERROR:	<axis>	specifies	invalid	units:	\""	<<	unitspec	<<	"\"."	<<	std::endl;
												return	-1;
								}
								ut_free(validation_unit)	;
				}

				return	0;
}

//	XML	CONSTRUCTOR
DPM_axis::DPM_axis(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(Base_node)	<	0)	{
						throw	std::invalid_argument("DPM_axis	specification	is	un-usable.");
		}
}

//	DESTRUCTOR
DPM_axis::~DPM_axis()	{
		if	(unitspec)	{	free	(unitspec);	}
		if	(label)	{	free	(label);	}
}

//	MEMBER	FUNCTION
const	char	*DPM_axis::getLabel()	{
		if	(label	!=	NULL)	{
						return	((const	char*)label);
		}

return	NULL;
}

//	MEMBER	FUNCTION
const	char	*DPM_axis::getUnits()	{
		return	(	(const	char*)unitspec	);
}


//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_axis	*axis)	{
		int	i;
		int	n_attrs	=	(int)axis->attribute_list.size();

		s	<<	"<axis";
		//	Output	the	axis	element	attributes.
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	axis->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;
		//	Output	the	optional	label	(if	it	exists).
		if	(strcmp(axis->label,""))	{
				s	<<	"<label>"	<<	axis->label	<<	"</label>"	<<	std::endl;
		}
		//	Output	the	unitspec.
		s	<<	"<units>"	<<	axis->unitspec	<<	"</units>"	<<	std::endl;

		s	<<	"</axis>";
		s	<<	std::endl;
		return	s;
}

