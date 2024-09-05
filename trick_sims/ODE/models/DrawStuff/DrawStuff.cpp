
#include	<string.h>
#include	<unistd.h>
#include	"DrawStuff.hh"

std::vector<DrawStuffObject	*>	DrawStuff::objects	;

void	DrawStuff::start()	{
				static	float	xyz[3]	=	{2.0f,-2.0f,1.7600f};
				static	float	hpr[3]	=	{140.000f,-17.0000f,0.0000f};
				dsSetViewpoint	(xyz,hpr);
}

void	DrawStuff::step(int)	{
				std::vector<DrawStuffObject	*>::iterator	it	;
				for(	it	=	objects.begin()	;	it	!=	objects.end()	;	++it	)	{
								(*it)->draw()	;
				}
}

void	DrawStuff::command(int	cmd)	{
				std::vector<DrawStuffObject	*>::iterator	it	;
				for(	it	=	objects.begin()	;	it	!=	objects.end()	;	++it	)	{
								(*it)->command(cmd)	;
				}
}

DrawStuff::DrawStuff()	:
	fn(),
	window_width(1024),
	window_height(768)
{
				fn.version	=	DS_VERSION;
				fn.path_to_textures	=	"/users/alin/ode-0.14/drawstuff/textures";

				fn.start	=	&DrawStuff::start	;
				fn.step	=	&DrawStuff::step	;
				fn.command	=	&DrawStuff::command	;
}

int	DrawStuff::draw()	{
				dsSimulationLoop(0	,	NULL,	window_width,	window_height,	&fn)	;
				return	0	;
}

int	DrawStuff::shutdown()	{
				dsStop()	;
				//	wait	a	second	to	allow	time	for	the	loop	to	exit.		Yeah,	it's	unscientific.
				sleep(1)	;
				return	0	;
}

void	DrawStuff::add_object(	DrawStuffObject	*	dso	)	{
				objects.push_back(dso)	;
}
