#include	<stdio.h>
#include	<stdlib.h>
#include	"trick/RefParseContext.hh"

/**
	Constructor
	*/	RefParseContext::RefParseContext(	Trick::MemoryManager	*in_mem_mgr,	std::istream*	in_is	=	&std::cin	)	{

			//	Set	verif	input	and	echo	input	flags.
			verify_input	=	0;
			echo_input	=	1;

			//	Initialize	the	scanner
			init_scanner();

			this->is	=	in_is;
			this->mem_mgr	=	in_mem_mgr;

}

/**
	Destructor
	*/
RefParseContext::~RefParseContext()	{
				destroy_scanner();
}

