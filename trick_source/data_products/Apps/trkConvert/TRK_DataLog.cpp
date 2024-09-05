#include	"TRK_DataLog.hh"
#include	<iostream>
#include	<string.h>
#include	<cstring>
#include	<cstdlib>
#include	<stdexcept>

typedef	enum	{
				TRICK_VOID															=			0,	/*	No	type	*/
				TRICK_CHARACTER										=			1,	/*	(char)	*/
				TRICK_UNSIGNED_CHARACTER	=			2,	/*	(unsigned	char)	*/
				TRICK_STRING													=			3,	/*	(char	*)	*/
				TRICK_SHORT														=			4,	/*	(short)	*/
				TRICK_UNSIGNED_SHORT					=			5,	/*	(unsigned	short)	*/
				TRICK_INTEGER												=			6,	/*	(int)	*/
				TRICK_UNSIGNED_INTEGER			=			7,	/*	(unsigned	int)	*/
				TRICK_LONG															=			8,	/*	(long)	*/
				TRICK_UNSIGNED_LONG						=			9,	/*	(unsigned	long)	*/
				TRICK_FLOAT														=		10,	/*	(float)	*/
				TRICK_DOUBLE													=		11,	/*	(double)	*/
				TRICK_BITFIELD											=		12,	/*	(signed	int	:	1)	*/
				TRICK_UNSIGNED_BITFIELD		=		13,	/*	(unsigned	int	:	1)	*/
				TRICK_LONG_LONG										=		14,	/*	(long	long)	*/
				TRICK_UNSIGNED_LONG_LONG	=		15,	/*	(long	long)	*/
				TRICK_FILE_PTR											=		16,	/*	(file	*)	*/
				TRICK_BOOLEAN												=		17,	/*	(C++	boolean)	*/
				TRICK_WCHAR														=		18,	/*	(wchar_t)	*/
				TRICK_WSTRING												=		19,	/*	(wchar_t	*)	*/
				TRICK_VOID_PTR											=		20,	/*	an	arbitrary	address	*/
				TRICK_ENUMERATED									=		21,	/*	User	defined	type	(enumeration)	*/
				TRICK_STRUCTURED									=		22,	/*	User	defined	type	(struct/class)	*/
				TRICK_OPAQUE_TYPE								=		23,	/*	User	defined	type	(where	type	details	are	as	yet	unknown)	*/
				TRICK_STL																=		24,	/*	Standard	template	library	type	*/
				TRICK_NUMBER_OF_TYPES
}	TRICK_TYPE	;

const	char*	TypeName[]	=	{
				"--0--",
				"char",
				"unsigned	char",
				"--3--",
				"short",
				"unsigned	short",
				"int",
				"unsigned	int",
				"long",
				"unsigned	long",
				"float",
				"double",
				"--12--",
				"--13--",
				"long	long",
				"unsigned	long	long",
				"--16--",
				"bool",
				"--18--",
				"--19--",
				"--20--",
				"--21--",
				"--22--",
				"--23--",
				"--24--"
};

const	int	TRK_DataLog::LittleEndian	=	1;
const	int	TRK_DataLog::BigEndian				=	2;

TRK_DataLog::TRK_DataLog(std::string	file_name)	{

				fileName	=	file_name;
				in_fp	=	fopen(fileName.c_str(),	"rb");

				if	(in_fp	!=	NULL)	{

								char	trick_header_string[11];
								if	(fread(	trick_header_string,	1,	10,	in_fp)	!=	10)
												throw	std::runtime_error("fread()	failed.");
								trick_header_string[10]	=	0;

								if	(!strncmp(	trick_header_string,	"Trick-",	6))	{
													
												char	version_txt[3];			
												memcpy(version_txt,	&trick_header_string[6],	2)	;
												version_txt[2]	=	0;
												version	=	atoi(version_txt);

												char	endian_txt[2];			
												memcpy(endian_txt,	&trick_header_string[9],	1)	;
												endian_txt[1]	=	0;

												if	(!strncmp(	endian_txt,	"L",	1))	{
																endianness	=	LittleEndian;
												}	else	if	(!strncmp(	endian_txt,	"B",	1))	{
																endianness	=	BigEndian;
												}	else	{
																fprintf	(stderr,	"Trick	header	error.	Endianness	should	be	\"L\"	or	\"B\".");
												}

												if	(fread(	&N_params,	1,	4,	in_fp)	!=	4)	
																throw	std::runtime_error("fread()	failed.");

												dataRecordSize	=	0;
												for	(int	ii	=	0	;	ii	<	(int)N_params	;	ii++	)	{
																ParamDescription*	paramDescription	=	new	ParamDescription(in_fp);
																paramDescriptions.push_back(paramDescription);
																paramOffsets.push_back(dataRecordSize);
																paramSelected.push_back(false);

																dataRecordSize	+=	paramDescription->dataSize;
												}
												dataRecord	=	new	char[dataRecordSize];

												//	Time	should	always	be	selected.
												paramSelected[0]	=	true;

												if	(fgetpos(in_fp,	&dataPosition)	!=	0)	{
																fprintf	(stderr,	"fgetpos	failure.\n");
												}
								}	else	{
												std::cerr	<<	"File	\""	<<	fileName	<<	"\"	isn't	a	Trick	binary	log	file."	<<	std::endl;
								}
				}	else	{
								std::cerr	<<	"File	\""	<<	fileName	<<	"\"	failed	to	open."	<<	std::endl;
				}
}

std::string	TRK_DataLog::getFileName()	const	{
				return	fileName;
}

int	TRK_DataLog::parameterCount()	const	{
				return	(int)N_params;
}

const	char*	TRK_DataLog::parameterName(unsigned	int	n)	const	{
				if	(n	<	N_params)	
								return	paramDescriptions[n]->parameterName;
				else
								return	"BadIndex";
}

const	char*	TRK_DataLog::parameterUnits(unsigned	int	n)	const	{
				if	(n	<	N_params)	
								return	paramDescriptions[n]->unitsName;
				else
								return	"BadIndex";
}

const	char*	TRK_DataLog::parameterType(unsigned	int	n)	const	{
				if	(n	<	N_params)	
								return	TypeName[	paramDescriptions[n]->dataType	];
				else
								return	"BadIndex";
}


void	TRK_DataLog::selectAllParameters()	{
				for	(int	ii	=	1	;	ii	<	(int)N_params	;	ii++	)	{
								paramSelected[ii]	=	true;
				}
}

void	TRK_DataLog::selectParameter(unsigned	int	index)	{
				if	((index	>	0)	&&	(index	<	N_params))	{
								paramSelected[index]	=	true;
				}
}

void	TRK_DataLog::selectParameter(const	char	*	paramName)	{
				bool	found	=	false;
				int	ii	=	1;
				while	((ii	<	(int)N_params)	&&	(found	==	false))	{
								if	(	!strcmp(	paramName,	paramDescriptions[ii]->parameterName	))	{
												paramSelected[ii]	=	true;
												found	=	true;
								}
								ii	++;
				}
}

void	TRK_DataLog::deselectParameter(unsigned	int	index)	{
				if	((index	>	0)	&&	(index	<	N_params))	{
								paramSelected[index]	=	false;
				}
}

void	TRK_DataLog::formattedWrite(FILE*	out_fp,	LogFormatter*	formatter)	{

				formatter->writeHeader(out_fp,	version,	endianness);
				formatter->writeColumnLabel(out_fp,	paramDescriptions[0]->parameterName,	paramDescriptions[0]->unitsName);	
				for	(int	ii	=	1;	ii	<	(int)N_params	;	ii++)	{
								if	(paramSelected[ii])	{
												formatter->writeColumnLabelSeparator(out_fp);
												formatter->writeColumnLabel(out_fp,	paramDescriptions[ii]->parameterName,	paramDescriptions[ii]->unitsName);	
								}
				}

				if	(	fsetpos(in_fp,	&dataPosition)	!=	0	)	{
							fprintf(stderr,"fsetpos()	error.\n");
				}
				while	(	fread(	dataRecord,	1,	dataRecordSize,	in_fp)	==	(size_t)dataRecordSize)	{
								formatter->writeRecordSeparator(out_fp);
								formatter->writeDatum(out_fp,	*(double*)&dataRecord[0]);	//	Time	is	always	selected.
								for	(int	ii	=	1	;	ii	<	(int)N_params	;	ii++	)	{
												if	(paramSelected[ii])	{
																formatter->writeDatumSeparator(out_fp);
																switch	(paramDescriptions[ii]->dataType)	{
																				case	TRICK_CHARACTER:	{	/*1*/
																								formatter->writeDatum(out_fp,	*(int8_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_UNSIGNED_CHARACTER:	{	/*2*/
																								formatter->writeDatum(out_fp,	*(uint8_t*)&dataRecord[paramOffsets[ii]])	;
																				}	break;
																				case	TRICK_SHORT:	{	/*4*/
																								formatter->writeDatum(out_fp,	*(int16_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_UNSIGNED_SHORT:	{	/*5*/
																								formatter->writeDatum(out_fp,	*(uint16_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_INTEGER:	{/*6*/
																								formatter->writeDatum(out_fp,	*(int32_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_UNSIGNED_INTEGER:	{/*7*/
																								formatter->writeDatum(out_fp,	*(uint32_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_LONG:	{	/*8*/
																								formatter->writeDatum(out_fp,	*(int64_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_UNSIGNED_LONG:	{	/*9*/
																								formatter->writeDatum(out_fp,	*(uint64_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_FLOAT:	{/*10*/
																								formatter->writeDatum(out_fp,	*(float*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_DOUBLE:	{/*11*/
																								formatter->writeDatum(out_fp,	*(double*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_LONG_LONG:	{	/*14*/
																								formatter->writeDatum(out_fp,	*(int64_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_UNSIGNED_LONG_LONG:	{	/*15*/
																								formatter->writeDatum(out_fp,	*(uint64_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				case	TRICK_BOOLEAN:	{	/*17*/
																								formatter->writeDatum(out_fp,	*(uint8_t*)&dataRecord[paramOffsets[ii]]);
																				}	break;
																				default:	{
																								fprintf(stdout,	"Unknown	Data	Type	[%d]	of	size	[%d]	bytes.",
																												paramDescriptions[ii]->dataType,
																												paramDescriptions[ii]->dataSize);
																								fflush(stdout);
																								exit(1);
																				}
																}
												}
								}
				}
				formatter->writeRecordSeparator(out_fp);
}
