
#include	"trick_utils/units/include/Unit.hh"
#include	"data_products/Log/DataStream.hh"
#include	"data_products/Log/DataStreamFactory.hh"

#include	"DPC/DPC_TimeCstrDataStream.hh"
#include	"DPM/DPM_time_constraints.hh"
#include	"DS_exercisor/DS_exercisor.hh"

#include	<iostream>
#include	<string>

using	namespace	std;

int	main(	int	argc,	char*	argv[])	{

		DPM_time_constraints	*time_constraints;
		DataStreamFactory*	data_stream_factory;
		DataStream*	testds;
		DataStream*	srcds;
		DS_exercisor	*exercisor;

		char*	RUN_dir	=	"../TEST_DATA";
		char*	VarName	=	"sun_predictor.sun.solar_azimuth";

		data_stream_factory	=	new	DataStreamFactory();
		srcds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);

		time_constraints	=	new	DPM_time_constraints(0.0,					//	Start	time	in	seconds.
																																														5.0,					//	Stop	time	in	seconds
																																														50.0);			//	Max	Frequency	in	Hertz

		testds	=	new	DPC_TimeCstrDataStream(srcds,	time_constraints);

		exercisor	=	new	DS_exercisor	(testds);
		exercisor->run();

		return	(0);
}
