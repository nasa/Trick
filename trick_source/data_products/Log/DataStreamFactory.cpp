
#include	"DataStreamFactory.hh"

DataStreamFactory::~DataStreamFactory()
{
								//	Nada
}

DataStream*	DataStreamFactory::create(	const	char*	iRunDir,
																																							const	char*	iParamName	,
																																							const	char*	timeName	)
{
	DataStream*	stream	=	0	;
				DIR	*dirp;
				struct	dirent	*dp;
				int	len;
				char*	full_path	;
				string	runDir	;
				string	paramName	;
				string::size_type	idx	;

				//	Make	a	string	out	of	the	input	parameter	name
				string	pName	=	iParamName	;

				//	Is	this	a	delta(<var:run>,<var:run>)	parameter?
				idx	=	pName.find("delta(");
				if	(idx	==	0	)	{
								stream	=	new	Delta(iParamName,	timeName);
								return(	stream	)	;

				}

				//	Determine	parameter	name	and	run	directory
				//
				//	If	parameter	name	contains	a	run	directory,	this	will
				//	take	precedence	over	the	runDir	given.
				//	The	syntax	for	a	parameter	with	run	dir	name	is:
				//	<param_name>:<run>
				//	If	no	colon,	it	is	just	a	plain	param	name
				idx	=	pName.find(':');
				if	(	idx	!=	string::npos	)	{
								//	Extract	param	name	from	pName
								paramName	=	pName.substr(0,idx)	;

								//	Extract	run	dir	from	pName
								runDir	=	pName.substr(idx+1,pName.size()-1);
				}	else	{
								runDir	=	iRunDir	;
								paramName	=	iParamName	;
				}

				//	Open	up	RUN	dir
				if	((dirp	=	opendir(runDir.c_str()))	==	NULL)	{
								fprintf(stderr,
								"Couldn't	open	\"%s\"	for	reading.	\n",	runDir.c_str());
					return(NULL)	;
				}

				//	HDF5	(Hierarchical	Data	Format	-	h5,	hdf5)	log	files
#ifdef	HDF5
				while	((dp	=	readdir(dirp))	!=	NULL)	{
								len	=	strlen(dp->d_name);
								if	(	!strcmp(	&(dp->d_name[len	-	3])	,	".h5"))	{
												size_t	full_path_len	=	runDir.length()	+	strlen(dp->d_name)	+	2;
									full_path	=	(char*)	malloc	(full_path_len)	;
												snprintf(full_path,	full_path_len,	"%s/%s"	,	runDir.c_str()	,	dp->d_name);
												if	(	HDF5LocateParam((const	char*)full_path	,	paramName.c_str())	)	{
													closedir(dirp)	;
																stream	=	new	TrickHDF5(full_path	,	(char	*)paramName.c_str()	,	(char	*)timeName	)	;
																free(	full_path	)	;
																return(stream)	;
												}
												free(	full_path	)	;
								}
				}
#endif

				//	Trick	binary
				rewinddir(dirp)	;
				while	((dp	=	readdir(dirp))	!=	NULL)	{
								len	=	strlen(dp->d_name);
								if	(	!strcmp(	&(dp->d_name[len	-	4])	,	".trk"))	{
												size_t	full_path_len	=	runDir.length()	+	strlen(dp->d_name)	+	2;
												full_path	=	(char*)	malloc(	full_path_len)	;
												snprintf(full_path,	full_path_len,	"%s/%s",	runDir.c_str(),	dp->d_name);
												if	(	TrickBinaryLocateParam((const	char*)full_path	,	paramName.c_str())	)	{
													closedir(dirp)	;
																stream	=	new	TrickBinary(full_path	,	(char	*)paramName.c_str())	;
																free(	full_path	)	;
																return(stream)	;
												}
												free(	full_path	)	;
								}
				}

				//	CSV	Files
				rewinddir(dirp)	;
				while	((dp	=	readdir(dirp))	!=	NULL)	{
								len	=	strlen(dp->d_name);
								if	(	!strcmp(	&(dp->d_name[len	-	4])	,	".csv"))	{
												size_t	full_path_len	=	runDir.length()	+	strlen(dp->d_name)	+	2;
									full_path	=	(char*)	malloc(	full_path_len)	;
												snprintf(full_path,	full_path_len,	"%s/%s",	runDir.c_str(),	dp->d_name);
												if	(	CsvLocateParam(full_path	,	(char	*)paramName.c_str())	)	{
													closedir(dirp)	;
																stream	=	new	Csv(full_path	,	(char	*)paramName.c_str())	;
														free(	full_path	)	;
																return(stream)	;
												}
								free(	full_path	)	;
					}
				}

				//	MatLab	Binary	Files
				rewinddir(dirp)	;
				while	((dp	=	readdir(dirp))	!=	NULL)	{
						len	=	strlen(dp->d_name);
								if	(	!strcmp(	&(dp->d_name[len	-	4])	,	".mat"))	{
												size_t	full_path_len	=	runDir.length()	+	strlen(dp->d_name)	+	2;
											full_path	=	(char*)	malloc	(	full_path_len)	;
												snprintf(full_path,	full_path_len,	"%s/%s",	runDir.c_str(),	dp->d_name);
												if	(	MatLabLocateParam(full_path	,
																				(char	*)paramName.c_str(),
																							(char	*)timeName	)	)	{
																closedir(dirp)	;
																stream	=	new	MatLab(full_path	,
																					(char	*)paramName.c_str(),
																					(char	*)timeName	)	;
																free(	full_path	)	;
																return(stream)	;
												}
									if	(	MatLab4LocateParam(full_path	,
																																	(char	*)paramName.c_str(),
																																	(char	*)timeName)	)	{
										closedir(dirp)	;
													stream	=	new	MatLab4(full_path	,
										(char	*)paramName.c_str(),
																			(char	*)timeName	)	;
													free(	full_path	)	;
													return(stream)	;
									}
								free(	full_path	)	;
		}
	}

				closedir(dirp)	;

				return	stream	;
}

DataStream*	DataStreamFactory::create(	const	char*	Machine,
																																							const	unsigned	short	Port	,
																																							const	char*	VarName	)
{
				DataStream*	stream	=	0	;

				//stream	=	new	VariableServerStream(	Machine,	Port,	VarName	)	;

				return	stream	;
}

//	Create	an	external	program	data	stream
DataStream*	DataStreamFactory::create(		const	char*	sharedLibName,
																																			int	nInputStreams,	DataStream**	istreams,
																																			int	nOutputStreams,	int	outputIdx	)
{
				DataStream*	stream	=	0		;

				stream	=	new	ExternalProgram(	sharedLibName,
																																		nInputStreams,	istreams,
																																		nOutputStreams,	outputIdx	)	;

				return(stream);
}

int	getVariableNames(	const	char*	pathToData,
																						int*	numVariables,	char***	variableNames	)
{
				int	len	;

				len	=	strlen(	pathToData	)	;

				if	(	!strcmp(	&pathToData[len	-	4]	,	".trk"	))	{
					*numVariables		=	TrickBinaryGetNumVariables(pathToData)	;
								if	(	*numVariables	==	0	)	{
									return	0	;
								}
								*variableNames	=	TrickBinaryGetVariableNames(pathToData)	;
								return	1	;
}

				return(0);
}
