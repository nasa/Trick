
#include	<iostream>
#include	"clang/AST/ASTContext.h"
#include	"ICGASTConsumer.hh"

ICGASTConsumer::ICGASTConsumer(	clang::CompilerInstance	&	in_ci	,	HeaderSearchDirs	&	in_hsd	,
	CommentSaver	&	in_cs	,	PrintAttributes	&	in_pa	)	:
	ci(in_ci)	,	hsd(in_hsd)	,	tuv(in_ci,	in_hsd,	in_cs,	in_pa)	{}

TranslationUnitVisitor	&	ICGASTConsumer::getTranslationUnitVisitor()	{
				return	tuv	;
}

/**
@details
-#	Traverse	the	translation	unit	declaration	and	everything	it	contains.
*/
void	ICGASTConsumer::HandleTranslationUnit(clang::ASTContext	&Ctx)	{
				tuv.TraverseDecl(Ctx.getTranslationUnitDecl());
}

