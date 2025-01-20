#include "FunctionVisitor.hh"
#include "FunctionDescription.hh"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Type.h"
#include "llvm/Support/raw_ostream.h" 

FunctionVisitor::FunctionVisitor(clang::CompilerInstance & in_ci ,
                HeaderSearchDirs & in_hsd ,
                CommentSaver & in_cs ,
                PrintAttributes & in_pa ,
                std::string container_class ) 
                :
ci(in_ci) ,
hsd(in_hsd) ,
cs(in_cs) ,
pa(in_pa)
{
    fdes = new FunctionDescription(container_class) ;
}

bool FunctionVisitor::VisitMethodDecl(clang::CXXMethodDecl* fd)
{
   

    return true;
}

bool FunctionVisitor::VisitCXXConstructorDecl(clang::CXXConstructorDecl *ctor)
{
    
    uint32_t num_params = ctor->getNumParams();
    
    //Return type for constuctors is void    
    fdes->setReturnType(ctor->getType().getAsString());

    for(uint32_t i = 0; i < num_params; ++i)
    {
        FunctionArgument funcArg;

        clang::ParmVarDecl *param = ctor->getParamDecl(i);
        clang::QualType qt = param->getType();
        std::string paramTypeStr = qt.getAsString();

        // Get the parameter name
        std::string paramName = param->getNameAsString();

        funcArg.typeName = paramTypeStr;
        funcArg.argName = paramName;
        funcArg.isPointer = qt->isPointerType();
        funcArg.isReference = qt->isReferenceType();
        funcArg.isConst = qt.isConstQualified();

        fdes->addFunctionArgument(funcArg);
    }

    fdes->setIsConstructor(true);
    
    fdes->setIsDefaultConstructor(ctor->isDefaultConstructor());

    fdes->setAccess(ctor->getAccess());
    
    fdes->setFunctionName(ctor->getNameAsString());
    
    fdes->setFullyQualifiedContainerClassName(ctor->getParent()->getQualifiedNameAsString());

    fdes->setIsCopyConstructor(ctor->isCopyConstructor());
    
    fdes->setIsMoveConstructor(ctor->isMoveConstructor());


    return true;
}