
#ifndef CLASSVALUES_HH
#define CLASSVALUES_HH

#include <string>
#include <vector>
#include <set>
#include <map>
#include <set>
#include "ConstructValues.hh"

class FieldDescription ;
class FunctionDescription ;
/**

  ClassValues holds information describing a class found with ICG.  The
  information includes the name and location of the class, properties of the
  class, and a vector list of the member data contained in the class.

  The class also provides print routines for each function in the io_src.

  @author Alexander S. Lin

  @date July 2012

 */

class ClassValues : public ConstructValues {
    public:
        ClassValues() ;

        ~ClassValues() ;

        /** Appends a single field to field_descripts */
        void addFieldDescription(FieldDescription * in_fdes) ;
        
        void addFunctionDescription(FunctionDescription* in_fdes);

        /** Appends a vector of fields to field_descripts.
            A vector comes from adding all inherited fields at once */
        void addInheritedFieldDescriptions(std::vector<FieldDescription *>,
         unsigned int class_offset, bool virtual_inherited) ;

        /** Gets the list of fields in this class */
        const std::vector<FieldDescription*>& getFieldDescriptions() {
            return field_descripts ;
        }

        const std::vector<FunctionDescription*>& getFunctionDescriptions()  {
            return function_descripts ;
        }

        void clearFieldDescription() ;

        void clearFunctionDescription() ;

        /** Appends an inherited class name to the list this class inherits from */
        void addInheritedClass( std::string class_name ) ;

        /** Gets the list of inherited classes */
        const std::vector<std::string>& getInheritedClasses() {
            return inherited_classes ;
        }

        void saveInheritAncestry( ClassValues * in_cv ) ;
        void setContainerClassForFields() ;
        void clearAmbiguousVariables() ;

        void clearInheritedClass() ;

        void setVirtualInherited(bool in_inh) ;
        bool isVirtualInherited() ;
        void setHasInitAttrFriend(bool in_val) ;
        bool getHasInitAttrFriend() ;
        void setPOD(bool in_val) ;
        bool isPOD() ;
        void setSize(unsigned int size) ;
        unsigned int getSize() ;
        void setAbstract(bool in_val) ;
        bool isAbstract() ;
        void setHasDefaultConstructor(bool in_val) ;
        bool getHasDefaultConstructor() ;
        void setHasPublicDestructor(bool in_val) ;
        bool getHasPublicDestructor() ;
        void setMangledTypeName( std::string in_val ) ;
        std::string getMangledTypeName() ;
        std::string getFullyQualifiedMangledTypeName(const std::string& delimiter = "::") ;
        std::string getFullyQualifiedNameIfEqual();
        void setCompat15(bool in_val) ;
        bool isCompat15() ;
        bool isInStandardNamespace();

        friend std::ostream & operator << (std::ostream & os , ClassValues & cv ) ;

    private:
        std::vector< FieldDescription * > field_descripts ;

        std::vector< FunctionDescription * > function_descripts ;

        std::map< std::string , FieldDescription * > field_name_to_info_map ;
        std::set< std::string > field_names_to_qualify ;

        /** List of classes we inherit from */
        std::vector< std::string > inherited_classes ;

        /** Map of all inherited classes. Counts how many times a class is inherited. */
        std::map< std::string , unsigned int > all_inherited_class_names_map ;

        /** Does this class a "friend class init_attr<class_name>" statement */
        bool has_init_attr_friend ;

        /** Is this class plain old data? */
        bool is_pod ;

        /** Size of class in bytes */
        unsigned int size ;

        /** Is this class abstract? */
        bool is_abstract ;

        /** Does this class have a default constructor? */
        bool has_default_constructor ;

        /** Does this class have a public destructor? */
        bool has_public_destructor ;

        /** Mangled type name. Templates will have a mangled_type_name.  */
        std::string mangled_type_name ;

        /** Generate Trick 15 compatible io_src code */
        bool compat15 ;

} ;

#endif
