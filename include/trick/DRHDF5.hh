/*
PURPOSE:
    (Data Record HDF5 class.)
PROGRAMMERS:
    (((Derek Bankieris) (NASA) (2009)))
*/

#ifndef DRHDF5_HH
#define DRHDF5_HH

#include <pthread.h>
#include <string>
#include <vector>

#include "trick/DataRecordGroup.hh"

#ifdef HDF5
#ifndef SWIG
#ifndef TRICK_ICG
#include "hdf5.h"
#include "H5PTpublic.h"
#endif
#endif
#endif

#ifdef SWIG
%feature("compactdefaultargs","0") ;
%feature("shadow") Trick::DRHDF5::DRHDF5(std::string in_name) %{
    def __init__(self, *args):
        this = $action(*args)
        try: self.this.append(this)
        except: self.this = this
        this.own(0)
        self.this.own(0)
%}
#endif

namespace Trick {

    /**
      The DRHDF5 recording format is an industry conforming HDF5 formatted file.  Files written in this format are named
      log_<group_name>.h5.  The contents of this file type are readable by the Trick Data Products packages from
      Trick 07 to the current version.  The contents of the file are binary and is not included here.  The HDF5 layout
      of the file follows.

      @verbatim
GROUP "/" {
    GROUP "header" {
        DATASET "byte_order" {
            "little_endian"
        }
        DATASET "file_names" {
            "param_1_file_name", "param_2_file_name", etc...
        }
        DATASET "param_names" {
            "param_1_name", "param_2_name", etc...
        }
        DATASET "param_types" {
            "param_1_type", "param_2_type", etc...
        }
        DATASET "param_units" {
            "param_1_units", "param_2_units", etc...
        }
    }
    DATASET "parameter #1" {
        value1 , value2 , value3 , etc...
    }
    DATASET "parameter #2" {
        value1 , value2 , value3 , etc...
    }
    .
    .
    .
    DATASET "parameter #n" {
        value1 , value2 , value3 , etc...
    }
}
    @endverbatim
*/
    class DRHDF5 : public Trick::DataRecordGroup {

        public:

            #ifndef SWIG
            /**
             @brief DRHDF5 default constructor.
             */
            DRHDF5() {}
            #endif
            ~DRHDF5() {}

            /**
             @brief @userdesc Create a new HDF5 data recording group.
             @par Python Usage:
             @code <my_drg> = trick.DRHDF5("<in_name>") @endcode
             @copydoc Trick::DataRecordGroup::DataRecordGroup(string in_name)
             */
            DRHDF5( std::string in_name, Trick::DR_Type dr_type = Trick::DR_Type::DR_Type_HDF5) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_header
             */
            virtual int format_specific_header(std::fstream & outstream) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_init
             */
            virtual int format_specific_init() ;

            /**
             @brief Override the write_data function in DAtaRecordGroup
             @returns always 0
            */
            virtual int write_data(bool must_write = false) ;

            /**
             @copybrief Trick::DataRecordGroup::format_specific_write_data
             */
            virtual int format_specific_write_data(unsigned int writer_offset) ;

            /**
             @copybrief Trick::DataRecordGroup::shutdown
             */
            virtual int format_specific_shutdown() ;

        protected:

#ifdef HDF5
            /**
             The HDF5 file handle.
             */
            hid_t file;  // trick_io(**)
            /**
             Root group and header group in the HDF5 file.
             */
            hid_t root_group, header_group;  // trick_io(**)
            
            /** 
             Parameter names array to be used in the HDF5 packet table.
             Each array item  is a string of the parameter name that is
             the copy of the reference name.
             This is needed so when the dataset is closed, the reference
             name in rec_buffer is still valid and won't cause double 
             deleting when variables are removed from rec_buffer.
             */ 
            char** param_names;  // trick_io(**)
         
            /**
             The dataset ids for each parameter.
             */
            hid_t* param_dataset_ids; // trick_io(**)
#endif

    } ;

} ;

#endif
