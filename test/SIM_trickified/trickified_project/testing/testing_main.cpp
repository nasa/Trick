#include "testing.hh"

int some_function()
{
	test_class test ;
	std::cout << test.a << ", " << test.b << ", " << test.c << std::endl ;
	test.print() ;

	return 0 ;

}
