/*
   PURPOSE: (Returns value of parameter referenced by right side parameter.)

   ASSUMPTIONS AND LIMITATIONS: ((Handles only the following 'C' types: char, unsigned char, char*, int, unsigned int,
   long, unsinged long, short, unsigned short, float, double))

   PROGRAMMERS: (((Alex Lin) (NASA) (9/00))) */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef __Lynx__
#include <wchar.h>
#endif

#include "trick/reference.h"
#include "trick/parameter_types.h"
#include "trick/mm_error.h"
#include "trick/bitfield_proto.h"

int ref_to_value(REF2 * R, V_DATA * V)
{

    char *cp;
    unsigned char *ucp;
    char **cpp;
#ifndef __Lynx__
    wchar_t **wcpp;
#endif
    short *shp;
    int *ip;
    long *lp;
    float *fp;
    double *dp;
    long long *llp;
    int bf;
    unsigned int ubf;
    int type;

    char *address = 0;

    /*
     * This function is called from within the YACC parser.
     * This function relies on the REF data structure to be filled --
     * YACC fills part of the REF struct and ref_attributes() fills
     * the rest.
     *
     * This function will get the value of the parameter pointed by REF and
     * returns the value in param_right_ret.
     */

    /* I'm trying to keep from casting integer values to doubles and back again, esp.  for long longs, for they could
       get changed in all of the conversions */

//    if (R->deprecated) {
//        /* Removed input_processor as a parameter to ref_to_value for simplification but that means we no longer know
//           what I->print_deprecated is. Heck with it -- always print out this warning here. */
//        //send_hs(stderr, "\n[33mWARNING: Deprecated variable referenced:[00m\n"
//        //        "%s\nReturning value = 0.\n", R->reference);
//        V->type = R->attr->type;
//        V->value.ll = 0;
//        return (MM_NO_ERROR);
//    }

    type = R->attr->type;
    /* treat C++ bool reference as an unsigned char */
    if (type == TRICK_BOOLEAN) {
        type = TRICK_UNSIGNED_CHARACTER;
    }
    address = R->address;

//    if ((R->num_index < R->attr->num_index - 1) || ((R->address_req == 0) && (R->num_index < R->attr->num_index))) {
//        type = VOID_PTR;
//    }

    V->type = type;

    switch (type) {
        case TRICK_VOID_PTR:
            //if ((R->num_index < R->attr->num_index - 1) || ((R->address_req == 0) && (R->num_index < R->attr->num_index))) {
            //    V->value.vp = address;
            //} else {
                V->value.vp = *(void **) address;
            // }
            break;

        case TRICK_CHARACTER:
            cp = address;
            V->value.c = *cp;
            break;

        case TRICK_UNSIGNED_CHARACTER:
#if ( __linux__ | __sgi )
        case TRICK_BOOLEAN:
#endif
            ucp = (unsigned char *) address;
            V->value.c = (char) *ucp;
            break;

        case TRICK_STRING:
            cpp = (char **) address;
            V->value.cp = *cpp;
            break;

        case TRICK_WSTRING:
#ifndef __Lynx__
            wcpp = (wchar_t **) address;
            V->value.wcp = *wcpp;
#endif
            break;
        case TRICK_SHORT:
        case TRICK_UNSIGNED_SHORT:
            shp = (short *) address;
            V->value.s = *shp;
            V->type = TRICK_SHORT;
            break;

        case TRICK_INTEGER:
        case TRICK_ENUMERATED:
        case TRICK_UNSIGNED_INTEGER:
#if ( __sun | __APPLE__ )
        case TRICK_BOOLEAN:
#endif
            ip = (int *) address;
            V->value.i = *ip;
            break;

        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
            lp = (long *) address;
            V->value.ll = (long long) *lp;
            break;

        case TRICK_FLOAT:
            fp = (float *) address;
            V->value.f = *fp;
            break;

        case TRICK_DOUBLE:
            dp = (double *) address;
            V->value.d = *dp;
            V->type = TRICK_DOUBLE;
            break;

        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            llp = (long long *) address;
            V->value.ll = *llp;
            V->type = TRICK_LONG_LONG;
            break;

        case TRICK_BITFIELD:
            bf = GET_BITFIELD(address, R->attr->size, R->attr->index[0].start, R->attr->index[0].size);
            if (R->attr->size == sizeof(int)) {
            } else if (R->attr->size == sizeof(short)) {
            } else if (R->attr->size == sizeof(char)) {
            } else {
                // ERROR
            }
            // TODO: always integer? or should we return short or char for those types?
            V->value.i = bf;
            V->type = TRICK_INTEGER;
            break;

        case TRICK_UNSIGNED_BITFIELD:
            ubf = GET_UNSIGNED_BITFIELD(address, R->attr->size, R->attr->index[0].start, R->attr->index[0].size);
            // TODO: always integer? or should we return short or char for those types?
            V->value.i = (int) ubf;
            V->type = TRICK_UNSIGNED_INTEGER;
            break;

    }

    return (MM_OK);

}
