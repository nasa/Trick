
#ifndef TRANSLATIONUNITVISITOR_HH
#define TRANSLATIONUNITVISITOR_HH

#include <string>
#include <vector>
#include <set>
#include <map>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"

class CommentSaver ;
class ClassValues ;
class EnumValues ;
class HeaderSearchDirs ;
class PrintAttributes ;
class PrintConstructors;

/**

  This is the top level AST visitor.  This class dispatched nodes we are interested
  in to lower level AST visitors.  All class information is stored in this visitor.

  @author Alexander S. Lin

  @date July 2012

 */

class TranslationUnitVisitor : public clang::RecursiveASTVisitor<TranslationUnitVisitor> {
    public:
        TranslationUnitVisitor( clang::CompilerInstance & in_ci , HeaderSearchDirs & hsd ,
         CommentSaver & cs , PrintAttributes & in_pa, PrintConstructors & in_pc) ;

        /** custom node traversal function that dispatches nodes we are interested in
            to lower level AST visitors */
        bool TraverseDecl(clang::Decl *D);

    private:
        /** The compiler instance  */
        clang::CompilerInstance & ci ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** Holds all comments */
        CommentSaver & cs ;

        /** attributes printer */
        PrintAttributes & pa ;

        /* Used to print constructors to file */
        PrintConstructors & ctor_printer;

        std::map < std::string , std::set< std::string > > fwd_declared_classes ;

} ;

#endif
