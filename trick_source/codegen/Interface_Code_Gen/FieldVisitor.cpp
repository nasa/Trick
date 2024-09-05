
#include	<iostream>

#include	"llvm/Support/CommandLine.h"
#include	"clang/Basic/SourceManager.h"
#include	"clang/AST/RecordLayout.h"
#include	"clang/AST/ASTContext.h"
#include	"clang/AST/Comment.h"
#include	"clang/Sema/Sema.h"

#include	"FieldVisitor.hh"
#include	"FieldDescription.hh"
#include	"ClassValues.hh"
#include	"EnumValues.hh"
#include	"ClassVisitor.hh"
#include	"CommentSaver.hh"
#include	"Utilities.hh"

extern	llvm::cl::opt<	int	>	debug_level	;

FieldVisitor::FieldVisitor(clang::CompilerInstance	&	in_ci	,
	HeaderSearchDirs	&	in_hsd	,
	CommentSaver	&	in_cs	,
	PrintAttributes	&	in_pa	,
	std::string	container_class	)	:
		ci(in_ci)	,
		hsd(in_hsd)	,
		cs(in_cs)	,
		pa(in_pa)	{
				fdes	=	new	FieldDescription(container_class)	;
}

bool	FieldVisitor::VisitDecl(clang::Decl	*d)	{
				if	(	debug_level	>=	4	)	{
								std::cout	<<	"\n\033[32mFieldVisitor	VisitDecl	Decl	=	"	<<	d->getDeclKindName()	<<	"\033[00m"	<<	std::endl	;
								d->dump()	;
				}
				return	true	;
}

bool	FieldVisitor::VisitType(clang::Type	*t)	{
				if	(	debug_level	>=	4	)	{
								std::cout	<<	"FieldVisitor	VisitType	Type	=	"	<<	t->getTypeClassName()	<<	std::endl	;
								t->dump()	;
				}
				if	(	t->isReferenceType()	)	{
								if	(	debug_level	>=	3	)	{
												std::cout	<<	"FieldVisitor	VisitType	found	reference,	setIO	=	3	"	<<	std::endl	;
								}
								fdes->setIO(3)	;
								fdes->setReference(true)	;
				}
				return	true;
}

bool	FieldVisitor::VisitBuiltinType(clang::BuiltinType	*bt)	{

				if	(	debug_level	>=	3	)	{
								std::cout	<<	"FieldVisitor::VisitBuiltinType	"	<<	bt->desugar().getAsString()	<<	std::endl	;
				}
				fdes->setTypeName(bt->desugar().getAsString())	;
				if	(	fdes->isBitField()	)	{
								if	(	bt->isUnsignedInteger()	)	{
												fdes->setEnumString("TRICK_UNSIGNED_BITFIELD")	;
								}	else	{
												fdes->setEnumString("TRICK_BITFIELD")	;
								}
								if	(	bt->getKind()	==	clang::BuiltinType::Bool	)	{
												fdes->setTypeName("bool")	;
								}
				}	else	{
								switch	(	bt->getKind()	)	{
												case	clang::BuiltinType::Bool:
																fdes->setEnumString("TRICK_BOOLEAN")	;
																fdes->setTypeName("bool")	;
																break	;
												case	clang::BuiltinType::Char_S:
												case	clang::BuiltinType::SChar:
																fdes->setEnumString("TRICK_CHARACTER")	;
																break	;
												case	clang::BuiltinType::UChar:
												case	clang::BuiltinType::Char_U:
																fdes->setEnumString("TRICK_UNSIGNED_CHARACTER")	;
																break	;
												case	clang::BuiltinType::WChar_U:
												case	clang::BuiltinType::WChar_S:
																fdes->setEnumString("TRICK_WCHAR")	;
																break	;
												case	clang::BuiltinType::Short:
																fdes->setEnumString("TRICK_SHORT")	;
																break	;
												case	clang::BuiltinType::UShort:
												case	clang::BuiltinType::Char16:
																fdes->setEnumString("TRICK_UNSIGNED_SHORT")	;
																break	;
												case	clang::BuiltinType::Int:
																fdes->setEnumString("TRICK_INTEGER")	;
																break	;
												case	clang::BuiltinType::UInt:
																fdes->setEnumString("TRICK_UNSIGNED_INTEGER")	;
																break	;
												case	clang::BuiltinType::Long:
																fdes->setEnumString("TRICK_LONG")	;
																break	;
												case	clang::BuiltinType::ULong:
																fdes->setEnumString("TRICK_UNSIGNED_LONG")	;
																break	;
												case	clang::BuiltinType::LongLong:
																fdes->setEnumString("TRICK_LONG_LONG")	;
																break	;
												case	clang::BuiltinType::ULongLong:
																fdes->setEnumString("TRICK_UNSIGNED_LONG_LONG")	;
																break	;
												case	clang::BuiltinType::Float:
																fdes->setEnumString("TRICK_FLOAT")	;
																break	;
												case	clang::BuiltinType::Double:
																fdes->setEnumString("TRICK_DOUBLE")	;
																break	;
												default:
																fdes->setEnumString("TRICK_VOID")	;
																break	;
								}
				}
				return	false;
}

bool	FieldVisitor::VisitConstantArrayType(clang::ConstantArrayType	*cat)	{
				//cat->dump()	;	std::cout	<<	std::endl	;
				fdes->addArrayDim(cat->getSize().getZExtValue())	;
				//	If	this	field	is	an	arrayed	STL,	skip	it!
				if	(	fdes->isSTL()	)	{
								fdes->setIO(0)	;
				}
				return	true;
}

/*	Both	FieldDecl	and	VarDecl	derive	from	DeclaratorDecl.		We	can	do
			common	things	to	both	node	types	in	this	function	*/
bool	FieldVisitor::VisitDeclaratorDecl(	clang::DeclaratorDecl	*dd	)	{

				fdes->setFileName(getFileName(ci	,	dd->getLocation(),	hsd))	;
				fdes->setName(dd->getNameAsString())	;
				fdes->setAccess(dd->getAccess())	;

				/*	Get	the	source	location	of	this	field.*/
				clang::SourceRange	dd_range	=	dd->getSourceRange()	;
				clang::PresumedLoc	PLoc	=	ci.getSourceManager().getPresumedLoc(dd_range.getEnd());
				std::string	file_name	;
				if	(!PLoc.isInvalid())	{
								char	*	resolved_path	=	almostRealPath(PLoc.getFilename())	;
								if	(	resolved_path	!=	NULL	)	{
												file_name	=	std::string(resolved_path)	;
												free(resolved_path)	;
								}
				}

				if	(	!	file_name.empty()	)	{
								if	(	isInUserOrTrickCode(	ci	,	dd_range.getEnd()	,	hsd	)	)	{
												fdes->setLineNo(ci.getSourceManager().getSpellingLineNumber(dd_range.getEnd()))	;
												/*	process	comment	if	neither	ICG:(No)	or	ICG:(NoComment)	is	present	*/
												if	(		cs.hasTrickHeader(file_name)	and
																	!cs.hasICGNoComment(file_name)	and
																	!hsd.isPathInICGNoComment(file_name)	)	{
																/*	Get	the	possible	comment	on	this	line	and	parse	it	*/
																fdes->parseComment(cs.getComment(file_name	,	fdes->getLineNo()))	;
												}
								}
				}

				if	(	debug_level	>=	3	)	{
								if	(	!	ci.getSourceManager().isInSystemHeader(dd_range.getEnd())	)	{
												std::cout	<<	"FieldVisitor	VisitDeclaratorDecl"	<<	std::endl	;
												std::cout	<<	"				file_name	=	"	<<	file_name	<<	std::endl	;
												std::cout	<<	"				line	num	=	"	<<	fdes->getLineNo()	<<	std::endl	;
												std::cout	<<	"				comment	=	"	<<	cs.getComment(file_name	,	fdes->getLineNo())	<<	std::endl	;
												std::cout	<<	"				public/private	=	"	<<	fdes->getAccess()	<<	std::endl	;
												std::cout	<<	"				io	=	"	<<	fdes->getIO()	<<	std::endl	;
								}
				}

				//	returns	true	if	any	io	is	allowed.	returning	false	will	stop	processing	of	this	variable	here.
				return	fdes->getIO()	;
}

bool	FieldVisitor::VisitEnumType(	clang::EnumType	*et	)	{

				std::string	enum_type_name	=	et->desugar().getAsString()	;
				if	(	debug_level	>=	3	)	{
								std::cout	<<	"\nFieldVisitor	VisitEnumType"	<<	std::endl	;
								std::cout	<<	et->desugar().getAsString()	<<	std::endl	;
				}
				size_t	pos	;
				if	((pos	=	enum_type_name.find("enum	"))	!=	std::string::npos	)	{
								enum_type_name.erase(pos	,	5)	;
				}
				//	If	this	enum	is	to	an	enumeration	found	inside	a	template,	e.g.	template<type>::enum_type	ignore	it.
				//	because	there	will	not	be	enumeration	attribute	information	generated	for	this	enum.
				if	((pos	=	enum_type_name.find("<"))	!=	std::string::npos	)	{
								size_t	last_pos	=	enum_type_name.find_last_of(">::")	;
								enum_type_name.replace(pos,	last_pos	-	pos	+	1,	"__")	;
								//fdes->setIO(0)	;
				}
				fdes->setMangledTypeName("")	;
				fdes->setTypeName(enum_type_name)	;
				fdes->setEnumString("TRICK_ENUMERATED")	;
				fdes->setEnum(true)	;
				return	true	;
}

bool	FieldVisitor::VisitFieldDecl(	clang::FieldDecl	*field	)	{

				clang::QualType	qt	=	field->getType()	;

				//	set	the	offset	and	size	field
				fdes->setFieldOffset(field->getASTContext().getFieldOffset(field))	;
				fdes->setFieldWidth(field->getASTContext().getTypeSize(qt))	;

				if	(	field->isBitField())	{
								fdes->setBitField(true)	;
								fdes->setBitFieldWidth(field->getBitWidthValue(field->getASTContext()))	;
								unsigned	int	field_offset_bits	=	field->getASTContext().getFieldOffset(field)	+	fdes->getBaseClassOffset()	*	8	;
								fdes->setBitFieldStart(	32	-	(field_offset_bits	%	32)	-	fdes->getBitFieldWidth())	;
								fdes->setBitFieldByteOffset((field_offset_bits	/	32)	*	4	)	;
				}

				if	(	debug_level	>=	3	)	{
								std::cout	<<	"FieldVisitor	VisitFieldDecl"	<<	std::endl	;
								std::cout	<<	"				is_bitfield	=	"	<<	fdes->isBitField()	<<	std::endl	;
								std::cout	<<	"				is_canonical	=	"	<<	qt.isCanonical()	<<	std::endl	;
								//	isHidden()	removed	in	clang	11.0
								//std::cout	<<	"				is_hidden	=	"	<<	field->isHidden()	<<	std::endl	;
								//field->dump()	;
				}

				//	If	the	current	type	is	not	canonical	because	of	typedefs	or	template	parameter	substitution,
				//	traverse	the	canonical	type
				if	(	!qt.isCanonical()	)	{
								fdes->setNonCanonicalTypeName(qt.getAsString())	;
								clang::QualType	ct	=	qt.getCanonicalType()	;
								if	(	debug_level	>=	3	)	{
												std::cout	<<	"\033[33mFieldVisitor	VisitFieldDecl:	Processing	canonical	type\033[00m"	<<	std::endl	;
												ct.dump()	;
								}
								TraverseType(ct)	;
								//	We	have	extracted	the	canonical	type	and	everything	else	we	need
								//	return	false	so	we	cut	off	processing	of	this	AST	branch
								return	false	;
				}

				return	true	;
}

bool	FieldVisitor::VisitPointerType(clang::PointerType	*p)	{
				fdes->addArrayDim(-1)	;
				//	If	this	field	is	a	pointer	to	an	STL,	skip	it!
				if	(	fdes->isSTL()	)	{
								fdes->setIO(0)	;
				}
				return	true;
}

std::map	<	std::string	,	std::string	>	FieldVisitor::processed_templates	;

bool	FieldVisitor::ProcessTemplate(std::string	in_name	,	clang::CXXRecordDecl	*	crd	)	{

				//	Save	container	namespaces	and	classes.
				//	If	we	have	trouble	getting	the	namespaces	and	classes	immediately	return.
				if	(	!fdes->getNamespacesAndClasses(crd->getDeclContext()))	{
								fdes->setIO(0)	;
								return	false	;
				}

				//	Check	to	see	if	we've	processed	this	template	before
				//	If	not	we	need	to	create	attributes	for	this	template
				if	(	processed_templates.find(in_name)	==	processed_templates.end()	)	{
								std::string	mangled_name	=	sanitize(in_name)	;

								//	save	off	the	mangled	name	of	this	template	to	be	used	if	another	variable	is	the	same	template	type
								processed_templates[in_name]	=	fdes->getContainerClass()	+	"_"	+
									fdes->getName()	+	"_"	+	mangled_name	;

								//	Traverse	the	template	declaration
								CXXRecordVisitor	template_spec_cvis(ci	,	cs,	hsd	,	pa,	true)	;
								template_spec_cvis.get_class_data()->setMangledTypeName(processed_templates[in_name])	;
								template_spec_cvis.TraverseCXXRecordDecl(crd)	;

								//	Set	the	actual	type	name	and	file	name.	Print	the	attributes	for	this	template	type
								template_spec_cvis.get_class_data()->setName(in_name)	;
								template_spec_cvis.get_class_data()->setFileName(fdes->getFileName())	;
								pa.printClass(template_spec_cvis.get_class_data())	;

								if	(	debug_level	>=	4	)	{
												std::cout	<<	"Added	template	class	from	FieldVisitor	ProcessTemplate	"	;
												std::cout	<<	in_name	<<	std::endl	;
												std::cout	<<	*fdes	<<	std::endl	;
								}
				}

				fdes->setMangledTypeName(processed_templates[in_name])	;
				fdes->setEnumString("TRICK_STRUCTURED")	;
				fdes->setRecord(true)	;

				//	processing	the	template	will	process	the	type,	return	false	to	stop	processing
				return	false	;
}

static	std::map<std::string,	bool>	init_stl_classes()	{
				std::map<std::string,	bool>	my_map	;
				my_map.insert(std::pair<std::string,	bool>("std::array",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::deque",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::list",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::map",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::multiset",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::multimap",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::pair",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::priority_queue",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::queue",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::set",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::stack",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::vector",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::array",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::deque",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::list",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::map",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::multiset",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::multimap",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::pair",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::priority_queue",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::queue",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::set",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::stack",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__1::vector",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::array",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::deque",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::list",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::map",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::multiset",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::multimap",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::pair",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::priority_queue",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::queue",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::set",	1))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::stack",	0))	;
				my_map.insert(std::pair<std::string,	bool>("std::__cxx11::vector",	1))	;
				return	my_map	;
}

static	bool	checkForPrivateTemplateArgs(	clang::ClassTemplateSpecializationDecl	*	ctsd	)	{
				for	(const	clang::TemplateArgument&	ta	:	ctsd->getTemplateArgs().asArray())	{
								if	(	ta.getKind()	==	clang::TemplateArgument::Type	)	{
												clang::QualType	qt	=	ta.getAsType()	;
												//std::cout	<<	qt.getAsString()	<<	std::endl	;
												if	(	CXXRecordVisitor::isPrivateEmbeddedClass(qt.getAsString())	)	{
																//std::cout	<<	"		is	private	embedded	class"	<<	std::endl	;
																return	true	;
												}	else	{
																//std::cout	<<	"		is	public	embedded	class"	<<	std::endl	;
																const	clang::Type	*	t	=	qt.getTypePtrOrNull()	;
																if	(	t	!=	NULL	)	{
																				if	(t->getTypeClass()	==	clang::Type::Record	)	{
																								clang::CXXRecordDecl	*	crd	=	t->getAsCXXRecordDecl()	;
																								if	(	clang::isa<clang::ClassTemplateSpecializationDecl>(crd)	)	{
																												return	checkForPrivateTemplateArgs(clang::cast<clang::ClassTemplateSpecializationDecl>(crd))	;
																								}
																				}
																}
												}
								}
				}
				return	false	;
}

static	bool	checkForConstTemplateArgs(	clang::ClassTemplateSpecializationDecl	*	ctsd	)	{
				for	(const	clang::TemplateArgument&	ta	:	ctsd->getTemplateArgs().asArray())	{
								if	(	ta.getKind()	==	clang::TemplateArgument::Type	)	{
												clang::QualType	qt	=	ta.getAsType()	;
												//std::cout	<<	qt.getAsString()	<<	std::endl	;
												if	(	qt.isConstQualified()	)	{
																//std::cout	<<	"		is	const	qualified"	<<	std::endl	;
																return	true	;
												}	else	{
																//std::cout	<<	"		is	public	embedded	class"	<<	std::endl	;
																const	clang::Type	*	t	=	qt.getTypePtrOrNull()	;
																if	(	t	!=	NULL	)	{
																				if	(t->getTypeClass()	==	clang::Type::Record	)	{
																								clang::CXXRecordDecl	*	crd	=	t->getAsCXXRecordDecl()	;
																								if	(	clang::isa<clang::ClassTemplateSpecializationDecl>(crd)	)	{
																												return	checkForConstTemplateArgs(clang::cast<clang::ClassTemplateSpecializationDecl>(crd))	;
																								}
																				}
																}
												}
								}
				}
				return	false	;
}

static	std::map<std::string,	bool>	stl_classes	=	init_stl_classes()	;

bool	FieldVisitor::VisitRecordType(clang::RecordType	*rt)	{
				if	(	debug_level	>=	3	)	{
								std::cout	<<	"FieldVisitor	VisitRecordType"	<<	std::endl	;
								std::cout	<<	rt->getDecl()->getQualifiedNameAsString()	<<	std::endl	;
								rt->dump()	;
				}
				/*	String	types	are	typed	as	records	but	we	treat	them	differently.
							The	attributes	type	is	set	to	TRICK_STRING	instead	of	TRICK_STRUCTURE.
							The	type	is	set	to	std::string.		We	can	return	false	here	to	stop	processing	of	this	type.	*/
				std::string	type_name	=	rt->getDecl()->getQualifiedNameAsString()	;
				if	(	!	type_name.compare("std::basic_string")	||	!type_name.compare("std::__1::basic_string")	||
									!	type_name.compare("std::__cxx11::basic_string")	)	{
								fdes->setEnumString("TRICK_STRING")	;
								fdes->setTypeName("std::string")	;
								return	false	;
				}
				//	FILE	*	types	resolve	to	these	typenames.		We	need	to	ignore	them
				if	(!type_name.compare("__sFILE")	||
								!type_name.compare("_IO_FILE")	||
								!type_name.compare("__gnu_cxx::__normal_iterator"))	{
								fdes->setIO(0)	;
								return	false	;
				}

				std::string	tst_string	=	rt->desugar().getAsString()	;
				//	remove	class	keyword	if	it	exists
				size_t	pos	;
				while	((pos	=	tst_string.find("class	"))	!=	std::string::npos	)	{
								tst_string.erase(pos	,	6)	;
				}
				while	((pos	=	tst_string.find("struct	"))	!=	std::string::npos	)	{
								tst_string.erase(pos	,	7)	;
				}
				//	clang	changes	bool	to	_Bool.		We	need	to	change	it	back
				while	((pos	=	tst_string.find("<_Bool"))	!=	std::string::npos	)	{
								tst_string.replace(pos	,	6,	"<bool")	;
				}
				while	((pos	=	tst_string.find("	_Bool"))	!=	std::string::npos	)	{
								tst_string.replace(pos	,	6,	"	bool")	;
				}

				//	Test	if	we	have	some	type	from	STL.
				if	(!tst_string.compare(	0	,	5	,	"std::"))	{
								//	If	we	have	some	type	from	std,	figure	out	if	it	is	one	we	support.
								for	(	std::map<std::string,	bool>::iterator	it	=	stl_classes.begin()	;	it	!=	stl_classes.end()	;	++it	)	{
												/*	Mark	STL	types	that	are	not	strings	and	exit	*/
												if	(!tst_string.compare(	0	,	(*it).first.size()	,	(*it).first))	{

																clang::RecordDecl	*	rd	=	rt->getDecl()->getDefinition()	;
																if	(	rd	!=	NULL	and	clang::ClassTemplateSpecializationDecl::classof(rd)	)	{
																				clang::ClassTemplateSpecializationDecl	*	ctsd	;
																				ctsd	=	clang::cast<clang::ClassTemplateSpecializationDecl>(rd)	;

																				//	If	a	private	embedded	class	is	in	an	STL	the	resulting	io_src	code	will	not	compile.
																				//	Search	the	template	arguments	for	private	embedded	classes,	if	found	remove	io	capabilites.
																				if	(	checkForPrivateTemplateArgs(	ctsd	))	{
																								fdes->setIO(0)	;
																				}

																				//	If	the	template	is	using	a	const	type	the	STL	checkpoint	code	will	not	compile,
																				//	we	need	to	ignore	the	variable.
																				if	(	checkForConstTemplateArgs(	ctsd	))	{
																								fdes->setIO(0)	;
																				}

																				fdes->setEnumString("TRICK_STL")	;
																				fdes->setSTL(true)	;
																				fdes->setTypeName(tst_string)	;
																				fdes->setSTLClear((*it).second)	;
																				//	set	the	type	name	to	the	non	canonical	name,	the	name	the	user	put	in	the	header	file
																				//	The	typename	is	not	used	by	STL	variables,	and	it	is	nice	to	see	the	type	that	was
																				//	actually	inputted	by	the	user
																				fdes->setMangledTypeName(fdes->getNonCanonicalTypeName())	;
																				return	false	;
																}
												}
								}
								//	If	the	record	type	is	in	std::	but	not	one	we	can	process,	set	the	I/O	spec	to	zero	and	return.
								fdes->setIO(0)	;
								return	false	;
				}

				//	If	the	type	is	a	private	embedded	class	there	will	not	be	any	io_src	code	for	the	type.		Don't	create	attributes
				if	(	CXXRecordVisitor::isPrivateEmbeddedClass(tst_string)	)	{
								//std::cout	<<	"Type	is	a	private	embedded	class!"	<<	std::endl	;
								fdes->setIO(0)	;
								return	false	;
				}
				/*	Template	specialization	types	will	be	processed	here	because	the	canonical	type
							will	be	typed	as	a	record.		We	test	if	we	have	a	template	specialization	type.
							If	so	process	the	template	type	and	return	*/
				clang::RecordDecl	*	rd	=	rt->getDecl()->getDefinition()	;
				if	(	rd	!=	NULL	)	{
								if	(	clang::ClassTemplateSpecializationDecl::classof(rd)	)	{
												if	(	checkForPrivateTemplateArgs(	clang::cast<clang::ClassTemplateSpecializationDecl>(rd))	)	{
																fdes->setIO(0)	;
																if	(	debug_level	>=	3	)	{
																				std::cout	<<	"				template	using	private/protected	class	as	argument,	not	processing"	<<	std::endl	;
																}
																return	false	;
												}
												if	(	debug_level	>=	3	)	{
																rd->dump()	;
																std::cout	<<	"				tst_string	=	"	<<	tst_string	<<	std::endl	;
																std::cout	<<	"				is_a_template_specialization"	<<	std::endl	;
												}
												return	ProcessTemplate(tst_string,	clang::cast<clang::CXXRecordDecl>(rd))	;
								}	else	if	(tst_string.find(">::")	!=	std::string::npos)	{
												/*	Hacky	check	to	see	if	we	are	using	an	embedded	class	within	a	template	definition.
														template	<class	T>	class	A	{
																public:	class	B	{	T	t	;}	;
														};

														class	C	{
																public:	A<int>::B	ab	;				//	This	is	the	pattern	we	are	looking	for.
														}	;
															There	must	be	a	better	way	to	determine	this	condition
															We	need	to	make	attributes	for	th	A<int>::B	class.
													*/
												return	ProcessTemplate(tst_string,	clang::cast<clang::CXXRecordDecl>(rd))	;
								}
				}

				/*	Test	to	see	if	we	have	an	embedded	anonymous	struct/union.		e.g.	SB	is	anonymous	below.
							struct	SA	{
										struct	{
													double	d	;
										}	SB	;
							}	;
				*/
				//std::cout	<<	"hasNameForLinkage	"	<<	rt->getDecl()->hasNameForLinkage()	<<	std::endl	;
				if	(	rt->getDecl()->hasNameForLinkage()	)	{
								if	(	rt->getDecl()->getDeclName()	)	{
												//std::cout	<<	"getDeclName	"	<<	type_name	<<	std::endl	;
												fdes->setTypeName(type_name)	;
								}	else	{
												//std::cout	<<	"getTypedefNameForAnonDecl	"	<<	rt->getDecl()->getTypedefNameForAnonDecl()	<<	std::endl	;
												fdes->setTypeName(rt->getDecl()->getTypedefNameForAnonDecl()->getQualifiedNameAsString())	;
								}
				}	else	{
								//	io_src	code	not	possible	for	anonymous	struct/unions.		Set	the	I/O	to	0	to	ignore	it.
								if	(	debug_level	>=	3	)	{
												std::cout	<<	"FieldVisitor	VisitRecordType	found	anonymous	type,	setIO	=	0"	<<	std::endl	;
								}
								fdes->setIO(0)	;
				}

				fdes->setEnumString("TRICK_STRUCTURED")	;
				fdes->setRecord(true)	;
				//	We	have	our	type,	return	false	to	stop	processing	this	AST	branch
				return	false;
}

bool	FieldVisitor::VisitVarDecl(	clang::VarDecl	*v	)	{

				fdes->setStatic(v->isStaticDataMember())	;
				/*	If	we	have	a	static	const	integer	type	with	an	initializer	value,	this	variable	will
							not	be	instantiated	by	the	compiler.	The	compiler	substitutes	in	the	value	internally.
							set	the	IO	to	0	to	stop	attribute	printing	*/
				//	Note:	gcc	allows	an	initializer	for	floating	point	types	too.
				if	(	v->isStaticDataMember()	and
									v->getType().isConstQualified()	and
									v->hasInit()	)	{
								fdes->setIO(0)	;
								return	false	;
				}	else	if	(	v->isStaticDataMember()	and
									v->getType().isConstQualified()	)	{
								/*	Static	const	members	cannot	be	set	through	attributes	code.	Remove	input
											capabilities	by	taking	current	io	specification	&	1	*/
								fdes->setIO(fdes->getIO()	&	1)	;
				}
				if	(	debug_level	>=	3	)	{
								std::cout	<<	"FieldVisitor	VisitVarDecl	"	<<	fdes->getName()	<<	std::endl	;
								std::cout	<<	"				is	static	=	"	<<	fdes->isStatic()	<<	std::endl	;
								std::cout	<<	"				is	const	=	"	<<	v->getType().isConstQualified()	<<	std::endl	;
								std::cout	<<	"				has	initializer	value	=	"	<<	v->hasInit()	<<	std::endl	;
								std::cout	<<	"				IO	=	"	<<	fdes->getIO()	<<	std::endl	;
								//v->dump()	;	std::cout	<<	std::endl	;
				}

				clang::QualType	qt	=	v->getType()	;
				//	If	the	current	type	is	not	canonical	because	of	typedefs	or	template	parameter	substitution,
				//	traverse	the	canonical	type
				if	(	!qt.isCanonical()	)	{
								fdes->setNonCanonicalTypeName(qt.getAsString())	;
								clang::QualType	ct	=	qt.getCanonicalType()	;
								std::string	tst_string	=	ct.getAsString()	;
								if	(	debug_level	>=	3	)	{
												std::cout	<<	"\033[33mFieldVisitor	VisitVarDecl:	Processing	canonical	type	"	<<	tst_string	<<	"\033[00m"	<<	std::endl	;
												ct.dump()	;
								}
								TraverseType(ct)	;
								//	We	have	extracted	the	canonical	type	and	everything	else	we	need
								//	return	false	so	we	cut	off	processing	of	this	AST	branch
								return	false	;
				}

				return	true	;
}

FieldDescription	*	FieldVisitor::get_field_data()	{
				return	fdes	;
}

