#include	"Integrator.hh"
#include	<math.h>
#include	<iostream>
#include	<ostream>
#include	<fstream>
#include	<iomanip>

#include	<string.h>

#define	PI	3.141592653589793
#define	RAD_PER_DEG	(PI/180.0)

typedef	struct	{
				double	pos[2];
				double	vel[2];
				double	acc[2];
}	BALL;

void	deriv(BALL	*B)	{

				B->acc[0]	=	-9.81;
				B->acc[1]	=	0;

}

void	IBall_sim(	Integrator_type	Alg,
																std::ostream&			dataout)	{

				BALL			ball;
				long			tick;
				double	sim_time;

				const	double	seconds_per_tick	=	0.5;
				const	double	initial_speed	=	50.0;
				const	double	initial_angle	=	30.0;

				dataout.width(16);
				dataout.precision(14);

				//	Initialization
				tick	=	0;
				sim_time	=	0.0;

				ball.pos[0]	=	5.0;
				ball.pos[1]	=	5.0;
				ball.vel[0]	=	initial_speed	*	sin(initial_angle	*	RAD_PER_DEG);
				ball.vel[1]	=	initial_speed	*	cos(initial_angle	*	RAD_PER_DEG);

				Trick::Integrator	*I	=	Trick::getIntegrator(	Alg,	4,	seconds_per_tick	);

				do	{	//	Simulation	loop

								sim_time	=	tick	*	seconds_per_tick	;

								do	{	//	Integration	Loop

												I->time	=	sim_time;

												deriv(	&ball);
												//printf("TIME:	%g\n",sim_time);

												I->state_in(	&ball.pos[0],	&ball.pos[1],	&ball.vel[0],	&ball.vel[1],	NULL);
												//printf("LOAD	STATE:	%g	%g	%g	%g\n",ball.pos[0],	ball.pos[1],	ball.vel[0],	ball.vel[1]);

												I->deriv_in(	&ball.vel[0],	&ball.vel[1],	&ball.acc[0],	&ball.acc[1],	NULL);
												//printf("LOAD	DERIV:	%g	%g	%g	%g\n",	ball.vel[0],	ball.vel[1],	ball.acc[0],	ball.acc[1]);

												I->integrate();

												I->state_out(	&ball.pos[0],	&ball.pos[1],	&ball.vel[0],	&ball.vel[1],	NULL);
												//printf("STATE	OUT:	%g	%g	%g	%g\n",		ball.pos[0],	ball.pos[1],	ball.vel[0],	ball.vel[1]);

								}	while	(	I->intermediate_step);

								dataout	<<	sim_time	<<	"	"	<<	ball.pos[0]	<<	"	"	<<	ball.pos[1]	<<	std::endl;

								tick++;

				}	while	(ball.pos[0]	>=	0.0);

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
