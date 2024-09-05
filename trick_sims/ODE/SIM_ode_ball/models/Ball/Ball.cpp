
#include	<ode/ode.h>
#include	<drawstuff/drawstuff.h>

#include	"Ball.hh"
#include	"trick/memorymanager_c_intf.h"

Ball::Ball()	{}

int	Ball::init()	{
				dInitODE	();
				world	=	dWorldCreate	();
				space	=	dHashSpaceCreate	(0);
				dWorldSetGravity	(world,0,0,-9.81);
				dWorldSetCFM	(world,1e-5);
				dCreatePlane	(space,0,0,1,0);
				contactgroup	=	dJointGroupCreate	(0);

				body	=	dBodyCreate	(world);
				geom	=	dCreateSphere	(space,0.5);
				dMassSetSphere	(&m,1,0.5);
				dBodySetMass	(body,&m);
				dGeomSetBody	(geom,body);
				//	set	initial	position
				dBodySetPosition	(body,0,0,3);

				pos	=	(dReal	*)dGeomGetPosition(geom)	;
				TMM_declare_ext_var_1d(	pos	,	"float"	,	3)	;

				add_object()	;

				return	0	;
}

int	Ball::step(	double	time_step)	{
				//	find	collisions	and	add	contact	joints
				dSpaceCollide	(space,this,&Ball::nearCallback);
				//	step	the	simulation
				//dWorldQuickStep	(world,time_step);
				dWorldStep	(world,time_step);
				//	remove	all	contact	joints
				dJointGroupEmpty	(contactgroup);
				return	0	;
}

int	Ball::shutdown()	{
				//	clean	up
				dJointGroupDestroy	(contactgroup);
				dSpaceDestroy	(space);
				dWorldDestroy	(world);
				dCloseODE();
				return	0	;
}

int	Ball::draw()	{
				dsDrawSphere	(dGeomGetPosition(geom),dGeomGetRotation(geom),dGeomSphereGetRadius(geom));
				return	0	;
}

//	this	is	called	by	dSpaceCollide	when	two	objects	in	space	are
//	potentially	colliding.
void	Ball::nearCallback	(void	*	void_ball,	dGeomID	o1,	dGeomID	o2)	{

				Ball	*	bp	=	(Ball	*)void_ball	;

				dBodyID	b1	=	dGeomGetBody(o1);
				dBodyID	b2	=	dGeomGetBody(o2);
				dContact	contact;
				contact.surface.mode	=	dContactBounce	|	dContactSoftCFM;
				//	friction	parameter
				contact.surface.mu	=	dInfinity;
				//	bounce	is	the	amount	of	"bouncyness".
				contact.surface.bounce	=	0.999;
				//	bounce_vel	is	the	minimum	incoming	velocity	to	cause	a	bounce
				contact.surface.bounce_vel	=	0.1;
				//	constraint	force	mixing	parameter
				contact.surface.soft_cfm	=	0.001;
				if	(int	numc	=	dCollide	(o1,o2,1,&contact.geom,sizeof(dContact)))	{
								dJointID	c	=	dJointCreateContact	(bp->world,bp->contactgroup,&contact);
								dJointAttach	(c,b1,b2);
				}
}

