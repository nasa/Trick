/*********************************	TRICK	HEADER	*******************************
PURPOSE:	(Simulate	a	pool	table.)
LIBRARY	DEPENDENCY:
				((pool_table.o)
					(ball.o))
*******************************************************************************/
#include	<math.h>
#include	<iostream>
#include	"trick/integrator_c_intf.h"
#include	"trick/memorymanager_c_intf.h"
#include	"../include/pool_table.hh"
#include	"trick/trick_math_proto.h"


//	Leaving	these	here,	but	should	probably	go	somewhere	else
double	dot(																		/*	Return:	Scalar	dot	or	inner	product	*/
																	double	vec1[],								/*	In:	Vector	1	*/
																	double	vec2[],								/*	In:	Vector	2	*/
																	int	dim)
{																		
				double	dot	=	0;
				for	(int	i	=	0;	i	<	dim;	i++)	{
								dot	+=	vec1[i]	*	vec2[i];
				}																				
				return	dot;
}

void	scaleInPlace	(double	vec[],	double	scale,	int	dim)	{
				for	(int	i	=	0;	i	<	dim;	i++)	{
								vec[i]	*=	scale;
				}
}

void	mProduct	(double	product[2],	double	matrix[2][2],	double	vec[2])	{
				product[0]	=	matrix[0][0]	*	vec[0]	+	matrix[1][0]	*	vec[1];
				product[1]	=	matrix[0][1]	*	vec[0]	+	matrix[1][1]	*	vec[1];
}

int	PoolTable::default_data()	{
				//	balls.clear();
				//	bumpers.clear();

				numBalls	=	0;
				numBumpers	=	0;
				return	0;
}

//	Input	stage	runs	before	this,	which	populates	balls	and	bumpers
int	PoolTable::state_init()	{
				
				//	Do	regula	falsi	setup	here

				//	Vars	for	ball/ball	collisions
				double	now	;
				numAssociations	=	(numBalls*(numBalls-1))/2;
				ballAssociations	=	(REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI",	numAssociations);
				unsigned	int	ii,jj;
				for	(ii=1;	ii<numBalls;	ii++)	{
								for	(jj=0;	jj<ii;	jj++)	{
												unsigned	int	association_index	=	ii*(ii-1)/2+jj;
												ballAssociations[association_index].mode	=	Decreasing;
												ballAssociations[association_index].error_tol	=	0.0000001;
												now	=	get_integ_time()	;
												reset_regula_falsi(	now,	&ballAssociations[association_index]	);
								}
				}

				//	Bumper/ball	collisions
				bumperBallCombos	=	numBalls	*	numBumpers;
				bumperAssociations	=	(REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI",	bumperBallCombos);
				for	(ii=0;	ii<numBalls;	ii++)	{
								for	(jj=0;	jj<numBumpers;	jj++)	{
												unsigned	int	association_index	=	(ii*numBumpers)	+	jj;
												bumperAssociations[association_index].mode	=	Any;
												bumperAssociations[association_index].error_tol	=	0.0000001;
												now	=	get_integ_time()	;
												reset_regula_falsi(	now,	&bumperAssociations[association_index]	);
								}
				}

				//	Need	to	do	the	same	thing	with	pocket/ball	associations
				pocketBallCombos	=	numBalls	*	numPockets;
				pocketAssociations	=	(REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI",	pocketBallCombos);
				for	(ii=0;	ii<numBalls;	ii++)	{
								for	(jj=0;	jj<numPockets;	jj++)	{
												unsigned	int	association_index	=	(ii*numPockets)	+	jj;
												pocketAssociations[association_index].mode	=	Decreasing;
												pocketAssociations[association_index].error_tol	=	0.0000001;
												now	=	get_integ_time()	;
												reset_regula_falsi(	now,	&pocketAssociations[association_index]	);
								}
				}

				return	0;
}

int	PoolTable::state_deriv()	{
				//	derivative	-	determine	the	acceleration	here
				//	This	is	probably	just	friction
				//	And	angular	stuff
				//	F	=	MA	=>	A	=	M^-1	*	F

				//	Will	have	to	account	for	rolling	vs	sliding	here
				//	For	now,	just	rolling

				for	(int	i	=	0;	i	<	numBalls;	i++)	{
								//	Frictional	force	a	constant	applied	in	the	opposing	direction	of	velocity.
								//	Magnitude	of	velocity	is	irrelevant	unless	very	close	to	0
								//	Has	weird	behavior	when	velocity	is	very	small,	so	only	apply	friction	if	velocity	is	greater	than	a	tolerance

								if	(!balls[i]->inPlay)	{
												continue;
								}

								if	(balls[i]->vel.norm()	>	frictionTolerance)	{
												Vec	velocityNorm	=	balls[i]->vel.normalized();

												balls[i]->accel[0]	=	-	(frictionScale	*	frictionRolling	*	velocityNorm[0]);
												balls[i]->accel[1]	=	-	(frictionScale	*	frictionRolling	*	velocityNorm[1]);
												balls[i]->accel[2]	=	0;
								}	else	{
												balls[i]->vel[0]	=	0;
												balls[i]->vel[1]	=	0;
												balls[i]->vel[2]	=	0;

												balls[i]->accel[0]	=	0;
												balls[i]->accel[1]	=	0;
												balls[i]->accel[2]	=	0;

								}


				}

				return	0;
}

int	PoolTable::state_integ()	{
				//	Apply	the	acceleration	by	changing	the	velocity	by	the	appropriate	amount	over	the	time	step	

				//	How	many	state	variables	are	needed	for	each	ball
				int	n	=	6;

				for	(int	i	=	0;	i	<	numBalls;	i++)	{
								if	(!balls[i]->inPlay)	{
												continue;
								}

								int	inner_index	=	0;
								load_indexed_state(n*i	+	inner_index++,	balls[i]->pos[0]);
								load_indexed_state(n*i	+	inner_index++,	balls[i]->pos[1]);
								load_indexed_state(n*i	+	inner_index++,	balls[i]->pos[2]);

								load_indexed_state(n*i	+	inner_index++,	balls[i]->vel[0]);
								load_indexed_state(n*i	+	inner_index++,	balls[i]->vel[1]);
								load_indexed_state(n*i	+	inner_index++,	balls[i]->vel[2]);

								//	Derivatives	of	all	of	this	junk
								inner_index	=	0;
								load_indexed_deriv(n*i	+	inner_index++,	balls[i]->vel[0]);
								load_indexed_deriv(n*i	+	inner_index++,	balls[i]->vel[1]);
								load_indexed_deriv(n*i	+	inner_index++,	balls[i]->vel[2]);

								load_indexed_deriv(n*i	+	inner_index++,	balls[i]->accel[0]);			
								load_indexed_deriv(n*i	+	inner_index++,	balls[i]->accel[1]);			
								load_indexed_deriv(n*i	+	inner_index++,	balls[i]->accel[2]);			

				}

				int	integration_step	=	integrate();

				for	(int	i	=	0;	i	<	numBalls;	i++)	{
								if	(!balls[i]->inPlay)	{
												continue;
								}

								//	pos[0]	pos[1]	pos[2]	vel[0]	vel[1]	vel[2]
								int	inner_index	=	0;
								balls[i]->pos[0]	=	unload_indexed_state(n*i	+	inner_index++);
								balls[i]->pos[1]	=	unload_indexed_state(n*i	+	inner_index++);
								balls[i]->pos[2]	=	unload_indexed_state(n*i	+	inner_index++);

								balls[i]->vel[0]	=	unload_indexed_state(n*i	+	inner_index++);
								balls[i]->vel[1]	=	unload_indexed_state(n*i	+	inner_index++);
								balls[i]->vel[2]	=	unload_indexed_state(n*i	+	inner_index++);

								balls[i]->accel[0]	=	0;
								balls[i]->accel[1]	=	0;
								balls[i]->accel[2]	=	0;

				}

				return	integration_step;
}

double	closestPointOnLine(Line&	line,	Vec	pos,	Vec&	result,	bool	print)	{
				Vec	a(line.p1);
				Vec	b(line.p2);
				Vec	diff	=	pos	-	a;
				Vec	m	=	b	-	a;

				double	t	=	(diff	*	m)	/	(m	*	m);

				if	(t	<	0)
								t	=	0;
				
				if	(t	>	1)
								t	=	1;

				m	=	m	*	t;
				result	=	a	+	m;

				return	t;
}

double	PoolTable::removeBall(int	id)	{
				balls[id]->inPlay	=	false;
				balls[id]->clearAllState();
}


double	PoolTable::collision()	{

				//	Handle	when	the	balls	collide	with	others

				double	now	;	/*	current	integration	time.	*/
				double	event_tgo;
				unsigned	int	ii,jj;

				now	=	get_integ_time()	;
				event_tgo	=	BIG_TGO;

				std::vector<unsigned	int>	collisionsToProcess;

				for	(ii=1;	ii<numBalls;	ii++)	{
								if	(!balls[ii]->inPlay)	{
																continue;
								}
								for	(jj=0;	jj<ii;	jj++)	{
												if	(!balls[jj]->inPlay)	{
																continue;
												}

												unsigned	int	associationIndex	=	ii*(ii-1)/2+jj;

												//	boundary	is	distance	between	balls	-	radiuses	of	balls
												double	distanceBetweenBalls	=	(balls[ii]->pos	-	balls[jj]->pos).norm();
												double	error	=	distanceBetweenBalls	-	(balls[ii]->radius	+	balls[jj]->radius);
												ballAssociations[associationIndex].error	=	error;

												double	this_tgo	=	regula_falsi(	now,	&(ballAssociations[associationIndex]))	;
												
												if	(this_tgo	<	event_tgo)	{
																event_tgo	=	this_tgo;
												}

												if	(this_tgo	==	0)	{
																std::cout	<<	"Found	colliding	balls"	<<	std::endl;

																//	Add	this	collision	to	a	list	of	collisions	to	process
																//	probably	don't	need	to	do	this
																collisionsToProcess.push_back(ii);
																collisionsToProcess.push_back(jj);
																reset_regula_falsi(	now,	&(ballAssociations[associationIndex])	);

												}	else	{
																regula_falsi_set_upper	(now,	error,	&ballAssociations[associationIndex]);
												}
								}
				}

				//	Handle	collisions
				for	(int	i	=	0;	i	<	collisionsToProcess.size();	i+=2)	{
								int	index1	=	collisionsToProcess[i];
								int	index2	=	collisionsToProcess[i+1];
								std::cout	<<	"Collision	detected	between	balls	"	<<	index1	<<	"	and	"	<<	index2	<<	std::endl;

								Vec	q1(balls[index1]->pos);
								Vec	q2(balls[index2]->pos);

								//	dg	=	(q1	-	q2)	/	(|q1	-	q2|)
								Vec	diff	=	q1	-	q2;
								Vec	dg	=	diff.normalized();

								//	Have	to	stuff	both	velocities	and	dg	values	into	4d	vector	to	do	the	calculation
								//	Otherwise	I	have	to	do	more	math
								double	dg4[4]	=	{dg[0],	dg[1],	-dg[0],	-dg[1]};
								double	vel4[4]	=	{balls[index1]->vel[0],	balls[index1]->vel[1],	balls[index2]->vel[0],	balls[index2]->vel[1]};

								//	Calculate	the	impulse
								//	J	=	((-(1	+	c)	*	dg	*	v)	/	(dg	*	M^-1	*	dg^T)	)	dg
								//	For	now	let's	just	pretend	all	the	masses	are	1
								double	impulse	=	((1.0	+	coefficientOfElasticity)	*	dot(dg4,	vel4,	4))	/	(dot(dg4,	dg4,	4));
								scaleInPlace(dg4,	impulse,	4);

								//	Impulse[0:1]	is	x	and	y	components	of	what	should	be	applied	to	v1,	Impulse[2:3]	goes	to	v2
								Vec	impulse1(dg4[0],	dg4[1],	0);
								Vec	impulse2(dg4[2],	dg4[3],	0);

								Vec	newV1(balls[index1]->vel);
								Vec	newV2(balls[index2]->vel);

								newV1	=	((newV1	*	balls[index1]->mass)	-	impulse1)	*	(1.0	/	balls[index1]->mass);
								newV2	=	((newV2	*	balls[index2]->mass)	-	impulse2)	*	(1.0	/	balls[index2]->mass);

								std::cout	<<	"Impulses	applied:	\n\tV1:	"	<<	newV1[0]	<<	"	"	<<	newV1[1]	<<	"	\n\tV2:	"	<<	newV2[0]	<<	"	"	<<	newV2[1]	<<	std::endl;

								balls[index1]->vel[0]	=	newV1[0];
								balls[index1]->vel[1]	=	newV1[1];

								balls[index2]->vel[0]	=	newV2[0];
								balls[index2]->vel[1]	=	newV2[1];
				}

				int	numBumperCollisions	=	0;
				for	(ii=0;	ii<numBalls;	ii++)	{
								if	(!balls[ii]->inPlay)	{
																continue;
								}
								for	(jj=0;	jj<numBumpers;	jj++)	{
												unsigned	int	association_index	=	(ii*numBumpers)	+	jj;

												Ball	*ball	=	balls[ii];
												Bumper	*bumper	=	bumpers[jj];

												Vec	closestPointOnBumper;
												double	t	=	closestPointOnLine(bumper->border,	ball->pos,	closestPointOnBumper,	true);
												double	distanceToBumper	=	(ball->pos	-	closestPointOnBumper).norm()	-	ball->radius;
												bumperAssociations[association_index].error	=	distanceToBumper;

												double	this_tgo	=	regula_falsi(	now,	&(bumperAssociations[association_index]))	;
												if	(this_tgo	<	event_tgo)	{
																event_tgo	=	this_tgo;
												}

												if	(this_tgo	==	0)	{
																numBumperCollisions++;
																std::cout	<<	"Found	colliding	ball	"	<<	ii	<<	"	and	bumper	"	<<	jj	<<	std::endl;
																reset_regula_falsi(	now,	&(bumperAssociations[association_index])	);

																if	(numBumperCollisions	>	1)	{
																				//	When	a	ball	hits	exactly	a	corner	of	2	bumpers,	both	collisions	will	be	detected	and	impulses	applied.
																				//	But	since	an	impulse	takes	into	consideration	the	original	velocity,	the	second	impulse	applied
																				//	just	cancels	out	the	first	one	most	of	the	time.	Both	impulses	should	be	identical	since	the
																				//	collision	point	is	the	same,	so	make	sure	only	1	gets	applied.	This	is	kind	of	hacky
																				continue;
																}
																				
																Vec	q1(ball->pos);
																Vec	q2(closestPointOnBumper);

																Vec	diff	=	q1	-	q2;
																Vec	dg	=	diff.normalized();

																//	Have	to	stuff	both	velocities	and	dg	values	into	4d	vector	to	do	the	calculation
																//	Otherwise	I	have	to	do	more	math
																double	dg4[4]	=	{dg[0],	dg[1],	-dg[0],	-dg[1]};
																double	vel4[4]	=	{ball->vel[0],	ball->vel[1],	0,	0};

																//	Calculate	the	impulse
																//	J	=	((-(1	+	c)	*	dg	*	v)	/	(dg	*	M^-1	*	dg^T)	)	dg
																//	Really	need	to	get	some	better	matrix	math	in	here
																//	only	used	the	dot	product	of	the	first	2	in	the	last	term	since	it's	supposed	to	be	dg	*	minv	*	dg
																//	and	since	the	bumpers	are	immovable	the	mass	is	infinite	which	inverse	is	0
																//	So	this	isn't	robust	at	all
																//	Setting	the	CoE	to	be	1	always	-	perfectly	elastic
																//	Otherwise	there's	a	weird	bouncy	thing
																double	impulse	=	((1.0	+	1.0)	*	dot(dg4,	vel4,	4))	/	(dot(dg4,	dg4,	2));
																std::cout	<<	"Impulse:	"	<<	impulse	<<	std::endl;
																scaleInPlace(dg4,	impulse,	4);

																//	Impulse[0:1]	is	x	and	y	components	of	what	should	be	applied	to	v1,	Impulse[2:3]	goes	to	v2
																Vec	impulse1(dg4[0],	dg4[1],	0);
																Vec	impulse2(dg4[2],	dg4[3],	0);


																Vec	newV1	(ball->vel);

																newV1	=	((newV1	*	ball->mass)	-	impulse1)	*	(1.0	/	ball->mass);

																std::cout	<<	"Impulses	applied:	\n\tV1:	"	<<	newV1[0]	<<	"	"	<<	newV1[1]	<<	std::endl;

																ball->vel[0]	=	newV1[0];
																ball->vel[1]	=	newV1[1];

												}	else	{
																regula_falsi_set_upper	(now,	distanceToBumper,	&bumperAssociations[association_index]);
												}
								}

				}
				if	(numBumperCollisions	>	0)	{
								std::cout	<<	"In	this	time	step,	"	<<	numBumperCollisions	<<	"	collisions	applied"	<<	std::endl;
				}

				//	Pockets
				for	(ii=0;	ii<numBalls;	ii++)	{
								if	(!balls[ii]->inPlay)	{
																continue;
								}
								for	(jj=0;	jj<numPockets;	jj++)	{
												unsigned	int	association_index	=	(ii*numPockets)	+	jj;
												Ball	*ball	=	balls[ii];
												Pocket	*pocket	=	pockets[jj];

												//	Ball	is	pocketed	when	center	is	within	pocket	radius
												double	error	=	(ball->pos	-	pocket->pos).norm()	-	(pocket->radius);
												pocketAssociations[association_index].error	=	error;

												double	this_tgo	=	regula_falsi(	now,	&(pocketAssociations[association_index]));

												if	(this_tgo	<	event_tgo)	{
																event_tgo	=	this_tgo;
												}

												if	(this_tgo	==	0)	{
																std::cout	<<	"Found	pocketed	ball	"	<<	ii	<<	std::endl;
																reset_regula_falsi(	now,	&(pocketAssociations[association_index])	);
																removeBall(ii);
												}
								}
				}

				return	event_tgo;
}

void	PoolTable::resetCueBall()	{
				resetCueBall(defaultCueBallX,	defaultCueBallY);
}

void	PoolTable::resetCueBall(double	x,	double	y)	{
				balls[cueBallIndex]->clearAllState();
				balls[cueBallIndex]->setPos(x,	y);

				balls[cueBallIndex]->inPlay	=	true;
}

void	PoolTable::applyCueForce(double	x_end,	double	y_end)	{
				applyCueForce(x_end,	y_end,	0,	0,	0);
}

void	PoolTable::applyCueForce(double	x_end,	double	y_end,	double	cueHorizontalDisplacement,	double	cueVerticalDisplacement,	double	cueAngle)	{
				std::cout	<<	"Applying	cue	force"	<<	std::endl;

				//	assume	index	0	is	cue	ball
				int	cueIndex	=	0;
				Vec	cueEnd(x_end,	y_end);
				Vec	cueBallPos(balls[cueIndex]->pos);

				double	r	=	balls[cueIndex]->radius;
				double	m	=	balls[cueIndex]->mass;
				
				double	a	=	cueHorizontalDisplacement	*	r;
				double	b	=	cueVerticalDisplacement	*	r;
				double	c	=	abs(sqrt(pow(r,2)	-	pow(a,2)	-	pow(b,2)));

				double	theta	=	cueAngle	*	M_PI	/	180.0;
				double	angleScaling	=	1.0	+	(cueAngle	/	25.0);

				Vec	cueAxis	=	cueEnd	-	cueBallPos;
				double	cue_v	=	cueAxis.norm()	*	cueForceScale	*	angleScaling;
				cueAxis	=	cueAxis.normalized();

				double	axis2[2]	=	{cueAxis[0],	cueAxis[1]};

				double	force	=	(2	*	m	*	cue_v)	/	(1.0	*	(m	/	cueMass)	+	(5.0	/	(2.0*pow(r,	2)))	*	(pow(a,2)	+	pow(b,2)*pow(cos(theta),2)	+	pow(c,2)*pow(sin(theta),2)	-	2*b*c*sin(theta)*cos(theta)));

				//	Velocity	in	the	frame	of	the	cue
				double	v_multiplier	=	-force/m	*	cos(theta);
				Vec	velocityAfterHit	=	cueAxis	*	v_multiplier;

				std::cout	<<	"Velocity	of	ball	after	cue	hit:	"	<<	velocityAfterHit[0]	<<	"	"	<<	velocityAfterHit[1]	<<	std::endl;

				balls[cueIndex]->vel	=	velocityAfterHit;

				//	TODO:	logic	for	angular	and	relative	velocity

}


//	Add	a	ball	to	the	table	and	return	the	ID
int	PoolTable::addBall	(double	x,	double	y,	double	mass,	double	radius,	bool	fixed)	{
				int	id	=	nextBallSlot++;
				if	(id	<	numBalls)	{
								Ball	*	ball	=	(Ball*)	TMM_declare_var_s("Ball");
								balls[id]	=	(new	(ball)	Ball(x,y,mass,radius,	fixed,	id));
								return	id;
				}
				return	-1;
}

//	Add	a	bumper	to	the	table	and	return	the	ID
//	Only	takes	in	the	actual	effective	line,	need	to	add	something	else	for	the	rendered	shape
int	PoolTable::addBumper	(int	numPoints,	double	x1,	double	y1,	double	x2,	double	y2)	{
				int	id	=	nextBumperSlot++;
				if	(id	<	numBumpers)	{
								Bumper	*	bumper	=	(Bumper*)	TMM_declare_var_s("Bumper");
								bumpers[id]	=	(new	(bumper)	Bumper(numPoints,	x1,	y1,	x2,	y2));
								return	id;
				}
				return	-1;
}

int	PoolTable::addPointToBumper(int	id,	double	x,	double	y)	{
				Bumper	*	bumper	=	bumpers[id];
				bumper->AddPointToRender(x,	y);

				return	0;
}

int	PoolTable::addPointToTable(double	x,	double	y)	{
				int	id	=	nextTablePointSlot++;
				if	(id	<	numTablePoints)	{
								Vec	*	point	=	(Vec*)	TMM_declare_var_s("Vec");
								tableShape[id]	=	(new	(point)	Vec(x,	y));
								return	id;
				}
				return	-1;
}

int	PoolTable::addPocket(double	x,	double	y,	double	r)	{
				int	id	=	nextPocketSlot++;
				if	(id	<	numPockets)	{
								Pocket	*	pocket	=	(Pocket*)	TMM_declare_var_s("Pocket");
								pockets[id]	=	(new	(pocket)	Pocket(x,	y,	r));
								return	id;
				}
				return	-1;
}



int	PoolTable::setBallPos(int	id,	double	x,	double	y)	{
				if	(id	<	numBalls	&&	balls[id]	!=	NULL)	{
								balls[id]->pos[0]	=	x;
								balls[id]->pos[1]	=	y;
								return	1;
				}

				return	-1;
}

int	PoolTable::setBallVel(int	id,	double	v_x,	double	v_y)	{
				if	(id	<	numBalls	&&	balls[id]	!=	NULL)	{
								balls[id]->vel[0]	=	v_x;
								balls[id]->vel[1]	=	v_y;
								return	1;
				}
				return	-1;
}



