
#define	protected	public

#include	<iostream>
#include	<sys/types.h>
#include	<signal.h>

#include	"gtest/gtest.h"
#include	"trick/MonteCarlo.hh"
#include	"trick/MonteVar.hh"
#include	"trick/MonteVarFile.hh"
#include	"trick/MonteVarRandom.hh"
#include	"trick/SimObject.hh"
#include	"trick/ExecutiveException.hh"


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

								testSimObject()	{}
}	;

class	MonteCarloTest	:	public	::testing::Test	{

				protected:
								Trick::MonteCarlo	exec;

								MonteCarloTest()	{}
								~MonteCarloTest()	{}
}	;

TEST_F(MonteCarloTest,	MonteVarFile_ColException)	{
				bool	got_exception	=	false;

				try	{
								Trick::MonteVarFile	var("time_to_fire_1",	"M_jet_firings_inline",	7)	;
								var.get_next_value();
				}	catch	(Trick::ExecutiveException	&e)	{
								std::cout	<<	e.message	<<	std::endl;
								got_exception	=	true;
				}
				EXPECT_EQ(got_exception,	true);
}

TEST_F(MonteCarloTest,	MonteVarFile_BadFileException)	{
				bool	got_exception	=	false;

				try	{
								Trick::MonteVarFile	var("time_to_fire_1",	"_M_jet_firings_inline",	1)	;
								var.get_next_value();
				}	catch	(Trick::ExecutiveException	&e)	{
								std::cout	<<	e.message	<<	std::endl;
								got_exception	=	true;
				}
				EXPECT_EQ(got_exception,	true);
}

TEST_F(MonteCarloTest,	TestMonteVarRandom_Exception)	{
				bool	got_exception	=	false;

				try	{
								Trick::MonteVarRandom	var1("time_to_fire_2",	Trick::MonteVarRandom::GAUSSIAN)	;
								var1.set_mu(4.0)	;
								var1.set_min(4.0)	;
								var1.set_max(4.000001)	;
								exec.add_variable(&var1)	;
								var1.get_next_value();
				}	catch	(Trick::ExecutiveException	&e)	{
								std::cout	<<	e.message;
								got_exception	=	true;
				}
				EXPECT_EQ(got_exception,	true);
}

}
