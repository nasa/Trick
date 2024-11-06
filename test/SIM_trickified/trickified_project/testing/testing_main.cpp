#include "testing.hh"

int main()
{
	test_class test ;
	std::cout << test.a << ", " << test.b << ", " << test.c << std::endl ;
	test.print() ;

	return 0 ;

}
