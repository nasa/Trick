#include	<iostream>
#include	<sstream>
#include	<stdio.h>
#include	<cmath>
#include	<limits>

#include	"trick/MonteVar.hh"
#include	"trick/MonteVarRandom.hh"
#include	"trick/exec_proto.h"

Trick::MonteVarRandom::MonteVarRandom(std::string	in_name,	Distribution	in_distribution,	std::string	in_unit,	StlEngine	in_engine)	:	engineType(in_engine),	randist(),	stlGenPtr(0)	{
				this->name	=	in_name;
				this->distribution	=	in_distribution;
				this->unit	=	in_unit;

				//	perform	this	init	in	all	cases,	in	case	fall	back	to	Trick-coded	distributions
				trick_gsl_rand_init(&randist);

				if	(NO_ENGINE	!=	engineType)	{
								//	minimum	set	of	randist	defaults	when	using	C++11	distributions
								randist.mu	=	0.0;
								randist.sigma	=	1.0;
								randist.max	=	std::numeric_limits<double>::max();
								randist.min	=	-	randist.max;
								randist.rel_min	=	1;
								randist.rel_max	=	1;
								randist.sigma_range	=	0.0;
				}
				randist.type	=	(TRICK_GSL_TYPE)	distribution;
				randist.sigma_range	=	1;

#if	(defined(_HAVE_TR1_RANDOM)	||	defined(_HAVE_STL_RANDOM))
				unsigned	long	seed	=	randist.seed;
				double	unused	=	0.0;
				StlRandomGenerator::StlDistribution	stlDist	=
								static_cast<StlRandomGenerator::StlDistribution>(distribution);
				StlRandomGenerator::StlEngine	stlEngine	=
								static_cast<StlRandomGenerator::StlEngine>(engineType);
				if	(engineType	!=	NO_ENGINE)	{
								//	note:	in	practice,	these	will	have	to	be	changed	by	calling	set_mu,	etc,
								//	in	the	input	file	after	construction.

								switch	(randist.type)	{
								case	GAUSSIAN:
												stlGenPtr	=	StlRandomGeneratorFactory::newGenerator(randist.mu,	randist.sigma,
																								seed,	stlDist,	stlEngine);
												break;
								case	FLAT:
												//	StlRandomGenerator	min,max	are	always	absolute
												stlGenPtr	=	StlRandomGeneratorFactory::newGenerator(
																								get_absolute_min(),	get_absolute_max(),
																								seed,	stlDist,	stlEngine);
												break;
								case	POISSON:
												stlGenPtr	=	StlRandomGeneratorFactory::newGenerator(randist.mu,	unused,
																								seed,	stlDist,	stlEngine);
												break;
								default:
												break;
								}
				}
#endif
}

Trick::MonteVarRandom::~MonteVarRandom()
{
				delete	stlGenPtr;
}

//	Composite	the	various	properties	of	this	MonteVarRandom.
std::string	Trick::MonteVarRandom::describe_variable()
{
				std::string	dist_list[]	=	{"GAUSSIAN",	"FLAT",	"POISSON"};
				std::stringstream	ss;

				ss	<<	"#NAME:\t\t\t"	<<	this->name	<<	"\n"
							<<	"#TYPE:\t\t\tRANDOM\n"	
							<<	"#UNIT:\t\t\t"	<<	this->unit	<<	"\n"
							<<	"#DISTRIBUTION:\t"	<<	dist_list[this->randist.type]	<<	"\n"
							<<	"#SEED:\t\t\t"	<<	this->randist.seed	<<	"\n"
							<<	"#SIGMA:\t\t\t"	<<	this->randist.sigma	<<	"\n"
							<<	"#MU:\t\t\t"	<<	this->randist.mu	<<	"\n"
							<<	"#MIN:\t\t\t"	<<	this->randist.min	<<	"\n"
							<<	"#MAX:\t\t\t"	<<	this->randist.max	<<	"\n"
							<<	"#REL_MIN:\t\t"	<<	this->randist.rel_min	<<	"\n"
							<<	"#REL_MAX:\t\t"	<<	this->randist.rel_max	<<	"\n";

				return	ss.str();
}

void	Trick::MonteVarRandom::set_seed(unsigned	long	seed)	{
				randist.seed	=	seed;
				if	(engineType	!=	NO_ENGINE	&&	stlGenPtr)	{
								stlGenPtr->set_seed(seed);
				}	else	{
								trick_gsl_rand_seed(&randist);
				}
}

void	Trick::MonteVarRandom::set_sigma(double	sigma)	{
				if	(engineType	!=	NO_ENGINE	&&	TRICK_GSL_GAUSS	==	randist.type	&&	stlGenPtr)	{
								randist.sigma	=	sigma;
								updateStlRandom();
				}	else	{
								randist.sigma	=	sigma;
				}
}

void	Trick::MonteVarRandom::set_sigma_range(int	sigma_range)	{
				randist.sigma_range	=	sigma_range;
}

void	Trick::MonteVarRandom::set_mu(double	mu)	{
				if	(engineType	!=	NO_ENGINE	&&	stlGenPtr)	{
								randist.mu	=	mu;
								updateStlRandom();
				}	else	{
								randist.mu	=	mu;
				}
}

void	Trick::MonteVarRandom::set_min(double	min)	{
				if	(engineType	!=	NO_ENGINE	&&	stlGenPtr)	{
								randist.min	=	min;
								updateStlRandom();
				}	else	{
								randist.min	=	min;
				}
}

void	Trick::MonteVarRandom::set_max(double	max)	{
				if	(engineType	!=	NO_ENGINE	&&	stlGenPtr)	{
								randist.max	=	max;
								updateStlRandom();
				}	else	{
								randist.max	=	max;
				}
}

void	Trick::MonteVarRandom::set_min_is_relative(bool	relative)	{
				randist.rel_min	=	(int)relative;
				updateStlRandom();
}

void	Trick::MonteVarRandom::set_max_is_relative(bool	relative)	{
				randist.rel_max	=	(int)relative;
				updateStlRandom();
}

void	Trick::MonteVarRandom::set_uniform_generator(uniform_generator	uniform)	{
				randist.uniform	=	uniform;
}

std::string	Trick::MonteVarRandom::get_next_value()	{
				TRICK_GSL_RETURN_TYPE	return_value;
				char	buffer[128];

				return_value.d	=	0;
				if	(stlGenPtr)	{
								double	sigma_range	=	static_cast<double>(randist.sigma_range)	*	randist.sigma;
								double	min	=	get_absolute_min();
								double	max	=	get_absolute_max();

								unsigned	int	count	=	0;
								while	(count	<	100)	{
												return_value	=	(*stlGenPtr)();
												count++;

												if	(return_value.d	<	min	||	return_value.d	>	max)	{
																continue;
												}	else	{

																if	(0	==	randist.sigma_range	//	0==	means	sigma_range	algorithm	is	turned	off
																				||	std::fabs(return_value.d	-	randist.mu)	<=	sigma_range)	{
																				break;
																}
												}
								}
								if	(count	>=	100)	{
												char	string[100];
												snprintf(string,	sizeof(string),	"Trick:MonteVarRandom	failed	to	generate	a	random	value	for	variable	\"%s\"\n",	name.c_str());
												exec_terminate_with_return(-1,	__FILE__,	__LINE__,	string);
								}

				}	else	{
								if	(trick_gsl_rand(&randist,	&return_value)	!=	0)	{
												char	string[100];
												snprintf(string,	sizeof(string),	"Trick:MonteVarRandom	failed	to	generate	a	random	value	for	variable	\"%s\"\n",	name.c_str());
												exec_terminate_with_return(-1,	__FILE__,	__LINE__,	string);
								}
				}

				switch	(randist.type)	{
								case	TRICK_GSL_POISSON:
												//	STL	returns	int,	GSL	returns	unsigned	int
												if	(stlGenPtr)	{
																snprintf(buffer,	sizeof(buffer),	"	%d",	return_value.ii);
												}	else	{
																snprintf(buffer,	sizeof(buffer),	"	%u",	return_value.ui);
												}
												value	=	buffer;
												break;
								case	TRICK_GSL_GAUSS:
								case	TRICK_GSL_FLAT:
								default:
												snprintf(buffer,	sizeof(buffer),	"%.15g",	return_value.d);
												value	=	buffer;
												break;
				}

				if	(unit.empty())	{
								return	name	+	std::string("	=	")	+	value	;
				}	else	{
								return	name	+	std::string("	=	trick.attach_units(\"")	+	unit	+	std::string("\",	")		+	value	+
															std::string(")")	;
				}
}

///@details	Update	the	full	set	of	STL	random	settings
///	(Call	after	any	of	the	settings:	randist.mu	.set_min,	max,	min_is_relative,	max_is_relative
///	are	called.)
void
Trick::MonteVarRandom::updateStlRandom()	{

				if	(stlGenPtr)	{

								switch	(randist.type)	{
								case	GAUSSIAN:
												stlGenPtr->set_param(randist.mu,	randist.sigma);
												break;
								case	FLAT:
												//	StlRandomGenerator	min,max	are	always	absolute
												stlGenPtr->set_param(get_absolute_min(),	get_absolute_max());
												break;
								case	POISSON:
												stlGenPtr->set_param(randist.mu);
												break;
								default:
												break;
								}
				}
}

const	TRICK_GSL_RANDIST&	Trick::MonteVarRandom::get_random_distribution()	{
				return	randist;
}
