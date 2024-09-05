#define	protected	public

#include	<stdio.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>
#include	<string.h>
#include	"DPC/DPC_product.hh"
#include	"DPM/DPM_parse_tree.hh"
#include	"DPM/DPM_session.hh"
#include	"test_view.hh"

#include	"gtest/gtest.h"
//#include	"trick_utils/reqs/include/RequirementScribe.hh"

namespace	Trick	{

class	DPCTest	:	public	::testing::Test	{
				protected:
								//Trick::RequirementScribe	req;
								std::string	testxml[15];

								DPCTest()	{}
								~DPCTest()	{}
								virtual	void	SetUp();
								virtual	void	TearDown();

		std::string	parseDPCData(const	char*	name);
};

void	DPCTest::SetUp()	{
	testxml[0]		=	"../TEST_DATA/session_1_1.xml";
	testxml[1]		=	"../TEST_DATA/session_1_2.xml";
				testxml[2]		=	"../TEST_DATA/session_2_1.xml";
				testxml[3]		=	"../TEST_DATA/session_3_1.xml";
				testxml[4]		=	"../TEST_DATA/session_4_1.xml";
				testxml[5]		=	"../TEST_DATA/session_5_1.xml";
				testxml[6]		=	"../TEST_DATA/session_6_1.xml";
				testxml[7]		=	"../TEST_DATA/session_7_1.xml";
				testxml[8]		=	"../TEST_DATA/session_8_1.xml";
	testxml[9]		=	"../TEST_DATA/session_8_2.xml";
				testxml[10]	=	"../TEST_DATA/session_9_1.xml";
				testxml[11]	=	"../TEST_DATA/session_9_2.xml";
				testxml[12]	=	"../TEST_DATA/session_9_3.xml";
				testxml[13]	=	"../TEST_DATA/session_testview.xml";
	testxml[14]	=	"../TEST_DATA/session_invDTD.xml";
}

void	DPCTest::TearDown()	{}

int	strcmp_IgnoringWhiteSpace(const	char*	s1,	const	char*	s2)	{
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

std::string	DPCTest::parseDPCData(const	char*	name)	{
				DPM_parse_tree	*session_parse_tree;
				xmlNode	*root_node;
				DPM_session	*session;
				const	char	*session_file_name;

				vector	<DPC_product*>	product_list;

				Test_view	*view;

				int	n_product_files;
				int	product_ix;
	std::stringstream	output;

	session_file_name	=	name;

				//std::cout	<<	"START	CTRL	TEST"	<<	std::endl;
				//std::cout	<<	"NEW	TEST	VIEW"	<<	std::endl;
				view	=	new	Test_view();

				try	{
								session_parse_tree	=	new	DPM_parse_tree(session_file_name);
				}	catch	(std::invalid_argument)	{
								std::cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	std::endl;
								exit(1);
				}

				root_node	=	session_parse_tree->getRootNode();

				try	{
								session	=	new	DPM_session(	NULL,	root_node	);
				}	catch	(std::invalid_argument)	{
								std::cerr	<<	"ERROR:	<session>	specification	is	invalid	and	un-usable.	Quitting."	<<	std::endl;
								exit(1);
				}

				delete	session_parse_tree;

				n_product_files	=	session->product_file_list.size();

				for	(	product_ix	=	0;	product_ix	<	n_product_files;	product_ix++	)	{
								DPC_product	*product;
								char	tmp	[50];
								snprintf(tmp,	sizeof(tmp),	"../TEST_DATA/%s",	session->product_file_list[product_ix]);
								const	char	*product_file_name	=	tmp;

								try	{
												//std::cout	<<	"NEW	DPC_PRODUCT"	<<	std::endl;
												product	=	new	DPC_product(	session,	product_file_name	);
								}	catch		(	std::invalid_argument	)	{
												std::cerr	<<	"ERROR:	Unable	to	create	the	product	specified	by	file:	\""	<<
																product_file_name	<<	"\""	<<	std::endl;
												product	=	NULL;
								}

								if	(product	!=	NULL)	{
												product_list.push_back(	product	);

												//std::cout	<<	"RENDER	VIEW"	<<	std::endl;
												product->render(	view	);
			output	<<	view->getOutput();
								}
				}
				return	output.str();
}

TEST_F(DPCTest,	ValidateDTD)	{
				unsigned	int	i;
				xmlParserCtxtPtr	parser_context;
				xmlDocPtr	doc;

				LIBXML_TEST_VERSION
				if	((parser_context	=	xmlNewParserCtxt())	==	NULL)	{
								fprintf(stderr,"Out	of	Memory\n");
								exit(1);
				}

				for	(i=0	;	i<(sizeof(testxml)/sizeof(testxml[0]))	;	i++)	{
								doc	=	xmlCtxtReadFile(parser_context,	testxml[i].c_str(),	NULL,	XML_PARSE_DTDVALID+XML_PARSE_NOERROR);

								if	(i	<	14)	{
												ASSERT_TRUE(parser_context->valid);
								}	else	{
												ASSERT_FALSE(parser_context->valid);
								}
								xmlFreeDoc(doc);
				}
				xmlFreeParserCtxt(parser_context);
}

//	Session	1_1
TEST_F(DPCTest,	VarvsTime_Binary)	{
	//req.add_requirement("2416241399	3201880761");

	std::string	tmp	=	parseDPCData(testxml[0].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"
		"PAGE	VIEW:		Attribute	presentation:	simple"

		"PLOT	VIEW:"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination	Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"First	5	points:"
		"<0,23.4391>[0]	<1,23.4391>[1]	<2,23.4391>[2]"
		"<3,23.4391>[3]	<4,23.4391>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination	Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"First	5	points:"
		"<0,23.4391>[0]	<1,23.4391>[1]	<2,23.4391>[2]"
		"<3,23.4391>[3]	<4,23.4391>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	1_2
TEST_F(DPCTest,	VarvsTime_CSV)	{
	//req.add_requirement("2416241399	32631267");

				std::string	tmp	=	parseDPCData(testxml[1].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:		Attribute	version:	1.0"
		"PAGE	VIEW:"
		"Attribute	presentation:	stream"

		"PLOT	VIEW:"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination	Y	Units:	degree"
		"RUN	Directory:	RUN_ASCII"
		"First	5	points:"
		"<0,14.0809>[0]	<1,14.0809>[1]	<2,14.0809>[2]"
		"<3,14.0809>[3]	<4,14.0809>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination	Y	Units:	degree"
		"RUN	Directory:	RUN_ASCII"
		"First	5	points:"
		"<0,14.0809>[0]	<1,14.0809>[1]	<2,14.0809>[2]"
		"<3,14.0809>[3]	<4,14.0809>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	2_1
TEST_F(DPCTest,	SinglePlot)	{
				std::string	tmp	=	parseDPCData(testxml[2].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"
		"PAGE	VIEW:		Attribute	presentation:	SIMPLE"

		"PLOT	VIEW:"
		"Number	of	Curves:	1"
		"X-axis	label:	solar_azimuth	Y-axis	label:	solar_elevation"
		"CURVE	VIEW:"
		"X	Var	Name:	sun_predictor.sun.solar_azimuth	X	Units:	degree"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation	Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"First	5	points:"
		"<353.673,-36.7426>[0]	<353.678,-36.743>[1]	<353.683,-36.7434>[2]"
		"<353.688,-36.7438>[3]	<353.692,-36.7442>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}


//	Session	3_1
TEST_F(DPCTest,	SingleTable)	{
	//req.add_requirement("3667219994");

				std::string	tmp	=	parseDPCData(testxml[3].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"

		"TABLE	VIEW:"
		"RUN	Directory:	../TEST_DATA/RUN_BINARY"
		"Number	of	Columns:	3"
		"Column	Label	[0]:	time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s"
		"Column	Label	[1]:	solar_azimuth"
		"Variable	Name[1]:	sun_predictor.sun.solar_azimuth"
		"Column	Units	[1]:	degree"
		"Column	Label	[2]:	solar_elevation"
		"Variable	Name[2]:	sun_predictor.sun.solar_elevation"
		"Column	Units	[2]:	degree"
		"time:	0	values:		0	353.673	-36.7426"
		"time:	1	values:		1	353.678	-36.743"
		"time:	2	values:		2	353.683	-36.7434"
		"time:	3	values:		3	353.688	-36.7438"
		"time:	4	values:		4	353.692	-36.7442"
		"FINALIZE	TABLE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	4_1
TEST_F(DPCTest,	PlotAttr_Binary)	{
				std::string	tmp	=	parseDPCData(testxml[4].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"
		"Product	title:	Solar	Elevation"
		"PAGE	VIEW:		Page	title:	Solar	Elevation	for	a	Day"
		"Attribute	presentation:	SIMPLE"

		"PLOT	VIEW:		Plot	title:	Sun	Elevation	vs.	Time"
		"Number	of	Curves:	1"
		"X-axis	label:	Time		Y-axis	label:	Elevation"
		"Attribute	grid:				yes"
		"Attribute	grid_color:				blue"
		"Attribute	background_color:	#eef0ff"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time					X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation	Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_style:	dash"
		"Attribute	y	line_color:	red"
		"First	5	points:"
		"<0,-36.7426>[0]	<1,-36.743>	[1]	<2,-36.7434>[2]"
		"<3,-36.7438>[3]	<4,-36.7442>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	5_1
/*	This	session	is	based	on	product_5.xml	which	has	3	dimensional	plotting	schematic.
	*	DPV	currently	only	supports	2D	plots.
	*/
TEST_F(DPCTest,	3DProduct_2DPlot)	{
	//req.add_requirement("2469402355");

				std::string	tmp	=	parseDPCData(testxml[5].c_str());
	//std::cout	<<	tmp	<<	std::endl;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"
		"Product	title:					title	of	the	product"
		"Attribute	foreground_color:	#000000"
		"Attribute	background_color:	#ffffff"
		"PAGE	VIEW:		Page	title:		Solar	Elevation	for	a	Day"
		"Attribute	foreground_color:	#000000"
		"Attribute	background_color:	#ffffff"
		"Attribute	vcells:					#000000"
		"Attribute	presentation:			SIMPLE"

		"PLOT	VIEW:		Plot	title:	Sun	Time	vs.	Elevation	vs.	Azimuth"
		"Number	of	Curves:	1"
		"X-axis	label:	Azimuth		Y-axis	label:	Elevation"
		"Attribute	grid:					yes"
		"Attribute	foreground_color:	#000000"
		"Attribute	background_color:	#fffff0"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sun_predictor.sun.solar_azimuth	X	Units:	degree"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation	Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_color:			#323200"
		"First	5	points:"
		"<353.673,-36.7426>[0]	<353.678,-36.743>[1]	<353.683,-36.7434>[2]"
		"<353.688,-36.7438>[3]	<353.692,-36.7442>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}


//	Session	6_1
TEST_F(DPCTest,	Table_Attributes)	{
				std::string	tmp	=	parseDPCData(testxml[6].c_str());
	//std::cout	<<	tmp	<<	std::endl;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"
		"Product	title:	Table	of	Sun	Positions"

		"TABLE	VIEW:"
		"RUN	Directory:	../TEST_DATA/RUN_BINARY"
		"Title:	Sun	Azimuth	vs.	Time"
		"Number	of	Columns:	5"
		"Column	Label	[0]:	Time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s	format	[0]:	%24.10f"
		"Column	Label	[1]:	Azimuth"
		"Variable	Name[1]:	sun_predictor.sun.solar_azimuth"
		"Column	Units	[1]:	degree	format	[1]:	%16.5q"
		"Column	Label	[2]:	Hours"
		"Variable	Name[2]:	sys.exec.out.time"
		"Column	Units	[2]:	s	format	[2]:	%24.10f"
								"Column	Label	[3]:	Hours"
								"Variable	Name[3]:	sys.exec.out.time"
								"Column	Units	[3]:	hr				format	[3]:	%24.10f"
		"Column	Label	[4]:	Elevation"
		"Variable	Name[4]:	sun_predictor.sun.solar_elevation"
		"Column	Units	[4]:	degree	format	[4]:	%16.5f"
		"time:	400	values:		400	355.576	400	0.111111	-36.8784"
		"time:	401	values:		401	355.58	401	0.111389	-36.8787"
		"time:	402	values:		402	355.585	402	0.111667	-36.879"
		"time:	403	values:		403	355.59	403	0.111944	-36.8792"
		"time:	404	values:		404	355.595	404	0.112222	-36.8795"
		"FINALIZE	TABLE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}


//	Session	7_1
TEST_F(DPCTest,	MultiplePlots)	{
	//req.add_requirement("445237094");

				std::string	tmp	=	parseDPCData(testxml[7].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:		Attribute	version:	1.0"
		"Product	title:	title	of	the	product"
		"PAGE	VIEW:		Page	title:	Solar	Elevation	for	a	Day"
		"Attribute	presentation:	SIMPLE"

		"PLOT	VIEW:		Plot	title:	Sun	Elevation	vs.	Time"
		"Number	of	Curves:	1"
		"X-axis	label:	Time			Y-axis	label:	Elevation"
		"Attribute	grid:					yes"
		"Attribute	background_color:	#fff0ff"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time						X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation		Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_color:			#323200"
		"First	5	points:"
		"<0,-36.7426>[0]	<1,-36.743>	[1]	<2,-36.7434>[2]"
		"<3,-36.7438>[3]	<4,-36.7442>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:	Plot	title:	Sun	Azimuth	vs.	Time"
		"Number	of	Curves:	1"
		"X-axis	label:	Time			Y-axis	label:	Azimuth"
		"Attribute	grid:				yes"
		"Attribute	background_color:	#f0fff0"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time						X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.solar_azimuth		Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_color:			#323200"
		"First	5	points:"
		"<0,353.673>[0]	<1,353.678>[1]	<2,353.683>[2]"
		"<3,353.688>[3]	<4,353.692>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:	Plot	title:	Sun	Elevation	vs.	Azimuth"
		"Number	of	Curves:	1"
		"X-axis	label:	Azimuth		Y-axis	label:	Elevation"
		"Attribute	grid:					yes"
		"Attribute	background_color:	#fffff0"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sun_predictor.sun.solar_azimuth		X	Units:	degree"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation		Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_color:			#323200"
		"First	5	points:"
		"<353.673,-36.7426>[0]	<353.678,-36.743>	[1]	<353.683,-36.7434>[2]"
		"<353.688,-36.7438>[3]	<353.692,-36.7442>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	8_1
TEST_F(DPCTest,	MultPlots_MultTables)	{
	//req.add_requirement("150267139	32631267");

				std::string	tmp	=	parseDPCData(testxml[8].c_str());
	//std::cout	<<	tmp	<<	std::endl;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:						Attribute	version:	1.0"
		"Attribute	background_color:		#885588"
		"PAGE	VIEW:	Page	title:			Solar	Elevation	for	a	Day"
		"Attribute	background_color:		#885588"
		"Attribute	presentation:			SIMPLE"

		"PLOT	VIEW:		Plot	title:	Sun	Elevation	vs.	Time"
		"Number	of	Curves:	1"
		"X-axis	label:	Time		Y-axis	label:	Elevation"
		"Attribute	grid:					yes"
		"Attribute	background_color:		#885588"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time						X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation		Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_color:			#323200"
		"First	5	points:"
		"<500,-36.9048>[0]	<501,-36.9051>[1]	<502,-36.9053>[2]"
		"<503,-36.9056>[3]	<504,-36.9058>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW."

		"PAGE	VIEW:		Attribute	background_color:	#885588"
		"Attribute	presentation:			SIMPLE"

		"PLOT	VIEW:		Plot	title:	Sun	Elevation	vs.	Azimuth"
		"Number	of	Curves:	1"
		"X-axis	label:	Azimuth			Y-axis	label:	Elevation"
		"Attribute	grid:					yes"
		"Attribute	background_color:		#fffff0"
		"Attribute	font:					-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*"
		"CURVE	VIEW:"
		"X	Var	Name:	sun_predictor.sun.solar_azimuth		X	Units:	degree"
		"Y	Var	Name:	sun_predictor.sun.solar_elevation		Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	y	line_color:			#323200"
		"First	5	points:"
		"<356.052,-36.9048>[0]	<356.057,-36.9051>[1]	<356.062,-36.9053>[2]"
		"<356.067,-36.9056>[3]	<356.071,-36.9058>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW."

		"TABLE	VIEW:		RUN	Directory:	../TEST_DATA/RUN_BINARY"
		"Title:	My	Two	Column	Table"
		"Number	of	Columns:	3"
		"Column	Label	[0]:	Time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s		format[0]:	%24.10f"
		"Column	Label	[1]:	Azimuth"
		"Variable	Name[1]:	sun_predictor.sun.solar_azimuth"
		"Column	Units	[1]:	degree	format[1]:	%16.5q"
		"Column	Label	[2]:	Elevation"
		"Variable	Name[2]:	sun_predictor.sun.solar_elevation"
		"Column	Units	[2]:	degree	format[2]:	%16.5f"
		"time:	500	values:		500	356.052	-36.9048"
		"time:	501	values:		501	356.057	-36.9051"
		"time:	502	values:		502	356.062	-36.9053"
		"time:	503	values:		503	356.067	-36.9056"
		"time:	504	values:		504	356.071	-36.9058"
		"FINALIZE	TABLE	VIEW."

		"TABLE	VIEW:		RUN	Directory:	../TEST_DATA/RUN_BINARY"
		"Title:	Four	Column	Table"
		"Number	of	Columns:	5"
		"Column	Label	[0]:	time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s		format[0]:	%24.10f"
		"Column	Label	[1]:	Seconds"
		"Variable	Name[1]:	sun_predictor.sun.local_time.sec"
		"Column	Units	[1]:	s	format[1]:	%16.5f"
		"Column	Label	[2]:	Day"
		"Variable	Name[2]:	sun_predictor.sun.local_time.day"
		"Column	Units	[2]:	day	format[2]:	%16.5f"
		"Column	Label	[3]:	Month"
		"Variable	Name[3]:	sun_predictor.sun.local_time.month"
		"Column	Units	[3]:	--	format[3]:	%16.5f"
		"Column	Label	[4]:	Year"
		"Variable	Name[4]:	sun_predictor.sun.local_time.year"
		"Column	Units	[4]:	--	format[4]:	%16.5f"
		"time:	500	values:		500	20	21	6	2006"
		"time:	501	values:		501	21	21	6	2006"
		"time:	502	values:		502	22	21	6	2006"
		"time:	503	values:		503	23	21	6	2006"
		"time:	504	values:		504	24	21	6	2006"
		"FINALIZE	TABLE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}


//	Session_8_2

TEST_F(DPCTest,	MultTables)	{
	//req.add_requirement("2064022342	3201880761");

	std::string	tmp	=	parseDPCData(testxml[9].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:		Attribute	version:	1.0"
		"Attribute	background_color:	#885588"

		"TABLE	VIEW:"
		"RUN	Directory:	../TEST_DATA/RUN_ASCII"
		"Title:	My	Two	Column	Table"
		"Number	of	Columns:	3"
		"Column	Label	[0]:	Time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s		format	[0]:	%24.10f"
		"Column	Label	[1]:	Azimuth"
		"Variable	Name[1]:	sun_predictor.sun.solar_azimuth"
		"Column	Units	[1]:	degree	format	[1]:	%16.5q"
		"Column	Label	[2]:	Elevation"
		"Variable	Name[2]:	sun_predictor.sun.solar_elevation"
		"Column	Units	[2]:	degree	format	[2]:	%16.5f"
		"time:	500	values:		500	354.177	-46.1843"
		"time:	501	values:		501	354.183	-46.1846"
		"time:	502	values:		502	354.189	-46.185"
		"time:	503	values:		503	354.195	-46.1854"
		"time:	504	values:		504	354.201	-46.1857"
		"FINALIZE	TABLE	VIEW."

		"TABLE	VIEW:"
		"RUN	Directory:	../TEST_DATA/RUN_ASCII"
		"Title:	Four	Column	Table"
		"Number	of	Columns:	5"
		"Column	Label	[0]:	time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s	format	[0]:	%24.10f"
		"Column	Label	[1]:	Seconds"
		"Variable	Name[1]:	sun_predictor.sun.local_time.sec"
		"Column	Units	[1]:	s	format	[1]:	%16.5f"
		"Column	Label	[2]:	Day"
		"Variable	Name[2]:	sun_predictor.sun.local_time.day"
		"Column	Units	[2]:	day	format	[2]:	%16.5f"
		"Column	Label	[3]:	Month"
		"Variable	Name[3]:	sun_predictor.sun.local_time.month"
		"Column	Units	[3]:	--	format	[3]:	%16.5f"
		"Column	Label	[4]:	Year"
		"Variable	Name[4]:	sun_predictor.sun.local_time.year"
		"Column	Units	[4]:	--	format	[4]:	%16.5f"
		"time:	500	values:		500	20	15	8	2006"
		"time:	501	values:		501	21	15	8	2006"
		"time:	502	values:		502	22	15	8	2006"
		"time:	503	values:		503	23	15	8	2006"
		"time:	504	values:		504	24	15	8	2006"
		"FINALIZE	TABLE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	9_1
TEST_F(DPCTest,	ComparisonCurves)	{
	//req.add_requirement("665188150");

				std::string	tmp	=	parseDPCData(testxml[10].c_str());
				//std::cout	<<	tmp;
				int	result	=	strcmp_IgnoringWhiteSpace(
					"PRODUCT	VIEW:		Attribute	version:	1.0"
		"PAGE	VIEW:	Page	title:	Page	of	Squiggles"
		"Attribute	presentation:	comparison"

		"PLOT	VIEW:		Plot	title:	Squiggle	1"
		"Number	of	Curves:	2"
		"X-axis	label:	time		Y-axis	label:	JD"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time			X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.JD		Y	Units:	day"
		"RUN	Directory:	RUN5"
		"First	5	points:"
		"<0,2.45661e+06>[0]	<1,2.45661e+06>[1]	<2,2.45661e+06>[2]"
		"<3,2.45661e+06>[3]	<4,2.45661e+06>[4]"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time			X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.JD		Y	Units:	day"
		"RUN	Directory:	RUN6"
		"First	5	points:"
		"<0,2.45661e+06>[0]	<1,2.45661e+06>[1]	<2,2.45661e+06>[2]"
		"<3,2.45661e+06>[3]	<4,2.45661e+06>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Squiggle	2"
		"Number	of	Curves:	2"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time					X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination		Y	Units:	degree"
		"RUN	Directory:	RUN5"
		"First	5	points:"
		"<0,-18.1012>[0]	<1,-18.1012>[1]	<2,-18.1012>[2]"
		"<3,-18.1012>[3]	<4,-18.1012>[4]"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time					X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination		Y	Units:	degree"
		"RUN	Directory:	RUN6"
		"First	5	points:"
		"<0,-18.1668>[0]	<1,-18.1668>[1]	<2,-18.1668>[2]"
		"<3,-18.1668>[3]	<4,-18.1668>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Squiggle	3"
		"Number	of	Curves:	2"
		"X-axis	label:	time		Y-axis	label:	hour_angle"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time					X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.hour_angle		Y	Units:	degree"
		"RUN	Directory:	RUN5"
		"First	5	points:"
		"<0,-61.18>[0]			<1,-61.1758>[1]	<2,-61.1717>[2]"
		"<3,-61.1675>[3]	<4,-61.1633>[4]"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time					X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.hour_angle		Y	Units:	degree"
		"RUN	Directory:	RUN6"
		"First	5	points:"
		"<0,28.8108>[0]	<1,28.8149>[1]	<2,28.8191>[2]"
		"<3,28.8233>[3]	<4,28.8274>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
				EXPECT_EQ(result,	0);
}

//	Session	9_2
TEST_F(DPCTest,	DeltaCurves)	{
	//req.add_requirement("2904854297");

				std::string	tmp	=	parseDPCData(testxml[11].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:		Attribute	version:	1.0"
		"PAGE	VIEW:		Page	title:	Page	of	Squiggles"
		"Attribute	presentation:	DELTA"

		"PLOT	VIEW:		Plot	title:	Squiggle	1"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	JD"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time					X	Units:	s"
		"Y	Var	Name:	DELTA(sun_predictor.sun.JD)		Y	Units:	day"
		"RUN	Directory:	RUN1	-	RUN2"
		"First	5	points:"
		"<0,-0.0416667>[0]	<1,-0.0416667>[1]	<2,-0.0416667>[2]"
		"<3,-0.0416667>[3]	<4,-0.0416667>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Squiggle	2"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time							X	Units:	s"
		"Y	Var	Name:	DELTA(sun_predictor.sun.declination)		Y	Units:	degree"
		"RUN	Directory:	RUN1	-	RUN2"
		"First	5	points:"
		"<0,0.0110458>[0]	<1,0.0110457>[1]	<2,0.0110457>[2]"
		"<3,0.0110457>[3]	<4,0.0110457>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Squiggle	3"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	hour_angle"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time							X	Units:	s"
		"Y	Var	Name:	DELTA(sun_predictor.sun.hour_angle)		Y	Units:	degree"
		"RUN	Directory:	RUN1	-	RUN2"
		"First	5	points:"
		"<0,-14.9985>[0]	<1,-14.9985>[1]	<2,-14.9985>[2]"
		"<3,-14.9985>[3]	<4,-14.9985>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Session	9_3
TEST_F(DPCTest,	ContrastCurves)	{
	//req.add_requirement("3042036071");

				std::string	tmp	=	parseDPCData(testxml[12].c_str());
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:"
		"Attribute	version:		1.0"
		"PAGE	VIEW:		Page	title:	Page	of	Squiggles"
		"Attribute	presentation:	contrast"

		"PLOT	VIEW:		Plot	title:	Contrast"
		"Number	of	Curves:	2"
		"X-axis	label:	time		Y-axis	label:	JD"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time			X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.JD	Y	Units:	day"
		"RUN	Directory:	RUN4"
		"First	5	points:"
		"<0,2.45661e+06>[0]	<1,2.45661e+06>[1]	<2,2.45661e+06>[2]"
		"<3,2.45661e+06>[3]	<4,2.45661e+06>[4]"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time		X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.JD	Y	Units:	day"
		"RUN	Directory:	RUN5"
		"First	5	points:"
		"<0,2.45661e+06>[0]	<1,2.45661e+06>[1]	<2,2.45661e+06>[2]"
		"<3,2.45661e+06>[3]	<4,2.45661e+06>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Contrast"
		"Number	of	Curves:	2"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination	Y	Units:	degree"
		"RUN	Directory:	RUN4"
		"First	5	points:"
		"<0,-18.0353>[0]	<1,-18.0353>[1]	<2,-18.0353>[2]"
		"<3,-18.0353>[3]	<4,-18.0353>[4]"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.declination	Y	Units:	degree"
		"RUN	Directory:	RUN5"
		"First	5	points:"
		"<0,-18.1012>[0]	<1,-18.1012>[1]	<2,-18.1012>[2]"
		"<3,-18.1012>[3]	<4,-18.1012>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Contrast"
		"Number	of	Curves:	2"
		"X-axis	label:	time		Y-axis	label:	hour_angle"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.hour_angle	Y	Units:	degree"
		"RUN	Directory:	RUN4"
		"First	5	points:"
		"<0,-151.171>[0]	<1,-151.167>[1]	<2,-151.163>[2]"
		"<3,-151.158>[3]	<4,-151.154>[4]"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	sun_predictor.sun.hour_angle	Y	Units:	degree"
		"RUN	Directory:	RUN5"
		"First	5	points:"
		"<0,-61.18>[0]			<1,-61.1758>[1]	<2,-61.1717>[2]"
		"<3,-61.1675>[3]	<4,-61.1633>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Contrast"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	JD"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time				X	Units:	s"
		"Y	Var	Name:	DELTA(sun_predictor.sun.JD)	Y	Units:	day"
		"RUN	Directory:	RUN4	-	RUN5"
		"First	5	points:"
		"<0,-0.25>[0]	<1,-0.25>[1]	<2,-0.25>[2]"
		"<3,-0.25>[3]	<4,-0.25>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Contrast"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	declination"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time						X	Units:	s"
		"Y	Var	Name:	DELTA(sun_predictor.sun.declination)	Y	Units:	degree"
		"RUN	Directory:	RUN4	-	RUN5"
		"First	5	points:"
		"<0,0.0659199>[0]	<1,0.0659199>[1]	<2,0.0659199>[2]"
		"<3,0.0659198>[3]	<4,0.0659198>[4]"
		"FINALIZE	PLOT	VIEW."

		"PLOT	VIEW:		Plot	title:	Contrast"
		"Number	of	Curves:	1"
		"X-axis	label:	time		Y-axis	label:	hour_angle"
		"CURVE	VIEW:"
		"X	Var	Name:	sys.exec.out.time						X	Units:	s"
		"Y	Var	Name:	DELTA(sun_predictor.sun.hour_angle)	Y	Units:	degree"
		"RUN	Directory:	RUN4	-	RUN5"
		"First	5	points:"
		"<0,-89.991>[0]	<1,-89.991>[1]	<2,-89.991>[2]"
		"<3,-89.991>[3]	<4,-89.991>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

TEST_F(DPCTest,	DPV_Test)	{
	//req.add_requirement("2826924382	1366633954");

	std::string	tmp	=	parseDPCData(testxml[13].c_str());
				//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:	Attribute	version:	1.0"
		"Attribute	foreground_color:	#ffffff"
		"Attribute	background_color:	#885588"

		"PAGE	VIEW:		Page	title:	This	is	a	page"
		"Attribute	foreground_color:	#ffffff"
		"Attribute	background_color:	#885588"
		"Attribute	hcells:	1						Attribute	vcells:	#ffffff"
		"Attribute	presentation:	SIMPLE				Attribute	gnuplot_template:	this	one"
		"Attribute	gnuplot_object:	ball				Attribute	gnuplot_geom:	square"
		"Attribute	gnuplot_plot_ratio:	1.5	Attribute	gnuplot_page_orientation:	landscape"

		"PLOT	VIEW:		Plot	title:	This	is	a	page"
		"Number	of	Curves:	1"
		"X-axis	label:	Mass					Y-axis	label:	Density"
		"Attribute	xmin:	-100000			Attribute	xmax:	100000"
		"Attribute	x_scale:	1.0				Attribute	ymin:	-200000"
		"Attribute	ymax:	200000				Attribute	y_scale:	2.0"
		"Attribute	grid:	yes				Attribute	grid_color:	pink"
		"Attribute	foreground_color:	#ffffff"
		"Attribute	background_color:	#000000"
		"Attribute	font:	helvetica"

		"CURVE	VIEW:"
		"X	Var	Name:	sun_predictor.sun.observer_offset_from_UTC"
		"X	Units:	min"
		"Y	Var	Name:	sun_predictor.sun.observer_latitude"
		"Y	Units:	degree"
		"RUN	Directory:	RUN_BINARY"
		"Attribute	x	max:	100000000			Attribute	x	symbol_style:	circles"
		"Attribute	x	symbol_size:	small		Attribute	x	line_style:	dotted"
		"Attribute	x	line_color:	green		Attribute	x	gnuplot_line_style:	box"
		"First	5	points:"
		"<-360,29.553>[0]	<-360,29.553>[1]	<-360,29.553>[2]"
		"<-360,29.553>[3]	<-360,29.553>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW."

		"TABLE	VIEW:"
		"RUN	Directory:	../TEST_DATA/RUN_BINARY"
		"Title:	This	is	a	table"
		"Number	of	Columns:	2"
		"Column	Label	[0]:	Time"
		"Variable	Name[0]:	sys.exec.out.time"
		"Column	Units	[0]:	s	format	[0]:	feng	shui"
		"Column	Label	[1]:	Pressure"
		"Variable	Name[1]:	sun_predictor.sun.hour_angle"
		"Column	Units	[1]:	arcsecond	format	[1]:	normal"
		"time:	10	values:		10	628267"
		"time:	11	values:		11	628282"
		"time:	12	values:		12	628297"
		"time:	13	values:		13	628312"
		"time:	14	values:		14	628327"
		"FINALIZE	TABLE	VIEW.	FINALIZE	PRODUCT	VIEW.",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

TEST_F(DPCTest,	Plot_Matlab)	{
	//req.add_requirement("2533684432");

	std::string	tmp	=	parseDPCData("../TEST_DATA/session_mat.xml");
	//std::cout	<<	tmp;
	int	result	=	strcmp_IgnoringWhiteSpace(
		"PRODUCT	VIEW:		Attribute	version:	1.0"
		"PAGE	VIEW:			Attribute	presentation:	simple"
		"PLOT	VIEW:"
		"Number	of	Curves:	1"
		"X-axis	label:	s_simtime		Y-axis	label:	dvvl_01c_3"

		"CURVE	VIEW:"
		"X	Var	Name:	s_simtime			X	Units:	s"
		"Y	Var	Name:	dvvl_01c_3			Y	Units:	1"
		"RUN	Directory:	RUN_MATLAB"
		"First	5	points:"
		"<0,0>[0]				<0.05,-4.61208e-45>[1]"
		"<0.1,-8.39122e-45>[2]	<0.15,1.51334e-44>[3]"
		"<0.2,4.47631e-45>[4]"
		"FINALIZE	PLOT	VIEW.	FINALIZE	PAGE	VIEW.	FINALIZE	PRODUCT	VIEW."
		,tmp.c_str());
	EXPECT_EQ(result,	0);
}

}
