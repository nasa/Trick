
#include	"DPM_component.hh"

//	DEFAULT	CONSTRUCTOR
DPM_component::DPM_component()	{
				parent	=	NULL;
}

void	DPM_component::add_attribute(const	char	*key,	const	char*	value)	{
				std::string	key_string(	key);
				std::string	value_string(	value);
				attribute_list.push_back(	new	DPM_attribute(key_string,	value_string)	);
}

void	DPM_component::Initialize(	xmlNode	*Base_node)	{

				if	(Base_node	!=	NULL)	{
								if	(Base_node->type	!=	XML_ELEMENT_NODE	)	{
												std::cerr	<<	"DPM_component::Initialize:	Bad	parameters."	<<	std::endl;
								}	else	{
												xmlAttr	*current_attr	=	NULL;

												/*	Get	Attributes	from	the	XML	parse	tree.	*/
												for	(current_attr	=	Base_node->properties;	current_attr	;	current_attr	=	current_attr->next	)	{
																if	(current_attr->type	==	XML_ATTRIBUTE_NODE	)	{
																				add_attribute(	(const	char	*)current_attr->name,
																																			(const	char	*)current_attr->children->content);
																}
												}
								}
				}
}

//	XML	CONSTRUCTOR
DPM_component::DPM_component(DPM_component	*Parent,	xmlNode	*Base_node)	{

				Initialize(	Base_node);
				parent	=	Parent;
}

//	DESTRUCTOR
DPM_component::~DPM_component()	{
				//	destroy	each	of	the	attributes	in	the	attribute	list
				int	i;
				int	n	=	(int)this->attribute_list.size();
				for	(i	=	0;	i	<	n	;	i	++)	{
								delete	this->attribute_list[i];
				}
}

//	MEMBER	FUNCTION
int	DPM_component::NumberOfAttributes()	{
				return	(int)attribute_list.size();
}

//	MEMBER	FUNCTION
const	char	*DPM_component::AttributeValue(	const	char	*name	)	{
				int	i,	ret;
				int	n_attrs	=	(int)attribute_list.size();
				for	(i=0	;	i<n_attrs	;	i++)	{
								if	(	(ret	=	attribute_list[i]->key_is(name))	)	{
												return	(	attribute_list[i]->value_of()->c_str()	);
								}
				}
				//	We	didnt	find	the	attribute	in	the	current	component
				//	so	check	the	parent	component	for	the	attribute.
				if	(	parent	)	{
								return	(	parent->AttributeValue(name)	);
				}

				return	NULL;
}
