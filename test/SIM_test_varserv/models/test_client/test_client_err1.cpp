#include	"test_client.hh"

int	VariableServerTest::numSession	=	0;


TEST_F	(VariableServerTest,	SendExecutiveException)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	command	=	"trick.exec_terminate_with_return(1,	\""	+	std::string(__FILE__)	+	"\",	"	+	std::to_string(__LINE__)	+	",	\"Error	termination	for	testing\")\n";
				socket	<<	command;
}
