#include	"DS_exercisor.hh"

#include	<iostream>
#include	<string>

using	namespace	std;

int	DS_exercisor::run()	{

		int	ch,	done;
		int	ret;
		double	time,	value;
		string	dsfilename,	unitspec,	timeunitspec;

		cout	<<	"DataStream	Exercisor"	<<	endl;
		cout	<<	"Type	\"?\"	for	help."	<<	endl;

		done	=	0;
		while	(!done)	{
				ch	=	getc(stdin);
				switch	(ch)	{
				case	'?'	:	{
						cout	<<	"===	COMMANDS	==="	<<	endl;
						cout	<<	"?	->	help"								<<	endl;
						cout	<<	"!	->	quit"								<<	endl;
						cout	<<	"g	->	get"									<<	endl;
						cout	<<	"p	->	peek"								<<	endl;
						cout	<<	"f	->	getFileName"	<<	endl;
						cout	<<	"u	->	getUnit"					<<	endl;
						cout	<<	"t	->	getTimeUnit"	<<	endl;
						cout	<<	"b	->	begin"							<<	endl;
						cout	<<	"e	->	end"									<<	endl;
						cout	<<	"s	->	step"								<<	endl;
						cout	<<	"================"	<<	endl;
				}	break;
				case	'!'	:	{
						cout	<<	"================"	<<	endl;
						cout	<<	"==		Quitting		=="	<<	endl;
						cout	<<	"================"	<<	endl;
						done	=	1;
				}	break;
				case	'g'	:	{
						ret	=	testds->get(&time,	&value);
						cout	<<	"get	:"	<<	"time	=	"	<<	time		<<	"					"	<<	"value=	"	<<	value	<<	endl;
						cout	<<	"					"	<<	"ret=	"	<<	ret	<<	endl;
				}	break;
				case	'p'	:	{
						ret	=	testds->peek(&time,	&value);
						cout	<<	"peek:"	<<	"time	=	"	<<	time		<<	"					"	<<	"value=	"	<<	value	<<	endl;
						cout	<<	"					"	<<	"ret=	"	<<	ret	<<	endl;
				}	break;
				case	'f'	:	{
						dsfilename	=	testds->getFileName();
						cout	<<	"getFileName:"	<<	"filename=	"	<<	"\""	<<	dsfilename	<<	"\""	<<	endl;
				}	break;
				case	'u'	:	{
						unitspec	=	testds->getUnit();
						cout	<<	"getUnit:"	<<	"unitspec=	"	<<	"\""	<<	unitspec	<<	"\""	<<	endl;
				}	break;
				case	't'	:	{
						timeunitspec	=	testds->getTimeUnit();
						cout	<<	"getTimeUnit:"	<<	"timeunitspec=	"	<<	"\""	<<	timeunitspec	<<	"\""	<<	endl;
				}	break;
				case	'b'	:	{
						testds->begin();
						cout	<<	"begin"	<<	endl;
				}	break;
				case	'e'	:	{
						testds->end();
						cout	<<	"end"	<<	endl;
				}	break;
				case	's'	:	{
						testds->step();
						cout	<<	"step"	<<	endl;
				}	break;
				default	:	{
				}
				}
		}
		return	(0);
}

