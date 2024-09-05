/*
PURPOSE:	(Units	Conversion	C++	Language	Interface)

PROGRAMMERS:
				(((John	M.	Penn)	(L-3Com/Titan)	(May	2006)	(v1.0)
						(Initial	Release)))
*/

#include	<string>
#include	"trick/Unit.hh"
#include	"trick/units_conv.h"

Unit::Unit()	{
				const	char	*name	=	"--";
				if	((this->units	=	new_units(name))	==	NULL)	{
								throw	Unit::CONVERSION_ERROR();
				}
				this->units_name	=	name;
}


Unit::Unit(const	char	*name)	{
				if	((this->units	=	new_units(name))	==	NULL)	{
								throw	Unit::CONVERSION_ERROR();
				}
				this->units_name	=	name;
}


Unit::~Unit()	{
				CONV_FREE(this->units);
}

std::string	Unit::setUnitName(const	char	*name)	{
				CONV_FREE(this->units);

				if	((this->units	=	new_units(name))	==	NULL)	{
								throw	Unit::CONVERSION_ERROR();
				}
				this->units_name	=	name;
				return(this->units_name);
}

double	Unit::Convert_to	(double	val,	const	Unit	*other)	{
				UnitsConvFn_t	conv_fn;

				if	(conv_fn_u(this->units,	other->units,	&conv_fn)	!=	CONV_OK)	{
								throw	Unit::CONVERSION_ERROR();
				}
				return	(	conv_fn.C[1]*	val	+	conv_fn.C[0]	);
}

double	Unit::Convert_to	(double	val,	const	char	*in_units_name)	{
				Unit	*other	=	new	Unit(in_units_name);
				double	conv_val	=	this->Convert_to(val,	other);
				delete	other;
				return	(conv_val);
}

UCFn	*Unit::Conversion_to	(const	Unit	*other)	{
				UnitsConvFn_t	conv_fn;

				if	(conv_fn_u(this->units,	other->units,	&conv_fn)	!=	CONV_OK)	{
								throw	CONVERSION_ERROR();
				}
				return	(	new	UCFn(other->units_name.c_str(),	this->units_name.c_str(),	conv_fn.C[1],	conv_fn.C[0])	);
}


UCFn	*Unit::Conversion_to	(const	char	*in_units_name)	{
				UCFn	*ucf;
				Unit	*	other;
				if	((other	=	new	Unit(in_units_name))==NULL)	{
								throw	CONVERSION_ERROR();
				}

				ucf	=	this->Conversion_to(other);

				delete	other;
				return	ucf;
}

std::ostream&	operator<<	(std::ostream&	s,	const	Unit	*u)	{
				return	s	<<	u->units_name;
};
