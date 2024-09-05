
#include	"trick/MonteCarlo.hh"
#include	"trick/SysThread.hh"


/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::slave_shutdown()	{
				/**	<ul><li>	Kill	any	active	child	process	executing	a	run.	*/
				slave_kill_run();
				/**	<li>	Run	the	shutdown	jobs	and	exit.	*/
				run_queue(&slave_shutdown_queue,	"in	slave_shutdown	queue")	;

				SysThread::ensureAllShutdown();

				exit(0);
}

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::slave_die()	{
				/**	<ul><li>	Kill	any	active	child	process	executing	a	run	and	exit	immediately.	*/
				slave_kill_run();

				SysThread::ensureAllShutdown();

				exit(1);
}

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::slave_kill_run()	{
				/**
					*	The	child	process,	if	running,	has	a	group	ID	equal	to	the	parent's	process	ID.	Sending	a	kill	signal	to	this	ID	will
					*	signal	both	the	child	and	the	parent,	so	ignore	it	in	the	parent,	and	restore	the	current	signal	handler	afterward.
					*/
				struct	sigaction	ignore,	restore;
				ignore.sa_handler	=	SIG_IGN;
				sigaction(SIGTERM,	&ignore,	&restore);
				kill(getpid(),	SIGTERM);
				sigaction(SIGTERM,	&restore,	NULL);
}

/**	@par	Detailed	Design:	*/
void	Trick::MonteSlave::set_S_main_name(std::string	name)	{
				S_main_name	=	name;
}
