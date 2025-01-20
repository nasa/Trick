#include <iostream>

#include "llvm/Support/CommandLine.h"
#include "ClassValues.hh"
#include "TypedefVisitor.hh"
#include "ClassVisitor.hh"
#include "Utilities.hh"

extern llvm::cl::opt< int > debug_level ;

TypedefVisitor::TypedefVisitor(clang::CompilerInstance & in_ci , CommentSaver & in_cs ,
 HeaderSearchDirs & in_hsd , PrintAttributes & in_pa , std::set< std::string > & fdc ) :
  ci(in_ci) ,
  cs(in_cs) ,
  hsd(in_hsd) ,
  pa(in_pa) ,
  fwd_declared_classes(fdc) ,
  has_dims(false) ,
  cval(NULL) {}

bool TypedefVisitor::VisitDecl(clang::Decl *d) {
    if ( debug_level >=2 ) {
        std::cout << "\nTypedefVisitor Decl = " << d->getDeclKindName() << std::endl ;
    }
    return true ;
}

bool TypedefVisitor::VisitType(clang::Type *t) {
    if ( debug_level >=2 ) {
        std::cout << "\nTypedefVisitor Type = " << t->getTypeClassName() << std::endl ;
    }
    return true;
}

bool TypedefVisitor::VisitConstantArrayType(clang::ConstantArrayType *ct) {
    has_dims = true ;
    return true;
}

bool TypedefVisitor::VisitElaboratedType(clang::ElaboratedType *et) {
    // Test if this typedef is based on a forward declaration.  If it is then we do not
    // need to print out attributes for the type.
    std::string source_type = et->desugar().getAsString() ;
    size_t pos ;
    if ((pos = source_type.find("class ")) != std::string::npos ) {
        source_type.erase(pos , 6) ;
    }
    if ((pos = source_type.find("struct ")) != std::string::npos ) {
        source_type.erase(pos , 7) ;
    }
    std::set< std::string >::iterator it ;
    it = fwd_declared_classes.find(source_type) ;
    if ( it != fwd_declared_classes.end() ) {
        return false;
    }

    return true;
}

bool TypedefVisitor::VisitPointerType(clang::PointerType *pt) {
    has_dims = true ;
    return true;
}

bool TypedefVisitor::VisitRecordType(clang::RecordType *rt) {
    clang::RecordDecl * rd = rt->getDecl() ;
    if ( debug_level >=2 ) {
        std::cout << "\nTypedefVisitor VisitRecordType" << std::endl ;
        rd->dump() ; std::cout << std::endl ;
    }

    if ( rd != NULL and ! has_dims ) {
        CXXRecordVisitor cvis(ci, cs, hsd, pa, true) ;
        cvis.TraverseCXXRecordDecl(clang::cast<clang::CXXRecordDecl>(rd)) ;
        /* Test to see if the typedef name and the struct/union have the same name.
           If they do, we won't add the typedeffed record */
        if ( typedef_name.compare(cvis.get_class_data()->getName()) ) {
            cval = cvis.get_class_data() ;
            cval->setName(typedef_name) ;
            cval->clearNamespacesAndClasses() ;
            cval->getNamespacesAndClasses(typedef_decl_context) ;
            // Set the filename of the typedeffed target to the file where the typedef is.
            cval->setFileName(getFileName( ci , typedef_location.getBegin() , hsd )) ;
            pa.printClass(cval) ;
        }
    }
    return true ;
}

bool TypedefVisitor::VisitTemplateSpecializationType(clang::TemplateSpecializationType *tst) {

    clang::CXXRecordDecl *trec = tst->getAsCXXRecordDecl() ;

    if ( debug_level >=2 ) {
        std::cout << "\nTypedefVisitor VisitTemplateSpecializationType" << std::endl ;
        trec->dump() ; std::cout << std::endl ;
    }

    /* The getDifinition() call retrieves the resolved template if it exists.  The call
       returns NULL if the template was defined but never used */
    clang::TagDecl * td = trec->getDefinition() ;

    if ( td != NULL and ! has_dims ) {
        if ( debug_level >=2 ) {
            td->dump() ; std::cout << std::endl ;
        }
        CXXRecordVisitor cvis(ci, cs, hsd, pa, true) ;
        cvis.TraverseCXXRecordDecl(clang::cast<clang::CXXRecordDecl>(td)) ;
        cval = cvis.get_class_data() ;

        if (cval->isInStandardNamespace()) {
            if ( debug_level >= 4 ) {
                std::cout << "\nTypedefVisitor VisitTemplateSpecializationType not adding class" << std::endl ;
            }
            cval = NULL ;
        } else {
            cval->setName(typedef_name) ;
            cval->clearNamespacesAndClasses() ;
            cval->getNamespacesAndClasses(typedef_decl_context) ;
            // Set the filename of the typedeffed target to the file where the typedef is.
            cval->setFileName(getFileName( ci , typedef_location.getBegin() , hsd )) ;
            pa.printClass(cval) ;
        }
    }

    return true;
}

bool TypedefVisitor::VisitTypedefDecl(clang::TypedefDecl *td) {
    if ( debug_level >=2 ) {
        std::cout << "TypedefVisitior VisitTypedefDecl " << td->getName().str() << "\n" ;
    }
    /* Save the typedef name and location.  We substitute these into the targetted class/struct we parse */

    typedef_name = td->getName().str() ;
    typedef_location = td->getSourceRange() ;
    typedef_decl_context = td->getDeclContext() ;

    this->pa.addTypedefClass(typedef_name) ;

    return true;
}

