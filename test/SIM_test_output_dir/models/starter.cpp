#include	<cstdio>
#include	"starter.h"
#include	"trick/integrator_c_intf.h"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/mm_macros.hh"

Starter::Starter()	{

}

int	Starter::default_data()	{
		//	Allocate	a	dynamic	object
		DynamicObj	*	obj	=	new	DynamicObj()	;
		obj->a	=	5;
		obj->b	=	"Hello	world!";
		obj->c	=	(double	*)	malloc	(sizeof(double)	*	3);
		obj->c[0]	=	0.0;
		obj->c[1]	=	1.0;
		obj->c[2]	=	2.0;

		TMM_declare_ext_var(obj,	TRICK_STRUCTURED,"DynamicObj",	0,	"DynamicObj_alloc",	0,	NULL)	;

		return	0;
}

int	Starter::init()	{
		return	0;
}


int	Starter::scheduled()	{
		message_publish(MSG_NORMAL,	"Hello	World!\n");
		return	0;
}
int	Starter::deriv()	{
		return	0;
}

int	Starter::integ()	{

		int	ipass;
		load_state(	
				NULL	/*	list	is	NULL	terminated	*/
		);

			/*	LOAD	THE	POSITION	AND	VELOCITY	STATE	DERIVATIVES	*/
		load_deriv(
				NULL	/*	list	is	NULL	terminated	*/
		);

			/*	Call	the	Trick	integrate	service	*/
			ipass	=	integrate();

			/*	unload	new	state	*/
		unload_state(
				NULL	/*	list	is	NULL	terminated	*/
		);

		/*	returns	0	if	integerate()	was	successful	*/
		return	ipass;
}

int	Starter::shutdown()	{
		return	0;
}
