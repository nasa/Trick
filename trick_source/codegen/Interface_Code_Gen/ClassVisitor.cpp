
#include <iostream>
#include <sstream>
#include <stack>

#include "llvm/Support/CommandLine.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/FileManager.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/ASTContext.h"

#include "ClassVisitor.hh"
#include "EnumVisitor.hh"
#include "EnumValues.hh"
#include "FieldVisitor.hh"
#include "Utilities.hh"
#include "CommentSaver.hh"
#include "PrintAttributes.hh"
#include "BraceMacro.hh"
#include "FunctionVisitor.hh"
#include "ConstructorPrinter.hh"

extern llvm::cl::opt< int > debug_level ;

CXXRecordVisitor::CXXRecordVisitor(
 clang::CompilerInstance & in_ci ,
 CommentSaver & in_cs ,
 HeaderSearchDirs & in_hsd ,
 PrintAttributes & in_pa ,
 PrintConstructors * in_pc,
 bool in_include_virtual_base ) :
  ci(in_ci) ,
  cs(in_cs) ,
  hsd(in_hsd) ,
  pa(in_pa) ,
  ctor_printer(in_pc),
  cval() ,
  include_virtual_base(in_include_virtual_base) {}


CXXRecordVisitor::CXXRecordVisitor(
 clang::CompilerInstance & in_ci ,
 CommentSaver & in_cs ,
 HeaderSearchDirs & in_hsd ,
 PrintAttributes & in_pa ,
 bool in_include_virtual_base ) :
  ci(in_ci) ,
  cs(in_cs) ,
  hsd(in_hsd) ,
  pa(in_pa) ,
  ctor_printer(nullptr),
  cval() ,
  include_virtual_base(in_include_virtual_base) {}

CXXRecordVisitor::~CXXRecordVisitor() {
}

bool CXXRecordVisitor::TraverseDecl(clang::Decl *d) {
    if (!d)
        return true;
    if (d->isImplicit())
        return true;

    //std::cout << "CV " << d->getDeclKindName() << std::endl ;

    switch ( d->getKind() ) {
        case clang::Decl::ClassTemplateSpecialization : {
            TraverseClassTemplateSpecializationDecl(static_cast<clang::ClassTemplateSpecializationDecl *>(d)) ;
        }
        break ;
        case clang::Decl::CXXRecord : {
            clang::CXXRecordDecl * crd = static_cast<clang::CXXRecordDecl *>(d) ;
            /* The definition of the record must exist before we can process it.  The definition is
               NULL when this is only a forward declaration of a class.  We also only want to
               process embedded classes that have public access. */
            clang::RecordDecl * rd = crd->getDefinition() ;
            if ( rd != NULL ) {
                if ( rd->getAccess() == clang::AS_public ) {
                    if ( isInUserCode(ci , crd->RBRACELOC(), hsd) ) {
                        CXXRecordVisitor embedded_cvis(ci , cs, hsd , pa, ctor_printer, true) ;
                        embedded_cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(d)) ;
                        pa.printClass(embedded_cvis.get_class_data()) ;
                    }
                } else {
                    // protected and private embedded classes cannot be used outside of their class
                    // in our auto-generated code.  Keep a set of all classes of this type so we can
                    // test against them.
                    ClassValues temp_cv ;
                    temp_cv.getNamespacesAndClasses(crd->getDeclContext()) ;
                    /*
                    for (auto const &pec : private_embedded_classes) {
                        std::cout << "===private_embedded_classes..." << pec << "===" << std::endl;
                    }
                    if (friend_classes.size() > 0) {
                        for (auto const &fc : friend_classes) {
                            std::cout << "===friend_classes..." << fc << "===" << std::endl;
                        }
                    }
                    */
                    std::string class_str = temp_cv.getFullyQualifiedName() + crd->getNameAsString();
                    // Private embedded classes are not printed to io source unless they are friend classes.
                    if (friend_classes.find(class_str) == friend_classes.end()) {
                        private_embedded_classes.insert(class_str);
                    }
                }
            }
        }
        break ;
        case clang::Decl::Enum : {
            clang::EnumDecl * ed = static_cast<clang::EnumDecl *>(d) ;
            /* This is an embedded enumeration within a class.  Only process it if is public */
            if ( ed->getAccess() == clang::AS_public ) {
                EnumVisitor evis(ci, hsd) ;
                // Before llvm 14, TraverseDecl also traversed the type.
                // llvm believed this to be a bug, so now we call TraverseType
                // in addition to TraverseDecl.
                evis.TraverseDecl(ed) ;
                #if (LIBCLANG_MAJOR >= 14)
                evis.TraverseType(clang::QualType(ed->getTypeForDecl(), 0));
                #endif
                pa.printEnum(evis.get_enum_data()) ;
            } else {
                // protected and private embedded classes cannot be used outside of their class
                // in our auto-generated code.  Keep a set of all classes of this type so we can
                // test against them.
                ClassValues temp_cv ;
                temp_cv.getNamespacesAndClasses(ed->getDeclContext()) ;
                private_embedded_classes.insert(temp_cv.getFullyQualifiedName() + ed->getNameAsString()) ;
            }
        }
        break ;
        case clang::Decl::Field : {
            FieldVisitor fvis(ci , hsd , cs, pa, cval.getName()) ;
            fvis.TraverseFieldDecl(static_cast<clang::FieldDecl *>(d)) ;
            cval.addFieldDescription(fvis.get_field_data()) ;
        }
        break;
        case clang::Decl::CXXConstructor : {
            FunctionVisitor fvis(ci , hsd , cs, pa, cval.getName()) ;

            auto* ctor = static_cast<clang::CXXConstructorDecl *>(d);
            
            fvis.TraverseDecl(ctor) ;
            
            cval.addFunctionDescription(fvis.get_function_data()) ;
            std::string class_name = cval.getName();

            //This won't exist for every CXXRecord 
            if(ctor_printer != nullptr)
            {
                ctor_printer->setFullyQualifiedClassName(cval.getFullyQualifiedMangledTypeName());
                ctor_printer->addFunctionDescription(class_name, fvis.get_function_data());
            }
        }
        break ;
        case clang::Decl::Friend : {
            ClassValues temp_cv ;
            temp_cv.getNamespacesAndClasses(d->getDeclContext()) ;
            clang::FriendDecl * fd = static_cast<clang::FriendDecl *>(d) ;
            std::string class_str;

            // Only use namespaces for identifying class name as the class name can't be the same within the same namespace.
            if (fd->getFriendDecl() != NULL) {
                class_str = temp_cv.getNameOnlyWithNamespaces() + fd->getFriendDecl()->getNameAsString();
            } 
            // For friend class, only need to get type here but the above getting class_str is for just in case needed.
            if (fd->getFriendType() != NULL) {
                class_str = temp_cv.getNameOnlyWithNamespaces() + fd->getFriendType()->getType().getAsString();
            }
            size_t pos;
            // Only save class name to the friend class list
            if ((pos = class_str.find("class ")) != std::string::npos ) {
                class_str.erase(pos , 6) ;
                friend_classes.insert(class_str);
            }

            TraverseFriendDecl(static_cast<clang::FriendDecl *>(d)) ;
        }
        break ;
        case clang::Decl::Var : {
            /* Static fields appear as vars. Treat it as a field. */
            FieldVisitor fvis(ci , hsd , cs, pa, cval.getName()) ;
            fvis.TraverseVarDecl(static_cast<clang::VarDecl *>(d)) ;
            cval.addFieldDescription(fvis.get_field_data()) ;
        }
        break ;
        default :
        break ;
    }
    return true ;
}

bool CXXRecordVisitor::VisitDecl(clang::Decl *d) {
    if ( debug_level >=2 ) {
        std::cout << "\n\033[34mCXXRecordVisitor VisitDecl Decl = " << d->getDeclKindName() << "\033[0m" << std::endl ;
    }
    return true ;
}

bool CXXRecordVisitor::VisitType(clang::Type *t) {
    if ( debug_level >=2 ) {
        std::cout << "\nCXXRecordVisitor Type = " << t->getTypeClassName() << std::endl ;
    }
    return true;
}

static bool isTypeTemplateSpecialization(const clang::Type * type_ptr) {
    if ( type_ptr->getTypeClass() == clang::Type::TemplateSpecialization ) {
        return true ;
    } else if ( type_ptr->getTypeClass() == clang::Type::Elaborated ) {
        const clang::ElaboratedType * et = type_ptr->getAs<clang::ElaboratedType>() ;
        //std::cout << "\n[32minherited Type = " << et->getNamedType().getTypePtr()->getTypeClassName() << "[00m" << std::endl ;
        return et->getNamedType().getTypePtr()->getTypeClass() == clang::Type::TemplateSpecialization ;
    }
    return false ;
}

bool CXXRecordVisitor::VisitCXXRecordDecl( clang::CXXRecordDecl *rec ) {
    if ( debug_level >= 2 ) {
        rec->dump() ; std::cout << std::endl ;
    }

    // Return false to stop processing if there is no definition of this class
    if ( rec->getDefinition() == NULL ) {
        return false ;
    }

    // Return false to stop processing if this header file is excluded by one of many reasons.
    std::string header_file_name = getFileName(ci , rec->RBRACELOC(), hsd) ;
    char * rp = almostRealPath(header_file_name.c_str()) ;
    if ( rp == NULL || pa.isHeaderExcluded(header_file_name, false) ) {
        // mark the header as visited so PrintAttributes doesn't process it during addEmptyFiles()
        pa.markHeaderAsVisited(header_file_name);
        return false ;
    }
    cval.setCompat15(hsd.isPathInCompat15(rp)) ;
    free(rp) ;

    // If we have trouble determining the containing namespace and classes skip this variable.
    if ( !cval.getNamespacesAndClasses(rec->getDeclContext())) {
        return false ;
    }

    // If this class needs a default constructor, then the complier will generate one and we can call it.
    if ( rec->needsImplicitDefaultConstructor() ) {
        cval.setHasDefaultConstructor(true) ;
    } else {
        // Test all constructors to see if any of those are the default and public
        clang::CXXRecordDecl::ctor_iterator cit ;
        for ( cit = rec->ctor_begin() ; cit != rec->ctor_end() ; ++cit ) {
            if ( ( !(*cit)->isDeleted() ) and (*cit)->isDefaultConstructor() and (*cit)->getAccess() == clang::AS_public ) {
                cval.setHasDefaultConstructor(true) ;
            }
        }
    }

    // Test to see there is no declared destructor or it is declared public
    clang::CXXDestructorDecl * dd  = rec->getDestructor() ;
    if ( dd == NULL or dd->getAccess() == clang::AS_public ) {
        cval.setHasPublicDestructor(true) ;
    }

    cval.setFileName(header_file_name) ;
    cval.setAbstract(rec->isAbstract()) ;
    cval.setName(rec->getNameAsString()) ;
    cval.setPOD(rec->isPOD()) ;

    cval.setSize(rec->getASTContext().getASTRecordLayout(rec).getSize().getQuantity()) ;


    //std::cout << "parsing " << cval.getName() << std::endl ;
    //std::cout << "    [34mprocessing inheritance " <<  rec->getNumBases() << " " << rec->getNumVBases() << "[00m" << std::endl ;
    clang::CXXRecordDecl::base_class_iterator bcii ;
    for ( bcii = rec->bases_begin() ; bcii != rec->bases_end() ; ++bcii ) {
        if ( !bcii->isVirtual() ) {
            const clang::Type * temp = bcii->getType().getTypePtr() ;
            //std::cout << "\n[33minherited Type = " << temp->getTypeClassName() << "[00m" << std::endl ;
            const clang::RecordType * rt = temp->getAs<clang::RecordType>() ;
            if ( rt != NULL ) {
                clang::RecordDecl * rd = rt->getDecl() ;
                //std::cout << "    [34m" << cval.getName() << " inherits from " << rd->getNameAsString() << "[00m" << std::endl ;
                //rd->dump() ; std::cout << std::endl ;
                if ( isInUserOrTrickCode(ci , rd->RBRACELOC(), hsd) ) {
                    const clang::ASTRecordLayout &record_layout = rec->getASTContext().getASTRecordLayout(rec);
                    unsigned int inherit_class_offset ;

                    inherit_class_offset = record_layout.getBaseClassOffset(llvm::cast<clang::CXXRecordDecl>(rd)).getQuantity() ;
                    //std::cout << "    [34minherit_class_offset = " << inherit_class_offset << "[00m" << std::endl ;
                    //std::cout << "    [34m" << getFileName(ci , rd->RBRACELOC(), hsd) << "[00m" << std::endl ;
                    CXXRecordVisitor inherit_cvis(ci , cs, hsd , pa, false) ;
                    if (inherit_cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(rd))) {
                        cval.addInheritedFieldDescriptions(inherit_cvis.get_class_data()->getFieldDescriptions(), inherit_class_offset, false) ;
                        // clear the field list in the inherited class so they are not freed when inherit_cvis
                        // goes out of scope.
                        inherit_cvis.get_class_data()->clearFieldDescription() ;
                        // If we are inheriting from a template specialization, don't save the inherited class.  This list
                        // is maintained to call init_attr of the inherited classes.  A template specialization does not
                        // havethese attributes.
                        if ( ! isTypeTemplateSpecialization(temp) ) {
                            // We want to save the inherited class data, but it's going to go out of scope so we need
                            // to make a copy of it.
                            ClassValues * icv = new ClassValues(*(inherit_cvis.get_class_data())) ;
                            // The inherited classes of this inherited class are not required in the copy,
                            // and they are going out of scope
                            cval.saveInheritAncestry(icv) ;
                            icv->clearInheritedClass() ;

                            // Save the copy of the inherited class to the current class
                            cval.addInheritedClass(inherit_cvis.get_class_data()->getFullyQualifiedTypeName()) ;
                        }
                    }
                }
            }
        }
    }
    //std::cout << "    [34mend inheritance[00m" << std::endl ;

    // Virtual base classes are parsed once
    // When processing inherited classes include_virtual_base will be set to true
    // so we don't process virtual inherited classes multiple times.
    if ( include_virtual_base ) {
        for ( bcii = rec->vbases_begin() ; bcii != rec->vbases_end() ; ++bcii ) {
            const clang::Type * temp = bcii->getType().getTypePtr() ;
            //std::cout << "\n[33minherited Type = " << temp->getTypeClassName() << "[00m" << std::endl ;
            const clang::RecordType * rt = temp->getAs<clang::RecordType>() ;
            if ( rt != NULL ) {
                clang::RecordDecl * rd = rt->getDecl() ;
                //std::cout << "    [34m" << cval.getName() << " virtually inherits from "
                // << rd->getNameAsString() << "[00m" << std::endl ;
                //rd->dump() ; std::cout << std::endl ;
                if ( isInUserOrTrickCode(ci , rd->RBRACELOC(), hsd) ) {
                    const clang::ASTRecordLayout &record_layout = rec->getASTContext().getASTRecordLayout(rec);
                    unsigned int inherit_class_offset ;

                    // Virtual base classes have a fixed offset that cannot be accessed at runtime.  We have clang's calculation
                    // to where the base class will be.
                    inherit_class_offset = record_layout.getVBaseClassOffset(llvm::cast<clang::CXXRecordDecl>(rd)).getQuantity() ;

                    //std::cout << "    [34minherit_class_offset = " << inherit_class_offset << "[00m" << std::endl ;
                    //std::cout << "    [34m" << getFileName(ci , rd->RBRACELOC(), hsd) << "[00m" << std::endl ;
                    CXXRecordVisitor inherit_cvis(ci , cs, hsd , pa, false) ;
                    inherit_cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(rd)) ;
                    cval.addInheritedFieldDescriptions(inherit_cvis.get_class_data()->getFieldDescriptions(), inherit_class_offset, true) ;
                    // clear the field list in the inherited class so they are not freed when inherit_cvis goes out of scope.
                    inherit_cvis.get_class_data()->clearFieldDescription() ;
                    // If we are inheriting from a template specialization, don't save the inherited class.  This list
                    // is maintained to call init_attr of the inherited classes.  A template specialization does not have
                    // these attributes.
                    if ( ! isTypeTemplateSpecialization(temp) ) {
                        ClassValues * icv = new ClassValues(*(inherit_cvis.get_class_data())) ;
                        // The inherited classes of this inherted class are not required in the copy, and they are going out of scope
                        icv->clearInheritedClass() ;

                        // Save the copy of the inherited class to the current class
                        cval.addInheritedClass(inherit_cvis.get_class_data()->getFullyQualifiedTypeName()) ;
                    }
                }
            }
        }
    }

    // clears obscured inherited variables caused by diamond inheritance
    cval.clearAmbiguousVariables() ;

    // sets the container class for all fields to the current class name
    cval.setContainerClassForFields() ;

    return true;
}

// Test if we have a friend with the name "init_attr<class_name>"
bool CXXRecordVisitor::VisitFriendDecl( clang::FriendDecl *fd ) {
    //std::cout << "    [33mClassVisitor::VisitFriendDecl[00m" << std::endl ;
    clang::TypeSourceInfo * tsi = fd->getFriendType() ;
    if ( tsi == NULL ) {
        clang::NamedDecl * nd = fd->getFriendDecl() ;
        if ( nd != NULL and nd->getDeclName().isIdentifier()) {
            //nd->dump() ;
            std::string init_func = std::string("init_attr") ;
            std::stringstream name_sp ;
            cval.printNamespaces( name_sp , "__") ;
            init_func += name_sp.str() + cval.getName() ;
            std::string friend_str = nd->getName().str() ;
            if ( ! friend_str.find(init_func) ) {
                //std::cout << "        [32mfound a friend![00m" << std::endl ;
                cval.setHasInitAttrFriend(true) ;
            } else if ( ! friend_str.find("init_attr") ) {
                std::cerr << bold(color(WARNING,"Warning")) << "    " << cval.getFileName() << ":" <<
                 ci.getSourceManager().getSpellingLineNumber(fd->getSourceRange().getBegin()) << ":" << std::endl ;
                std::cerr << "           friend " << friend_str << " does not match expected name " <<
                  init_func << std::endl ;
            }
        }
    }
    return true ;
}

ClassValues * CXXRecordVisitor::get_class_data() {
    return &cval ;
}

std::set<std::string> CXXRecordVisitor::private_embedded_classes ;

std::set<std::string> CXXRecordVisitor::friend_classes ;

void CXXRecordVisitor::addPrivateEmbeddedClass( std::string in_name ) {
    private_embedded_classes.insert(in_name) ;
}

bool CXXRecordVisitor::isPrivateEmbeddedClass( std::string in_name ) {
    size_t pos ;
    while ((pos = in_name.find("class ")) != std::string::npos ) {
        in_name.erase(pos , 6) ;
    }
    while ((pos = in_name.find("struct ")) != std::string::npos ) {
        in_name.erase(pos , 7) ;
    }
    while ((pos = in_name.find("enum ")) != std::string::npos ) {
        in_name.erase(pos , 5) ;
    }
    while ((pos = in_name.find("const ")) != std::string::npos ) {
        in_name.erase(pos , 6) ;
    }
    // remove any array or pointer dimensions.
    if ( in_name.find_first_of("[*") != std::string::npos ) {
        in_name.erase(in_name.find_first_of("[*")) ;
    }
    // remove trailing spaces
    in_name.erase(std::find_if(in_name.rbegin(), in_name.rend(), 
        [](int c) {return !std::isspace(c);}).base(), in_name.end());

    // remove all template arguments "<text>"
    bool template_arg_found ;
    do {
        template_arg_found = false ;
        std::stack<int> bracket_pos ;
        for ( int ii = 0 ; ii < in_name.length() ; ii++ ) {
            if ( in_name[ii] == '<' ) {
                bracket_pos.push(ii) ;
            } else if ( in_name[ii] == '>' ) {
                int begin = bracket_pos.top() ;
                bracket_pos.pop() ;
                if ( bracket_pos.empty() ) {
                    template_arg_found = true ;
                    in_name.erase(begin, ii - begin + 1) ;
                    break ;
                }
            }
        }
    } while ( template_arg_found == true ) ;

    std::set<std::string>::iterator it ;
    it = private_embedded_classes.find(in_name) ;
    if ( it != private_embedded_classes.end() ) {
        return true ;
    }
    return false ;
}
