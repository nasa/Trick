#include "ConstructorPrinter.hh"
#include "FunctionDescription.hh"


#include <fstream>
#include <algorithm>

PrintConstructors::PrintConstructors()
:
tab_count(0),
spacesPerTab(2)
{
    
}

PrintConstructors::~PrintConstructors()
{

}

std::stringstream& PrintConstructors::getStream()
{
    return stream;
}

void PrintConstructors::setOutputFileName(std::string name, std::string path)
{
    outputFileName = name;
    outputFilePath = path;
}

void PrintConstructors::addFunctionDescription(std::string class_name, FunctionDescription* function_description)
{
    if(function_description != nullptr)
    {
        auto it = classFuncDes.find(class_name);

        if(it == classFuncDes.end())
        {
            //Didn't find this key
            classFuncDes.insert({class_name, std::vector<FunctionDescription*>({function_description})});
        }
        else 
        {
            //Found class, append function_description to back of matrix
            classFuncDes[class_name].emplace_back(function_description);
        }
    }
}

void PrintConstructors::printAll()
{
    stream << "[\n";
    tab_count++;
    
    for(auto classIter = classFuncDes.begin(); classIter != classFuncDes.end(); ++classIter)
    {
        stream << "{\n";
        tab_count++;
        
        writeString(stream, "className", classIter->first);
        writeString(stream, "qualifiedClassName", (*classIter->second.begin())->getFullyQualifiedContainerClassName());

        stream << getSpaces(tab_count) << "\"constructors\": " << "[\n";
        
        for(auto funcIt = classIter->second.begin(); funcIt != classIter->second.end(); ++funcIt)
        {
            if(std::next(funcIt) == classIter->second.end())
            {
                 printSingle(stream, *funcIt, classIter->first, true) ;
            }
            else 
            {
                printSingle(stream, *funcIt, classIter->first) ;
            }
        }

        stream << getSpaces(tab_count) << "]\n";

        tab_count--;
        if(std::next(classIter) == classFuncDes.end())
        {
            stream << "}\n";
        }
        else 
        {
            stream << "},\n";
        }
        
    }
    
    tab_count--;
    stream << "]\n";
}

void PrintConstructors::printSingle(std::stringstream& out_stream, FunctionDescription* functionDescription, const std::string& class_name, bool last)
{
    out_stream << getSpaces(tab_count) << "{\n" ;
    tab_count++;


    writeString(out_stream, "accessSpecifier", accessToChar(functionDescription->getAccess()));
    

    out_stream << getSpaces(tab_count) << "\"parameters\": [\n";

    tab_count++;
    printParameters(out_stream, functionDescription, class_name);
    tab_count--;

    out_stream << getSpaces(tab_count) << "],\n";

    writeBool(out_stream, "isDefault", boolToChar(functionDescription->getIsDefaultConstructor()));
    writeBool(out_stream, "isCopy", boolToChar(functionDescription->getIsCopyConstructor()));
    writeBool(out_stream, "isMove", boolToChar(functionDescription->getIsMoveConstructor()), true);

    tab_count--;

    if(last)
    {
        out_stream << getSpaces(tab_count) << "}\n";
    }
    else 
    {
        out_stream << getSpaces(tab_count) << "},\n";
    }
}

void PrintConstructors::printParameters(std::stringstream& out_stream, FunctionDescription* functionDescription, const std::string& class_name)
{
    std::vector<FunctionArgument>& arguments = functionDescription->getFunctionArgs();

    for(auto argIt = arguments.begin(); argIt != arguments.end(); ++argIt)
    {
        printParameter(out_stream, *argIt);
        //Last entry doesn't need a comma
        if(std::next(argIt) == arguments.end())
        {
            out_stream << "\n";
        }
        else 
        {
            out_stream << ",\n";
        }
    } 
}

void PrintConstructors::printParameter(std::stringstream& out_stream, FunctionArgument& arg)
{
    /*
    Output format
    {
        "type": double,
        "name": a_name
    }
    */

    out_stream << getSpaces(tab_count) << "{\n";
    tab_count++;
    writeString(out_stream, "type", arg.typeName);
    writeString(out_stream, "name", arg.argName, true);
    tab_count--;
    out_stream << getSpaces(tab_count) << "}";

    //Let the calling function add the newline and/or comma
}

std::stringstream& PrintConstructors::writeString(std::stringstream& out_stream, std::string label, std::string in_string, bool last)
{   

    out_stream << getSpaces(tab_count) << "\"" << label << "\": " << "\"" << in_string <<  "\"";

    if(!last)
    {
        out_stream << ",";
    }
    
    out_stream << "\n";
    
    return out_stream;
}

std::stringstream& PrintConstructors::writeBool(std::stringstream& out_stream, std::string label, std::string in_bool, bool last)
{   
    
    out_stream << getSpaces(tab_count) << "\"" << label << "\": " << in_bool;

    if(!last)
    {
        out_stream << ",";
    }
    
    out_stream << "\n";
    
    return out_stream;
}


std::string PrintConstructors::getSpaces(size_t count)
{
    std::string spaces;
    
    for(size_t i = 0; i < count*spacesPerTab; ++i)
    {
        spaces.append(" ");    
    }

    return spaces;
}

void PrintConstructors::writeToFile(std::string file_name)
{
    std::ofstream output_file;
    output_file.open(file_name);
    output_file << stream.str();
    output_file.close();
}

void PrintConstructors::setFullyQualifiedClassName(std::string fully_qualified_name)
{
    fullyQualifiedClassName = fully_qualified_name;
}