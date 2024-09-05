#include	<gtest/gtest.h>
#define	private	public
#include	"DCMotor.hh"

TEST(	MotorModelTest	,	one	)	{
				//	Attempt	to	create	a	Motor
			DCMotor	*	motor;
			motor	=	new	DCMotor(8.0,	7.0);
			EXPECT_NE(	(void*)0,	motor);
			//delete	motor;
}

TEST(	MotorModelTest	,	two	)	{
				//	Test	Motor	Torque
				DCMotor	motor(8.0,	7.0);
				EXPECT_DOUBLE_EQ(	0.0,	motor.motorTorque	);

}

TEST(	MotorModelTest	,	three	)	{
				//	Test	Motor	Current
				DCMotor	motor(8.0,	7.0);
				EXPECT_DOUBLE_EQ(	0.0,	motor.motorCurrent	);

}

TEST(	MotorModelTest	,	four	)	{
				//	Test	Battery	Current
				DCMotor	motor(8.0,	7.0);
				EXPECT_DOUBLE_EQ(	0.0,	motor.currentLoad	);

}

TEST(	MotorModelTest	,	five	)	{
				//	Test	Internal	Resistance
				DCMotor	motor(8.0,	7.0);
				EXPECT_DOUBLE_EQ(	8.0,	motor.internalResistance	);

}

TEST(	MotorModelTest	,	six	)	{
				//	Test	Motor	Torque	Constant
				DCMotor	motor(8.0,	7.0);
				EXPECT_DOUBLE_EQ(	7.0,	motor.motorTorqueConstant	);

}

TEST(	MotorModelTest	,	seven	)	{
				//	Test	Caculation	for	Motor	Torque
				DCMotor	motor(8.0,	7.0);
				double	motorVoltage	=	(16.0);
				motor.update	(motorVoltage	);
				EXPECT_DOUBLE_EQ(	14.0,	motor.motorTorque	);

}

TEST(	MotorModelTest	,	eight	)	{
				//	Test	Caculation	for	Motor	Current
				DCMotor	motor(8.0,	7.0);
				double	motorVoltage	=	(16.0);
				motor.update	(motorVoltage	);
				EXPECT_DOUBLE_EQ(	2.0,	motor.motorCurrent	);

}

TEST(	MotorModelTest	,	nine	)	{
				//	Test	Caculation	for	Battery	Current
				DCMotor	motor(8.0,	7.0);
				double	motorVoltage	=	(24.0);
				motor.update	(motorVoltage	);
				EXPECT_DOUBLE_EQ(	3.0,	motor.currentLoad	);

}


TEST(	MotorModelTest	,	ten	)	{
				//	Test	Caculation	for	Battery	Current	that	returns	an	Absolute	Value
				DCMotor	motor(8.0,	7.0);
				double	motorVoltage	=	(-24.0);
				motor.update	(motorVoltage	);
				EXPECT_DOUBLE_EQ(	3.0,	motor.currentLoad	);

}
