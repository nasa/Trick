
#include <iostream>
#include "clang/AST/ASTContext.h"
#include "ICGASTConsumer.hh"

ICGASTConsumer::ICGASTConsumer( clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ,
 CommentSaver & in_cs , PrintAttributes & in_pa, PrintConstructors & in_pc) :
 ci(in_ci) , hsd(in_hsd) , pc(in_pc),  tuv(in_ci, in_hsd, in_cs, in_pa, in_pc) {}

TranslationUnitVisitor & ICGASTConsumer::getTranslationUnitVisitor() {
    return tuv ;
}

/**
@details
-# Traverse the translation unit declaration and everything it contains.
*/
void ICGASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
    tuv.TraverseDecl(Ctx.getTranslationUnitDecl());
}

