
#include	<iostream>
#include	<fstream>

#include	"trick/UnitTest.hh"
#include	"trick/exec_proto.h"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"

Trick::UnitTest	*	the_unit_test_output	;

Trick::TestCase::TestCase(std::string	in_test_case_name	,	std::string	in_failure_string)	{
				name	=	in_test_case_name	;
				status	=	std::string("run")	;
				elapsed_time	=	0.0	;
				failure_string	=	in_failure_string	;
}

Trick::TestSuite::TestSuite()	{
				num_failures	=	0	;
}

int	Trick::TestSuite::add_test_results(std::string	in_test_case_name	,	std::string	in_failure_string	)	{

				Trick::TestCase	*	new_test	;
				new_test	=	new	TestCase(in_test_case_name	,	in_failure_string)	;

				test_results.push_back(new_test)	;
				if	(	in_failure_string.compare("")	)	{
								num_failures++	;
				}
				return(0)	;
}

int	Trick::TestSuite::add_test_requirements(std::string	test_case	,	std::string	par_num)	{
				unsigned	int	jj;
				TestCase	*	curr_case	;

				for	(jj	=	0	;	jj	!=	test_results.size()	;	jj++	)	{
								curr_case	=	test_results[jj]	;
								if	(curr_case->name	==	test_case)	{
												curr_case->parent	=	par_num;
								}
				}
				return(0);
}

void	Trick::TestSuite::delete_test_results()	{
				std::vector	<Trick::TestCase	*>::iterator	it	;
				for	(	it	=	test_results.begin()	;	it	!=	test_results.end()	;	++it	)	{
								delete	(*it)	;
				}
				test_results.clear()	;
}

Trick::UnitTest::UnitTest()	{
				the_unit_test_output	=	this	;
				enabled	=	false	;
				exit_code_enabled	=	true	;
				file_name	=	std::string("test_details.xml")	;
				name	=	std::string("AllTests")	;
}

Trick::UnitTest::~UnitTest()	{
				std::map<	std::string	,	TestSuite	>::iterator	it	;
				for	(	it	=	test_suites.begin()	;	it	!=	test_suites.end()	;	++it	)	{
								(it->second).delete_test_results()	;
				}
}

int	Trick::UnitTest::add_test_results(std::string	in_test_suite_name	,	std::string	in_test_case	,	std::string	in_failure_string	)	{

				if	(	enabled	)	{
								test_suites[in_test_suite_name].add_test_results(in_test_case	,	in_failure_string)	;
								if	(	in_failure_string.compare("")	)	{
												message_publish(MSG_ERROR,	"[		FAILED		]	%s:%s	%s\n",
													in_test_suite_name.c_str()	,	in_test_case.c_str()	,	in_failure_string.c_str())	;
								}	else	{
												message_publish(MSG_NORMAL,		"\033[32m[							OK	]\033[0m	%s:%s\n",	in_test_suite_name.c_str()	,	in_test_case.c_str()	)	;
								}
				}

				return(0)	;
}

int	Trick::UnitTest::add_test_requirements(std::string	in_test_suite_name,	std::string	in_test_case,	std::string	par_num)	{
				if	(	enabled	)	{
								test_suites[in_test_suite_name].add_test_requirements(in_test_case	,	par_num)	;
				}
							return	0	;
}

bool	Trick::UnitTest::enable()	{
				enabled	=	true	;
				return(true)	;
}

int	Trick::UnitTest::set_exit_code_enabled(bool	in_enable)	{
				exit_code_enabled	=	in_enable	;
				return	0	;
}

int	Trick::UnitTest::set_test_name(std::string	in_name)	{
				name	=	in_name	;
				return	0	;
}

int	Trick::UnitTest::set_file_name(std::string	in_name)	{
				file_name	=	in_name	;
				return	0	;
}

int	Trick::UnitTest::write_output()	{

				std::ofstream	out	;
				unsigned	int	num_tests	=	0	;
				unsigned	int	num_failures	=	0	;
				std::map<	std::string	,	TestSuite	>::iterator	it	;
				TestSuite	*	curr_suite	;
				TestCase	*	curr_case	;
				unsigned	int	jj	;

				if	(	enabled	)	{
								for	(	it	=	test_suites.begin()	;	it	!=	test_suites.end()	;	++it	)	{
												curr_suite	=	&(it->second)	;
												num_tests	+=	curr_suite->test_results.size()	;
												num_failures	+=	curr_suite->num_failures	;
								}
								message_publish(MSG_NORMAL,		"Total	tests:	%d	,	Failures:	%d\n",	num_tests	,	num_failures	)	;

								out.open(file_name.c_str())	;
								out	<<	"<?xml	version=\"1.0\"	encoding=\"UTF-8\"?>"	<<	std::endl	;
								out	<<	"<testsuites	tests=\""	<<	num_tests	<<	"\"	failures=\""	<<	num_failures	<<
									"\"	disabled=\"0\"	errors=\"0\"	time=\"0.0\"	name=\""	<<	name	<<	"\">"	<<	std::endl	;

								for	(	it	=	test_suites.begin()	;	it	!=	test_suites.end()	;	++it	)	{
												curr_suite	=	&(it->second)	;
												out	<<	"		<testsuite	name=\""	<<	it->first	<<	"\"	tests=\""	<<	curr_suite->test_results.size()	;
												out	<<	"\"	failures=\""	<<	num_failures	<<	"\"	disabled=\"0\"	errors=\"0\"	time=\"0.0\">"	<<	std::endl	;
												for	(	jj	=	0	;	jj	<	curr_suite->test_results.size()	;	jj++	)	{
																curr_case	=	curr_suite->test_results[jj]	;
																out	<<	"				<testcase	name=\""	<<	curr_case->name	<<	"\"	status=\""	<<	curr_case->status	;
																out	<<	"\"	time=\"0\"	classname=\""	<<	it->first	<<	"\""	;

																//	parent	stuff	goes	here
																//out	<<	"	parent=\""	<<	curr_case->parent	<<	"\""	;

																if	(	curr_case->failure_string.compare("")	)	{
																				out	<<	"	>"	<<	std::endl		;
																				out	<<	"							<failure	message=\""	<<	curr_case->failure_string	<<	"\"	type=\"\"/>"	<<	std::endl	;
																				out	<<	"				</testcase>"	<<	std::endl;
																}	else	{
																				out	<<	"	/>"	<<	std::endl	;
																}
												}
												out	<<	"		</testsuite>"	<<	std::endl		;
								}
								out	<<	"</testsuites>"	<<	std::endl		;

								if	(	exit_code_enabled	&&	num_failures	>	0	)	{
												exec_terminate_with_return(	1	,	__FILE__	,	__LINE__	,	"Unit	Test	failure	detected."	)	;
								}
				}

				return(0)	;
}

