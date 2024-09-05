#include	"test_client.hh"
#include	"trick/var_binary_parser.hh"
#include	<math.h>


class	VariableServerUDPTest	:	public	::testing::Test	{
				protected:
								VariableServerUDPTest()	{
												socket_status	=	socket.init("localhost",	48000,	SOCK_DGRAM);

												if	(socket_status	==	0)	{
																std::stringstream	request;
																request	<<	"trick.var_set_client_tag(\"UDP_VSTest";
																request	<<	numSession++;
																request	<<	"\")	\n";

																socket	<<	request.str();
												}
								}
								~VariableServerUDPTest()	{
												socket.close();
								}

								Socket	socket;
								int	socket_status;
								
								static	int	numSession;
};

class	VariableServerTestAltListener	:	public	::testing::Test	{
				protected:
								VariableServerTestAltListener()	{
												socket_status	=	socket.init("localhost",	49000);

												if	(socket_status	==	0)	{
																std::stringstream	request;
																request	<<	"trick.var_set_client_tag(\"altListener_VSTest";
																request	<<	numSession++;
																request	<<	"\")	\n";

																socket	<<	request.str();
												}
								}
								~VariableServerTestAltListener()	{
												socket	<<	"trick.var_exit()\n";
												socket.close();
								}

								Socket	socket;
								int	socket_status;
								
								static	int	numSession;
};

int	VariableServerTest::numSession	=	0;
int	VariableServerUDPTest::numSession	=	0;
int	VariableServerTestAltListener::numSession	=	0;

#ifndef	__APPLE__
class	VariableServerTestMulticast	:	public	::testing::Test	{
				protected:
								VariableServerTestMulticast()	{
												socket_status	=	socket.init("",	47000,	SOCK_DGRAM);
												socket_status	=	multicast_listener.init_multicast("224.10.10.10",	47000);

												if	(socket_status	==	0)	{
																std::stringstream	request;
																request	<<	"trick.var_set_client_tag(\"multicast_VSTest";
																request	<<	numSession++;
																request	<<	"\")	\n";

																socket	<<	request.str();
												}
								}
								~VariableServerTestMulticast()	{
												socket.close();
												multicast_listener.close();
								}

								Socket	socket;
								Socket	multicast_listener;

								int	socket_status;
								
								static	int	numSession;
};
int	VariableServerTestMulticast::numSession	=	0;
#endif

/**********************************************************/
/*											Helpful	constants	and	functions														*/
/**********************************************************/


const	int	MODE_RUN	=	5;
const	int	MODE_FREEZE	=	1;

int	strcmp_IgnoringWhiteSpace(std::string	s1_str,	std::string	s2_str)	{
				const	char	*	s1	=	s1_str.c_str();
				const	char	*	s2	=	s2_str.c_str();

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

void	spin	(Socket&	socket,	int	wait_cycles	=	5)	{
				for	(int	i	=	0;	i	<	wait_cycles;	i++)	
								socket.receive();
}

void	wait_for_mode_change	(Socket&	socket,	int	expected_mode,	int	max_wait_iterations	=	10)	{
				int	iteration	=	0;
				std::string	mode_reply;
				std::string	expected_reply	=	"5\t"	+	std::to_string(expected_mode)	+	"\n";
				while	(iteration++	<	max_wait_iterations)	{
								socket	<<	"trick.var_send_once(\"trick_sys.sched.mode\")\n";
								socket	>>	mode_reply;
								if	(mode_reply	==	expected_reply)
												break;
				}
}

void	dump_checkpoint	(Socket&	socket,	const	std::string&	checkpoint_name)	{
				socket	<<	"trick.var_pause()\ntrick.exec_freeze()\n";
				wait_for_mode_change(socket,	MODE_FREEZE);

				std::string	checkpoint_cmd	=	"trick.checkpoint(\""	+	checkpoint_name	+	"\")\n";
				socket	<<	checkpoint_cmd;

				socket	<<	"trick.exec_run()\n";
				wait_for_mode_change(socket,	MODE_RUN);
				socket	<<	"trick.var_unpause()\n";
}

void	load_checkpoint	(Socket&	socket,	const	std::string&	checkpoint_name)	{
				socket	<<	"trick.var_pause()\ntrick.exec_freeze()\n";
				wait_for_mode_change(socket,	MODE_FREEZE);

				std::string	checkpoint_cmd	=	"trick.load_checkpoint(\""	+	checkpoint_name	+	"\")\n";
				socket	<<	checkpoint_cmd;
				sleep(5);

				socket	<<	"trick.exec_run()\n";
				wait_for_mode_change(socket,	MODE_RUN);
				socket	<<	"trick.var_unpause()\n";
}

/*****************************************/
/*											Multicast	Test														*/
/*****************************************/

#ifndef	__APPLE__

TEST_F	(VariableServerTestMulticast,	Strings)	{
	if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				socket	<<	"trick.var_send_once(\"vsx.vst.o\")\n";
				std::string	expected("5\tYou	will	rejoice	to	hear	that	no	disaster	has	accompanied	the	commencement	of	an	enterprise	which	you	have	regarded	with	such	evil	forebodings.	I	arrived	here	yesterday,	and	my	first	task	is	to	assure	my	dear	sister	of	my	welfare	and	increasing	confidence	in	the	success	of	my	undertaking.");

				multicast_listener	>>	reply;	

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				expected	=	std::string("5\tI	am	already	far	north	of	London,	and	as	I	walk	in	the	streets	of	Petersburgh,	I	feel	a	cold	northern	breeze	play	upon	my	cheeks,	which	braces	my	nerves	and	fills	me	with	delight.	Do	you	understand	this	feeling?");
				socket	<<	"trick.var_send_once(\"vsx.vst.p\")\n";

				multicast_listener	>>	reply;	

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}


TEST_F	(VariableServerTestMulticast,	AddRemove)	{
					if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				int	max_tries	=	3;
				int	tries	=	0;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				multicast_listener	>>	reply;
				expected	=	std::string("0		-1234");
				
				tries	=	0;
				while	(strcmp_IgnoringWhiteSpace(reply,	expected)	!=	0	&&	tries++	<	max_tries)	{
								multicast_listener	>>	reply;
				}

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Expected:	"	<<	expected	<<	"\tAcutal:	"	<<	reply;

				multicast_listener	>>	reply;
				tries	=	0;
				while	(strcmp_IgnoringWhiteSpace(reply,	expected)	!=	0	&&	tries++	<	max_tries)	{
								multicast_listener	>>	reply;
				}

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Expected:	"	<<	expected	<<	"\tAcutal:	"	<<	reply;

				socket	<<	"trick.var_add(\"vsx.vst.m\")\n";
				multicast_listener	>>	reply;
				expected	=	std::string("0		-1234	1");
				tries	=	0;
				while	(strcmp_IgnoringWhiteSpace(reply,	expected)	!=	0	&&	tries++	<	max_tries)	{
								multicast_listener	>>	reply;
				}
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Expected:	"	<<	expected	<<	"\tAcutal:	"	<<	reply;

				socket	<<	"trick.var_remove(\"vsx.vst.m\")\n";
				multicast_listener	>>	reply;
				expected	=	std::string("0		-1234");
				tries	=	0;
				while	(strcmp_IgnoringWhiteSpace(reply,	expected)	!=	0	&&	tries++	<	max_tries)	{
								multicast_listener	>>	reply;
				}
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Expected:	"	<<	expected	<<	"\tAcutal:	"	<<	reply;

				socket	<<	"trick.var_add(\"vsx.vst.n\")\n";
				multicast_listener	>>	reply;
				expected	=	std::string("0		-1234				0,1,2,3,4");
				tries	=	0;
				while	(strcmp_IgnoringWhiteSpace(reply,	expected)	!=	0	&&	tries++	<	max_tries)	{
								multicast_listener	>>	reply;
				}
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Expected:	"	<<	expected	<<	"\tAcutal:	"	<<	reply;

				socket	<<	"trick.var_exit()\n";
}

#endif

/************************************/
/*											UDP	Tests														*/
/************************************/


TEST_F	(VariableServerUDPTest,	Strings)	{
	if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				socket	<<	"trick.var_send_once(\"vsx.vst.o\")\n";
				socket	>>	reply;
				std::string	expected("5\tYou	will	rejoice	to	hear	that	no	disaster	has	accompanied	the	commencement	of	an	enterprise	which	you	have	regarded	with	such	evil	forebodings.	I	arrived	here	yesterday,	and	my	first	task	is	to	assure	my	dear	sister	of	my	welfare	and	increasing	confidence	in	the	success	of	my	undertaking.");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);


				expected	=	std::string("5\tI	am	already	far	north	of	London,	and	as	I	walk	in	the	streets	of	Petersburgh,	I	feel	a	cold	northern	breeze	play	upon	my	cheeks,	which	braces	my	nerves	and	fills	me	with	delight.	Do	you	understand	this	feeling?");
				socket	<<	"trick.var_send_once(\"vsx.vst.p\")\n";

				socket	>>	reply;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}


TEST_F	(VariableServerUDPTest,	AddRemove)	{
					if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	>>	reply;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234	1");
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_remove(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.n\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234				0,1,2,3,4");
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_exit()\n";
}

/*********************************************/
/*											Alt	Listener	Tests														*/
/*********************************************/

TEST_F	(VariableServerTestAltListener,	Strings)	{
	if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				socket	<<	"trick.var_send_once(\"vsx.vst.o\")\n";
				socket	>>	reply;
				std::string	expected("5\tYou	will	rejoice	to	hear	that	no	disaster	has	accompanied	the	commencement	of	an	enterprise	which	you	have	regarded	with	such	evil	forebodings.	I	arrived	here	yesterday,	and	my	first	task	is	to	assure	my	dear	sister	of	my	welfare	and	increasing	confidence	in	the	success	of	my	undertaking.");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);


				expected	=	std::string("5\tI	am	already	far	north	of	London,	and	as	I	walk	in	the	streets	of	Petersburgh,	I	feel	a	cold	northern	breeze	play	upon	my	cheeks,	which	braces	my	nerves	and	fills	me	with	delight.	Do	you	understand	this	feeling?");
				socket	<<	"trick.var_send_once(\"vsx.vst.p\")\n";

				socket	>>	reply;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}

TEST_F	(VariableServerTestAltListener,	AddRemove)	{
					if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	>>	reply;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234	1");
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_remove(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.n\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234				0,1,2,3,4");
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}


TEST_F	(VariableServerTestAltListener,	RestartAndSet)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\n");

				EXPECT_EQ(reply,	expected);

				dump_checkpoint(socket,	"reload_file.ckpnt");

				socket	<<	"trick.var_add(\"vsx.vst.e\",\"m\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\t-123456	{m}\n");

				socket	<<	"trick.var_set(\"vsx.vst.c\",	5)\n";
				socket	>>	reply;
				expected	=	std::string("0\t5\t-123456	{m}\n");

				EXPECT_EQ(reply,	expected);

				load_checkpoint(socket,	"RUN_test/reload_file.ckpnt");

				socket	>>	reply;
				expected	=	std::string("0\t-1234\t-123456	{m}\n");

				EXPECT_EQ(reply,	expected);
}

/*********************************************/
/*											Normal	case	tests															*/
/*********************************************/

TEST_F	(VariableServerTest,	Strings)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				socket	<<	"trick.var_send_once(\"vsx.vst.o\")\n";
				socket	>>	reply;
				std::string	expected("5\tYou	will	rejoice	to	hear	that	no	disaster	has	accompanied	the	commencement	of	an	enterprise	which	you	have	regarded	with	such	evil	forebodings.	I	arrived	here	yesterday,	and	my	first	task	is	to	assure	my	dear	sister	of	my	welfare	and	increasing	confidence	in	the	success	of	my	undertaking.");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);


				expected	=	std::string("5\tI	am	already	far	north	of	London,	and	as	I	walk	in	the	streets	of	Petersburgh,	I	feel	a	cold	northern	breeze	play	upon	my	cheeks,	which	braces	my	nerves	and	fills	me	with	delight.	Do	you	understand	this	feeling?");
				socket	<<	"trick.var_send_once(\"vsx.vst.p\")\n";

				socket	>>	reply;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				//	TODO:	Does	wchar	actually	work?
				//	expected	=	std::string("5\tThis	breeze,	which	has	travelled	from	the	regions	towards	which	I	am	advancing,	gives	me	a	foretaste	of	those	icy	climes.	Inspirited	by	this	wind	of	promise,	my	daydreams	become	more	fervent	and	vivid.");
				//	socket	<<	"trick.var_send_once(\"vsx.vst.q\")\n";

				//	socket	>>	reply;

				//	std::cout	<<	"\tExpected:	"	<<	expected	<<	"\n\tActual:	"	<<	reply	<<	std::endl;

				//	EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}

TEST_F	(VariableServerTest,	NoExtraTab)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\n");

				EXPECT_STREQ(reply.c_str(),	expected.c_str());

				socket	>>	reply;

				EXPECT_STREQ(reply.c_str(),	expected.c_str());

				socket	<<	"trick.var_add(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\t1\n");
				
				EXPECT_STREQ(reply.c_str(),	expected.c_str());

				socket	<<	"trick.var_remove(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\n");

				socket	<<	"trick.var_add(\"vsx.vst.n\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\t0,1,2,3,4\n");
				
				EXPECT_STREQ(reply.c_str(),	expected.c_str());
}

TEST_F	(VariableServerTest,	AddRemove)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	>>	reply;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234	1");
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_remove(\"vsx.vst.m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");
				
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.n\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234				0,1,2,3,4");
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}

TEST_F	(VariableServerTest,	BadRefResponse)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				std::cerr	<<	"The	purpose	of	this	test	is	to	cause	an	error.	Error	messages	are	expected."	<<	std::endl;

				socket	<<	"trick.var_send_once(\"vsx.vst.no_such_variable\")\n";
				socket	>>	reply;
				expected	=	std::string("5			BAD_REF");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}


TEST_F	(VariableServerTest,	Units)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				std::cerr	<<	"The	purpose	of	this	test	is	to	cause	an	error.	Error	messages	are	expected."	<<	std::endl;
				socket	<<	"trick.var_add(\"vsx.vst.c\")\ntrick.var_units(\"vsx.vst.c\",\"g\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_add(\"vsx.vst.e\",\"m\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234		-123456	{m}");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_units(\"vsx.vst.e\",\"ft\")\n";
				socket	>>	reply;
				expected	=	std::string("0		-1234		-405039	{ft}");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}

TEST_F	(VariableServerTest,	dump_info)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}
				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";

				std::string	command	=	"trick.var_server_list_connections()\n";
				socket	<<	command;
				sleep(1);
}


TEST_F	(VariableServerTest,	SendOnce)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_send_once(\"vsx.vst.e\")\n";
				socket	>>	reply;
				expected	=	std::string("5		-123456");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				//	Check	that	nothing	is	sent	cyclically
				EXPECT_EQ(socket.check_for_message_availible(),	false);

				socket	<<	"trick.var_send_once(\"vsx.vst.n[0],	vsx.vst.n[1],	vsx.vst.n[2],\",	3)\n";
				socket	>>	reply;
				expected	=	std::string("5			0			1			2");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				std::cerr	<<	"The	purpose	of	this	test	is	to	cause	an	error.	Error	messages	are	expected."	<<	std::endl;
				socket	<<	"trick.var_send_once(\"vsx.vst.n[0],	vsx.vst.n[1],	vsx.vst.n[2],\",	4)\n";

				//	Wrong	number	of	variables,	should	not	send	a	response	but	should	see	an	error	message	from	the	Trick	side
				EXPECT_EQ(socket.check_for_message_availible(),	false);
}

TEST_F	(VariableServerTest,	Exists)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_exists(\"vsx.vst.e\")\n";
				socket	>>	reply;
				expected	=	std::string("1		1");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);


				socket	<<	"trick.var_exists(\"vsx.vst.z\")\n";
				socket	>>	reply;
				expected	=	std::string("1		0");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				socket	<<	"trick.var_binary()\n";

				std::vector<unsigned	char>	actual_bytes;
				std::vector<unsigned	char>	expected_bytes;
				socket	<<	"trick.var_exists(\"vsx.vst.e\")\n";
				actual_bytes	=	socket.receive_bytes();
				expected_bytes	=	{0x01,	0x00,	0x00,	0x00,	0x01};

				EXPECT_EQ(expected_bytes,	actual_bytes);


				socket	<<	"trick.var_exists(\"vsx.vst.z\")\n";
				actual_bytes	=	socket.receive_bytes();
				expected_bytes	=	{0x01,	0x00,	0x00,	0x00,	0x00};

				EXPECT_EQ(expected_bytes,	actual_bytes);

}

TEST_F	(VariableServerTest,	ListSize)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.a\")\ntrick.var_add(\"vsx.vst.b\")\ntrick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.d\")\ntrick.var_pause()\n";
				socket	<<	"trick.var_send_list_size()\n";

				socket	>>	reply;
				expected	=	"3			4";

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Reply:	"	<<	reply	<<	"\tExpected:	"	<<	expected;

				//	Adding	a	variable	to	the	list	more	than	once	is	allowed
				socket	<<	"trick.var_add(\"vsx.vst.a\")\n";
				socket	<<	"trick.var_send_list_size()\n";

				socket	>>	reply;
				expected	=	"3			5";

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0)	<<	"Reply:	"	<<	reply	<<	"\tExpected:	"	<<	expected;

				socket	<<	"trick.var_add(\"vsx.vst.e\")\ntrick.var_binary()\ntrick.var_send_list_size()\n";

				std::vector<unsigned	char>	actual_bytes;
				std::vector<unsigned	char>	expected_bytes;
				actual_bytes	=	socket.receive_bytes();
				expected_bytes	=	{0x03,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x06,	0x00,	0x00,	0x00};

				EXPECT_EQ(expected_bytes,	actual_bytes);
}


TEST_F	(VariableServerTest,	bitfields)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.var_send_once(\"vsx.vst.my_bitfield.var1,vsx.vst.my_bitfield.var2,vsx.vst.my_bitfield.var3,vsx.vst.my_bitfield.var4\",	4)\n";
				std::string	reply_str;
				socket	>>	reply_str;

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply_str,	"5	3	-128	0	2112"),	0);

				socket	<<	"trick.var_binary()\ntrick.var_send_once(\"vsx.vst.my_bitfield.var1,vsx.vst.my_bitfield.var2,vsx.vst.my_bitfield.var3,vsx.vst.my_bitfield.var4\",	4)\n";

				std::vector<unsigned	char>	reply	=	socket.receive_bytes();

				ParsedBinaryMessage	message;
				try	{
								message.parse(reply);
				}	catch	(const	MalformedMessageException&	ex)	{	
								FAIL()	<<	"Parser	threw	an	exception:	"	<<	ex.what();
				}

				for	(int	i	=	0;	i	<	message.getNumVars();	i++)	{
								EXPECT_TRUE(message.variables[i].getType()	==	12	||	message.variables[i].getType()	==	13);
				}
}

TEST_F	(VariableServerTest,	transmit_file)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.send_file(\"file_to_send.txt\")\n";

				std::stringstream	file_contents;

				while	(socket.check_for_message_availible())	{
								socket	>>	file_contents;
				}

				std::ifstream	actual_file("file_to_send.txt");
				std::string	expected_contents;
				getline(actual_file,	expected_contents);

				std::string	expected_header	=	"2\t"	+	std::to_string(expected_contents.size());

				std::string	test_line;

				std::getline(file_contents,	test_line);
				EXPECT_EQ(test_line,	expected_header);

				std::getline(file_contents,	test_line);
				EXPECT_EQ(test_line,	expected_contents);
}


TEST_F	(VariableServerTest,	SendSieResource)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.send_sie_resource()\n";

				std::stringstream	received_file;

				while	(socket.check_for_message_availible())	{
								socket	>>	received_file;
				}

				std::string	first_line;
				std::getline(received_file,	first_line);

				//	We're	not	gonna	bother	comparing	the	contents
				//	Just	check	to	see	that	the	message	type	is	correct
				ASSERT_EQ(first_line.at(0),	'2');
}

TEST_F	(VariableServerTest,	SendSieClass)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.send_sie_class()\n";

				std::stringstream	received_file;
				std::string	file_temp;

				while	(socket.check_for_message_availible())	{
								socket	>>	file_temp;
								received_file	<<	file_temp;
				}

				std::string	first_line;
				std::getline(received_file,	first_line);

				//	We're	not	gonna	bother	comparing	the	contents
				//	Just	check	to	see	that	the	message	type	is	correct
				ASSERT_EQ(first_line.at(0),	'2');
}

TEST_F	(VariableServerTest,	SendSieEnum)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.send_sie_enum()\n";

				std::stringstream	received_file;
				std::string	file_temp;

				while	(socket.check_for_message_availible())	{
								socket	>>	file_temp;
								received_file	<<	file_temp;
				}

				std::string	first_line;
				std::getline(received_file,	first_line);

				//	We're	not	gonna	bother	comparing	the	contents
				//	Just	check	to	see	that	the	message	type	is	correct
				ASSERT_EQ(first_line.at(0),	'2');
}


TEST_F	(VariableServerTest,	SendSieClassTopLevelObjects)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.send_sie_top_level_objects()\n";

				std::stringstream	received_file;
				std::string	file_temp;

				while	(socket.check_for_message_availible())	{
								socket	>>	file_temp;
								received_file	<<	file_temp;
				}

				std::string	first_line;
				std::getline(received_file,	first_line);

				//	We're	not	gonna	bother	comparing	the	contents
				//	Just	check	to	see	that	the	message	type	is	correct
				ASSERT_EQ(first_line.at(0),	'2');
}

TEST_F	(VariableServerTest,	RestartAndSet)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\n");

				EXPECT_EQ(reply,	expected);

				dump_checkpoint(socket,	"reload_file.ckpnt");

				socket	<<	"trick.var_add(\"vsx.vst.e\",\"m\")\n";
				socket	>>	reply;
				expected	=	std::string("0\t-1234\t-123456	{m}\n");

				socket	<<	"trick.var_set(\"vsx.vst.c\",	5)\n";
				socket	>>	reply;
				expected	=	std::string("0\t5\t-123456	{m}\n");

				EXPECT_EQ(reply,	expected);

				load_checkpoint(socket,	"RUN_test/reload_file.ckpnt");

				socket	>>	reply;
				expected	=	std::string("0\t-1234\t-123456	{m}\n");

				EXPECT_EQ(reply,	expected);
}

TEST_F	(VariableServerTest,	Cycle)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				double	cycle	=	1.0;
				int	num_cycles	=	5;

				//	Challenge:	no	loops	allowed
				//	I've	been	reading	about	lamdbas	and	when	you	have	a	hammer........

				//	Test:	compare	the	differences	in	the	returned	sim	time,	make	sure	the	difference	
				//	between	them	are	equal	to	var_cycle
				//	In	copy	mode	VS_COPY_SCHEDULED	and	write	mode	VS_WRITE_WHEN_COPY,	we	should	see	exactly	the	correct	copy	rate
				//	Use	a	very	small	tolerance	to	compensate	for	floating	point	error

				auto	parse_message_for_sim_time	=	[](const	std::string&	message)	{
								//	For	this	case	the	message	will	be
								//	0\t<time>	{s}\t
								//	We	only	care	about	<time>
								std::stringstream	message_stream(message);
								std::string	token;
								std::getline(message_stream,	token,	'\t');
								std::getline(message_stream,	token,	'	');
								return	std::stod(token);
				};

				//	Tail	recursion,	just	for	fun
				std::function<void(int,	double)>	compare_cycle	=	[&]	(int	n_cycles,	double	last_sim_time)	{
								if	(n_cycles	<=	0)
												return;

								double	sim_time	=	parse_message_for_sim_time(socket.receive());
								EXPECT_FEQ((sim_time	-	last_sim_time),	cycle)	<<	"Expected	cycle:	"	<<	cycle	<<	"	Measured	cycle:	"	<<	(sim_time	-	last_sim_time);
								compare_cycle(n_cycles-1,	sim_time);
				};

				auto	run_cycle_test	=	[&]	()	{
								std::string	command	=	"trick.var_cycle("	+	std::to_string(cycle)	+	")\n";
								socket	<<	command;
								//	Give	it	a	cycle	to	update
								socket.receive();
								socket.receive();
								double	sim_time	=	parse_message_for_sim_time(socket.receive());
								compare_cycle(num_cycles,	sim_time);
				};

				std::string	command	=	"trick.var_set_copy_mode(1)\ntrick.var_set_write_mode(1)\ntrick.var_add(\"time\")\n";
				socket	<<	command;

				cycle	=	0.1;
				num_cycles	=	5;
				run_cycle_test();

				cycle	=	0.5;
				run_cycle_test();

				cycle	=	3.0;
				num_cycles	=	3;
				run_cycle_test();
}


TEST_F	(VariableServerTest,	Pause)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}
				
				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.f\")\ntrick.var_add(\"vsx.vst.i\")\ntrick.var_pause()\n";

				//	Make	sure	it's	paused
				EXPECT_EQ(socket.check_for_message_availible(),	false);

				socket	<<	"trick.var_send()\n";
				socket	>>	reply;
				expected	=	std::string("0		123456		1234.5677");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);

				//	Make	sure	it	doesn't	send	cyclically
				EXPECT_EQ(socket.check_for_message_availible(),	false);

				socket	<<	"trick.var_unpause()\n";
				socket	>>	reply;
				expected	=	std::string("0		123456		1234.5677");

				EXPECT_EQ(strcmp_IgnoringWhiteSpace(reply,	expected),	0);
}

#ifndef	__APPLE__

TEST_F	(VariableServerTest,	Multicast)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.var_server_set_user_tag(\"VSTestServer\")\n";

				Socket	multicast_socket;
				if	(multicast_socket.init_multicast("224.3.14.15",	9265)	!=	0)	{
								FAIL()	<<	"Multicast	Socket	failed	to	initialize.";
				}

				int	max_multicast_tries	=	10000;
				int	tries	=	0;
				bool	found	=	false;

				char	expected_hostname[80];
				gethostname(expected_hostname,	80);
				int	expected_port	=	40000;
				
				//	get	expected	username
				struct	passwd	*passp	=	getpwuid(getuid())	;
				char	*	expected_username;
				if	(	passp	==	NULL	)	{
								expected_username	=	strdup("unknown")	;
				}	else	{
								expected_username	=	strdup(passp->pw_name)	;
				}
				
				//	Don't	care	about	PID,	just	check	that	it's	>	0
				char	*	expected_sim_dir	=	"trick/test/SIM_test_varserv";				//	Compare	against	the	end	of	the	string	for	this	one
				//	Don't	care	about	cmdline	name
				char	*	expected_input_file	=	"RUN_test/unit_test.py";
				//	Don't	care	about	trick_version
				char	*	expected_tag	=	"VSTestServer";

				//	Variables	to	be	populated	by	the	multicast	message
				char	actual_hostname[80];
				unsigned	short	actual_port	=	0;
				char	actual_username[80];
				int	actual_pid	=	0;
				char	actual_sim_dir[80];
				char	actual_cmdline_name[80];
				char	actual_input_file[80];
				char	actual_trick_version[80];
				char	actual_tag[80];
				unsigned	short	actual_duplicate_port	=	0;

				while	(!found	&&	tries++	<	max_multicast_tries)	{
								std::string	broadcast_data	=	multicast_socket.receive();
								sscanf(broadcast_data.c_str(),	"%s\t%hu\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%hu\n"	,	actual_hostname,	&actual_port	,
													actual_username	,	&actual_pid	,	actual_sim_dir	,	actual_cmdline_name	,
													actual_input_file	,	actual_trick_version	,	actual_tag,	&actual_duplicate_port)	;
								
								if	(strcmp(actual_hostname,	expected_hostname)	==	0	&&	strcmp(expected_tag,	actual_tag)	==	0)	{
												found	=	true;
												EXPECT_STREQ(actual_hostname,	expected_hostname);
												EXPECT_EQ(actual_port,	expected_port);
												EXPECT_STREQ(actual_username,	expected_username);
												EXPECT_GT(actual_pid,	0);
												std::string	expected_sim_dir_str(expected_sim_dir);
												std::string	actual_sim_dir_str(actual_sim_dir);
												std::string	end_of_actual	=	actual_sim_dir_str.substr(actual_sim_dir_str.length()	-	expected_sim_dir_str.length(),	actual_sim_dir_str.length());								
												EXPECT_EQ(expected_sim_dir_str,	end_of_actual);
												EXPECT_STREQ(actual_input_file,	expected_input_file);
												EXPECT_STREQ(actual_tag,	expected_tag);
												EXPECT_EQ(actual_duplicate_port,	expected_port);
								}
				}

				if	(!found)
								FAIL()	<<	"Multicast	message	never	received";
}

#endif

TEST_F	(VariableServerTest,	Freeze)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}
				
				std::string	reply;
				std::string	expected;
				int	mode;
				long	long	frame_count;
				long	long	freeze_frame_count;

				//	lambda	capture	by	refence	is	neat
				auto	parse_message_for_sim_mode_and_frames	=	[&](const	std::string&	message)	{
								//	For	this	case	the	message	will	be
								//	0\t<mode>\t<frame_count>\t<freeze_frame_count>\t
								//	Sets	the	local	variables	to	new	values
								std::stringstream	message_stream(message);
								std::string	token;
								std::getline(message_stream,	token,	'\t');
								std::getline(message_stream,	token,	'\t');
								mode	=	std::stoi(token);
								std::getline(message_stream,	token,	'\t');
								frame_count	=	std::stoll(token);
								std::getline(message_stream,	token,	'\t');
								freeze_frame_count	=	std::stoll(token);
				};

				auto	wait_for_mode_change	=	[&]	(int	expected_mode,	int	max_wait_iterations	=	10)	{
								int	iteration	=	0;
								while	(iteration++	<	max_wait_iterations	&&	mode	!=	expected_mode)	{
												parse_message_for_sim_mode_and_frames(socket.receive());
								}
				};

				//	Send	the	sim	mode,	frame_count,	freeze_frame_count,	and	sim_time
				socket	<<	"trick.var_add(\"trick_sys.sched.mode\")\ntrick.var_add(\"trick_sys.sched.frame_count\")\ntrick.var_add(\"trick_sys.sched.freeze_frame_count\")\n";
				parse_message_for_sim_mode_and_frames(socket.receive());

				//	Sim	mode	should	be	MODE_RUN	==	5
				EXPECT_EQ(mode,	MODE_RUN);

				//	Set	to	Freeze	mode
				socket	<<	"trick.exec_freeze()\n";

				//	The	mode	takes	a	little	bit	of	time	to	change
				wait_for_mode_change(MODE_FREEZE);

				//	Sim	mode	should	be	MODE_FREEZE	==	1
				ASSERT_EQ(mode,	MODE_FREEZE);

				//	Set	to	back	to	run	mode
				socket	<<	"trick.exec_run()\n";

				//	The	mode	takes	a	little	bit	of	time	to	change
				wait_for_mode_change(MODE_RUN);

				//	Sim	mode	should	be	MODE_RUN
				ASSERT_EQ(mode,	MODE_RUN);

				//	Test	VS_COPY_SCHEDULED	-	should	see	1	message	per	frame
				socket	<<	"trick.var_set_copy_mode(1)\n";


				//	Change	back	to	freeze	mode
				//	Set	to	Freeze	mode
				socket	<<	"trick.exec_freeze()\n";

				//	The	mode	takes	a	little	bit	of	time	to	change
				wait_for_mode_change(MODE_FREEZE);

				//	Sim	mode	should	be	MODE_FREEZE	==	1
				ASSERT_EQ(mode,	MODE_FREEZE);

				//	Test	VS_COPY_SCHEDULED	-	should	see	1	message	per	frame
				socket	<<	"trick.var_set_copy_mode(1)\n";
				parse_message_for_sim_mode_and_frames(socket.receive());
				int	num_tests	=	5;
				int	prev_frame	=	0;
				for	(int	i	=	0;	i	<	num_tests;	i++)	{
								prev_frame	=	freeze_frame_count;
								parse_message_for_sim_mode_and_frames(socket.receive());
								EXPECT_EQ(freeze_frame_count	-	prev_frame,	1);
				}


				//	Test	VS_COPY_TOP_OF_FRAME,	along	with	freeze	frame	multiplier	and	offset
				socket	<<	"trick.var_set_copy_mode(2)\ntrick.var_set_freeze_frame_multiple(3)\ntrick.var_set_freeze_frame_offset(1)\n";
				parse_message_for_sim_mode_and_frames(socket.receive());
				for	(int	i	=	0;	i	<	num_tests;	i++)	{
								prev_frame	=	freeze_frame_count;
								parse_message_for_sim_mode_and_frames(socket.receive());
								EXPECT_EQ(freeze_frame_count	%	3,	1);
								EXPECT_EQ(freeze_frame_count	-	prev_frame,	3);
				}
				
				//	Set	the	mode	back	to	run	or	the	next	tests	will	get	confused
				socket	<<	"trick.exec_run()\n";
				socket	<<	"trick.var_set_copy_mode(0)\n";

				//	Wait	for	the	mode	to	actually	change
				wait_for_mode_change(MODE_RUN);

				ASSERT_EQ(mode,	MODE_RUN);
}

TEST_F	(VariableServerTest,	CopyAndWriteModes)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				int	num_tests	=	5;
				double	sim_time;
				int	frame_count;
				std::string	vars;
				std::string	command;
				std::string	test_vars_command	=	"trick.var_add(\"time\")\ntrick.var_add(\"trick_sys.sched.frame_count\")\n";

				double	expected_cycle	=	0.5;
				command	=	"trick.var_cycle("	+	std::to_string(expected_cycle)	+	")\n";
				socket	<<	command;

				auto	parse_message	=	[&](const	std::string&	message)	{
								//	For	this	case	the	message	will	be
								//	0\t<sim_time>	{s}\t<frame_count>\t<other	variables>\n
								//	Sets	the	local	variables	to	new	values
								std::stringstream	message_stream(message);
								std::string	token;
								std::getline(message_stream,	token,	'\t');
								std::getline(message_stream,	token,	'	');
								sim_time	=	std::stod(token);
								std::getline(message_stream,	token,	'\t');
								std::getline(message_stream,	token,	'\t');
								frame_count	=	std::stoll(token);
								std::getline(message_stream,	token,	'\n');
								vars	=	token;
				};

				//	Check	that	every	combination	of	modes	is	functional
				//	Check	that	reasonable	times	and	frames	are	returned	as	well
				//	Default	is	VS_COPY_ASYNC=0	and	VS_WRITE_ASYNC=0
				//	This	mode	doesn't	give	us	any	guarantees	about	frame	or	cycle	consistency,	so	we	can't	test	it
				//	Just	make	sure	it	works

				command	=	test_vars_command	+	"trick.var_add(\"vsx.vst.a\")\ntrick.var_add(\"vsx.vst.b\")\n";
				socket	<<	command;
				parse_message(socket.receive());

				expected	=	"97	98";
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;

				//	Clear	out	anything	else	that's	been	sent
				socket	<<	"trick.var_pause()\n";
				socket.clear_buffered_data();

				//	Copy	mode	1	(VS_COPY_SCHEDULED)	write	mode	0	(VS_WRITE_ASYNC)
				command	=	"trick.var_clear()\n"	+	test_vars_command	+	"trick.var_sync(1)\ntrick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.d\")\ntrick.var_unpause()\n";
				socket	<<	command;

				//	With	copy	mode	VS_COPY_SCHEDULED	and	write	mode	VS_WRITE_ASYNC,	the	first	reply	will	be	all	0	since	the	main	time	to	copy	has	not	occurred	yet.
				//	Is	this	what	we	want?	Maybe	we	should	have	more	strict	communication	on	whether	the	data	has	been	staged	so	the	first	message	isn't	incorrect

				spin(socket);

				expected	=	"-1234	1234";
				parse_message(socket.receive());
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;


				//	Test	that	we	see	a	difference	of	exactly	expected_cycle	(with	a	tolerance	for	floating	point	issues)
				int	prev_frame	=	0;
				double	prev_time	=	0;
				for	(int	i	=	0;	i	<	num_tests;	i++)	{		
								prev_time	=	sim_time;
								parse_message(socket.receive());
								EXPECT_LT(fmod(sim_time	-	prev_time,	expected_cycle),	DOUBLE_TOL);
								EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;
				}

				//	Clear	out	anything	else	that's	been	sent
				socket	<<	"trick.var_pause()\n";
				socket.clear_buffered_data();


				//	Copy	mode	1	(VS_COPY_SCHEDULED)	write	mode	1	(VS_WRITE_WHEN_COPIED)
				command	=	"trick.var_clear()\n"	+	test_vars_command	+	"trick.var_set_write_mode(1)\ntrick.var_add(\"vsx.vst.e\")\ntrick.var_add(\"vsx.vst.f\")\ntrick.var_unpause()\n";
				socket	<<	command;

				spin(socket);

				parse_message(socket.receive());
				expected	=	"-123456	123456";
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;

				//	Test	that	we	still	see	a	difference	of	exactly	expected_cycle	(with	a	tolerance	for	floating	point	issues)
				prev_frame	=	0;
				prev_time	=	0;
				for	(int	i	=	0;	i	<	num_tests;	i++)	{		
								prev_time	=	sim_time;
								parse_message(socket.receive());
								EXPECT_LT(fmod(sim_time	-	prev_time,	expected_cycle),	DOUBLE_TOL);
								EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;
				}

				//	Clear	out	anything	else	that's	been	sent
				socket	<<	"trick.var_pause()\n";
				socket.clear_buffered_data();


				//	Copy	mode	2	(VS_COPY_TOP_OF_FRAME)	write	mode	0	(VS_WRITE_ASYNC)
				//	Test	frame_multiple	and	frame_offset	as	well,	with	stupid	values
				int	frame_multiple	=	13*5;
				int	frame_offset	=	7;
				command	=	"trick.var_clear()\n"	+	test_vars_command	+	"trick.var_set_copy_mode(2)\ntrick.var_set_write_mode(0)\ntrick.var_set_frame_multiple("	+	std::to_string(frame_multiple)	+	")\ntrick.var_set_frame_offset("	+	std::to_string(frame_offset)	+	")\ntrick.var_add(\"vsx.vst.g\")\ntrick.var_add(\"vsx.vst.h\")\ntrick.var_unpause()\n";
				socket	<<	command;

				//	Same	issue	as	copy	mode	1	write	mode	0
				spin(socket);
				parse_message(socket.receive());
				expected	=	"-1234567	123456789";
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;

				//	Test	that	we	see	a	difference	in	frame_count	of	exactly	frame_multiple	with	an	offset	of	frame_offset
				prev_frame	=	0;
				for	(int	i	=	0;	i	<	num_tests;	i++)	{		
								prev_frame	=	frame_count;
								parse_message(socket.receive());
								EXPECT_EQ(frame_count	-	prev_frame,	frame_multiple);
								EXPECT_EQ(frame_count	%	frame_multiple,	frame_offset);
								EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;
				}

				//	Clear	out	anything	else	that's	been	sent
				socket	<<	"trick.var_pause()\n";
				socket.clear_buffered_data();


				//	Copy	mode	2	(VS_COPY_TOP_OF_FRAME)	write	mode	1	(VS_WRITE_WHEN_COPIED)
				frame_multiple	=	17*5;
				frame_offset	=	11;
				command	=	"trick.var_clear()\n"	+	test_vars_command	+	"trick.var_set_copy_mode(2)\ntrick.var_set_write_mode(1)\ntrick.var_set_frame_multiple("	+	std::to_string(frame_multiple)	+	")\ntrick.var_set_frame_offset("	+	std::to_string(frame_offset)	+	")\ntrick.var_add(\"vsx.vst.i\")\ntrick.var_add(\"vsx.vst.j\")\ntrick.var_unpause()\n";
				socket	<<	command;

				spin(socket);

				expected	=	"1234.5677	-1234.56789";
				parse_message(socket.receive());
				EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;

				//	Test	that	we	see	a	difference	in	frame_count	of	exactly	frame_multiple	with	an	offset	of	frame_offset
				prev_frame	=	0;
				for	(int	i	=	0;	i	<	num_tests;	i++)	{		
								prev_frame	=	frame_count;
								parse_message(socket.receive());
								EXPECT_EQ(frame_count	-	prev_frame,	frame_multiple);
								EXPECT_EQ(frame_count	%	frame_multiple,	frame_offset);
								EXPECT_EQ(strcmp_IgnoringWhiteSpace(vars,	expected),	0)	<<	"Received:	"	<<	vars	<<	"	Expected:	"	<<	expected;
				}

				//	Clear	out	anything	else	that's	been	sent
				socket	<<	"trick.var_pause()\n";
				socket.clear_buffered_data();
}

TEST_F	(VariableServerTest,	var_set)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	reply;
				std::string	expected;

				socket	<<	"trick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.blocked_from_output\")\ntrick.var_add(\"vsx.vst.blocked_from_input\")\n";

				socket	>>	reply;
				expected	=	std::string("0\t-1234\t0\t500\n");

				EXPECT_EQ(reply,	expected);

				socket	<<	"trick.var_set(\"vsx.vst.blocked_from_input\",	0)\n";
				socket	<<	"trick.var_set(\"vsx.vst.blocked_from_output\",	0)\n";


}

bool	getCompleteBinaryMessage(ParsedBinaryMessage&	message,	Socket&	socket,	bool	print	=	false)	{
				static	const	int	max_retries	=	5;

				int	tries	=	0;
				bool	parse_success	=	false;
				std::vector<unsigned	char>	reply;

				//	Try	parsing	until	message	received	is	correct
				//	May	take	a	cycle	or	2	to	update	properly
				while	(!parse_success	&&	tries++	<	max_retries)	{
								try	{
												reply	=	socket.receive_bytes();
												message.parse(reply);
												parse_success	=	true;
								}	catch	(const	MalformedMessageException&	ex)	{	
												std::cout	<<	"Attempt	"	<<	tries	<<	"	failed	with	exception	"	<<	ex.what()	<<	std::endl;;
								}
				}

				if	(print)	{
								std::cout	<<	"Message:	";
								for	(unsigned	char	byte	:	reply)	{
												std::cout	<<	"0x"	<<	std::setw(2)	<<	std::setfill('0')	<<	std::hex	<<	(int)byte	<<	",	";
								}
								std::cout	<<	std::endl;
				}

				return	parse_success;
}

TEST_F	(VariableServerTest,	send_stdio)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.var_set_send_stdio(True)\n";
				socket	<<	"sys.stdout.write(\"This	message	should	redirect	to	varserver\")\n";

				std::string	message;
				socket	>>	message;

				std::stringstream	message_stream(message);
				std::string	token;

				int	message_type;
				int	stream_num;
				int	text_size;
				std::string	text;

				message_stream	>>	message_type;
				message_stream	>>	stream_num;
				std::getline(message_stream,	token,	'\n');
				text_size	=	stoi(token);
				std::getline(message_stream,	text);

				if	(text.size()	!=	text_size)	{
								socket	>>	text;
				}
				
				EXPECT_EQ	(message_type,	4);
				EXPECT_EQ	(stream_num,	1);
				EXPECT_EQ	(text_size,	41);

				EXPECT_EQ(text,	std::string("This	message	should	redirect	to	varserver"));
}


TEST_F	(VariableServerTest,	Binary)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				socket	<<	"trick.var_binary()\ntrick.var_add(\"vsx.vst.n\")\n";
				ParsedBinaryMessage	message_n;

				if	(!getCompleteBinaryMessage(message_n,	socket))	{
								FAIL()	<<	"Parser	was	not	able	to	interpret	the	message.";
				}

				Var	arr_var	=	message_n.getVariable("vsx.vst.n");
				EXPECT_EQ(arr_var.getArraySize(),	5);
				std::vector<unsigned	char>	raw_arr	=	arr_var.getRawBytes();
				std::vector<int>	values;
				for	(unsigned	int	i	=	0;	i	<	raw_arr.size();	i	+=	4)	{
								int	val	=	0;
								for	(unsigned	int	j	=	i;	j	<	i+4;	j++)	{
												val	|=	raw_arr[j]	<<	j*8;
								}
								values.push_back(val);
				}

				std::vector<int>	expected_arr	=	{0,	1,	2,	3,	4};
				EXPECT_EQ(values,	expected_arr);


				std::vector<unsigned	char>	reply;
				socket	<<	"trick.var_clear()\ntrick.var_add(\"vsx.vst.c\")\ntrick.var_add(\"vsx.vst.k\")\ntrick.var_add(\"vsx.vst.o\")\ntrick.var_add(\"vsx.vst.m\")\n";

				ParsedBinaryMessage	message;

				if	(!getCompleteBinaryMessage(message,	socket))	{
								FAIL()	<<	"Parser	was	not	able	to	interpret	the	message.";
				}

				EXPECT_EQ(message.getMessageType(),	0);
				ASSERT_EQ(message.getNumVars(),	4);

				EXPECT_EQ(strcmp(message.variables[0].getName().c_str(),	"vsx.vst.c"),	0);
				EXPECT_EQ(message.variables[0].getType(),	TRICK_SHORT);
				EXPECT_EQ(message.variables[0].getValue<short>(),	-1234);


				EXPECT_EQ(strcmp(message.variables[1].getName().c_str(),	"vsx.vst.k"),	0);
				EXPECT_EQ(message.variables[1].getType(),	TRICK_LONG_LONG);
				EXPECT_EQ(message.variables[1].getValue<long	long>(),	-12345678912345);

				EXPECT_EQ(strcmp(message.variables[2].getName().c_str(),	"vsx.vst.o"),	0);
				EXPECT_EQ(message.variables[2].getType(),	TRICK_STRING);
				std::string	actual	=	message.variables[2].getValue<std::string>();
				std::string	expected	=	"You	will	rejoice	to	hear	that	no	disaster	has	accompanied	the	commencement	of	an	enterprise	which	you	have	regarded	with	such	evil	forebodings.	I	arrived	here	yesterday,	and	my	first	task	is	to	assure	my	dear	sister	of	my	welfare	and	increasing	confidence	in	the	success	of	my	undertaking.\0";
				EXPECT_EQ(actual.substr(0,	actual.size()-1),	expected);

				Var	variable	=	message.getVariable("vsx.vst.m");
				EXPECT_EQ(strcmp(variable.getName().c_str(),	"vsx.vst.m"),	0);
				EXPECT_EQ(variable.getType(),	TRICK_BOOLEAN);
				EXPECT_EQ(variable.getValue<bool>(),	true);

				//	Switch	to	nonames
				socket	<<	"trick.var_binary_nonames()\n";

				ParsedBinaryMessage	noname_message(false,	true);
				if	(!getCompleteBinaryMessage(noname_message,	socket))	{
								FAIL()	<<	"Parser	was	not	able	to	interpret	the	message.";
				}

				EXPECT_EQ(noname_message.getMessageType(),	0);
				ASSERT_EQ(noname_message.getNumVars(),	4);

				EXPECT_EQ(strcmp(noname_message.variables[0].getName().c_str(),	"<no	name>"),	0);
				EXPECT_EQ(noname_message.variables[0].getType(),	TRICK_SHORT);
				EXPECT_EQ(noname_message.variables[0].getValue<short>(),	-1234);

				EXPECT_EQ(strcmp(noname_message.variables[1].getName().c_str(),	"<no	name>"),	0);
				EXPECT_EQ(noname_message.variables[1].getType(),	TRICK_LONG_LONG);
				EXPECT_EQ(noname_message.variables[1].getValue<long	long>(),	-12345678912345);

				EXPECT_EQ(strcmp(noname_message.variables[2].getName().c_str(),	"<no	name>"),	0);
				EXPECT_EQ(noname_message.variables[2].getType(),	TRICK_STRING);
				actual	=	noname_message.variables[2].getValue<std::string>();
				EXPECT_EQ(actual.substr(0,	actual.size()-1),	expected);

				Var	variable_noname	=	noname_message.getVariable(3);
				EXPECT_EQ(strcmp(variable_noname.getName().c_str(),	"<no	name>"),	0);
				EXPECT_EQ(variable_noname.getType(),	TRICK_BOOLEAN);
				EXPECT_EQ(variable_noname.getValue<bool>(),	true);
}

TEST_F	(VariableServerTest,	BinaryByteswap)	{

				std::vector<unsigned	char>	reply;
				socket	<<	"trick.var_binary()\ntrick.var_byteswap(False)\ntrick.var_add(\"vsx.vst.f\")\ntrick.var_add(\"vsx.vst.j\")\n";

				socket.receive_bytes();
				reply	=	socket.receive_bytes();

				//	Test	byteswap

				//	std::cout	<<	"Message:	";
				//	for	(unsigned	char	byte	:	reply)	{
				//					std::cout	<<	"0x"	<<	std::setw(2)	<<	std::setfill('0')	<<	std::hex	<<	(int)byte	<<	"	";
				//	}
				//	std::cout	<<	std::endl;

				ParsedBinaryMessage	message;

				try	{
								message.parse(reply);
				}	catch	(const	MalformedMessageException&	ex)	{	
								FAIL()	<<	"Parser	threw	an	exception:	"	<<	ex.what();
				}

				EXPECT_EQ(message.getMessageType(),	0);
				ASSERT_EQ(message.getNumVars(),	2);

				EXPECT_EQ(strcmp(message.variables[0].getName().c_str(),	"vsx.vst.f"),	0);
				EXPECT_EQ(message.variables[0].getType(),	TRICK_UNSIGNED_INTEGER);
				EXPECT_EQ(message.variables[0].getValue<unsigned	int>(),	123456);

				EXPECT_EQ(strcmp(message.variables[1].getName().c_str(),	"vsx.vst.j"),	0);
				EXPECT_EQ(message.variables[1].getType(),	TRICK_DOUBLE);
				EXPECT_EQ(message.variables[1].getValue<double>(),	-1234.567890);

				socket	<<	"trick.var_byteswap(True)\n";

				socket.receive_bytes();
				reply	=	socket.receive_bytes();

				//	std::cout	<<	"Message:	";
				//	for	(unsigned	char	byte	:	reply)	{
				//					std::cout	<<	"0x"	<<	std::setw(2)	<<	std::setfill('0')	<<	std::hex	<<	(int)byte	<<	"	";
				//	}
				//	std::cout	<<	std::endl;

				ParsedBinaryMessage	byteswap_message(true,	false);

				try	{
								byteswap_message.parse(reply);
				}	catch	(const	MalformedMessageException&	ex)	{	
								FAIL()	<<	"Parser	threw	an	exception:	"	<<	ex.what();
				}

				EXPECT_EQ(byteswap_message.getMessageType(),	0);
				ASSERT_EQ(byteswap_message.getNumVars(),	2);

				EXPECT_EQ(strcmp(byteswap_message.variables[0].getName().c_str(),	"vsx.vst.f"),	0);
				EXPECT_EQ(byteswap_message.variables[0].getType(),	TRICK_UNSIGNED_INTEGER);
				EXPECT_EQ(byteswap_message.variables[0].getValue<unsigned	int>(),	123456);

				EXPECT_EQ(strcmp(byteswap_message.variables[1].getName().c_str(),	"vsx.vst.j"),	0);
				EXPECT_EQ(byteswap_message.variables[1].getType(),	TRICK_DOUBLE);
				EXPECT_EQ(byteswap_message.variables[1].getValue<double>(),	-1234.567890);
}


int	main(int	argc,	char	**argv)	{
				::testing::InitGoogleTest(&argc,	argv);
				int	result	=	RUN_ALL_TESTS();

				Socket	socket;
				socket.init("localhost",	40000);
				
				if	(result	==	0)	{
								//	Success
								std::cout	<<	"Exiting	peacefully!"	<<	std::endl;
								socket	<<	"vsx.vst.success()	\n";
				}	else	{
								//	Failure
								std::cout	<<	"Exiting	with	failure!"	<<	std::endl;
								socket	<<	"vsx.vst.fail()	\n";
				}
				socket	<<	"trick.stop()	\n";

				return	result;
}