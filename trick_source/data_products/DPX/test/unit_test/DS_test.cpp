#define	protected	public

#include	<iostream>
#include	<string.h>
#include	"Log/DataStream.hh"
#include	"Log/DataStreamFactory.hh"
#include	"DPC/DPC_UnitConvDataStream.hh"
#include	"DPC/DPC_TimeCstrDataStream.hh"
#include	"DPM/DPM_time_constraints.hh"

#include	"gtest/gtest.h"
//#include	"trick_utils/reqs/include/RequirementScribe.hh"

namespace	Trick	{

int	strcmp_IgnoreWhiteSpace(const	char*	s1,	const	char*	s2)	{
				int	i1	=	0;
				int	i2	=	0;

				while	(1)	{
								while	(	!isgraph(	s1[i1]	)	&&	s1[i1]	!=	'\0')	{	i1++;	}
								while	(	!isgraph(	s2[i2]	)	&&	s2[i2]	!=	'\0')	{	i2++;	}
								if	(	s1[i1]	==	'\0'	&&	s2[i2]	==	'\0')	{	return	0;	}
								if	(	s1[i1]	!=	s2[i2])	{
												if	(s1[i1]	<	s2[i2])	{
																return	-1;
												}	else	{
																return	1;
												}
								}
								i1++;	i2++;
				}
}

class	DSTest	:	public	::	testing::Test	{
				protected:
								//Trick::RequirementScribe	req;
								std::string	testxml[14];

								DSTest()	{}
								~DSTest()	{}
								virtual	void	SetUp()	{}
								virtual	void	TearDown()	{}

								std::string	run(char	ch);

		const	char*	RUN_dir;
					const	char*	VarName;
		std::string	output;
		int	result;

		DataStreamFactory*	data_stream_factory;
		DataStream	*testds,	*srcds;

};

std::string	DSTest::run(char	ch)	{

	int	ret;
	double	time,	value;
	std::string	spec;
	std::stringstream	s;

	//	===	COMMANDS	===
	//	g	->	get
	//	p	->	peek
	//	f	->	getFileName
	//	u	->	getUnit
	//	t	->	getTimeUnit
	//	b	->	begin
	//	e	->	end
	//	s	->	step

				switch	(ch)	{
		case	'g'	:	{
			ret	=	testds->get(&time,	&value);
			s	<<	"get	:	"	<<	"time	=	"	<<	time	<<	"					"	<<	"value	=	"	<<	value;
			s	<<	"return	=	"	<<	ret;
		}	break;
		case	'p'	:	{
			ret	=	testds->peek(&time,	&value);
			//s	<<	"peek:	"	<<	"time	=	"	<<	time	<<	"					"	<<	"value	=	"	<<	value;
			if	(ret)	{
				s	<<	"peek:	"	<<	"time	=	"	<<	time	<<	"					"	<<	"value	=	"	<<	value;
			}	else	{
				s	<<	"peek:	Reached	end	of	stream.	No	value	to	peek.";
			}
			s	<<	"return	=	"	<<	ret;
		}	break;
		case	'f'	:	{
			spec	=	testds->getFileName();
			s	<<	"getFileName:	"	<<	"filename	=	"	<<	"\""	<<	spec	<<	"\"";
		}	break;
		case	'u'	:	{
			spec	=	testds->getUnit();
			s	<<	"getUnit:	"	<<	"unitspec	=	"	<<	"\""	<<	spec	<<	"\"";
		}	break;
		case	't'	:	{
			spec	=	testds->getTimeUnit();
			s	<<	"getTimeUnit:	"	<<	"timeunitspec	=	"	<<	"\""	<<	spec	<<	"\"";
		}	break;
		case	'b'	:	{
			testds->begin();
			s	<<	"begin";
		}	break;
		case	'e'	:	{
			ret	=	testds->end();
			s	<<	"end	:	return	=	"	<<	ret;
		}	break;
		case	's'	:	{
						ret	=	testds->step();
						s	<<	"step:	return	=	"	<<	ret;
					}	break;
					default	:	{
			s	<<	"Invalid	command";
		}	break;
	}
			return	s.str();
}



//	ASCII	(CSV)	DATASTREAM
TEST_F(DSTest,	DataStream_Ascii)	{
			//req.add_requirement("32631267");

	RUN_dir	=	"../TEST_DATA/RUN_ASCII";
				VarName	=	"sun_predictor.sun.solar_azimuth";

	data_stream_factory	=	new	DataStreamFactory();
				testds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);

	//	GET	FILE	NAME
	output	=	run('f');
				result	=	strcmp_IgnoreWhiteSpace(
								"getFileName	:	filename	=	\"../TEST_DATA/RUN_ASCII/log_helios.csv\"",	output.c_str());
				EXPECT_EQ(result,	0);

	//	GET	UNIT
				output	=	run('u');
				result	=	strcmp_IgnoreWhiteSpace("getUnit	:	unitspec	=	\"degree\"",	output.c_str());
				EXPECT_EQ(result,	0);

	//	GET	TIME	UNIT
				output	=	run('t');
				result	=	strcmp_IgnoreWhiteSpace("getTimeUnit	:	timeunitspec	=	\"s\"",	output.c_str());
				EXPECT_EQ(result,	0);

	//	GET
				output	=	run('g');
				result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	351.283					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	1					value	=	351.289					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	//	PEEK
	output	=	run('p');
				result	=	strcmp_IgnoreWhiteSpace(
		"peek	:	time	=	2					value=	351.295					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	output	=	run('p');
				result	=	strcmp_IgnoreWhiteSpace(
		"peek	:	time	=	2					value=	351.295					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	//	BEGIN
	output	=	run('b');
	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	351.283				return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	//	STEP
	output	=	run('s');
	output	=	run('s');
	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	3					value=	351.301				return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	//	END
	output	=	run('e');
	result	=	strcmp_IgnoreWhiteSpace("end	:	return	=	0",	output.c_str());
	EXPECT_EQ(result,	0);

	delete	data_stream_factory;
}

//	TRICK	BINARY	DATASTREAM
TEST_F(DSTest,	DataStream_Binary)	{
	//req.add_requirement("3201880761");

	RUN_dir	=	"../TEST_DATA/RUN_BINARY";
				VarName	=	"sun_predictor.sun.solar_elevation";

				data_stream_factory	=	new	DataStreamFactory();
				testds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);

				//	GET	FILE	NAME
				output	=	run('f');
				result	=	strcmp_IgnoreWhiteSpace(
					"getFileName	:	filename	=	\"../TEST_DATA/RUN_BINARY/log_helios.trk\"",	output.c_str());
				EXPECT_EQ(result,	0);

	//	GET	UNIT
				output	=	run('u');
				result	=	strcmp_IgnoreWhiteSpace("getUnit	:	unitspec	=	\"degree\"",	output.c_str());
				EXPECT_EQ(result,	0);

	//	GET	TIME	UNIT
				output	=	run('t');
				result	=	strcmp_IgnoreWhiteSpace("getTimeUnit	:	timeunitspec	=	\"s\"",	output.c_str());
				EXPECT_EQ(result,	0);

	//	GET
				output	=	run('g');
				result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	-36.7426					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	output	=	run('g');
				result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	1					value	=	-36.743					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	//	PEEK
				output	=	run('p');
				result	=	strcmp_IgnoreWhiteSpace(
		"peek	:	time	=	2					value=	-36.7434					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	output	=	run('p');
				result	=	strcmp_IgnoreWhiteSpace(
		"peek	:	time	=	2					value=	-36.7434					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	//	BEGIN
				output	=	run('b');
				output	=	run('g');
				result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	-36.7426						return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	//	STEP
				output	=	run('s');
				output	=	run('s');
				output	=	run('g');
				result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	3					value=	-36.7438					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	//	END
	output	=	run('e');
	result	=	strcmp_IgnoreWhiteSpace("end	:	return	=	0",	output.c_str());
	EXPECT_EQ(result,	0);

	delete	data_stream_factory;
}

//	MATLAB	DATASTREAM
TEST_F(DSTest,	DataStream_MatLab)	{
	//req.add_requirement("2533684432	1366633954");

				RUN_dir	=	"../TEST_DATA/RUN_MATLAB";
				VarName	=	"ch_joint_angle_5";

				data_stream_factory	=	new	DataStreamFactory();
				testds	=	data_stream_factory->create(RUN_dir,	VarName,	"s_simtime");

	//	GET	FILE	NAME
	output	=	run('f');
	result	=	strcmp_IgnoreWhiteSpace(
		"getFileName	:	filename=	\"../TEST_DATA/RUN_MATLAB/nasa_p1.mat\"",	output.c_str());
	EXPECT_EQ(result,	0);

	//	GET	UNIT
	output	=	run('u');
	result	=	strcmp_IgnoreWhiteSpace("getUnit	:	unitspec=	\"1\"",	output.c_str());
	EXPECT_EQ(result,	0);

	//	GET	TIME	UNIT
	output	=	run('t');
	result	=	strcmp_IgnoreWhiteSpace("getTimeUnit	:	timeunitspec	=	\"s\"",	output.c_str());
	EXPECT_EQ(result,	0);

	//	GET
	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	-210.146					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0.05					value	=	-210.146					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	//	PEEK
	output	=	run('p');
	result	=	strcmp_IgnoreWhiteSpace(
		"peek	:	time	=	0.1					value=	-210.146					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	output	=	run('p');
	result	=	strcmp_IgnoreWhiteSpace(
		"peek	:	time	=	0.1					value=	-210.146					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	//	BEGIN
	output	=	run('b');
	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	-210.146					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	//	STEP
	output	=	run('s');
	output	=	run('s');
	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0.15					value=	-210.146					return	=	1",	output.c_str());
	EXPECT_EQ(result,	0);

	//	END
	output	=	run('e');
	result	=	strcmp_IgnoreWhiteSpace("end	:	return	=	0",	output.c_str());
	EXPECT_EQ(result,	0);

	delete	data_stream_factory;
}

//	DELTA	DATASTREAM
//	The	delta	statement	is	formatted	as	follows:
//			delta(<var1:run1>,	<var2:run2>)
TEST_F(DSTest,	DataStream_Delta)	{
	//req.add_requirement("2904854297");

	RUN_dir	=	NULL;
	char	DeltaName[1000];

	const	char*	a	=	"sun_predictor.sun.solar_azimuth:../TEST_DATA/BUNCHORUNS/RUN1";
	const	char*	b	=	"sun_predictor.sun.solar_azimuth:../TEST_DATA/BUNCHORUNS/RUN2";

	snprintf(DeltaName,	sizeof(DeltaName),	"delta(%s,	%s)",	a,	b);

				data_stream_factory	=	new	DataStreamFactory();
				testds	=	data_stream_factory->create(RUN_dir,	DeltaName,	"s_simtime");

	delete	data_stream_factory;
}

//	DPC	UNIT	CONVERSION	DATASTREAM
TEST_F(DSTest,	DataStream_DPCUnitConv)	{
	//req.add_requirement("2269871888");

	RUN_dir	=	"../TEST_DATA/RUN_BINARY";
	VarName	=	"sun_predictor.sun.right_ascension";

	data_stream_factory	=	new	DataStreamFactory();
				srcds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);
	testds	=	new	DPC_UnitConvDataStream(srcds,	"rad",	"degree");

	output	=	run('g');
				result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	0					value	=	1.56605					return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	output	=	run('u');
	result	=	strcmp_IgnoreWhiteSpace("getUnit	:	unitspec	=	\"rad\"",	output.c_str());
	EXPECT_EQ(result,	0);

	output	=	run('t');
				result	=	strcmp_IgnoreWhiteSpace("getTimeUnit	:	timeunitspec	=	\"s\"",	output.c_str());
				EXPECT_EQ(result,	0);

	delete	data_stream_factory;
	delete	testds;
}

//	TIME	CONSTRAINT	DATASTREAM
TEST_F(DSTest,	DataStream_DPCTimeCstr)	{
	//req.add_requirement("3610816325");

	int	i;
	DPM_time_constraints	*time_constraints;

	RUN_dir	=	"../TEST_DATA/RUN_BINARY";
	VarName	=	"sun_predictor.sun.utc.sec";

	data_stream_factory	=	new	DataStreamFactory();
				srcds	=	data_stream_factory->create(RUN_dir,	VarName,	NULL);
				time_constraints	=	new	DPM_time_constraints(0.0,	5.0,	50.0);

	testds	=	new	DPC_TimeCstrDataStream(srcds,	time_constraints);

	for	(i	=	0;	i	<	6;	i++)	{
		output	=	run('s');
		result	=	strcmp_IgnoreWhiteSpace("step:	return	=	1",	output.c_str());
					EXPECT_EQ(result,	0);
	}
	//	REACHED	END	OF	STREAM
	output	=	run('s');
	result	=	strcmp_IgnoreWhiteSpace("step:	return	=	0",	output.c_str());
	EXPECT_EQ(result,	0);

	output	=	run('g');
	result	=	strcmp_IgnoreWhiteSpace(
		"get	:	time	=	5					value	=	5					return	=	0",	output.c_str());
	EXPECT_EQ(result,	0);

	output	=	run('p');
	result	=	strcmp_IgnoreWhiteSpace(
		"peek:	Reached	end	of	stream.	No	value	to	peek.					return	=	0",	output.c_str());
				EXPECT_EQ(result,	0);

	output	=	run('e');
				result	=	strcmp_IgnoreWhiteSpace("end	:	return	=	1",	output.c_str());
				EXPECT_EQ(result,	0);

	delete	data_stream_factory;
	delete	testds;
}

}

