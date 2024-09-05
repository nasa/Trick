#include	<iostream>
#include	<stdlib.h>
#include	<time.h>
#include	"euler_quat_test.hh"
int	main()
{
			EulerQuatTest		test;
			int	ii;

			//	=========		Testing	deuler_*_quat.c	========================
			//	There	are	2	capabilities	for	every	deuler_*_quat.c	function:
			//	1.	Convert	form	euler	to	left	handed	quaternion
			//	2.	Convert	from	left	handed	quaternion	to	euler:	(2	methods)
			//						-	Make	angles	from	quaternion
			//						-	Make	angles	from	quaternion	but	use	previous	values	to
			//								prevent	singularities
			//	============================================================

			srand(time(NULL));
			for	(ii=0;	ii<test.num_tests;	ii++){

						//	Randomly	input	euler	angles:
						test.init_data.euler_angles[0]	=	(rand()%test.num_tests)	*	(2*M_PI/test.num_tests);
						test.init_data.euler_angles[1]	=	(rand()%test.num_tests)	*	(2*M_PI/test.num_tests);
						test.init_data.euler_angles[2]	=	(rand()%test.num_tests)	*	(2*M_PI/test.num_tests);

						//	Testing	deuler_123_quat.c:
						test.init_data.euler_sequence	=	Roll_Pitch_Yaw;
						euler_matrix(test.init_data.euler_angles,	test.mat,	0	,	test.init_data.euler_sequence);
						mat_to_quat(test.quat,	test.mat);
						test.test_euler_to_quat();
						test.test_quat_to_euler(1);
						test.test_quat_to_euler(2);			//	Tesing	singularities

						//	Testing	deuler_132_quat.c:
						test.init_data.euler_sequence	=	Roll_Yaw_Pitch;
						euler_matrix(test.init_data.euler_angles,	test.mat,	0	,	test.init_data.euler_sequence);
						mat_to_quat(test.quat,	test.mat);
						test.test_euler_to_quat();
						test.test_quat_to_euler(1);
						test.test_quat_to_euler(2);

						//	Testing	deuler_231_quat.c:
						test.init_data.euler_sequence	=	Pitch_Yaw_Roll;
						euler_matrix(test.init_data.euler_angles,	test.mat,	0	,	test.init_data.euler_sequence);
						mat_to_quat(test.quat,	test.mat);
						test.test_euler_to_quat();
						test.test_quat_to_euler(1);
						test.test_quat_to_euler(2);

						//	Testing	deuler_213_quat.c:
						test.init_data.euler_sequence	=	Pitch_Roll_Yaw;
						euler_matrix(test.init_data.euler_angles,	test.mat,	0	,	test.init_data.euler_sequence);
						mat_to_quat(test.quat,	test.mat);
						test.test_euler_to_quat();
						test.test_quat_to_euler(1);
						test.test_quat_to_euler(2);

						//	Testing	deuler_312_quat.c:
						test.init_data.euler_sequence	=	Yaw_Roll_Pitch;
						euler_matrix(test.init_data.euler_angles,	test.mat,	0	,	test.init_data.euler_sequence);
						mat_to_quat(test.quat,	test.mat);
						test.test_euler_to_quat();
						test.test_quat_to_euler(1);
						test.test_quat_to_euler(2);

						//	Testing	deuler_321_quat.c:
						test.init_data.euler_sequence	=	Yaw_Pitch_Roll;
						euler_matrix(test.init_data.euler_angles,	test.mat,	0	,	test.init_data.euler_sequence);
						mat_to_quat(test.quat,	test.mat);
						test.test_euler_to_quat();
						test.test_quat_to_euler(1);
						test.test_quat_to_euler(2);

						test.num_pass	++;
			}
			if	(test.num_pass	==	test.num_tests){
							printf("	\e[1;34m	Tested	all	%d	sets	of	Euler	angles	and	they	were	all	PASSED	\n\e",		test.num_tests);
							printf("%c[%dm",	0x1B,	0);
			}
			return(0);
}
