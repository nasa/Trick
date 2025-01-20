
#ifndef PRINTATTRIBUTES_HH
#define PRINTATTRIBUTES_HH

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>

#include "Utilities.hh"

namespace clang {
    class CompilerInstance ;
}

class ConstructValues ;
class ClassValues ;
class EnumValues ;
class PrintFileContentsBase ;
class HeaderSearchDirs ;
class CommentSaver ;
class PrintFileContentsBase ;

/**

  This class creates a PrintFileContentsBase class for each include file that we
  have parsed.  It then adds all of the classes and enums into PrintFileContentBase
  classes keyed by the files they were found.

  @author Alexander S. Lin

  @date July 2012

 */

class PrintAttributes {
    public:
        PrintAttributes( int attr_version , HeaderSearchDirs & hsd , CommentSaver & cs ,
         clang::CompilerInstance & in_ci, bool force , bool sim_services, std::string output_dir ) ;

        /** Adds construct names to ignore from TRICK_ICG_IGNORE_TYPES environment variable */
        void addIgnoreTypes() ;

        /** Prints all of the processed classes and enumerations */
        virtual void createMapFiles() ;
        virtual void closeMapFiles() ;

        /** Create makefile for IO files */
        virtual void printIOMakefile() ;

        /** Prints list of files that contain ICG:(No) in the Trick header */
        virtual void printICGNoFiles() ;

        /** Prints a class to the io_src file */
        virtual void printClass( ClassValues * in_class) ;

        /** Prints an enum to the io_src file */
        virtual void printEnum( EnumValues * in_enum) ;

        /** Prints a class to the XML resource file */
        void printSieClass( ClassValues * cv ) ;

        /** Prints an enum to the XML resource file */
        void printSieEnum( EnumValues * ev ) ;

        bool isHeaderExcluded(const std::string& header, bool exclude_ext_libs = true);

        void markHeaderAsVisited(const std::string& header);
        
        void writeTrickTypeToStructHeader();

        void addTypedefClass(std::string typedef_class);

        void setUseTMMAllocArgs(bool use_tmm_alloc_args);
    protected:

        const bool verboseBuild = (getenv("TRICK_VERBOSE_BUILD") || getenv("VERBOSE"));
        const std::string skipping = color(SKIP, "ICG Skip   ");

        /** Directory to put class and enum map files */
        std::string map_dir ;

        /** Output stream to be used to print individual io_src files */
        std::ofstream outfile ;

        /** Output stream to be used for class_map */
        std::ofstream class_map_outfile ;

        /** Output stream to be used for enum_map */
        std::ofstream enum_map_outfile ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** Comments for all of the header files */
        CommentSaver & cs ;

        /** Compiler instance */
        clang::CompilerInstance & ci ;

        /** Version specific attributes printer */
        PrintFileContentsBase * printer ;

        /** Force all io_src files to be written */
        bool force ;

        /** We are processing sim_services */
        bool sim_services_flag ;

        /** Control if files related to tmm_alloc_args are generated */
        bool use_tmm_alloc_args ;

        /** We are specifying an output directory for all files */
        std::string output_dir ;

        bool openIOFile(const std::string& header_file_name) ;

        bool isIOFileOutOfDate(std::string header_file_name, std::string io_file_name ) ;
        bool hasBeenProcessed(EnumValues& enumValues);
        bool hasBeenProcessed(ClassValues& classValues);
        bool hasBeenProcessed(const std::string& name, std::set<std::string>& processedSet);
        bool isIgnored(ConstructValues& constructValues);

        std::set<std::string> getEmptyFiles();

        /** Determines the io_file_name based on the given header file name
            @param header_file_name = full path to header file
            @return string of full path to io_src file name.
          */
        std::string createIOFileName(std::string header_file_name) ;

        /** set of all visited files */
        std::set< std::string > visited_files ;

        /** map of all io_files we processed */
        std::map< std::string , std::string > all_io_files ;

        /** set of external library header files */
        std::set< std::string > ext_lib_io_files ;

        /** map of open files to the out of date io_src file */
        std::map< std::string , std::string > out_of_date_io_files ;

        /** List of files that have ICG: No */
        std::vector< std::string > icg_no_files ;

        /** set of types from the TRICK_ICG_IGNORE_TYPES environment variable */
        std::set< std::string > global_ignore_types ;

        /** map of ignored types sorted by file */
        std::map< std::string , std::set< std::string > > ignored_types ;
        /** map of processed classes sorted by file */
        std::map< std::string , std::set< std::string > > processed_classes ;
        /** map of processed enums sorted by file */
        std::map< std::string , std::set< std::string > > processed_enums ;
        
        std::set<std::string> typedef_classes;


} ;

#endif
