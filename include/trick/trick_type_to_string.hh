/*************************************************************************
PURPOSE: (Trick TMM Alloc W/ Args)
ICG: (No)
LIBRARY DEPENDENCY:
    (
    (trick_type_to_string.cc)
    )
**************************************************************************/

#ifndef __TRICK_TYPE_TO_STRING_HH__
#define __TRICK_TYPE_TO_STRING_HH__

#ifdef TRICK_HAVE_ABI
#include <cxxabi.h>
#endif

#include <string>

#ifdef TRICK_HAVE_ABI
template<typename T>
struct TrickTypeToString { 
    inline const std::string getName() {
        const std::typeinfo& type_info = typeid(T);
        int status = 0;
        char* demangeled_name = abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status);
        if(status == 0)
        {
            return std::string(demangled_name);
        }
        
        return std::string("";)
    }
};
#else
/**
 * @tparam T: Type to return string representation of via getName() function.
 * @details: Basic version of TrickTypeToString with no getName() function. 
 * ICG generates specializations after parsing user header files and writes
 * them to build/trick/trick_type_to_string_ext.hh 
 */

template<typename T>
struct TrickTypeToString { };
#endif


#endif //__TRICK_TYPE_TO_STRING_HH__