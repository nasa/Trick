
#include	<string.h>

#include	"DPM_run.hh"

int	DPM_run::Initialize(	xmlNode	*base_node)	{

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_run::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"run")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<run>	specification	but	alas	it	wasn\'t	found."	<<	std::endl;
								return	-1;
				}

				time_name	=	AttributeValue("timename");

				//	If	timename	is	not	defined	in	the	xml	file,	use	default	one.
				if	(time_name	==	NULL)	{
					//	Set	the	default	time	name.
					time_name	=	"sys.exec.out.time";
				}

				/*	Process	children	*/
				xmlNode	*current_node	=	base_node->children;
				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												if	(	strcmp(	(const	char	*)current_node->name,	"dir")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																dir	=	strdup(	(const	char	*)text_node->content);
												}	else	{
																std::cerr	<<	"WARNING:	<run>	specification	contains	an	unknown	element	\"<"
																					<<	current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}
				return	0;
}

//	CONSTRUCTOR
DPM_run::DPM_run(DPM_component	*Parent,	xmlNode	*Base_node)
					:	DPM_component	(Parent,	Base_node)	{

					if	(	Initialize(	Base_node)	<	0)	{
									throw	std::invalid_argument("DPM_run	specification	is	un-usable.");
					}
}

//	DESTRUCTOR
DPM_run::~DPM_run()	{

		free(	dir);
}

const	char*	DPM_run::getDir()	{
		return	(	(const	char*)dir	);
}

const	char*	DPM_run::getTimeName()	{
		return	time_name;
}

std::ostream&	operator<<	(std::ostream&	s,	const	DPM_run	*run)	{
					int	i;
					int	n	=	(int)run->attribute_list.size();

					s	<<	"<run";
					for	(i=0	;	i<n	;	i++)	{
										s	<<	"	"	<<	run->attribute_list[i];
					}
					s	<<	">";
					s	<<	"<dir>";
					s	<<	run->dir;
					s	<<	"</dir>";
					s	<<	"</run>";
					s	<<	std::endl;
					return	s;
}
