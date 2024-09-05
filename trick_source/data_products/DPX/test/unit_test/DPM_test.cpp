#define	protected	public

#include	<ctype.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<libxml/parser.h>
#include	<libxml/tree.h>

#include	"gtest/gtest.h"
//#include	"trick_utils/reqs/include/RequirementScribe.hh"
#include	"DPM/DPM_product.hh"

namespace	Trick	{

class	DPMTest	:	public	::testing::Test	{
				protected:
								//Trick::RequirementScribe	req;
		std::string	testxml[10];

		DPMTest()	{}
		~DPMTest()	{}
		virtual	void	SetUp();
								virtual	void	TearDown();

		std::string	parseTestData(const	char*	name);
};

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


void	DPMTest::SetUp()	{

	testxml[0]	=	"../TEST_DATA/product_1.xml";
				testxml[1]	=	"../TEST_DATA/product_2.xml";
				testxml[2]	=	"../TEST_DATA/product_3.xml";
				testxml[3]	=	"../TEST_DATA/product_4.xml";
				testxml[4]	=	"../TEST_DATA/product_5.xml";
				testxml[5]	=	"../TEST_DATA/product_6.xml";
				testxml[6]	=	"../TEST_DATA/product_7.xml";
				testxml[7]	=	"../TEST_DATA/product_8.xml";
				testxml[8]	=	"../TEST_DATA/product_9.xml";

	//	product_10.xml	has	an	inconsistency	with	DTD
	testxml[9]	=	"../TEST_DATA/product_10.xml";
}

void	DPMTest::TearDown()	{}

std::string	DPMTest::parseTestData(const	char*	name)	{
				xmlParserCtxtPtr	parser_context;
				xmlDocPtr	doc;
	xmlNode	*root_node;
	DPM_product	*product;

	std::stringstream	s;
	std::string	tmp;

	LIBXML_TEST_VERSION
				if	((parser_context	=	xmlNewParserCtxt())	==	NULL)	{
								fprintf(stderr,"Out	of	Memory\n");
								exit(1);
				}

	doc	=	xmlCtxtReadFile(parser_context,	name,	NULL,	XML_PARSE_DTDVALID+XML_PARSE_NOERROR);

	if	(parser_context->valid)	{
								xmlFreeParserCtxt(parser_context);
		root_node	=	xmlDocGetRootElement(doc);

								if	((product	=	new	DPM_product(	NULL,	root_node))	==	NULL	)	{
											fprintf(stderr,"Failed	to	create	a	DPM_product.\n");
								}

								s	<<	product;
		tmp	=	s.str();

		delete	product;

	}	else	{
		xmlFreeParserCtxt(parser_context);
		fprintf(stderr,"Failed	to	validate	\"%s\"\n",name);
		tmp	=	"Failed	to	validate";
	}
				xmlFreeDoc(doc);

	return	tmp;
}

TEST_F(DPMTest,	ValidateDTD)	{
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

		if	(i	<	9)	{
			ASSERT_TRUE(parser_context->valid);
		}	else	{
			ASSERT_FALSE(parser_context->valid);
		}
		xmlFreeDoc(doc);
	}
	xmlFreeParserCtxt(parser_context);
}

//	Product	1
TEST_F(DPMTest,	VarvsTime_NoAttributes)	{
	//req.add_requirement("2416241399");

	std::string	tmp	=	parseTestData(testxml[0].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<page>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<curve>"
		"<var>sys.exec.out.time</var>"
		"<var>sun_predictor.sun.declination</var>"
		"</curve>"
		"</plot>"
		"<plot>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<curve>"
		"<var>sys.exec.out.time</var>"
		"<var>sun_predictor.sun.declination</var>"
		"</curve>"
		"</plot>"
		"</page>"
		"</product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Product	2
TEST_F(DPMTest,	VarvsVar_NoAttributes)	{
	//req.add_requirement("1589175037");

				std::string	tmp	=	parseTestData(testxml[1].c_str());
				int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<page>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<curve>"
		"<var>sun_predictor.sun.solar_azimuth</var>"
		"<var>sun_predictor.sun.solar_elevation</var>"
		"</curve>"
		"</plot>"
		"</page>"
		"</product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Product	3
TEST_F(DPMTest,	Table_NoAttributes)	{
	std::string	tmp	=	parseTestData(testxml[2].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<table>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<column>"
		"<units>s</units>"
		"<var>sys.exec.out.time</var>"
		"</column>"
		"<column>"
		"<var>sun_predictor.sun.solar_azimuth</var>"
		"</column>"
		"<column>"
		"<var>sun_predictor.sun.solar_elevation</var>"
		"</column>"
		"</table>"
		"</product>",
		tmp.c_str());
		EXPECT_EQ(result,	0);
}

//	Product	4
TEST_F(DPMTest,	VarvsTime_Attributes)	{
				std::string	tmp	=	parseTestData(testxml[3].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<title>Solar	Elevation</title>"
		"<page>"
		"<title>Solar	Elevation	for	a	Day</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot	grid=\"yes\"	grid_color=\"blue\""
		"font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\""
		"background_color=\"#eef0ff\">"
		"<title>Sun	Elevation	vs.	Time</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis>"
		"<label>Time</label>"
		"<units>hr</units>"
		"</axis>"
		"<axis>"
		"<label>Elevation</label>"
		"<units>d</units>"
		"</axis>"
		"<curve>"
		"<var>sys.exec.out.time</var>"
		"<var	line_color=\"red\"	line_style=\"dash\">sun_predictor.sun.solar_elevation</var>"
		"</curve>"
		"</plot>"
		"</page>"
		"</product>",
		tmp.c_str());
				EXPECT_EQ(result,	0);
}

//	Product	5
TEST_F(DPMTest,	VarvsVarvsTime_Attributes)	{
	//req.add_requirement("2469402355");

	std::string	tmp	=	parseTestData(testxml[4].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\"	foreground_color=\"#000000\"	background_color=\"#ffffff\">"
		"<title>title	of	the	product</title>"
		"<page>"
		"<title>Solar	Elevation	for	a	Day</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot	grid=\"yes\""
		"font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\""
		"background_color=\"#fffff0\">"
		"<title>Sun	Time	vs.	Elevation	vs.	Azimuth</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis>"
		"<label>Azimuth</label>"
		"<units>d</units>"
		"</axis>"
		"<axis	format=\"%g\">"
		"<label>Elevation</label>"
		"<units>d</units>"
		"</axis>"
		"<axis>"
		"<label>Time</label>"
		"<units>s</units>"
		"</axis>"
		"<curve>"
		"<var>sun_predictor.sun.solar_azimuth</var>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_elevation</var>"
		"<var>sys.exec.out.time</var>"
		"</curve>"
		"</plot>"
		"</page>"
		"</product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Product	6
TEST_F(DPMTest,	Table_Attributes)	{
				std::string	tmp	=	parseTestData(testxml[5].c_str());
	//std::cout	<<	tmp	<<	std::endl;
				int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<title>Table	of	Sun	Positions</title>"
		"<table	foreground_color=\"#00ff00\"	background_color=\"#556677\">"
		"<title>Sun	Azimuth	vs.	Time</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<column	format=\"%24.10f\">"
		"<label>Time</label>"
		"<units>s</units>"
		"<var>sys.exec.out.time</var>"
		"</column>"
		"<column	format=\"%16.5q\">"
		"<label>Azimuth</label>"
		"<units>d</units>"
		"<var>sun_predictor.sun.solar_azimuth</var>"
		"</column>"
		"<column	format=\"%24.10f\">"
		"<label>Hours</label>"
		"<units>hr</units>"
		"<var>sys.exec.out.time</var>"
		"</column>"
								"<column	format=\"%24.10f\">"
		"<label>Hours</label>"
		"<units>hr</units>"
		"<var	units=\"hr\">sys.exec.out.time</var>"
		"</column>"
		"<column	format=\"%16.5f\">"
		"<label>Elevation</label>"
		"<units>d</units>"
		"<var>sun_predictor.sun.solar_elevation</var>"
		"</column>"
		"</table>"
		"</product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Product	7
TEST_F(DPMTest,	MultiplePlots)	{
	//req.add_requirement("445237094");

				std::string	tmp	=	parseTestData(testxml[6].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<title>title	of	the	product</title>"
		"<page	vcells=\"3\">"
		"<title>Solar	Elevation	for	a	Day</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot	grid=\"yes\"	font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\""
		"background_color=\"#fff0ff\">"
		"<title>Sun	Elevation	vs.	Time</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis><label>Time</label>"
		"<units>hr</units>"
		"</axis><axis>"
		"<label>Elevation</label>"
		"<units>d</units>"
		"</axis><curve>"
		"<var>sys.exec.out.time</var>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_elevation</var>"
		"</curve></plot>"
		"<plot	grid=\"yes\"	font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\""
		"background_color=\"#f0fff0\">"
		"<title>Sun	Azimuth	vs.	Time</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis><label>Time</label>"
		"<units>hr</units>"
		"</axis><axis	format=\"%g\">"
		"<label>Azimuth</label>"
		"<units>d</units>"
		"</axis><curve>"
		"<var>sys.exec.out.time</var>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_azimuth</var>"
		"</curve></plot>"
		"<plot	grid=\"yes\"	font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\""
		"	background_color=\"#fffff0\">"
		"<title>Sun	Elevation	vs.	Azimuth</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis><label>Azimuth</label>"
		"<units>d</units>"
		"</axis><axis	format=\"%g\">"
		"<label>Elevation</label>"
		"<units>d</units>"
		"</axis><curve>"
		"<var>sun_predictor.sun.solar_azimuth</var>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_elevation</var>"
		"</curve></plot></page></product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Product	8
TEST_F(DPMTest,	MultipleTables_Multipe_Plots)	{
	//req.add_requirement("1422666851	150267139	445237094");

				std::string	tmp	=	parseTestData(testxml[7].c_str());
	int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\"	background_color=\"#885588\">"
		"<page><title>Solar	Elevation	for	a	Day</title>"
		"<tstart>0</tstart><tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot	grid=\"yes\"	font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\">"
		"<title>Sun	Elevation	vs.	Time</title>"
		"<tstart>0</tstart><tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis><label>Time</label><units>hr</units></axis><axis>"
		"<label>Elevation</label><units>d</units></axis><curve>"
		"<var>sys.exec.out.time</var>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_elevation</var>"
		"</curve></plot></page><page>"
		"<tstart>0</tstart><tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot	grid=\"yes\"	font=\"-*-new	century	schoolbook-bold-r-*--12-*-*-*-*-*-*-*\""
		"background_color=\"#fffff0\">"
		"<title>Sun	Elevation	vs.	Azimuth</title>"
		"<tstart>0</tstart><tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<axis><label>Azimuth</label>"
		"<units>d</units></axis><axis>"
		"<label>Elevation</label><units>d</units></axis><curve>"
		"<var>sun_predictor.sun.solar_azimuth</var>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_elevation</var>"
		"</curve></plot></page><table>"
		"<title>My	Two	Column	Table</title>"
		"<tstart>0</tstart><tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<column	format=\"%24.10f\">"
		"<label>Time</label>"
		"<var>sys.exec.out.time</var>"
		"</column><column	format=\"%16.5q\">"
		"<label>Azimuth</label><units>d</units>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_azimuth</var>"
		"</column><column	format=\"%16.5f\">"
		"<label>Elevation</label><units>d</units>"
		"<var	line_color=\"#323200\">sun_predictor.sun.solar_elevation</var>"
		"</column></table>"
		"<table	background_color=\"#ffeebb\">"
		"<title>Four	Column	Table</title>"
		"<tstart>0</tstart><tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<column	format=\"%24.10f\">"
		"<units>s</units><var>sys.exec.out.time</var>"
		"</column><column	format=\"%16.5f\">"
		"<label>Seconds</label>"
		"<var	line_color=\"#323200\">sun_predictor.sun.local_time.sec</var>"
		"</column><column	format=\"%16.5f\">"
		"<label>Day</label>"
		"<var	line_color=\"#323200\">sun_predictor.sun.local_time.day</var>"
		"</column><column	format=\"%16.5f\">"
		"<label>Month</label>"
		"<var	line_color=\"#323200\">sun_predictor.sun.local_time.month</var>"
		"</column><column	format=\"%16.5f\">"
		"<label>Year</label>"
		"<var	line_color=\"#323200\">sun_predictor.sun.local_time.year</var>"
		"</column></table></product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}

//	Product	9
TEST_F(DPMTest,	Multipe_Plots_NoAttributes)	{
	//req.add_requirement("445237094");

				std::string	tmp	=	parseTestData(testxml[8].c_str());
				int	result	=	strcmp_IgnoringWhiteSpace(
		"<product	version=\"1.0\">"
		"<page	vcells=\"3\">"
		"<title>Page	of	Squiggles</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<plot>"
		"<title>Squiggle	1</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<curve>"
		"<var>sys.exec.out.time</var>"
		"<var>sun_predictor.sun.JD</var>"
		"</curve>"
		"</plot>"
		"<plot>"
		"<title>Squiggle	2</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<curve>"
		"<var>sys.exec.out.time</var>"
		"<var>sun_predictor.sun.declination</var>"
		"</curve>"
		"</plot>"
		"<plot>"
		"<title>Squiggle	3</title>"
		"<tstart>0</tstart>"
		"<tstop>1.79769e+308</tstop>"
		"<frequency>1.79769e+308</frequency>"
		"<curve>"
		"<var>sys.exec.out.time</var>"
		"<var>sun_predictor.sun.hour_angle</var>"
		"</curve>"
		"</plot>"
		"</page>"
		"</product>",
		tmp.c_str());
	EXPECT_EQ(result,	0);
}
}
