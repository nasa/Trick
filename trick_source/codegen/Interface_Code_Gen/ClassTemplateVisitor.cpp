
#include	<iostream>
#include	<sstream>

#include	"llvm/Support/CommandLine.h"
#include	"clang/Basic/SourceManager.h"
#include	"clang/Basic/FileManager.h"
#include	"clang/AST/RecordLayout.h"
#include	"clang/AST/ASTContext.h"

#include	"ClassTemplateVisitor.hh"
#include	"ClassVisitor.hh"
#include	"EnumVisitor.hh"
#include	"EnumValues.hh"
#include	"PrintAttributes.hh"
#include	"Utilities.hh"

extern	llvm::cl::opt<	int	>	debug_level	;

ClassTemplateVisitor::ClassTemplateVisitor(
	clang::CompilerInstance	&	in_ci	,
	CommentSaver	&	in_cs	,
	HeaderSearchDirs	&	in_hsd,
	PrintAttributes	&	in_pa	)	:
		ci(in_ci)	,
		cs(in_cs)	,
		hsd(in_hsd),
		pa(in_pa)
{}

/*	We	want	to	process	enumerations	out	of	template	definitions	*/

bool	ClassTemplateVisitor::TraverseDecl(clang::Decl	*d)	{
				if	(!d)
								return	true;
				if	(d->isImplicit())
								return	true;

				if	(	debug_level	>=2	)	{
								std::cout	<<	"ClassTemplateVisitor	TraverseDecl	"	<<	d->getDeclKindName()	<<	std::endl	;
				}

				switch	(	d->getKind()	)	{
								case	clang::Decl::CXXRecord	:	{
												TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl	*>(d))	;

												clang::CXXRecordDecl	*	crd	=	static_cast<clang::CXXRecordDecl	*>(d)	;
												/*	The	definition	of	the	record	must	exist	before	we	can	process	it.		The	definition	is
															NULL	when	this	is	only	a	forward	declaration	of	a	class.		We	also	only	want	to
															process	embedded	classes	that	have	public	access.	*/
												clang::RecordDecl	*	rd	=	crd->getDefinition()	;
												if	(	rd	!=	NULL	)	{
																//d->dump()	;
																//std::cout	<<	"access	=	"	<<	rd->getAccess()	<<	std::endl	<<	std::endl	;
																if	(	rd->getAccess()	==	clang::AS_protected	||		rd->getAccess()	==	clang::AS_private	)	{
																				//	protected	and	private	embedded	classes	cannot	be	used	outside	of	their	class
																				//	in	our	auto-generated	code.		Keep	a	set	of	all	classes	of	this	type	so	we	can
																				//	test	against	them.
																				ClassValues	temp_cv	;
																				temp_cv.getNamespacesAndClasses(crd->getDeclContext())	;
																				//std::cout	<<	"marking	private	"	<<	temp_cv.getFullyQualifiedName()	+	crd->getNameAsString()	<<	std::endl	;
																				CXXRecordVisitor::addPrivateEmbeddedClass(temp_cv.getFullyQualifiedName()	+	crd->getNameAsString())	;
																}	else	{
																				TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl	*>(d))	;
																}
												}
								}
								break	;
								case	clang::Decl::Enum	:	{
												clang::EnumDecl	*	ed	=	static_cast<clang::EnumDecl	*>(d)	;
												/*	This	is	an	embedded	enumeration	within	a	class	template.		Only	process	it	if	is	public	*/
												if	(	ed->getAccess()	==	clang::AS_public	)	{
																if	(	debug_level	>=2	)	{
																				ed->dump()	;	std::cout	<<	std::endl	;
																}
																EnumVisitor	evis(ci,	hsd)	;
																evis.TraverseDecl(ed)	;
																evis.get_enum_data()->setHasDefinition(false)	;
																pa.printEnum(evis.get_enum_data())	;
												}

								}
								break	;
								default	:
								break	;
				}
				return	true	;
}

bool	ClassTemplateVisitor::VisitDecl(clang::Decl	*d)	{
				if	(	debug_level	>=2	)	{
								std::cout	<<	"\nClassTemplateVisitor	Decl	=	"	<<	d->getDeclKindName()	<<	std::endl	;
				}
				return	true	;
}

bool	ClassTemplateVisitor::VisitType(clang::Type	*t)	{
				if	(	debug_level	>=2	)	{
								std::cout	<<	"\nClassTemplateVisitor	Type	=	"	<<	t->getTypeClassName()	<<	std::endl	;
				}
				return	true;
}

