
#include	<string.h>

#include	"DPM_var.hh"

DPM_var::DPM_var(const	char*	Name)
								:DPM_component()	{
			name	=	strdup(	Name);
}

int	DPM_var::Initialize(	xmlNode	*base_node)	{

				if	(base_node	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_var::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}
				if	(	strcmp(	(char	*)base_node->name,	"var")	!=	0)	{
							return	-1;
				}	else	{
							xmlNode	*text_node	=	base_node->children;
	name	=	strdup(	(const	char	*)text_node->content);
				}
				return	0;
}

//	CONSTRUCTOR
DPM_var::DPM_var(DPM_component	*Parent,	xmlNode	*Base_node)
					:	DPM_component	(Parent,	Base_node)	{

					if	(	Initialize(	Base_node)	<	0)	{
									throw	std::invalid_argument("DPM_var	specification	is	un-usable.");
					}
}

//	DESTRUCTOR
DPM_var::~DPM_var()	{
}

const	char*	DPM_var::getName()	{
		return	(	(const	char*)name	);
}

std::string	DPM_var::getShortName()	{
				char	*p;

				std::string::size_type	idx	;

				//	Make	a	string	out	of	the	parameter	name
				std::string	pName	=	name	;

				//	Is	this	a	delta(<var:run>,<var:run>)	parameter?
				idx	=	pName.find("delta(");
				//	if	not,	the	short	name	is	that	last	part	of	whole	name	path
				if	(idx	!=	0	)	{
								if	((p=strrchr(	name,'.'))	!=	NULL)	{
												p++;
												return	(	(const	char*)p);
								}	else	{
												return	(	(const	char*)name	);
								}
				}	else	{
								//	if	is	a	delta(...),	the	short	name	is	<last	part	of	var1>	-	<last	part	of	var2>
								int	ret;
								char	vr1[1024];
								char	vr2[1024];
								std::string	paramName1;
								std::string	paramName2;
								std::string	combinedName;

								//	The	delta	statement	is	formatted	like:
								//	delta(<var1:run1>,	<var2:run2>)

								//	Extract	<var1:run1>	<var2:run2>
								ret	=	sscanf(name,	"delta(%[^,],%[^)]",	vr1,	vr2);
								if	(	ret	!=	2	)	{
												fprintf(stderr,	"ERROR:	Delta::Delta()	received	bad	spec	\"%s\"\nAborting.\n",	name);
												exit(-1);
								}

								//	now	vr1	is	var1:run1	and	vr2	is	var2:run2
								//	only	var1	and	var2	are	needed
								paramName1	=	vr1;
								idx	=	paramName1.find(':');
								if	(	idx	!=	std::string::npos	)	{
												//	Extract	var1	name	from	vr1
												paramName1	=	paramName1.substr(0,idx)	;
								}
								if	((p=strrchr(	(char*)paramName1.c_str(),'.'))	!=	NULL)	{
												p++;
												combinedName	=	std::string(p);
								}	else	{
												combinedName	=	paramName1;
								}
								paramName2	=	vr2;
								idx	=	paramName2.find(':');
								if	(	idx	!=	std::string::npos	)	{
												//	Extract	var2	name	from	vr2
												paramName2	=	paramName2.substr(0,idx)	;
								}
								if	((p=strrchr(	(char*)paramName2.c_str(),'.'))	!=	NULL)	{
												p++;
												combinedName	+=	"	-	"	+	std::string(p);
								}	else	{
												combinedName	+=	"	-	"	+	paramName2;
								}
								return	combinedName;
				}
}

std::ostream&	operator<<	(std::ostream&	s,	const	DPM_var	*var)	{
					int	i;
					int	n	=	(int)var->attribute_list.size();

					s	<<	"<var";
					for	(i=0	;	i<n	;	i++)	{
										s	<<	"	"	<<	var->attribute_list[i];
					}
					s	<<	">";
					s	<<	var->name;
					s	<<	"</var>";
					s	<<	std::endl;
					return	s;
}
