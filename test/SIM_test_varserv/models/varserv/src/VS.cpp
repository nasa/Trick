/******************************TRICK	HEADER*************************************
PURPOSE:																					(	Test	variable	server	)
REFERENCE:																			(	None	)
ASSUMPTIONS	AND	LIMITATIONS:	(	None	)
CLASS:																							(	scheduled	)
LIBRARY	DEPENDENCY:										(	VS.o	)
PROGRAMMERS:																	(	(Lindsay	Landry)	(L3)	(9-12-2013)	
								(Jackie	Deans)	(CACI)	(11-30-2022)	)
*******************************************************************************/
#include	<iostream>
#include	<stdexcept>
#include	<limits>

#include	"../include/VS.hh"
#include	"trick/exec_proto.h"

VSTest::VSTest()	{}
VSTest::~VSTest()	{}

int	VSTest::default_vars()	{

	a	=	'a';												//char
	b	=	'b';												//unsigned	char
	c	=	-1234;										//short
	d	=	1234;											//unsigned	short
	e	=	-123456;								//int
	f	=	123456;									//unsigned	int
	g	=	-1234567;							//long
	h	=	123456789;						//unsigned	long
	i	=	1234.5678;						//float
	j	=	-1234.567890;			//double
	k	=	-12345678912345;//long	long
	l	=	12345678912345;	//unsigned	long
	m	=	true;										//boolean
	for	(int	i	=	0;	i	<	5;	i++)	{
		n[i]	=	i;
	}
	o	=	std::string("You	will	rejoice	to	hear	that	no	disaster	has	accompanied	the	commencement	of	an	enterprise	which	you	have	regarded	with	such	evil	forebodings.	I	arrived	here	yesterday,	and	my	first	task	is	to	assure	my	dear	sister	of	my	welfare	and	increasing	confidence	in	the	success	of	my	undertaking.");
	p	=	"I	am	already	far	north	of	London,	and	as	I	walk	in	the	streets	of	Petersburgh,	I	feel	a	cold	northern	breeze	play	upon	my	cheeks,	which	braces	my	nerves	and	fills	me	with	delight.	Do	you	understand	this	feeling?";
	q	=	L"This	breeze,	which	has	travelled	from	the	regions	towards	which	I	am	advancing,	gives	me	a	foretaste	of	those	icy	climes.	Inspirited	by	this	wind	of	promise,	my	daydreams	become	more	fervent	and	vivid.";

	for	(int	i	=	0;	i	<	4000;	i++)	{
		large_arr[i]	=	i;
	}

	/*	Mixed	Types	*/
	//	3,-128,0,2112
				my_bitfield.var1	=	(1	<<	(3	-	1))	-	1;
				my_bitfield.var2	=	-(1	<<	(8	-	1));
				my_bitfield.var3	=	0;
				my_bitfield.var4	=	(1	<<	(12	-	1))	+	(1	<<	12/2);;


	blocked_from_input	=	500;
	blocked_from_output	=	1000;

	return	0;
}

int	VSTest::init()	{
	status	=	2;	
	return	(0);
}

int	VSTest::fail()	{
				status	=	1;	
	return	0;
}

int	VSTest::success()	{
				status	=	0;
	return	0;	
}

void	VSTest::throw_exception()	{
	throw	std::logic_error("Pretend	an	error	has	occured	for	testing");
}

int	VSTest::shutdown()	{
	std::cout	<<	"Shutting	down	with	status:	"	<<	status	<<	"	Message:	"	<<	status_messages[status]	<<	std::endl;

	//	Check	that	the	blocked_from_input	variable	hasnt	changed
	if	(blocked_from_input	!=	500	||	blocked_from_output	!=	0)	{
		status	=	1;
	}

				exec_terminate_with_return(	status	,	__FILE__	,	__LINE__	,	status_messages[status]	)	;
	
	return(0);
}