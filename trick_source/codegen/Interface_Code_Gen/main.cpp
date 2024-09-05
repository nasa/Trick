#include	<iostream>
#include	<sstream>
#include	<libgen.h>
#include	<string.h>
#include	<stdio.h>
#include	<sys/stat.h>
#include	<unistd.h>

//	`llvm/Support/Host.h`	is	deprecated	in	favour	of	`llvm/TargetParser/Host.h`	since	clang	17
#if	LIBCLANG_MAJOR	>	16
#include	"llvm/TargetParser/Host.h"
#else
#include	"llvm/Support/Host.h"
#endif
#include	"llvm/Support/CommandLine.h"
#include	"llvm/Support/raw_ostream.h"

#include	"clang/Basic/Builtins.h"
#include	"clang/Frontend/CompilerInstance.h"
#include	"clang/Basic/TargetOptions.h"
#include	"clang/Basic/TargetInfo.h"
#include	"clang/Basic/FileManager.h"
#include	"clang/Lex/Preprocessor.h"
#include	"clang/Lex/PreprocessorOptions.h"
#include	"clang/Basic/Diagnostic.h"
#include	"clang/Parse/ParseAST.h"

#include	"ICGDiagnosticConsumer.hh"
#include	"ICGASTConsumer.hh"
#include	"HeaderSearchDirs.hh"
#include	"CommentSaver.hh"
#include	"TranslationUnitVisitor.hh"
#include	"PrintAttributes.hh"
#include	"Utilities.hh"
#include	"FindTrickICG.hh"

/*	Command	line	arguments.		These	work	better	as	globals,	as	suggested	in	llvm/CommandLine	documentation	*/
llvm::cl::list<std::string>	include_dirs("I",	llvm::cl::Prefix,	llvm::cl::desc("Include	directory"),	llvm::cl::value_desc("directory"));
llvm::cl::list<std::string>	f_options("f",	llvm::cl::Prefix,	llvm::cl::desc("Compiler	options	that	have	f	prefix"),	llvm::cl::value_desc("option"));
llvm::cl::list<std::string>	isystem_dirs("isystem",	llvm::cl::Prefix,	llvm::cl::desc("Include	directory,	suppress	all	warnings"),	llvm::cl::value_desc("directory"));
llvm::cl::list<std::string>	defines("D",	llvm::cl::Prefix,	llvm::cl::desc("Defines"),	llvm::cl::value_desc("define"));
//	TODO:	remove	units_truth_is_scary	in	2021.
llvm::cl::opt<bool>	units_truth_is_scary("units-truth-is-scary",	llvm::cl::desc("DEPRECATED:	Don't	print	units	conversion	messages"));
llvm::cl::opt<bool>	sim_services_flag("sim_services",	llvm::cl::desc("Gernerate	io_src	for	Trick	core	headers"));
llvm::cl::opt<bool>	force("force",	llvm::cl::desc("Force	all	io_src	files	to	be	generated"));
llvm::cl::opt<int>	attr_version("v",	llvm::cl::desc("Select	version	of	attributes	to	produce.		10	and	13	are	valid"),	llvm::cl::init(10));
llvm::cl::opt<std::string>	standard_version("icg-std",	llvm::cl::desc("Set	the	C++	standard	to	use	when	parsing.	c++11,	c++14,	c++17,	and	c++20	are	valid.	Default	is	c++17	or	the	newest	supported	by	your	LLVM	version."),	llvm::cl::init(""),	llvm::cl::ZeroOrMore);
llvm::cl::opt<int>	debug_level("d",	llvm::cl::desc("Set	debug	level"),	llvm::cl::init(0),	llvm::cl::ZeroOrMore);
llvm::cl::opt<bool>	create_map("m",	llvm::cl::desc("Create	map	files"),	llvm::cl::init(false));
llvm::cl::opt<std::string>	output_dir("o",	llvm::cl::desc("Output	directory"));
llvm::cl::list<std::string>	input_file_names(llvm::cl::Positional,	llvm::cl::desc("<input_file>"),	llvm::cl::ZeroOrMore);
llvm::cl::list<std::string>	sink(llvm::cl::Sink,	llvm::cl::ZeroOrMore);
llvm::cl::list<std::string>	pre_compiled_headers("include",	llvm::cl::Prefix,	llvm::cl::desc("pre-compiled	headers"),	llvm::cl::value_desc("pre_compiled_headers"));

llvm::cl::opt<bool>	global_compat15("c",	llvm::cl::desc("Print	the	offsetof	calculations	in	attributes"))	;
llvm::cl::opt<llvm::cl::boolOrDefault>	print_trick_icg("print-TRICK-ICG",	llvm::cl::desc("Print	warnings	where	TRICK_ICG	may	cause	io_src	inconsistencies"))	;
llvm::cl::alias	compat15_alias	("compat15"	,	llvm::cl::desc("Alias	for	-c")	,	llvm::cl::aliasopt(global_compat15))	;
llvm::cl::opt<bool>	m32("m32",	llvm::cl::desc("Generate	io	code	for	use	with	32bit	mode"),	llvm::cl::init(false),	llvm::cl::ZeroOrMore)	;


void	set_lang_opts(clang::CompilerInstance	&	ci)	{
				ci.getLangOpts().CXXExceptions	=	true	;
				//	Activate	C++11	parsing
				ci.getLangOpts().Bool	=	true	;
				ci.getLangOpts().WChar	=	true	;
				ci.getLangOpts().CPlusPlus	=	true	;
				ci.getLangOpts().CXXOperatorNames	=	true	;

				//	Always	use	at	least	C++11
				ci.getLangOpts().CPlusPlus11	=	true	;


#if	(LIBCLANG_MAJOR	<	6)
				//	Check	if	standard_version	was	specified	and	if	it's	a	version	that	is	supported	by	this	libclang
				if	(standard_version	!=	"")	{
								if	(standard_version	==	"c++11")	{
												//	Nothing	to	be	done	here	really
								}	else	if	(standard_version	==	"c++14"	||	standard_version	==	"c++17"	||	standard_version	==	"c++20")	{
												std::cerr	<<	"C++	standard	"	<<	standard_version	<<	"	is	not	supported	by	this	version	of	Clang."	<<	std::endl;
								}	else	{
												std::cerr	<<	"Invalid	C++	standard	version	specified:"	<<	standard_version	<<	std::endl;
								}
				}
#endif

				//	Activate	C++14	parsing
#if	(LIBCLANG_MAJOR	>=	6)
				ci.getLangOpts().CPlusPlus14	=	true	;
				ci.getLangOpts().DoubleSquareBracketAttributes	=	true	;				
#endif

#if	(LIBCLANG_MAJOR	>=	6	&&	LIBCLANG_MAJOR	<	10)
				//	Check	if	standard_version	was	specified	and	if	it's	a	version	that	is	supported	by	this	libclang
				if	(standard_version	!=	"")	{
								if	(standard_version	==	"c++11")	{
												//	Turn	off	c++14,	c++11	is	already	on
												ci.getLangOpts().CPlusPlus14	=	false	;
								}	else	if	(standard_version	==	"c++14")	{
												//	Nothing	to	be	done	here
								}else	if	(standard_version	==	"c++17"	||	standard_version	==	"c++20")	{
												std::cerr	<<	"C++	standard	"	<<	standard_version	<<	"	is	not	supported	by	this	version	of	Clang."	<<	std::endl;
								}	else	{
												std::cerr	<<	"Invalid	C++	standard	version	specified:"	<<	standard_version	<<	std::endl;
								}
				}
#endif


				//	Activate	C++17	parsing
#ifdef	TRICK_GCC_VERSION
const	char	*	gcc_version	=	TRICK_GCC_VERSION;
#else
const	char	*	gcc_version	=	"";
#endif



#if	(LIBCLANG_MAJOR	>=	10)
				ci.getLangOpts().GNUCVersion	=	gccVersionToIntOrDefault(gcc_version,	40805);
				ci.getLangOpts().CPlusPlus17	=	true	;

				//	Check	if	standard_version	was	specified	and	if	it's	a	version	that	is	supported	by	this	libclang
				if	(standard_version	!=	"")	{
								if	(standard_version	==	"c++11")	{
												ci.getLangOpts().CPlusPlus14	=	false	;
												ci.getLangOpts().CPlusPlus17	=	false	;
								}	else	if	(standard_version	==	"c++14")	{
												ci.getLangOpts().CPlusPlus17	=	false	;
								}	else	if	(standard_version	==	"c++17"	)	{
												//	Nothing	to	do	here
								}	else	if	(standard_version	==	"c++20")	{
												#if	(LIBCLANG_MAJOR	==	10)
												//	https://github.com/nasa/trick/issues/1519
												//	Before	LLVM	11,	the	flag	was	named	CPlusPlus2a
												ci.getLangOpts().CPlusPlus2a	=	true	;
												#else	
												ci.getLangOpts().CPlusPlus20	=	true	;
												#endif
								}	else	{
												std::cerr	<<	"Invalid	C++	standard	version	specified:"	<<	standard_version	<<	std::endl;
								}
				}
#endif
}
/**
Most	of	the	main	program	is	pieced	together	from	examples	on	the	web.	We	are	doing	the	following:

-#	Creating	lists	of	include	directories,	defines,	and	input	files	from	the	command	line	arguments.
-#	Initializing	the	compiler	to	read	C++	code,	and	setting	the	compiler	to	think	we	are	creating	code	for	the	default	target	architecture.
-#	Creating	the	necessary	source	and	file	managers	as	well	as	the	preprocessor.
-#	Adding	search	directories	and	creating	#define	statements	for	-D	command	line	arguments.
-#	Telling	clang	to	use	our	ICGASTConsumer	as	an	ASTConsumer.
-#	Parsing	the	input	file.
*/
int	main(int	argc,	char	*	argv[])	{
				llvm::cl::SetVersionPrinter([]
#if	(LIBCLANG_MAJOR	>=	6)
								(llvm::raw_ostream&	stream)	{stream
#else
								{std::cout
#endif
												<<	"Trick	Interface	Code	Generator	(trick-ICG)	"	<<	TRICK_VERSION	<<	'\n';}
				);

				/**
					*	Gather	all	of	the	command	line	arguments	into	lists	of	include	directories,	defines,	and	input	files.
					*	All	other	arguments	will	be	ignored.
					*
					*	Filter	out	-W	because	of	LLVM	cl	option	that	has	been	enabled	and	cannot	be	disabled	in	LLVM	10	when	linking	libclang-cpp.so.
					*	TODO:	Troubleshoot	or	contact	LLVM	for	a	fix.		
					*/
				std::vector<const	char	*>	filtered_args;
				for	(	unsigned	int	ii	=	0;		ii	<	argc	;	ii++	)	{
								if(	strncmp(argv[ii],	"-W",	2)	)	{
												filtered_args.push_back(argv[ii]);
								}
				}
				
				llvm::cl::ParseCommandLineOptions(filtered_args.size(),	filtered_args.data());

				if	(input_file_names.empty())	{
								std::cerr	<<	"No	header	file	specified"	<<	std::endl;
								return	1;
				}
				clang::CompilerInstance	ci	;
#if	(LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	<	9)
				clang::CompilerInvocation::setLangDefaults(ci.getLangOpts()	,	clang::IK_CXX)	;
#endif

				ci.createDiagnostics();
				ci.getDiagnosticOpts().ShowColors	=	1	;
				ci.getDiagnostics().setIgnoreAllWarnings(true)	;
				set_lang_opts(ci);

				//	Create	all	of	the	necessary	managers.
				ci.createFileManager();
				ci.createSourceManager(ci.getFileManager());

				//	Tell	the	preprocessor	to	use	its	default	predefines
				clang::PreprocessorOptions	&	ppo	=	ci.getPreprocessorOpts()	;
				ppo.UsePredefines	=	true;

				//	Set	the	default	target	architecture
#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	5))
				clang::TargetOptions	to;
				if	(	m32	)	{
								to.Triple	=	llvm::Triple(llvm::sys::getDefaultTargetTriple()).get32BitArchVariant().str();
				}	else	{
								to.Triple	=	llvm::sys::getDefaultTargetTriple();
				}
				std::shared_ptr<clang::TargetOptions>	shared_to		=	std::make_shared<clang::TargetOptions>(to)	;
				clang::TargetInfo	*pti	=	clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(),	shared_to);
				ci.setTarget(pti);
				ci.createPreprocessor(clang::TU_Complete);
#else
				clang::TargetOptions	*	to	=	new	clang::TargetOptions()	;
				if	(	m32	)	{
								to->Triple	=	llvm::Triple(llvm::sys::getDefaultTargetTriple()).get32BitArchVariant().str();
				}	else	{
								to->Triple	=	llvm::sys::getDefaultTargetTriple();
				}
				clang::TargetInfo	*pti	=	clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(),	to);
				ci.setTarget(pti);
				ci.createPreprocessor();
#endif

				//	Set	all	of	the	defaults	to	c++
#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	9))
				llvm::Triple	trip	(to.Triple)	;
#if	(LIBCLANG_MAJOR	>=	15)
				//clang::CompilerInvocation::setLangDefaults(ci.getLangOpts(),	clang::Language::CXX,	trip,	ppo.Includes)	;
#elif	(LIBCLANG_MAJOR	>=	12)
				clang::CompilerInvocation::setLangDefaults(ci.getLangOpts(),	clang::Language::CXX,	trip,	ppo.Includes)	;
#elif	(LIBCLANG_MAJOR	>=	10)
				clang::CompilerInvocation::setLangDefaults(ci.getLangOpts(),	clang::Language::CXX,	trip,	ppo)	;
#elif	(LIBCLANG_MAJOR	>=	5)
				clang::CompilerInvocation::setLangDefaults(ci.getLangOpts(),	clang::InputKind::CXX,	trip,	ppo)	;
#else
				clang::CompilerInvocation::setLangDefaults(ci.getLangOpts(),	clang::IK_CXX,	trip,	ppo)	;
#endif

				//	setting	the	language	defaults	clears	some	of	the	language	opts,	set	them	again.
				set_lang_opts(ci);

#endif
				clang::Preprocessor&	pp	=	ci.getPreprocessor();

#if	(LIBCLANG_MAJOR	>=	10)	&&	(LIBCLANG_MAJOR	<	18)
				clang::InitializePreprocessor(pp,	ppo,	ci.getPCHContainerReader(),	ci.getFrontendOpts());
#elif	(LIBCLANG_MAJOR	>=	18)
				clang::InitializePreprocessor(pp,	ppo,	ci.getPCHContainerReader(),	ci.getFrontendOpts(),	ci.getCodeGenOpts());
#endif

				//	Add	all	of	the	include	directories	to	the	preprocessor
				HeaderSearchDirs	hsd(ci.getPreprocessor().getHeaderSearchInfo(),	ci.getHeaderSearchOpts(),	pp,	sim_services_flag);
				hsd.addSearchDirs(include_dirs,	isystem_dirs);

				//	Add	a	preprocessor	callback	to	search	for	TRICK_ICG
#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	6))
				std::unique_ptr<FindTrickICG>	ftg(new	FindTrickICG(ci,	hsd,	print_trick_icg	!=	llvm::cl::BOU_FALSE	))	;
				pp.addPPCallbacks(std::move(ftg))	;
#else
				FindTrickICG	*	ftg	=	new	FindTrickICG(ci,	hsd,	print_trick_icg	!=	llvm::cl::BOU_FALSE	)	;
				pp.addPPCallbacks(ftg)	;
#endif

#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	8))
				pp.getBuiltinInfo().initializeBuiltins(pp.getIdentifierTable(),	pp.getLangOpts());
#else
				pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(),	pp.getLangOpts());
#endif
				//	Add	all	of	the	#define	from	the	command	line	to	the	default	predefines
				hsd.addDefines(defines);

				//	Add	our	comment	saver	as	a	comment	handler	in	the	preprocessor
				CommentSaver	cs(ci,	hsd);
				pp.addCommentHandler(&cs);

				PrintAttributes	printAttributes(attr_version,	hsd,	cs,	ci,	force,	sim_services_flag,	output_dir);

				printAttributes.addIgnoreTypes()	;
				//	Create	new	class	and	enum	map	files
				if	(create_map)	{
								printAttributes.createMapFiles();
				}

				//	Tell	the	compiler	to	use	our	ICGASTconsumer
				ICGASTConsumer*	astConsumer	=	new	ICGASTConsumer(ci,	hsd,	cs,	printAttributes);
#if	(LIBCLANG_MAJOR	>	3)	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	6))
				ci.setASTConsumer(std::move(std::unique_ptr<clang::ASTConsumer>(astConsumer)));
#else
				ci.setASTConsumer(astConsumer);
#endif
				ci.createASTContext();
				ci.createSema(clang::TU_Complete,	NULL);

				//	Get	the	full	path	of	the	file	to	be	read
				char	buffer[input_file_names[0].size()	+	1];
				strcpy(buffer,	input_file_names[0].c_str());
				std::string	path(dirname(buffer));
				path	+=	"/";
				strcpy(buffer,	input_file_names[0].c_str());
				path	+=	basename(buffer);
				char*	inputFilePath	=	almostRealPath(path);

				struct	stat	dummy;
				if	(stat(inputFilePath,	&dummy))	{
								std::cerr	<<	"Could	not	open	file	"	<<	inputFilePath	<<	std::endl;
								exit(-1);
				}
				//	Open	up	the	input	file	and	parse	it
#if	(LIBCLANG_MAJOR	>=	10	&&	LIBCLANG_MAJOR	<	18)
				const	clang::FileEntry*	fileEntry	=	ci.getFileManager().getFile(inputFilePath).get();
#elif	(LIBCLANG_MAJOR	>=	18)
				clang::FileEntryRef	fileEntryRef	=	llvm::cantFail(ci.getFileManager().getFileRef(inputFilePath));
#else
				const	clang::FileEntry*	fileEntry	=	ci.getFileManager().getFile(inputFilePath);
#endif
				free(inputFilePath);
#if	((LIBCLANG_MAJOR	>	3	&&	LIBCLANG_MAJOR	<	18))	||	((LIBCLANG_MAJOR	==	3)	&&	(LIBCLANG_MINOR	>=	5))
				ci.getSourceManager().setMainFileID(ci.getSourceManager().createFileID(fileEntry,	clang::SourceLocation(),	clang::SrcMgr::C_User));
#elif	(LIBCLANG_MAJOR	>=	18)
				ci.getSourceManager().setMainFileID(ci.getSourceManager().createFileID(fileEntryRef,	clang::SourceLocation(),	clang::SrcMgr::C_User));
#else
				ci.getSourceManager().createMainFileID(fileEntry);
#endif
				ICGDiagnosticConsumer	*icgDiagConsumer	=	new	ICGDiagnosticConsumer(llvm::errs(),	&ci.getDiagnosticOpts(),	ci,	hsd);
				ci.getDiagnostics().setClient(icgDiagConsumer);
				ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(),	&ci.getPreprocessor());
				clang::ParseAST(ci.getSema());
				ci.getDiagnosticClient().EndSourceFile();

				if	(!sim_services_flag)	{
								printAttributes.printIOMakefile();
				}

				//	Close	the	map	files
				printAttributes.closeMapFiles();

				//	Print	the	list	of	headers	that	have	the	ICG:(No)	comment
				printAttributes.printICGNoFiles();

				if	(icgDiagConsumer->error_in_user_code)	{
								std::cout	<<	color(ERROR,	"Trick	build	was	terminated	due	to	error	in	user	code!")	<<	std::endl;
								exit(-1);
				}

				return	0;
}
