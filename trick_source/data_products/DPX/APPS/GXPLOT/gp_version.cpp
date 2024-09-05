
#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>

using	namespace	std;

double	GnuplotVersion()	{
								FILE*	pipe	;
								char	buf[256]	;
								double	version	=	-1.0	;
								int	ret	;

								buf[0]	=	'\0'	;

								if	(	(pipe	=	popen("gnuplot	--version	2>	/dev/null",	"r"))	!=	NULL	)	{
																fgets(	buf,	255,	pipe	)	;

																//	In	gnuplot	3.x,	the	gnuplot	--version	command
																//	causes	an	error,	so	return	3.0	if	buf[0]	!=	'g'
																if	(	buf[0]	!=	'g'	)	{
																								pclose(pipe);
																								return(3.0);
																}
																//	Okay	in	4.0
																//	%>	gnuplot	--version
																//	%>	gnuplot	4.0	patchlevel	0
																//	For	now	assume	it	will	stay	like	this
																ret	=	sscanf(buf,	"%*s	%lf",	&version);
																if	(	ret	==	1	)	{
																								pclose(pipe);
																								return(version);
																}	else	{
																								cerr	<<	"ERROR:	getGnuplotVersion()	:	Died	while	"
																													<<	"\"	scanning	gnuplot	--version\".		Aborting."
																													<<	endl	;
																								exit(-1);
																}
								}	else	{
																cerr	<<	"ERROR:	getGnuplotVersion()	:	Died	while	running	"
																					<<	"\"gnuplot	--version\".		Aborting."
																					<<	endl	;
																pclose(pipe);
																exit(-1);
								}
								//	This	just	makes	the	compiler	happy,
								//	it	will	not	be	executed
								return(	version	)	;
}

