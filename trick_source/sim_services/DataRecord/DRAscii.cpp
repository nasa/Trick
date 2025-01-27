 
/*
PURPOSE:
    (Data record in ascii format.)
PROGRAMMERS:
     (((Robert W. Bailey) (LinCom Corp) (3/96) (SES upgrades)
     ((Alex Lin) (NASA) (April 2009) (--) (c++ port)))
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "trick/DRAscii.hh"
#include "trick/command_line_protos.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/bitfield_proto.h"

Trick::DRAscii::DRAscii( std::string in_name, Trick::DR_Type dr_type ) : Trick::DataRecordGroup( in_name, dr_type ) {

    ascii_float_format = "%20.8g" ;
    ascii_double_format = "%20.16g" ;
    delimiter = ",";
    register_group_with_mm(this, "Trick::DRAscii") ;
}

int Trick::DRAscii::format_specific_header( std::fstream & out_st ) {
    out_st << " is in ASCII" << std::endl ;
    return(0) ;
}

/**
@details
-# If the #delimiter is not empty and not a comma then set the file extension to ".txt"
-# Else set the file extension to ".csv"
-# Allocate enough memory to hold #record_size of records in memory
-# Open the log file
   -# Return an error if the open failed.
-# Write out the title line of the log file.  The title line includes the names of
   recorded variables and the units of measurement separated by the #delimiter
-# Declare the recording group to the memory manager so that the group can be checkpointed
   and restored
*/
int Trick::DRAscii::format_specific_init() {

    unsigned int jj ;
    std::streampos before_write;

    /* Store log information in csv/txt file */
    if ( ! delimiter.empty()  &&  delimiter.compare(",") != 0 ) {
        file_name.append(".txt");
    } else {
        file_name.append(".csv");
    }

    /* Calculate a "worst case" for space used for 1 record. */
    writer_buff_size = record_size * rec_buffer.size();
    writer_buff = (char *)calloc(1 , writer_buff_size) ;

    /* This loop touches all of the memory locations in the allocation forcing the
       system to actually do the allocation */
    for ( jj= 0 ; jj < record_size * rec_buffer.size() ; jj += 1024 ) {
        writer_buff[jj] = 1 ;
    }
    writer_buff[record_size * rec_buffer.size() - 1] = 1 ;

    out_stream.open(file_name.c_str(), std::fstream::out | std::fstream::app ) ;
    if ( !out_stream || !out_stream.good() ) {
        message_publish(MSG_ERROR, "Can't open Data Record file %s.\n", file_name.c_str()) ;
        record = false ;
        return -1 ;
    }
    before_write = out_stream.tellp();
    // Write out the title line of the recording file
    /* Start with the 1st item in the buffer which should be "sys.exec.out.time" */
    out_stream << rec_buffer[0]->ref->reference ;
    if ( rec_buffer[0]->ref->attr->units != NULL ) {
        if ( rec_buffer[0]->ref->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
            out_stream << " {--}" ;
        } else {
            out_stream << " {" << rec_buffer[0]->ref->attr->units << "}" ;
        }
    }
    
    /* Write out specified recorded parameters */
    for (jj = 1; jj < rec_buffer.size() ; jj++) {
        out_stream << delimiter << rec_buffer[jj]->ref->reference ;

        if ( rec_buffer[jj]->ref->attr->units != NULL ) {
            if ( rec_buffer[jj]->ref->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
                out_stream << " {--}" ;
            } else {
                out_stream << " {" << rec_buffer[jj]->ref->attr->units << "}" ;
            }
        }
    }
    out_stream << std::endl ;
    total_bytes_written += out_stream.tellp() - before_write;
    return(0) ;
}

/**
@details
-# While there is data in memory that has not been written to disk
   -# Write out the time to a temporary #writer_buff
   -# Write out each of the other parameter values preceded by the delimiter to the temporary #writer_buff
   -# Write #writer_buff to the output file
-# Flush the output file stream
-# Return the number of bytes written
*/
int Trick::DRAscii::format_specific_write_data(unsigned int writer_offset) {
    unsigned int ii ;
    char *buf;

    buf = writer_buff ;

    /* Write out the first parameters (time) */
    copy_data_ascii_item(rec_buffer[0], writer_offset, buf );
    buf += strlen(buf);

    /* Write out all other parameters */
    for (ii = 1; ii < rec_buffer.size() ; ii++) {
        strcat(buf, delimiter.c_str() );
        buf += delimiter.length() ;
        copy_data_ascii_item(rec_buffer[ii], writer_offset, buf );
        buf += strlen(buf);
    }

    out_stream << writer_buff << std::endl ;

    /*! Flush the output */
    out_stream.flush() ;
    /*! +1 for endl */
    return(strlen(writer_buff) + 1) ;
}

/**
@details
-# Close the output file stream
*/
int Trick::DRAscii::format_specific_shutdown() {

    if ( inited ) {
        out_stream.close() ;
    }
    return(0) ;
}

int Trick::DRAscii::set_ascii_float_format( std::string in_float_format ) {
    ascii_float_format = in_float_format ;
    return(0) ;
}

int Trick::DRAscii::set_ascii_double_format( std::string in_double_format ) {
    ascii_double_format = in_double_format ;
    return(0) ;
}

int Trick::DRAscii::set_delimiter( std::string in_delimiter ) {
    delimiter = in_delimiter ;
    return(0) ;
}

int Trick::DRAscii::set_single_prec_only( bool in_single_prec_only ) {
    Trick::DataRecordGroup::set_single_prec_only(in_single_prec_only) ;
    if( single_prec_only ) {
        ascii_double_format = "%20.8g";
    }
    else {
        ascii_double_format = "%20.16g";
    }
    return(0) ;
}

int Trick::DRAscii::copy_data_ascii_item( Trick::DataRecordBuffer * DI, int item_num, char *buf ) {

    char *address = 0;

    unsigned long bf;
    int sbf;

    address = DI->buffer + (item_num * DI->ref->attr->size) ;

    size_t writer_buf_spare = writer_buff + writer_buff_size - buf;

    switch (DI->ref->attr->type) {
        case TRICK_CHARACTER:
            snprintf(buf, writer_buf_spare, "%c", *((char *) address));
            break;

        case TRICK_UNSIGNED_CHARACTER:
#if ( __linux__ | __sgi )
        case TRICK_BOOLEAN:
#endif
            snprintf(buf, writer_buf_spare, "%u", *((unsigned char *) address));
            break;

        case TRICK_STRING:
            snprintf(buf, writer_buf_spare, "%s", *((char **) address));
            break;

        case TRICK_SHORT:
            snprintf(buf, writer_buf_spare, "%d", *((short *) address));
            break;

        case TRICK_UNSIGNED_SHORT:
            snprintf(buf, writer_buf_spare, "%u", *((unsigned short *) address));
            break;

        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
#if ( __sun | __APPLE__ )
        case TRICK_BOOLEAN:
#endif
            snprintf(buf, writer_buf_spare, "%d", *((int *) address));
            break;

        case TRICK_UNSIGNED_INTEGER:
            snprintf(buf, writer_buf_spare, "%u", *((unsigned int *) address));
            break;

        case TRICK_LONG:
            snprintf(buf, writer_buf_spare, "%ld", *((long *) address));
            break;

        case TRICK_UNSIGNED_LONG:
            snprintf(buf, writer_buf_spare, "%lu", *((unsigned long *) address));
            break;

        case TRICK_FLOAT:
            snprintf(buf, writer_buf_spare, ascii_float_format.c_str() , *((float *) address));
            break;

        case TRICK_DOUBLE:
            snprintf(buf, writer_buf_spare, ascii_double_format.c_str() , *((double *) address));
            break;

        case TRICK_BITFIELD:
            sbf = GET_BITFIELD(address, DI->ref->attr->size, DI->ref->attr->index[0].start, DI->ref->attr->index[0].size);
            snprintf(buf, writer_buf_spare, "%d", sbf);
            break;

        case TRICK_UNSIGNED_BITFIELD:
            bf = GET_UNSIGNED_BITFIELD(address, DI->ref->attr->size, DI->ref->attr->index[0].start, DI->ref->attr->index[0].size);
            snprintf(buf, writer_buf_spare, "%lu", bf);
            break;

        case TRICK_LONG_LONG:
            snprintf(buf, writer_buf_spare, "%lld", *((long long *) address));
            break;

        case TRICK_UNSIGNED_LONG_LONG:
            snprintf(buf, writer_buf_spare, "%llu", *((unsigned long long *) address));
            break;
        default:
            break;
    }

    return(0) ;
}
