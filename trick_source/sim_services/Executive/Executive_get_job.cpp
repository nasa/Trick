
#include	<iostream>

#include	"trick/Executive.hh"

Trick::JobData	*	Trick::Executive::get_job(std::string	job_name,	unsigned	int	j_instance)	{

				unsigned	int	ii	;
				std::multimap<std::string	,	Trick::JobData	*>::iterator	it	;
				std::pair<std::multimap<std::string	,	Trick::JobData	*>::iterator	,	std::multimap<std::string	,	Trick::JobData	*>::iterator>	range	;

				/**	@par	Detailed	Design	*/

				/**	@li	Find	all	jobs	that	have	the	target	name	and	hold	them	in	a	list	*/
				range	=	all_jobs.equal_range(job_name)	;

				/**	@li	Move	to	the	j_instance-th	item	in	the	list	*/
				for	(	it	=	range.first	,	ii	=	1	;	(it	!=	range.second)	&&	(ii	<	j_instance)	;	++it	,	ii++	)	;

				/**	@li	If	the	j_instance-th	item	doesn't	exist	return	NULL	*/
				if	(	it	==	range.second	)	{
								return(NULL)	;
				}

				/**	@li	return	the	j_instance-th	item	*/
				return((*it).second)	;

}

