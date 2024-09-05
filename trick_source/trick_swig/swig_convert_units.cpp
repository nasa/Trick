
#include	<Python.h>
#include	<iostream>
#include	<udunits2.h>

#include	"trick/UdUnits.hh"

int	convert_united_value(	std::string	&	to_units	,	std::string	&	from_units	,	long	long	*	val	)	{
				if	(	from_units.compare("1")	)	{
								ut_unit	*	from	=	ut_parse(Trick::UdUnits::get_u_system(),	from_units.c_str(),	UT_ASCII)	;
								if	(	!from	)	{
												PyErr_SetString(PyExc_AttributeError,(std::string("could	not	covert	from	units	"+from_units).c_str()));
												return	-1	;
								}
								ut_unit	*	to	=	ut_parse(Trick::UdUnits::get_u_system(),	to_units.c_str(),	UT_ASCII)	;
								if	(	!to	)	{
												PyErr_SetString(PyExc_AttributeError,(std::string("could	not	covert	to	units	"+to_units).c_str()));
												return	-1	;
								}

								cv_converter	*	converter	=	ut_get_converter(from,to)	;
								if	(	converter	)	{
												*val	=	(long	long)cv_convert_double(converter,	(double)*val	)	;
												cv_free(converter)	;
								}	else	{
												PyErr_SetString(PyExc_AttributeError,"Units	conversion	Error");
												return	-1	;
								}

								ut_free(from)	;
								ut_free(to)	;
				}
				return	0	;
}

int	convert_united_value(	std::string	&	to_units	,	std::string	&	from_units	,	double	*	val	)	{

				if	(	from_units.compare("1")	)	{
								ut_unit	*	from	=	ut_parse(Trick::UdUnits::get_u_system(),	from_units.c_str(),	UT_ASCII)	;
								if	(	!from	)	{
												PyErr_SetString(PyExc_AttributeError,(std::string("could	not	covert	from	units	"+from_units).c_str()));
												return	-1	;
								}
								ut_unit	*	to	=	ut_parse(Trick::UdUnits::get_u_system(),	to_units.c_str(),	UT_ASCII)	;
								if	(	!to	)	{
												PyErr_SetString(PyExc_AttributeError,(std::string("could	not	covert	to	units	"+to_units).c_str()));
												return	-1	;
								}

								cv_converter	*	converter	=	ut_get_converter(from,to)	;
								if	(	converter	)	{
												*val	=	cv_convert_double(converter,	*val	)	;
												cv_free(converter)	;
								}	else	{
												PyErr_SetString(PyExc_AttributeError,"Units	conversion	Error");
												return	-1	;
								}

								ut_free(from)	;
								ut_free(to)	;
				}
				return	0	;
}

int	scale_united_value(	std::string	&	to_units	,	std::string	&	from_units	,	long	long	*	val	)	{
				return	convert_united_value(to_units,	from_units,	val)	;
}

int	scale_united_value(	std::string	&	to_units	,	std::string	&	from_units	,	double	*	val	)	{
				return	convert_united_value(to_units,	from_units,	val)	;
}

