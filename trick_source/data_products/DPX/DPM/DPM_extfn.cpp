
#include	<string.h>

#include	"DPM_extfn.hh"

int	DPM_extfn::Initialize(	xmlNode	*base_node)	{

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_extfn::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}

				if	(	strcmp(	(char	*)base_node->name,	"extfn")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<extfn>	specification	but	didnt	find	it."	<<	std::endl;
								return	-1;
				}

				xmlNode	*current_node	=	base_node->children;

				while	(current_node	!=	NULL)	{

						if	(current_node->type	==	XML_ELEMENT_NODE)	{
	if	(	strcmp(	(const	char	*)current_node->name,	"fname")	==	0)	{

			xmlNode	*text_node	=	current_node->children;
			fname	=	strdup(	(const	char	*)text_node->content);

	}	else	if	(	strcmp(	(const	char	*)current_node->name,	"inputs")	==	0)	{

									try	{
													inputs	=	new	DPM_inputs(this,	current_node);
									}	catch	(std::invalid_argument)	{
													inputs	=	NULL;
													std::cerr	<<	"ERROR:	<inputs>	specification	is	invalid."	<<	std::endl;
									}

	}	else	if	(	strcmp(	(const	char	*)current_node->name,	"outputs")	==	0)	{

									try	{
							outputs	=	new	DPM_outputs(this,	current_node);
									}	catch	(std::invalid_argument)	{
													inputs	=	NULL;
													std::cerr	<<	"ERROR:	<outputs>	specification	is	invalid."	<<	std::endl;
									}

	}	else	{
									std::cerr	<<	"WARNING:	Unknown	element	\""	<<	current_node->name	<<	"\".	Skipping."	<<	std::endl;
	}
						}
						current_node	=	current_node->next;
				}

				//	Validation
				//	1.	function	name	is	required.	Its	needed	to	identify	the	external	dynamically	loaded	function.
				//	2.	inputs	are	required.
				//	3.	outputs	are	required.

				if	(fname	==	NULL)	{
								std::cerr	<<	"ERROR:	<extfn>	specification	is	missing	an	<fname>	(function	name)	specification."	<<	std::endl;
								return	-1;
				}
				if	(inputs	==	NULL)	{
								std::cerr	<<	"ERROR:	<extfn>	specification	is	missing	an	<inputs>	specification."	<<	std::endl;
								return	-1;
				}
				if	(outputs	==	NULL)	{
								std::cerr	<<	"ERROR:	<extfn>	specification	is	missing	an	<outputs>	specification."	<<	std::endl;
								return	-1;
				}
				return	0;
}

//	XML	CONSTRUCTOR
DPM_extfn::DPM_extfn(DPM_component	*Parent,	xmlNode	*Base_node)
		:	DPM_component	(Parent,	Base_node)	{

		if	(	Initialize(	Base_node)	<	0)	{
						throw	std::invalid_argument("<extfn>	specification	is	un-usable.");
		}
}

//	DESTRUCTOR
DPM_extfn::~DPM_extfn()	{
		if	(inputs)	{	delete	inputs;	}
		if	(outputs)	{	delete	outputs;	}
}

//	MEMBER	FUNCTION
const	char	*DPM_extfn::getFName()	{
		return	(	(const	char	*)fname);
}

//	MEMBER	FUNCTION
int	DPM_extfn::NumberOfInputs()	{
		return(	inputs->NumberOfInputs()	);
}

//	MEMBER	FUNCTION
const	char*	DPM_extfn::getInputVar(	unsigned	int	index)	{
		return(	inputs->getVarName(index)	);
}

//	MEMBER	FUNCTION
int	DPM_extfn::NumberOfOutputs()	{
		return	(	outputs->NumberOfOutputs()	);
}

//	MEMBER	FUNCTION
const	char*	DPM_extfn::getOutputVar(	unsigned	int	index)	{
		return(	outputs->getVarName(index)	);
}

//	MEMBER	FUNCTION
const	char*	DPM_extfn::getOutputUnits(	unsigned	int	index)	{
		return(	outputs->getUnits(index)	);
}

//	MEMBER	FUNCTION
int	DPM_extfn::index_of(	const	char*	var_name)	{
		//	NOT	YET	IMPLEMENTED.
		return	(0);
}

//	FRIEND	FUNCTION
std::ostream&	operator<<	(std::ostream&	s,	const	DPM_extfn	*extfn)	{
		int	i;
		int	n	=	(int)extfn->attribute_list.size();

		s	<<	"<extfn";
		for	(i=0	;	i<n	;	i++)	{
				s	<<	"	"	<<	extfn->attribute_list[i];
		}
		s	<<	">";
		s	<<	std::endl;
		s	<<	extfn->inputs;
		s	<<	extfn->outputs;
		s	<<	"</relation>";
		s	<<	std::endl;
		return	s;
}
