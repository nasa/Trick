#include	<math.h>
#include	<stdio.h>
#include	"trick/integrator_c_intf.h"
#include	"trick/vector_macros.h"
#include	"trick/matrix_macros.h"
#include	"../include/CrewModuleDynamics.hh"
#include	"trick/trick_math_proto.h"

const	double	acceleration_of_gravity[3]	=	{0.0,	0.0,	-9.81};

CrewModuleDynamics::CrewModuleDynamics()	{
				init_defaults();
}

void	CrewModuleDynamics::init_inertia_tensor(double	A,	double	B,	double	C)	{
		I_body[0][0]	=	mass_vehicle	*	(B*B	+	C*C);
		I_body[0][1]	=	0.0;
		I_body[0][2]	=	0.0;
		I_body[1][0]	=	0.0;
		I_body[1][1]	=	mass_vehicle	*	(A*A	+	C*C);
		I_body[1][2]	=	0.0;
		I_body[2][0]	=	0.0;
		I_body[2][1]	=	0.0;
		I_body[2][2]	=	mass_vehicle	*	(A*A	+	B*B);
		dm_invert(I_body_inverse,	I_body);
}

void	CrewModuleDynamics::init_defaults()	{
				M_IDENT(R);
				M_INIT(Rdot);
				mass_vehicle	=	9300.0;
				density_of_water	=	1025.0;
				momentum[0]	=	0.0;
				momentum[1]	=	0.0;
				momentum[2]	=	0.0;
				position[0]	=	0.0;
				position[1]	=	0.0;
				position[2]	=	0.0;
				angular_momentum[0]	=	0.0;
				angular_momentum[1]	=	0.0;
				angular_momentum[2]	=	0.0;
				crewModuleShape.transformCoordinates(R,	position);
				init_inertia_tensor(1.0,	1.0,	1.0);
}

void	CrewModuleDynamics::calcVolumeAndCenterOfBuoyancy()	{
				double	x_start	=	position[0]	-	2.6;
				double	x_end			=	position[0]	+	2.6;
				double	y_start	=	position[1]	-	2.6;
				double	y_end			=	position[1]	+	2.6;
				double	z_start	=	position[2]	-	2.6;
				double	z_end			=	position[2]	+	2.6;
				double	sum_r_dv[3]	=	{0.0,	0.0,	0.0};
				volume_displaced	=	0.0;
				double	P[3];
				double	dx	=	0.1;
				double	dy	=	0.1;
				double	dz	=	0.1;
				double	dv	=	dx	*	dy	*	dz;
				for	(double	x	=	x_start;	x	<	x_end	;	x+=	dx	)	{
								for	(double	y	=	y_start;	y	<	y_end	;	y+=	dy	)	{
												for	(double	z	=	z_start;	z	<	z_end	;	z+=	dz	)	{
																P[0]=x;	P[1]=y;	P[2]=z;
																if	(	crewModuleShape.containsPoint(P)	)	{
																				if	(P[2]	<	0.0)	{		//	Below	the	water	level.
																								sum_r_dv[0]	+=	x	*	dv;
																								sum_r_dv[1]	+=	y	*	dv;
																								sum_r_dv[2]	+=	z	*	dv;
																								volume_displaced	+=	dv;
																				}
																}
												}
								}
				}
				if	(volume_displaced	>	0.0)	{
								center_of_buoyancy[0]	=	sum_r_dv[0]	/	volume_displaced;
								center_of_buoyancy[1]	=	sum_r_dv[1]	/	volume_displaced;
								center_of_buoyancy[2]	=	sum_r_dv[2]	/	volume_displaced;
				}	else	{
								center_of_buoyancy[0]	=	position[0];
								center_of_buoyancy[1]	=	position[1];
								center_of_buoyancy[2]	=	position[2];
				}
}

void	CrewModuleDynamics::calcVelocity()	{
				double	mass_reciprocal	=	(1.0/mass_vehicle);
				V_SCALE(velocity,	momentum,	mass_reciprocal);
}

void	CrewModuleDynamics::calcRDot()	{
				double	omega_skew[3][3];
				calcAngularVelocity();
				V_SKEW(omega_skew,	angular_velocity);
				MxM(Rdot,	omega_skew,	R);
}

void	CrewModuleDynamics::calcAngularVelocity()	{
				double	R_transpose[3][3];
				double	R_I_body_inverse[3][3];
				double	I_world_inverse[3][3];
				MxM(R_I_body_inverse,	R,	I_body_inverse);
				M_TRANS(R_transpose,	R);
				MxM(I_world_inverse,	R_I_body_inverse,	R_transpose);
				MxV(angular_velocity,	I_world_inverse,	angular_momentum);
}

void	CrewModuleDynamics::calcBuoyancyTorque()	{
				double	lever[3];
				V_SUB(lever,	center_of_buoyancy,	position);
				V_CROSS(torque_buoyancy,	lever,	force_buoyancy);
}

void	CrewModuleDynamics::calcDragTorque()	{
				V_SCALE(torque_drag,	angular_velocity,	-5000.0);
}

void	CrewModuleDynamics::calcTotalTorque()	{
				calcBuoyancyTorque();
				calcDragTorque();
				V_ADD(torque_total,	torque_buoyancy,	torque_drag);
}

void	CrewModuleDynamics::calcMassDisplaced()	{
				mass_displaced	=	density_of_water	*	volume_displaced;
}

void	CrewModuleDynamics::calcBuoyancyForce()	{
				calcMassDisplaced();
				V_SCALE(force_buoyancy,	-acceleration_of_gravity,	mass_displaced);
}

void	CrewModuleDynamics::calcGravityForce()	{
				V_SCALE(force_gravity,	acceleration_of_gravity,	mass_vehicle);
}

void	CrewModuleDynamics::calcDragForce()	{
				V_SCALE(force_drag,	velocity,	-2000.0);
}

void	CrewModuleDynamics::calcTotalForce()	{
				calcGravityForce();
				calcDragForce();
				/*	force_buoyancy	is	calculated	in	calc_derivatives()	because	two	different	derivative	calculations	depend	on	it.	*/
				V_ADD(force_total,	force_gravity,	force_buoyancy);
				V_ADD(force_total,	force_total,	force_drag);
}

int	CrewModuleDynamics::calc_derivatives()	{
				crewModuleShape.transformCoordinates(R,	position);
				calcVolumeAndCenterOfBuoyancy	();
				calcBuoyancyForce();
				calcTotalForce();
				calcVelocity();
				calcTotalTorque();
				calcRDot();
				return	0;
}

int	CrewModuleDynamics::calc_state()	{
				int	ipass;
				load_state(
								&momentum[0],	&momentum[1],	&momentum[2],
								&position[0],	&position[1],	&position[2],
								&angular_momentum[0],	&angular_momentum[1],	&angular_momentum[2],
								&R[0][0],&R[0][1],&R[0][2],
								&R[1][0],&R[1][1],&R[1][2],
								&R[2][0],&R[2][1],&R[2][2],
								NULL);
				load_deriv(
								&force_total[0],	&force_total[1],	&force_total[2],
								&velocity[0],	&velocity[1],	&velocity[2],
								&torque_total[0],	&torque_total[1],	&torque_total[2],
								&Rdot[0][0],&Rdot[0][1],&Rdot[0][2],
								&Rdot[1][0],&Rdot[1][1],&Rdot[1][2],
								&Rdot[2][0],&Rdot[2][1],&Rdot[2][2],
								NULL);
				ipass	=	integrate();
				unload_state(
								&momentum[0],	&momentum[1],	&momentum[2],
								&position[0],	&position[1],	&position[2],
								&angular_momentum[0],	&angular_momentum[1],	&angular_momentum[2],
								&R[0][0],&R[0][1],&R[0][2],
								&R[1][0],&R[1][1],&R[1][2],
								&R[2][0],&R[2][1],&R[2][2],
								NULL);
								int	axis	=	0;
								dm_orthonormal(R,	&axis);	//	Square	up	the	rotation	matrix.
				return(ipass);
}
