#include	<QApplication>
#include	<QListView>
#include	<QMenu>
#include	<QMenuBar>
#include	<QFileDialog>
#include	"DocWindow.h"
#include	<QTextStream>
#include	<QFileInfo>

#include	<stdio.h>
static	const	char	*usage_doc[]	=	{
"----------------------------------------------------------------------------",
"	trkConvert	-																																																															",
"																																																																												",
"	USAGE:		trkConvert	-help																																																			",
"									trkConvert	<trk_file_name>																																									",
"	Options:																																																																			",
"					-help				Print	this	message	and	exit.																																		",
"----------------------------------------------------------------------------"};
#define	N_USAGE_LINES	(sizeof(usage_doc)/sizeof(usage_doc[0]))

void	print_doc(char	*doc[],	int	nlines)	{
				int	i;
				for	(i=0;	i	<	nlines;	i++)	{
								printf("%s\n",doc[i]);
				}
				fflush(stdout);
}

void	usage()	{
				print_doc((char	**)usage_doc,N_USAGE_LINES);
}

int	main(int	argc,	char	*argv[])	{

				QTextStream	out(stdout);

				QApplication	app(argc,	argv);

				QStringList	args	=	app.arguments();	

				QString	programName	=	args.at(0);
				QString	trkFilePath;
				QString	outputName;

				int	i	=	1;
				QString	arg;
				while	(i	<	argc)	{
								arg	=	args.at(i);
								if	(	(QString::compare(arg,	"-help")	==	0)	|
													(QString::compare(arg,	"--help")	==	0)	)	{
												usage();
												exit(0);
								}	else	if	((QString::compare(arg.right(4),	".trk")	==	0))	{
												trkFilePath	=	arg;
								}	else	{
												out	<<	programName	<<	":	Invalid	argument	\""	<<	arg	<<	"\"."	<<	endl;
												usage();	
												exit(1);
								}
								i++;
				}

				if	(trkFilePath.isEmpty())	{
								trkFilePath	=	QFileDialog::getOpenFileName(0,	"Open	Data	File",	".",	"Data	Files	(*.trk)");
				}

				if	(!trkFilePath.isEmpty())	{
								QFileInfo	trkFileInfo(	trkFilePath);

								TRK_DataLog*	datalog	=
												new	TRK_DataLog(	trkFileInfo.absoluteFilePath().toStdString().c_str()	);

								DocWindow*	w1	=	new	DocWindow(datalog);	
								w1->setWindowTitle(	trkFileInfo.fileName());
								w1->resize(800,	500);
								w1->show();

								return	app.exec();
				}
}
