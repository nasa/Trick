#if	(	__APPLE__	)
#include	<GLUT/glut.h>
#else
#include	<GL/glut.h>
#endif
#include	"Camera.hh"
#include	<iostream>

Camera::Camera()	{
				pos[0]	=	0.0;
				pos[1]	=	0.0;
				pos[2]	=	0.0;

				tgt[0]	=	0.0;
				tgt[1]	=	0.0;
				tgt[2]	=	0.0;

				up[0]	=	0.0	;
				up[1]	=	0.0	;
				up[2]	=	1.0	;
}

void	Camera::look()	{

				gluLookAt	(pos[0],	pos[1],	pos[2],
															tgt[0],	tgt[1],	tgt[2],
															up[0],		up[1],		up[2]
														);
}
