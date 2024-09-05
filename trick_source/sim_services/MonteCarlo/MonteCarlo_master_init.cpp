
#include	<sys/stat.h>
#include	<libgen.h>

#include	"trick/MonteCarlo.hh"
#include	"trick/message_proto.h"

/**	@par	Detailed	Design:	*/
int	Trick::MonteCarlo::master_init()	{

				/**	<ul><li>	Construct	the	run	directory.	*/
				if	(construct_run_directory()	==	-1)	return	-1	;

				/**	<ul><li>	Create	the	runs	and	header	files	and	open	them	for	writing.	*/
				std::string	file_name	=	run_directory+"/monte_header"	;
				if	(open_file(file_name,	&run_header_file)==-1)	return	-1	;

				file_name	=	run_directory+"/monte_runs"	;
				if	(open_file(file_name,	&run_data_file)==-1)	return	-1	;

				write_to_run_files(file_name)	;

				/**	<li>	If	this	is	a	dry	run	return	else	initialize	sockets:	*/
				if	(dry_run)	{
							return	0	;
				}

				return	initialize_sockets()	;
}
