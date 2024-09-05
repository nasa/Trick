#include	<stdio.h>
#include	<stdlib.h>
#include	"trick/ChkPtParseContext.hh"

/**
	Constructor
	*/
ChkPtParseContext::ChkPtParseContext(	Trick::MemoryManager	*mem_mgr,	std::istream*	is	=	&std::cin	)	{

			//	Set	verif	input	and	echo	input	flags.
			verify_input	=	0;
			echo_input	=	0;

			//	Initialize	the	scanner
			init_scanner();

			this->buf	=	(char*)calloc(	0xffff,	sizeof(char));

			this->bad_declaration_count	=	0;
			this->bad_assignment_count	=	0;

			this->is	=	is;
			this->mem_mgr	=	mem_mgr;

}

/**
	Destructor
	*/
ChkPtParseContext::~ChkPtParseContext()	{
				destroy_scanner();
				free(	this->buf	);
}

