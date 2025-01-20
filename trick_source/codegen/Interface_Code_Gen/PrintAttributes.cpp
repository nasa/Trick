
#include <iostream>
#include <fstream>
#include <sstream>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>

#include <limits.h>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileManager.h"

#include "PrintAttributes.hh"
#include "PrintFileContentsBase.hh"
#include "PrintFileContents10.hh"
#include "FieldDescription.hh"
#include "FunctionDescription.hh"
#include "HeaderSearchDirs.hh"
#include "CommentSaver.hh"
#include "ClassValues.hh"
#include "EnumValues.hh"
#include "Utilities.hh"

PrintAttributes::PrintAttributes(int in_attr_version , HeaderSearchDirs & in_hsd ,
  CommentSaver & in_cs , clang::CompilerInstance & in_ci, bool in_force , bool in_sim_services_flag ,
  std::string in_output_dir ) :
   hsd(in_hsd) ,
   cs(in_cs) ,
   ci(in_ci) ,
   force(in_force) ,
   sim_services_flag( in_sim_services_flag ) ,
   output_dir( in_output_dir )
{
    printer = new PrintFileContents10() ;
}

void PrintAttributes::addIgnoreTypes() {

    char * env_var_contents = getenv("TRICK_ICG_IGNORE_TYPES") ;

    if( env_var_contents != NULL ) {
        std::string s = std::string(env_var_contents) ;
        std::stringstream ss(s);
        std::string item;
        while(std::getline(ss, item, ';')) {
            item = trim(item) ;
            if ( ! item.empty() ) {
                global_ignore_types.insert(item) ;
            }
        }
    }
}

/**
@details

In order for us to create an io_src file for an include the header must:

1.  Reside in a user directory ( not system dirs like /usr/include
2.  Not reside in an excluded directory
3.  Not have ICG: (No) present in the Trick header

If the header file meets all of these conditions, a PrintFileContentsBase
instance is created for that file name.

 */

bool PrintAttributes::isIOFileOutOfDate(std::string header_file_name, std::string io_file_name) {
    struct stat header_stat ;
    struct stat io_stat ;
    int ret ;

    stat( header_file_name.c_str() , &header_stat ) ;
    ret = stat( io_file_name.c_str() , &io_stat ) ;

    if ( ret == 0 ) {
        // If the force flag is true, return that the io file is out of date.
        // Otherise test if the header is newer than the io file.
        return ( force || (header_stat.st_mtime > io_stat.st_mtime) ) ;
    }
    return true ;
}

static void _mkdir(const char *dir) {
    char tmp[PATH_MAX];
    char *p = NULL;
    struct stat buf ;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            if ( stat( tmp , &buf ) != 0 ) {
                int returnValue = mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
                if (returnValue) {
                    std::cerr << bold(color(ERROR, "Error")) << "      Unable to create " << quote(bold(tmp)) << " for writing: " << strerror(errno) << std::endl;
                    return ;
                }
            }
            *p = '/';
        }
    if ( stat( tmp , &buf ) != 0 ) {
        int returnValue = mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
        if (returnValue) {
            std::cerr << bold(color(ERROR, "Error")) << "      Unable to create " << quote(bold(tmp)) << " for writing: " << strerror(errno) << std::endl;
            return ;
        }
    }
}

bool PrintAttributes::openIOFile(const std::string& header_file_name) {
    /**
     * There are a lot of conditions to be met in order to open an IO file.  We store the headers
     * we have visited so we don't have to retest the them each time a new class/enum is processed.
     */
    if (visited_files.find(header_file_name) != visited_files.end()) {
        // We have visited this header before. If there is a valid name, append to the existing IO file.
        if (out_of_date_io_files.find(header_file_name) != out_of_date_io_files.end()) {
            outfile.open(out_of_date_io_files[header_file_name].c_str(), std::fstream::app);
            return true ;
        }
        return false;
    }

    visited_files.insert(header_file_name) ;

    if (isHeaderExcluded(header_file_name)) {
        return false;
    }

    // map the header to its IO file
    char* realPath  = almostRealPath(header_file_name.c_str());
    const std::string io_file_name = createIOFileName(realPath) ;
    all_io_files[header_file_name] = io_file_name ;

    // make the parent directories
    char* name = strdup(io_file_name.c_str());
    _mkdir(dirname(name));
    free(name);

    // no further processing is required if it's not out of date
    if (!isIOFileOutOfDate(realPath, io_file_name)) {
        free(realPath);
        return false;
    }
    free(realPath);

    // add it to the map of out of date IO files
    out_of_date_io_files[header_file_name] = io_file_name ;

    // write header information
    std::cout << color(INFO, "Writing    ") << out_of_date_io_files[header_file_name] << std::endl;
    outfile.open(out_of_date_io_files[header_file_name].c_str());
    printer->printIOHeader(outfile, header_file_name);
    if (!cs.hasTrickHeader(header_file_name) ) {
        std::cout << bold(color(WARNING, "Warning    ") + header_file_name) << std::endl
            << "           No Trick header comment found" << std::endl;
    }

    return true ;
}

/** Determines the io_file_name based on the given header file name */
std::string PrintAttributes::createIOFileName(std::string header_file_name) {
    std::string dir_name ;
    std::string base_name ;
    std::string io_file_name ;
    size_t found ;
    char cwd[1024] ;
    char * temp_str = strdup(header_file_name.c_str()) ;

    base_name = std::string(basename(temp_str)) ;
    found = base_name.find_last_of(".") ;
    base_name = std::string("io_") + base_name.substr(0,found) + std::string(".cpp") ;

    dir_name = std::string(dirname(temp_str)) ;
    if ( ! dir_name.compare(".") ) {
        dir_name = std::string(getcwd(cwd, 1024)) ;
    } else if ( ! dir_name.compare(0, 1, ".") ) {
        dir_name = std::string(getcwd(cwd, 1024)) + std::string("/") + dir_name ;
    }

    free(temp_str) ;

    if ( hsd.isPathInUserDir( dir_name ) ) {
        if ( ! output_dir.empty() ) {
            io_file_name = output_dir + "/" + base_name ;
        } else {
            // Put all of the sim_services io_files in ${TRICK_HOME}/trick_source/sim_services/include/io_src unless
            // it is in er7_utils.  The er7_utils io_files have duplicate file names so the overwrite each other
            // leave those in their respective directories.
            if ( sim_services_flag ) {
                if ( dir_name.length() >= 8 and ! dir_name.compare(dir_name.size() - 8 , dir_name.size() , "/include" )) {
                    if ( dir_name.length() < 13 or dir_name.compare(dir_name.size() - 13 , dir_name.size() , "trick/include" )) {
                        dir_name.replace(dir_name.size() - 8 , dir_name.size() , "") ;
                    }
                }
                if ( dir_name.find("er7_utils") == std::string::npos ) {
                    io_file_name = std::string(getenv("TRICK_HOME")) + "/trick_source/sim_services/include/io_src/" + base_name ;
                } else {
                    io_file_name = dir_name + "/io_src/" + base_name ;
                }
            } else {
                //TODO: only use build directory if we are ICG'ing a sim
                // All files go into a build directory based in the current directory.
                io_file_name =  std::string("build") + dir_name + "/" + base_name ;
            }
        }
        return io_file_name ;
    }

    return std::string() ;
}

void PrintAttributes::printClass( ClassValues * cv ) {
    ClassValues& classValues = *cv;

    if (classValues.isNameOrFileNameEmpty()) {
        return;
    }

    if (hasBeenProcessed(classValues)) {
        return;
    }

    if (isIgnored(classValues)) {
        return;
    }

    const std::string& fileName = classValues.getFileName();
    if (openIOFile(fileName)) {
        printer->printClass(outfile, cv);
        outfile.close();
        printSieClass(cv);
    }

    if (!isHeaderExcluded(fileName, false)) {
         printer->printClassMap(class_map_outfile, cv);
    }
}

void PrintAttributes::printEnum(EnumValues* ev) {
    EnumValues& enumValues = *ev;

    if (enumValues.isNameOrFileNameEmpty()) {
        return;
    }

    if (hasBeenProcessed(enumValues)) {
        return;
    }

    if (isIgnored(enumValues)) {
        return;
    }

    const std::string& fileName = enumValues.getFileName();
    if (openIOFile(fileName)) {
        printer->printEnum(outfile, ev) ;
        outfile.close() ;
        printSieEnum(&enumValues) ;
    }
    
    if (!isHeaderExcluded(fileName, false)) {
         printer->printEnumMap(enum_map_outfile, ev);
    }
}


void PrintAttributes::printSieClass( ClassValues * cv ) {
    std::string xmlFileName;
    if(sim_services_flag) {
    #ifdef EXTERNAL_BUILD
        xmlFileName = output_dir + "/sim_services_classes.resource";
    #else
        xmlFileName = std::string(getenv("TRICK_HOME")) + "/share/trick/xml/sim_services_classes.resource";
    #endif
    } else {
        xmlFileName = "build/classes.resource";
    }
    std::ofstream ostream(xmlFileName, std::ofstream::app);
    ostream << "  <class name=\"" << sanitize(cv->getFullyQualifiedMangledTypeName("__")) << "\">\n";
    for (FieldDescription* fdes : printer->getPrintableFields(*cv)) {
        std::string type = fdes->getFullyQualifiedMangledTypeName("__");
        std::replace(type.begin(), type.end(), ':', '_');
        ostream << "    <member\n"
                << "      name=\"" << fdes->getName() << "\"\n"
                << "      type=\"" << replace_special_chars(type) << "\"\n"
                << "      io_attributes=\"" << fdes->getIO()  << "\"\n"
                << "      units=\"";
        if(fdes->isDashDashUnits()) {
            ostream <<  "--\"" ;
        } else {
            ostream << fdes->getUnits()  << "\"" ;
        }
        std::string description = fdes->getDescription();
        if(!description.empty()) {
            ostream << "\n      description=\"" << replace_special_chars(description) << "\"";
        }
        ostream << ">\n" ;
        for(int i = 0; i < fdes->getNumDims(); i++) {
            ostream
                << "      <dimension>" << (fdes->getArrayDim(i) != -1 ? fdes->getArrayDim(i) : 0) << "</dimension>\n";
        }
        ostream << "    </member>\n";
    }
    ostream << "  </class>\n" << std::endl;
    ostream.close();
}

void PrintAttributes::printSieEnum( EnumValues * ev ) {
    std::string xmlFileName;
    if(sim_services_flag) {
    #ifdef EXTERNAL_BUILD
        xmlFileName = output_dir + "/sim_services_classes.resource";
    #else
        xmlFileName = std::string(getenv("TRICK_HOME")) + "/share/trick/xml/include/sim_services_classes.resource";
    #endif

    } else {
        xmlFileName = "build/classes.resource";
    }
    std::ofstream ostream(xmlFileName, std::ofstream::app);
    ostream << "  <enumeration name=\"" << sanitize(ev->getFullyQualifiedTypeName("__")) << "\">\n";
    for(EnumValues::NameValuePair nvp : ev->getFullyQualifiedPairs()) {
        ostream << "    <pair label =\"" << nvp.first << "\" value=\"" << nvp.second << "\"/>\n";
    }
    ostream << "  </enumeration>\n" << std::endl;
    ostream.close();
}

void PrintAttributes::createMapFiles() {
    struct stat buf ;
    std::string class_map_function_name ;
    std::string enum_map_function_name ;

    if ( sim_services_flag ) {
#ifdef EXTERNAL_BUILD
        map_dir = output_dir ;
#else
        map_dir = "trick_source/sim_services/include/io_src" ;
#endif
        class_map_function_name = "populate_sim_services_class_map" ;
        enum_map_function_name = "populate_sim_services_enum_map" ;
    } else {
        map_dir = "build" ;
        class_map_function_name = "populate_class_map" ;
        enum_map_function_name = "populate_enum_map" ;
    }

    if ( stat( map_dir.c_str() , &buf ) != 0 ) {
        if ( mkdir( map_dir.c_str() , 0755 ) != 0 ) {
            // dir does not exist and cannot make the directory.
            std::cout << bold(color(WARNING, "Warning")) << "    Unable to create " << quote(bold(map_dir)) << " for writing" << std::endl ;
        }
    }

    // Write processed code to temporary files
    class_map_outfile.open(std::string(map_dir + "/.class_map.cpp").c_str()) ;
    printer->printClassMapHeader(class_map_outfile, class_map_function_name ) ;
    enum_map_outfile.open(std::string(map_dir + "/.enum_map.cpp").c_str()) ;
    printer->printEnumMapHeader(enum_map_outfile, enum_map_function_name ) ;
}

void PrintAttributes::closeMapFiles() {
    printer->printClassMapFooter(class_map_outfile) ;
    class_map_outfile.close() ;

    printer->printEnumMapFooter(enum_map_outfile) ;
    enum_map_outfile.close() ;

    // If we wrote any new io_src files, move the temporary class and enum map files to new location
    if ( out_of_date_io_files.size() > 0 ) {
        std::ifstream class_map(std::string(map_dir + "/.class_map.cpp").c_str()) ;
        std::ifstream enum_map(std::string(map_dir + "/.enum_map.cpp").c_str()) ;
        std::ofstream combined_map(std::string(map_dir + "/class_map.cpp").c_str()) ;
        combined_map << class_map.rdbuf() << enum_map.rdbuf() ;
    } else {
        remove( std::string(map_dir + "/.class_map.cpp").c_str() ) ;
        remove( std::string(map_dir + "/.enum_map.cpp").c_str() ) ;
    }
}

// Make a list of the empty files we processed.
// This list is written to the ICG_processed file and used by other processors.
std::set<std::string> PrintAttributes::getEmptyFiles() {
    std::set<std::string> emptyFiles;
    for (auto fi = ci.getSourceManager().fileinfo_begin() ; fi != ci.getSourceManager().fileinfo_end() ; ++fi ) {
        const clang::FileEntry * fe = (*fi).first ;
#if (LIBCLANG_MAJOR < 4) // TODO delete when RHEL 7 no longer supported
        std::string header_file_name = fe->getName() ;
#elif (LIBCLANG_MAJOR >= 4 && LIBCLANG_MAJOR < 18) 
        std::string header_file_name = fe->getName().str() ;
#else
        const clang::FileEntryRef fer = fi->first ;
        std::string header_file_name = fer.getName().str();
#endif

        if ( visited_files.find(header_file_name) != visited_files.end() ) {
            continue;
        }

        visited_files.insert(header_file_name) ;

        if (isHeaderExcluded(header_file_name)) {
            continue;
        }

        char* path  = almostRealPath(header_file_name.c_str());
        emptyFiles.insert(path);
        free(path) ;
    }
    return emptyFiles;
}

//TODO: Move this into PrintFileContents10.
void PrintAttributes::printIOMakefile() {
    std::ofstream makefile_io_src ;
    std::ofstream makefile_ICG ;
    std::ofstream io_link_list ;
    std::ofstream trickify_io_link_list ;
    std::ofstream ICG_processed ;
    std::ofstream ext_lib ;

    // Don't create a makefile if we didn't process any files.
    if ( out_of_date_io_files.empty() ) {
       return ;
    }

    std::cout << color(INFO, "Writing") << "    Makefile_io_src" << std::endl ;

    makefile_io_src.open("build/Makefile_io_src") ;
    makefile_io_src
        << "TRICK_IO_CXXFLAGS += -Wno-invalid-offsetof -Wno-old-style-cast -Wno-write-strings -Wno-unused-variable -Ibuild" << std::endl
        << std::endl
        << "ifeq ($(IS_CC_CLANG), 0)" << std::endl
        << "    TRICK_IO_CXXFLAGS += -Wno-unused-local-typedefs -Wno-unused-but-set-variable" << std::endl
        << "    ifeq ($(shell test $(GCC_MAJOR) -lt 6; echo $$?), 0)" << std::endl
        << "        TRICK_IO_CXXFLAGS += -std=c++11" << std::endl
        << "    endif" << std::endl
        << "endif" << std::endl
        << "ifeq ($(IS_CC_CLANG), 1)" << std::endl
        << "    TRICK_IO_CXXFLAGS += -std=c++14" << std::endl
        << "endif" << std::endl
        << std::endl
        << "IO_OBJECTS =" ;

    std::map< std::string , std::string >::iterator mit ;
    for ( mit = all_io_files.begin() ; mit != all_io_files.end() ; ++mit ) {
        size_t found ;
        found = (*mit).second.find_last_of(".") ;
        makefile_io_src << " \\\n    " << (*mit).second.substr(0,found) << ".o" ;
    }

    makefile_io_src << " \\\n    build/class_map.o" << std::endl
        << std::endl
        << "$(IO_OBJECTS): \%.o : \%.cpp | \%.d" << std::endl
        << "\t$(PRINT_COMPILE)" << std::endl
        << "\t$(call ECHO_AND_LOG,$(TRICK_CXX) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(TRICK_IO_CXXFLAGS) -MMD -MP -c -o $@ $<)" << std::endl
        << std::endl
        << "$(IO_OBJECTS:.o=.d): ;" << std::endl
        << std::endl
        << "-include $(IO_OBJECTS:.o=.d)" << std::endl
        << std::endl
        << "$(S_MAIN): $(IO_OBJECTS)" << std::endl
        << std::endl
        << "LINK_LISTS += $(LD_FILELIST)build/io_link_list" << std::endl;

    makefile_io_src.close() ;

    /*
       Makefile_ICG lists all headers as dependencies of Makefile_io_src
       causing ICG to run if any header file changes.

       io_link_list lists all io object files to be linked.

       trickify_io_link_list lists all io object files to be linked for a Trickified project.

       ICG_process lists all header files to be used by SWIG.
     */
    makefile_ICG.open("build/Makefile_ICG") ;
    io_link_list.open("build/io_link_list") ;
    trickify_io_link_list.open("build/trickify_io_link_list") ;
    ICG_processed.open("build/ICG_processed") ;

    makefile_ICG << "build/Makefile_io_src:" ;
    for ( mit = all_io_files.begin() ; mit != all_io_files.end() ; ++mit ) {
        makefile_ICG << " \\\n    " << (*mit).first ;
        size_t found ;
        found = (*mit).second.find_last_of(".") ;
        io_link_list << (*mit).second.substr(0,found) << ".o" << std::endl ;
        trickify_io_link_list << (*mit).second.substr(0,found) << ".o" << std::endl ;
        ICG_processed << (*mit).first << std::endl ;
    }
    makefile_ICG.close() ;

    // Create the list of empty (of classes/enums) header files to be written to ICG_processed.
    for ( auto& file : getEmptyFiles() ) {
        ICG_processed << file << std::endl ;
    }
    ICG_processed.close() ;

    io_link_list << "build/class_map.o" << std::endl ;
    io_link_list.close() ;
    trickify_io_link_list.close() ;

    ext_lib.open("build/ICG_ext_lib") ;
    for ( auto& file : ext_lib_io_files ) {
        ext_lib << file << std::endl ;
    }
    ext_lib.close() ;
}

void PrintAttributes::printICGNoFiles() {
    if ( ! sim_services_flag ) {
        std::vector< std::string >::iterator it ;
        std::ofstream icg_no_outfile("build/ICG_no_found") ;
        for ( it = icg_no_files.begin() ; it != icg_no_files.end() ; ++it ) {
            icg_no_outfile << (*it) << std::endl ;
        }
        icg_no_outfile.close() ;
    }
}

bool PrintAttributes::hasBeenProcessed(EnumValues& enumValues) {
    return hasBeenProcessed(enumValues.getFullyQualifiedName(), processed_enums[enumValues.getFileName()]);
}

bool PrintAttributes::hasBeenProcessed(ClassValues& classValues) {
    return hasBeenProcessed(classValues.getFullyQualifiedMangledTypeName(), processed_classes[classValues.getFileName()]);
}

bool PrintAttributes::hasBeenProcessed(const std::string& name, std::set<std::string>& processedSet) {
    bool processed = processedSet.find(name) != processedSet.end();
    if (!processed) {
        processedSet.insert(name);
    }
    return processed;
}

bool PrintAttributes::isIgnored(ConstructValues& constructValues) {
    const std::string& fileName = constructValues.getFileName();
    if (ignored_types.find(fileName) == ignored_types.end()) {
        ignored_types[fileName] = cs.getIgnoreTypes(fileName) ;
    }

    std::set<std::string>& constructs = ignored_types[fileName];

    const bool ignored = constructs.find(constructValues.getName()) != constructs.end() or
                         constructs.find(constructValues.getFullyQualifiedName()) != constructs.end() or
                         global_ignore_types.find(constructValues.getName()) != global_ignore_types.end() or
                         global_ignore_types.find(constructValues.getFullyQualifiedName()) != global_ignore_types.end();

    if (ignored and verboseBuild) {
        std::cout << skipping << "ICG Ignore Type: " << constructValues.getName() << " (from " << fileName << ")" << std::endl;
    }

    return ignored;
}

bool PrintAttributes::isHeaderExcluded(const std::string& header, bool exclude_ext_libs) {
    char* temp = almostRealPath(header.c_str());
    if (!temp) {
        return true;
    }
    const std::string path = std::string(temp);
    free(temp);

    /**
     * Exclude files:
     * - in system directories
     * - in TRICK_EXCLUDE directories
     * - in TRICK_ICG_EXCLUDE directories
     * - in TRICK_EXT_LIB_DIRS directories (if requested)
     * - whose Trick header comments preclude ICG
     */
    if (!hsd.isPathInUserDir(path)) {
        return true;
    }

    if (hsd.isPathInExclude(path)) {
        if (verboseBuild) {
            std::cout << skipping << "TRICK_EXCLUDE: " << underline(path, hsd.getPathInExclude(path).size()) << std::endl;
        }
        return true;
    }

    if (hsd.isPathInICGExclude(path)) {
        if (verboseBuild) {
            std::cout << skipping << "TRICK_ICG_EXCLUDE: " << underline(path, hsd.getPathInICGExclude(path).size()) << std::endl;
        }
        return true;
    }

    if (hsd.isPathInExtLib(path) && exclude_ext_libs) {
        if (verboseBuild) {
            std::cout << skipping << "TRICK_EXT_LIB_DIRS: " << underline(path, hsd.getPathInExtLib(path).size()) << std::endl;
        }
        ext_lib_io_files.insert(header) ;
        return true;
    }

    if (cs.hasICGNo(header)) {
        if (verboseBuild) {
            std::cout << skipping << "ICG: (NO): " << path << std::endl;
        }
        icg_no_files.push_back(path);
        return true;
    }

    temp = realpath(header.c_str(), NULL);
    if ( temp ) {
        const std::string real_path = std::string(temp);
        free(temp) ;
        if ( real_path.compare(path) ) {
            if (hsd.isPathInExclude(real_path)) {
                if (verboseBuild) {
                    std::cout << skipping << "TRICK_EXCLUDE: " << underline(real_path, hsd.getPathInExclude(real_path).size()) << std::endl;
                }
                return true;
            }

            if (hsd.isPathInICGExclude(real_path)) {
                if (verboseBuild) {
                    std::cout << skipping << "TRICK_ICG_EXCLUDE: " << underline(real_path, hsd.getPathInICGExclude(real_path).size()) << std::endl;
                }
                return true;
            }

            if (hsd.isPathInExtLib(real_path) && exclude_ext_libs) {
                if (verboseBuild) {
                    std::cout << skipping << "TRICK_EXT_LIB_DIRS: " << underline(real_path, hsd.getPathInExtLib(real_path).size()) << std::endl;
                }
                ext_lib_io_files.insert(header) ;
                return true;
            }
        }
    }

    return false;
}

void PrintAttributes::markHeaderAsVisited(const std::string& header) {
    visited_files.insert(header);
}

void PrintAttributes::writeTrickTypeToStructHeader()
{
   
    _mkdir("build/trick");

    std::set<std::string> class_names_to_print;
    for(auto& pair : processed_classes)
    {
        bool foundMatch = false;
        const std::set<std::string>& stringSet = pair.second;
        for(const std::string& element : stringSet)
        {
            if(typedef_classes.find(element) != typedef_classes.end())
            {
                foundMatch = true;
            }
            else 
            {
                class_names_to_print.insert(element);
            }   
        }

    }
    
    std::ofstream out_file;
    out_file.open("build/trick/trick_type_to_string_ext.hh");

    out_file << "/********************************* TRICK HEADER *******************************\n";
    out_file << "PURPOSE: ( Map types that trick knows about to strings using tempalte specialization )\n";
    out_file << "LIBRARY DEPENDENCY:\n";
    out_file << "(())\n";
    out_file << "*******************************************************************************/\n";
    
    out_file << "\n";
    out_file << "#pragma once";
    out_file << "\n\n";
    out_file << "#include \"trick/trick_type_to_string.hh\"\n";
    out_file << "#include <string>\n";
    out_file << "//Forward declarations\n";



    //Forward declarations
    for (auto class_name : class_names_to_print)
    {
        if(class_name.find("::") != std::string::npos)
        {

            //TODO: Move this into utilities if there isn't already a version if it in there
            auto split_name = [](std::string& qualified_name) -> std::pair<std::vector<std::string>, std::string> {
                std::vector<std::string> namespaces;
                std::string type;

                std::string delimiter = "::";
                size_t start = 0 ;
                //pos will point to just BEFORE the first ':' in "::"
                size_t pos = qualified_name.find(delimiter, start);
                
                while(pos != std::string::npos)
                {
                    std::string ns = qualified_name.substr(start, pos - start);
                    if(!ns.empty())
                    {
                        namespaces.emplace_back(ns);
                    }

                    //Shift start so we don't find the same :: again
                    start = pos + 2;
                    pos = qualified_name.find(delimiter, start);
                }
                
                type = qualified_name.substr(start);

                return {namespaces, type};

            };

            auto qualified_name_components = split_name(class_name);
            //Lambda function to generate a string with count spaces (for indenting)
            auto spaces = [](size_t count) -> std::string {
                std::string some_spaces;
                for(size_t i = 0; i < count; ++i)
                {
                    some_spaces.append(" ");
                }

                return some_spaces;
            };

            //Wrap these in namespace <namespace> declarations
            size_t ns_count = 0;
            for (auto ns : qualified_name_components.first)
            {
                out_file << spaces(ns_count) << "namespace " << ns << "{\n";
                ns_count++;
            }

            out_file << spaces(ns_count) << "class " <<  qualified_name_components.second << ";\n";

            for(size_t i = ns_count; i > 0; --i)
            {
                out_file << spaces(i - 1) << "}\n";
            }
        }
        else 
        {
            out_file << "class " << class_name << ";\n";
        }
        
    }

    out_file << "\n\n";
    //Create specializations for all types contained in class_names_to_print
    for (auto class_name : class_names_to_print)
    {
        out_file << "template<>\n";
        out_file << "struct TrickTypeToString<" << class_name << ">\n";
        out_file << "{\n";
        out_file << "   static std::string getName() \n";
        out_file << "   {\n";
        out_file << "       return \"" << class_name << "\";\n";
        out_file << "   }\n";
        out_file << "};\n\n";
        out_file << "\n\n";
    }

    out_file.close();

}


void PrintAttributes::addTypedefClass(std::string typedef_class)
{
    typedef_classes.insert(typedef_class);
}

void PrintAttributes::setUseTMMAllocArgs(bool use_tmm_alloc_args)
{
    this->use_tmm_alloc_args = use_tmm_alloc_args;
}