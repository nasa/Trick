#include	<gtest/gtest.h>
#include	"trick/trick_math.h"
#include	"trick/reference_frame.h"
using	namespace	std;

class	EulerQuatTest	:	public	::testing::Test	{

			public:

						TRANSFORM	init_data;
						bool	pass;
						int	num_tests;
						int	num_pass;
						int	method;
						double	quat[4];
						double	mat[3][3];

						EulerQuatTest()	{
										this->num_tests	=	100;
										this->num_pass	=	0;
										this->pass	=	false;
						}
						~EulerQuatTest()	{}
						void	SetUp()	{}
						void	TearDown()	{}

						void	compare_euler(double	eul[3]){
										double	eul_error[3];
										double	mat_test[3][3];
										double	mat_error[3][3];

										M_IDENT(mat_error);
										M_IDENT(mat_test);
										V_INIT(eul_error);
										euler_matrix(eul,	mat_test,	0	,	this->init_data.euler_sequence);
										MxMt(mat_error,	mat_test,	this->mat);
										euler_matrix(eul_error,	mat_error,	0	,	this->init_data.euler_sequence);
										if(V_MAG(eul_error)	<	0.00001){
													this->num_pass	++;
													pass	=	true;
										}	else{
													pass	=	false;
													printf("EulerAngle	=	%lf		%lf		%lf\n",this->init_data.euler_angles[0],
																																																			this->init_data.euler_angles[1],
																																																			this->init_data.euler_angles[2]);
										}
										return;
						}
						void	compare_quat(double	Q[4]){
										double	Q_error[4];
										double	mat_error[3][3];
										double	eul_error[3];

										M_IDENT(mat_error);
										V_INIT(eul_error);
										QxQt(Q_error,	Q,	quat);
										quat_to_mat(mat_error,	Q_error);
										euler_matrix(eul_error,	mat_error,	0	,	this->init_data.euler_sequence);
										if(V_MAG(eul_error)	<	0.00001){
													this->num_pass	++;
													this->pass	=	true;
										}	else{
													this->pass	=	false;
													printf("EulerAngle	=	%lf		%lf		%lf\n",this->init_data.euler_angles[0],
																																																			this->init_data.euler_angles[1],
																																																			this->init_data.euler_angles[2]);
										}
										return;
						}
						void	test_euler_to_quat(){
										double	Q[4];
										euler_quat(this->init_data.euler_angles,	Q,	0,		this->init_data.euler_sequence);
										compare_quat(	Q	);
										return;
						}
						void	test_quat_to_euler(int	test){
										double	eul[3];
										this->method	=	test;
										if(this->method	==	1){
														euler_quat(eul,	this->quat,	this->method,	this->init_data.euler_sequence);
										}
										else	{
													switch	(this->init_data.euler_sequence)	{
																	case	Roll_Pitch_Yaw:
																					euler123_quat(	eul,	this->quat,	2,	this->init_data.euler_angles	);
																					break;
																	case	Roll_Yaw_Pitch:
																					euler132_quat(	eul,	this->quat,	2,	this->init_data.euler_angles	);
																					break;
																	case	Pitch_Yaw_Roll:
																					euler231_quat(	eul,	this->quat,	2,	this->init_data.euler_angles	);
																					break;
																	case	Pitch_Roll_Yaw:
																					euler213_quat(	eul,	this->quat,	2,	this->init_data.euler_angles	);
																					break;
																	case	Yaw_Roll_Pitch:
																					euler312_quat(	eul,	this->quat,	2,	this->init_data.euler_angles	);
																					break;
																	case	Yaw_Pitch_Roll:
																					euler321_quat(	eul,	this->quat,	2,	this->init_data.euler_angles	);
																					break;
													}
										}
										compare_euler(eul);
										return;
						}
};

TEST_F(EulerQuatTest,	euler123_to_quat)
{
		init_data.euler_sequence	=	Roll_Pitch_Yaw;
		num_pass	=	0;
		srand(time(NULL));
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_euler_to_quat();
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	euler132_to_quat)
{
		init_data.euler_sequence	=	Roll_Yaw_Pitch;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_euler_to_quat();
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	euler231_to_quat)
{
		init_data.euler_sequence	=	Pitch_Yaw_Roll;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_euler_to_quat();
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	euler213_to_quat)
{
		init_data.euler_sequence	=	Pitch_Roll_Yaw;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_euler_to_quat();
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	euler312_to_quat)
{
		init_data.euler_sequence	=	Yaw_Roll_Pitch;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_euler_to_quat();
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	euler321_to_quat)
{
		init_data.euler_sequence	=	Yaw_Pitch_Roll;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_euler_to_quat();
		}
		EXPECT_EQ(num_pass,	num_tests);
}


TEST_F(EulerQuatTest,	quat_to_euler123_method1)
{
		init_data.euler_sequence	=	Roll_Pitch_Yaw;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(1);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler123_method2)
{
		init_data.euler_sequence	=	Roll_Pitch_Yaw;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(2);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler132_method1)
{
		init_data.euler_sequence	=	Roll_Yaw_Pitch;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(1);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler132_method2)
{
		init_data.euler_sequence	=	Roll_Yaw_Pitch;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(2);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler231_method1)
{
		init_data.euler_sequence	=	Pitch_Yaw_Roll;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(1);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler231_method2)
{
		init_data.euler_sequence	=	Pitch_Yaw_Roll;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(2);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler213_method1)
{
		init_data.euler_sequence	=	Pitch_Roll_Yaw;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(1);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler213_method2)
{
		init_data.euler_sequence	=	Pitch_Roll_Yaw;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(2);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler312_method1)
{
		init_data.euler_sequence	=	Yaw_Roll_Pitch;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(1);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler312_method2)
{
		init_data.euler_sequence	=	Yaw_Roll_Pitch;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(2);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler321_method1)
{
		init_data.euler_sequence	=	Yaw_Pitch_Roll;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(1);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

TEST_F(EulerQuatTest,	quat_to_euler321_method2)
{
		init_data.euler_sequence	=	Yaw_Pitch_Roll;
		num_pass	=	0;
		for	(int	ii=0;	ii<num_tests;	ii++){
						//	Randomly	input	euler	angles:
						init_data.euler_angles[0]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[1]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						init_data.euler_angles[2]	=	(rand()%num_tests)	*	(2*M_PI/num_tests);
						euler_matrix(init_data.euler_angles,	mat,	0	,	init_data.euler_sequence);
						mat_to_quat(quat,	mat);
						test_quat_to_euler(2);
		}
		EXPECT_EQ(num_pass,	num_tests);
}

