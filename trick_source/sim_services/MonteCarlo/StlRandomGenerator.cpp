
#include	<stdexcept>
#include	"trick/StlRandomGenerator.hh"

StlRandomGenerator::StlRandomGenerator(double										in_param_a,
																																							double										in_param_b,
																																							unsigned	long			in_seed,
																																							StlDistribution	in_dist_type,
																																							StlEngine							in_engine_type)
:			initialSeed(in_seed),
				engineEnum(in_engine_type),
				distEnum(in_dist_type)
{
}

StlRandomGenerator::~StlRandomGenerator()
{
}


///@details	return	a	pointer-to-base-class	for	a	new	StlRandomGeneratorSub	object
///									of	the	input-specified	type
StlRandomGenerator*
StlRandomGeneratorFactory::newGenerator(
				double								in_param_a,
				double								in_param_b,
				unsigned	long	in_seed,
				StlRandomGenerator::StlDistribution	in_dist_type,
				StlRandomGenerator::StlEngine							in_engine_type)
{

#if	(defined(_HAVE_TR1_RANDOM)	||	defined(_HAVE_STL_RANDOM))

#ifdef	_HAVE_STL_RANDOM
#define	UNIFORM_INT_DIST			std::uniform_int_distribution
#define	UNIFORM_REAL_DIST		std::uniform_real_distribution
#define	TRICK_DEFAULT_ENGINE_CLASS	std::mt19937
#else
#define	UNIFORM_INT_DIST			std::uniform_int
#define	UNIFORM_REAL_DIST		std::uniform_real
#define	TRICK_DEFAULT_ENGINE_CLASS	std::ranlux_base_01	//	because	mt19937	doesnt	return	within	canonical	range	for	GCC	4.4.7
#endif

				switch	(in_engine_type	*	1000	+	in_dist_type)	{

				//	-------------	TRICK_DEFAULT_ENGINE	--------------------------------------

				case	StlRandomGenerator::TRICK_DEFAULT_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	TRICK_DEFAULT_ENGINE_CLASS,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::TRICK_DEFAULT_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	TRICK_DEFAULT_ENGINE_CLASS,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::TRICK_DEFAULT_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	TRICK_DEFAULT_ENGINE_CLASS,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;


#ifdef	_HAVE_TR1_RANDOM
				//	-------------	RANLUX_BASE_01_ENGINE	---------------------------------------------

				case	StlRandomGenerator::RANLUX_BASE_01_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux_base_01,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_BASE_01_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux_base_01,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_BASE_01_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux_base_01,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	RANLUX_64_BASE_01_ENGINE	---------------------------------------------

				case	StlRandomGenerator::RANLUX_64_BASE_01_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux64_base_01,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_64_BASE_01_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux64_base_01,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_64_BASE_01_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux64_base_01,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

#else

//	std::minstd_rand	and	mt19937	are	not	provided	for	TR1,	because	GCC	4.4.7	(at	least)
//	they	return	outside	the	canonical	range	0	<=	x	<	1.
//	This	causes	an	infinite	loop	in	std::normal_distribution

				//	-------------	MINSTD_RAND_ENGINE	---------------------------------------------

				case	StlRandomGenerator::MINSTD_RAND_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::minstd_rand,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::MINSTD_RAND_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::minstd_rand,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::MINSTD_RAND_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::minstd_rand,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	MT19937_ENGINE	---------------------------------------------

				case	StlRandomGenerator::MT19937_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::mt19937,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::MT19937_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::mt19937,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::MT19937_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::mt19937,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

//	these	engines	are	all	C++11	only
				//	-------------	MT19937_64_ENGINE	---------------------------------------------

				case	StlRandomGenerator::MT19937_64_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::mt19937_64,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::MT19937_64_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::mt19937_64,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::MT19937_64_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::mt19937_64,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	RANLUX_24_BASE_ENGINE	---------------------------------------------

				case	StlRandomGenerator::RANLUX_24_BASE_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux24_base,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_24_BASE_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux24_base,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_24_BASE_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux24_base,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	RANLUX_44_BASE_ENGINE	---------------------------------------------

				case	StlRandomGenerator::RANLUX_44_BASE_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux48_base,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_44_BASE_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux48_base,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_44_BASE_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux48_base,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	RANLUX_24_ENGINE	---------------------------------------------

				case	StlRandomGenerator::RANLUX_24_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux24,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_24_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux24,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_24_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux24,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	RANLUX_44_ENGINE	---------------------------------------------

				case	StlRandomGenerator::RANLUX_44_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux48,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_44_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux48,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::RANLUX_44_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::ranlux48,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;

				//	-------------	KNUTH_B_ENGINE	---------------------------------------------

				case	StlRandomGenerator::KNUTH_B_ENGINE*1000	+	StlRandomGenerator::GAUSSIAN:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::knuth_b,	std::normal_distribution<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::KNUTH_B_ENGINE*1000	+	StlRandomGenerator::FLAT:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::knuth_b,	UNIFORM_REAL_DIST<double>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
				case	StlRandomGenerator::KNUTH_B_ENGINE*1000	+	StlRandomGenerator::POISSON:
								return	static_cast<	StlRandomGenerator*	>	(
												new	StlRandomGeneratorSub<	std::knuth_b,	std::poisson_distribution<STL_POISSON_TEMPLATE_TYPES>	>
																(in_param_a,	in_param_b,	in_seed,	in_dist_type,	in_engine_type)
								);
								break;
#endif

				default:
								throw	std::invalid_argument(std::string("error:	StlRandomGeneratorFactory::newGenerator	called	for	unimplemented	std::random		(engine,distribution)	combination."));
				}


#else
				//	without	either	_HAVE_TR1_RANDOM	or	_HAVE_STL_RANDOM,	return	null	pointer.
				return	0;	//	????	what	was	I	thinking	here						static_cast<	StlRandomGenerator*	>(	0	);
#endif
}
