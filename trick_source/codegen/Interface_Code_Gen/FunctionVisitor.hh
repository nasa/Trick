#ifndef __FUNCTION_VISITOR_HH__
#define __FUNCTION_VISITOR_HH__

#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/DeclCXX.h"

#include "PrintAttributes.hh"

class CommentSaver ;
class HeaderSearchDirs ;
class SourceManager ;
class FunctionDescription ;

class FunctionVisitor : public clang::RecursiveASTVisitor<FunctionVisitor> 
{
    public:
    FunctionVisitor(clang::CompilerInstance & in_ci ,
          HeaderSearchDirs & in_hsd ,
          CommentSaver & cs ,
          PrintAttributes & in_pa ,
          std::string container_class ) ;

    bool VisitMethodDecl(clang::CXXMethodDecl* fd) ;
    bool VisitCXXConstructorDecl(clang::CXXConstructorDecl *ctor);

    inline FunctionDescription * get_function_data();

    /** The compiler's source manager.  Holds file/line info for everything. */
    clang::CompilerInstance & ci ;

    /** Holds all comments */
    CommentSaver & cs ;

    /** The header search directories */
    HeaderSearchDirs & hsd ;

    /** attributes printer */
    PrintAttributes & pa ;

    /** Holds the field information found, usually returned to caller of this visitor. */
    FunctionDescription * fdes ;
};

inline FunctionDescription* FunctionVisitor::get_function_data() { return fdes ;}

#endif // __FUNCTION_VISITOR_HH__