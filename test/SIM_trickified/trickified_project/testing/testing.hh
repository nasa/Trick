#include <iostream>

class test_class
{
	public:

	test_class();

	void print() 
	{
		int i = 0 ;
		std::cout << "Printing Test Class: " << a+i << ", " << b+i << ", " << c+i << std::endl ;
		i++ ;
	}

	int a;
	int b;
	int c;
};
