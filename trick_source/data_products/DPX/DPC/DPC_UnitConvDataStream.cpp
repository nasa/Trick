
#include	<string.h>
#include	<udunits2.h>
#include	"DPC/DPC_UnitConvDataStream.hh"

extern	ut_system	*	u_system	;

//	CONSTRUCTOR
DPC_UnitConvDataStream::DPC_UnitConvDataStream(DataStream*	ds,	const	char	*ToUnits,	const	char	*FromUnitsHint	)	{


				ut_unit	*	to	=	NULL	;
				ut_unit	*	from	=	NULL	;
				
				std::string	recorded_units	=	ds->getUnit();

				source_ds	=	ds;

				if	(ToUnits	!=	NULL)	{
								to	=	ut_parse(u_system,	ToUnits,	UT_ASCII)	;
								to_units	=	ToUnits	;
				}

				//	If	the	user	has	specified	a	units	conversion	and	those	units	are	valid	...
				if	(	to	!=	NULL	)	{
								//	If	the	recorded	data	file	doesn't	contain	the	units	in	which	the	data	is	recorded	...
								if	(recorded_units.empty())	{
												//	If	the	user	didn't	give	us	a	hint	as	to	what	the	units	are	(using	var@from_units)	...
												if	((FromUnitsHint	==	NULL)	||	(strcmp(FromUnitsHint,"")	==	0))	{
																//	set	the	from	units	to	the	same	as	the	to	units.
																cf	=	cv_get_trivial()	;
																std::cerr	<<	"ERROR:	Unable	to	to	perform	units	conversion"
																										<<	"	because	the	recorded	data	doesn't	indicate	it's"
																										<<	"	units	and	no	@from_units	hint	is	provided."
																										<<	std::endl;
																std::cerr.flush();
												}	else	{	//	the	user	did	give	us	a	hint.
																from	=	ut_parse(u_system,	FromUnitsHint,	UT_ASCII)	;
																if	(	!	from	)	{
																				std::cerr	<<	"ERROR:	Unable	to	to	perform	units	conversion"
																														<<	"	because	the	recorded	data	doesn't	indicate	it's"
																														<<	"	units	and	although	a	@from_units	hint	is	provided	("
																														<<	"(\""	<<		FromUnitsHint	<<	"\"),	they	are	invalid."
																														<<	std::endl;
																				std::cerr.flush();
																}
												}
								}	else	{	//	the	recorded	data	file	does	"know"	the	units	in	which	the	data	was	recorded,
												//	so	those	will	be	the	units	that	we	convert	from.
												from	=	ut_parse(u_system,	recorded_units.c_str(),	UT_ASCII)	;
												if	(	!from	)	{
																std::cerr	<<	"ERROR:	Unable	to	to	perform	units	conversion	because	the"
																										<<	"	units	in	the	data	recording	file	appear	to	be	corrupt."
																										<<	std::endl;
																std::cerr.flush();
												}
								}

								//	If	we	know	what	units	the	data	was	recorded	in	...
								if	(	from	!=	NULL	)	{
												cf	=	ut_get_converter(from,to)	;
												if	(	cf	==	NULL	)	{
																std::cerr	<<	"ERROR:	Unable	to	convert	from	\""	<<	FromUnitsHint	<<	"\"	to	\""
																										<<	to_units	<<	"\"	because	they	are	incompatible."	<<	std::endl;
																std::cerr.flush();
																cf	=	cv_get_trivial()	;
												}
								}	else	{
												std::cerr	<<	"ERROR:	Unable	to	perform	units	conversion	becuase	the	units"
																						<<	"	that	the	data	is	recorded	in	is	unknown."	<<	std::endl;
												cf	=	cv_get_trivial()	;
								}
				}	else	{	//	The	user	has	not	specified	a	units	conversion	or	the	units	were	not	valid.
								//	If	the	recorded	data	file	doesn't	contain	the	units	in	which	the	data	is	recorded	...
								if	(recorded_units.empty())	{
												//	If	the	user	didn't	give	us	a	hint	as	to	what	the	units	are	(using	var@from_units)	...
												if	((FromUnitsHint	==	NULL)	||	(strcmp(FromUnitsHint,"")	==	0))	{
																cf	=	cv_get_trivial()	;
												}	else	{	//	the	user	did	give	us	a	hint.
																to_units	=	FromUnitsHint	;
																cf	=	cv_get_trivial()	;
												}
								}	else	{	//	the	recorded	data	file	does	"know"	the	units	in	which	the	data	was	recorded,
												to_units	=	recorded_units	;
												cf	=	cv_get_trivial()	;
								}
				}

				if	(to)	ut_free(to)	;
				if	(from)	ut_free(from)	;

				this->begin();
}

//	DESTRUCTOR
DPC_UnitConvDataStream::~DPC_UnitConvDataStream()	{

				cv_free(cf);
				delete	source_ds;
}

//	MEMBER	FUNCTION
int	DPC_UnitConvDataStream::get(double*	timestamp,	double*	paramValue)	{
				double	time,	value;
				int	ret	;

				ret	=	source_ds->get(&time,	&value)	;
				*timestamp		=	time;
				*paramValue	=	cv_convert_double(cf,	value)	;;
				return	ret	;
}

//	MEMBER	FUNCTION
int	DPC_UnitConvDataStream::peek(double*	timestamp,	double*	paramValue)	{
				double	time,	value;

				if	(!	source_ds->peek(&time,	&value)	)	{
								*timestamp		=	time;
								*paramValue	=	cv_convert_double(cf,	value)	;;
								return	(0);
				}	else	{
								return	(-1);
				}
}

//	MEMBER	FUNCTION
std::string	DPC_UnitConvDataStream::getFileName()	{
				return(	source_ds->getFileName());
}

//	MEMBER	FUNCTION
std::string	DPC_UnitConvDataStream::getUnit()	{
				return	to_units	;
}

//	MEMBER	FUNCTION
std::string	DPC_UnitConvDataStream::getTimeUnit()	{
				return(	source_ds->getTimeUnit());
}

//	MEMBER	FUNCTION
void	DPC_UnitConvDataStream::begin()	{
				source_ds->begin();
}

//	MEMBER	FUNCTION
int	DPC_UnitConvDataStream::end()	{
				return(	source_ds->end());
}

//	MEMBER	FUNCTION
int	DPC_UnitConvDataStream::step()	{
				return(	source_ds->step());
}
