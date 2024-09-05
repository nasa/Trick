
#include	<string.h>

#include	"DPM_inputs.hh"

//	MEMBER	FUNCTION
int	DPM_inputs::Initialize(	xmlNode	*base_node)	{
				int	n_vars;

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_inputs::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"inputs")	!=	0)	{
							std::cerr	<<"ERROR:	Expected	an	<inputs>	specification,	but	alas	it	wasn\'t	found."	<<	std::endl;
							return	-1;
				}

				xmlNode	*current_node	=	base_node->children;
				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
								DPM_var	*var;
												if	(	strcmp(	(const	char	*)current_node->name,	"var")	==	0)	{
																try	{
																				var	=	new	DPM_var(this,	current_node);
																}	catch	(std::invalid_argument)	{
																				var	=	NULL;
																				std::cerr	<<"ERROR:	<input>	specification	contains	an	invalid	<var>	specification."	<<	std::endl;
																				std::cerr	<<"::::::	Therefore	the	entire	<inputs>	specification	is	invalid."	<<	std::endl;
																				return	-1;
																}
																if	(var	!=	NULL)	{
																				var_list.push_back(	var	);
																}
												}	else	{
																std::cerr	<<	"WARNING:	<input>	specification	contains	the	unknown	element	\"<"	<<
																								current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}

				//	Validation
				//	1.	Inputs	must	have	at	least	one	var.
				n_vars	=	(int)var_list.size();
				if	(	n_vars	<	1)	{
								std::cerr	<<"ERROR:	<inputs>	specification	must	have	at	least	one	<var>	specification."	<<	std::endl;
								return	-1;
				}
				return	0;
}

//	CONSTRUCTOR
DPM_inputs::DPM_inputs(DPM_component	*Parent,	xmlNode	*Base_node)
					:	DPM_component(Parent,	Base_node)	{

		if	(	Initialize(	Base_node)	<	0)	{
						throw	std::invalid_argument("<inputs>	specification	is	invalid	and	un-usable.");
		}
}

//	DESTRUCTOR
DPM_inputs::~DPM_inputs()	{
				//	Destroy	all	of	the	strings	in	the	var_list.
				int	i;
				int	n	=	(int)var_list.size();
				for	(i	=	0;	i	<	n	;	i	++)	{
									delete	var_list[i];
				}
}

//	MEMBER	FUNCTION
int	DPM_inputs::NumberOfInputs()	{
		return	((int)var_list.size());
}

//	MEMBER	FUNCTION
const	char	*DPM_inputs::getVarName(unsigned	int	index)	{

		if	(index	<	var_list.size(	))		{
				return	(	var_list[index]->getName()	);
		}	else	{
				return	NULL;
		}
}

//	MEMBER	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_inputs	*inputs)	{
					int	i;
					int	n_attrs	=	(int)inputs->attribute_list.size();
					int	n_vars		=	(int)inputs->var_list.size();

					s	<<	"<inputs";
					for	(i=0	;	i<n_attrs	;	i++)	{
										s	<<	"	"	<<	inputs->attribute_list[i];
					}
					s	<<	">"	<<	std::endl;

					for	(i=0	;	i<n_vars	;	i++)	{
										s	<<	inputs->var_list[i];
					}
					s	<<	"</inputs>"	<<	std::endl;
					return	s;
}

