/********************************* TRICK HEADER *******************************
PURPOSE: ( Test tmm_alloc_args in a sim environment )
LIBRARY DEPENDENCY:
((alloc_with_args.o))
*******************************************************************************/
#pragma once
#include <iostream>
#include "trick/MemoryManager.hh"

namespace Test {
    struct SomeStruct
    {
        SomeStruct(double new_val)
        {
            val = new_val;
        }
        double val = 5.0;
    };
}

class AllocTestWithArguments {

    friend class InputProcessor;
    
    public:

    AllocTestWithArguments();

    AllocTestWithArguments(int in_int, double in_double);

    AllocTestWithArguments(int* in_int, double *in_double, std::string &a_name);

    AllocTestWithArguments(int* in_int, double *in_double, const char* a_name);

    AllocTestWithArguments(int in_int, double in_double, const char* a_name);

    AllocTestWithArguments(int*, double&, std::string);

    AllocTestWithArguments(Test::SomeStruct in_struct);

    ~AllocTestWithArguments() {
        std::cout << "AllocTestWithArguments desctruct.\n";
    }
    
    int some_int;
    double some_double;
    std::string some_string;

    Test::SomeStruct* my_struct;
};