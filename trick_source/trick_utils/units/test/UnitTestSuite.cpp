
#include	<gtest/gtest.h>
#include	"trick/Unit.hh"
#include	<math.h>
//#include	"trick_utils/reqs/include/RequirementScribe.hh"

/*********************FIXME**********************************************************************************
	*	1.	The	behavior	of	the	conversion	between	composite	units	containing
	*	temperature	(an	offest)	is	defined	arbitrarily	in	the	source	code,	namely,	that	conversion
	*	between	units	of	temperature	occurs	first	within	a	product	containing
	*	temperature,	and	the	offset	of	the	temperature	conversion	is	ignored
	*	within	a	ratio	containing	temperature.
	*	This	presents	a	problem	for	a	user	who	wants	to	use	a	behavior	which	differs
	*	from	that	described	above.	Realisitically,	the	impact	of	this
	*	problem	is	minimal	since	the	use	of	temperature	in	composite	units	rarely	occurs
	*	in	application.
	*
	*	2.	The	use	of	Unit::CONVERSION_ERROR	as	the	only	exception	for	unit	is	misleading.
	*	Unit::CONVERSION_ERROR	is	thrown	during	invalid	unit	conversions,	but	also	when
	*	initializing	invalid	units.	Possible	solutions	include	a	broader	reaching	name
	*	or	the	inclusion	of	another	exception.
	*
	*	3.	The	use	of	multiplication	in	denominator	of	a	ratio	violates	order	of	operations
	*	kg*m/s*s	=	kg*m/s/s.	It	is	misleading	since	multiplication	seems	to	equate	to	division.
	*	A	possible	solution	would	be	to	allow	the	multiplication	in	the	denominator	within	parentheses.
	*
	*	4.	Within	the	units_conv.c	file	in	the	table	of	units	and	conversion	factors	at	line	463
	*				The	conversion	of	ounces	to	newtons	is	given	as	0.27801385.	This	conversion	can	be	obtained	with	more
	*				precision	by	using	the	conversion	from	pound(force)	to	Newton	and	dividing	by	16.	That	is	the	conversion
	*				of	ounces	to	newtons	can	be	written	as	4.4482216152605/16	=	0.27801385095378.	Note
	*				that	only	14	significant	figures	are	given	in	4.4482216152605	and	only	14	are	used	in	the	result.
	*				After	revision	of	the	conversion	rate,	the	tolerance	used	in	the	test	(around	line	348)	can
	*				be	lowered	to	reflect	the	increase	in	precision.
	*
	*	5.	Within	the	units_conv.c	file	at	line	428	the	conversion	rate	of	arcseconds	to	radians	is	given	as	4.848136811095362e-6
	*				The	conversion	rate	should	be	pi/(180*60*60).	When	doing	this	division	with	20	digits	of	pi,	then	the	result
	*				is	4.8481368110953599...e-6.	To	double	floating	point	precision	that	is	4.848136811095360e-6.	After	correction,
	*				then	the	tolerance	can	be	lowered	to	reflect	the	increase	in	precision.
	*
	*	--Taylor	Dowlen
*	**********************************************************************************************************/


#define	TOL	1e-7
#define	PI	3.14159265358979323846

namespace	Trick	{

double	tolerance(int	exp)
{
		return	pow(10,	exp);
}

/*********************WARNING******************************************
	*	1.	Conversion_to	allocates	memory	for	a	UCFn	object	using	new
	*	Use	clean_up_UCFn()	to	deallocate	the	memory	and	set	the	pointer	to	NULL
	*
	*	2.	Do	not	call	clean_up_UCFn()	on	unitialized	pointers!
	*	Initialize/Define	pointers	at	declaration
	*	Declared	pointers	that	don't	point	to	a	valid	address	or	NULL	will
	*	cause	an	error
**********************************************************************/

#if	0
void	units_requirements(bool	isThrow)	{
		Trick::RequirementScribe	req;

		if	(isThrow)	{
				req.add_requirement("3321868560");
		}	else	{
						req.add_requirement("1724238641");
		}

}
#endif

void	clean_up_UCFn(UCFn**	ptr)
{
		if(*ptr	!=	(UCFn*)	NULL)
		{
						delete	*ptr;
		}
		else
		{
				std::cerr	<<	"WARNING:	Call	to	clean_up_UCFn()	on	a	NULL	pointer"	<<	std::endl
														<<	"Calling	function	should	check	for	non-NULL	value	of	pointer"	<<	std::endl
														<<	"before	calling	clean_up_UCFn()"	<<	std::endl;
		}
		*ptr	=	(UCFn*)	NULL;
}

void	test_conversion_no_throw(std::string	unitString1,	std::string	unitString2,	double	expected_offset,	double	expected_scale,	double	tolerance	=	TOL)
{
		ASSERT_NO_THROW(Unit	unit1(unitString1.c_str()));
		ASSERT_NO_THROW(Unit	unit2(unitString2.c_str()));

		Unit	unit1(unitString1.c_str());
		Unit	unit2(unitString2.c_str());
		UCFn	*cf	=	(UCFn*)	NULL;

		ASSERT_NO_THROW(cf	=	unit1.Conversion_to(&unit2));

		EXPECT_NEAR(expected_offset,	cf->C[0],	tolerance	);
		EXPECT_NEAR(expected_scale,		cf->C[1],	tolerance	);

		clean_up_UCFn(&cf);
		ASSERT_EQ((UCFn*)	NULL,	cf);

		//units_requirements(false);

}

void	test_conversion_no_expect(std::string	unitString1,	std::string	unitString2)
{
		ASSERT_NO_THROW(Unit	unit1(unitString1.c_str()));
		ASSERT_NO_THROW(Unit	unit2(unitString2.c_str()));

		Unit	unit1(unitString1.c_str());
		Unit	unit2(unitString2.c_str());
		UCFn	*cf	=	(UCFn*)	NULL;

		ASSERT_NO_THROW(cf	=	unit1.Conversion_to(&unit2));

		clean_up_UCFn(&cf);
		ASSERT_EQ((UCFn*)	NULL,	cf);
}

void	test_temperature_conversion(std::string	unitString,
																													double	temperature,
																													double	expected_Celsius	=	0.0,
																													double	expected_Kelvin	=	0.0,
																													double	expected_Fahrenheit	=	0.0,
																													double	expected_Rankine	=	0.0,
																													double	tolerance	=	TOL)
{
		ASSERT_NO_THROW(Unit	unit(unitString.c_str()));

		Unit	unit(unitString.c_str());

		double	temp_Celsius	=	unit.Convert_to(temperature,	"C");
		double	temp_Fahrenheit	=	unit.Convert_to(temperature,	"F");
		double	temp_Kelvin	=	unit.Convert_to(temperature,	"K");
		double	temp_Rankine	=	unit.Convert_to(temperature,	"R");

		EXPECT_NEAR(expected_Celsius,	temp_Celsius,	tolerance);
		EXPECT_NEAR(expected_Fahrenheit,	temp_Fahrenheit,	tolerance);
		EXPECT_NEAR(expected_Kelvin,	temp_Kelvin,	tolerance);
		EXPECT_NEAR(expected_Rankine,	temp_Rankine,	tolerance);
}

void	test_conversion_throw(std::string	unitString1,	std::string	unitString2)
{
		ASSERT_NO_THROW(Unit	unit1(unitString1.c_str()));
		ASSERT_NO_THROW(Unit	unit2(unitString2.c_str()));

		Unit	unit1(unitString1.c_str());
		Unit	unit2(unitString2.c_str());
		UCFn*	cf	=	(UCFn*)	NULL;

		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;

		ASSERT_THROW(cf	=	unit1.Conversion_to(&unit2),	Unit::CONVERSION_ERROR);

		if(cf!=	(UCFn*)	NULL)
		{
				clean_up_UCFn(&cf);
		}
		ASSERT_EQ((UCFn*)	NULL,	cf);

		//units_requirements(true);
}

class	UnitConversion	:	public	::testing::Test	{

				protected:
								//Trick::RequirementScribe	testReq;

								UnitConversion()	{}
								~UnitConversion()	{}
								virtual	void	SetUp()	{}
								virtual	void	TearDown()	{}

};

//	Time	Conversions
TEST_F(UnitConversion,	SecondToSecond)
{
		test_conversion_no_throw("s",	"s",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	SecondToMinute)
{
		test_conversion_no_throw("s",	"min",	0.0,	(1.0/60.0),	TOL);
}

TEST_F(UnitConversion,	SecondToHour)
{
		test_conversion_no_throw("s",	"hr",	0.0,	(1.0/(60.0*60.0)),	TOL);
}

TEST_F(UnitConversion,	SecondToDay)
{
		test_conversion_no_throw("s",	"day",	0.0,	(1.0/(60.0*60.0*24.0)),	TOL);
}

TEST_F(UnitConversion,	MinuteToSecond)
{
		test_conversion_no_throw("min",	"s",	0.0,	60.0,	TOL);
}

TEST_F(UnitConversion,	HourToSecond)
{
		test_conversion_no_throw("hr",	"s",	0.0,	(60.0*60.0),	TOL);
}

TEST_F(UnitConversion,	DayToSecond)
{
		test_conversion_no_throw("day",	"s",	0.0,	(60.0*60.0*24.0),	TOL);
}

//Length	Conversion

TEST_F(UnitConversion,	MeterToMeter)
{
		test_conversion_no_throw("m",	"m",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	MeterToFeet)
{
		test_conversion_no_throw("m",	"ft",	0.0,	(1.0/0.3048),	TOL);
}

TEST_F(UnitConversion,	MeterToInch)
{
		test_conversion_no_throw("m",	"in",	0.0,	(12.0/0.3048),	TOL);
}


TEST_F(UnitConversion,	MeterToMilli)
{
		test_conversion_no_throw("m",	"mm",	0.0,	1000.0,	TOL);
}

TEST_F(UnitConversion,	MeterToCenti)
{
		test_conversion_no_throw("m",	"cm",	0.0,	100.0,	TOL);
}

TEST_F(UnitConversion,	MeterToKilometer)
{
		test_conversion_no_throw("m",	"km",	0.0,	(1.0/1000.0),	TOL);
}

TEST_F(UnitConversion,	MeterToYard)
{
		test_conversion_no_throw("m",	"yd",	0.0,	(1.0/(0.3048*3.0)),	TOL);
}

TEST_F(UnitConversion,	MeterToMile)
{
		test_conversion_no_throw("m",	"mi",	0.0,	(1.0/(0.3048*5280.0)),	TOL);
}

TEST_F(UnitConversion,	MeterToNautMile)
{
		test_conversion_no_throw("m",	"n.m.",	0.0,	(1.0/1852.0),	TOL);
}

TEST_F(UnitConversion,	MeterToKilofoot)
{
		test_conversion_no_throw("m",	"kft",	0.0,	(1.0/(1000.0*0.3048)),	TOL);
}


TEST_F(UnitConversion,	FootToInch)
{
		test_conversion_no_throw("ft",	"in",	0.0,	12.0,	TOL);
}

TEST_F(UnitConversion,	YardToFoot)
{
		test_conversion_no_throw("yd",	"ft",	0.0,	3.0,	TOL);
}

TEST_F(UnitConversion,	YardToInch)
{
		test_conversion_no_throw("yd",	"in",	0.0,	(3.0*12.0),	TOL);
}

TEST_F(UnitConversion,	MileToFeet)
{
		test_conversion_no_throw("mi",	"ft",	0.0,	5280.0,	TOL);
}


TEST_F(UnitConversion,	NautMileToFeet)
{
		test_conversion_no_throw("n.m.",	"ft",	0.0,	(1852.0/0.3048),	TOL);
}

TEST_F(UnitConversion,	KilofootToFeet)
{
		test_conversion_no_throw("kft",	"ft",	0.0,	1000.0,	TOL);
}

//Angle	Conversion

TEST_F(UnitConversion,	RadToRad)
{
		test_conversion_no_throw("r",	"r",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	RadToDeg)
{
		test_conversion_no_throw("r",	"d",	0.0,	(180.0/PI),	TOL);
}

TEST_F(UnitConversion,	RadToArcSec)
{
		test_conversion_no_throw("r",	"as",	0.0,	(180.0*60.0*60.0/PI),	tolerance(-9));
}

TEST_F(UnitConversion,	RadToArcMin)
{
		test_conversion_no_throw("r",	"am",	0.0,	(180.0*60.0/PI),	TOL);
}

TEST_F(UnitConversion,	RadToRev)
{
		test_conversion_no_throw("r",	"rev",	0.0,	(1.0/(2.0*PI)),	TOL);
}

TEST_F(UnitConversion,	RadToMilliRad)
{
		test_conversion_no_throw("r",	"mr",	0.0,	1000.0,	TOL);
}

TEST_F(UnitConversion,	RevToDeg)
{
		test_conversion_no_throw("rev",	"d",	0.0,	360.0,	TOL);
}

TEST_F(UnitConversion,	RevToRad)
{
		test_conversion_no_throw("rev",	"r",	0.0,	2*PI,	TOL);
}

TEST_F(UnitConversion,	ArcMinToDeg)
{
		test_conversion_no_throw("am",	"d",	0.0,	(1.0/60.0),	TOL);
}

TEST_F(UnitConversion,	ArcSecToDeg)
{
		test_conversion_no_throw("as",	"d",	0.0,	(1.0/(60.0*60.0)),	TOL);
}

//Mass	Conversions

TEST_F(UnitConversion,	KilogramToKilogram)
{
		test_conversion_no_throw("kg",	"kg",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	KilogramToSlug)
{
		test_conversion_no_throw("kg",	"sl",	0.0,	(1.0/14.59390),	TOL);
}

TEST_F(UnitConversion,	KilogramToPoundMass)
{
		test_conversion_no_throw("kg",	"lbm",	0.0,	(1.0/0.45359237),	TOL);
}

TEST_F(UnitConversion,	KilogramToGram)
{
		test_conversion_no_throw("kg",	"g",	0.0,	1000.0,	TOL);
}

TEST_F(UnitConversion,	KilogramToMetricTon)
{
		test_conversion_no_throw("kg",	"mt",	0.0,	(1.0/1000.0),	TOL);
}

TEST_F(UnitConversion,	MetricTonToKilogram)
{
		test_conversion_no_throw("mt",	"kg",	0.0,	1000.0,	TOL);
}

//Force	Conversions

TEST_F(UnitConversion,	NewtonToNewton)
{
		test_conversion_no_throw("N",	"N",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	NewtonToKiloNewton)
{
		test_conversion_no_throw("N",	"kN",	0.0,	(1.0/1000.0),	TOL);
}

TEST_F(UnitConversion,	NewtonToOunce)
{
		test_conversion_no_throw("N",	"oz",	0.0,	3.596944,	tolerance(-5));
}

TEST_F(UnitConversion,	NewtonToPoundForce)
{
		test_conversion_no_throw("N",	"lbf",	0.0,	0.224809,	TOL);
}

//Voltage	Conversions

TEST_F(UnitConversion,	VoltToVolt)
{
		test_conversion_no_throw("v",	"v",	0.0,	1.0,	TOL);
}


TEST_F(UnitConversion,	VoltToKiloVolt)
{
		test_conversion_no_throw("v",	"kv",	0.0,	(1.0/1000.0),	TOL);
}

//Current	Conversions

TEST_F(UnitConversion,	AmpToAmp)
{
		test_conversion_no_throw("amp",	"amp",	0.0,	1.0,	TOL);
}


TEST_F(UnitConversion,	AmpToMilliAmp)
{
		test_conversion_no_throw("amp",	"mamp",	0.0,	1000.0,	TOL);
}

//Unitless	Conversions

TEST_F(UnitConversion,	DoubleDashToDoubleDash)
{
		test_conversion_no_throw("--",	"--",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	DoubleDashToCount)
{
		test_conversion_no_throw("--",	"cnt",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	DoubleDashToOne)
{
		test_conversion_no_throw("--",	"one",	0.0,	1.0,	TOL);
}

//Frequency	Conversions

TEST_F(UnitConversion,	HertzToHertz)
{
		test_conversion_no_throw("Hz",	"Hz",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	HertzToMegaHertz)
{
		test_conversion_no_throw("Hz",	"MHz",	0.0,	(1.0/1000000.0),	TOL);
}

TEST_F(UnitConversion,	HertzToGigaHertz)
{
		test_conversion_no_throw("Hz",	"GHz",	0.0,	(1.0/1000000000.0),	TOL);
}

//Temperature	Conversions

TEST_F(UnitConversion,	CelsiusToKelvin)
{
		test_conversion_no_throw("C",	"K",	273.15,	1.0,	TOL);
}

TEST_F(UnitConversion,	CelsiusToFahrenheit)
{
		test_conversion_no_throw("C",	"F",	32.0,	1.8,	TOL);
}

TEST_F(UnitConversion,	CelsiusToRankine)
{
		test_conversion_no_throw("C",	"R",	(1.8*273.15),	1.8,	TOL);
}

TEST_F(UnitConversion,	KelvinToCelsius)
{
		test_conversion_no_throw("K",	"C",	-273.15,	1.0,	TOL);
}

TEST_F(UnitConversion,	KelvinToFahrenheit)
{
		test_conversion_no_throw("K",	"F",	(-273.15*1.8+32),	1.8,	TOL);
}

TEST_F(UnitConversion,	KelvinToRankine)
{
		test_conversion_no_throw("K",	"R",	0.0,	1.8,	TOL);
}

TEST_F(UnitConversion,	RankineToCelsius)
{
		test_conversion_no_throw("R",	"C",	-273.15,	(5.0/9.0),	TOL);
}

TEST_F(UnitConversion,	RankineToKelvin)
{
		test_conversion_no_throw("R",	"K",	0.0,	(5.0/9.0),	TOL);
}

TEST_F(UnitConversion,	RankineToFahrenheit)
{
		test_conversion_no_throw("R",	"F",	(-273.15*1.8+32),	1.0,	TOL);
}

TEST_F(UnitConversion,	CelsiusZero)
{
		test_temperature_conversion("C",	0.0,	0.0,	273.15,	32.0,	491.67,	TOL);
}

TEST_F(UnitConversion,	KelvinZero)
{
		test_temperature_conversion("K",	0.0,	-273.15,	0.0,	-459.67,	0.0,	TOL);
}

TEST_F(UnitConversion,	FahrenheitZero)
{
		test_temperature_conversion("F",	0.0,	(-160.0/9.0),	((-160.0/9.0)+273.15),	0.0,	459.67);
}

TEST_F(UnitConversion,	RankineZero)
{
		test_temperature_conversion("R",	0.0,	-273.15,	0.0,	-459.67,	0.0,	TOL);
}

TEST_F(UnitConversion,	CelsiusBodyTemp)
{
		test_temperature_conversion("C",	36.8,	36.8,	309.95,	98.24,	557.91,	TOL);
}

TEST_F(UnitConversion,	KelvinBodyTemp)
{
		test_temperature_conversion("K",	310.0,	36.85,	310.0,	98.33,	558.0,	TOL);
}

TEST_F(UnitConversion,	FahrenheitBodyTemp)
{
		test_temperature_conversion("F",	98.2,	(331.0/9.0),	(2789.35/9.0),	98.2,	557.87,	TOL);
}

TEST_F(UnitConversion,	RankineBodyTemp)
{
		test_temperature_conversion("R",	557.9,	(331.15/9.0),	(2789.5/9.0),	98.23,	557.9,	TOL);
}

//Inverted	Units

TEST_F(UnitConversion,	InvSecondsToInvMinutes)
{
test_conversion_no_throw("1/s",	"1/min",	0.0,	60.0,	TOL);
}

TEST_F(UnitConversion,	InvMinutesToInvSeconds)
{
		test_conversion_no_throw("1/min",	"1/s",	0.0,	(1.0/60.0),	TOL);
}

//Due	to	the	arbitrary	definition	of	the	behavior	of	converstions	of
//composite	units	with	temperature,	no	assertions	are	given	as	to	the	value	of
//the	conversion	function	for	such	conversions.	The	tests	only	validate	that
//conversions	are	valid	and	throw	no	exeption.

//Products	Containing	Temperature	Pimitives

TEST_F(UnitConversion,	CelsiusSecondToFahrenheitMinute)
{
		test_conversion_no_expect("C*s",	"F*min");
}

TEST_F(UnitConversion,	FahrenheitMinuteToCelsiusSecond)
{
		test_conversion_no_expect("F*min",	"C*s");
}

//Products	of	a	temperature	primitive

TEST_F(UnitConversion,	FahrenheitSquaredToFahrenheitSquared)
{
		test_conversion_no_throw("F2",	"F2",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	CelsiusFahrenheitToCelsiusFahrenheit)
{
		test_conversion_no_throw("C*F",	"C*F",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	FahrenheitSquaredToCelsiusSquared)
{
		test_conversion_throw("F2",	"C2");
}

TEST_F(UnitConversion,	CelsiusSquaredToFahrenheitSquared)
{
		test_conversion_throw("C2",	"F2");
}

TEST_F(UnitConversion,	FahrenheitSquaredToCelsiusFahrenheit)
{
		test_conversion_throw("F2",	"C*F");
}


//Ratios	containing	a	temperature	primitive

TEST_F(UnitConversion,	CelsiusPerSecondToFahrenheitPerSecond)
{
		test_conversion_no_expect("C/s",	"F/s");
}

TEST_F(UnitConversion,	FahrenheitPerSecondToCelsiusPerSecond)
{
		test_conversion_no_expect("F/s",	"C/s");
}

TEST_F(UnitConversion,	FahrenheitPerSecondToCelsiusPerMinute)
{
		test_conversion_no_expect("F/s",	"C/min");
}

//Complex	Conversions

TEST_F(UnitConversion,	CompositeNewtonToCompositePoundForce)
{
		test_conversion_no_throw("kg*m/s2",	"lbm*ft/s*s",	0.0,	(1/(0.45359237*0.3048))	,	TOL);
}

TEST_F(UnitConversion,	CompositePoundForceToCompositeNewton)
{
		test_conversion_no_throw("lbm*ft/s2",	"kg*m/s*s",	0.0,	(0.45359237*0.3048),	TOL);
}

TEST_F(UnitConversion,	FootPoundsToNewtonMeters)
{
		test_conversion_no_throw("lbf*ft",	"N*m",	0.0,	(4.4482216152605*0.3048),	TOL);
}

TEST_F(UnitConversion,	FootPoundsToNewtonMetersWithConvertTo)
{
		//testReq.add_requirement("2060756805");

		ASSERT_NO_THROW(Unit	NewtonMeter("N*m"));
		Unit	NewtonMeter("N*m");
		ASSERT_NO_THROW(NewtonMeter.Convert_to(1,	"lbf*ft"));
		double	result	=	NewtonMeter.Convert_to(1,	"lbf*ft");
		ASSERT_NEAR(1.0/(4.4482216152605*0.3048),	result,	TOL);
}

//Communtativity	of	factors

TEST_F(UnitConversion,	CompositePoundForceToCompositeNewtonCommutative)
{
		test_conversion_no_throw("lbm*ft/s*s",	"m*kg/s2",	0.0,	(0.45359237*0.3048),	TOL);
}

TEST_F(UnitConversion,	CompositeNewtonToCompositePoundForceCommutative)
{
		test_conversion_no_throw("m*kg/s2",	"lbm*ft/s*s",	0.0,	(1/(0.45359237*0.3048)),	TOL);
}

//Collection	of	Denominators

TEST_F(UnitConversion,	PerSecondSquaredToPerSecondTimesSecond)
{
		test_conversion_no_throw("kg*m/s2",	"kg*m/s*s",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	PerSecondSquaredToPerSecondPerSecond)
{
		test_conversion_no_throw("kg*m/s2",	"kg*m/s/s",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	PerSecondTimesSecondToPerSecondPerSecond)
{
		test_conversion_no_throw("kg*m/s*s",	"kg*m/s/s",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	PerSecondTimesSecondToPerSecondSquared)
{
		test_conversion_no_throw("kg*m/s*s",	"kg*m/s2",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	PerSecondPerSecondToPerSecondTimesSecond)
{
		test_conversion_no_throw("kg*m/s/s",	"kg*m/s*s",	0.0,	1.0,	TOL);
}

TEST_F(UnitConversion,	PerSecondPerSecondToPerSecondSquared)
{
		test_conversion_no_throw("kg*m/s/s",	"kg*m/s2",	0.0,	1.0,	TOL);
}

//Test	getUnitName()

TEST_F(UnitConversion,	GetUnitName)
{
		//testReq.add_requirement("");

		Unit	meter_per_second_squared("m/s2");
		std::string	unit	=	"m/s2";

		ASSERT_STRCASEEQ(unit.c_str(),	meter_per_second_squared.getUnitName());
}

//Testing	setUnitName()

TEST_F(UnitConversion,	SetUnitName)
{
		//testReq.add_requirement("");

		Unit	meter_per_second_squared("m/s2");
		Unit	copy;
		UCFn	*cf	=	(UCFn*)	NULL;

		copy.setUnitName("m/s2");

		ASSERT_NO_THROW(cf	=	meter_per_second_squared.Conversion_to(&copy));

		EXPECT_NEAR(0.0,	cf->C[0],	TOL);
		EXPECT_NEAR(1.0,	cf->C[1],	TOL);

		clean_up_UCFn(&cf);
		ASSERT_EQ((UCFn*)	NULL,	cf);
}

//Syntax	Error

TEST_F(UnitConversion,	InvalidUnit)
{
		//testReq.add_requirement("2743423356");

		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;
		ASSERT_THROW(Unit	u("foo"),	Unit::CONVERSION_ERROR);
}

TEST_F(UnitConversion,	MissingProduct)
{
		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;
		ASSERT_THROW(Unit	u("C**m"),	Unit::CONVERSION_ERROR);
}

TEST_F(UnitConversion,	InvalidExponent)
{
		//testReq.add_requirement("2743423356");

		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;
		ASSERT_THROW(Unit	u("m4"),	Unit::CONVERSION_ERROR);
		ASSERT_NO_THROW(Unit	u("m3"));
}

TEST_F(UnitConversion,	InvalidRatio)
{
		//testReq.add_requirement("2743423356");

		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;
		ASSERT_THROW(Unit	u("/C"),	Unit::CONVERSION_ERROR);
}


TEST_F(UnitConversion,	InvalidFootPound)
{
		//testReq.add_requirement("2743423356");
		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;
		ASSERT_THROW(Unit	u("lbfft"),	Unit::CONVERSION_ERROR);
}

//Invalid	Conversion	Tests
//Should	throw	an	exception	(exeption	=	PASS)

TEST_F(UnitConversion,	NewtonMeterToInvalidFootPound)
{
		//testReq.add_requirement("2743423356");
		Unit	unit1("N*m");

		std::cerr	<<	"The	purpose	of	this	test	is	to	throw	to	an	exception.	Error	messages	are	expected."	<<	std::endl;
		ASSERT_THROW(unit1.Convert_to(1.0,	"lbfft"),	Unit::CONVERSION_ERROR);
}

TEST_F(UnitConversion,	CelsiusToMeter)
{
		test_conversion_throw("C",	"m");
}

TEST_F(UnitConversion,	MilliAmpsToKilogram)
{
		test_conversion_throw("mamp",	"kg");
}

TEST_F(UnitConversion,	VoltsToMegaHertz)
{
		test_conversion_throw("v",	"MHz");
}

TEST_F(UnitConversion,	RadianToDoubleDash)
{
		test_conversion_throw("r",	"--");
}

TEST_F(UnitConversion,	DoubleDashToMeterPerSecondSquared)
{
		test_conversion_throw("--",	"m/s2");
}

TEST_F(UnitConversion,	SecondToMeterPerSecond)
{
		test_conversion_throw("s",	"m/s");
}

TEST_F(UnitConversion,	KilogramMeterPerSecondToCount)
{
		test_conversion_throw("kg*m/s",	"cnt");
}

TEST_F(UnitConversion,	KilogramMeterPerSecondToMeterSquared)
{
		test_conversion_throw("kg*m/s",	"m2");
}

}
