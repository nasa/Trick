#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<unistd.h>
#include	<udunits2.h>

#include	"log.h"
#include	"trick_byteswap.h"

//	For	DBL_MAX	def
#include	<math.h>
#include	<float.h>

extern	ut_system	*	u_system	;

/**
	*	LogData	constructor
	*/
LogData::LogData()
{
								unsigned	char	localByteOrder;

								open_	=	0;
								dataByteOrder	=	0;
								currBinExtension_	=	0;
								fp_	=	0;
								nBinFiles_	=	0;

								binFileName_	=	new	char[1];
								binFileName_[0]	=	'\0';

								binFilePath_	=	new	char[1];
								binFilePath_[0]	=	'\0';

								currRecord_	=	new	char[0];

								isEOF_	=	0;
								currValIdx_	=	0;
								convertToDouble_	=	0;
								nVars_	=	0;

								isEnabledStats_	=	0;

								statsMin_	=	0	;
								statsMax_	=	0	;

								//	Get	local	machine's	byte	odor	(smelly	to	be	sure)
								TRICK_GET_BYTE_ORDER(localByteOrder)
												if	(localByteOrder	==	(char)	(0x00))	{
																//	Big	Endian
																machineByteOrder	=	1;
								}	else	{
																//	Little	Endian
																machineByteOrder	=	0;
								}
}

/**	Destructor	for	LogData	*/
LogData::~LogData()
{

								int	ii;

								if	(fp_)	{
																fclose(fp_);
								}

								delete[]binFilePath_;
								delete[]binFileName_;

								for	(ii	=	0;	ii	<	nVars_;	ii++)	{
																delete	vars[ii];
								}

								delete[]currRecord_;
								delete[]varVal_;
								delete[]sizeVal_;
								delete[]scaleVal_;
								delete[]biasVal_;
								delete[]unitVal_;
								delete[]maxVal_;
								delete[]minVal_;
								delete[]typeVal_;

								if	(	statsMin_	&&	statsMax_	)	{
																delete[]	statsMin_;
																delete[]	statsMax_;
								}
}

/**	Return	the	number	of	variable	(not	values)	in	binary	log	file	header
	*
	*		If	log	header	contains	"a[0-1].b[0-1],	x.y[4-5]"	there	are	2
	*		variables	NOT	6.
	*/
int	LogData::getNumVars()
{
								return	(nVars_);
}

/**	Set	the	number	of	variables	(not	values)	in	binary	log	file	header
	*
	*		If	log	header	contains	"a[0-1].b[0-1],	x.y[4-5]"	there	are	2
	*		variables	NOT	6.
	*
	*		This	should	be	used	with	care!		Currently	-only-	a	parse	routine
	*		in	LogGroup	uses	it.		I	could	have	made	LogGroup	a	friend...	but
	*		wanted	to	keep	the	interface	stiff.
	*/
void	LogData::setNumVars(int	nVars)
{
								nVars_	=	nVars;
}

/**	Close	a	Trick	data	log
	*	@see	rewind()	close()
	*/
int	LogData::close()
{

								//	Delete	currRecord
								delete[]currRecord_;

								open_	=	0;
								fclose(fp_);

								//	Return
								return	(1);
}

/**	Puts	next	read	at	beginning	of	data	log
	*		@see	open()	close()
	*/
int	LogData::rewind()
{

								if	(!open_)	{
																return	(-1);
								}

								isEOF_	=	0;

								if	(currBinExtension_	>	0)	{
																//	If	we	have	multiple	bin	files,	we	must	go	back	to	first	one
																fclose(fp_);
																currBinExtension_	=	0;
																openCurrBinaryFile();
								}	else	{
																::rewind(fp_);
								}

								return	(0);
}

/**	Open	a	data	log	at	location	specified	by	path
	*		Path	will	normally	point	to	a	RUN_	directory
	*		@see	rewind()	close()
	*/
int	LogData::open(char	*pathToBinaryData)
{

								int	i,	j;
								int	idx;
								int	nDims;
								int	ret	;

								if	(open_)	{
																printf("WARNING:	Data	file	already	opened.\n");
																return	(1);
								}
								//	Make	sure	that	number	of	variables	have	been	set
								if	(nVars_	==	0)	{
																printf("ERROR:	The	number	of	variables	"
																							"in	log	file	is	not	set.\n");
																return	(-1);
								}
								//	Count	number	of	binary	files	for	this	log	data	dude	(log*000-?)
								nBinFiles_	=	countNumBinaryFiles(pathToBinaryData);

								//	Save	off	binary	file	path
								delete[]binFilePath_;
								binFilePath_	=	new	char[strlen(pathToBinaryData)	+	1];
								strcpy(binFilePath_,	pathToBinaryData);

								//	Open	first	binary	file	(log*.000)
								ret	=	openCurrBinaryFile();
								if	(	ret	==	-1	)	{
																return(-1);
								}

								//	Initialize	size	of	record
								sizeRecord_	=	getSizeRecord();
								if	(sizeRecord_	<	1)	{
																printf("ERROR:	Size	of	record	is	%d.\n",	sizeRecord_);
								}
								//	Initialize	currRecord
								delete[]currRecord_;
								currRecord_	=	new	char[sizeRecord_];

								//	Initialize	the	value	array's	size	and	types
								//	Values	are	elements	in	record,	each	one	has	a	size	and	type

								//	Allocate	value	attributes
								nVals_	=	calcNumValRecord();
								varVal_	=	new	int[nVals_];
								sizeVal_	=	new	int[nVals_];
								scaleVal_	=	new	double[nVals_];
								biasVal_	=	new	double[nVals_];
								unitVal_	=	new	double[nVals_];
								maxVal_	=	new	double[nVals_];
								minVal_	=	new	double[nVals_];
								typeVal_	=	new	Var::enumType[nVals_];

								//	Allocate	variables	that	hold	stats
								statsMin_	=	new	Stats[nVals_];
								statsMax_	=	new	Stats[nVals_];
								for	(i	=	0;	i	<	nVals_;	i++)	{
																statsMin_[i].val	=	DBL_MAX;
																statsMax_[i].val	=	-DBL_MAX;
								}

								//	Initialize	value	attributes
								idx	=	0;
								for	(i	=	0;	i	<	nVars_;	i++)	{
																nDims	=	vars[i]->getNumDims();
																for	(j	=	0;	j	<	nDims;	j++)	{
																								varVal_[idx]	=	i;
																								sizeVal_[idx]	=	vars[i]->getTypeSize();
																								scaleVal_[idx]	=	vars[i]->getScaleFactor();
																								biasVal_[idx]	=	vars[i]->getBias();
																								unitVal_[idx]	=	1.0;
																								maxVal_[idx]	=	vars[i]->getMaxRange();
																								minVal_[idx]	=	vars[i]->getMinRange();
																								typeVal_[idx]	=	vars[i]->getType();
																								idx++;
																}
								}

								//	Finally	set	open	flag
								open_	=	1;

								//	Return
								return	(1);
}

/**	Go	through	all	data	and	calculate	statistics
	*/
void	LogData::enableStats()
{

								int	ii;
								double	val,	tval;

								//	Stats	on,	just	return
								if	(isEnabledStats_)	{
																return;
								}	else	{
																isEnabledStats_	=	1;
								}

								int	tIdx	=	getParamIdx("sys.exec.out.time");
								if	(tIdx	==	-1)	{
																fprintf(stderr,	"ERROR:	enableStats()	because	time	is	"
																																"something	other	than	sys.exec.out.time\n");
																exit(-1);
								}
								//	Mins	and	maxs
								rewind();															//	Start	at	top	of	records
								while	(getNextRecord()	!=	NULL)	{

																//	Get	time	stamp	and	BE	SURE	to	reset	pointers
																tval	=	*((double	*)	getVal(tIdx));
																currValPtr_	=	(unsigned	char	*)	currRecord_;
																currValIdx_	=	0;
																bytesRead_	=	0;

																for	(ii	=	0;	ii	<	nVals_;	ii++)	{
																								val	=	*((double	*)	getNextVal());							//	For	speed	:)
																								if	(statsMin_[ii].val	>	val)	{
																																statsMin_[ii].val	=	val;
																																statsMin_[ii].tval	=	tval;
																								}
																								if	(statsMax_[ii].val	<	val)	{
																																statsMax_[ii].val	=	val;
																																statsMax_[ii].tval	=	tval;
																								}
																}
								}
}

/**	Returns	min/max	time	stamps	and	values	calculated	by	calculateStats()	*/
void	LogData::getMinMaxStats(int	paramIdx,
																													double	*tmin,	double	*min,
																													double	*tmax,	double	*max)
{

								if	(!isEnabledStats_)	{
																enableStats();
								}

								*tmin	=	statsMin_[paramIdx].tval;
								*min	=	statsMin_[paramIdx].val;
								*tmax	=	statsMax_[paramIdx].tval;
								*max	=	statsMax_[paramIdx].val;
}


/**	Count	the	number	of	binary	files	located	at	path	given	e.g.
	*		In	/user3/vetter/trick_sims/SIM_dog/RUN_test
	*		there	may	be	log_Cat.000	log_Cat.001	....	log_Cat.034
	*		In	this	case	countNumBinaryFiles	returns	35
	*/
int	LogData::countNumBinaryFiles(char	*pathToBinaryData)
{

								DIR	*dirp;
								struct	dirent	*dp;
								int	count;
								int	ii;																	//	Long	live	Rob!
								int	binary;
								int	len_binFileName;
								int	len_dname;
								int	len_extension	=	4;		//	length	.###	binary	extension

								//	Open	up	RUN	dir
								if	((dirp	=	opendir(pathToBinaryData))	==	NULL)	{
																fprintf(stderr,	"Couldn't	open	\"%s\"	for	reading.	\n",
																								pathToBinaryData);
																fprintf(stderr,	"Maybe	LogGroup	Constructor	not	called.\n");
																exit(-1);
								}

								count	=	0;
								while	((dp	=	readdir(dirp))	!=	NULL)	{
																len_dname	=	strlen(dp->d_name);
																len_binFileName	=	strlen(binFileName_);
																if	(len_dname	==	len_binFileName	+	len_extension	&&
																				!strncmp(dp->d_name,	binFileName_,	len_binFileName))	{

																								//	We	have	found	bin	file	name,
																								//	make	sure	it	has	a	binary	extension
																								binary	=	1;
																								for	(ii	=	len_dname	-	1;	ii	>=	len_dname	-	3;	ii--)	{
																																if	(!(dp->d_name[ii]	>=	'0'	&&
																																						dp->d_name[ii]	<=	'9'))	{
																																								binary	=	0;
																																}
																								}

																								//	If	this	is	a	binary	file,
																								//	000-999	extension,	increment	count
																								if	(binary)	{
																																count++;
																								}
																}
								}

								closedir(dirp);

								return	(count);
}


/*
	*	Used	privately	to	open	each	subsequent	binary	file	on	a	the	lowest	level
	*/
int	LogData::openCurrBinaryFile()
{

								char	extension[4];
								char	*file;
								int	len;

								//	Construct	extension
								if	(currBinExtension_	<	10)	{
																snprintf(extension,	sizeof(extension),	"00%d",	currBinExtension_);
								}	else	if	(currBinExtension_	<	100)	{
																snprintf(extension,	sizeof(extension),	"0%d",	currBinExtension_);
								}	else	{
																snprintf(extension,	sizeof(extension),	"%d",	currBinExtension_);
								}

								//	Construct	binary	filename
								len	=	strlen(binFilePath_)	+	strlen(binFileName_)	+
												strlen(extension)	+	3;
								file	=	new	char[len];
								snprintf(file,	len,	"%s/%s.%s",	binFilePath_,	binFileName_,	extension);

								//	Do	sanity	check	on	file
								if	(access(file,	F_OK	|	R_OK)	==	-1)	{
																fprintf(stderr,	"ERROR:	Search	for	parameter	exhausted."
																							"							Or	\"%s\"	is	not	readable	or	doesn't	exist.	\n",
																							file);
																delete	[]	file;
																return(-1);
								}
								//	Close	previous	binary	file	if	not	first	file	to	open
								if	(currBinExtension_	>	0)	{
																fclose(fp_);
								}
								//	Open	binary	file
								fp_	=	fopen(file,	"r");
								if	(fp_	==	NULL)	{
																printf("ERROR:	Couldn't	open	\"%s\"	\n",	file);
																exit(-1);
								}

								delete[]file;

								return	(1);
}

/**	Request	to	convert	all	data	from	log	file	into	doubles
	*		@see	convertToDoubleNo()
*/
int	LogData::convertToDoubleYes()
{
								convertToDouble_	=	1;
								return	(1);
}

/**	Do	NOT	convert	log	data	to	doubles
	*		@see	convertToDoubleYes()
	*/
int	LogData::convertToDoubleNo()
{
								convertToDouble_	=	0;
								return	(1);
}

/**	Get	number	of	records	within	a	Trick	binary	log	e.g.
	*
	*		If	time,	x	and	y	are	logged,	where	time,	x	&	y	are	doubles
	*		And	the	Trick	binary	files	contains	29616	bytes,	we'd	have
	*		29616/24	=	1,234	records.
	*/
int	LogData::getNumRecords()
{

								FILE	*fpTmp;
								char	*file;
								char	extension[4];

								int	fileOffset;
								int	numRecs;
								int	totalNumRecs;
								int	ii;
								int	len;

								totalNumRecs	=	0;
								for	(ii	=	0;	ii	<	nBinFiles_;	ii++)	{

																//	Construct	extension
																if	(ii	<	10)	{
																								snprintf(extension,	sizeof(extension),	"00%d",	ii);
																}	else	if	(ii	<	100)	{
																								snprintf(extension,	sizeof(extension),	"0%d",	ii);
																}	else	{
																								snprintf(extension,	sizeof(extension),	"%d",	ii);
																}

																//	Allocate	&	construct	file	name
																//	(+3	for	'/'	,		'.'	&	null	terminator)
																len	=	strlen(binFilePath_)	+	strlen(binFileName_)	+
																				strlen(extension)	+	3;
																file	=	new	char[len];
																snprintf(file,	len,	"%s/%s.%s",	binFilePath_,
																								binFileName_,	extension);

																fpTmp	=	fopen(file,	"r");
																if	(fpTmp	==	NULL)	{
																								printf("ERROR:	Couldn't	open	\"%s\"	\n",	file);
																								exit(-1);
																}

																fseek(fpTmp,	0,	SEEK_END);
																fileOffset	=	ftell(fpTmp);
																numRecs	=	fileOffset	/	getSizeRecord();
																totalNumRecs	+=	numRecs;

																fclose(fpTmp);
																delete[]file;
								}

								return	(totalNumRecs);
}

/**	Are	all	params	read	from	current	record	?	*/
int	LogData::isEOF()
{
								return	(isEOF_);
}

/**	Return	size	of	record	in	bytes	e.g.
	*
	*		If	a	record	contains	time,	x,	y,	i,	j	where
	*		time,	x,	&	y	are	doubles	and	i,j	are	ints
	*		You'd	have	3*8	+	2*4	=	32	bytes	(on	most	machines)
	*/
int	LogData::getSizeRecord()
{

								int	i;
								int	sizeRec;

								sizeRec	=	0;
								for	(i	=	0;	i	<	nVars_;	i++)	{
																sizeRec	=	sizeRec	+	vars[i]->getSize();
								}
								if(sizeRec	<=	0)	{
												std::cerr	<<	"ERROR:	LogData::getSizeRecord()	is	zero	or	negative,	may	cause	divide-by-zero.	Exiting."	<<	std::endl;
												exit(-1);
								}
								return	(sizeRec);
}

/**	Calculate	the	number	of	values	in	a	record	e.g.
	*
	*		A	record	with	"x.y.z[0-2]	&	a[0-1].b[7]"	is
	*		x.y.z[0],	x.y.z[1],	x.y.z[2],	a[0].b[7],	a[1].b[7]
	*		This	is	5	vals	for	the	record.		Use	getSizeRecord
	*		for	byte	size.
	*/
int	LogData::calcNumValRecord()
{


								int	i;
								int	nVals;

								nVals	=	0;
								for	(i	=	0;	i	<	nVars_;	i++)	{
																nVals	=	nVals	+	vars[i]->getNumDims();
								}

								return	(nVals);
}

/**	Retrieve	number	of	values	in	a	record
	*		(calculated	by	calcNumValRecord())
	*		nVals_	set	once	in	the	open	routine
	*/
int	LogData::getNumValRecord()
{
								return	(nVals_);
}

/**	Read	the	next	binary	record	in	current	binary	file
	*	For	external	data	logs	we	just	return	a	1
	**/
char	*LogData::getNextRecord()
{

								int	ret;
								int	sr;

								//	Reset	variable	index	to	first	variable
								currVarIndex_	=	0;

								//	Reset	variable	pointer	to	top	of	record
								currValPtr_	=	(unsigned	char	*)	currRecord_;
								currValIdx_	=	0;
								bytesRead_	=	0;
								isEOF_	=	0;

								//	Read	in	next	record
								sr	=	sizeRecord_;
								ret	=	fread(currRecord_,	sr,	1,	fp_);
								if	(ret	<=	0)	{
																//	Hit	EOF	on	current	bin	file
																currBinExtension_++;
																if	(currBinExtension_	<	nBinFiles_)	{
																								//	Open	up	next	bin	file,	and	get	first	rec
																								openCurrBinaryFile();
																								fread(currRecord_,	sr,	1,	fp_);
																								return	(currRecord_);
																}	else	{
																								//	We	are	at	end	of	log	data	(no	more	recs)
																								currValIdx_	=	0;
																								isEOF_	=	1;
																								return	((char	*)	NULL);
																}
								}	else	{
																//	Read	went	OK
																return	(currRecord_);
								}
}

/*	Very	low	level	routine	for	advancing	value	pointer	across	record	*/
void	*LogData::advanceCurrValPtr()
{

								currValPtr_	=	currValPtr_	+	sizeVal_[currValIdx_];
								bytesRead_	=	bytesRead_	+	sizeVal_[currValIdx_];
								currValIdx_++;

								return	((void	*)	&currVal_);
}

#define	DP_ASSIGN_VAL(V,	TYPE)	\
							if	(	convertToDouble_	)	{	\
															currVal_.d	=	(double)	V	;		\
							}	else	{	\
															currVal_.V	=		(TYPE)	V	;		\
															currVal_.V	*=	(TYPE)scaleVal_[currValIdx_]	;	\
															currVal_.V	*=	(TYPE)unitVal_[currValIdx_]	;	\
															currVal_.V	+=	(TYPE)scaleVal_[currValIdx_]	;	\
															if	(	currVal_.V	>	(TYPE)	maxVal_[currValIdx_]	)	{	\
																							currVal_.V	=	(TYPE)	maxVal_[currValIdx_]	;	\
															}	\
															if	(	currVal_.V	<	(TYPE)	minVal_[currValIdx_]	)	{	\
																							currVal_.V	=	(TYPE)	minVal_[currValIdx_]	;	\
															}	\
							}

/**
	*	Main	function	for	byte-by-byte	grabbing	values	out	of	a	binary	record
	*	TODO:	Can	I	use	a	Template<>	???
	*/
void	*LogData::getNextVal()
{

								int	idx;
								unsigned	char	*cp1;

								char	c;
								short	s;
								int	i;
								long	l;
								unsigned	char	uc;
								unsigned	short	us;
								unsigned	int	ui;
								unsigned	long	ul;
								float	f;
								double	d;
								long	long	ll;
								unsigned	long	long	ull;

								if	(currRecord_	==	0)	{
																return	(0);
								}

								if	(bytesRead_	>=	sizeRecord_)	{
																return	(0);
								}

								switch	(typeVal_[currValIdx_])	{

								case	Var::CHAR:
																cp1	=	(unsigned	char	*)	&c;
																cp1[0]	=	currValPtr_[0];
																if	(convertToDouble_)	{
																								currVal_.d	=	(double)	c;
																}	else	{
																								currVal_.c	=	(char)	c;
																}
																break;
								case	Var::SHORT:
																cp1	=	(unsigned	char	*)	&s;
																for	(idx	=	0;	idx	<	2;	idx++)
																								cp1[idx]	=	currValPtr_[idx];
																if	(machineByteOrder	!=	dataByteOrder)	{
																								s	=	trick_byteswap_short(s);
																}
																DP_ASSIGN_VAL(s,	short	int);
																break;
								case	Var::INT:
																cp1	=	(unsigned	char	*)	&i;
																for	(idx	=	0;	idx	<	4;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								i	=	trick_byteswap_int(i);
																}
																DP_ASSIGN_VAL(i,	int);
																break;
								case	Var::LONG:
																cp1	=	(unsigned	char	*)	&l;
																for	(idx	=	0;	idx	<	8;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								l	=	trick_byteswap_long(l);
																}
																DP_ASSIGN_VAL(l,	long);
																break;
								case	Var::UNSIGNED_CHAR:
																cp1	=	(unsigned	char	*)	&uc;
																cp1[0]	=	currValPtr_[0];
																if	(convertToDouble_)	{
																								currVal_.d	=	(double)	uc;
																}	else	{
																								currVal_.uc	=	(unsigned	char)	uc;
																}
																break;
								case	Var::UNSIGNED_SHORT:
																cp1	=	(unsigned	char	*)	&us;
																for	(idx	=	0;	idx	<	2;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								us	=	trick_byteswap_short(us);
																}
																DP_ASSIGN_VAL(us,	unsigned	short);
																break;
								case	Var::UNSIGNED_INTEGER:
																cp1	=	(unsigned	char	*)	&ui;
																for	(idx	=	0;	idx	<	4;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								ui	=	trick_byteswap_int(ui);
																}
																DP_ASSIGN_VAL(ui,	unsigned	int);
																break;
								case	Var::UNSIGNED_LONG:
																cp1	=	(unsigned	char	*)	&ul;
																for	(idx	=	0;	idx	<	8;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								ul	=	trick_byteswap_long(ul);
																}
																DP_ASSIGN_VAL(ul,	unsigned	long);
																break;
								case	Var::FLOAT:
																cp1	=	(unsigned	char	*)	&f;
																for	(idx	=	0;	idx	<	4;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								f	=	trick_byteswap_float(f);
																}
																DP_ASSIGN_VAL(f,	float);
																break;
								case	Var::DOUBLE:
																cp1	=	(unsigned	char	*)	&d;
																for	(idx	=	0;	idx	<	8;	idx++)	{
																								cp1[idx]	=	currValPtr_[idx];
																}
																if	(machineByteOrder	!=	dataByteOrder)	{
																								d	=	trick_byteswap_double(d);
																}
																currVal_.d	=	(double)	d;
																if	(!convertToDouble_)	{
																								//	If	converting,	scaling	done	later
																								currVal_.d	*=	scaleVal_[currValIdx_];
																								currVal_.d	*=	unitVal_[currValIdx_];
																								currVal_.d	+=	biasVal_[currValIdx_];
																								if	(currVal_.d	>	maxVal_[currValIdx_])	{
																																currVal_.d	=	maxVal_[currValIdx_];
																								}
																								if	(currVal_.d	<	minVal_[currValIdx_])	{
																																currVal_.d	=	minVal_[currValIdx_];
																								}
																}
																break;
								case	Var::LONG_LONG:
																cp1	=	(unsigned	char	*)	&ll;
																if	(machineByteOrder	!=	dataByteOrder)	{
																								for	(idx	=	0;	idx	<	8;	idx++)	{
																																cp1[7	-	idx]	=	currValPtr_[idx];
																								}
																}	else	{
																								for	(idx	=	0;	idx	<	8;	idx++)	{
																																cp1[idx]	=	currValPtr_[idx];
																								}
																}
																DP_ASSIGN_VAL(ll,	long	long);
																break;
								case	Var::UNSIGNED_LONG_LONG:
																cp1	=	(unsigned	char	*)	&ull;
																if	(machineByteOrder	!=	dataByteOrder)	{
																								for	(idx	=	0;	idx	<	8;	idx++)	{
																																cp1[7	-	idx]	=	currValPtr_[idx];
																								}
																}	else	{
																								for	(idx	=	0;	idx	<	8;	idx++)	{
																																cp1[idx]	=	currValPtr_[idx];
																								}
																}

																DP_ASSIGN_VAL(ull,	unsigned	long	long);
																break;
								case	Var::VOID:
																break;
								default:
																printf("ERROR:	GetNextVal	getting	weird	type.\n");
																exit(-1);
								}

								//	Do	scaling	etc...	if	converting	to	doubles
								if	(convertToDouble_)	{
																currVal_.d	*=	scaleVal_[currValIdx_];
																currVal_.d	*=	unitVal_[currValIdx_];
																currVal_.d	+=	biasVal_[currValIdx_];
																if	(currVal_.d	>	maxVal_[currValIdx_])	{
																								currVal_.d	=	maxVal_[currValIdx_];
																}
																if	(currVal_.d	<	minVal_[currValIdx_])	{
																								currVal_.d	=	minVal_[currValIdx_];
																}
								}
								//	Now	that	we	have	current	val,	advance	pointer
								advanceCurrValPtr();

								return	((void	*)	&currVal_);
}

/**	Get	value	in	record	at	given	index	e.g.
	*
	*		So	if	the	current	record	contains	a[0-1],	b[0-1]
	*		getVal(2)	will	return	b[0]'s	value
	*/
void	*LogData::getVal(int	index)
{

								int	i;

								//	Make	sure	record	exists
								if	(currRecord_	==	0)	{
																return	(0);
								}

								//	Make	sure	index	is	valid
								if	(index	>=	nVals_	||	index	<	0)	{
																fprintf(stderr,	"ERROR:	getVal	attempting	to	grab	val	"
																								"out	of	bounds.\n"
																								"							getVal	received	faulty	index	\"%d\".\n",	index);
																fprintf(stderr,	"							%s	has	only	\"%d\"	vals	per	record.\n",
																								binFileName_,	getNumValRecord());
																exit(-1);
								}
								//	Reset	variable	pointer	to	top	of	current	record
								currValPtr_	=	(unsigned	char	*)	currRecord_;
								currValIdx_	=	0;
								bytesRead_	=	0;

								//	Now	advance	to	index	given
								for	(i	=	0;	i	<	index;	i++)	{
																advanceCurrValPtr();
								}

								//	Now	return	value	at	index
								return	(getNextVal());
}

/**	Get	C-type	for	value	of	current	record	at	given	index	*/
Var::enumType	LogData::getType(int	idx)
{
								return	(typeVal_[idx]);
}

/**	Get	size	in	bytes	for	value	of	current	record	at	given	index	*/
int	LogData::getSize(int	idx)
{
								return	(sizeVal_[idx]);
}

/**
	*	Get	index	of	a	parameter	in	log	header	e.g.
	*
	*	If	header	has	a[0-1].b[3],	x[0-1].y[0-1]
	*	getParamIdx(	"x[1].y[0]"	)	returns	4	since
	*	x[1].y[0]	is	4rth	idx	into
	*	a[0].b[3],	a[1].b[3],	x[0].y[0],	x[0].y[1],	x[1].y[0],	x[1].y[1]
	*
	*	If	this	log	data	is	external	data	just	pass	back	index
	*	to	external	parameter.		No	worries	about	multi-dimensions	:)
	*/
int	LogData::getParamIdx(const	char	*param)
{

								int	i;
								int	valIndex;
								int	offset;

								valIndex	=	0;
								for	(i	=	0;	i	<	nVars_;	i++)	{
																if	(!vars[i]->compareNames(param))	{
																								/*
																									*	We	have	a	match
																									*
																									*	So	the	given	parameter	was	found	in	the	variable
																									*	list	given	in	the	header	file
																									*
																									*	We	-almost-	know	which	variable	index	this	is,
																									*	but	we	must	now	get	the	offset
																									*
																									*	Must	also	make	sure	that	param	is	in	vars[i]	space
																									*	E.g.	a.b[0][1]	is	in	a.b[0-5][1-3]	space
																									*	but		a.b[0][1]	is	NOT	in	a.b[5][6-10]	space
																									*/

																								offset	=	vars[i]->getDimOffset(param);
																								if	(offset	!=	-1)	{
																																valIndex	=	valIndex	+	offset;
																																return	(valIndex);
																								}
																}
																valIndex	=	valIndex	+	vars[i]->getNumDims();
								}

								//	Not	found
								return	(-1);
}

/**	Get	the	unit	associated	with	the	parameter	as	specified	in	Log	header	*/
std::string	LogData::getUnit(char	*param)
{

								int	ii;

								for	(ii	=	0;	ii	<	nVars_;	ii++)	{
																if	(!vars[ii]->compareNames(param))	{
																								return	(vars[ii]->getUnit());
																}
								}

								//	Not	found,	return	NULL
								return	(0);
}

/**	Set	scale	factor	for	parameter	with	given	factor	*/
int	LogData::setScaleFactor(int	paramIdx,	double	factor)
{

								if	(paramIdx	>	nVals_	||	paramIdx	<	0)	{
																fprintf(stderr,	"ERROR:	setScaleFactor	received	paramIdx	"
																								"%d	out	of	bounds.\n",	paramIdx);
																exit(-1);
								}

								vars[varVal_[paramIdx]]->setScaleFactor(factor);
								scaleVal_[paramIdx]	=	factor;
								return	(1);
}

/**	Set	bias	(offset/shift)	for	parameter	with	given	value	*/
int	LogData::setBias(int	paramIdx,	double	bias)
{

								if	(paramIdx	>	nVals_	||	paramIdx	<	0)	{
																fprintf(stderr,	"ERROR:	setBias	received	paramIdx	%d	"
																								"out	of	bounds.\n",	paramIdx);
																exit(-1);
								}

								vars[varVal_[paramIdx]]->setBias(bias);
								biasVal_[paramIdx]	=	bias;
								return	(1);
}

/**	Set	maximal	value	for	parameter	*/
int	LogData::setMax(int	paramIdx,	double	max)
{

								if	(paramIdx	>	nVals_	||	paramIdx	<	0)	{
																fprintf(stderr,	"ERROR:	setMax	received	paramIdx	%d"
																								"	out	of	bounds.\n",	paramIdx);
																exit(-1);
								}

								vars[varVal_[paramIdx]]->setMaxRange(max);
								maxVal_[paramIdx]	=	max;
								return	(1);
}

/**	Set	maximal	value	for	parameter	*/
int	LogData::setMin(int	paramIdx,	double	min)
{

								if	(paramIdx	>	nVals_	||	paramIdx	<	0)	{
																fprintf(stderr,	"ERROR:	setMin	received	paramIdx	%d	"
																								"out	of	bounds.\n",	paramIdx);
																exit(-1);
								}

								vars[varVal_[paramIdx]]->setMinRange(min);
								minVal_[paramIdx]	=	min;
								return	(1);
}

/**	Set	unit	for	parameter
	*		Internally	this	will	set	a	scale	factor	from	unit	in	log
	*		file	to	unit	given	as	argument.		If	the	unit	passed	is
	*		NULL,	the	scale	factor	will	be	set	to	1.0.
	*/
int	LogData::setUnit(int	paramIdx,	std::string	to_units)
{

								if	(paramIdx	>	nVals_	||	paramIdx	<	0)	{
																fprintf(stderr,	"ERROR:	setUnit	received	paramIdx	%d	"
																								"out	of	bounds.\n",	paramIdx);
																exit(-1);
								}

								//	If	no	unit	passed,	set	scale	factor	to	1.0	(default)
								if	(to_units.empty())	{
																unitVal_[paramIdx]	=	1.0;
																return	(-1);
								}

								std::string	from_units	=	vars[varVal_[paramIdx]]->getUnit()	;
								ut_unit	*	from	=	ut_parse(u_system,	from_units.c_str(),	UT_ASCII)	;
								if	(	!from	)	{
												unitVal_[paramIdx]	=	1.0;
												std::cout	<<	"could	not	covert	from	units	"	<<	from_units	<<	std::endl	;
												return	-1	;
								}

								ut_unit	*	to	=	ut_parse(u_system,	to_units.c_str(),	UT_ASCII)	;
								if	(	!to	)	{
												unitVal_[paramIdx]	=	1.0;
												std::cout	<<	"could	not	covert	to	units	"	<<	to_units	<<	std::endl	;
												return	-1	;
								}

								cv_converter	*	converter	=	ut_get_converter(from,to)	;
								if	(	converter	)	{
												biasVal_[paramIdx]	=	cv_convert_double(converter,	0.0	)	;
												unitVal_[paramIdx]	=	cv_convert_double(converter,	1.0	)	-	biasVal_[paramIdx]	;
												cv_free(converter)	;
								}	else	{
												std::cerr	<<	"Units	conversion	error	from	"	<<	from_units	<<	"	to	"	<<	to_units	<<	std::endl	;
												return	-1	;
								}

								ut_free(from)	;
								ut_free(to)	;

								return	(1);
}

/**	Get	the	value	of	a	paramter	at	a	given	time	stamp
	*		Find	value	around	an	epsilon	of	time	stamp
	*		return	-1	if	can't	find	value	with	time	stamp
	*/
int	LogData::getValueAtTime(	int	tIdx,	double	time,
																													int	vIdx,	double*	value)	{

								double	tVal	;

								if	(vIdx	>	nVals_	||	vIdx	<	0)	{
																fprintf(stderr,	"ERROR:	getValueAtTime	received	paramIdx	%d	"
																								"out	of	bounds.\n",	vIdx);
																exit(-1);
								}
								if	(	time	<	0	)	{
																fprintf(stderr,	"ERROR:	getValueAtTime()	received	"
																								"a	time	stamp	of	%lf.\n",	time);
																exit(-1);
								}

								//	Sequentially	look	for	data	associated	with	time	stamp
								//	I'll	optimize	this	if	it	gets	more	use,	or	I	get	more	time
								rewind();
								while	(getNextRecord()	!=	NULL)	{

																//	Get	time	stamp	and	BE	SURE	to	reset	pointers
																tVal	=	*((double	*)	getVal(tIdx));
																currValPtr_	=	(unsigned	char	*)	currRecord_;
																currValIdx_	=	0;
																bytesRead_	=	0;

																if	(	fabs(	tVal	-	time	)	<	0.0000001	)	{
																								*value	=		*((double*)getVal(vIdx))	;
																								return(	1	)	;
																}
								}

								//	Never	found	value	associated	with	time	stamp
								return(-1);
}

/**	Set	the	binary	file	name	e.g.
		*	log_Ball1,	without	the	000,	001	extensions...
		*/
int	LogData::setBinaryFileName(char	*fileName)
{

								delete[]binFileName_;
								binFileName_	=	new	char[strlen(fileName)	+	1];
								strcpy(binFileName_,	fileName);

								return	(0);
}

/**	Get	the	binary	file	name	e.g.
		*	log_Ball1,	without	the	000,	001	extensions...
		*/
char	*LogData::getBinaryFileName()
{
								return	(binFileName_);
}

/**	Default	LogGroup	constructor	*/
LogGroup::LogGroup()
{
								fprintf(stderr,	"ERROR:	Please	use	LogGroup::LogGroup(char*	headerFile)"
																"	constructor.\n");

								exit(0);
}

/**	This	LogGroup	constructor	takes	a	path	to	the	Trick	binary	data	files	*/
LogGroup::LogGroup(char	*path)
{

								int	ret;
								int	i;

								nGroups_	=	0;
								isEOF_	=	0;

								pathToBinaryData_	=	new	char[strlen(path)	+	1];
								strcpy(pathToBinaryData_,	path);

								getHeaders();

								if	(parseLogHeaders()	==	-1)	{
																fprintf(stderr,	"ERROR:	Failed	parsing	log	headers	"
																																"for	\"%s\".\n",	path);
								}
								//	Make	sure	there	is	at	least	one	log	file
								if	(nGroups_	==	0)	{
																fprintf(stderr,	"ERROR:	No	log	header	files	found	in	\"%s\"\n",
																								pathToBinaryData_);
								}
								//	Open	up	data	files
								for	(i	=	0;	i	<	nGroups_;	i++)	{
																ret	=	log[i]->open(pathToBinaryData_);
																if	(	ret	==	-1	)	{
																								fprintf(stderr,	"ERROR:	Failed	open()	for	\"%s\"\n",
																								pathToBinaryData_);
																}
								}
}

/**	Default	destructor	---	broken	:(	*/
LogGroup::~LogGroup()
{

								int	ii;

								delete[]pathToBinaryData_;

								for	(ii	=	0;	ii	<	nGroups_;	ii++)	{
																delete	log[ii];
								}

								if	(headers_)	{
																for	(ii	=	0;	ii	<	nHeaders_;	ii++)	{
																								free(headers_[ii]);
																}
																free(headers_);
								}
}


/**	All	data	will	be	converted	to	doubles	*/
int	LogGroup::convertToDoubleYes()
{
								int	i;

								for	(i	=	0;	i	<	nGroups_;	i++)	{
																log[i]->convertToDoubleYes();
								}

								return	(0);
}

/**	Get	number	of	records	in	this	log	group
	*		Return	the	max	number	of	records	among	all	the	Trick	logs
	*/
int	LogGroup::getNumRecords()
{
								int	i;
								int	ret;
								int	max;

								max	=	0;
								for	(i	=	0;	i	<	nGroups_;	i++)	{
																ret	=	log[i]->getNumRecords();
																if	(ret	>	max)	{
																								max	=	ret;
																}
								}

								return	(max);
}

/**	Enable	statistics	(min/max...)
	*		This	will	actually	calculate	stats	on	the	first	pass
	*		Find	absolute	min/max	value	for	a	param
	*		This	will	move	the	file	offsets	in	the	binary	files
	*		Maybe	we'll	add	the	"ftell()"	later!
	*/
void	LogGroup::getMinMaxStats(const	char	*param,
																														double	*tmin,	double	*min,
																														double	*tmax,	double	*max)
{

								int	xIdx,	xLogIdx;
								int	yIdx,	yLogIdx;
								int	ret	;

								//	We	need	to	insure	all	are	doubles	for	these	calculations
								convertToDoubleYes();

								ret	=	getParamIdx("sys.exec.out.time",	&xIdx,	&xLogIdx,
																				param,	&yIdx,	&yLogIdx);
								if	(	ret	==	-1	)	{
																fprintf(stderr,	"ERROR:	Couldn't	get	min/max	statistics	"
																																"	due	to	bad	log	indices.\n");
																exit(-1);
								}

								log[yLogIdx]->getMinMaxStats(yIdx,	tmin,	min,	tmax,	max);
}

/**	Set	scale	factor	for	given	paramter	and	given	factor	*/
int	LogGroup::setScaleFactor(int	logIdx,	int	paramIdx,	double	factor)
{

								log[logIdx]->setScaleFactor(paramIdx,	factor);

								return	(1);
}


/**	Set	bias	(offset/shift)	parameter	*/
int	LogGroup::setBias(int	logIdx,	int	paramIdx,	double	bias)
{

								log[logIdx]->setBias(paramIdx,	bias);

								return	(1);
}

/**	Set	maximum	value	for	parameter	*/
int	LogGroup::setMax(int	logIdx,	int	paramIdx,	double	max)
{

								log[logIdx]->setMax(paramIdx,	max);

								return	(1);
}

/**	Set	minimum	value	for	parameter	*/
int	LogGroup::setMin(int	logIdx,	int	paramIdx,	double	min)
{

								log[logIdx]->setMin(paramIdx,	min);

								return	(1);
}

/**	Set	unit	for	parameter
	*		@see	LogData::setUnit()
	*/
int	LogGroup::setUnit(int	logIdx,	int	paramIdx,	std::string	u)
{

								log[logIdx]->setUnit(paramIdx,	u);

								return	(1);
}

/**	Set	unit	for	parameter
	*		@see	LogData::setUnit()
	*/
int	LogGroup::setUnit(const	char	*paramName,	std::string	u)
{

								int	xIdx,	xLogIdx;
								int	yIdx,	yLogIdx;
								int	ret	;

								ret	=	getParamIdx("sys.exec.out.time",	&xIdx,	&xLogIdx,
																				paramName,	&yIdx,	&yLogIdx);
								if	(	ret	==	-1	)	{
																fprintf(stderr,	"ERROR:	In	setUnit()	died	with	faulty	"
																																"parameter	name.\n");
																exit(-1);
								}

								log[yLogIdx]->setUnit(yIdx,	u);

								return	(1);
}

/**	Get	the	value	of	a	paramter	at	a	given	time	stamp
	*		Return	value	around	an	epsilon	of	time	stamp
	*/
int	LogGroup::getValueAtTime(	const	char	*paramName,
																														double	time,	double*	value	)	{

								int	tIdx,	tLogIdx;
								int	yIdx,	yLogIdx;
								double	vv	;
								int	rr	;
								int	ret	;

								//	Get	indices	based	on	paramName
								ret	=	getParamIdx("sys.exec.out.time",	&tIdx,	&tLogIdx,
																				paramName,	&yIdx,	&yLogIdx);
								if	(	ret	==	-1	)	{
																fprintf(stderr,	"ERROR:	In	getValueAtTime()	died	with	faulty	"
																																"parameter	name.\n");
																exit(-1);
								}

								//	Get	the	value	at	time
								rr	=	log[yLogIdx]->getValueAtTime(tIdx,	time,
																																										yIdx,	&vv)	;
								//		If	value	with	timestamp	found
								if(rr	>	0){
												*value	=	vv	;
								}
								
								return(rr);

}

/**	Move	all	file	pointers	of	all	data	log	files	back	to	start	*/
int	LogGroup::rewind()
{

								int	i;

								//	Make	sure	there	is	at	least	one	log	file
								if	(nGroups_	==	0)	{
																fprintf(stderr,	"ERROR:	No	log	files	specified.\n");
																fprintf(stderr,	"							GetNextRecord	aborting.\n");
																exit(-1);
								}

								isEOF_	=	0;

								//	Rewind	each	log	data	group
								for	(i	=	0;	i	<	nGroups_;	i++)	{
																log[i]->rewind();
								}

								return	(0);
}

/**	Get	next	record	across	data	log	files	by
	*		advancing	ALL	files	in	log	group	to	next	record
	*/
char	*LogGroup::getNextRecord()
{

								int	i;
								int	eof;
								char	*record;

								//	Make	sure	there	is	at	least	one	log	file
								if	(nGroups_	==	0)	{
																fprintf(stderr,	"ERROR:	No	log	files	specified.\n");
																fprintf(stderr,	"							GetNextRecord	aborting.\n");
																exit(-1);
								}

								//	Get	next	record	in	each	group
								eof	=	0;
								for	(i	=	0;	i	<	nGroups_;	i++)	{
																if	((record	=	log[i]->getNextRecord())	==	(char*)NULL)	{
																								eof++;
																}
								}

								//	Return	NULL	if	all	groups	data	is	read
								if	(	eof	>=	nGroups_	)	{
																isEOF_	=	1;
																return	((char	*)	NULL);
								}	else	{
																return	((char	*)	1);
								}
}

/**
	*	Given	xVarName	&	yVarName;	return	xIdx,	yIdx,	xLogDataIdx,	yLogDataIdx
	*
	*	x&y	Idx	are	parameter	indices	into	the	x&y	LogData	files
	*
	*	xVarName	&	yVarName	may	be	in	different	log	data	groups,
	*	so	it	get's	a	little	complicated.		Also	there	may
	*	more	than	one	param	with	same	xVarName	(time)	or	same	yVarName!
	*
	*	The	algorithm	here	will	find	first	group	with	yVarName,	and	then
	*	see	if	xVarName	is	in	same	group.		If	all	possibilities	of
	*	getting	x	&	y	out	same	log	data	group	are	exhausted,	it	will
	*	then	choose	y	to	be	last	group	it	found	it	in,	then	find
	*	first	x	with	xVarName.		Thus	x	&	y	will	be	pulled	out	of
	*	different	log	data	groups.		If	x	&	y	not	found,	it	will	issue	an
	*	error	and	exit.
	*/
int	LogGroup::getParamIdx(const	char	*xVarName,	int	*xIdx,	int	*xLogDataIdx,
																										const	char	*yVarName,	int	*yIdx,	int	*yLogDataIdx)
{
								int	i;
								int	yFoundIdx;

								//	Search	by	log	data	groups	for	x	&	y	parameters
								*xLogDataIdx	=	-1;
								*yLogDataIdx	=	-1;
								yFoundIdx	=	-1;
								for	(i	=	0;	i	<	nGroups_;	i++)	{

																if	((*yIdx	=	log[i]->getParamIdx(yVarName))	!=	-1)	{
																								*yLogDataIdx	=	i;
																								yFoundIdx	=	*yIdx;
																								if	((*xIdx	=	log[i]->getParamIdx(xVarName))	!=	-1)	{
																																//	Yippee.		x	&	y	in	same	group
																																*xLogDataIdx	=	*yLogDataIdx;
																																break;
																								}
																}
								}

								if	(*xLogDataIdx	==	-1)	{
																//	So	x	&	y	not	in	same	group

																if	(yFoundIdx	!=	-1)	{
																								//	Let's	see	if	xVarName	is	in	a	different
																								//	log	data	group	than	y
																								//	Choose	first	one	you	find
																								*yIdx	=	yFoundIdx;
																								for	(i	=	0;	i	<	nGroups_;	i++)	{
																																*xIdx	=	log[i]->getParamIdx(xVarName);
																																if	(*xIdx	!=	-1)	{
																																								*xLogDataIdx	=	i;
																																								break;
																																}
																								}
																								if	(*xLogDataIdx	==	-1)	{
																																//	Never	found	x	parameter
																																fprintf(stderr,	"ERROR:	Couldn't	find	x	"
																																								"param	\"%s\".\n",	xVarName);
																																fprintf(stderr,	"							Param	is	not	in	log	"
																																								"header.\n");
																																return(-1);
																								}
																}	else	{
																								//	Never	found	y	parameter
																								fprintf(stderr,
																																"ERROR:	Couldn't	find	y	param	\"%s\".\n"
																																"	Param	is	not	in	log	header	for	"
																																"run:	\"%s\".\n",
																																yVarName,	getDataDir());
																								return(-1);
																}
								}

								return	(0);
}

//	Private	function	for	caching	a	list	of	headers	in	a	RUN_	dir
int	LogGroup::getHeaders()
{
								DIR	*dirp;
								struct	dirent	*dp;

								char*	full_header_name	;

								unsigned	int	maxLengthHeaderName;

								int	i;
								int	len	;

								//	Open	up	RUN	dir
								if	((dirp	=	opendir(pathToBinaryData_))	==	NULL)	{
																fprintf(stderr,	"ERROR:	Couldn't	open	\"%s\"	for	reading.	\n",
																								pathToBinaryData_);
																fprintf(stderr,	"Maybe	LogGroup	constructer	wasn't	done.	\n");
																exit(-1);
								}
								//	Count	num	headers,	and	max	file	name	length	in	RUN	dir
								maxLengthHeaderName	=	0;
								nHeaders_	=	0;
								while	((dp	=	readdir(dirp))	!=	NULL)	{
																if	(strstr(dp->d_name,	".header")	!=	NULL)	{
																								len		=	strlen(dp->d_name)	+
																															strlen(pathToBinaryData_)	+	2	;
																								full_header_name	=	(char*)	malloc	(	len	)	;
																								snprintf(full_header_name,	len,	"%s/%s",
																																pathToBinaryData_,	dp->d_name);
																								if	(!strncmp(dp->d_name,	"log_",	4)	&&
																													is04BinaryHeader_(full_header_name))	{
																																if	(strlen(dp->d_name)	>	maxLengthHeaderName)	{
																																								maxLengthHeaderName	=
																																												strlen(dp->d_name);
																																}
																																nHeaders_++;
																								}
																								free(full_header_name)	;
																}
								}
								maxLengthHeaderName++;		//	Add	null	character	onto	length

								//	Malloc	room	for	header	names
								rewinddir(dirp);
								headers_	=	(char	**)	malloc((nHeaders_)	*	sizeof(char	*));
								i	=	0;
								while	((dp	=	readdir(dirp))	!=	NULL)	{
																if	(strstr(dp->d_name,	".header")	!=	NULL)	{
																								if	(!strncmp(dp->d_name,	"log_",	4))	{
																																headers_[i]	=	(char	*)	malloc
																																				(maxLengthHeaderName);
																																strcpy(headers_[i],	dp->d_name);
																																i++;
																								}
																}
								}

								closedir(dirp);

								return	(0);
}

bool	LogGroup::is04BinaryHeader_(	const	char*	file_name	)
{
								FILE*	fp	;
								char*	rc	;
								char	str[4][256]	;
								char	line[1024]	;


								fp	=	fopen(	file_name,	"r"	)	;
								if	(	fp	==	NULL	)	{
																fprintf(stderr,	"Died	while	attempting	to	open	\"%s\".\n",
																								file_name);
																exit(-1)	;
								}

								//	Either	the	first	or	second	line	must	have
								//	*	byte_order	is	(little|big)_endian
								//	where	*	is	some	text
								rc	=	fgets(line,	1024,	fp);
								if	(	rc	==	NULL	)	{
																return	0	;
								}

								if	(	sscanf(line,	"%s	%s	%s	%s",
																										str[0],	str[1],	str[2],	str[3])	==	4	)	{

																if	(!strcmp(str[1],	"byte_order"))	{
																								return	1	;
																}
								}

								//	If	we	get	here,	the	header	is	not	an	04	header
								return	0	;

}








/**	Returns	number	of	Trick	Log	Data	groups	in	RUN_	dir	*/
int	LogGroup::getNumGroups()
{
								return	(nGroups_);
}

/**	Returns	path	to	Binary	data	(RUN_	dir)	*/
char	*LogGroup::getDataDir()
{
								return	(pathToBinaryData_);
}

void	LogGroup::setEOF_()
{
								isEOF_	=	1	;
}

int	LogGroup::isEOF()
{
								int	ii	;
								int	nGroupsToCheck	;
								int	eof	;

								if	(	isEOF_	)	{
																return(	1	)	;
								}

								nGroupsToCheck	=	nGroups_	;

								eof	=	1	;
								for	(ii	=	0;	ii	<	nGroupsToCheck	;	ii++)	{
																if	(	!	log[ii]->isEOF()	)	{
																								eof	=	0	;
																								break	;
																}
								}
								isEOF_	=	eof	;

								return(	eof	)	;
}

IterMultiRun::IterMultiRun(	LogGroup*	lg1,	set<int>&	logIndices1,
																												LogGroup*	lg2,	set<int>&	logIndices2,
																												double	frequency,
																												double	startTime,	double	stopTime	)	{

								//	Two	iterators	for	iterating	over	both	runs
								iter1_	=	new	IterPreserveTime(lg1,	logIndices1,
																															frequency,	startTime,	stopTime);
								iter2_	=	new	IterPreserveTime(lg2,	logIndices2,
																															frequency,	startTime,	stopTime);

								startTime_	=	startTime;
								stopTime_	=	stopTime;

								logIndices1_	=	logIndices1;
								logIndices2_	=	logIndices2;
}

IterMultiRun::~IterMultiRun()
{
								delete	iter1_	;
								delete	iter2_	;
}

void	IterMultiRun::begin()
{
								iter1_->begin();
								iter2_->begin();
								return	;
}

bool	IterMultiRun::end()
{
								if	(	iter1_->end()	||	iter2_->end()	)	{
																return(	1	)	;
								}	else	{
																return(	0	)	;
								}
}

void	IterMultiRun::operator++(int)	{

								double	minTime,	t1,	t2;

								//	No	more	iteration	to	be	done	if	at	end
								if	(	end()	)	{
																return	;
								}

								//	Get	all	values	in	all	data	log	groups
								//	Also	get	mininum	time	stamp	from	all	log	data	files
								t1	=	iter1_->getTime();
								t2	=	iter2_->getTime();
								if	(	t1	<=	t2	)	{
																minTime	=	t1	;
								}	else	{
																minTime	=	t2	;
								}

								//	Take	a	step	through	time	(sounds	like	a	song)
								if	(	fabs(t1	-	minTime)	<	0.0000001	)	{
																(*iter1_)++	;
								}
								if	(	fabs(t2	-	minTime)	<	0.0000001	)	{
																(*iter2_)++	;
								}

								//	Now	that	a	time	step	has	occured
								//	Get	the	new	mininum	time	stamp
								t1	=	iter1_->getTime();
								t2	=	iter2_->getTime();
								if	(	t1	<=	t2	)	{
																minTime	=	t1	;
								}	else	{
																minTime	=	t2	;
								}

								//	Insure	that	time	stamp	remains	same	across	runs
								if	(	fabs(t1-t2)	>	0.0000001	)	{
																(*this)++	;
								}

								//	If	time	<	start	time	requested	by	user,
								//	continue	marching	through	time	until	start	time	reached
								if	(minTime	<	startTime_)	{
																(*this)++	;
								}
}

IterPreserveTime::IterPreserveTime(	LogGroup*	lg,	set<int>&	logIndices,
																																				double	frequency,
																																				double	startTime,	double	stopTime	)
{
								unsigned	int	ii	;

								lg_	=	lg	;
								startTime_	=	startTime	;
								stopTime_	=	stopTime	;
								logIndices_	=	logIndices	;
								frequency_	=	frequency	;
								frequencyMultiple_	=	1.0	;

								//	If	there	is	an	external	program,	make	sure	there	is	at	least
								//	one	log	group	other	than	the	external	one	in	logIndices
								if	(	logIndices.size()	==	1	)	{
																logIdx_	=	logIndices.begin()	;
								}

								eof_	=	new	bool[logIndices.size()]	;
								for	(ii	=	0;	ii	<	logIndices.size()	;	ii++)	{
																eof_[ii]	=	0	;
								}

								//	Allocate	a	time	idx	for	log	member
								timeIdx_	=	new	int[logIndices.size()];
								for	(ii	=	0;	ii	<	logIndices.size();	ii++	)	{
																//	TODO:	get	real	time	stamp
																//	It	is	safe	for	now	to	assume	it's
																//	the	first	param	logged
																timeIdx_[ii]	=	0	;
								}

								currTime_	=	new	double[logIndices.size()]	;
}

IterPreserveTime::~IterPreserveTime()
{
}


void	IterPreserveTime::begin()
{
								double	timeStamp;

								lg_->rewind();

								//	Reset	frequency
								if	(	frequency_	==	0.0	)	{
																nextFrequencyCheck_	=	DBL_MAX;
								}	else	{
																if	(	startTime_	==	0.0	)	{
																								nextFrequencyCheck_	=	startTime_	+	frequency_	;
																}	else	{
																								nextFrequencyCheck_	=	startTime_	;
																}
								}
								frequencyMultiple_	=	1.0	;

								//	Need	to	initialize	by	reading	in	all	records
								for	(logIdx_	=	logIndices_.begin()	;	logIdx_	!=	logIndices_.end()	;
																logIdx_++)	{

																lg_->log[*logIdx_]->getNextRecord()	;
								}

								//	If	time	<	start	time	requested	by	user,
								//	continue	marching	through	time	until	start	time	reached
								timeStamp	=	0.0	;
								while	(	timeStamp	<	startTime_	)	{
																(*this)++	;
																timeStamp	=	getTime();
								}
}

bool	IterPreserveTime::end()
{
								if	(	lg_->isEOF_	)	{
																return(	1	)	;
								}	else	{
																return(	0	)	;
								}
}

void	IterPreserveTime::operator++(int)	{

								//	I	did	it	this	way	to	avoid	recursion
								//	The	stack	got	out	of	hand	with	recursion
								while	(	!	stepInTime()	)	{
								}
}

//	This	returns	minimum	time	stamp	across	all	data	logs
//
//	If	one	log	has	logged	more	data	in	time,
//	(i.e.	one	log	has	100	seconds	of	data,	another	300	seconds)
//	by	default	getTime()	will	ignore	the	log	that	has	expired
//	all	of	its	data.		Therefore,	the	data	from	the	short	log
//	will	not	force	the	longer	to	be	truncated.
//
//	In	the	case	of	error	plotting,	you	do	not	want	any	extra
//	data	points	if	the	time	stamps	do	not	match
double	IterPreserveTime::getTime(	)	{

								double	minTime	=	DBL_MAX;
								int	ii	=	0	;

								for	(logIdx_	=	logIndices_.begin()	;	logIdx_	!=	logIndices_.end()	;
																logIdx_++)	{

																if	(	!	lg_->log[*logIdx_]->isEOF()	)	{
																								currTime_[ii]	=	*((double	*)	lg_->log[*logIdx_]->
																																																getVal(timeIdx_[ii]))	;
																}	else	{
																								currTime_[ii]	=	DBL_MAX	;
																}

																if	(currTime_[ii]	<	minTime)	{
																								minTime	=	currTime_[ii];
																}
																ii++	;
								}

								return(	minTime	)	;
}

//	Advance	through	records	in	each	data	log	synchronously
//
//	Example:	If	data	log	"A"	is	recorded	at	1.0	second,
//	and	"B"	at	10	seconds.		Both	will	advance	at	time	zero.
//	Then	at	time=1.0,	log	"A"	will	have	a	record	read.		"B"	will	not	be	read.
//	At	time=2.0	through	time=9.0	the	same	will	occur.	A	advances,	B	stays	put.
//	At	time=10.0,	both	A	and	B	will	be	read	from.		Both	will	advance.
//	This	keeps	the	data	on	the	same	time	stamp	on	like	intervals.
//
//	If	A	has	more	records	than	B	in	time,	by	default,	read	all	of	A's
//	data.		E.g.	'A'	logged	for	300	seconds,		while	'B'	100	seconds,	by	default
//	walk	through	all	of	A's	data
int	IterPreserveTime::stepInTime()	{


#define	DPLOG_ABS(x)	((x)	<	0.0	?	-(x)	:	(x))

#define	NEXTFREQ_EQUALS_MINTIME	\
								(DPLOG_ABS(nextFrequencyCheck_	-	minTime)	<	0.000000001)

#define	NEXTFREQ_LESSTHAN_MINTIME	\
								((nextFrequencyCheck_	-	minTime	<	0.0)	&&	!	NEXTFREQ_EQUALS_MINTIME)

#define	NEXTFREQ_INCR	\
								frequencyMultiple_++	;	\
								nextFrequencyCheck_	=	frequencyMultiple_*frequency_	;


								int	ii	;
								double	minTime	;
								int	isEOF	;
								char*	rec	;

								//	Get	all	values	in	all	data	log	groups
								//	Also	get	mininum	time	stamp	from	all	log	data	files
								minTime	=	getTime();

								//	Take	a	step	through	time	(sounds	like	a	song)
								//	Check	for	EOF	along	the	way
								isEOF	=	1	;
								ii	=	0	;
								for	(logIdx_	=	logIndices_.begin()	;	logIdx_	!=	logIndices_.end()	;
																logIdx_++)	{
																if	(currTime_[ii]	==	minTime	)	{

																								rec	=	lg_->log[*logIdx_]->getNextRecord()	;
																								if	(rec)	{
																																isEOF	=	0	;
																								}
																}
																ii++	;
								}
								if	(isEOF)	{
																lg_->isEOF_	=	1	;
																return(1)	;
								}

								//	Now	that	a	time	step	has	occured
								//	Get	the	new	mininum	time	stamp	and	associated
								//	values	from	all	log	data	files
								minTime	=	getTime();

								//	Step	depending	on	frequency
								if	(minTime	<=	stopTime_)	{

																//	Handle	Frequency	(if	specified)
																if	(	nextFrequencyCheck_	!=	DBL_MAX	)	{

																								if	(	NEXTFREQ_EQUALS_MINTIME	)	{

																																//	Frequency	synced	with	a	data	point
																																//	Stops	recursion
																																NEXTFREQ_INCR	;

																								}	else	if	(	minTime	-	nextFrequencyCheck_	>	0	)	{

																																//	Mintime	exceeded	nextFreqCheck
																																//	Frequency	must	be	higher	than
																																//	data	frequency

																																//	Bump	nextFrequencyCheck	past	minTime
																																while	(	NEXTFREQ_LESSTHAN_MINTIME	)	{

																																								NEXTFREQ_INCR	;
																																}

																																if	(	NEXTFREQ_EQUALS_MINTIME	)	{

																																							//	We	synced
																																							NEXTFREQ_INCR	;

																																}	else	{

																																								//	No	sync,	keep	iterating	through	data
																																								return(0);
																																}
																								}	else	{

																																//	Mintime	is	less	than	nextFreq	check
																																//	Need	to	iterate	again
																																return(0)	;
																								}
																}
								}

								if	(minTime	>	stopTime_)	{
																lg_->isEOF_	=	1	;
																return(1)	;
								}

								//	Tells	caller	that	time	step	did	indeed	occur
								return(1)	;
}
