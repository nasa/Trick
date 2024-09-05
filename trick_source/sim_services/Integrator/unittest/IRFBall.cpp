#include	"Integrator.hh"
#include	<math.h>
#include	<iostream>
#include	<ostream>
#include	<fstream>
#include	<iomanip>
#include	"regula_falsi.h"

#include	<string.h>

#define	PI	3.141592653589793
#define	RAD_PER_DEG	(2.0*PI/180.0)

typedef	struct	{
				double	pos[2];
				double	vel[2];
				double	acc[2];
}	BALL;

void	deriv(BALL	*B)	{

				B->acc[0]	=	-9.81;
				B->acc[1]	=	0;

}

void	integ(Trick::Integrator	*I,	BALL	*ball	)	{

				do	{
								deriv(	ball);
								I->state_in(	&ball->pos[0],	&ball->pos[1],	&ball->vel[0],	&ball->vel[1],	NULL);
								I->deriv_in(	&ball->vel[0],	&ball->vel[1],	&ball->acc[0],	&ball->acc[1],	NULL);
								I->integrate();
								I->state_out(	&ball->pos[0],	&ball->pos[1],	&ball->vel[0],	&ball->vel[1],	NULL);
				}	while	(	I->intermediate_step);
}






void	IBall_sim(	Integrator_type	Alg,
																std::ostream&			dataout)	{

				BALL			ball;
				long			tick;
				double	sim_time;
				REGULA_FALSI	rf;

				const	double	seconds_per_tick	=	0.01;
				const	double	initial_angle	=	30.0;
				const	double	initial_speed	=	50.0;
				const	int				doing_dynamic_events	=	1;

				dataout.width(16);
				dataout.precision(14);

				//	========================================
				//													Initialization
				//	========================================
				tick	=	0;
				sim_time	=	0.0;

				ball.pos[0]	=	0.0;
				ball.pos[1]	=	0.0;
				ball.vel[0]	=	initial_speed	*	cos(	initial_angle	*	RAD_PER_DEG);
				ball.vel[1]	=	initial_speed	*	sin(	initial_angle	*	RAD_PER_DEG);

				Trick::Integrator	*I	=	Trick::getIntegrator(	Alg,	4,	seconds_per_tick	);

				sim_time	=	tick	*	seconds_per_tick	;

				//	Initialize	Regula	Falsi.
				reset_regula_falsi(sim_time,	&rf);
				rf.error_tol	=	1.0e-15;
				rf.mode	=	Any;

				//	Note:	We	don't	care	what	the	tgo	estimate	is	because	here,
				//	we	are	just	initializing	the	bounds.
				rf.error	=	ball.pos[0];
				regula_falsi(sim_time,	&rf);

				//	========================================
				//															Simulation	loop
				//	========================================
				do	{

								dataout	<<	sim_time	<<	"	"	<<	ball.pos[0]	<<	"	"	<<	ball.pos[1]	<<	std::endl;

								I->time	=	sim_time;

								//	###I###	Integrate	over	the	time	step.
								integ(I,	&ball);

								//	Advance	time.
								tick++;
								sim_time	=	tick	*	seconds_per_tick	;

								//	If	we	are	looking	for	roots	...
								if	(	doing_dynamic_events	)	{
												double	tgo;

												//	###RF###	Given	the	current	error,	estimate	how	far	(in	time)	we	are	from	a	root.
												rf.error	=	ball.pos[0];
												tgo	=	regula_falsi(sim_time,	&rf);

												//	If	regula_falsi	found	a	root	in	the	last	interval	...
												if	(	tgo	<	seconds_per_tick)	{

																int	root_found	=	0;
																double	t_test	=	sim_time;

																//	Iterate	until	we	find	the	root.

																//	NOTE:	the	regula_falsi	function	gives	up	and	returns	with	tgo=0	if
																//	it	hasn't	converged	on	a	root	after	20	iterations.
																while	(!	root_found)	{

																				//	###I###	Integrate	over	the	time-correction.
																				I->dt	=	tgo;
																				integ(I,	&ball);

																				t_test	+=	tgo;

																				//	###RF###	Given	the	current	error,	estimate	how	far	(in	time)	we	are	from	the	root.
																				rf.error	=	ball.pos[0];
																				tgo	=	regula_falsi(	t_test,	&rf);

																				//	If	the	estimated	time-to-go	is	less	than	the	chosen	tolerance,	then	we	have	our	root.
																				if	(fabs(	tgo)	<	rf.error_tol)	{
																								printf("ROOT@	%18.14g\n",	t_test);
																								root_found	=	1;
																								reset_regula_falsi(t_test,	&rf);
																				}
																}
																root_found	=	0;

																//	###I###	Integrate	from	t=t_test	back	(forward	actually)	to	t=sim_time.
																I->dt	=		sim_time	-	t_test	;
																integ(I,	&ball);

																//	Restore	the	normal	time-step.
																I->dt	=	seconds_per_tick;
												}

								}	//	End	of	doing_dynamic_events.

				}	while	(ball.pos[0]	>=	-3.0);

				dataout	<<	sim_time	<<	"	"	<<	ball.pos[0]	<<	"	"	<<	ball.pos[1]	<<	std::endl;

				delete(	I);
}


int	main(int	argc,	const	char*	argv[])	{

				std::ofstream	dataout;
				std::ofstream	gplout;

				char	dataout_name[80];
				Integrator_type	Algorithm	;
				const	char*	Algorithm_name;
				int	test_number;

				gplout.open("IBall.gpl",	std::ofstream::out);
				gplout	<<	"plot	\\"	<<	std::endl;

				for	(test_number	=	0;	test_number	<	5	;	test_number++)	{

								//	Select	test.
								switch	(test_number)	{
								case	0:
												Algorithm	=	Euler;
												Algorithm_name	=	"Euler";
												break;
								case	1:
												Algorithm	=	Runge_Kutta_2;
												Algorithm_name	=	"Runge_Kutta_2";
												break;
								case	2:
												Algorithm	=	Runge_Kutta_4;
												Algorithm_name	=	"Runge_Kutta_4";
												break;
								case	3:
												Algorithm	=	Runge_Kutta_Fehlberg_45;
												Algorithm_name	=	"Runge_Kutta_Fehlberg_45";
												break;
								case	4:
												Algorithm	=	Runge_Kutta_Fehlberg_78;
												Algorithm_name	=	"Runge_Kutta_Fehlberg_78";
												break;
//						case	5:
//										Algorithm	=	Runge_Kutta_Gill_4;
//										Algorithm_name	=	"Runge_Kutta_Gill_4";
//										break;
								default:
												std::cerr	<<	"Invalid	test	number."	<<	std::endl;
								}

								strcpy	(dataout_name,	"IBall_");
								strcat	(dataout_name,	Algorithm_name);
								strcat	(dataout_name,	".dat");
								dataout.open(	dataout_name);
								IBall_sim(	Algorithm,	dataout);
								dataout.close();

								if	(test_number	>	0)	{
												gplout	<<	",	\\"	<<	std::endl;
								}

								gplout	<<	"\""	<<	dataout_name	<<	"\"	using	3:2	title	\'"	<<	Algorithm_name	<<	"\'	with	lines";

				}

				gplout.close();

				system("gnuplot	-persist	IBall.gpl");

				return	(0);
}
