
#include	<sstream>
#include	<iomanip>
#include	<fstream>
#include	<sys/stat.h>
#include	<unistd.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/message_proto.h"
#include	"trick/message_type.h"
#include	"trick/command_line_protos.h"

/**	@par	Detailed	Design:	*/
void	Trick::MonteCarlo::dryrun()	{

				/**	<ul><li>	For	all	runs:	run	the	pre	run	jobs.	*/
				MonteRun	*	curr_run;
				while	((curr_run	=	get_next_dispatch()))	{
								prepare_run(curr_run);

								//	Write	out	the	input	file	for	this	run.
								std::stringstream	buffer_stream;
								buffer_stream	<<	run_directory	<<	"/RUN_"	<<	std::setw(5)	<<	std::setfill('0')	<<	curr_run->id;
								std::ofstream	of	;
								if	(	(access(buffer_stream.str().c_str(),	F_OK)	==	0)	or	(mkdir(buffer_stream.str().c_str(),	0755)	==	0)	)	{
												of.open(std::string(buffer_stream.str()	+	"/monte_input").c_str())	;
												of	<<	"#	This	run	can	be	executed	in	stand	alone	(non-Monte	Carlo)	mode	by	running\n"
													"#	the	S_main	executable	with	this	file	specified	as	the	input	file.\n\n"	;
												of	<<	"if	(sys.version_info	>	(3,	0)):\n";
												of	<<	"				exec(open(\""	<<	command_line_args_get_input_file()	<<	"\").read())\n";
												of	<<	"else:\n";
												of	<<	"				execfile(\""	<<	command_line_args_get_input_file()	<<	"\")\n\n";
												of	<<	"trick.mc_set_enabled(0)\n";
												for	(std::vector<std::string>::size_type	j	=	0;	j	<	curr_run->variables.size();	++j)	{
																of	<<	curr_run->variables[j]	<<	"\n";
												}
												of	<<	"trick.set_output_dir(\""	<<	buffer_stream.str()	<<	"\")\n";
												of	<<	"trick.mc_set_current_run("	<<	curr_run->id	<<	")"	<<	std::endl	;
												of.close()	;
								}
				}

				/**	<ul><li>	Run	the	master	shutdown	jobs	*/
				run_queue(&master_shutdown_queue,	"in	master_shutdown	queue")	;

				message_publish(MSG_INFO,	"Monte	[Master]	Dry	run	complete.\n")	;

				exit(0)	;
}
