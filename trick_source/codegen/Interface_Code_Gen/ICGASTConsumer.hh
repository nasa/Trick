
#ifndef ICGASTCONSUMER_HH
#define ICGASTCONSUMER_HH

#include "clang/AST/ASTConsumer.h"
#include "clang/Sema/SemaConsumer.h"
#include "TranslationUnitVisitor.hh"

class HeaderSearchDirs ;
class CommentSaver ;
class PrintAttributes ;
class PrintConstructors;

/**

  This class is passed to the clang parser as an ASTconsumer. We are
  only interested in parsing the AST when all nodes are finished.
  Therefore we only have a single TranslationUnit handler that will
  handle everything.

  @author Alexander S. Lin

  @date July 2012

 */


class ICGASTConsumer : public clang::SemaConsumer {
    public:
        ICGASTConsumer( clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ,
         CommentSaver & in_cs , PrintAttributes & in_pa, PrintConstructors & in_pc) ;

        TranslationUnitVisitor & getTranslationUnitVisitor() ;

        /** Called with the compiler is finished parsing everything */
        void HandleTranslationUnit(clang::ASTContext &Ctx);

    private:

        /** The compiler instance */
        clang::CompilerInstance & ci ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** The top level AST visitor. Called to parse tree in HandleTranslationUnit */
        TranslationUnitVisitor tuv ;

        PrintConstructors & pc;
};

#endif
