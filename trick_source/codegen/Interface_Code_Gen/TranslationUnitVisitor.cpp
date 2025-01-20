
#include <iostream>
#include <fstream>
#include <libgen.h>
#include <sys/stat.h>

#include "clang/AST/RecordLayout.h"
#include "clang/Basic/FileManager.h"

#include "TranslationUnitVisitor.hh"
#include "HeaderSearchDirs.hh"
#include "ClassTemplateVisitor.hh"
#include "ClassValues.hh"
#include "ClassVisitor.hh"
#include "EnumValues.hh"
#include "EnumVisitor.hh"
#include "TypedefVisitor.hh"
#include "VariableVisitor.hh"
#include "Utilities.hh"
#include "PrintAttributes.hh"
#include "BraceMacro.hh"
#include "ConstructorPrinter.hh"

TranslationUnitVisitor::TranslationUnitVisitor(clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ,
  CommentSaver & in_cs , PrintAttributes & in_pa, PrintConstructors & in_pc) :
  ci(in_ci) , hsd(in_hsd) , cs(in_cs) , pa(in_pa), ctor_printer(in_pc) {}

bool TranslationUnitVisitor::TraverseDecl(clang::Decl *d) {
    if (!d)
        return true;
    if (d->isImplicit())
        return true;

    //std::cout << "\nTranslationUnitVisitor " << d->getDeclKindName() << std::endl ;
    //d->dump() ;

    switch ( d->getKind() ) {
        case clang::Decl::TranslationUnit : {
            TraverseTranslationUnitDecl(static_cast<clang::TranslationUnitDecl *>(d)) ;
        }
        break ;
        case clang::Decl::CXXRecord : {
            clang::CXXRecordDecl * crd = static_cast<clang::CXXRecordDecl *>(d) ;
            clang::RecordDecl * rd = crd->getDefinition() ;
            bool is_forward_declaration = false;

            /* The definition of the record must exist before we can process it.  The definition is
               NULL when this is only a forward declaration of a class */
            if ( rd == NULL ) {
                is_forward_declaration = true;
            }
            /* Sometimes rd is not NULL when we still only have a forward declaration because
               the file that defines the class is included before processing this fwd declare.
               In this case the CXXRecordDecl file name will not match the current file name, and is
               in fact empty */
            else {
                std::string rd_file = getFileName(ci , rd->RBRACELOC(), hsd) ;
                std::string crd_file = getFileName(ci , crd->RBRACELOC(), hsd) ;
                if (crd_file.empty() || crd_file.compare(rd_file)) {
                    is_forward_declaration = true;
                }
            }

            if (is_forward_declaration) {
                // These are forward declarations.  Insert this into the set of fwd declares keyed by the current file.
                if ( ! crd->getNameAsString().empty() ) {
                    fwd_declared_classes[getFileName(ci , d->GETLOCEND(), hsd)].insert(crd->getNameAsString()) ;
                }
            }
            else {
                //crd->dump() ; std::cout << std::endl ;
                if ( isInUserCode(ci , crd->RBRACELOC(), hsd) ) {
                    CXXRecordVisitor cvis(ci , cs, hsd , pa, &ctor_printer, true) ;
            
                    cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(d)) ;
                    pa.printClass(cvis.get_class_data()) ;

                    /* Check to see if the struct/class is forward declared in the same file.
                       If it is, then remove the notation that it is forward declared.  This
                       is to allow C structs to be forward declared and typedeffed and io_src
                       code will be generated for both the original structure name and typedeffed
                       name.

                       struct Astruct ;
                       typedef struct Astruct {} Bstruct ;
                    */
                    std::set< std::string >::iterator it ;
                    std::string file_name = getFileName(ci , d->GETLOCEND(), hsd) ;
                    std::string source_type = cvis.get_class_data()->getName() ;
                    it = fwd_declared_classes[file_name].find(source_type) ;
                    if ( it != fwd_declared_classes[file_name].end() ) {
                        fwd_declared_classes[file_name].erase(it) ;
                    }
                }
            }
        }
        break ;
        case clang::Decl::Enum : {
            clang::EnumDecl * ed = static_cast<clang::EnumDecl *>(d) ;
            if ( isInUserCode(ci , ed->RBRACELOC(), hsd) ) {
                EnumVisitor evis(ci, hsd) ;
                // Before llvm 14, TraverseDecl also traversed the type.
                // llvm believed this to be a bug, so now we call TraverseType
                // in addition to TraverseDecl.
                evis.TraverseDecl(ed) ;
                #if (LIBCLANG_MAJOR >= 14)
                evis.TraverseType(clang::QualType(ed->getTypeForDecl(), 0));
                #endif
                //if ( evis.get_enum_data() != NULL ) {
                    pa.printEnum(evis.get_enum_data()) ;
                //}
            }
        }
        break ;
        case clang::Decl::LinkageSpec : {
            // This is an extern "C" block, traverse the node and the contents within the extern "C" block.
            TraverseLinkageSpecDecl(static_cast<clang::LinkageSpecDecl *>(d)) ;
        }
        break ;
        case clang::Decl::Namespace : {
            TraverseNamespaceDecl(static_cast<clang::NamespaceDecl *>(d)) ;
        }
        break ;
        case clang::Decl::ClassTemplate : {
            clang::ClassTemplateDecl * ctd = static_cast<clang::ClassTemplateDecl *>(d) ;
            if ( isInUserCode(ci, ctd->getSourceRange().getBegin(), hsd) ) {
                clang::CXXRecordDecl * rd = ctd->getTemplatedDecl() ;
                ClassTemplateVisitor ctvis(ci, cs, hsd, pa) ;
                ctvis.TraverseDecl(rd) ;
            }
        }
        break ;
        case clang::Decl::Typedef : {
            clang::TypedefDecl * td = static_cast<clang::TypedefDecl *>(d) ;
            if ( isInUserCode(ci, td->getSourceRange().getBegin(), hsd) ) {
                TypedefVisitor tv(ci , cs, hsd, pa, fwd_declared_classes[getFileName(ci , d->GETLOCEND(), hsd)]) ;
                tv.TraverseDecl(d) ;
            }
        }
        break ;
        case clang::Decl::Var : {
            if ( isInUserCode(ci, d->getSourceRange().getBegin(), hsd) ) {
                VariableVisitor tv(ci , cs, hsd, pa, fwd_declared_classes[getFileName(ci , d->GETLOCEND(), hsd)]) ;
                tv.TraverseDecl(d) ;
            }
        }
        break ;
        default :
        break ;
    }
    return true ;
}

