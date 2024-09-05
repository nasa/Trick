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

int	main()	{
	return	0;
}