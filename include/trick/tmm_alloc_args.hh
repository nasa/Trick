/*************************************************************************
PURPOSE: (Trick TMM Alloc W/ Args)
ICG: (No)
LIBRARY DEPENDENCY:
    (
    ()
    )
**************************************************************************/
#ifndef SWIG
#ifndef __ALLOC_WITH_ARGS_HH__
#define __ALLOC_WITH_ARGS_HH__

#include "trick/trick_type_traits.hh"

/**
 * @brief Allocates and constructs an object of type `T` using Trick's Memory Manager.
 * @tparam T Type to be allocated.
 * @tparam Args parameter pack for forwarding arguments to placement new.
 * @param args Parameter pack for forwarding arguments to the constructor of type `T`.
 * 
 * @details Wrapper around Trick's `void* MemoryManager::declare_var( const char* declaration)`. 
 * It allocates memory for an object of type `T` and constructs it using placement new.
 * Users can pass constructor arguments, which will be perfectly forward to `T`'s constructor.
 * 
 * @return pointer to type T that was just allocated.
 * 
 * @note This uses SFINAE to determine which overload should be selected.
 * Where `Trick::has_getname<T>::value` evaluates to `false`, this overload is not selected.
 * When it evalutes to `true`, this overload is used and the return type is `T*`.
 * 
 * @example
 * @code
 * //Assuming:
 * //1. TrickTypeToString has a specialization for TestClass.
 * //2. TestClass has a default constructor.
 * //3. TestClass has a constructor that takes two arguments.
 * TestClass* test_class = tmm_alloc_args<TestClass>(arg_1, arg_2);
 * @endcode
*/
template<typename T, typename ...Args>
typename std::enable_if<Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    void* new_alloc = trick_MM->declare_var(TrickTypeToString<T>::getName().c_str());
    return new (new_alloc) T(std::forward<Args>(args)...);
}

/**
 * @brief Constructs an object of type `T` using placement new.
 * @tparam T Type to be constructed.
 * @tparam Args parameter pack for forwarding arguments to placement new.
 * @param address Address of memory in which T will be constructed.
 * @param args Parameter pack for forwarding arguments to the constructor of type `T`.
 * 
 * @details Generalization of placement new for any type for which a specialization of 
 * TrickTypeToString exists.
 * 
 * @return T Pointer to address.  
 * 
 * @note This uses SFINAE to determine which overload should be selected.
 * Where `Trick::has_getname<T>::value` evaluates to `false`, this overload is not selected.
 * When it evalutes to `true`, this overload is used and the return type is `T*`.
 * 
 * @example
 * @code
 * //Assuming:
 * //1. TrickTypeToString has a specialization for TestClass.
 * //2. TestClass has a default constructor.
 * //3. TestClass has a constructor that takes two arguments.
 * 
 * TestClass* test_class = trick_MM->declare_var("TestClass");
 * TestClass* test_class = tmm_alloc_args<TestClass>(test_class, arg_1, arg_2);
 * // or 
 * TestClass* test_class = new TestClass();
 * TestClass* test_class = tmm_alloc_args<TestClass>(test_class, arg_1, arg_2);
 * @endcode
*/
template<typename T, typename ...Args>
typename std::enable_if<Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(T* address, Args&&... args)
{
    return new (address) T(std::forward<Args>(args)...);
}

/**
 * @tparam T Type to be allocated.
 * @tparam Args parameter pack for forward arguments to placement new.
 * @details Version of tmm_alloc_args that will be compiled if TrickTypeToString has no 
 * member function 'getName()'.  Static assert will end compilation and print a useful
 * error message.
 * @return Technically a nullptr, but this will never compile.
*/
template<typename T, typename ...Args>
typename std::enable_if<!Trick::has_getname<T>::value, T*>::type
tmm_alloc_args(Args&&... args)
{
    //Compilation will always fail here.
    static_assert(Trick::always_false<T>::value,
                "You've attempted to call tmm_alloc_args using a type(T) that does not have an implemented specialization for TrickTypeToString.");

    return nullptr;
}

#endif 
#endif