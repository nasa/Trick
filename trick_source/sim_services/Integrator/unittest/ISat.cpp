#include	"Integrator.hh"
#include	<math.h>
#include	<iostream>

//	This	is	from	matrix_macros.h	in	Trick.
						/*	matrix	times	vector	:	prod[i]	=	mat[i][j]	*	vect[i]	*/
#define	MxV(	prod	,	mat	,	vect	)	{	\
			prod[0]	=	mat[0][0]	*	vect[0]	+	mat[0][1]	*	vect[1]	+	mat[0][2]	*	vect[2]	;	\
			prod[1]	=	mat[1][0]	*	vect[0]	+	mat[1][1]	*	vect[1]	+	mat[1][2]	*	vect[2]	;	\
			prod[2]	=	mat[2][0]	*	vect[0]	+	mat[2][1]	*	vect[1]	+	mat[2][2]	*	vect[2]	;	\
}

//	GRAVITATIONAL_CONSTANT	(Nm2/kg2)
#define	GRAVITATIONAL_CONSTANT	6.67e-11
//	LUNAR_MASS	(kg)
#define	LUNAR_MASS			7.36e22
//	LUNAR	RADIUS	(m)
#define	LUNAR_RADIUS	1737400.0

#define	RADIANS_PER_DEGREE	0.0174532925

typedef	struct	{
				double	mass;
				double	pos[3];
				double	vel[3];
				double	acc[3];
}	SATELLITE;

typedef	struct	{
				double	radius;
				double	mass;
}	PLANET;

typedef	struct	{
				double	semi_major_axis;					/*	(m)		*/
				double	eccentricity;								/*	(--)	*/
				double	inclination;									/*	(r)		*/
				double	argument_of_perigee;	/*	(r)		*/
				double	right_ascension;					/*	(r)		*/
				double	true_anomaly;								/*	(r)		*/
}	ORBITAL_ELEMENTS;

void	calc_gravity(	SATELLITE	*	S,
																			PLANET				*	P,
																			double				*	G	)	{

			double	distance;
			double	pos_hat[3];
			double	g_force_mag;

			//	Calculate	Gravitational	Force
			distance	=	sqrt(	S->pos[0]	*	S->pos[0]	+
																				S->pos[1]	*	S->pos[1]	+
																				S->pos[2]	*	S->pos[2]	);

			pos_hat[0]	=	S->pos[0]	/	distance	;
			pos_hat[1]	=	S->pos[1]	/	distance	;
			pos_hat[2]	=	S->pos[2]	/	distance	;

			g_force_mag	=	(	GRAVITATIONAL_CONSTANT	*	P->mass	*	S->mass	)	/	(	distance	*	distance	)	;

			G[0]	=	g_force_mag	*	-	pos_hat[0]	;
			G[1]	=	g_force_mag	*	-	pos_hat[1]	;
			G[2]	=	g_force_mag	*	-	pos_hat[2]	;

}

//
//	Reference:	ISS	SIGI	Attitude	Processor	Orbit	Equations	EG2/Rodolfo	Gonzalez	20	Aug	2003
void	orb_elem_to_intl(	ORBITAL_ELEMENTS	*	O,										/*	IN		*/
																							PLANET											*	P,										/*	IN		*/
																							double											*	R_intl,					/*	OUT	*/
																							double											*	V_intl	)	{		/*	OUT	*/

				double	ra,	wp,	in	;
				double	M_pci_perif[3][3];			/*	Perifocal	-->	Inertial	transform	*/
				double	semi_latus_rectum;
				double	r_magnitude;
				double	r_perifocal[3];
				double	v_perifocal[3];
				double	v_mag;

				semi_latus_rectum	=	O->semi_major_axis	*	(	1.0	-	O->eccentricity	*	O->eccentricity	);

				r_magnitude	=	semi_latus_rectum	/	(1.0	+	O->eccentricity	*	cos(	O->true_anomaly	));

				r_perifocal[0]	=	r_magnitude	*	cos(	O->true_anomaly);
				r_perifocal[1]	=	r_magnitude	*	sin(	O->true_anomaly);
				r_perifocal[2]	=	0;

				v_mag	=	sqrt(	GRAVITATIONAL_CONSTANT	*	P->mass	/	semi_latus_rectum);

				v_perifocal[0]	=	v_mag	*	(-sin(O->true_anomaly));
				v_perifocal[1]	=	v_mag	*	(O->eccentricity	+	cos(O->true_anomaly));
				v_perifocal[2]	=	0;

				ra	=	O->right_ascension;
				wp	=	O->argument_of_perigee;
				in	=	O->inclination;

				M_pci_perif[0][0]	=		cos(ra)*cos(wp)	-	sin(ra)*cos(in)*sin(wp)	;
				M_pci_perif[0][1]	=	-cos(ra)*sin(wp)	-	sin(ra)*cos(in)*cos(wp)	;
				M_pci_perif[0][2]	=		sin(ra)*sin(in);

				M_pci_perif[1][0]	=		sin(ra)*cos(wp)	+	cos(ra)*cos(in)*sin(wp)	;
				M_pci_perif[1][1]	=	-sin(ra)*sin(wp)	+	cos(ra)*cos(in)*cos(wp)	;
				M_pci_perif[1][2]	=	-cos(ra)*sin(in)	;

				M_pci_perif[2][0]	=	sin(in)*sin(wp)	;
				M_pci_perif[2][1]	=	sin(in)*cos(wp)	;
				M_pci_perif[2][2]	=	cos(in)	;

				MxV(	R_intl	,	M_pci_perif	,	r_perifocal)	;
				MxV(	V_intl	,	M_pci_perif	,	v_perifocal)	;

}





void	deriv(SATELLITE	*S,	PLANET	*P)	{

				double				gforce[3];

				calc_gravity(S,	P,	gforce	);

				S->acc[0]	=	gforce[0]	/	S->mass	;
				S->acc[1]	=	gforce[1]	/	S->mass	;
				S->acc[2]	=	gforce[2]	/	S->mass	;

}

int	main(int	argc,	const	char*	argv[])	{

				SATELLITE	satellite;
				PLANET				planet;
				ORBITAL_ELEMENTS	orbital_elements;

				long						tick	=	0;
				double				seconds_per_tick	=	1.0;
				double				sim_time	=	0.0;

				//	Initialization
				satellite.mass	=	100;
				planet.mass			=	LUNAR_MASS;
				planet.radius	=	LUNAR_RADIUS;

				orbital_elements.semi_major_axis	=	2000000;	//	meters
				orbital_elements.eccentricity	=	0.2;
				orbital_elements.inclination	=	5.0	*	RADIANS_PER_DEGREE;
				orbital_elements.argument_of_perigee	=	0.0;
				orbital_elements.right_ascension	=	0.0;			//	radians
				orbital_elements.true_anomaly	=	0.0;						//	radians

				orb_elem_to_intl(	&orbital_elements,
																						&planet,
																						satellite.pos,
																						satellite.vel);

				Trick::Integrator	*I	=	getIntegrator(	Euler,	6,	1.0);

				do	{	//	sim	loop
								sim_time	=	tick	*	seconds_per_tick	;
								std::cout	<<	"sim_time	=	"	<<	sim_time	<<	std::endl;

								do	{	//	Integration	Loop

												I->time	=	sim_time;

												std::cout	<<	"step	#"	<<	I->intermediate_step	<<	std::endl;	std::cout.flush();

												std::cout	<<	"deriv	...";		std::cout.flush();
												deriv(	&satellite,	&planet);
												std::cout	<<	"done."	<<	std::endl;	std::cout.flush();

												std::cout	<<	"loading	state	...";	std::cout.flush();
												I->state[0]	=	satellite.pos[0];
												I->state[1]	=	satellite.pos[1];
												I->state[2]	=	satellite.pos[2];
												I->state[3]	=	satellite.vel[0];
												I->state[4]	=	satellite.vel[1];
												I->state[5]	=	satellite.vel[2];
												std::cout	<<	"done."	<<	std::endl;	std::cout.flush();

												std::cout	<<	"loading	state	deriv	...";	std::cout.flush();
												I->deriv[I->intermediate_step][0]	=	satellite.vel[0];
												I->deriv[I->intermediate_step][1]	=	satellite.vel[1];
												I->deriv[I->intermediate_step][2]	=	satellite.vel[2];
												I->deriv[I->intermediate_step][3]	=	satellite.acc[0];
												I->deriv[I->intermediate_step][4]	=	satellite.acc[1];
												I->deriv[I->intermediate_step][5]	=	satellite.acc[2];
												std::cout	<<	"done."	<<	std::endl;	std::cout.flush();

												std::cout	<<	"integrate"	<<	std::endl;	std::cout.flush();
												I->integrate();
												std::cout	<<	"done."	<<	std::endl;	std::cout.flush();

												satellite.pos[0]	=	I->state_ws[I->intermediate_step][0];
												satellite.pos[1]	=	I->state_ws[I->intermediate_step][1];
												satellite.pos[2]	=	I->state_ws[I->intermediate_step][2];
												satellite.vel[0]	=	I->state_ws[I->intermediate_step][3];
												satellite.vel[1]	=	I->state_ws[I->intermediate_step][4];
												satellite.vel[2]	=	I->state_ws[I->intermediate_step][5];

								}	while	(	I->intermediate_step);

								std::cout	<<	"("	<<	satellite.pos[0]	<<","<<	satellite.pos[1]	<<","<<	satellite.pos[2]	<<	")"	<<	std::endl;

								tick++;

				}	while	(sim_time	<	10.0);
}
