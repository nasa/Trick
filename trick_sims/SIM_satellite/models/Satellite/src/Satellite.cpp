#include	"Satellite.hh"
#include	"trick_utils/math/include/quat_macros.h"
#include	"trick_utils/math/include/matrix_macros.h"
#include	"trick_utils/math/include/trick_math_proto.h"
#include	"sim_services/Integrator/include/integrator_c_intf.h"
#include	<iostream>

#define	EARTH_MASS	5.9721986e24
#define	EARTH_RADIUS	6367500.0
#define	GRAVITATIONAL_CONSTANT	6.674e-11

//	Force	is	in	inertial	frame.
void	Satellite::get_force_of_gravity	(	double*	gForceV)	{

					double	body_to_planet[3];
					double	body_to_planet_unit_vector[3];
					double	distance;
					double	gForceMag;

					V_SUB(	body_to_planet,	planet->pos,	pos	);
					V_NORM(	body_to_planet_unit_vector,	body_to_planet)
					distance	=	V_MAG(	body_to_planet);
					gForceMag	=	(GRAVITATIONAL_CONSTANT	*	mass	*	planet->mass)	/	(distance	*	distance);
					V_SCALE	(gForceV,	body_to_planet_unit_vector,	gForceMag);

}

void	Satellite::get_Force_and_Torque()	{

			double	gforcev[3];

			//	Zero	force.
			for	(int	ii=0;	ii<3;	ii++)	{	force[ii]	=	0.0;	}
			for	(int	ii=0;	ii<3;	ii++)	{	torque[ii]	=	0.0;	}

			get_force_of_gravity(	gforcev);
			for	(int	ii=0;	ii<3;	ii++)	{	force[ii]	+=	gforcev[ii];	}

			main_engine.add_force_and_torque(	force,	torque);

			thruster_T1.add_force_and_torque(	force,	torque);
			thruster_T2.add_force_and_torque(	force,	torque);
			thruster_T3.add_force_and_torque(	force,	torque);
			thruster_T4.add_force_and_torque(	force,	torque);

			thruster_R1.add_force_and_torque(	force,	torque);
			thruster_R2.add_force_and_torque(	force,	torque);
			thruster_R3.add_force_and_torque(	force,	torque);
			thruster_R4.add_force_and_torque(	force,	torque);

			thruster_B1.add_force_and_torque(	force,	torque);
			thruster_B2.add_force_and_torque(	force,	torque);
			thruster_B3.add_force_and_torque(	force,	torque);
			thruster_B4.add_force_and_torque(	force,	torque);

			thruster_L1.add_force_and_torque(	force,	torque);
			thruster_L2.add_force_and_torque(	force,	torque);
			thruster_L3.add_force_and_torque(	force,	torque);
			thruster_L4.add_force_and_torque(	force,	torque);

}

void	Satellite::initialize(Planet*	planet_p)	{

				planet	=	planet_p;

				mass	=	1000.0;			/*	(kg)	*/

				double	A=2,	B=2,	C=2;
				Ibody[0][0]	=	mass	*	0.2	*	(B*B	+	C*C);
				Ibody[0][1]	=	0.0;
				Ibody[0][2]	=	0.0;
				Ibody[1][0]	=	0.0;
				Ibody[1][1]	=	mass	*	0.2	*	(A*A	+	C*C);
				Ibody[1][2]	=	0.0;
				Ibody[2][0]	=	0.0;
				Ibody[2][1]	=	0.0;
				Ibody[2][2]	=	mass	*	0.2	*	(A*A	+	B*B);

				dm_invert(Ibodyinv,	Ibody);

				//	Altitude	of	500	km.
				pos[0]	=	EARTH_RADIUS	+	500000.0;
				pos[1]	=	0.0;
				pos[2]	=	0.0;

				//	Initialize	velocity	for	a	circular	orbit.
				vel[0]	=	0.0;
				vel[1]	=	sqrt(	(GRAVITATIONAL_CONSTANT	*	EARTH_MASS)	/	pos[0]	);
				vel[2]	=	0.0;

				//	Compute	Platform	to	World	Rotation	Matrix
				compute_platform_to_world();

				//	Compute	Body	to	World	Rotation	Matrix
				for	(int	ii=0;	ii<3;	ii++)	{
								for	(int	jj=0;	jj<3;	jj++)	{
												R[ii][jj]	=	Rplat[ii][jj];
								}
				}

				//	Linear	Momentum
				for	(int	ii=0;	ii<3;	ii++)	{	P[ii]	=	vel[ii]	*	mass;	}

				//	Angular	Momentum
				for	(int	ii=0;	ii<3;	ii++)	{	L[ii]	=	0.0;	}
				L[0]	=	0.0;
				L[1]	=	0.0;
				L[2]	=	0.0;

			compute_auxiliary_vars();

			for	(int	ii=0;	ii<3;	ii++)	{	force[ii]	=	0.0;	}
			for	(int	ii=0;	ii<3;	ii++)	{	torque[ii]	=	0.0;	}

			main_engine.initialize(	0.0,	0.0,	0.0,		1000.0,		0.0,		0.0);

			//	Initialize	RCS	Thrusters
			thruster_T1.initialize(	0.0,	0.0,	1.956,		10.0,		0.0,		0.0);
			thruster_T2.initialize(	0.0,	0.0,	1.956,		0.0,		10.0,		0.0);
			thruster_T3.initialize(	0.0,	0.0,	1.956,	-10.0,		0.0,		0.0);
			thruster_T4.initialize(	0.0,	0.0,	1.956,		0.0,	-10.0,		0.0);

			thruster_R1.initialize(	0.0,	1.956,	0.0,		10.0,		0.0,		0.0);
			thruster_R2.initialize(	0.0,	1.956,	0.0,		0.0,		0.0,	-10.0);
			thruster_R3.initialize(	0.0,	1.956,	0.0,	-10.0,		0.0,		0.0);
			thruster_R4.initialize(	0.0,	1.956,	0.0,		0.0,		0.0,		1.0);

			thruster_B1.initialize(	0.0,	0.0,	-1.956,		10.0,		0.0,		0.0);
			thruster_B2.initialize(	0.0,	0.0,	-1.956,		0.0,		10.0,		0.0);
			thruster_B3.initialize(	0.0,	0.0,	-1.956,	-10.0,		0.0,		0.0);
			thruster_B4.initialize(	0.0,	0.0,	-1.956,		0.0,	-10.0,		0.0);

			thruster_L1.initialize(	0.0,	-1.956,	0.0,		10.0,		0.0,		0.0);
			thruster_L2.initialize(	0.0,	-1.956,	0.0,		0.0,		0.0,		10.0);
			thruster_L3.initialize(	0.0,	-1.956,	0.0,	-10.0,		0.0,		0.0);
			thruster_L4.initialize(	0.0,	-1.956,	0.0,		0.0,		0.0,	-10.0);
}


void	Satellite::compute_platform_to_world()	{

				//	Compute	the	Body	to	World	Rotation	Matrix

				//	The	X	body	axis	initially	points	in	the	direction	of	the	velocity	vector.
				double	vhat[3];
				V_NORM(vhat,	vel);

				//	The	Z	body	axis	initially	points	in	the	direction	of	the	position	vector.
				double	phat[3];
				V_NORM(phat,	pos);

				//	The	Y	body	axis	is	the	cross	product	of	the	previous	two	axes.
				double	phatXvhat[3];
				V_CROSS	(phatXvhat,	phat,	vhat);

				Rplat[0][0]		=	vhat[0];
				Rplat[0][1]		=	vhat[1];
				Rplat[0][2]		=	vhat[2];

				Rplat[1][0]		=	phatXvhat[0];
				Rplat[1][1]		=	phatXvhat[1];
				Rplat[1][2]		=	phatXvhat[2];

				Rplat[2][0]		=	phat[0];
				Rplat[2][1]		=	phat[1];
				Rplat[2][2]		=	phat[2];

}

Satellite::Satellite(Planet*	planet_p)	{
				initialize(	planet_p);
}


void	Satellite::compute_auxiliary_vars()	{

				double	Rtrans[3][3];
				double	tempM[3][3];
				int	axis	=	0;

				//	Square	up	the	rotation	matrix.
				dm_orthonormal(R,	&axis);

				//	-	Compute	Iinv	=	R	*	Ibodyinv	*	Transpose(R);
				MxM(tempM,	R,	Ibodyinv);
				M_TRANS(Rtrans,	R);
				MxM(Iinv,	tempM,	Rtrans);

				//	-	Compute	velocity	from	momentum	and	mass.
				for	(int	ii=0;	ii<3;	ii++)	{	vel[ii]	=	P[ii]	/	mass;	}

				//	-	Compute	omega	=	Iinv	*	L;
				MxV(omega,	Iinv,	L);
}


void	Satellite::compute_state_deriv()	{

				double	omega_star[3][3];
				V_SKEW(omega_star,	omega);
				MxM(Rdot,	omega_star,	R);

				get_Force_and_Torque();

				//	-	Compute	velocity	from	momentum	and	mass.
				for	(int	ii=0;	ii<3;	ii++)	{	vel[ii]	=	P[ii]	/	mass;	}
}


int	Satellite::compute_state()	{

				int	ipass;

				load_state(	&P[0],	&P[1],	&P[2],
																&pos[0],	&pos[1],	&pos[2],
																&L[0],	&L[1],	&L[2],
																&R[0][0],&R[0][1],&R[0][2],
																&R[1][0],&R[1][1],&R[1][2],
																&R[2][0],&R[2][1],&R[2][2],
																NULL);

				load_deriv(	&force[0],	&force[1],	&force[2],
																&vel[0],	&vel[1],	&vel[2],
																&torque[0],	&torque[1],	&torque[2],
																&Rdot[0][0],&Rdot[0][1],&Rdot[0][2],
																&Rdot[1][0],&Rdot[1][1],&Rdot[1][2],
																&Rdot[2][0],&Rdot[2][1],&Rdot[2][2],
																NULL);

				ipass	=	integrate();

				unload_state(	&P[0],	&P[1],	&P[2],
																		&pos[0],	&pos[1],	&pos[2],
																		&L[0],	&L[1],	&L[2],
																		&R[0][0],&R[0][1],&R[0][2],
																		&R[1][0],&R[1][1],&R[1][2],
																		&R[2][0],&R[2][1],&R[2][2],
																		NULL);

				compute_auxiliary_vars();

				return(ipass);

}

void	Satellite::print_state()	{

}

