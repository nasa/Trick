
#define	protected	public

#include	<iostream>
#include	<sys/types.h>
#include	<signal.h>
#include	<string>
#include	<sstream>
#include	<cmath>

#include	"gtest/gtest.h"
#include	"trick/ExecutiveException.hh"
#include	"trick/Executive.hh"
#include	"trick/MonteCarlo.hh"
#include	"trick/MonteVar.hh"
#include	"trick/MonteVarCalculated.hh"
#include	"trick/MonteVarFile.hh"
#include	"trick/MonteVarFixed.hh"
#include	"trick/MonteVarRandom.hh"
#include	"trick/montecarlo_c_intf.h"
#include	"trick/exec_proto.h"
#include	"trick/exec_proto.hh"
#include	"trick/SimObject.hh"
#include	"trick/MemoryManager.hh"
#include	"trick/memorymanager_c_intf.h"
#include	"trick/rand_generator.h"
//#include	"trick/RequirementScribe.hh"

void	sig_hand(int	sig)	;
void	child_handler(int	sig)	;
#if	(__APPLE__	|	__CYGWIN__	|	__INTERIX	)
void	fpe_sig_handler(int	sig)	;
#else
void	fpe_sig_handler(int	sig,	siginfo_t	*	sip,	void	*uap)	;
#endif

namespace	Trick	{

class	testSimObject	:	public	Trick::SimObject	{
				public:

								int	default_data_1()	{
												return	0	;
								}

								int	initialization_1()	{
												return	0	;
								}

								int	return_error()	{
												return	-1	;
								}

								int	throw_exception()	{
												return	exec_terminate("throw_exception",	"exec_terminate	called")	;
								}

								testSimObject()	{
												int	ii	=	0	;
												add_job(0,	ii++,	"monte_master_init",	NULL,	1,	"master_init",	"TRK")	;
												add_job(0,	ii++,	"monte_master_pre",	NULL,	1,	"master_pre",	"TRK")	;
												add_job(0,	ii++,	"monte_master_post",	NULL,	1,	"master_post",	"TRK")	;
												add_job(0,	ii++,	"monte_master_shutdown",	NULL,	1,	"master_shutdown",	"TRK")	;
												add_job(0,	ii++,	"monte_slave_init",	NULL,	1,	"slave_init",	"TRK")	;
												add_job(0,	ii++,	"monte_slave_pre",	NULL,	1,	"slave_pre",	"TRK")	;
												add_job(0,	ii++,	"monte_slave_post",	NULL,	1,	"slave_post",	"TRK")	;
												add_job(0,	ii++,	"monte_slave_shutdown",	NULL,	1,	"slave_shutdown",	"TRK")	;
								}

								virtual	int	call_function(	Trick::JobData	*	curr_job	)	;
								virtual	double	call_function_double(	Trick::JobData	*	curr_job	)	{	(void)curr_job	;	return	0.0	;	}	;
}	;

int	testSimObject::call_function(	__attribute__((unused))	Trick::JobData	*	curr_job	)	{
				int	trick_ret	=	0	;

				return	trick_ret	;
}	;

class	MonteCarloTest	:	public	::testing::Test	{

				protected:
								Trick::MonteCarlo	exec;
								Trick::Executive	executive;
								testSimObject	so1	;
								//Trick::RequirementScribe	req;
								Trick::MemoryManager	mm	;

								MonteCarloTest()	{	exec_register_scheduler(&exec)	;	}
								~MonteCarloTest()	{		}
								virtual	void	SetUp()	{}
								virtual	void	TearDown()	{}

								int	get_class_map_value(std::string	job_class)	{	return	exec.class_map[job_class]	;	}
								Trick::ScheduledJobQueue	*	get_master_init_queue()	{	return	&exec.master_init_queue	;	}
								Trick::ScheduledJobQueue	*	get_master_pre_queue()	{	return	&exec.master_pre_queue	;	}
								Trick::ScheduledJobQueue	*	get_master_post_queue()	{	return	&exec.master_post_queue	;	}
								Trick::ScheduledJobQueue	*	get_master_shutdown_queue()	{	return	&exec.master_shutdown_queue	;	}
								Trick::ScheduledJobQueue	*	get_slave_init_queue()	{	return	&exec.slave_init_queue	;	}
								Trick::ScheduledJobQueue	*	get_slave_pre_queue()	{	return	&exec.slave_pre_queue	;	}
								Trick::ScheduledJobQueue	*	get_slave_post_queue()	{	return	&exec.slave_post_queue	;	}
								Trick::ScheduledJobQueue	*	get_slave_shutdown_queue()	{	return	&exec.slave_shutdown_queue	;	}
}	;

TEST_F(MonteCarloTest	,	Job_Queue_Master_Init)	{
				//req.add_requirement("722132127");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_master_init_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.master_init")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Master_Pre)	{
				//req.add_requirement("587551115");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_master_pre_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.master_pre")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Master_Post)	{
				//req.add_requirement("4165308678");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_master_post_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.master_post")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Master_Shutdown)	{
				//req.add_requirement("3461634900");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_master_shutdown_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.master_shutdown")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Slave_Init)	{
				//req.add_requirement("1412318284");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_slave_init_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.slave_init")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Slave_Pre)	{
				//req.add_requirement("3301658297");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_slave_pre_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.slave_pre")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Slave_Post)	{
				//req.add_requirement("3882184138");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_slave_post_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.slave_post")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Job_Queue_Slave_Shutdown)	{
				//req.add_requirement("350185460");

				Trick::JobData	*	curr_job	;

				EXPECT_EQ(exec_get_time_tic_value()	,	1000000)	;

				exec_add_sim_object(&so1	,	"so1")	;

				curr_job	=	get_slave_shutdown_queue()->get_next_job()	;
				ASSERT_FALSE(	curr_job	==	NULL	)	;
				EXPECT_STREQ(	curr_job->name.c_str()	,	"so1.slave_shutdown")	;

				EXPECT_TRUE(executive.check_all_jobs_handled()	==	0	)	;
}

TEST_F(MonteCarloTest	,	Good_Initialization)	{
				//req.add_requirement("1452306647");

				exec_add_sim_object(&so1	,	"so1")	;
				EXPECT_EQ(exec.execute_monte()	,	0	)	;
}

TEST_F(MonteCarloTest	,	TestDefaultValues)	{

				EXPECT_EQ(exec.get_enabled(),	false)	;
				EXPECT_EQ(exec.get_dry_run(),	false)	;
				EXPECT_EQ(exec.get_localhost_as_remote(),	false)	;
				EXPECT_EQ(exec.get_custom_slave_dispatch(),	false)	;
				EXPECT_EQ(exec.get_timeout(),	120)	;
				EXPECT_EQ(exec.get_max_tries(),	2)	;
				EXPECT_EQ(exec.get_verbosity(),	exec.MC_INFORMATIONAL)	;
				EXPECT_EQ(exec.get_num_runs(),	0)	;
				EXPECT_EQ(exec.get_slave_id(),	0)	;
				EXPECT_EQ(exec.actual_num_runs,	0)	;
				EXPECT_EQ(exec.num_results,	0)	;

}

TEST_F(MonteCarloTest,	TestSettingValues)	{
				//req.add_requirement("2840823120");

				exec.set_enabled(true)	;
				EXPECT_EQ(exec.get_enabled(),	true)	;
				exec.set_dry_run(true)	;
				EXPECT_EQ(exec.get_dry_run(),	true)	;
				exec.set_localhost_as_remote(true)	;
				EXPECT_EQ(exec.get_localhost_as_remote(),	true)	;
				exec.set_custom_slave_dispatch(true)	;
				EXPECT_EQ(exec.get_custom_slave_dispatch(),	true)	;
				exec.set_timeout(60)	;
				EXPECT_EQ(exec.get_timeout(),	60)	;
				exec.set_max_tries(4)	;
				EXPECT_EQ(exec.get_max_tries(),	4)	;
				exec.set_verbosity(exec.MC_NONE)	;
				EXPECT_EQ(exec.get_verbosity(),	exec.MC_NONE)	;
				exec.set_verbosity(exec.MC_ERROR)	;
				EXPECT_EQ(exec.get_verbosity(),	exec.MC_ERROR)	;
				exec.set_verbosity(exec.MC_INFORMATIONAL)	;
				EXPECT_EQ(exec.get_verbosity(),	exec.MC_INFORMATIONAL)	;
				exec.set_verbosity(exec.MC_ALL)	;
				EXPECT_EQ(exec.get_verbosity(),	exec.MC_ALL)	;

}

TEST_F(MonteCarloTest,	TestRanges)	{
				exec.set_num_runs(25)	;
				EXPECT_EQ(exec.get_num_runs(),	25)	;
				EXPECT_EQ(exec.actual_num_runs,	25)	;
				exec.add_range(3,	5)	;
				EXPECT_EQ(exec.actual_num_runs,	3)	;
				exec.add_range(20)	;
				EXPECT_EQ(exec.actual_num_runs,	4)	;
				EXPECT_EQ(exec.in_range(exec.runs[3])	,	true)	;
				EXPECT_EQ(exec.in_range(exec.runs[10]),	false)	;
}

TEST_F(MonteCarloTest,	TestSlaves)	{
				//req.add_requirement("1098748189");

				Trick::MonteSlave	slave0("localhost")	;
				Trick::MonteSlave	slave1("WonderWoman")	;
				EXPECT_EQ(exec.slaves.empty(),	true)	;
				EXPECT_EQ(exec.get_slave_index(1),	-1)	;
				exec.add_slave(&slave0)	;
				EXPECT_EQ(exec.slaves.empty(),	false)	;
				EXPECT_EQ(exec.slaves.size(),	1)	;
				exec.add_slave(&slave1)	;
				EXPECT_EQ(exec.slaves.size(),	2)	;
				EXPECT_EQ(slave0.id,	1)	;
				EXPECT_EQ(slave1.id,	2)	;
				EXPECT_EQ(exec.get_slave_index(1),	0)	;
				EXPECT_EQ(exec.get_slave_index(2),	1)	;
				EXPECT_EQ(slave0.state,	Trick::MonteSlave::MC_UNINITIALIZED)	;
				EXPECT_EQ(slave1.state,	Trick::MonteSlave::MC_UNINITIALIZED)	;
				exec.disable_slave("WonderWoman",	true)	;
				EXPECT_EQ(slave1.state,	Trick::MonteSlave::MC_STOPPED)	;
				EXPECT_EQ(exec.slaves.size(),	2)	;
				exec.disable_slave("WonderWoman",	false)	;
				EXPECT_EQ(slave1.state,	Trick::MonteSlave::MC_UNINITIALIZED)	;
				EXPECT_EQ(slave0.machine_name,	"localhost")	;
				EXPECT_EQ(slave1.machine_name,	"WonderWoman")	;
}

TEST_F(MonteCarloTest,	MonteVarFile)	{
				//req.add_requirement("3932595803");

				//	Test	MonteVarFile
				Trick::MonteVarFile	var0("time_to_fire_1",	"M_jet_firings_inline",	2)	;
				EXPECT_EQ(exec.variables.size(),	0)	;
				exec.add_variable(&var0)	;
				EXPECT_EQ(var0.get_next_value(),	"time_to_fire_1	=	1.0000")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
}

TEST_F(MonteCarloTest,	MonteVarFileWithEOFLine)	{
				Trick::MonteVarFile	var0("time_to_fire_1",	"MonteCarlo_eof_line",	2)	;
				EXPECT_EQ(exec.variables.size(),	0)	;
				exec.add_variable(&var0)	;
				EXPECT_EQ(var0.get_next_value(),	"time_to_fire_1	=	1.0000")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
				EXPECT_EQ(var0.get_next_value(),	"EOF")	;
}

TEST_F(MonteCarloTest,	MonteVarFileWithLineEndedEOF)	{
				Trick::MonteVarFile	var0("time_to_fire_1",	"MonteCarlo_line_ended_eof",	2)	;
				EXPECT_EQ(exec.variables.size(),	0)	;
				exec.add_variable(&var0)	;
				EXPECT_EQ(var0.get_next_value(),	"time_to_fire_1	=	1.0000")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
				EXPECT_EQ(var0.get_next_value(),	"EOF")	;
}

TEST_F(MonteCarloTest,	MonteVarRandom_Gaussian)	{
				std::string	str;
				double	value;
				//req.add_requirement("3932595803");

				//	Test	MonteVarRandom
				//	Gaussian
				Trick::MonteVarRandom	var1("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				var1.set_mu(4.0)	;
				var1.set_min(-4.0)	;
				var1.set_max(4.0)	;
				var1.set_seed(12345);
				var1.set_sigma_range(0)	;
				exec.add_variable(&var1)	;
				var1.get_next_value();
				str	=	var1.value;

				std::stringstream	sstream(str);
				sstream	>>	value;

				EXPECT_LE(value,	8);
				EXPECT_GE(value,	0);

				EXPECT_EQ(var1.name,	"time_to_fire_1")	;
				EXPECT_EQ(var1.distribution,	0)	;
				EXPECT_EQ(var1.randist.mu,	4.0)	;
				EXPECT_EQ(var1.randist.min,	-4.0)	;
				EXPECT_EQ(var1.randist.max,	4.0)	;
				EXPECT_EQ(var1.unit,	"")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
}

TEST_F(MonteCarloTest,	MonteVarRandom_Poisson)	{
				std::string	str;
				double	value;
				//req.add_requirement("3932595803");

				//	Test	MonteVarRandom
				//	Poisson
				Trick::MonteVarRandom	var2("time_to_fire_1",	Trick::MonteVarRandom::POISSON,	"s")	;
				var2.set_mu(4.0)	;
				var2.set_min(-4.0)	;
				var2.set_max(4.0)	;
				var2.set_min_is_relative(0);
				var2.set_max_is_relative(0);
				var2.set_seed(12345);
				var2.set_sigma_range(0)	;
				exec.add_variable(&var2)	;
				var2.get_next_value();
				str	=	var2.value;

				std::stringstream	sstream(str);
				sstream	>>	value;

#if	_HAVE_GSL
				EXPECT_GE(value,	0);
				EXPECT_LE(value,	4);
#else
				EXPECT_EQ(value,	0);
#endif

				EXPECT_EQ(var2.name,	"time_to_fire_1")	;
				EXPECT_EQ(var2.distribution,	2)	;
				EXPECT_EQ(var2.unit,	"s")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
}

TEST_F(MonteCarloTest,	MonteVarRandom_Flat)	{
				std::string	str;
				double	value;
				//req.add_requirement("3932595803");

				//	Test	MonteVarRandom
				//	Flat
				Trick::MonteVarRandom	var3("time_to_fire_4",	Trick::MonteVarRandom::FLAT)	;
				var3.set_mu(4.0)	;
				var3.set_min(-4.0)	;
				var3.set_max(4.0)	;
				var3.set_seed(12345);
				var3.set_sigma_range(0)	;
				exec.add_variable(&var3)	;
				var3.get_next_value();
				str	=	var3.value;

				std::stringstream	sstream(str);
				sstream	>>	value;

				EXPECT_GE(value,	0);
				EXPECT_LE(value,	8);

				EXPECT_EQ(var3.name,	"time_to_fire_4")	;
				EXPECT_EQ(var3.distribution,	1)	;
				EXPECT_EQ(var3.unit,	"")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
}

TEST_F(MonteCarloTest,	MonteVarRandom_NonGSL)	{
				//req.add_requirement("3932595803");

				Trick::MonteVarRandom	var4("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				var4.set_mu(4.0)	;
				var4.set_min(-4.0)	;
				var4.set_max(4.0)	;
				var4.set_seed(12345);
				//	for	this	test,	let	sigma_range	default	to	1			var4.set_sigma_range(0)	;

				exec.add_variable(&var4)	;
				//	Test	MonteVarRandom
				//	non	GSL	library	variables
				EXPECT_EQ(var4.randist.uniform,	LCG1)	;
				var4.set_uniform_generator(TRIPLE)	;
				EXPECT_EQ(var4.randist.uniform,	1)	;
				var4.set_uniform_generator(LCG1)	;
				EXPECT_EQ(var4.randist.uniform,	LCG1)	;
				EXPECT_EQ(var4.randist.sigma_range,	1)	;
				var4.set_sigma_range(8)	;
				EXPECT_EQ(var4.randist.sigma_range,	8)	;
}

TEST_F(MonteCarloTest,	MonteVarFixed)	{
				//req.add_requirement("3932595803");

				//	Test	MonteVarFixed
				Trick::MonteVarFixed	var5("time_to_fire_5",	3.1415)	;
				exec.add_variable(&var5)	;
				EXPECT_EQ(var5.get_next_value(),	"time_to_fire_5	=	3.1415")	;
				EXPECT_EQ(var5.name,	"time_to_fire_5")	;
				EXPECT_EQ(var5.unit,	"")	;
				EXPECT_EQ(exec.variables.size(),	1)	;
}

///@brief	check	that	the	final	distribution	is	correct.	(Bug	6950:	The	non-GSL	was	wrong	for	GAUSSIAN	and	FLAT)
TEST_F(MonteCarloTest,	MonteVarRandom_Gaussian_distributionMatchesSpec)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var6("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				exec.add_variable(&var6)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var6.set_seed(12345)	;
				var6.set_mu(expected_mean)	;
				var6.set_sigma(expected_sigma)	;
				var6.set_min(-10.0)	;
				var6.set_max(10.0)	;
				var6.set_sigma_range(0)	;

				//	maintain	running	sums	for	calculating	mean	and	variance
				double	mean	=	0.0	;
				double	sum_variance	=	0.0	;
				int				n	=	0	;
				double	delta	=	0.0	;
				int				numRuns	=	100	;

				for	(int	ii	=	0;	ii	<=	numRuns;	++ii)	{

								{
												var6.get_next_value()	;
												str	=	var6.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}

								n++	;
								delta	=	value	-	mean	;
								mean	+=	delta	/	(double)n	;
								sum_variance	+=	delta	*	(value	-	mean)	;
				}

				//	final	statistics
				double	variance	=	sum_variance	/	(double)	(n	-	1)	;
				double	sigma	=	sqrt(variance)	;


				EXPECT_NEAR(expected_mean,		mean,		0.05	*	expected_mean)	;
				EXPECT_NEAR(expected_sigma,	sigma,	0.10	*	expected_sigma)	;
}


#ifndef	_HAVE_GSL

///@brief	set_sigma_range(int)	feature	was	only	implemented	for	nonGsl	with	non-C++11	fallbacks
TEST_F(MonteCarloTest,	MonteVarRandom_Gaussian_nonGslSigmaRangeDefaulted_maxDeviationFromMeanIs1Sigma)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var7("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				exec.add_variable(&var7)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var7.set_seed(12345)	;
				var7.set_mu(expected_mean)	;
				var7.set_sigma(expected_sigma)	;
				var7.set_min(-10.0)	;
				var7.set_max(10.0)	;

				//	without	this	call,	default	sigma_range	is	1
				//	var7.set_sigma_range(0)	;

				int				num_runs	=	100	;
				double	sigmas_max_deviation_from_mean	=	0.0	;
				double	deviation_sigmas	=	0.0	;

				for	(int	ii	=	0;	ii	<=	num_runs;	++ii)	{

								{
												var7.get_next_value()	;
												str	=	var7.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}

								deviation_sigmas	=	fabs(	(value	-	expected_mean)	/	expected_sigma	)	;

								if	(deviation_sigmas	>	sigmas_max_deviation_from_mean)	{
												sigmas_max_deviation_from_mean	=	deviation_sigmas	;
								}
				}

				//	Loose	constraint:	anything	.LE.	1	will	pass.
				//	(Doesn't	require	the	largest	deviations	to	be	around	1.
				//	In	other	words,	even	max	deviations	of	zero	would	pass.)
				//EXPECT_LE(sigmas_max_deviation_from_mean,	1.0)	;

				//	tighter	constraint:	the	max	deviation	with	100	runs
				//	should	be	pretty	close	to	but	still	<=	1.0.
				//	This	specifies	the	range	0.9	..	1.0
				EXPECT_NEAR(sigmas_max_deviation_from_mean,	0.95,	0.05)	;
}

///@brief	test	set_sigma_range	feature	works	for	STL	random
TEST_F(MonteCarloTest,	MonteVarRandom_StlGaussian_nonGslSigmaRangeDefaulted_maxDeviationFromMeanIs1Sigma)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var9("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN,	"",	Trick::MonteVarRandom::TRICK_DEFAULT_ENGINE)	;
				exec.add_variable(&var9)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var9.set_seed(12345)	;
				var9.set_mu(expected_mean)	;
				var9.set_sigma(expected_sigma)	;
				var9.set_min(-10.0)	;
				var9.set_max(10.0)	;

				//	without	this	call,	default	sigma_range	is	1
				//	var9.set_sigma_range(0)	;

				int				num_runs	=	1000	;
				double	sigmas_max_deviation_from_mean	=	0.0	;
				double	deviation_sigmas	=	0.0	;

				for	(int	ii	=	0;	ii	<=	num_runs;	++ii)	{

								{
												var9.get_next_value()	;
												str	=	var9.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}

								deviation_sigmas	=	fabs(	(value	-	expected_mean)	/	expected_sigma	)	;

								if	(deviation_sigmas	>	sigmas_max_deviation_from_mean)	{
												sigmas_max_deviation_from_mean	=	deviation_sigmas	;
								}
				}

				//	Loose	constraint:	anything	.LE.	1	will	pass.
				//	(Doesn't	require	the	largest	deviations	to	be	around	1.
				//	In	other	words,	even	max	deviations	of	zero	would	pass.)
				//EXPECT_LE(sigmas_max_deviation_from_mean,	1.0)	;

				//	tighter	constraint:	the	max	deviation	with	100	runs
				//	should	be	pretty	close	to	but	still	<=	1.0.
				//	This	specifies	the	range	0.9	..	1.0
				EXPECT_NEAR(sigmas_max_deviation_from_mean,	0.95,	0.05)	;
}


#endif	//	not	_HAVE_GSL

#if	(defined(_HAVE_TR1_RANDOM)	||	defined(_HAVE_STL_RANDOM))

TEST_F(MonteCarloTest,	MonteVarRandom_StlGaussian_distributionMatchesSpec)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var8("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN,	"--",
																																Trick::MonteVarRandom::TRICK_DEFAULT_ENGINE)	;
				exec.add_variable(&var8)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var8.set_seed(12345)	;
				var8.set_sigma_range(0)	;
				var8.set_mu(expected_mean)	;
				var8.set_sigma(expected_sigma)	;
				var8.set_min(-10.0)	;
				var8.set_max(10.0)	;

				//	maintain	running	sums	for	calculating	mean	and	variance
				double	mean	=	0.0	;
				double	sum_variance	=	0.0	;
				int				n	=	0	;
				double	delta	=	0.0	;
				int				numRuns	=	100	;

				for	(int	ii	=	0;	ii	<=	numRuns;	++ii)	{

								{
												var8.get_next_value()	;
												str	=	var8.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}

								n++	;
								delta	=	value	-	mean	;
								mean	+=	delta	/	(double)n	;
								sum_variance	+=	delta	*	(value	-	mean)	;
				}

				//	final	statistics
				double	variance	=	sum_variance	/	(double)	(n	-	1)	;
				double	sigma	=	sqrt(variance)	;

				EXPECT_NEAR(expected_mean,		mean,		0.05	*	expected_mean)	;
				EXPECT_NEAR(expected_sigma,	sigma,	0.10	*	expected_sigma)	;
}


///@breif	test	relative	min/max	work	as	expected	for	STL
TEST_F(MonteCarloTest,	MonteVarRandom_StlGaussian_relativeMinMaxWorks)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var6("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				exec.add_variable(&var6)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var6.set_seed(12345)	;
				var6.set_sigma_range(0)	;
				var6.set_mu(expected_mean)	;
				var6.set_sigma(expected_sigma)	;
				var6.set_min(-1.0)	;
				var6.set_max(1.0)	;
				var6.set_min_is_relative(true)	;
				var6.set_max_is_relative(true)	;

				//	maintain	running	sums	for	calculating	mean	and	variance
				int				numRuns	=	100	;
				double	saw_min	=		1000.0;
				double	saw_max	=	-1000.0;

				for	(int	ii	=	0;	ii	<=	numRuns;	++ii)	{
								{
												var6.get_next_value()	;
												str	=	var6.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}
								if	(value	<	saw_min)	{
												saw_min	=	value;
								}
								if	(value	>	saw_max)	{
												saw_max	=	value;
								}
				}

				//	these	conditions	should	be	true	if	valid
				//	relative	min/max	range	of	10	+/-	1	is	being	imposed
				EXPECT_NEAR(9.05,	saw_min,	0.05)	;
				EXPECT_NEAR(10.95,	saw_max,	0.05)	;
}

///@breif	test	relative	min/max	works	after	calling	set_mu,	as	expected	for	STL
TEST_F(MonteCarloTest,	MonteVarRandom_StlGaussian_callSetMu_relativeMinMaxWorks)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var11("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				exec.add_variable(&var11)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var11.set_seed(12345)	;
				var11.set_sigma_range(0)	;
				var11.set_mu(expected_mean	+	100.0)	;
				var11.set_sigma(expected_sigma)	;
				var11.set_min(-1.0)	;
				var11.set_max(1.0)	;
				var11.set_min_is_relative(true)	;
				var11.set_max_is_relative(true)	;
				var11.set_mu(expected_mean)	;

				//	maintain	running	sums	for	calculating	mean	and	variance
				int				numRuns	=	100	;
				double	saw_min	=		1000.0;
				double	saw_max	=	-1000.0;

				for	(int	ii	=	0;	ii	<=	numRuns;	++ii)	{
								{
												var11.get_next_value()	;
												str	=	var11.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}
								if	(value	<	saw_min)	{
												saw_min	=	value;
								}
								if	(value	>	saw_max)	{
												saw_max	=	value;
								}
				}

				//	these	conditions	should	be	true	if	valid
				//	relative	min/max	range	of	10	+/-	1	is	being	imposed
				EXPECT_NEAR(9.05,	saw_min,	0.05)	;
				EXPECT_NEAR(10.95,	saw_max,	0.05)	;
}

///@breif	test	absolute	min/max	works	after	changing	from	relative,	as	expected	for	STL
TEST_F(MonteCarloTest,	MonteVarRandom_StlGaussian_changeToAbsolute_MinMaxWorks)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var11("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				exec.add_variable(&var11)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var11.set_seed(12345)	;
				var11.set_sigma_range(0)	;
				var11.set_mu(expected_mean)	;
				var11.set_sigma(expected_sigma)	;
				var11.set_min(9.0)	;
				var11.set_max(11.0)	;

				var11.set_min_is_relative(false)	;
				var11.set_max_is_relative(false)	;

				//	maintain	running	sums	for	calculating	mean	and	variance
				int				numRuns	=	100	;
				double	saw_min	=		1000.0;
				double	saw_max	=	-1000.0;

				for	(int	ii	=	0;	ii	<=	numRuns;	++ii)	{
								{
												var11.get_next_value()	;
												str	=	var11.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}
								if	(value	<	saw_min)	{
												saw_min	=	value;
								}
								if	(value	>	saw_max)	{
												saw_max	=	value;
								}
				}

				//	these	conditions	should	be	true	if	valid
				//	relative	min/max	range	of	10	+/-	1	is	being	imposed
				EXPECT_NEAR(9.05,	saw_min,	0.05)	;
				EXPECT_NEAR(10.95,	saw_max,	0.05)	;
}

///@brief	test	that	the	sequence	repeats	after	calling	set_seed
TEST_F(MonteCarloTest,	MonteVarRandom_StlGaussian_setSeed_TakesEffect)	{
				//req.add_requirement("3932595803");
				std::string	str	;
				double						value	;

				Trick::MonteVarRandom	var10("time_to_fire_1",	Trick::MonteVarRandom::GAUSSIAN)	;
				exec.add_variable(&var10)	;

				double	expected_mean		=	10.0	;
				double	expected_sigma	=	2.0	;

				var10.set_seed(12345)	;
				var10.set_sigma_range(0)	;
				var10.set_mu(expected_mean)	;
				var10.set_sigma(expected_sigma)	;
				var10.set_min(-10.0)	;
				var10.set_max(10.0)	;
				var10.set_min_is_relative(true)	;
				var10.set_max_is_relative(true)	;

				int	numRuns	=	10;

				for	(int	ii	=	0;	ii	<	numRuns;	++ii)	{
								{
												var10.get_next_value()	;
												str	=	var10.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}
				}
				double	first_value	=	value;

				var10.set_seed(12345)	;

				for	(int	ii	=	0;	ii	<	numRuns;	++ii)	{
								{
												var10.get_next_value()	;
												str	=	var10.value	;
												std::stringstream	sstream(str)	;
												sstream	>>	value	;
								}
				}
				double	second_value	=	value;

				EXPECT_NEAR(first_value,	second_value,	0.000001)	;
}

#endif	//	_HAVE_TR1_RANDOM	or	_HAVE_STL_RANDOM

}
