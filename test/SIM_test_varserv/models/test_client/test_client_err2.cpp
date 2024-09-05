#include	"test_client.hh"

int	VariableServerTest::numSession	=	0;


TEST_F	(VariableServerTest,	SendStdException)	{
				if	(socket_status	!=	0)	{
								FAIL();
				}

				std::string	command	=	"vsx.vst.throw_exception()\n";
				socket	<<	command;
}
