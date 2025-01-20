
#ifndef UTILITIES_HH
#define UTILITIES_HH

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/Specifiers.h"

#include "HeaderSearchDirs.hh"

enum Color {
    ERROR = 31,
    WARNING = 33,
    INFO = 34,
    SKIP = 95
};

std::string sanitize(const std::string&) ;
std::string trim( const std::string& str, const std::string& whitespace = " \t\n\r" ) ;
bool isInUserCode( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) ;
bool isInUserOrTrickCode( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) ;
std::string getFileName( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) ;
char * almostRealPath( const std::string& in_path ) ;
char * almostRealPath( const char * in_path ) ;
char * getResolvedPath(clang::CompilerInstance & ci , clang::SourceLocation sl);
std::string color(const Color& color, const std::string& text);
std::string bold(const std::string& text);
std::string underline(const std::string& text);
std::string underline(const std::string& text, unsigned length);
std::string quote(const std::string& text);
std::string & replace_special_chars( std::string & str);
int gccVersionToIntOrDefault(const char* verno, int def);



inline const char * boolToChar(bool b) { return b ? "true" : "false";}
const char * accessToChar(clang::AccessSpecifier spec);

#endif
