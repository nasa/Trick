#if	(	__APPLE__	)
#							include	<GLUT/glut.h>
#else
#							include	<GL/glut.h>
#endif
#include	"SceneElement.hh"
#include	<stdlib.h>
#include	<string.h>
#include	<dlfcn.h>
#include	<stdio.h>

SceneElement::SceneElement(const	char*	model_name)	{
				char	name[128];

				pos[0]	=	0.0;
				pos[1]	=	0.0;
				pos[2]	=	0.0;

				vel[0]	=	1.0;
				vel[1]	=	0.0;
				vel[2]	=	0.0;

				R[0][0]	=	1.0;
				R[0][1]	=	0.0;
				R[0][2]	=	0.0;
				R[0][3]	=	0.0;

				R[1][0]	=	0.0;
				R[1][1]	=	1.0;
				R[1][2]	=	0.0;
				R[1][3]	=	0.0;

				R[2][0]	=	0.0;
				R[2][1]	=	0.0;
				R[2][2]	=	1.0;
				R[2][3]	=	0.0;

				R[3][0]	=	0.0;
				R[3][1]	=	0.0;
				R[3][2]	=	0.0;
				R[3][3]	=	1.0;

				dl_handle	=	dlopen(	NULL,	RTLD_LAZY	)	;

				name[0]	=	'\0';
				strcat(name,	model_name);
				strcat(name,	"_initialize");

				if	((model_init_func	=	(void(*)(void))	dlsym(dl_handle,	name))	!=	NULL)	{
								(*model_init_func)();
				}	else	{
								fprintf(stderr,	"Failed	to	find	%s.\n",	name);
								fflush(stdout);
				}

				name[0]	=	'\0';
				strcat(name,	model_name);
				strcat(name,	"_display");

				if	((model_display_func	=	(void(*)(void))	dlsym(dl_handle,	name))	==	NULL)	{
								printf("Failed	to	find	%s.\n",	name);
								fflush(stdout);
				}

};

void	SceneElement::init()	{
				if	(model_init_func	!=	0)	{
						(*model_init_func)();
				}
}

void	SceneElement::display()	{

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslated	(pos[0],	pos[1],	pos[2]);
				glMultMatrixd((GLdouble*)R);
				if	(model_display_func	!=	0)	{
						(*model_display_func)();
				}
				glPopMatrix();

}
