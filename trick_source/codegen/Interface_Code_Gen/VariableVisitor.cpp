#include	<iostream>
#include	<algorithm>

#include	"llvm/Support/CommandLine.h"
#include	"ClassValues.hh"
#include	"VariableVisitor.hh"
#include	"ClassVisitor.hh"
#include	"Utilities.hh"

extern	llvm::cl::opt<	int	>	debug_level	;

VariableVisitor::VariableVisitor(clang::CompilerInstance	&	in_ci	,	CommentSaver	&	in_cs	,
	HeaderSearchDirs	&	in_hsd	,	PrintAttributes	&	in_pa	,	std::set<	std::string	>	&	fdc	)	:
		ci(in_ci)	,
		cs(in_cs)	,
		hsd(in_hsd)	,
		pa(in_pa)	,
		fwd_declared_classes(fdc)	,
		has_dims(false)	,
		cval(NULL)	{}

bool	VariableVisitor::VisitDecl(clang::Decl	*d)	{
				if	(	debug_level	>=2	)	{
								std::cout	<<	"\nVariableVisitor	Decl	=	"	<<	d->getDeclKindName()	<<	std::endl	;
				}
				return	true	;
}

bool	VariableVisitor::VisitType(clang::Type	*t)	{
				if	(	debug_level	>=2	)	{
								std::cout	<<	"\nVariableVisitor	Type	=	"	<<	t->getTypeClassName()	<<	std::endl	;
				}
				return	true;
}

/**
We	are	only	interested	in	global	variable	declarations	that	are	template	class	types.
If	it	is	a	template	class	we	may	have	to	create	IO	code	for	the	class.

@details
*/

bool	VariableVisitor::VisitTemplateSpecializationType(clang::TemplateSpecializationType	*tst)	{

				clang::CXXRecordDecl	*trec	=	tst->getAsCXXRecordDecl()	;

				if	(	debug_level	>=2	)	{
								std::cout	<<	"\nVariableVisitor	VisitTemplateSpecializationType"	<<	std::endl	;
								trec->dump()	;	std::cout	<<	std::endl	;
				}

				/*	The	getDifinition()	call	retrieves	the	resolved	template	if	it	exists.		The	call
							returns	NULL	if	the	template	was	defined	but	never	used	*/
				clang::TagDecl	*	td	=	trec->getDefinition()	;

				if	(	td	!=	NULL	and	!	has_dims	)	{
								if	(	debug_level	>=2	)	{
												td->dump()	;	std::cout	<<	std::endl	;
								}
								CXXRecordVisitor	cvis(ci,	cs,	hsd,	pa,	true)	;
								cval	=	cvis.get_class_data()	;
								cvis.TraverseCXXRecordDecl(clang::cast<clang::CXXRecordDecl>(td))	;

								if	(cval->isInStandardNamespace())	{
												if	(	debug_level	>=4	)	{
																std::cout	<<	"\nTypedefVisitor	VisitTemplateSpecializationType	not	adding	class"	<<	std::endl	;
												}
												cval	=	NULL	;
								}	else	{
												std::string	mangled_name	=	tst->desugar().getAsString()	;
												size_t	pos	;
												if	((pos	=	mangled_name.find("class	"))	!=	std::string::npos	)	{
																mangled_name.erase(pos	,	6)	;
												}
												cval->setName(mangled_name)	;

												mangled_name.erase(remove_if(mangled_name.begin(),	mangled_name.end(),	isspace),	mangled_name.end());
												std::replace(	mangled_name.begin(),	mangled_name.end(),	'<',	'_')	;
												std::replace(	mangled_name.begin(),	mangled_name.end(),	'>',	'_')	;
												std::replace(	mangled_name.begin(),	mangled_name.end(),	'	',	'_')	;
												std::replace(	mangled_name.begin(),	mangled_name.end(),	',',	'_')	;
												std::replace(	mangled_name.begin(),	mangled_name.end(),	':',	'_')	;
												std::replace(	mangled_name.begin(),	mangled_name.end(),	'*',	'_')	;
												cval->setMangledTypeName(mangled_name)	;

												pa.printClass(cval)	;
								}
				}

				return	true;
}

