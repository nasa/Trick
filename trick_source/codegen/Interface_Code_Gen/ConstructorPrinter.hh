#ifndef __CONSTRUCTOR_PRINTER_HH
#define __CONSTRUCTOR_PRINTER_HH



#include "Utilities.hh"

#include <memory>
#include <utility>
#include <string>
#include <map>
#include <vector>
#include <sstream>


class FunctionDescription;
class FunctionArgument;


class PrintConstructors
{
    public:
    PrintConstructors();
    ~PrintConstructors();

    void setOutputFileName(std::string name, std::string path);
    
    void setFullyQualifiedClassName(std::string fully_qualified_name);

    void printAll();
    
    void writeToFile(std::string file_name);

    void addFunctionDescription(std::string class_name, FunctionDescription* function_description);

    std::stringstream& getStream();


    public:
    
    private:
    void printSingle(std::stringstream& out_stream, FunctionDescription* functionDescription, const std::string& class_name, bool last=false);

    void printParameter(std::stringstream& out_stream, FunctionArgument& arg);
    void printParameters(std::stringstream& out_stream,  FunctionDescription* functionDescription, const std::string& class_name);

    std::stringstream& writeString(std::stringstream& out_stream, std::string label, std::string in_string, bool last=false);
    std::stringstream& writeBool(std::stringstream& out_stream, std::string label, std::string in_string, bool last=false);

    std::string getSpaces(size_t count);

    private:
    std::map<std::string, std::vector<FunctionDescription*>> classFuncDes;
    std::string outputFileName;
    std::string outputFilePath;
    std::stringstream stream;
    std::size_t tab_count;
    std::size_t spacesPerTab;
    
    std::string fullyQualifiedClassName;

};

#endif // __CONSTRUCTOR_PRINTER_HH