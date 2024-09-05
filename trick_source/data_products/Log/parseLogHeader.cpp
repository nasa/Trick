/**
				Used	to	parse	a	Trick	log	header	file,	and	load	info	for	Log	Classes
				@author	Keith	Vetter
				@version	May	2002
*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<ctype.h>
#include	<unistd.h>

#include	"log.h"

#include	"trick/units_conv.h"
#include	"trick/map_trick_units_to_udunits.hh"

//	index	labels	for	char	arrays
enum	strIndex	{
								LINE,
								STR1,
								STR2,
								STR3,
								STR4,
								NUM_STR
};

//	helper	function	to	free	memory	before	successful	or	unsuccessful	return
void	shutdown(FILE*,	char**,	char*);

/*	A	private	function	for	parsing	log	header	files.
	*	This	is	responsible	for	initialize	the	Log	classes	as	well
	*/
int	LogGroup::parseLogHeaders()
{

								int	nVars;
								int	lineNum;
								char**	strs	=	new	char*[NUM_STR];
								char	*headerName;
								int	i;
								int	headerSize;
								int	len;

								LogData	*currLogData	=	0;
								Var	*currVar;


								//	Foreach	log	header
								for	(i	=	0;	i	<	nHeaders_;	i++)	{

																//	Allocate	headerName
																len	=	strlen(pathToBinaryData_)	+	strlen(headers_[i])	+	2;
																headerName	=	new	char[len];

																//	Full	path	of	log	header
																snprintf(headerName,	len,	"%s/%s",	pathToBinaryData_,	headers_[i]);

																//	Open	log	header
																if	((fp_	=	fopen(headerName,	"r"))	==	NULL)	{
																								fprintf(stderr,
																																"\nERROR:\nCouldn't	open	file	\"%s\"	\n",
																																headerName);
																								delete[]	strs;
																								delete[]	headerName;
																								return	(-1);
																}
																//	Allocate	strings,	we	know	that	buffers	can't	be
																//	longer	than	file!
																//	Since	headers	are	typically	small,	allocate	them	at	]
																//	the	header	size
																fseek(fp_,	0,	SEEK_END);
																headerSize	=	ftell(fp_);
																::rewind(fp_);
																for(int	i	=	0;	i	<	NUM_STR;	i++){
																								strs[i]	=	new	char[headerSize	+	1];	//	Not	sure	about	+1	:)
																}

																//	Parse	rest	of	file
																lineNum	=	0;
																while	(fgets(strs[LINE],	headerSize,	fp_)	!=	NULL)	{

																								lineNum++;

																								/*	<log_file_name>	<data_type>	<units>				<param_name>
																											or
																											<log_file_name>		byte_order				is			[lit|big]_endian
																									*/
																								if	(	sscanf(strs[LINE],	"%s	%s	%s	%s",	strs[STR1],	strs[STR2],	strs[STR3],	strs[STR4])	==	4	)	{

																																//	Byte	order	statement	and	top	of	a	log	group?
																																if	(!strcmp(strs[STR2],	"byte_order"))	{

																																								LogData	*ld	=	new	LogData;
																																								log.push_back(ld);
																																								currLogData	=	log[nGroups_];
																																								nGroups_++;

																																								//	New	binary	file
																																								currLogData->setBinaryFileName(strs[STR1]);

																																								//	Set	byte	order
																																								if	(!strcmp(strs[STR4],	"big_endian"))	{
																																																currLogData->dataByteOrder	=	1;
																																								}	else	{
																																																currLogData->dataByteOrder	=	0;
																																								}

																																								continue;
																																}
																																//	Check	for	binary	file	name	mismatches
																																if	(currLogData	&&	strcmp(strs[STR1],	currLogData->getBinaryFileName()))	{
																																								printf("ERROR:	Parsing	log	header	\"%s\".\n"
																																															"							Line	%d.	Binary	file	name	"
																																															"mismatch	with	\"%s\".\n"
																																															"							May	need	little/big	endian	"
																																															"specification.\n\n",
																																															headerName,	lineNum,
																																															currLogData->getBinaryFileName());
																																								shutdown(fp_,	strs,	headerName);
																																								return	(-1);
																																}
																																//	New	variable
																																Var	*v	=	new	Var;
																																currLogData->vars.push_back(v);
																																nVars	=	currLogData->getNumVars();
																																currVar	=	currLogData->vars[nVars];
																																nVars++;
																																currLogData->setNumVars(nVars);

																																//	Set	Type
																																if	(currVar->setType(strs[STR2])	<	0)	{
																																								printf("ERROR:	In	log	header	\"%s\".\n"
																																															"							Line	%d.	Type	\"%s\"	is	"
																																															"not	supported.\n",
																																															headerName,	lineNum,	strs[STR2]);
																																								shutdown(fp_,	strs,	headerName);
																																								return	(-1);
																																}

																																//	For	Release-07	we	need	to	convert	the	old	unit	specs	by
																																//	adding	explicit	asterisk	for	multiplication.	)
																																{
																																								char	new_units_spec[100];
																																								new_units_spec[0]	=	0;
																																								if	(	convert_units_spec	(strs[STR3],	new_units_spec)	!=	0	)	{
																																																printf	("	ERROR:	Attempt	to	convert	old	units-spec,	\"%s\"	failed.\n\n",strs[STR3]);
																																																shutdown(fp_,	strs,	headerName);

																																																return	(-1);
																																								}
																																								delete	[]	strs[STR3];
																																								len	=	strlen(new_units_spec);
																																								strs[STR3]	=	new	char[len	+	1]	;
																																								strcpy(strs[STR3],	new_units_spec);
																																}

																																//	Initialize	Unit	class
																																if	(	!	strcmp(strs[STR3],"--"))	{
																																				currVar->setUnit(strs[STR3]);
																																}	else	{
																																				currVar->setUnit(map_trick_units_to_udunits(strs[STR3]));
																																}

																																//	Set	Var	Name
																																currVar->setVarName(strs[STR4])	;
																																if	(currVar->getSize()	<	0)	{
																																								printf("ERROR:	In	log	header	\"%s\".\n"
																																															"							Line	%d.	Problem	with	var	"
																																															"name	\"%s\"	.	\n",
																																															headerName,	lineNum,	strs[STR4]);
																																								shutdown(fp_,	strs,	headerName);
																																								return	(-1);
																																}
																								}
																}
								shutdown(fp_,	strs,	headerName);
								}
								delete[]	strs;
								return	1;
}

void	shutdown	(FILE*	fp_,	char**	strs,	char*	headerName)	{
								fclose(fp_);
								for(int	i	=	0;	i	<	NUM_STR;	i++)	{
																delete[]	strs[i];
								}
								delete[]	headerName;
}
