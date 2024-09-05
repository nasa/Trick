
#include	<iostream>
#include	<string.h>
#include	<stdlib.h>
#include	"trick/UnitsMap.hh"

//	Class	static	pointer	initialization
Trick::UnitsMap	*	Trick::UnitsMap::pInstance	=	NULL	;

//	Memeber	Function
Trick::UnitsMap	*	Trick::UnitsMap::units_map()	{
				if	(	pInstance	==	NULL	)	{
								pInstance	=	new	Trick::UnitsMap()	;
				}

				return(pInstance)	;
}	;

//	Destructor
Trick::UnitsMap::~UnitsMap()	{

				std::map<std::string,	char	*	>::iterator	pit	;

				for	(	pit	=	param_units.begin()	;	pit	!=	param_units.end()	;	++pit	)	{
								free((*pit).second)	;
				}
				param_units.clear()	;

}

//	Member	Function
int	Trick::UnitsMap::add_param(std::string	param	,	const	char	*	units)	{
	/**
		*	@par	Design	Details:
		*	@li	Adds	a	variable	with	specified	units	to	the	map.	The	key	is	the	name	of	a	variable	and	the	value	is	the	units	of	the	variable.
		*/
				param_units[param]	=	strdup(units)	;
				return(0)	;
}

//	Memeber	Function
std::string	Trick::UnitsMap::get_units(	std::string	param	)	{
	/**
		*	@par	Design	Details:
		*	@li	Finds	the	specified	variable	in	the	map	and	returns	the	units	of	the	found	variable.
		*					If	the	variable	can	not	be	found,	returns	"1"	(unitless).
		*/
				if	(	param_units.find(param)	!=	param_units.end()	)	{
								return(std::string(param_units[param]))	;
				}	else	{
								return(std::string("1"))	;
				}
}
