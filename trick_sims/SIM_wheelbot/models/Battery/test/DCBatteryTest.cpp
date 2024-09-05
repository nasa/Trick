#include	<gtest/gtest.h>
#define	private	public
#include	"DCBattery.hh"

TEST(	BatteryTest	,	one	)	{
				//	Attempt	to	create	an	Battery
				DCBattery	*	battery;
				battery	=	new	DCBattery(9.0,	2.0);
				EXPECT_NE(	(void*)0,	battery);
				//delete	battery;
}

TEST(	BatteryTest	,	two	)	{
				//	Test	Ideal	Voltage
				DCBattery	battery(9.0,	2.0);
				EXPECT_DOUBLE_EQ(	9.0,	battery.idealVoltage	);

}

TEST(	BatteryTest	,	three	)	{
				//	Test	Internal	Resistance
				DCBattery	battery(9.0,	2.0);
				EXPECT_DOUBLE_EQ(	2,	battery.internalResistance	);

}

TEST(	BatteryTest	,	four	)	{
				//	Test	Actual	Voltage	doesnt	go	below	zero
				DCBattery	battery(9.0,	2.0);
				battery.setCurrent(6.0);
				battery.update	();
				EXPECT_DOUBLE_EQ(	0.0,	battery.actualVoltage	);

}

TEST(	BatteryTest	,	five	)	{
				//	Test	Actual	Voltage	under	normal	load
				DCBattery	battery(9.0,	2.0);
				battery.setCurrent(4.0);
				battery.update	();
				EXPECT_DOUBLE_EQ(	1.0,	battery.actualVoltage	);

}

TEST(	BatteryTest	,	six	)	{
				//	Test	Actual	Voltage	doesnt	go	above	Ideal	Voltage
				DCBattery	battery(9.0,	2.0);
				battery.setCurrent(-4.0);
				battery.update	();
				EXPECT_DOUBLE_EQ(	9.0,	battery.actualVoltage	);

}
