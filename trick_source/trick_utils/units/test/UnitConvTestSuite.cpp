
#include	<gtest/gtest.h>
#include	"trick/units_conv.h"

#define	TOL	1e-7
/*
double	tolerance(int	exp)
{
		return	pow(10,	exp);
}
*/
/*********************WARNING******************************************
	*	1.	new_units	allocates	memory	for	a	Units_t	object	using	new
	*	Use	clean_up_Units_t()	to	deallocate	the	memory	and	set	the	pointer	to	NULL
	*
	*	2.	Do	not	call	clean_up_Units_t()	on	unitialized	pointers!
	*	Initialize/Define	pointers	at	declaration
	*	Declared	pointers	that	don't	point	to	a	valid	address	or	NULL	will
	*	cause	an	error
**********************************************************************/

void	clean_up_Units_t(Units_t**	ptr)
{
		if(*ptr	!=	(Units_t*)	NULL)
		{
						delete	*ptr;
		}
		else
		{
				std::cerr	<<	"WARNING:	Call	to	clean_up_Units_t()	on	a	NULL	pointer"	<<	std::endl
														<<	"Calling	function	should	check	for	non-NULL	value	of	pointer"	<<	std::endl
														<<	"before	calling	clean_up_Units_t()"	<<	std::endl;
		}
		*ptr	=	(Units_t*)	NULL;
}

TEST(UnitsInterface,	BogusUnit)
{
		Units_t	*Bogus	=	(Units_t*)	NULL;

		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;

		ASSERT_EQ((Units_t*)	NULL,	Bogus	=	new_units("foo"));
}

TEST(UnitsInterface,	Newton_to_Pound)
{
		UnitsConvFn_t	Newton_to_Pound;
		UnitsConvFn_t	Newton_to_Pound2;
		Units_t*	Newton	=	(Units_t*)	NULL;
		Units_t*	Pound	=	(Units_t*)	NULL;
		double	pounds;

		ASSERT_EQ(CONV_OK,	conv_fn_s("kg*m/s2",	"lbm*ft/s2",	&Newton_to_Pound));

		Newton	=	new_units("kg*m/s2");
		Pound	=	new_units("lbm*ft/s2");

		ASSERT_NE((Units_t*)	NULL,	Newton);
		ASSERT_NE((Units_t*)	NULL,	Pound);

		ASSERT_EQ(CONV_OK,	conv_fn_u(Newton,	Pound,	&Newton_to_Pound2));

		EXPECT_EQ(0,	memcmp(&Newton_to_Pound,	&Newton_to_Pound2,	sizeof(UnitsConvFn_t)));

		pounds	=	convert_units(1.0,	&Newton_to_Pound);

		ASSERT_NEAR((1.0/(0.45359237*0.3048)),	pounds,	TOL);

		clean_up_Units_t(&Newton);
		clean_up_Units_t(&Pound);
		ASSERT_EQ((Units_t*)	NULL,	Newton);
		ASSERT_EQ((Units_t*)	NULL,	Pound);
}
