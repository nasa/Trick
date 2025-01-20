
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <iomanip>
#include <sstream>

#include "llvm/Support/CommandLine.h"

#include "PrintFileContentsBase.hh"
#include "FieldDescription.hh"
#include "ClassValues.hh"
#include "EnumValues.hh"

extern llvm::cl::opt< bool > global_compat15 ;

PrintFileContentsBase::PrintFileContentsBase() {}

// provide empty default implementation of these routines.
void PrintFileContentsBase::printClassMapHeader(std::ostream & ostream, std::string function_name ) {}
void PrintFileContentsBase::printClassMap(std::ostream & ostream, ClassValues * cv) {}
void PrintFileContentsBase::printClassMapFooter(std::ostream & ostream) {}

void PrintFileContentsBase::printEnumMapHeader(std::ostream & ostream, std::string function_name ) {}
void PrintFileContentsBase::printEnumMap(std::ostream & ostream, EnumValues * ev) {}
void PrintFileContentsBase::printEnumMapFooter(std::ostream & ostream) {}

void PrintFileContentsBase::print_units_map(std::ostream & ostream, ClassValues * cv ) {
    const std::string name = cv->getFullyQualifiedMangledTypeName("__");
    ostream << "struct UnitsMap" << name << " {\n" ;

    auto fields = getPrintableFields(*cv);
    if (fields.size()) {
        ostream << "    UnitsMap" << name << "() {\n"
                << "        Trick::UnitsMap* units_map_ptr = Trick::UnitsMap::units_map();\n" ;
        for (auto& field : fields) {
            ostream << "        units_map_ptr->add_param(\"" ;
            cv->printContainerClasses(ostream, "__");
            ostream << cv->getName() << "_" << field->getName() << "\", \"" << field->getUnits() << "\") ;\n" ;
        }
        ostream << "    }\n" ;
    }

    ostream << "} um" << name << ";\n\n" ;
}

/* Utility routines for printing */
void PrintFileContentsBase::print_open_extern_c(std::ostream & ostream) {
    ostream << "extern \"C\" {\n\n" ;
}

void PrintFileContentsBase::print_close_extern_c(std::ostream & ostream) {
    ostream << "\n} //extern \"C\"\n\n" ;
}

/* internal function determines if a particular field is printable based
   on access to the field and the presense of init_attr friends.
*/
bool PrintFileContentsBase::isPrintable( ClassValues * c , FieldDescription * fdes , unsigned int ioMask) {
    if ( !(fdes->getIO() & ioMask) || !fdes->getTypeName().compare("void") || !fdes->getEnumString().compare("TRICK_VOID")) {
        return false;
    }
    if ( fdes->isSTL() and fdes->getNumDims() ) {
        return false;
    }
    /**
     * It is not possible to take the address of a reference as the referred-to variable's address
     * is always returned. As such, the init_attr function cannot obtain the absolute address for
     * static reference variables.
     */
    if ( fdes->isStatic() && fdes->isReference() ) {
        return false;
    }
    if ( fdes->getAccess() == clang::AS_public || (!fdes->isStatic() && !global_compat15 && !c->isCompat15())) {
        return true;
    }
    return c->getHasInitAttrFriend() && ( !fdes->isInherited() || fdes->getAccess() == clang::AS_protected ) ;
}

std::vector<FieldDescription*> PrintFileContentsBase::getPrintableFields(ClassValues& classValues, unsigned int ioMask) {
    std::vector<FieldDescription*> results;
    for (auto& field : classValues.getFieldDescriptions()) {
        if (isPrintable(&classValues, field)) {
            results.push_back(field);
        }
    }
    return results;
}