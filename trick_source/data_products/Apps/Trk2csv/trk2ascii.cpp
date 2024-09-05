
#include	<vector>
#include	<iostream>
#include	"Log/TrickBinary.hh"
#include	<string.h>
#include	<stdlib.h>

static	const	char	*usage_doc[]	=	{
"----------------------------------------------------------------------------",
"	trk2ascii	-																																																																",
"																																																																												",
"	USAGE:		trk2ascii	<ascii_format	[output_file_name]>	<trk_file_name>	[args]	",
"	Options:																																																																			",
"					-help																Print	this	message	and	exit.																						",
"					-csv,	-ascii									Generates	a	comma-separated	value	(CSV)	file	from	",
"																										a	Trick	binary	data	file.	CSV	files	are	a	common		",
"																										means	of	sharing	data	between	applications.							",
"					-fix																	Generatse	a	fixed	cloumn	width	ascii	file	from	a		",
"																										Trick	binary	data	files																												",
"					-xml																	Generates	an	extensible	markup	language	(XML)					",
"																										file	from	a	Trick	binary	data	file.	XML	files	may	",
"																										be	used	for	sharing	data	between	NExIOM	devices.		",
"					delimiter=\"<_delimit_string_>\"																																									",
"																										Change	the	default	delimiter	used	in	'csv'	&	'fix'",
"																										ascii	formats	from	comma	separated	\",\"	to	another	",
"																										string	(Note:	do	not	use	spaces	before	quotes).			",
"																																																																												",
"----------------------------------------------------------------------------"};
#define	N_USAGE_LINES	(sizeof(usage_doc)/sizeof(usage_doc[0]))

void	print_doc(char	*doc[],	int	nlines)	{
				int	i;
				for	(i=0;	i	<	nlines;	i++)	{
								cerr	<<	doc[i]	<<	'\n';
				}
				cerr.flush();
}

void	usage()	{
				print_doc((char	**)usage_doc,N_USAGE_LINES);
}

int	main(int	argc,	char*	argv[])
{
				double	t,	y	;
				char	*trk_file_name	=	NULL;
				char	*ascii_file_name	=	NULL;
				FILE	*fp;

				int	number_of_parameters;
				char	**	param_names;
				char	**	param_units;
				enum	{CSV,	FIX,	XML};
				int	Format=0;		/*	default	to	csv	*/
				string	delimiter(",");		/*	default	delimter	*/

				int	i,	done;
				char	*prog_name	=	argv[0];

				DataStream*	each_ds;
				vector	<DataStream*>	ds_list;
				//	idx	is	used	when	comparing	with	the	size()	of	the	vector
				vector	<DataStream*>::size_type	idx;

				if	(argc	<=	1	)	{
								cerr	<<	prog_name	<<	":	No	arguments	were	supplied.\n";
								cerr.flush();
								usage();
								exit(EXIT_FAILURE);
				}	else	{
								int	i	=	1;
								string	option,	next_option;
								int	found_it;
								while	(	i	<	argc	)	{
												option	=	argv[i++];
												if	(i<argc)	next_option	=	argv[i];

												if	(option	==	"-help")	{
																usage();
																exit(EXIT_SUCCESS);
												}	else	if	(option	==	"-csv"		||		option	==	"-ascii")	{
																Format	=	CSV;
																if	(i<argc		&&		(next_option.find(".trk")	==	string::npos))	{
																				ascii_file_name	=	argv[i++];
																}
												}	else	if	(option	==	"-fix")	{
																Format	=	FIX;
																if	(i<argc		&&		(next_option.find(".trk")	==	string::npos))	{
																				ascii_file_name	=	argv[i++];
																}
												}	else	if	(option	==	"-xml")	{
																Format	=	XML;
																if	(i<argc		&&		(next_option.find(".trk")	==	string::npos))	{
																				ascii_file_name	=	argv[i++];
																}
												}	else	if	(option.find(".trk")	!=	string::npos)	{
																trk_file_name	=	strdup(	option.c_str()	);
												}	else	if	(option.find("delim")	!=	string::npos)	{
																found_it	=	option.find_first_of("=");
																if	(found_it>=0)	{
																				delimiter	=	option.substr(found_it+1);
																}
												}	else	{
																cerr	<<	"\""	<<	option.c_str()	<<	"\"	is	not	a	valid	option.\n";
																cerr.flush();
																usage();
																exit(EXIT_FAILURE);
												}
								}
				}

				if	(trk_file_name	==	NULL)	{
								cerr	<<	prog_name	<<	":	No	Trick	binary	data	file	name	was	supplied.\n";
								cerr.flush();
								usage();
								exit(EXIT_FAILURE);
				}

				if	(ascii_file_name	!=	NULL)	{
								if	((	fp	=	fopen(ascii_file_name,	"w")	)	==	0)	{
												cerr	<<	"Couldn't	open	\"	<<	ascii_file_name	<<	\"	for	writing\n";
												cerr.flush();
												exit(EXIT_FAILURE);
								}
				}	else	{
								fp	=	stdout;
				}

				number_of_parameters	=	TrickBinaryGetNumVariables(trk_file_name);

				if	(number_of_parameters	==	0)	{
								cerr	<<	"No	parameters	found	in	the	Trk	data	log	file.\n";
								cerr.flush();
								exit(EXIT_FAILURE);
				}

				if	((	param_names	=	TrickBinaryGetVariableNames(trk_file_name))	==	NULL	)	{
								cerr	<<	"Unable	get	parameter	names	from	the	Trk	data	log	file.\n";
								cerr.flush();
								exit(EXIT_FAILURE);
				}

				if	((	param_units	=	TrickBinaryGetVariableUnits(trk_file_name))	==	NULL	)	{
								cerr	<<	"Unable	get	parameter	units	from	the	Trk	data	log	file.\n";
								cerr.flush();
								exit(EXIT_FAILURE);
				}

				string	ascii_title("Results");
				if	(ascii_file_name	!=	NULL)	{
								ascii_title	=	ascii_file_name;
				}	else	{
								ascii_title	=	trk_file_name;
				}
				/*	Strip	off	file	extension	*/
				ascii_title.erase(ascii_title.find_last_not_of(".trk")+1,	ascii_title.length());
				/*	Strip	off	log_	prefix	extension	*/
				ascii_title.erase(ascii_title.find_first_of("log_"),	(ascii_title.find_first_of("log_")+4));

				char	buf[64]	;
				string	current_line;
				switch	(	Format	)	{
								case	XML:
												fprintf(fp,"<DataTable	name=\"%s\">\n",	ascii_title.c_str());
												fprintf(fp,"%4s<Columns>\n",	"");
												for	(	i=0;	i<number_of_parameters;	i++	)	{
																fprintf(fp,	"%8s<Column	name=\"%s\"	units=\"%s\"	/>\n",	"",	param_names[i],	param_units[i]);
																if	((	each_ds	=	new	TrickBinary(trk_file_name,	param_names[i]	))	==	NULL)	{
																				cerr	<<	".\n";
																				cerr.flush();
																				exit(EXIT_FAILURE);
																}	else	{
																				ds_list.push_back(each_ds);
																}
												}
												fprintf(fp,"%4s</Columns>\n",	"");

												fprintf(fp,"%4s<Data>\n",	"");
												done	=	0;
												while	(	!done	)	{
																current_line.clear();
																snprintf(buf,	sizeof(buf),	"%8s<Row>",	"");
																current_line.append(buf);
																for	(	idx	=	0;	idx	<	ds_list.size();	idx++	)	{
																				if	(	ds_list[idx]->get(	&t,	&y)	==	0	)	{
																								done	=	1;
																								break;
																				}
																				snprintf(buf,	sizeof(buf),	"<Col>%.15G</Col>",	y);
																				current_line.append(buf);
																}
																current_line.append("</Row>");
																if	(	!done	)	{
																				fprintf(fp,	"%s\n",	current_line.c_str());
																}
												}
												fprintf(fp,"%4s</Data>\n",	"");
												fprintf(fp,"</DataTable>\n");
												break;

								case	CSV:
								case	FIX:
								default:
												for	(	i=0;	i<number_of_parameters;	i++	)	{
																if	(i	==	0)	{
																				fprintf(fp,"%s	{%s}",param_names[0],	param_units[i]);
																}	else	{
																				fprintf(fp,"%s%s	{%s}",	delimiter.c_str(),	param_names[i],	param_units[i]);
																}

																if	((	each_ds	=	new	TrickBinary(trk_file_name,	param_names[i]	))	==	NULL)	{
																				cerr	<<	".\n";
																				cerr.flush();
																				exit(EXIT_FAILURE);
																}	else	{
																				ds_list.push_back(each_ds);
																}
												}

												fprintf(fp,"\n");

												done	=	0;
												while	(	!done	)	{
																current_line.clear();
																for	(	idx	=	0;	idx	<	ds_list.size();	idx++	)	{
																				if	(	ds_list[idx]->get(	&t,	&y)	==	0	)	{
																								done	=	1;
																								break;
																				}
																				if	(	idx	!=	0)	{
																								snprintf(buf,	sizeof(buf),	"%s",	delimiter.c_str());
																								current_line.append(buf);
																				}
																				if	(	Format	==	FIX	)	{
																								snprintf(buf,	sizeof(buf),	"%20.16g",	y);
																								current_line.append(buf);
																				}	else	{
																								snprintf(buf,	sizeof(buf),	"%.15G",	y);
																								current_line.append(buf);
																				}
																}
																if	(	!done	)	{
																				fprintf(fp,	"%s\n",	current_line.c_str());
																}
												}
												break;
				}

				//	release	memory	for	the	DataStream	list
				//	vector	clear	function	won't	delete	objects	created	by	new
				for	(	idx	=	0;	idx	<	ds_list.size();	idx++)	{
								delete	ds_list[idx];
				}
				ds_list.clear();

				//	relese	memory	for	the	name	list
				for	(i	=	0;	i	<	number_of_parameters;	i	++)	{
								delete[]	param_names[i];
				}

				return	0	;
}
