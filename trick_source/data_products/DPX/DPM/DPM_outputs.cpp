
#include	<string.h>

#include	"DPM_outputs.hh"

//	MEMBER	FUNCTION
int	DPM_outputs::Initialize(	xmlNode	*base_node)	{
				int	n_measurements;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_outputs::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"outputs")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<outputs>	specification	but	alas,	is	wasn\'t	found."	<<	std::endl;
								return	-1;
				}
				/*	Process	children	*/
				xmlNode	*current_node	=	base_node->children;
				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												DPM_measurement	*measurement;

												if	(	strcmp(	(const	char	*)current_node->name,	"measurement")	==	0)	{
																try	{
																				measurement	=	new	DPM_measurement(this,	current_node);
																}	catch	(std::invalid_argument)	{
																				measurement	=	NULL;
																				std::cerr	<<	"ERROR:	<measurement>	is	invalid."	<<	std::endl;
																}
																if	(measurement	!=	NULL)	{
																				measurement_list.push_back(measurement);
																}
												}	else	{
																std::cerr	<<	"WARNING:	<outputs>	specification	contains	an	unknown	element	\"<"	<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}
				//	Validation
				//	1.		Outputs	must	have	at	least	one	valid	measurement.
				n_measurements	=	(int)measurement_list.size();
				if	(	n_measurements	<	1)	{
								std::cerr	<<	"ERROR:	<outputs>	specification	requires	at	least	one	valid	<measurement>	specification."	<<	std::endl;
								return	-1;
				}
				return	0;
}

//	CONSTRUCTOR
DPM_outputs::DPM_outputs(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		Initialize(	Base_node);
}

//	DESTRUCTOR
DPM_outputs::~DPM_outputs()	{
		int	i,n;
		n	=	(int)measurement_list.size();
		for	(i	=	0;	i	<	n	;	i	++)	{
				DPM_measurement	*measurement;
				measurement	=	measurement_list[i];
				if	(measurement)	{	delete	measurement;	}
		}
}

//	MEMBER	FUNCTION
int	DPM_outputs::NumberOfOutputs()	{
		return(		(int)measurement_list.size());
}

//	MEMBER	FUNCTION
const	char	*DPM_outputs::getVarName(	unsigned	int	index)	{
		if	(index	<	measurement_list.size(	))		{
				return	(	measurement_list[index]->getVarName()	);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
const	char	*DPM_outputs::getUnits(	unsigned	int	index)	{
		if	(index	<	measurement_list.size(	))		{
				return	(	measurement_list[index]->getUnits()	);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_outputs	*outputs)	{
		int	i;
		int	n_attrs	=	(int)outputs->attribute_list.size();
		int	n_measurements		=	(int)outputs->measurement_list.size();

		s	<<	"<outputs";
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	outputs->attribute_list[i];
		}
		s	<<	">"	<<	std::endl;

		for	(i=0	;	i<n_measurements	;	i++)	{
				s	<<	outputs->measurement_list[i];
		}
		s	<<	"</inputs>"	<<	std::endl;
		return	s;
}
