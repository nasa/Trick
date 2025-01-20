
#ifndef PRINTFILECONTENTSBASE_HH
#define PRINTFILECONTENTSBASE_HH

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>


class ConstructValues ;
class ClassValues ;
class EnumValues ;
class FieldDescription ;

/**

  This class prints Trick 10 style io_src code.  Given a vector of class information,
  this class determines the io_src code file names, excluding files in system directories,
  and writes out the attributes.

  @author Alexander S. Lin

  @date July 2012

 */

class PrintFileContentsBase {
    public:
        PrintFileContentsBase() ;

        virtual void printIOHeader(std::ostream & ostream, std::string header_file_name) = 0 ;
        virtual void printClass(std::ostream & ostream, ClassValues * cv) = 0 ;
        virtual void printEnum(std::ostream & ostream, EnumValues * ev) = 0 ;

        // these routines provide default empty implementation
        virtual void printClassMapHeader(std::ostream & ostream, std::string function_name ) ;
        virtual void printClassMap(std::ostream & ostream, ClassValues * cv) ;
        virtual void printClassMapFooter(std::ostream & ostream) ;

        virtual void printEnumMapHeader(std::ostream & ostream, std::string function_name ) ;
        virtual void printEnumMap(std::ostream & ostream, EnumValues * ev) ;
        virtual void printEnumMapFooter(std::ostream & ostream) ;


        /* gets a vector of fields that can be printed */
        std::vector<FieldDescription*> getPrintableFields(ClassValues& classValues, unsigned int ioMask = 0xFFFFFFF);

    protected:
        /** Prints the io_src_allocate function */
        virtual void print_units_map(std::ostream & ostream, ClassValues * cv) ;

        /** Prints "extern \"C\" {" */
        void print_open_extern_c(std::ostream & ostream) ;

        /** Prints "} // extern C" */
        void print_close_extern_c(std::ostream & ostream) ;

        /* internal function determines if a particular field is printable */
        bool isPrintable(ClassValues * c , FieldDescription *fdes , unsigned int ioMask = 0xFFFFFFF) ;

} ;

#endif
