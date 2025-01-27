#ifndef PARAMETER_TYPES_H
#define PARAMETER_TYPES_H

/*
    PURPOSE: ( Defines the Trick data-types, used in ATTRIBUTES descriptions of
               variables, found in io_src files.)
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The TRICK_TYPE enumeration represents the Trick data types.
 */
    typedef enum {
        TRICK_VOID               =   0, /* No type */
        TRICK_CHARACTER          =   1, /* (char) */
        TRICK_UNSIGNED_CHARACTER =   2, /* (unsigned char) */
        TRICK_STRING             =   3, /* (char *) */
        TRICK_SHORT              =   4, /* (short) */
        TRICK_UNSIGNED_SHORT     =   5, /* (unsigned short) */
        TRICK_INTEGER            =   6, /* (int) */
        TRICK_UNSIGNED_INTEGER   =   7, /* (unsigned int) */
        TRICK_LONG               =   8, /* (long) */
        TRICK_UNSIGNED_LONG      =   9, /* (unsigned long) */
        TRICK_FLOAT              =  10, /* (float) */
        TRICK_DOUBLE             =  11, /* (double) */
        TRICK_BITFIELD           =  12, /* (signed int : 1) */
        TRICK_UNSIGNED_BITFIELD  =  13, /* (unsigned int : 1) */
        TRICK_LONG_LONG          =  14, /* (long long) */
        TRICK_UNSIGNED_LONG_LONG =  15, /* (long long) */
        TRICK_FILE_PTR           =  16, /* (file *) */
        TRICK_BOOLEAN            =  17, /* (C++ boolean) */
        TRICK_WCHAR              =  18, /* (wchar_t) */
        TRICK_WSTRING            =  19, /* (wchar_t *) */
        TRICK_VOID_PTR           =  20, /* an arbitrary address */
        TRICK_ENUMERATED         =  21, /* User defined type (enumeration) */
        TRICK_STRUCTURED         =  22, /* User defined type (struct/class) */
        TRICK_OPAQUE_TYPE        =  23, /* User defined type (where type details are as yet unknown) */
        TRICK_STL                =  24, /* Standard template library type */
        TRICK_NUMBER_OF_TYPES
    } TRICK_TYPE ;

const char* trickTypeCharString( TRICK_TYPE type, const char* name);

/* here for backwards compatibility */
#define TRICK_USER_DEFINED_TYPE TRICK_OPAQUE_TYPE

/* Define int_64t and uint_64t depending on compiler options */
#if __linux__
#  include <stdint.h>
#  include <sys/types.h>
#  if __WORDSIZE == 64
#    define TRICK_INT64 TRICK_LONG
#    define TRICK_UINT64 TRICK_UNSIGNED_LONG
#  else
#    define TRICK_INT64 TRICK_LONG_LONG
#    define TRICK_UINT64 TRICK_UNSIGNED_LONG_LONG
#  endif
#endif

#if __APPLE__
   /* only checked the 32bit version of the OS, have no access to 64 bit version */
#  include <stdint.h>
#  include <sys/types.h>
#  define TRICK_INT64 TRICK_LONG_LONG
#  define TRICK_UINT64 TRICK_UNSIGNED_LONG_LONG
#endif

#if __QNX__
#  include <stdint.h>
#  include <sys/types.h>
#  if __INT_BITS__ == 64
#    define TRICK_INT64 TRICK_LONG
#    define TRICK_UINT64 TRICK_UNSIGNED_LONG
#  else
#    define TRICK_INT64 TRICK_LONG_LONG
#    define TRICK_UINT64 TRICK_UNSIGNED_LONG_LONG
#  endif
#endif

#if __Lynx__
#  include <stdint.h>
#  include <sys/types.h>
#  define TRICK_INT64 TRICK_LONG_LONG
#  define TRICK_UINT64 TRICK_UNSIGNED_LONG_LONG
#endif

#if __sgi
#  include <sgidefs.h>
#  include <sys/types.h>
#  if _MIPS_SZLONG == 64
#    define TRICK_INT64 TRICK_LONG
#    define TRICK_UINT64 TRICK_UNSIGNED_LONG
#  else
#    define TRICK_INT64 TRICK_LONG_LONG
#    define TRICK_UINT64 TRICK_UNSIGNED_LONG_LONG
#  endif
#endif

#ifdef __cplusplus
}
#endif
#endif
