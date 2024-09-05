#if	(	__APPLE__	)
#include	<GLUT/glut.h>
#else
#include	<GL/glut.h>
#endif
#include	"Scene.hh"
#include	<iostream>
#include	<vector>
#include	"trick/tc_proto.h"
#include	"trick/vector_macros.h"
#include	<stdio.h>

#ifndef	PI
#define	PI	(double)3.14159265358979323846
#endif

void	*	dl_handle	;
std::vector<SceneElement*>	element_list	;
std::vector<Camera*>	camera_list	;
int	window_ID;

double	near_clip_distance	=	10.0;
double	far_clip_distance	=	100000000.0;

Camera*	current_camera;
Camera*	satellite_camera;

double	camera_azimuth	=	0.0;
double	camera_range	=	100.0;

SceneElement*	satellite;
SceneElement*	planet;

TCDevice	listen_device;
TCDevice		connection;
Comm_packet	comm_packet;




SceneElement*	add_element(const	char*	model_name)	{

			SceneElement*	scene_element	=	new	SceneElement(model_name);
			element_list.push_back(	scene_element	);
			return	(	scene_element	);

}


void	display_init()	{

			near_clip_distance	=	10.0;
			far_clip_distance	=	100000000.0;
			current_camera	=	NULL;
			window_ID	=	0;

			/*	Init	glut	*/
			glutInitDisplayMode	(GLUT_DOUBLE	|	GLUT_RGB	|	GLUT_DEPTH);
			glutInitWindowSize	(810,	500);
			glutInitWindowPosition	(100,	100);
			window_ID	=	glutCreateWindow	("Satellite	Display");

}


void	keyboard(unsigned	char	key,	int	x,	int	y)	{

				switch	(key)	{

								case	'a':
									camera_azimuth	+=	1.0;
									if	(camera_azimuth	>	180.0)	camera_azimuth	-=	360.0;
								break;

								case	'd':
									camera_azimuth	-=	1.0;
									if	(camera_azimuth	<	-180.0)	camera_azimuth	+=	360.0;
								break;

								case	'w':
									camera_range	-=	1.0;
									if	(camera_range	<	20.0)	camera_range	=	20.0;
								break;

								case	's':
									camera_range	+=	1.0;
									if	(camera_range	>	500.0)	camera_range	=	500.0;
								break;

				}

}


void	display()	{

				glClear	(GL_COLOR_BUFFER_BIT	|	GL_DEPTH_BUFFER_BIT	);

				glLoadIdentity();

				current_camera->look();

				GLfloat	light_ambient[]	=	{	1.0,	1.0,	1.0,	1.0	};
				GLfloat	light_position[]	=	{	200.0,	200.0,	0.0,	1.0	};

				glLightfv(GL_LIGHT0,	GL_POSITION,	light_position);
				glLightfv(GL_LIGHT0,	GL_AMBIENT,	light_ambient);

				glEnable(GL_LIGHT0);
				glEnable(GL_DEPTH_TEST);

				glEnable(GL_BLEND);

				int	n	=		(int)element_list.size();
				for	(int	ii=0	;	ii	<	n	;	ii++)	{
								element_list[ii]->display();
				}

				glutSwapBuffers();
}

void	reshape(int	width,	int	height)	{

				glMatrixMode	(GL_PROJECTION);
				glLoadIdentity	();
				gluPerspective(30.0,	(GLdouble)width	/	(GLdouble)height,	near_clip_distance,	far_clip_distance);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				glViewport	(0,	0,	(GLsizei)	width,	(GLsizei)	height);

				current_camera->look();

}

void	comm_initialize()	{

				memset(&listen_device,	'\0',	sizeof(TCDevice));
				memset(&connection,	'\0',	sizeof(TCDevice));
				listen_device.port	=	9000;
				connection.blockio_type	=	TC_COMM_NOBLOCKIO;
				tc_init(&listen_device);
				tc_accept(&listen_device,	&connection);

}

void	comm_read()	{

				int	nbytes;
				double	packet[15];

				if	(	tc_isValid(	&connection)	&&	tc_pending(	&connection))	{

								nbytes	=	tc_read(&	connection,	(char*)&packet,	sizeof(packet));

								//	Satellite	Position
//								satellite->pos[0]	=	packet[0];
//								satellite->pos[1]	=	packet[1];
//								satellite->pos[2]	=	packet[2];

								satellite->pos[0]	=	0.0;
								satellite->pos[1]	=	0.0;
								satellite->pos[2]	=	0.0;

								planet->pos[0]	=	-packet[0];
								planet->pos[1]	=	-packet[1];
								planet->pos[2]	=	-packet[2];

								//	Satellite	Velocity
								satellite->vel[0]	=	packet[3];
								satellite->vel[1]	=	packet[4];
								satellite->vel[2]	=	packet[5];

								//	Vehicle	Body	to	World	Rotation	Matrix
								satellite->R[0][0]	=	packet[6];
								satellite->R[0][1]	=	packet[7];
								satellite->R[0][2]	=	packet[8];
								satellite->R[1][0]	=	packet[9];
								satellite->R[1][1]	=	packet[10];
								satellite->R[1][2]	=	packet[11];
								satellite->R[2][0]	=	packet[12];
								satellite->R[2][1]	=	packet[13];
								satellite->R[2][2]	=	packet[14];

								//	The	camera	position	is	relative	to	the	satellite	position.
								satellite_camera->pos[0]	=	satellite->pos[0]	+	camera_range	*	sin(camera_azimuth	*	(PI/180.0));
								satellite_camera->pos[1]	=	satellite->pos[1]	+	camera_range	*	cos(camera_azimuth	*	(PI/180.0));
								satellite_camera->pos[2]	=	satellite->pos[2]	+	0.0;

								//	The	camera	is	looking	at	the	satellite.
								satellite_camera->tgt[0]	=	satellite->pos[0];
								satellite_camera->tgt[1]	=	satellite->pos[1];
								satellite_camera->tgt[2]	=	satellite->pos[2];

								//	Camera	up	is	in	the	positive	World-Z	direction;
								satellite_camera->up[0]	=	0.0;
								satellite_camera->up[1]	=	0.0;
								satellite_camera->up[2]	=	1.0;

								glutPostRedisplay();

				}

}

int	main(int	argc,	char*	argv[])	{

				glutInit(&argc,	argv);

				display_init();

				comm_initialize();

			/*	Call	backs	*/
				glutDisplayFunc(	display);
				glutReshapeFunc(	reshape);
				glutKeyboardFunc(	keyboard);
				glutIdleFunc(	comm_read);

				satellite	=	add_element("Satellite");
				satellite_camera	=	new	Camera();
				current_camera	=	satellite_camera;

				planet	=	add_element("Ball");
				for	(int	ii=0;	ii<3	;	ii++)	{	planet->pos[ii]	=	0.0;	}

				glutMainLoop();

				return	0;

}
