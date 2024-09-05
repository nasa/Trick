
#include	"trick_utils/units/include/Unit.hh"
#include	"data_products/Log/DataStream.hh"
#include	"data_products/Log/DataStreamFactory.hh"

#include	"DPC/DPC_UnitConvDataStream.hh"
#include	"DS_exercisor/DS_exercisor.hh"

#include	<iostream>
#include	<string>

using	namespace	std;

int	main(	int	argc,	char*	argv[])	{

		DataStreamFactory*	data_stream_factory;
		DataStream*	testds;
		DataStream*	srcds;
		DS_exercisor	*exercisor;

		char*	RUN_dir	=	"../TEST_DATA";
		char*	VarName	=	"sun_predictor.sun.solar_azimuth";

		data_stream_factory	=	new	DataStreamFactory();
		srcds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);

		testds	=	new	DPC_UnitConvDataStream(srcds,	"d",	"r");

		exercisor	=	new	DS_exercisor	(testds);
		exercisor->run();

}
