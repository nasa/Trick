#ifndef __FUNCTION_DESCRIPTION_HH__
#define __FUNCTION_DESCRIPTION_HH__

#include <string>
#include <vector>
#include <utility>

#include "clang/Basic/Specifiers.h"

struct FunctionArgument
{
    std::string typeName;
    std::string argName;
    bool isPointer;
    bool isReference;
    bool isConst;
};


class FunctionDescription
{
    public:
    FunctionDescription();
    FunctionDescription(std::string& container_class);
    ~FunctionDescription();


    void setReturnType(std::string return_type_name);
    
    void setContainerClass(std::string& container_class);

    void setInherited(bool inherited);
    
    void setVirtualInherited(bool virtual_inherited);
    

    void setStatic(bool is_static);
    void getIsStatic() const;

    void setIsConstructor(bool is_constructor);
    void getIsConstructor() const;

    void setIsCopyConstructor(bool is_copy_constructor);
    bool getIsCopyConstructor() const;

    void setIsMoveConstructor(bool is_move_constructor);
    bool getIsMoveConstructor() const ;
    
    void setFunctionName(std::string in_function_name);
    std::string getFunctionName() const;

    void setAccess(clang::AccessSpecifier in_access);
    clang::AccessSpecifier getAccess() const;

    bool getIsDefaultConstructor() const;
    void setIsDefaultConstructor(bool is_default);

    std::string getMangledName();

    void addFunctionArgument(FunctionArgument arg);
    
    std::string getReturnType() const { return return_type_name; }

    inline std::vector<FunctionArgument>& getFunctionArgs();
    
    std::string getContainerClass() const;

    void setFullyQualifiedContainerClassName(std::string fully_qualified_name);
    std::string getFullyQualifiedContainerClassName();
    
    bool getIsPublic() const;

    private:
    /** Line number in current file where field is */
    unsigned int line_no ;

    /** Name of the class this field is in */
    std::string container_class ;

    /* Fully qualified name of the container class*/
    std::string fully_qualified_container_class_name;

    std::string return_type_name ;

    std::string function_name;

    std::vector<FunctionArgument> function_args;

    /** public/protected/private */
    clang::AccessSpecifier access ;

    /** is this function inherited from parent class */
    bool is_inherited ;

    /** is this function virtual inherited from parent class */
    bool is_virtual_inherited ;

    /** is this field declared static */
    bool is_static ;

    bool is_constructor ;

    bool is_default_constructor ;

    bool is_copy_constructor ;

    bool is_move_constructor ;

    private:

    std::string removePointerAndReference(const std::string& typeStr);
};

inline std::vector<FunctionArgument>& FunctionDescription::getFunctionArgs() { return  function_args; }


#endif // __FUNCTION_DESCRIPTION_HH__