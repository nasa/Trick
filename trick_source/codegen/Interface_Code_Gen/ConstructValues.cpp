
#include	<iostream>
#include	<sstream>
#include	<clang/AST/DeclTemplate.h>

#include	"ConstructValues.hh"

//====================================================================================

bool	ConstructValues::ContainerClass::extractTemplateArgType(const	clang::TemplateArgument&	ta)	{
				clang::QualType	qt	=	ta.getAsType()	;
				const	clang::Type	*	t	=	qt.getTypePtrOrNull()	;
				if	(	t	!=	NULL	)	{
								ContainerClass	inner_cc	;
								if	(t->getTypeClass()	==	clang::Type::Record	and	t->getAsCXXRecordDecl())	{
												clang::CXXRecordDecl	*	crd	=	t->getAsCXXRecordDecl()	;
												if	(	clang::isa<clang::ClassTemplateSpecializationDecl>(crd)	)	{
																//	template	argument	is	a	template	specialization
																if	(!inner_cc.extractType(clang::cast<clang::ClassTemplateSpecializationDecl>(crd)))	{
																				return	false	;
																}
												}	else	{
																//	template	argument	record	type,	but	not	a	template	specialization
																inner_cc.name	=	qt.getAsString()	;
												}
								}	else	{
												//	template	argument	not	a	record	type.
												inner_cc.name	=	qt.getAsString()	;
								}
								template_args.push_back(inner_cc)	;
				}
				return	true	;
}

bool	ConstructValues::ContainerClass::extractType(const	clang::RecordDecl	*	rd)	{
				if	(!	rd->getIdentifier())	{
								return	false	;
				}
				//	Set	the	name	of	this	type.
				name	=	rd->getName().str()	;
				//	If	this	type	is	a	template	specialization	we	need	to	save	the	template	argument	types.
				if	(const	clang::ClassTemplateSpecializationDecl	*ctsd	=	clang::dyn_cast<clang::ClassTemplateSpecializationDecl>(rd))	{
								for	(const	clang::TemplateArgument&	ta	:	ctsd->getTemplateArgs().asArray())	{
												if	(	ta.getKind()	==	clang::TemplateArgument::Type	)	{
																//	Class	types	and	intrinsic	types
																if	(	!extractTemplateArgType(ta)	)	{
																				return	false	;
																}
												}	else	if	(	ta.getKind()	==	clang::TemplateArgument::Pack	)	{
																//	a	Pack	is	the	variables	in	a	variadric	template
																for	(const	clang::TemplateArgument&	pack_ta	:	ta.getPackAsArray())	{
																				if	(	pack_ta.getKind()	==	clang::TemplateArgument::Type	)	{
																								if	(	!extractTemplateArgType(pack_ta)	)	{
																												return	false	;
																								}
																				}
																}
												}	else	{
																//	This	template	argument	is	not	a	clang::TemplateArgument::Type.		We	don't	currently	handle	these.
																return	false	;
												}
								}
				}
				//	if	we	reach	the	end	then	we	can	deal	with	the	class	name,	return	true.
				return	true	;
}

static	void	mangle_template_param(std::string	&work_string,	const	std::string	&	delimiter)	{
				if	(	!delimiter.compare("__")	)	{
								std::replace(	work_string.begin(),	work_string.end(),	'*',	'_')	;
								std::replace(	work_string.begin(),	work_string.end(),	'[',	'_')	;
								std::replace(	work_string.begin(),	work_string.end(),	']',	'_')	;
								std::replace(	work_string.begin(),	work_string.end(),	':',	'_')	;
								work_string.erase(std::remove_if(work_string.begin(),	work_string.end(),
								(int(*)(int))std::isspace),	work_string.end())	;
				}
}

void	ConstructValues::ContainerClass::printTemplateList(std::ostream&	ostream,	const	std::string&	delimiter)	{
				if	(	template_args.size()	)	{
								std::string	obracket("<"),	comma(","),	cbracket(">"),	work_string	;
								if	(	!delimiter.compare("__")	)	{
													obracket	=	comma	=	cbracket	=	"_"	;
								}
								ostream	<<	obracket	;
								unsigned	int	arg_size	=	template_args.size()	-	1	;
								for	(unsigned	int	ii	=	0	;	ii	<	arg_size	;	ii++	)	{
												work_string	=	template_args[ii].name	;
												mangle_template_param(work_string,	delimiter)	;
												ostream	<<	work_string	;
												template_args[ii].printTemplateList(ostream,	delimiter);
												ostream	<<	comma	;
								}
								work_string	=	template_args[arg_size].name	;
								mangle_template_param(work_string,	delimiter)	;
								ostream	<<	work_string	;
								template_args[arg_size].printTemplateList(ostream,	delimiter);
								ostream	<<	cbracket	;
				}
}

void	ConstructValues::ContainerClass::printContainerClass(std::ostream&	ostream,	const	std::string&	delimiter)	{
				ostream	<<	name	;
				printTemplateList(ostream,	delimiter)	;
				ostream	<<	delimiter	;
}

//====================================================================================

ConstructValues::ConstructValues()	{}

void	ConstructValues::setName(std::string	in_name)	{
				name	=	in_name	;
}

std::string	ConstructValues::getName()	{
				return	name	;
}

void	ConstructValues::setFileName(std::string	in_name)	{
				file_name	=	in_name	;
}

std::string	ConstructValues::getFileName()	{
				return	file_name	;
}

void	ConstructValues::clearNamespacesAndClasses()	{
				namespaces.clear()	;
				container_classes.clear()	;
}

bool	ConstructValues::getNamespacesAndClasses(	const	clang::DeclContext	*	Ctx	)	{
				//	Save	container	namespaces	and	classes.
				typedef	clang::SmallVector<const	clang::DeclContext	*,	8>	ContextsTy;
				ContextsTy	Contexts;
				//	Collect	contexts.
				while	(Ctx	&&	clang::isa<clang::NamedDecl>(Ctx))	{
								Contexts.push_back(Ctx);
								Ctx	=	Ctx->getParent();
				};
				for	(ContextsTy::reverse_iterator	I	=	Contexts.rbegin(),	E	=	Contexts.rend();	I	!=	E;	++I)	{
								if	(const	clang::NamespaceDecl	*nd	=	clang::dyn_cast<clang::NamespaceDecl>(*I))	{
												if	(!	nd->isAnonymousNamespace())	{
																//std::cout	<<	"namespace	"	<<	nd->getIdentifier()->getName().str()	<<	std::endl	;
																std::string	temp_name	=	nd->getIdentifier()->getName().str()	;
																if	(	temp_name.compare("std")	and	temp_name.compare("__1"))	{
																				namespaces.push_back(nd->getIdentifier()->getName().str())	;
																}
												}
								}	else	if	(const	clang::RecordDecl	*rd	=	clang::dyn_cast<clang::RecordDecl>(*I))	{
												ContainerClass	cc	;
												if	(!cc.extractType(rd))	{
																return	false	;
												}
												container_classes.push_back(cc)	;
								}
				}
				return	true	;
}

std::string	ConstructValues::getFullyQualifiedName(const	std::string&	delimiter)	{
				std::ostringstream	oss;
				printNamespaces(oss,	delimiter);
				printContainerClasses(oss,	delimiter);
				oss	<<	name;
				return	oss.str();
}

bool	ConstructValues::isNameOrFileNameEmpty()	{
				return	name.empty()	or	file_name.empty();
}

void	ConstructValues::printOpenNamespaceBlocks(std::ostream&	ostream)	{
				for	(auto	name	:	namespaces)	{
								ostream	<<	"namespace	"	<<	name	<<	"	{"	<<	std::endl;
				}
}

void	ConstructValues::printCloseNamespaceBlocks(std::ostream&	ostream)	{
				for	(auto	name	:	namespaces)	{
								ostream	<<	"}"	<<	std::endl;
				}
}

void	ConstructValues::printNamespaces(std::ostream&	ostream,	const	std::string&	delimiter)	{
				for	(auto	name	:	namespaces)	{
								ostream	<<	name	<<	delimiter;
				}
}

void	ConstructValues::printContainerClasses(std::ostream&	ostream,	const	std::string&	delimiter)	{
				for	(auto	c	:	container_classes)	{
								c.printContainerClass(ostream,	delimiter);
				}
}

void	ConstructValues::printNamespacesAndContainerClasses(std::ostream&	ostream,	const	std::string&	delimiter)	{
				printNamespaces(ostream,	delimiter);
				printContainerClasses(ostream,	delimiter);
}

std::string	ConstructValues::getNamespacesAndContainerClasses(const	std::string&	delimiter)	{
				std::string	result	=	"";
				for	(auto	name	:	namespaces)	{
								result	+=	name	+	delimiter;
				}
				for	(auto	c	:	container_classes)	{
								std::stringstream	ss	;
								c.printContainerClass(ss,	delimiter);
								result	+=	ss.str()	;
				}
				return	result;
}

std::string	ConstructValues::getFullyQualifiedTypeName(const	std::string&	delimiter)	{
				std::ostringstream	oss	;
				printNamespacesAndContainerClasses(oss,	delimiter)	;
				oss	<<	name	;
				return	oss.str()	;
}

std::string	ConstructValues::getNameOnlyWithNamespaces(const	std::string&	delimiter)	{
				std::ostringstream	oss	;
				printNamespaces(oss,	delimiter)	;
				oss	<<	name	;
				return	oss.str()	;
}
