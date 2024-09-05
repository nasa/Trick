#include	<stdio.h>
#include	<math.h>
#include	<iostream>
#include	"Unit.hh"

using	namespace	std;

//	GLOBAL
int	test_number	=	0;
int	n_failures	=	0;

#define	TOLERANCE	0.00001

int	pretty_close	(double	a,	double	b)	{
					int	close	=	1;;

					if	(	fabs(a	+	b)	>	TOLERANCE	)	{
			if	(	(	(	2.0	*	fabs(a-b))	/	fabs(a+b))	>	TOLERANCE	)		{
								return	0;
			}
					}
					return	close;
}


void	validate_conv_fn	(Unit	*u1,	Unit	*u2,	double	c1,	double	c0)	{
					UCFn	*cf;

					test_number	++;
					cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
					cout	<<	"expecting	{"	<<	c1	<<	","	<<	c0	<<	"}"	<<	endl;
					cf		=	u1->Conversion_to(u2);
					cout	<<	"Conversion	function:	"	<<	cf	<<	endl;

					if	(	(	pretty_close(c0,	cf->C[0]	))	&&
			(	pretty_close(c1,	cf->C[1])))	{
			cout	<<	"RESULT:	PASS"	<<	endl;
					}	else	{
			cout	<<	"RESULT:	FAIL"	<<	endl;
									n_failures++	;
					}

					delete	cf;
}


void	test_conversion	(	Unit	*u1,	Unit	*u2)	{
					UCFn	*cf;

					test_number	++;
					try	{
			cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
			cout	<<	"Try	"	<<	u1	<<	"	to	"	<<	u2	<<	endl;
			cf		=	u1->Conversion_to(u2);
			cout	<<	"Conversion	function:	"	<<	cf	<<	endl;
			delete	cf;
			cout	<<	"RESULT:	PASS."	<<	endl	<<	endl;
					}	catch	(Unit::CONVERSION_ERROR)	{
			cout	<<	"RESULT:	FAIL."	<<	endl	<<endl;
									n_failures++	;
					}
}

void	test_conversion_exception	(	Unit	*u1,	Unit	*u2)	{
					UCFn	*cf;

					test_number	++;
					try	{
			cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
			cout	<<	"Try	"	<<	u1	<<	"	to	"	<<	u2	<<	"	(We	expect	an	exception)"	<<	endl;
			cf		=	u1->Conversion_to(u2);
			cout	<<	"Conversion	function:	"	<<	cf	<<	endl;
			delete	cf;
			cout	<<	"RESULT:	FAIL."	<<	endl	<<	endl;
									n_failures++	;
					}	catch	(Unit::CONVERSION_ERROR)	{
			cout	<<	"RESULT:	PASS."	<<	endl	<<	endl;
					}
}

void	test_unit_creation	(const	char	*u_str)	{
					Unit	*	u;

					test_number	++;
					try	{
			cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
			cout	<<	"Attempt	to	create	a	Unit	\""	<<	u_str	<<	"\""	<<	endl;
			u	=	new	Unit(u_str);
			cout	<<	u	<<	endl;
			delete	u;
			cout	<<	"RESULT:	PASS."	<<	endl	<<	endl;
					}	catch	(Unit::CONVERSION_ERROR)	{
			cout	<<	"RESULT:	FAIL."	<<	endl	<<	endl;
									n_failures++	;
					}
}

void	test_unit_exception	(const	char	*u_str)	{
					Unit	*	u;

					test_number	++;
					try	{
			cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
			cout	<<	"Attempt	to	create	a	Unit	\""	<<	u_str	<<	"\""	<<	"(We	expect	a	an	exception.)"	<<	endl;
			u	=	new	Unit(u_str);
			cout	<<	u	<<	endl;
			delete	u;
			cout	<<	"RESULT:	FAIL."	<<	endl	<<	endl;
									n_failures++	;
					}	catch	(Unit::CONVERSION_ERROR)	{
			cout	<<	"RESULT:	PASS."	<<	endl	<<	endl;
					}
}


int	main	(int	argc,	char	*	argv[]	)	{

cout	<<	"==================================================================================."	<<	endl;
cout	<<	"Test	Suite	for	the	\"C++\"	Language	Interface	to	the	Trick	Unit	Conversion	Package."	<<	endl;
cout	<<	"==================================================================================."	<<	endl;

					cout	<<	endl	<<	"TIME	CONVERSIONS:"	<<	endl;
					{
			Unit	*Second		=	new	Unit("s");
			Unit	*Minute		=	new	Unit("min");
			Unit	*Hour				=	new	Unit("hr");
			Unit	*Day					=	new	Unit("day");

			validate_conv_fn	(Second	,Second,																												1.0,	0.0	);
			validate_conv_fn	(Second	,Minute,																					(1.0/60.0),	0.0	);
			validate_conv_fn	(Second	,Hour,																(1.0/(60.0*60.0)),	0.0	);
			validate_conv_fn	(Second	,Day,												(1.0/(60.0*60.0*24.0)),	0.0	);

			validate_conv_fn	(Minute,	Second,																											60.0,	0.0	);
			validate_conv_fn	(Hour,			Second,																									3600.0,	0.0	);
			validate_conv_fn	(Day,				Second,																								86400.0,	0.0	);
					}

					cout	<<	endl	<<	"LENGTH	CONVERSIONS:"	<<	endl;
					{
			Unit	*Meter										=	new	Unit("m");
			Unit	*Foot											=	new	Unit("ft");
			Unit	*Inch											=	new	Unit("in");
			Unit	*Millimeter					=	new	Unit("mm");
			Unit	*Centimeter					=	new	Unit("cm");
			Unit	*Kilometer						=	new	Unit("km");
			Unit	*Yard											=	new	Unit("yd");
			Unit	*Mile											=	new	Unit("mi");
			Unit	*Nautical_Mile		=	new	Unit("n.m.");
			Unit	*Kilofoot							=	new	Unit("kft");

			validate_conv_fn	(Meter	,Meter,																																1.0,	0.0	);
			validate_conv_fn	(Meter	,Foot,																													3.28084,	0.0	);
			validate_conv_fn	(Meter	,Inch,																													39.3701,	0.0	);
			validate_conv_fn	(Meter	,Millimeter,																								1000.0,	0.0	);
			validate_conv_fn	(Meter	,Centimeter,																									100.0,	0.0	);
			validate_conv_fn	(Meter	,Kilometer,																										0.001,	0.0	);
			validate_conv_fn	(Meter	,Yard,																													1.09361,	0.0	);
			validate_conv_fn	(Meter	,Mile,																						(1.0/1609.344),	0.0	);
			validate_conv_fn	(Meter	,Nautical_Mile,															(1.0/1852.0),	0.0	);
			validate_conv_fn	(Meter	,Kilofoot,																(3.28084/1000.0),	0.0	);

			validate_conv_fn	(Foot	,Inch,																																		12.0,	0.0);
			validate_conv_fn	(Yard	,Foot,																																			3.0,	0.0);
			validate_conv_fn	(Yard	,Inch,																																		36.0,	0.0);
			validate_conv_fn	(Mile	,Foot,																																5280.0,	0.0);
			validate_conv_fn	(Nautical_Mile	,Foot,																			6076.11549,	0.0);
			validate_conv_fn	(Kilofoot	,Foot,																												1000.0,	0.0);

					}

					cout	<<	endl	<<	"ANGLE	CONVERSIONS:"	<<	endl;
					{
			Unit	*Radian						=	new	Unit("r");
			Unit	*Degree						=	new	Unit("d");
			Unit	*ArcSecond			=	new	Unit("as");
			Unit	*ArcMinute			=	new	Unit("am");
			Unit	*Revolution		=	new	Unit("rev");
			Unit	*Milliradian	=	new	Unit("mr");

#define	PI	3.14159265

			validate_conv_fn	(Radian,	Radian,																															1.0,	0.0);
			validate_conv_fn	(Radian,	Degree,																								(180.0/PI),	0.0);
			validate_conv_fn	(Radian,	ArcSecond,											(180.0*60.0*60.0/PI),	0.0);
			validate_conv_fn	(Radian,	ArcMinute,																(180.0*60.0/PI),	0.0);
			validate_conv_fn	(Radian,	Revolution,																(1.0/(2.0*PI)),	0.0);
			validate_conv_fn	(Radian,	Milliradian,																							1000.0,	0.0);

			validate_conv_fn	(Revolution,	Degree,																									360.0,	0.0);
			validate_conv_fn	(Revolution,	Radian,																						(2.0*PI),	0.0);
			validate_conv_fn	(ArcMinute,		Degree,																				(1.0/60.0),	0.0);
			validate_conv_fn	(ArcSecond,		Degree,																		(1.0/3600.0),	0.0);
					}

					cout	<<	endl	<<	"MASS	CONVERSIONS:"	<<	endl;
					{
			Unit	*Kilogram						=	new	Unit("kg");
			Unit	*Slug										=	new	Unit("sl");
			Unit	*PoundMass					=	new	Unit("lbm");
			Unit	*Gram										=	new	Unit("g");
			Unit	*MetricTon					=	new	Unit("mt");

			validate_conv_fn	(Kilogram,	Kilogram,																											1.0,	0.0);
			validate_conv_fn	(Kilogram,	Slug,																			(1.0/14.593902),	0.0);
			validate_conv_fn	(Kilogram,	PoundMass,																			2.20462262,	0.0);
			validate_conv_fn	(Kilogram,	Gram,																												1000.0,	0.0);
			validate_conv_fn	(Kilogram,	MetricTon,																	(1.0/1000.0),	0.0);

			validate_conv_fn	(MetricTon,	Kilogram,																							1000.0,	0.0);
					}

					cout	<<	endl	<<	"FORCE	CONVERSIONS:"	<<	endl;
					{
			Unit	*Newton					=	new	Unit("N");
			Unit	*KiloNewton	=	new	Unit("kN");
			Unit	*Ounce						=	new	Unit("oz");
			Unit	*PoundForce	=	new	Unit("lbf");

			validate_conv_fn	(Newton,	Newton,																															1.0,	0.0);
			validate_conv_fn	(Newton,	KiloNewton,																									0.001,	0.0);
			validate_conv_fn	(Newton,	Ounce,																			(16.0/4.4482216),	0.0);
			validate_conv_fn	(Newton,	PoundForce,																					(1/4.4482216),	0.0);
					}

					cout	<<	endl	<<	"VOLTAGE	CONVERSIONS:"	<<	endl;
					{
			Unit	*Volt						=	new	Unit("v");
			Unit	*KiloVolt		=	new	Unit("kv");

			validate_conv_fn	(Volt,	Volt,																																			1.0,	0.0);
			validate_conv_fn	(Volt,	KiloVolt,																													0.001,	0.0);
					}

					cout	<<	endl	<<	"CURRENT	CONVERSIONS:"	<<	endl;
					{
			Unit	*Ampere							=	new	Unit("amp");
			Unit	*MilliAmpere		=	new	Unit("mamp");

			validate_conv_fn	(Ampere,	Ampere,																															1.0,	0.0);
			validate_conv_fn	(Ampere,	MilliAmpere,																							1000.0,	0.0);
					}

					cout	<<	endl	<<	"UNITLESS	CONVERSIONS:"	<<	endl;
					{
			Unit	*DoubleDash	=	new	Unit("--");
			Unit	*Count						=	new	Unit("cnt");
			Unit	*One								=	new	Unit("one");

			validate_conv_fn	(DoubleDash,	DoubleDash,																							1.0,	0.0);
			validate_conv_fn	(DoubleDash,	Count,																												1.0,	0.0);
			validate_conv_fn	(DoubleDash,	One,																														1.0,	0.0);

					}

					cout	<<	endl	<<	"FREQUENCY	CONVERSIONS:"	<<	endl;
					{
									Unit	*Hz		=	new	Unit("Hz");
			Unit	*Mhz	=	new	Unit("Mhz");
			Unit	*Ghz	=	new	Unit("Ghz");

			validate_conv_fn	(Hertz,	Hertz,						1.0,		0.0);
			validate_conv_fn	(Hertz,	MegaHertz,							1000000,		0.0);
			validate_conv_fn	(Hertz,	GigaHertz,							1000000000,	0.0);
					}

//	Note:	Issue	E..4-187	no	longer	allows	the	dubious	"default	units"	interpretation
//	of	double-dash.	Yeah!

					cout	<<	endl	<<	"PRIMITIVE	TEMPERATURE	CONVERSIONS:"	<<	endl;
					{
			double	C_val,	F_val,	K_val,	R_val;

			Unit	*Deg_Celsius				=	new	Unit("C");
			cout	<<	"new	Unit	=	"	<<	Deg_Celsius	<<	endl;
			Unit	*Deg_Kelvin					=	new	Unit("K");
			cout	<<	"new	Unit	=	"	<<	Deg_Kelvin	<<	endl;
			Unit	*Deg_Fahrenheit	=	new	Unit("F");
			cout	<<	"new	Unit	=	"	<<	Deg_Fahrenheit	<<	endl;
			Unit	*Deg_Rankine				=	new	Unit("R");
			cout	<<	"new	Unit	=	"	<<	Deg_Rankine	<<	endl	<<	endl;

			test_conversion	(Deg_Celsius	,Deg_Kelvin	);
			test_conversion	(Deg_Celsius	,Deg_Fahrenheit	);
			test_conversion	(Deg_Celsius	,Deg_Rankine	);
			test_conversion	(Deg_Kelvin		,Deg_Celsius	);
			test_conversion	(Deg_Kelvin		,Deg_Fahrenheit	);
			test_conversion	(Deg_Kelvin		,Deg_Rankine	);
			test_conversion	(Deg_Rankine	,Deg_Celsius	);
			test_conversion	(Deg_Rankine	,Deg_Kelvin	);
			test_conversion	(Deg_Rankine	,Deg_Fahrenheit	);

			cout	<<	"TEST	"	<<	test_number	<<	":		0	Celsius"	<<	endl;
			test_number	++;
			C_val	=	0.0;
			F_val	=	Deg_Celsius->Convert_to(C_val,	"F");
			K_val	=	Deg_Celsius->Convert_to(C_val,	"K");
			R_val	=	Deg_Celsius->Convert_to(C_val,	"R");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	0	Kelvin"	<<	endl;
			test_number	++;
			K_val	=	0.0;
			F_val	=	Deg_Kelvin->Convert_to(K_val,	"F");
			C_val	=	Deg_Kelvin->Convert_to(K_val,	"C");
			R_val	=	Deg_Kelvin->Convert_to(K_val,	"R");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	0	Fahrenheit"	<<	endl;
			test_number	++;
			F_val	=	0.0;
			K_val	=	Deg_Fahrenheit->Convert_to(F_val,	"K");
			C_val	=	Deg_Fahrenheit->Convert_to(F_val,	"C");
			R_val	=	Deg_Fahrenheit->Convert_to(F_val,	"R");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	0	Rankine"	<<	endl;
			test_number	++;
			R_val	=	0.0;
			K_val	=	Deg_Rankine->Convert_to(R_val,	"K");
			C_val	=	Deg_Rankine->Convert_to(R_val,	"C");
			F_val	=	Deg_Rankine->Convert_to(R_val,	"F");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"AVERAGE	HUMAN	BODY	TEMPERATURE"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	36.8	Celsius"	<<	endl;
			test_number	++;
			C_val	=	36.8;
			F_val	=	Deg_Celsius->Convert_to(C_val,	"F");
			K_val	=	Deg_Celsius->Convert_to(C_val,	"K");
			R_val	=	Deg_Celsius->Convert_to(C_val,	"R");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	310.0	Kelvin"	<<	endl;
			test_number	++;
			K_val	=	310.0;
			F_val	=	Deg_Kelvin->Convert_to(K_val,	"F");
			C_val	=	Deg_Kelvin->Convert_to(K_val,	"C");
			R_val	=	Deg_Kelvin->Convert_to(K_val,	"R");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	98.2	Fahrenheit"	<<	endl;
			test_number	++;
			F_val	=	98.2;
			K_val	=	Deg_Fahrenheit->Convert_to(F_val,	"K");
			C_val	=	Deg_Fahrenheit->Convert_to(F_val,	"C");
			R_val	=	Deg_Fahrenheit->Convert_to(F_val,	"R");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			cout	<<	"TEST	"	<<	test_number	<<	":	557.9	Rankine"	<<	endl;
			test_number	++;
			R_val	=	557.9;
			K_val	=	Deg_Rankine->Convert_to(R_val,	"K");
			C_val	=	Deg_Rankine->Convert_to(R_val,	"C");
			F_val	=	Deg_Rankine->Convert_to(R_val,	"F");
			cout	<<	C_val	<<	"C	=	"	<<	F_val	<<	"F	=	"	<<	K_val	<<	"K	=	"	<<	R_val	<<	"R"	<<	endl;
			cout	<<	endl;

			delete	Deg_Celsius;
			delete	Deg_Kelvin;
			delete	Deg_Fahrenheit;
			delete	Deg_Rankine;
					}

					cout	<<	endl	<<	"INVERSES:"	<<	endl;
					{
			Unit	*Seconds	=	new	Unit	("1/s");
			cout	<<	"new	Unit	=	"	<<	Seconds	<<	endl;
			Unit	*Minutes	=	new	Unit	("1/min");
			cout	<<	"new	Unit	=	"	<<	Minutes	<<	endl	<<	endl;

			test_conversion	(Seconds	,Minutes	);
			test_conversion	(Minutes	,Seconds	);

			delete	Seconds;
			delete	Minutes;
					}

					cout	<<	endl	<<	"PRODUCTS	CONTAINING	A	TEMPERATURE	PRIMITIVE:"	<<	endl;
					{
			Unit	*Celsius_seconds	=	new	Unit	("C*s");
			cout	<<	"new	Unit	=	"	<<	Celsius_seconds	<<	endl;
			Unit	*Fahrenheit_minutes	=	new	Unit	("F*min");
			cout	<<	"new	Unit	=	"	<<	Fahrenheit_minutes	<<	endl	<<	endl;

			test_conversion	(Celsius_seconds	,Fahrenheit_minutes	);
			test_conversion	(Fahrenheit_minutes	,Celsius_seconds	);

			delete	Celsius_seconds;
			delete	Fahrenheit_minutes;
					}


					cout	<<	endl	<<	"PRODUCTS	of	TEMPERATURE	PRIMITIVE:"	<<	endl;
					{
			Unit	*Celsius_squared	=	new	Unit	("C2");
			cout	<<	"new	Unit	=	"	<<	Celsius_squared	<<	endl;
			Unit	*Fahrenheit_squared	=	new	Unit	("F2");
			cout	<<	"new	Unit	=	"	<<	Fahrenheit_squared	<<	endl;
			Unit	*Celsius_Fahrenheit	=	new	Unit	("C*F");
			cout	<<	"new	Unit	=	"	<<	Celsius_Fahrenheit	<<	endl	<<	endl;

			test_conversion_exception	(Celsius_squared	,Fahrenheit_squared	);
			test_conversion_exception	(Fahrenheit_squared	,Celsius_squared	);

			test_conversion	(Fahrenheit_squared	,Fahrenheit_squared	);
			test_conversion	(Celsius_Fahrenheit	,Celsius_Fahrenheit);

			delete	Celsius_squared;
			delete	Fahrenheit_squared;
					}


					cout	<<	endl	<<	"RATIOS	CONTAINING	A	TEMPERATURE	PRIMITIVE:"	<<	endl;
					{
			Unit	*Deg_Celsius_per_second=	new	Unit	("C/s");
			cout	<<	"new	Unit	=	"	<<	Deg_Celsius_per_second	<<	endl;
			Unit	*Deg_Fahrenheit_per_second=	new	Unit	("F/s");
			cout	<<	"new	Unit	=	"	<<	Deg_Fahrenheit_per_second	<<	endl	<<	endl;

			test_conversion	(Deg_Celsius_per_second	,Deg_Fahrenheit_per_second	);
			test_conversion	(Deg_Fahrenheit_per_second	,Deg_Celsius_per_second	);

			delete	Deg_Celsius_per_second;
			delete	Deg_Fahrenheit_per_second;
					}


					cout	<<	endl	<<	"COMPLEX	CONVERSIONS:"	<<	endl;
					{
			Unit	*Newton	=	new	Unit("kg*m/s2");
			cout	<<	"new	Unit	=	"	<<	Newton	<<	endl;
			Unit	*Poundal	=	new	Unit("lbm*ft/s2");
			cout	<<	"new	Unit	=	"	<<	Poundal	<<	endl	<<	endl;

			test_conversion	(Newton	,Poundal	);
			test_conversion	(Poundal,	Newton);

			delete	Newton;
			delete	Poundal;
					}

//	Note:	Issue	E..4-187	no	longer	allows	the	dubious	"default	units"	interpretation
//	of	double-dash.	Yeah!

					cout	<<	endl	<<	"COMMUTATIVITY	of	FACTORS:"	<<	endl;
					{
			Unit	*CNewton	=	new	Unit("m*kg/s2");
			cout	<<	"new	Unit	=	"	<<	CNewton	<<	endl;
			Unit	*CPoundal	=	new	Unit("lbm*ft/s*s");
			cout	<<	"new	Unit	=	"	<<	CPoundal	<<	endl	<<	endl;

			test_conversion	(CNewton	,CPoundal	);
			test_conversion	(CPoundal,	CNewton);

			delete	CNewton;
			delete	CPoundal;
					}

					cout	<<	endl	<<	"TEST	COLLECTION	of	DENOMINATORS:"	<<	endl;
					{
			Unit	*Meters_per_minute_squared_1	=	new	Unit	("m/s2");
			cout	<<	"new	Unit	=	"	<<	Meters_per_minute_squared_1	<<	endl;
			Unit	*Meters_per_minute_squared_2	=	new	Unit	("m/s*s");
			cout	<<	"new	Unit	=	"	<<	Meters_per_minute_squared_2	<<	endl;
			Unit	*Meters_per_minute_squared_3	=	new	Unit	("m/s/s");
			cout	<<	"new	Unit	=	"	<<	Meters_per_minute_squared_3	<<	endl	<<	endl;

			test_conversion	(Meters_per_minute_squared_1	,Meters_per_minute_squared_2);
			test_conversion	(Meters_per_minute_squared_1	,Meters_per_minute_squared_3);
			test_conversion	(Meters_per_minute_squared_2	,Meters_per_minute_squared_1);
			test_conversion	(Meters_per_minute_squared_2	,Meters_per_minute_squared_3);
			test_conversion	(Meters_per_minute_squared_3	,Meters_per_minute_squared_1);
			test_conversion	(Meters_per_minute_squared_3	,Meters_per_minute_squared_2);

			delete	Meters_per_minute_squared_1;
			delete	Meters_per_minute_squared_2;
			delete	Meters_per_minute_squared_3;
					}

					cout	<<	endl	<<	"Unit::getUnitName	Functionality:"	<<	endl;
					{
									test_number	++;
									cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
									const	char	*name;

			Unit	*some_unit	=	new	Unit	("m/s2");
			cout	<<	"new	Unit	=	"	<<	some_unit	<<	endl;
									cout	<<	"Get	the	name	using	the	get_name	method	and	compare	it	to	\"m/s2\""	<<	endl;
									cout	<<	"They	should	match."	<<	endl;
									name	=	some_unit->getUnitName();

									if	(	strcmp(name,	"m/s2")	==	0)	{
								cout	<<	"RESULT:	PASS"	<<	endl;
									}	else	{
														cout	<<	"RESULT:	FAIL"	<<	endl;
														n_failures++	;
									}
			delete	some_unit;
					}

					cout	<<	endl	<<	"TEST	Unit::setUnitName	Functionality:"	<<	endl;
					{
									UCFn	*cf;
									test_number	++;
									cout	<<	"TEST	"	<<	test_number	<<	":"	<<	endl;
			Unit	*A_unit	=	new	Unit	("m/s2");
			cout	<<	"Unit	A	=	"	<<	A_unit	<<	endl;
			Unit	*B_unit	=	new	Unit	();
			cout	<<	"Unit	B	=	"	<<	B_unit	<<	endl;
									cout	<<	"Set	B's	Unit	name	to	m/s2	using	Unit::setUnitName"	<<	endl;
									B_unit->setUnitName("m/s2");

									cout	<<	"A	and	B	should	be	compatible."	<<	endl;
									try	{
													cf		=	A_unit->Conversion_to(B_unit);
													cout	<<	"Conversion	function:	"	<<	cf	<<	endl;
													cout	<<	"A	and	B	appear	to	be	compatible."	<<	endl;
									}	catch	(Unit::CONVERSION_ERROR)	{
							cout	<<	"FAIL."	<<	endl	<<endl;
													n_failures++	;
									}

									if	(	(cf->C[0]	==	0.0)	&&	(cf->C[1]	==	1.0)	)	{
														cout	<<	"The	conversion	from	A	to	B	is	correct."	<<	endl;
								cout	<<	"RESULT:	PASS"	<<	endl;
									}	else	{
														cout	<<	"RESULT:	FAIL"	<<	endl;
														n_failures++	;
									}

			delete	A_unit;
			delete	B_unit;

					}




					cout	<<	endl	<<	"SYNTAX	ERRORS:"	<<	endl;
					{
			test_unit_exception	("foo");
			test_unit_exception	("C**m");
			test_unit_exception	("/C");
					}

					cout	<<	"======================================================================="	<<	endl;
					cout	<<	"	END	OF	\"C++\"	TEST	SUITE"	<<	endl;
					cout	<<	"	Number	of	test	failures	=	"	<<	n_failures	<<	endl	<<	endl;
					cout	<<	"	NOTE:"	<<	endl;
					cout	<<	"	Be	sure	to	also	run	\"units_conv_c_test\"	to	complete	the	testing"	<<	endl;
					cout	<<	"	of	the	\"C\"	interface."	<<	endl;
					cout	<<	"======================================================================="	<<	endl;

					return	CONV_OK;
}
