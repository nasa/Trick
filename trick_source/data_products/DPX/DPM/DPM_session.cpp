
#include	<string.h>

#include	"DPM/DPM_session.hh"
#include	"float.h"
#include	<libxml/parser.h>
#include	<libxml/tree.h>

int	DPM_session::Initialize(	xmlNode	*SessionNode	)	{

				double	tstart				=	0.0;
				double	tstop					=	DBL_MAX;
				double	frequency	=	DBL_MAX;

				int	n_product_files,	n_runs;

				if	(SessionNode	==	NULL)	{
									std::cerr	<<	"ERROR:	DPM_session::Initialize:	Bad	parameters."	<<	std::endl;
									return	-1;
				}

				if	(strcmp((char*)	SessionNode->name,	"session")	!=	0)	{
								std::cerr	<<	"ERROR:	Expected	<session>	specification	but	alas	didnt	find	it."	<<	std::endl;
								return	-1;
				}

				if	(	!strcasecmp(AttributeValue("presentation")	,	"stream"	)	)	{
								if	(	AttributeValue("machine")	==	NULL	)	{
												std::cerr	<<	"ERROR:	Expected	attribute	\"machine\"	when	using	stream	presentation"	<<	std::endl;
												return	-1;
								}
								if	(	AttributeValue("port")	==	NULL	)	{
												std::cerr	<<	"ERROR:	Expected	attribute	\"port\"	when	using	stream	presentation"	<<	std::endl;
												return	-1;
								}
				}

				if	(AttributeValue("device")	!=	NULL)	{
								if	(setDevice((char*)AttributeValue("device"))	<	0)	{
												return	-1;
								}
				}	else	{
								sessionDevice	=	DPM_session::TERMINAL;
				}



				/*	Process	children	*/
				xmlNode	*current_node	=	SessionNode->children;

				while	(current_node	!=	NULL)	{
								if	(current_node->type	==	XML_ELEMENT_NODE)	{
												if	(	strcmp(	(char	*)current_node->name,	"run")	==	0)	{
																DPM_run	*run;
																try	{
																				run	=	new	DPM_run(	this,	current_node);
																}	catch	(std::invalid_argument)	{
																				run	=	NULL;
																				std::cerr	<<	"ERROR:	one	of	this	<session>s	<run>s	is	invalid."	<<	std::endl;
																}
																if	(run	!=	NULL)	{
																				run_list.push_back(	run);
																}
												}	else	if	(	strcmp(	(char*)current_node->name,	"product_files")	==	0)	{
																xmlNode	*current_child_node;
																current_child_node	=	current_node->children;
																while	(current_child_node	!=	NULL)	{
																				if	(current_child_node->type	==	XML_ELEMENT_NODE)	{
																								if	(	strcmp(	(const	char	*)current_child_node->name,	"file")	==	0)	{
																												char	*product_file;
																												xmlNode	*text_node	=	current_child_node->children;
																												product_file	=	strdup(	(const	char	*)text_node->content);
																												if	(product_file	!=	NULL)	{
																																product_file_list.push_back(	product_file);
																												}
																								}	else	{
																												std::cerr	<<	"WARNING:	<product_files>	specification	contains	an	unknown	element	\"<"	<<
																																				current_child_node->name	<<	">\".	Skipping."	<<	std::endl;
																								}
																				}
																				current_child_node	=	current_child_node->next;
																}
												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"tstart")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																tstart	=	strtod((const	char	*)text_node->content,(char	**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"tstop")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																tstop	=	strtod((const	char	*)text_node->content,(char	**)NULL);

												}	else	if	(	strcmp(	(const	char	*)current_node->name,	"frequency")	==	0)	{
																xmlNode	*text_node	=	current_node->children;
																frequency	=	strtod((const	char	*)text_node->content,(char**)NULL);

												}	else	{
																std::cerr	<<	"WARNING:	<session>	specification	contains	an	unknown	element	\"<"	<<
																								current_node->name	<<	">\".	Skipping."	<<	std::endl;
												}
								}
								current_node	=	current_node->next;
				}
				time_constraints	=	new	DPM_time_constraints(tstart,	tstop,	frequency);

				//	Validation	criteria:
				//	1.	Session	requires	at	least	one	RUN	directory.
				//	2.	Session	requires	at	least	one	Product	file.

				n_product_files	=	(int)product_file_list.size();
				if	(n_product_files	<	1)	{
								std::cerr	<<	"ERROR:	<session>	requires	at	least	one	product	file."	<<	std::endl;
								return	-1;
				}

				n_runs	=	(int)run_list.size();
				if	(n_runs	<	1)	{
								std::cerr	<<	"ERROR:	<session>	requires	at	least	one	RUN."	<<	std::endl;
								return	-1;
				}

				return	0;
}

DPM_session::DPM_session(DPM_component	*Parent,	xmlNode	*SessionNode)
	:	DPM_component(Parent,	SessionNode)	{

		if	(	Initialize(	SessionNode)	<	0)	{
						throw	std::invalid_argument("<session>	specification	is	invalid	and	un-usable.");
		}
}

DPM_session::~DPM_session()	{
				int	i,	n_product_files,	n_runs	;

				n_product_files	=	(int)product_file_list.size();
				for	(i	=	0;	i	<	n_product_files	;	i++)	{
								char	*	product_file;
								product_file	=		product_file_list[i];
								if	(product_file)	{	free(	product_file);	}
				}

				n_runs	=	(int)run_list.size();
				for	(i	=	0;	i	<	n_runs	;	i++)	{
								delete	run_list[i];
				}
				delete	time_constraints;

				if	(deviceFileName)	{
								free(deviceFileName);
				}
}


std::ostream&	operator<<	(std::ostream&	s,	const	DPM_session	*session)	{
		int	i;
		int	n_attrs	=	(int)session->attribute_list.size();
		int	n_runs		=	(int)session->run_list.size();
		int	n_product_files	=	(int)session->product_file_list.size();

		s	<<	"<session";
		//	Output	the	axis	element	attributes.
		for	(i=0	;	i<n_attrs	;	i++)	{
				s	<<	"	"	<<	session->attribute_list[i];
		}
		s	<<	">"	<<	std::endl;
		s	<<	"<tstart>"	<<	session->time_constraints->getStart()	<<	"</tstart>"	<<	std::endl;
		s	<<	"<tstop>"		<<	session->time_constraints->getStop()		<<	"</tstop>"		<<	std::endl;
		s	<<	"<frequency>"		<<	session->time_constraints->getFrequency()		<<	"</frequency>"		<<	std::endl;

		for	(i=0	;	i<n_runs	;	i++)	{
						s	<<	"<run>"	<<	std::endl;
						s	<<	session->run_list[i];
						s	<<	"</run>"	<<	std::endl;
		}

		s	<<	"<product_files>"	<<	std::endl;
		for	(i=0	;	i<n_product_files	;	i++)	{
						s	<<	"<file>"	<<	std::endl;
						s	<<	session->product_file_list[i];
						s	<<	"</file>"	<<	std::endl;
		}
		s	<<	"</product_files>"	<<	std::endl;
		s	<<	std::endl;
		s.flush();

		return	s;
}

int	DPM_session::setDevice(char*	device)	{

				if	(	device	==	NULL	)	{
								std::cerr	<<	"ERROR:	NULL	device	spec	given	to	Session::setDevice()"	<<	std::endl;
								return(-1);
				}

				if	(	!strcasecmp(device,	"terminal")	)	{
								sessionDevice=	DPM_session::TERMINAL	;
				}	else	if	(	!strcasecmp(device,	"printer")	)	{
								sessionDevice	=	DPM_session::PRINTER	;
				}	else	{
								char	tmp[5];
								strncpy(tmp,	device,	5);
								if	(	!strcasecmp(tmp,	"file:"))	{
												sessionDevice	=	DPM_session::FILE	;
												device	+=	5;
												deviceFileName	=	strdup(device);
								}	else	{
												std::cerr	<<	"ERROR:	Bad	device	spec:	\""	<<	device	<<	"\""	<<	std::endl;
												std::cerr	<<	"Correct	device	spec	format	(case-insensitive):	Printer|Terminal|File:<file_name>\n";
												return(-1);
								}
				}

				return(0)	;
}

DPM_session::Device	DPM_session::getDevice()	{
				return	sessionDevice;
}

char*	DPM_session::getDeviceFileName()	{
				return	deviceFileName;
}
