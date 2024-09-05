
#include	<iostream>
#include	<sstream>
#include	<algorithm>

#include	"llvm/Support/CommandLine.h"
#include	"EnumVisitor.hh"
#include	"EnumValues.hh"
#include	"Utilities.hh"
#include	"BraceMacro.hh"

extern	llvm::cl::opt<	int	>	debug_level	;

EnumVisitor::EnumVisitor(	clang::CompilerInstance	&	in_ci	,	HeaderSearchDirs	&	in_hsd	)	:
	ci(in_ci)	,	hsd(in_hsd)	{
				//eval	=	new	EnumValues()	;
}

bool	EnumVisitor::VisitDecl(clang::Decl	*d)	{
				//std::cout	<<	"\n[34mEnum	Decl	=	"	<<	d->getDeclKindName()	<<	"[00m"	<<	std::endl	;
				return	true	;
}

bool	EnumVisitor::VisitType(clang::Type	*t)	{
				//std::cout	<<	"\n[34mEnum	Type	=	"	<<	t->getTypeClassName()	<<	"[00m"	<<	std::endl	;
				return	true;
}

bool	EnumVisitor::VisitEnumDecl(clang::EnumDecl	*ed)	{
				eval.setFileName(getFileName(ci	,	ed->RBRACELOC(),	hsd))	;
				return	true;
}

bool	EnumVisitor::VisitEnumType(clang::EnumType	*et)	{

				clang::TagDecl	*	td	=	et->getDecl()	;
				eval.setName(td->getName().str())	;
				if	(	debug_level	>=	3	)	{
								std::cout	<<	"EnumVisitor	VisitEnumType	"	<<	std::endl	;
								std::cout	<<	"eval.getName()	=	"	<<	eval.getName()	<<	std::endl	;
								std::cout	<<	"td->getName()	=	"	<<	td->getName().str()	<<	std::endl	;
				}

				if	(	eval.getName().empty()	)	{
								std::string	enum_type_name	=	et->desugar().getAsString()	;
								size_t	pos	;
								//	If	this	enumeration	is	anonymous	return	an	error	condition
								if	((pos	=	enum_type_name.find("<anonymous"))	!=	std::string::npos	or
												//(pos	=	enum_type_name.find("(anonymous"))	!=	std::string::npos)	{
												(pos	=	enum_type_name.find("(anonymous"))	!=	std::string::npos	or
												(pos	=	enum_type_name.find("(unnamed"))	!=	std::string::npos)	{
												eval.setName("")	;
												return	false	;
								}
								if	((pos	=	enum_type_name.find("enum	"))	!=	std::string::npos	)	{
												enum_type_name.erase(pos	,	5)	;
								}

								//	Strip	off	containing	namespace/class	names.		They	will	be	added	back	later.
								if	((pos	=	enum_type_name.find_last_of(":"))	!=	std::string::npos	)	{
												eval.setName(enum_type_name.substr(pos	+	1))	;
								}	else	{
												eval.setName(enum_type_name)	;
								}
								//std::cout	<<	"\n[34mReplaced	Enum	name	=	"	<<	eval->getName()	<<	"[00m"	<<	std::endl	;
				}

				eval.getNamespacesAndClasses(td->getDeclContext())	;
				return	true	;
}

bool	EnumVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl	*ecd)	{
				//std::cout	<<	ecd->getName().str()	<<	"	=	"	<<	ecd->getInitVal().getSExtValue()	<<	std::endl	;
				eval.addEnum(ecd->getName().str()	,	ecd->getInitVal().getSExtValue())	;
				eval.addFullyQualifiedEnum(ecd->getQualifiedNameAsString(),	ecd->getInitVal().getSExtValue());
				return	true	;
}

EnumValues	*	EnumVisitor::get_enum_data()	{
				return	&eval	;
}


