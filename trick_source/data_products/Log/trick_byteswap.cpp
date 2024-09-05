/*
PURPOSE:
					(Routines	for	swapping	byte	order.		Borrowed	from	ISP.)
REFERENCE:
					((None))
ASSUMPTIONS	AND	LIMITATIONS:
					((None))
CLASS:
					(N/A)
LIBRARY	DEPENDENCY:
				((trick_byteswap.o))
PROGRAMMERS:
					(((Jane	B.	Falgout)	(LinCom)	(08	May	98)	(--)	(generic	HC)))
*/


/*	(note	from	ISP	developers)
	*	Normally	this	routine	would	just	zip	through	the	double	a	byte
	*	at	a	time	and	reassign	them,	but	the	optimizing	compiler	on	the
	*	alpha	core	dumps	on	this	file,	after	many	iterations	this	"reloading"
	*	of	bytes	into	the	"output"	double	works	with	the	alpha
	*/

#include	"trick_byteswap.h"

extern	"C"	{

long	long	trick_byteswap_long_long(							/*	RETURN:	--	swapped	byte	*/
																	long	long	input)									/*	IN:	--	bytes	to	be	swapped	*/
{

							long	long	output;
							unsigned	char	*out	=	(unsigned	char	*)&output;

							*(((unsigned	char	*)&output)				)	=	*(((unsigned	char	*)&input)	+	7);
							*(((unsigned	char	*)&output)	+	1)	=	*(	((unsigned	char	*)&input)	+	6);
							*(((unsigned	char	*)&output)	+	2)	=	*(	((unsigned	char	*)&input)	+	5);
							*(((unsigned	char	*)&output)	+	3)	=	*(	((unsigned	char	*)&input)	+	4);

							out	+=	4;
							*(((unsigned	char	*)out)				)	=	*(((unsigned	char	*)&input)	+	3);
							*(((unsigned	char	*)out)	+	1)	=	*(((unsigned	char	*)&input)	+	2);
							*(((unsigned	char	*)out)	+	2)	=	*(((unsigned	char	*)&input)	+	1);
							*(((unsigned	char	*)out)	+	3)	=	*(((unsigned	char	*)&input)				);

							return(output);
}

double	trick_byteswap_double(							/*	RETURN:	--	swapped	byte	*/
																	double	input)						/*	IN:	--	bytes	to	be	swapped	*/
{

							double	output;
							unsigned	char	*out	=	(unsigned	char	*)&output;

							*(((unsigned	char	*)&output)				)	=	*(((unsigned	char	*)&input)	+	7);
							*(((unsigned	char	*)&output)	+	1)	=	*(	((unsigned	char	*)&input)	+	6);
							*(((unsigned	char	*)&output)	+	2)	=	*(	((unsigned	char	*)&input)	+	5);
							*(((unsigned	char	*)&output)	+	3)	=	*(	((unsigned	char	*)&input)	+	4);

							out	+=	4;
							*(((unsigned	char	*)out)				)	=	*(((unsigned	char	*)&input)	+	3);
							*(((unsigned	char	*)out)	+	1)	=	*(((unsigned	char	*)&input)	+	2);
							*(((unsigned	char	*)out)	+	2)	=	*(((unsigned	char	*)&input)	+	1);
							*(((unsigned	char	*)out)	+	3)	=	*(((unsigned	char	*)&input)				);

							return(output);
}

float	trick_byteswap_float(							/*	RETURN:	--	swapped	bytes	*/
																float	input)						/*	IN:	--	bytes	to	be	swapped	*/
{
							float	output;

							*(((unsigned	char	*)&output)				)	=	*(((unsigned	char	*)&input)	+	3);
							*(((unsigned	char	*)&output)	+	1)	=	*(((unsigned	char	*)&input)	+	2);
							*(((unsigned	char	*)&output)	+	2)	=	*(((unsigned	char	*)&input)	+	1);
							*(((unsigned	char	*)&output)	+	3)	=	*(((unsigned	char	*)&input));

							return(output);
}

long	trick_byteswap_long(									/*	RETURN:	--	swapped	byte	*/
																	long	input)						/*	IN:	--	bytes	to	be	swapped	*/
{

							long	output;
							unsigned	char	*out	=	(unsigned	char	*)&output;

							if	(sizeof(long)	==	4)	{
															/*	long	is	4	bytes	*/
															*(((unsigned	char*)&output)		)	=	*(((unsigned	char*)&input)+3);
															*(((unsigned	char*)&output)+1)	=	*(((unsigned	char*)&input)+2);
															*(((unsigned	char*)&output)+2)	=	*(((unsigned	char*)&input)+1);
															*(((unsigned	char*)&output)+3)	=	*(((unsigned	char*)&input)		);
							}	else	{
															/*	long	is	8	bytes		(e.g.,	dec	alpha)	*/

															*(((unsigned	char*)&output)		)	=	*(((unsigned	char*)&input)+7);
															*(((unsigned	char*)&output)+1)	=	*(((unsigned	char*)&input)+6);
															*(((unsigned	char*)&output)+2)	=	*(((unsigned	char*)&input)+5);
															*(((unsigned	char*)&output)+3)	=	*(((unsigned	char*)&input)+4);

															out	+=	4;
															*(((unsigned	char*)out)			)	=	*(((unsigned	char*)&input)+3);
															*(((unsigned	char*)out)+	1)	=	*(((unsigned	char*)&input)+2);
															*(((unsigned	char*)out)+	2)	=	*(((unsigned	char*)&input)+1);
															*(((unsigned	char*)out)+	3)	=	*(((unsigned	char*)&input)		);
							}

							return(output);
}

int	trick_byteswap_int(							/*	RETURN:	--	swapped	bytes	*/
														int	input)						/*	IN:	--	bytes	to	be	swapped	*/
{
							int	output;

							*(	((unsigned	char	*)&output)					)	=	*(	((unsigned	char	*)&input)	+	3	);
							*(	((unsigned	char	*)&output)	+	1	)	=	*(	((unsigned	char	*)&input)	+	2	);
							*(	((unsigned	char	*)&output)	+	2	)	=	*(	((unsigned	char	*)&input)	+	1	);
							*(	((unsigned	char	*)&output)	+	3	)	=	*(	((unsigned	char	*)&input)		);
							return(output);
}

short	trick_byteswap_short(								/*	RETURN:	--	swapped	bytes			*/
																	short	input)						/*	IN:	--	bytes	to	be	swapped	*/
{
							short	output;

							*(	((unsigned	char	*)&output)					)	=	*(	((unsigned	char	*)&input)	+	1	);
							*(	((unsigned	char	*)&output)	+	1	)	=	*(	((unsigned	char	*)&input)		);
							return(output);
}

}
