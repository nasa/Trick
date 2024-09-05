
#include	<ode/ode.h>
#include	<drawstuff/drawstuff.h>

#include	"Buggy.hh"
#include	"trick/memorymanager_c_intf.h"

Buggy::Buggy()	:
	gravity(-0.5),
	chassis_length(0.7),
	chassis_width(0.5),
	chassis_height(0.2),
	chassis_mass(1.0),
	wheel_radius(.18),
	wheel_width(0.02),
	wheel_mass(0.2),
	start_z(0.5)
	{}

int	Buggy::init()	{

				dMass	m;
				int	i	;

				dInitODE2(0);
				world	=	dWorldCreate	();
				space	=	dHashSpaceCreate	(0);
				contactgroup	=	dJointGroupCreate(0);

				dWorldSetGravity	(world,0,0,gravity);

				ground	=	dCreatePlane	(space,0,0,1,0);

				//	chassis	body
				body[0]	=	dBodyCreate	(world);
				dBodySetPosition	(body[0],0,0,start_z);
				dMassSetBox	(&m,1,chassis_length,chassis_width,chassis_height);
				dMassAdjust	(&m,chassis_mass);
				dBodySetMass	(body[0],&m);
				box[0]	=	dCreateBox	(0,chassis_length,chassis_width,chassis_height);
				dGeomSetBody	(box[0],body[0]);

				buggy_body_pos	=	(dReal	*)dGeomGetPosition(box[0])	;
				TMM_declare_ext_var_1d(	buggy_body_pos	,	"float"	,	3)	;

				//	wheel	bodies
				for	(i=1;	i<=3;	i++)	{
								body[i]	=	dBodyCreate	(world);
								dQuaternion	q;
								dQFromAxisAndAngle	(q,1,0,0,M_PI*0.5);
								dBodySetQuaternion	(body[i],q);
								dMassSetSphere	(&m,1,wheel_radius);
								dMassAdjust	(&m,wheel_mass);
								dBodySetMass	(body[i],&m);
								sphere[i-1]	=	dCreateSphere	(0,wheel_radius);
								dGeomSetBody	(sphere[i-1],body[i]);
				}
				dBodySetPosition	(body[1],0.5*chassis_length,0,start_z-chassis_height*0.5);
				dBodySetPosition	(body[2],-0.5*chassis_length,	chassis_width*0.5,start_z-chassis_height*0.5);
				dBodySetPosition	(body[3],-0.5*chassis_length,-chassis_width*0.5,start_z-chassis_height*0.5);

				//	front	and	back	wheel	hinges
				for	(i=0;	i<3;	i++)	{
								joint[i]	=	dJointCreateHinge2	(world,0);
								dJointAttach	(joint[i],body[0],body[i+1]);
								const	dReal	*a	=	dBodyGetPosition	(body[i+1]);
								dJointSetHinge2Anchor	(joint[i],a[0],a[1],a[2]);
								dJointSetHinge2Axis1	(joint[i],0,0,1);
								dJointSetHinge2Axis2	(joint[i],0,1,0);
				}

				//	set	joint	suspension
				for	(i=0;	i<3;	i++)	{
								dJointSetHinge2Param	(joint[i],dParamSuspensionERP,0.4);
								dJointSetHinge2Param	(joint[i],dParamSuspensionCFM,0.8);
				}

				//	lock	back	wheels	along	the	steering	axis
				for	(i=1;	i<3;	i++)	{
								//	set	stops	to	make	sure	wheels	always	stay	in	alignment
								dJointSetHinge2Param	(joint[i],dParamLoStop,0);
								dJointSetHinge2Param	(joint[i],dParamHiStop,0);
				}

				//	create	car	space	and	add	it	to	the	top	level	space
				car_space	=	dSimpleSpaceCreate	(space);
				dSpaceSetCleanup	(car_space,0);
				dSpaceAdd	(car_space,box[0]);
				dSpaceAdd	(car_space,sphere[0]);
				dSpaceAdd	(car_space,sphere[1]);
				dSpaceAdd	(car_space,sphere[2]);

				//	environment
				ground_box	=	dCreateBox	(space,2,1.5,1);
				dMatrix3	R;
				dRFromAxisAndAngle	(R,0,1,0,-0.15);
				dGeomSetPosition	(ground_box,2,0,-0.34);
				dGeomSetRotation	(ground_box,R);

				//	Add	this	object	to	DrawStuff
				add_object()	;
				return	0	;
}

int	Buggy::step(double	time_step)	{
				dJointSetHinge2Param	(joint[0],dParamVel2,-speed);
				dJointSetHinge2Param	(joint[0],dParamFMax2,0.1);

				//	steering
				dReal	v	=	steer	-	dJointGetHinge2Angle1	(joint[0]);
				if	(v	>	0.1)	v	=	0.1;
				if	(v	<	-0.1)	v	=	-0.1;
				v	*=	10.0;
				dJointSetHinge2Param	(joint[0],dParamVel,v);
				dJointSetHinge2Param	(joint[0],dParamFMax,0.2);
				dJointSetHinge2Param	(joint[0],dParamLoStop,-0.75);
				dJointSetHinge2Param	(joint[0],dParamHiStop,0.75);
				dJointSetHinge2Param	(joint[0],dParamFudgeFactor,0.1);

				dSpaceCollide	(space,this,&Buggy::nearCallback);
				dWorldStep	(world,time_step);

				//	remove	all	contact	joints
				dJointGroupEmpty	(contactgroup);

				return	0	;
}

int	Buggy::shutdown()	{
				dGeomDestroy	(box[0]);
				dGeomDestroy	(sphere[0]);
				dGeomDestroy	(sphere[1]);
				dGeomDestroy	(sphere[2]);
				dJointGroupDestroy	(contactgroup);
				dSpaceDestroy	(space);
				dWorldDestroy	(world);
				dCloseODE();

				return	0	;
}

int	Buggy::draw()	{
				int	i	;

				dsSetColor	(0,1,1);
				dsSetTexture	(DS_WOOD);
				dReal	sides[3]	=	{chassis_length,chassis_width,chassis_height};
				dsDrawBox	(dBodyGetPosition(body[0]),dBodyGetRotation(body[0]),sides);
				dsSetColor	(1,1,1);
				for	(i=1;	i<=3;	i++)	{
								dsDrawCylinder	(dBodyGetPosition(body[i]),	dBodyGetRotation(body[i]),wheel_width,wheel_radius);
				}

				dVector3	ss;
				dGeomBoxGetLengths	(ground_box,ss);
				dsDrawBox	(dGeomGetPosition(ground_box),dGeomGetRotation(ground_box),ss);

				return	0	;
}

void	Buggy::command(	int	cmd	)	{
				switch	(cmd)	{
								case	'a':	case	'A':
												speed	+=	0.3;
												break;
								case	'z':	case	'Z':
												speed	-=	0.3;
												break;
								case	',':
												steer	-=	0.5;
												break;
								case	'.':
												steer	+=	0.5;
												break;
								case	'	':
												speed	=	0;
												steer	=	0;
												break;
								case	'1':	{
												FILE	*f	=	fopen	("state.dif","wt");
												if	(f)	{
														dWorldExportDIF	(world,f,"");
														fclose	(f);
												}
								}
				}
}

//	this	is	called	by	dSpaceCollide	when	two	objects	in	space	are
//	potentially	colliding.
void	Buggy::nearCallback	(void	*	void_buggy	,	dGeomID	o1,	dGeomID	o2)	{

				int	i,n;
				Buggy	*	bp	=	(Buggy	*)void_buggy	;

				//	only	collide	things	with	the	ground
				int	g1	=	(o1	==	bp->ground	||	o1	==	bp->ground_box);
				int	g2	=	(o2	==	bp->ground	||	o2	==	bp->ground_box);
				if	(!(g1	^	g2))	return;

				const	int	N	=	10;
				dContact	contact[N];
				n	=	dCollide	(o1,o2,N,&contact[0].geom,sizeof(dContact));
				if	(n	>	0)	{
								for	(i=0;	i<n;	i++)	{
												contact[i].surface.mode	=	dContactSlip1	|	dContactSlip2	|
														dContactSoftERP	|	dContactSoftCFM	|	dContactApprox1;
												contact[i].surface.mu	=	dInfinity;
												contact[i].surface.slip1	=	0.1;
												contact[i].surface.slip2	=	0.1;
												contact[i].surface.soft_erp	=	0.5;
												contact[i].surface.soft_cfm	=	0.3;
												dJointID	c	=	dJointCreateContact	(bp->world,bp->contactgroup,&contact[i]);
												dJointAttach	(c,	dGeomGetBody(contact[i].geom.g1),	dGeomGetBody(contact[i].geom.g2));
						}
		}
}
