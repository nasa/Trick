/**
				A	class	interface	for	variables
				@author	Keith	Vetter
				@version	May	2002
*/


#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<float.h>

#include	<string.h>
#include	<iostream>

#include	"var.h"

#define	VAR_DELETE	\
							delete[]	str	;		\
							delete[]	v1name	;	\
							delete[]	v2name	;	\
							delete[]	offsets	;	\
							delete[]	sizedims	;	\
							delete[]	sizeproducts	;	\
							delete[]	v1.dims	;	\
							delete[]	v2.dims	;	\
							for	(	i	=	0	;	i	<	nParms	;	i++	)	{		\
										delete[]	v1.parName[i]	;	\
										delete[]	v2.parName[i]	;	\
							}	\
							delete[]	v1.parName	;	\
							delete[]	v2.parName	;

/**	Variable	Constructor	*/
Var::Var()	{

							scaleFactor_	=	1.0	;
							bias_	=	0	;
							minRange_	=	-DBL_MAX	;
							maxRange_	=	DBL_MAX	;
							varType_	=	NONE	;
							byteSize_	=	0	;
							isProgramOutput	=	0	;

}

/**	Var	Destructor	*/
Var::~Var()	{
}

/**	Copy	constructor	*/
Var::Var(const	Var	&src)	{

							varName_	=	src.varName_	;
							shortName_	=	src.shortName_	;

}

/**
	*	Get	number	dimensions	a	variable	has	e.g.	a[8].b[9].c	has	72	dimensions
	*
	*	varName	-must-	be	set	before	calling	this
	*/
int	Var::getNumDims()	{

							if	(	varName_[0]	==	'\0'	)	{
															printf("ERROR:	Must	set	varName	before	calling	getNumDims().\n");
															return(	-1	)	;
							}

							//	numDimensions	was	previously	calculated
							return	(	nDimensions_	)	;
}

/**	Get	the	string	name	of	var	e.g.	"a[8].b[9].c"	*/
const	char*	Var::getVarName(		)	{
							return(	varName_.c_str()	)	;
}

const	char*	Var::getTimeName(		)	{
							return(	timeName_.c_str()	)	;
}

string	Var::setTimeName(const	char	*	name	)	{
							timeName_	=	name	;
							return(	timeName_)	;
}

/**		Get	variable	name	e.g.	getVarName()	returns	the	string	"a[8].b[9].c"	*/
double	Var::getScaleFactor()	{
							return	(	scaleFactor_	)	;
}

/**	Get	variable's	bias	(offset	or	shift)
	*
	*		bias	is	the	shift	value	for	a	variable
	*		So	for	a	list	of	values	1.0,	2.0,	3.0	with	a	bias	of	say	8.0
	*		My	list	becomes	9.0,	10.0,	11.0
	*/
double	Var::getBias()	{
							return	(	bias_	)	;
}

/**	Get	variable's	minimal	possible	value
	*
	*/
double	Var::getMinRange()	{
							return	(	minRange_	)	;
}

/**	Get	variable's	maximal	possible	value
	*
	*/
double	Var::getMaxRange()	{
							return	(	maxRange_	)	;
}

/**	Get	variable's	unit
	*/
std::string	Var::getUnit()	{
							return	(	unit_	)	;
}

std::string	Var::setUnit(std::string	unit_name)	{
							unit_	=	unit_name	;
							return	(	unit_	)	;
}

std::string	Var::getTimeUnit()	{
							return	(	timeUnit_	)	;
}

std::string	Var::setTimeUnit(std::string	unit_name)	{
							timeUnit_	=	unit_name	;
							return	(	timeUnit_	)	;
}

/**
	*	Return	variable	name	without	dimensions
	*	e.g.	if	varName	==	a[0-9].b[1-2].c
	*									a.b.c	will	be	returned
	*/
const	char*	Var::getVarNameNoDims(	)	{

							int	ii,	jj	;
							int	len;
							char	*	temp_name	;

							//	Reallocate	short	name
							len	=	varName_.length()	;
							temp_name	=	new	char[len	+	1]	;

							jj	=	0	;
							for	(	ii	=	0;	ii	<	len	;	ii++	)	{
															temp_name[jj]	=	varName_[ii];
															if	(	varName_[ii]	==	'['	||	varName_[ii]	==	'('	)	{
																							jj--;
																							while	(	varName_[ii]	!=	']'	&&
																															varName_[ii]	!=	')'	&&	ii	<	len	)	{
																															ii++;
																							}
															}
															jj++;
							}
							temp_name[jj]	=	'\0'	;
							shortName_	=	temp_name	;
							delete[]	temp_name	;

							return(shortName_.c_str());
}

/**	Compare	two	names	(ignoring	dimensions)		Returns	0	if	same	*/
int	Var::compareNames(	const	char*	name	)	{

							int	ret	;

							Var*	v	=	new	Var()	;
							v->setType((char	*)"void");																		//	All	new	vars	need	a	type
							v->setVarName(name)	;
							if	(	v->getSize()	==	-1	)	{
															fprintf(stderr,	"ERROR:	\"%s\"	is	not	a	valid	variable	name.\n",
																																name);
															return(	-1	);
							}

							ret	=	strcmp(this->getVarNameNoDims(),v->getVarNameNoDims());
							delete	v	;

							return(ret);
}

/**
	*	Returns	index	offset	of	name	relative	to	Var	(doesn't	return	byte	offset)
	*
	*	e.g.	Say	the	variable	is	a.b[3-24].c[8],
	*						getDimOffset("a.b[7].c[8]")	returns	4
	*						since	a.b[7]	is	the	4rth	element	of	a.b[3-24].c[8]	.
	*						a.b[3].c[8]	is	0th,	a.b[4].c[8]	is	the	1rst	etc...
	*/
int	Var::getDimOffset(	const	char*	param	)	{

							unsigned	int	i,j,k	;
							char*	str	;
							char*	v1name	;
							char*	v2name	;

							//	Need	this	for	sizing	of	arrays
							const	unsigned	int	nParms	=	getNumParms()	;

							enum	DIMENSION	{	MULTI,	SINGLE,	NONE	}	;

							typedef	struct		{
															enum	DIMENSION	dimensional	;
															int	lower	;
															int	upper	;
							}	DIM	;

							struct	variable	{

															//	a[0-1].b[8].c	has	3	params	->	"a"	"b"	"c"
															unsigned	int		numParams	;

															/*
																*	a[0-1].b[8].c	has	(0,1),(8,8),(?,?)	as	dims
																*	a,	b	&	c	are	MULTI,	SINGLE	&	NONE	dimensional
																*/
															DIM	*	dims;

															//	a[0-1].b[8].c	parNames	"a[0-1]"	"b[8]"	&	"c"
															char**	parName	;

							}	v1,	v2	;

							int	offset	;
							int*	offsets						=	new	int[nParms]	;		//	Used	for	calculating	offset
							int*	sizedims					=	new	int[nParms];			//													""
							int*	sizeproducts	=	new	int[nParms]	;		//													""
							v1.dims											=	new	DIM[nParms]	;
							v2.dims											=	new	DIM[nParms]	;

							//	Allocate	parName	2-d	array
							v1.parName								=	new	char*[nParms]	;
							v2.parName								=	new	char*[nParms]	;
							int	len	=	varName_.size()	+	1	;
							for	(	i	=	0	;	i	<	nParms	;	i++	)	{
															v1.parName[i]	=	new	char[len]	;
															v2.parName[i]	=	new	char[len]	;
							}

							//	Do	some	allocation
							//	Added	nParms,	since	multidims	expanded	outgrow	original	string
							//	e.g.	a[0-2][2-3].b[0-9].c	->	a[0-2].[2-3].b[0-9].c
							str				=	new	char[varName_.size()+1+nParms]	;
							v1name	=	new	char[varName_.size()+1+nParms]	;
							v2name	=	new	char[strlen(param)+1+nParms]	;

							//	Initialize	v1	and	v2	strings
							strcpy(v1name,	varName_.c_str());
							strcpy(v2name,	param);

							/*
								*	To	handle	multi	dim	specs,	do	the	following	ugliness
								*	change	things	like	a[0].b[0-2][9-18][3-4]	->	a[0].b[0-2].[9-18].[3-4]
								*	Unfortunately,	this	was	an	afterthought,	and	it	was	hurried	(sorry)
								*/
							j	=	0	;
							for	(	i	=	0	;	i	<	strlen(v1name)	;	i++	)	{
															if	(	v1name[i]	==	']'	&&	v1name[i+1]	==	'['	)	{
																							str[j]	=	']'	;	j++;
																							str[j]	=	'.'	;	j++;
															}	else	{
																							str[j]	=	v1name[i]	;	j++	;
															}
							}
							str[j]	=	'\0'	;
							strcpy(v1name,	str);

							j	=	0	;
							for	(	i	=	0	;	i	<	strlen(v2name)	;	i++	)	{
															if	(	v2name[i]	==	']'	&&	v2name[i+1]	==	'['	)	{
																								str[j]	=	']'	;	j++;
																								str[j]	=	'.'	;	j++;
															}	else	{
																								str[j]	=	v2name[i]	;	j++	;
															}
						}
						str[j]	=	'\0'	;
						strcpy(v2name,	str);


						/*	Get	param	names	out	of	variable	name	*/
						/*	a[0-1].b[8].c	->	"a[0-1]",	"b[8]",	"c"	*/
						j	=	0	;
						v1.numParams	=	0	;
						for	(	i	=	0	;	i	<	strlen(v1name)	;	i++	)	{
															str[j]	=	v1name[i]	;	j++	;
															if	(	v1name[i]	==	'.'	)	{
																							str[j-1]	=	'\0'	;
																							strcpy(v1.parName[v1.numParams],	str);
																							j	=	0	;
																							v1.numParams++	;
															}
						}
						str[j]	=	'\0'	;
						strcpy(v1.parName[v1.numParams],	str);
						v1.numParams++	;

						/*	Get	dimensions	for	each	param	name	*/
						for	(	i	=	0	;	i	<	v1.numParams;	i++	)	{

															v1.dims[i].dimensional	=	NONE	;
															v1.dims[i].lower	=	-1	;
															v1.dims[i].upper	=	-1	;

															for	(	j	=	0	;	j	<	strlen(v1.parName[i])	;	j++	)	{

																							if	(	v1.parName[i][j]	==	'['	)	{
																															j++	;

																															/*	Assign	lower	dimension	of	param	*/
																															k	=	0	;
																															while	(	isdigit(	v1.parName[i][j]	)	)	{
																																							str[k]	=	v1.parName[i][j]	;
																																							k++	;	j++	;
																															}
																															str[k]	=	'\0'	;
																															v1.dims[i].lower	=	atoi(str)	;

																															/*	Handle	upper	dimension	*/
																															if	(	v1.parName[i][j]	==	'-'	)	{
																																							/*	Multi-dimensional	*/
																																							v1.dims[i].dimensional	=	MULTI	;
																																							k	=	0	;
																																							j++	;
																																							while	(	isdigit(	v1.parName[i][j]	)	)	{
																																															str[k]	=	v1.parName[i][j]	;
																																															k++	;	j++	;
																																							}
																																							str[k]	=	'\0'	;
																																							v1.dims[i].upper	=	atoi(str)	;
																															}	else	{
																																							/*	Single-dimesional	*/
																																							v1.dims[i].dimensional	=	SINGLE	;
																																							v1.dims[i].upper	=	v1.dims[i].lower	;
																															}
																							}
															}
						}

						/*	Get	param	names	out	of	variable	name	*/
						/*	a[0-1].b[8].c	->	"a[0-1]",	"b[8]",	"c"	*/
						j	=	0	;
						v2.numParams	=	0	;
						for	(	i	=	0	;	i	<	strlen(v2name)	;	i++	)	{
															str[j]	=	v2name[i]	;	j++	;
															if	(	v2name[i]	==	'.'	)	{
																							str[j-1]	=	'\0'	;
																							strcpy(v2.parName[v2.numParams],	str);
																							j	=	0	;
																							v2.numParams++	;
															}
						}
						str[j]	=	'\0'	;
						strcpy(v2.parName[v2.numParams],	str);
						v2.numParams++	;


						/*	Get	dimensions	for	each	param	name	*/
						for	(	i	=	0	;	i	<	v2.numParams;	i++	)	{
															v2.dims[i].dimensional	=	NONE	;
															v2.dims[i].lower	=	-1	;
															v2.dims[i].upper	=	-1	;
															for	(	j	=	0	;	j	<	strlen(v2.parName[i])	;	j++	)	{

																							if	(	v2.parName[i][j]	==	'['	)	{
																															j++	;

																															/*	Assign	lower	dimension	of	param	*/
																															k	=	0	;
																															while	(	isdigit(	v2.parName[i][j]	)	)	{
																																							str[k]	=	v2.parName[i][j]	;
																																							k++	;	j++	;
																															}
																															str[k]	=	'\0'	;
																															v2.dims[i].lower	=	atoi(str)	;

																															/*	Handle	upper	dimension	*/
																															if	(	v2.parName[i][j]	==	'-'	)	{
																																							/*	Multi-dimensional	*/
																																							v2.dims[i].dimensional	=	MULTI	;
																																							k	=	0	;
																																							j++	;
																																							while	(	isdigit(	v2.parName[i][j]	)	)	{
																																															str[k]	=	v2.parName[i][j]	;
																																															k++	;	j++	;
																																							}
																																							str[k]	=	'\0'	;
																																							v2.dims[i].upper	=	atoi(str)	;
																															}	else	{
																																							/*	Single-dimesional	*/
																																							v2.dims[i].dimensional	=	SINGLE	;
																																							v2.dims[i].upper	=	v2.dims[i].lower	;
																															}
																							}
															}
						}

						/*	First	make	sure	that	v2	is	in	v1's	space	*/
						for	(	i	=	0	;	i	<	v1.numParams	;	i++	)	{
															if	(	v1.dims[i].dimensional	==	MULTI	)	{
																							if	(	v2.dims[i].lower	<	v1.dims[i].lower	||
																															v2.dims[i].upper	>	v1.dims[i].upper	)		{
																															VAR_DELETE	;
																															return	(	-1	)	;	/*	v2	not	in	v1's	space	*/
																							}
															}
															if	(	v1.dims[i].dimensional	==	SINGLE	)	{
																							if	(	v1.dims[i].lower	!=	v2.dims[i].lower	)	{
																															VAR_DELETE	;
																															return	(	-1	)	;	/*	v2	not	in	v1's	space	*/
																							}
															}
						}

						/*	Next	make	sure	that	v2	is	a	single	dimensional	var	*/
						for	(	i	=	0	;	i	<	v2.numParams	;	i++	)	{
															if	(	v2.dims[i].dimensional	==	MULTI	)	{
																							VAR_DELETE	;
																							return(-1);
															}
						}

						/*	Calculate	offset	of	v2	into	v1's	space	*/

						/*	First,	get	all	offsets	into	each	dimension	&	size	of	each	dimension	*/
						for	(	i	=	0	;	i	<	v1.numParams	;	i++	)	{
															offsets[i]	=	v2.dims[i].upper	-	v1.dims[i].lower	;
															sizedims[i]	=	v1.dims[i].upper	-	v1.dims[i].lower	+	1	;
						}

						/*
							*	Now	go	through	magical	algorithm	to	get	offset
							*	Think	of	it	like	an	odometer	where	each	cylinder	has	a	different
							*	number	of	numbers,	and	the	numbers	on	each	cylinder	can
							*	range	from	any	start	to	stop	point
							*
							*	say	v1	=	x0[a0-b0].x1[a1-b1]...xN[aN-bN]
							*	say	v2	=	x0[y0].x1[y1]...xN[yN]
							*
							*	let	si	=	ai	-	bi	+	1		(i	=	0..N)
							*	let	oi	=	yi	-	ai						(i	=	0..N)
							*
							*	offset=	sN*sN-1*...s1(o0)	+	sN*sN-1*...s2(o1)	+	...	+	sN(oN-1)	+	oN	+	1
							*
							*	Then	offset	=	offset	-	1	for	indexing
							*/
							for	(	i	=	0	;	i	<	v1.numParams	;	i++	)	{
															sizeproducts[i]	=	1	;
															for	(	j	=	i+1	;	j	<	v1.numParams	;	j++	)	{
																							sizeproducts[i]	=	sizeproducts[i]*sizedims[j]	;
															}
							}
							offset	=	0	;
							for	(	i	=	0	;	i	<	v1.numParams	;	i++	)	{
															offset	=	offset	+	sizeproducts[i]*offsets[i]	;
															if	(	offset	<	0	)	{
																							fprintf(stderr,	"ERROR:	Integer	overflow	"
																																							"calculating	offset!\n");
																							VAR_DELETE;
																							return(-1);
															}
							}

							VAR_DELETE	;

							return	(	offset	)	;
}

/**
	*	Get	byte	size	of	variable	e.g.
	*
	*		a[0-5]:	6*(4bytes/double)	=	24	bytes	in	all
	*
	*/
int	Var::getSize(		)	{
							return(	byteSize_	)	;
}

/**	Set	variable	name	string	*/
string	Var::setVarName(	const	char*	name	)	{

							//	Reallocate	varName_
							varName_	=	name	;

							//	Set	number	of	dimensions	for	variable
							nDimensions_	=	calcNumDimensions();

							//	Set	byte	size
							if	(	varType_	!=	NONE	)	{
										byteSize_	=	calcByteSize();
							}

							return(	varName_	)	;
}

/**	Get	variable	type	(double,	float,	int	...)	*/
Var::enumType	Var::getType	(	)	{
							return(	varType_	)	;
}

/**	Get	variable's	type	size	(double->8,	float->4	...)*/
int	Var::getTypeSize	()	{

							//	I	am	avoiding	another	switch	statement	on	type	here

							int	totalSize	;
							int	numDims	;

							//	Get	total	byte	size	for	variable	and	number	of	dimensions
							totalSize	=	calcByteSize()	;
							numDims			=	getNumDims()	;

							//	So	type	size	will	be	total	size	divided	by	number	of	dims
							return	(	totalSize/numDims	)	;
}

/**	Set	variable	type	(double,	float,	int...)	*/
int	Var::setType	(	char*	type	)	{

							if									(	!strcasecmp(type,	"char")	)	{
																varType_	=	CHAR	;
							}	else	if		(	!strcasecmp(type,	"short")	)	{
																varType_	=	SHORT	;
							}	else	if		(	!strcasecmp(type,	"int")	)	{
																varType_	=	INT	;
							}	else	if		(	!strcasecmp(type,	"long")	)	{
																varType_	=	LONG	;
							}	else	if		(	!strcasecmp(type,	"unsigned_char")	)	{
																varType_	=	UNSIGNED_CHAR	;
							}	else	if		(	!strcasecmp(type,	"unsigned_short")	)	{
																varType_	=	UNSIGNED_SHORT	;
							}	else	if		(	!strcasecmp(type,	"unsigned_int")	)	{
																varType_	=	UNSIGNED_INTEGER	;
							}	else	if		(	!strcasecmp(type,	"unsigned_long")	)	{
																varType_	=	UNSIGNED_LONG	;
							}	else	if		(	!strcasecmp(type,	"float")	)	{
																varType_	=	FLOAT	;
							}	else	if		(	!strcasecmp(type,	"double")	)	{
																varType_	=	DOUBLE	;
							}	else	if		(	!strcasecmp(type,	"long_long")	)	{
																varType_	=	LONG_LONG	;
							}	else	if		(	!strcasecmp(type,	"unsigned_long_long")	)	{
																varType_	=	UNSIGNED_LONG_LONG	;
							}	else	if		(	!strcasecmp(type,	"void")	)	{
																varType_	=	VOID	;
							}	else	{
																varType_	=	NONE	;
																return(	-1	)	;
							}

							return	(	0	)	;
}

/**	Calculates	and	returns	-total-	bytesize	of	variable	*/
int	Var::calcByteSize()	{

							int	size;
							int	totalSize;

							//	Make	sure	that	num	dimensions	set	(user	should	never	see	this)
							if	(	nDimensions_	<	1	)	{
															printf("ERROR:	nDimensions	must	be	set	before	"
																						"calculating	size.\n");
															return(	-1	);
							}

							switch	(	varType_	)	{
															case	CHAR:
																								size	=	sizeof(char);
																								break;
															case	SHORT:
																								size	=	sizeof(short);
																								break;
															case	INT:
																								size	=	sizeof(int);
																								break;
															case	LONG:
																								size	=	sizeof(long);
																								break;
															case	UNSIGNED_CHAR:
																								size	=	sizeof(unsigned	char);
																								break;
															case	UNSIGNED_SHORT:
																								size	=	sizeof(unsigned	short);
																								break;
															case	UNSIGNED_INTEGER:
																								size	=	sizeof(unsigned	int);
																								break	;
															case	UNSIGNED_LONG:
																								size	=	sizeof(unsigned	long);
																								break	;
															case	FLOAT:
																								size	=	sizeof(float);
																								break;
															case	DOUBLE:
																								size	=	sizeof(double);
																								break;
															case	LONG_LONG:
																								size	=	sizeof(long	long);
																								break	;
															case	UNSIGNED_LONG_LONG:
																								size	=	sizeof(unsigned	long	long);
																								break	;
															case	VOID:
																								size	=	0;
																								break;
															case	NONE:
																								printf("ERROR:	Type	must	be	set	before	"
																															"calculating	size.\n");
																								return(	-1	);
																								break;
															default:
																								printf("ERROR:	Type	must	be	set	before	"
																															"calculating	size.\n");
																								return(	-1	);
							}

							totalSize	=	size*nDimensions_	;

							return(	totalSize	);
}

/*	Calculate	and	return	total	number	of	dimensions	of	variable	e.g.
	*
	*	Input	a[0-4].b[8-9],
	*	Returns:	5*2	=	10	dimensions
	*/
int	Var::calcNumDimensions()	{

							int	i,j;
							int	stringLen;
							char*	dim1;
							char*	dim2;
							int		d1,	d2;
							int		numDims;
							int		numDimSpecs;

							//	Dims	can't	be	longer	than	varName
							stringLen	=	varName_.size();
							dim1	=	new	char[stringLen+1]	;
							dim2	=	new	char[stringLen+1]	;

							numDimSpecs	=	0	;
							numDims	=	1	;
							for	(	i	=	0;	i	<	stringLen;	i++	)	{

																if	(	varName_[i]	==	']'	)	{
																															cerr	<<	"ERROR:	Missing	open	bracket	in"
																																				<<	varName_	<<	endl	;
																															delete[]	dim1	;
																															delete[]	dim2	;
																															return(	-1	);
																}

																if	(	varName_[i]	==	'['	)	{

																							while	(	varName_[i]	!=	']'	)	{

																														i++	;

																														//	Missing	close	bracket?
																														if	(	i	==	stringLen	)	{
																																							cerr	<<	"ERROR:	Missing	close	bracket	in"
																																												<<	varName_	<<	endl	;
																																							delete[]	dim1	;
																																							delete[]	dim2	;
																																							return(	-1	)	;
																														}

																														//	Get	dimension	1
																														j	=	0;
																														dim1[0]	=	'\0';
																														while	(	isdigit(varName_[i])	)	{

																																						dim1[j]	=	varName_[i]	;

																																							i++;	j++;

																																							if	(	i	==	stringLen		)	{
																																														cerr	<<	"ERROR:	Missing	close	"
																																																			<<	"bracket	in	"
																																																			<<	varName_	<<	endl	;
																																														delete[]	dim1	;
																																														delete[]	dim2	;
																																														return(	-1	)	;
																																							}
																																							if	(	j	>	7	)	{
																																														printf("ERROR:	Dimension	spec	"
																																																					"too	long.\n");
																																														delete[]	dim1	;
																																														delete[]	dim2	;
																																														return(	-1	)	;
																																							}
																														}
																														dim1[j]	=	'\0';

																														//	Get	dimension	2
																														j	=	0	;
																														if	(	varName_[i]	==	'-'	)	{

																																							i++	;

																																							//	Multi-dimensional
																																							while	(	isdigit(varName_[i])	)	{

																																															dim2[j]	=	varName_[i]	;

																																															i++;	j++;

																																															if	(	i	==	stringLen		)	{
																																																							cerr	<<	"ERROR:	Missing	"
																																																												<<	"close	bracket"
																																																												<<	"	in	"
																																																												<<	varName_
																																																												<<	endl	;
																																																							delete[]	dim1	;
																																																							delete[]	dim2	;
																																																							return(	-1	)	;
																																															}
																																															if	(	j	>	7	)	{
																																																							cerr	<<	"ERROR:	Dim	"
																																																												<<	"spec	too	long	"
																																																												<<	"in	"
																																																												<<	varName_
																																																												<<	endl	;
																																																							delete[]	dim1	;
																																																							delete[]	dim2	;
																																																							return(	-1	)	;
																																															}

																																							}
																																							dim2[j]	=	'\0';

																															}	else	if	(	varName_[i]	==	']'	)	{

																																							//	Single	dimensional
																																							strcpy(dim2,	dim1);

																															}	else	{
																																							cerr	<<	"ERROR:	Dimension	has	syntax	"
																																												<<	"error	with	"
																																												<<	varName_	<<	endl	;
																																							delete[]	dim1	;
																																							delete[]	dim2	;
																																							return(	-1	)	;
																															}
																							}

																							if	(	dim1[0]	==	'\0'	||	dim2[0]	==	'\0'	)	{
																															cerr	<<	"ERROR:	Bad	syntax	in	dimension	"
																																				<<	"specification	for	"
																																				<<	varName_	<<	endl	;
																															delete[]	dim1	;
																															delete[]	dim2	;
																															return(	-1	);
																							}

																							//	Calculate	number	of	dimensions
																							d1	=	atoi(dim1);
																							d2	=	atoi(dim2);
																							numDims	=	numDims	*	(	d2	-	d1	+	1	)	;

																							//	Increment	number	of	dimension	specs
																							numDimSpecs++	;
															}
							}

							//	Free	dims
							delete[]	dim1	;
							delete[]	dim2	;

							return(	numDims	);
}

/**	Return	number	of	parameters	in	variable	e.g.
		*	a[0-9].b[2-3].c	has	3	parms	"a[0-9],	b[2-3]	&	c"
		*
		*	For	multi-dimensional	vars	it's	weird
		*	a[0-9][0-2].b[2-3].c	has	4	parms	"a[0-9],	a[0-2],	b[2-3]	&	c"
		*/
int	Var::getNumParms()	{

							unsigned	int	i	;
							int	nParms	;

							if	(	varName_[0]	==	'\0'	)	{
															return(0);
							}

							nParms	=	1	;
							for	(	i	=	0	;	i	<	varName_.size()	;	i++	)	{
															if	(	varName_[i]	==	'.'	)	{
																							nParms++	;
															}
															if	(	varName_[i]	==	']'	&&	varName_[i+1]	==	'['		)	{
																							nParms++	;
															}
							}

							return(nParms);
}

/**	Set	variable's	minimum	range	*/
void	Var::setMinRange(double	minRange)	{
							minRange_	=	minRange	;
}

/**	Set	variable's	maximum	range	*/
void	Var::setMaxRange(double	maxRange)	{
							maxRange_	=	maxRange	;
}

/**	Set	variable's	scale	factor	*/
void	Var::setScaleFactor(double	scaleFactor)	{
							scaleFactor_	=	scaleFactor	;
}

/**	Set	variable's	bias	@see	getBias()	*/
void	Var::setBias(double	bias)	{
							bias_	=	bias	;
}

/**	Assignment	*/
const	Var&	Var::operator=	(	Var&	right	)
{
								if	(	this	==	&right	)	{
																fprintf(stderr,	"Attempted	self	assignment	in	Xvar	"
																																"operator=.	\n");
																return	*this;
								}

								this->setVarName(	right.getVarName()	)	;
								this->setTimeName(	right.getTimeName()	)	;
								this->setMinRange(	right.getMinRange()	)	;
								this->setMaxRange(	right.getMaxRange()	)	;
								this->setScaleFactor(	right.getScaleFactor()	)	;
								this->setBias(	right.getBias()	)	;
								this->setLabel(	right.getLabel()	)	;
								this->setUnit(	right.getUnit())	;
								this->setTimeUnit(	right.getTimeUnit())	;
								this->isProgramOutput	=	right.isProgramOutput	;

								return	*this	;		//	Allows	concatenated	assignments
}

/**	Get	the	label	of	the	var	*/
const	char*	Var::getLabel(		)	{
							return(	label_.c_str()	)	;
}

/**	Get	the	label	of	the	var	*/
string	Var::setLabel(const	char	*	label	)	{
							label_	=	label	;
							return(	label_	)	;
}
