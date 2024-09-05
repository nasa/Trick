
#include	<iostream>

#include	"trick/Executive.hh"

/**
@details
-#	Gets	the	size	of	the	class	map.
-#	Add	the	incoming	job	class	string	as	a	key	to	the	class_job	map.	Assign	it	the	value	of
			of	(	scheduled_job_class_base	+	size_of_map	)
	*/
int	Trick::Executive::add_scheduled_job_class(	std::string	in_name	)	{

				unsigned	int	index	;

				if	(	in_name.empty()	)	{
								return(-1)	;
				}

				if	(	class_map.find(in_name)	==	class_map.end()	)	{
								index	=	scheduled_start_index	+	num_scheduled_job_classes++	;
								class_map[in_name]	=	index	;
								class_to_queue[index]	=	&scheduled_queue	;
				}

				return(0)	;

}

