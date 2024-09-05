
#include	"trick_utils/units/include/Unit.hh"
#include	"Log/DataStream.hh"
#include	"Log/DataStreamFactory.hh"

#include	"DS_exercisor/DS_exercisor.hh"

#include	<iostream>
#include	<string>

using	namespace	std;

int	main(	int	argc,	char*	argv[])	{

		DataStreamFactory*	data_stream_factory;
		DataStream*	testds;
		DS_exercisor	*exercisor;

		char*	RUN_dir	=	"../TEST_DATA";
		char*	VarName	=	"sun_predictor.sun.solar_azimuth";

		data_stream_factory	=	new	DataStreamFactory();
		testds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);

		exercisor	=	new	DS_exercisor	(testds);
		exercisor->run();

		return	(0);
}
