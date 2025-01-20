
#include <iostream>
#include <sstream>


#include "ClassValues.hh"
#include "FieldDescription.hh"

#ifdef __APPLE__
#include "llvm/Support/CommandLine.h"
extern llvm::cl::opt< bool > global_compat15 ;
#endif

ClassValues::ClassValues() :
  has_init_attr_friend(false) ,
  is_pod(false) ,
  is_abstract(false) ,
  has_default_constructor(false) ,
  has_public_destructor(false),
  compat15(false)
{}

ClassValues::~ClassValues() {
    std::vector<FieldDescription *>::iterator fdit ;
    for ( fdit = field_descripts.begin() ; fdit != field_descripts.end() ; ++fdit ) {
        delete (*fdit) ;
    }
}

void ClassValues::addFieldDescription(FieldDescription * in_fdes) {
    field_descripts.push_back(in_fdes) ;

#ifdef __APPLE__
    if ( !global_compat15 and !compat15 ) {
#else
    {
#endif
        // Test to see if the new field overloads a field of the same name.  If it does
        // then fully qualify the name of the inherited field (the one already in field_name_to_info).
        std::map< std::string , FieldDescription * >::iterator mit = field_name_to_info_map.find(in_fdes->getName()) ;
        if ( mit != field_name_to_info_map.end() ) {
            // If the matched variable is inherited, qualify it with its container class name.
            if ( (*mit).second->isInherited() ) {
                (*mit).second->setName( (*mit).second->getContainerClass() + "::" + (*mit).second->getName() ) ;
                field_name_to_info_map[(*mit).second->getName()] = (*mit).second ;
                field_name_to_info_map.erase(mit) ;
            }
        }
    }
    field_name_to_info_map[in_fdes->getName()] = in_fdes ;
}

void ClassValues::addFunctionDescription(FunctionDescription * in_fdes)
{
    function_descripts.push_back(in_fdes);
}

void ClassValues::addInheritedFieldDescriptions(std::vector<FieldDescription *> in_fdes,
  unsigned int class_offset, bool virtual_inherited ) {
    // Make a copy of all of the FieldDescription variables.
    field_descripts.insert(field_descripts.end(), in_fdes.begin() , in_fdes.end()) ;

    std::vector<FieldDescription *>::iterator fdit ;
    // Loop through the incoming inherited variable names
    for ( fdit = in_fdes.begin() ; fdit != in_fdes.end() ; ++fdit ) {

        (*fdit)->setBaseClassOffset( class_offset ) ;
        (*fdit)->setInherited( true ) ;
        (*fdit)->setVirtualInherited( virtual_inherited ) ;
        // Adds the class offset to the field offset giving the total offset to the inherited variable
        // The offset is stored in bits so multiply class_offset by 8.
        (*fdit)->addOffset( class_offset * 8 ) ;
    }

#ifdef __APPLE__
    if ( !global_compat15 and !compat15 ) {
#else
    {
#endif
        for ( fdit = in_fdes.begin() ; fdit != in_fdes.end() ; ++fdit ) {

            std::string in_name = (*fdit)->getName() ;
            // search existing names for incoming inherited variable name.
            std::map< std::string , FieldDescription * >::iterator mit = field_name_to_info_map.find(in_name) ;
            // if the variable name already exists we have overloaded variable names.
            if ( mit != field_name_to_info_map.end() ) {
                // the incoming variable is known to be inherited.  Qualify it with its container class name.
                (*fdit)->setName( (*fdit)->getContainerClass() + "::" + (*fdit)->getName() ) ;
                field_name_to_info_map[(*fdit)->getName()] = *fdit ;

                // If the matched variable is also inherited, qualify it with its container class name.
                if ( (*mit).second->isInherited() ) {
                    (*mit).second->setName( (*mit).second->getContainerClass() + "::" + (*mit).second->getName() ) ;
                    field_name_to_info_map[(*mit).second->getName()] = (*mit).second ;
                    field_name_to_info_map.erase(mit) ;
                    // Mark this name is one that we always have to qualify.
                    field_names_to_qualify.insert(in_name) ;
                }
            } else {
                // incoming name did not match an existing variable name.
                // Test to see if there variables of this name need to be qualified.
                if ( field_names_to_qualify.find(in_name) == field_names_to_qualify.end() ) {
                    // The name is not overloaded (yet), add the unqualified inherited name straight into map.
                    field_name_to_info_map[in_name] = *fdit ;
                    // Upgrade the inherited variable's container class to the current class.
                    //(*fdit)->setContainerClass( name ) ;
                } else {
                    // The name is overloaded by other inherited variables... qualify the name.
                    (*fdit)->setName( (*fdit)->getContainerClass() + "::" + (*fdit)->getName() ) ;
                    field_name_to_info_map[(*fdit)->getName()] = *fdit ;
                }
            }
        }
    }

}

void ClassValues::saveInheritAncestry( ClassValues * in_cv  ) {

    std::map< std::string , unsigned int >::iterator mit ;
    for ( mit = in_cv->all_inherited_class_names_map.begin() ; mit != in_cv->all_inherited_class_names_map.end() ; ++mit ) {
        all_inherited_class_names_map[(*mit).first] +=  (*mit).second ;
    }

    all_inherited_class_names_map[in_cv->getName()]++ ;
}

void ClassValues::setContainerClassForFields() {
    std::vector<FieldDescription *>::iterator fdit ;
    // Loop through all fields
    for ( fdit = field_descripts.begin() ; fdit != field_descripts.end() ; ++fdit ) {
        // Prepend the field container class with the current class name.
        (*fdit)->setContainerClass( name + "::" + (*fdit)->getContainerClass() ) ;
    }
}

void ClassValues::clearAmbiguousVariables() {

    std::map< std::string , unsigned int >::iterator mit ;
    std::vector<FieldDescription *>::iterator fdit ;

    for ( mit = all_inherited_class_names_map.begin() ; mit != all_inherited_class_names_map.end() ; ++mit ) {
        // If a class in inherited more than once we have a diamond.  We'll need to modify variables that were
        // inherited from that class.
        if ( (*mit).second > 1 ) {
            std::string str = (*mit).first + "::" ;
            // Loop through all fields testing for the diamond inherited class in the name.
            for ( fdit = field_descripts.begin() ; fdit != field_descripts.end() ; ++fdit ) {
                std::string fdit_name = (*fdit)->getName() ;
                size_t f = fdit_name.find(str) ;
                // If the variable contains the diamond class string, remove the diamond class qualification.
                if ( f != std::string::npos ) {
                    fdit_name.replace(f , str.length() , "") ;
                    (*fdit)->setName(fdit_name) ;
                }
            }
        }
    }

    // Removing diamond class qualifications possibly leaves fields with duplicate names.
    // We want to remove the duplicated names.
    std::set< std::string > field_names ;
    fdit = field_descripts.begin() ;
    while ( fdit != field_descripts.end() ) {
        // test if the current field is in the list of field names processed.
        if ( field_names.find( (*fdit)->getName() ) == field_names.end() ) {
            // This name is new.
            // Add the field to the test set
            field_names.insert( (*fdit)->getName() ) ;
            fdit++ ;
        } else {
            // This name was already present
            // Free the field's memory.
            delete (*fdit) ;
            // Erase the field from the list. Move iterator to next field.
            fdit = field_descripts.erase(fdit) ;

        }
    }

}

void ClassValues::clearFieldDescription() {
    field_descripts.clear() ;
}

void ClassValues::clearFunctionDescription() {
    function_descripts.clear() ;
}


void ClassValues::addInheritedClass(std::string  class_name) {
    inherited_classes.push_back(class_name) ;
}

void ClassValues::clearInheritedClass() {
    inherited_classes.clear() ;
}

void ClassValues::setHasInitAttrFriend(bool in_val) {
    has_init_attr_friend = in_val ;
}

bool ClassValues::getHasInitAttrFriend() {
    return has_init_attr_friend ;
}

void ClassValues::setPOD(bool in_val) {
    is_pod = in_val ;
}

bool ClassValues::isPOD() {
    return is_pod ;
}

void ClassValues::setSize(unsigned int in_size) {
    size = in_size ;
}

unsigned int ClassValues::getSize() {
    return size ;
}

void ClassValues::setAbstract(bool in_val) {
    is_abstract = in_val ;
}

bool ClassValues::isAbstract() {
    return is_abstract ;
}

void ClassValues::setHasDefaultConstructor(bool in_val) {
    has_default_constructor = in_val ;
}

bool ClassValues::getHasDefaultConstructor() {
    return has_default_constructor ;
}

void ClassValues::setHasPublicDestructor(bool in_val) {
    has_public_destructor = in_val ;
}

bool ClassValues::getHasPublicDestructor() {
    return has_public_destructor ;
}

void ClassValues::setMangledTypeName( std::string in_val ) {
    mangled_type_name = in_val ;
}

std::string ClassValues::getMangledTypeName() {
    if ( mangled_type_name.empty() ) {
        return name ;
    }
    return mangled_type_name ;
}

std::string ClassValues::getFullyQualifiedMangledTypeName(const std::string& delimiter) {
    std::ostringstream oss ;
    printNamespacesAndContainerClasses(oss, delimiter) ;
    oss << getMangledTypeName() ;
    return oss.str() ;
}

std::string ClassValues::getFullyQualifiedNameIfEqual() {
    std::string result;
    if (getName() == getMangledTypeName()) {
        result += getNamespacesAndContainerClasses();
    }
    result += getName();
    return result;
}


void ClassValues::setCompat15(bool in_val) {
    compat15 = in_val ;
}

bool ClassValues::isCompat15() {
    return compat15 ;
}

bool ClassValues::isInStandardNamespace() {
    return namespaces.size() && !namespaces[0].compare("std");
}

std::ostream & operator << (std::ostream & ostream, ClassValues & cv) {
    ostream << "  name = " << cv.name << std::endl ;
    ostream << "  mangled_name = " << cv.mangled_type_name << std::endl ;
    ostream << "  file_name = " << cv.file_name << std::endl ;
    ostream << "  namespaces =" ;
    cv.printNamespaces(ostream) ;
    ostream << std::endl ;
    ostream << "  parent classes =" ;
    cv.printContainerClasses(ostream) ;
    ostream << std::endl ;
    ostream << "  has_init_attr_friend = " << cv.has_init_attr_friend << std::endl ;
    ostream << "  is_pod = " << cv.is_pod << std::endl ;
    ostream << "  is_abstract = " << cv.is_abstract << std::endl ;
    ostream << "  has_default_constructor = " << cv.has_default_constructor << std::endl ;
    ostream << "  has_public_destructor = " << cv.has_public_destructor << std::endl ;

    for (auto& field : cv.getFieldDescriptions()) {
        ostream << field << std::endl ;
    }

    return ostream ;
}
