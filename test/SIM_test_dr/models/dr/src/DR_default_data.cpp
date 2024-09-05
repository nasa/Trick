/******************************TRICK	HEADER*************************************
PURPOSE:																					(	Test	data	record	)
REFERENCE:																			(	None	)
ASSUMPTIONS	AND	LIMITATIONS:	(	None	)
CLASS:																							(	scheduled	)
LIBRARY	DEPENDENCY:										(	DR.o	)
PROGRAMMERS:																	(	(Lindsay	Landry)	(L3)	(7-22-2013)	)
*******************************************************************************/
#include	"../include/DR.hh"


int	DRTypes::init()	{

	a	=	'a';			//char
	b	=	'b';			//unsigned	char
	c	=	-1234;			//short
	d	=	1234;			//unsigned	short
	e	=	-123456;		//int
	f	=	123456;			//unsigned	int
	g	=	-1234567;		//long
	h	=	123456789;		//unsigned	long
	i	=	1234.5678;		//float
	j	=	-1234.567890;	//double
	k	=	-12345678912345;//long	long
	l	=	12345678912345;	//unsigned	long
	m	=	false;			//boolean
	n	=	THREE;			//enumerated	type
	o	=	"3	May.	Bistritz.—Left	Munich	at	8:35	P.	M.,	on	1st	May,	arriving	at	Vienna	early	next	morning;	should	have	arrived	at	6:46,	but	train	was	an	hour	late.	Buda-Pesth	seems	a	wonderful	place,	from	the	glimpse	which	I	got	of	it	from	the	train	and	the	little	I	could	walk	through	the	streets.	I	feared	to	go	very	far	from	the	station,	as	we	had	arrived	late	and	would	start	as	near	the	correct	time	as	possible.	The	impression	I	had	was	that	we	were	leaving	the	West	and	entering	the	East;	the	most	western	of	splendid	bridges	over	the	Danube,	which	is	here	of	noble	width	and	depth,	took	us	among	the	traditions	of	Turkish	rule.";
	p	=	"We	left	in	pretty	good	time,	and	came	after	nightfall	to	Klausenburgh.	Here	I	stopped	for	the	night	at	the	Hotel	Royale.	I	had	for	dinner,	or	rather	supper,	a	chicken	done	up	some	way	with	red	pepper,	which	was	very	good	but	thirsty.	(Mem.,	get	recipe	for	Mina.)	I	asked	the	waiter,	and	he	said	it	was	called	“paprika	hendl,”	and	that,	as	it	was	a	national	dish,	I	should	be	able	to	get	it	anywhere	along	the	Carpathians.	I	found	my	smattering	of	German	very	useful	here;	indeed,	I	don't	know	how	I	should	be	able	to	get	on	without	it.";
	for	(int	i	=	0;	i	<	5;	i++)	{
		q[i]	=	i;
	}
	r[0][0]	=	10;
	r[0][1]	=	20;
	r[1][0]	=	30;
	r[1][1]	=	40;

/*============================================================================
										Bitfields
		============================================================================
	UNSIGNED
	var1:	Larger	value	than	will	fit
	var2:	Min	value
	var3:	Middle	value
	var4:	Max	value
*/

	/*	Unsigned	Int	*/
	//14	5	12	1
		uintB.var1	=	(1	<<	14)	+	(1	<<	14/2);
				uintB.var2	=	0;
				uintB.var3	=	(1	<<	12/2)	-	1;
				uintB.var4	=	(1	<<	1)	-	1;

	/*	Unsigned	Char	*/
	//	3	2	2	1
	ucharB.var1	=	(1	<<	3)	+	(1	<<	3/2);
				ucharB.var2	=	0;
				ucharB.var3	=	(1	<<	2/2)	-	1;
				ucharB.var4	=	(1	<<	1)	-	1;

	/*	Unsigned	Short	*/
	//	4	2	9	1
				ushortB.var1	=	(1	<<	4)	+	(1	<<	4/2);
				ushortB.var2	=	0;
				ushortB.var3	=	(1	<<	9/2)	-	1;
				ushortB.var4	=	(1	<<	1)	-	1;

/*	SIGNED
	var1:	Max	positive	value
	var2:	Max	negative	value
	var3:	Zero
	var4:	Too	large	value	->	negative
*/
	/*	Int	*/
	//	7	16	3	6
				intB.var1	=	(1	<<	(7	-	1))	-	1;
				intB.var2	=	-(1	<<	(16	-	1));
				intB.var3	=	0;
				intB.var4	=	(1	<<	(6	-	1))	+	(1	<<	6/2);;

	/*	Char	*/
	//	2	2	2	2
				charB.var1	=	(1	<<	(2	-	1))	-	1;
				charB.var2	=	-(1	<<	(2	-	1));
				charB.var3	=	0;
				charB.var4	=	(1	<<	(2	-	1))	+	(1	<<	2/2);;

	/*	Short	*/
	//	8	1	2	5
				shortB.var1	=	(1	<<	(8	-	1))	-	1;
				shortB.var2	=	-(1	<<	(1	-	1));
				shortB.var3	=	0;
				shortB.var4	=	(1	<<	(5	-	1))	+	(1	<<	5/2);;

	/*	Mixed	Types	*/
	//	3	8	9	12
				mixB.var1	=	(1	<<	(3	-	1))	-	1;
				mixB.var2	=	-(1	<<	(8	-	1));
				mixB.var3	=	0;
				mixB.var4	=	(1	<<	(12	-	1))	+	(1	<<	12/2);;

	return	(0);
}
