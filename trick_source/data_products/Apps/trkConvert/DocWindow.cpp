#include	<QAction>
#include	<QFileDialog>
#include	<QMenuBar>
#include	<QPushButton>
#include	<QVBoxLayout>
#include	<QLineEdit>
#include	<QWidget>
#include	<iostream>

#include	<QTextStream>

#include	"DocWindow.h"
#include	"VarTableWidget.h"
#include	"CSV_Formatter.hh"
#include	"Varlist_Formatter.hh"

DocWindow::DocWindow(TRK_DataLog*	data_log	)
				:	QMainWindow(	0,	0)	{

				QTextStream	out(stdout);

				foundItemIndex	=	0;
				datalog	=	data_log;

				//	Build	the	FILE	menu

				QMenu	*fileMenu	=	menuBar()->addMenu("&File");

				QAction	*	fileLoadAction	=	new	QAction(	"&Open	File...",	this	);
				fileLoadAction->setShortcut(tr("CTRL+O"));
				fileMenu->addAction(fileLoadAction);
				connect(	fileLoadAction,	&QAction::triggered	,	this,	&DocWindow::load	);

				fileMenu->addSeparator();

				QAction	*	csvSaveAction	=	new	QAction(	"&Export	as	CSV...",	this	);
				fileMenu->addAction(csvSaveAction);
				connect(	csvSaveAction,	&QAction::triggered,	this,	&DocWindow::saveAsCSV	);

				QAction	*	varListSaveAction	=	new	QAction(	"&Export	as	Variable	List...",	this	);
				fileMenu->addAction(varListSaveAction);
				connect(	varListSaveAction,	&QAction::triggered,	this,	&DocWindow::saveAsVarList	);

				//	Build	the	EDIT	menu

				QMenu	*editMenu	=	menuBar()->addMenu("&Edit");

				QAction	*	editSelectAction	=	new	QAction(	"&Select	All",	this	);
				editSelectAction->setShortcut(tr("CTRL+A"));
				editMenu->addAction(editSelectAction);
				connect(	editSelectAction,	&QAction::triggered,	this,	&DocWindow::checkAll	);

				QAction	*	editClearAction	=	new	QAction(	"&Clear	All",	this	);
				editMenu->addAction(editClearAction);
				connect(	editClearAction,	&QAction::triggered,	this,	&DocWindow::unCheckAll	);

				//	Build	the	search	interface.

				QHBoxLayout	*hbox	=	new	QHBoxLayout();

				QPushButton	*backward	=	new	QPushButton(QChar(0x25C0),	this);
				hbox->addWidget(backward);
				connect(	backward,	&QPushButton::released,	this,	&DocWindow::findAgainBackward);

				QPushButton	*forward		=	new	QPushButton(QChar(0x25B6),	this);
				hbox->addWidget(forward);
				connect(	forward,	&QPushButton::released,	this,	&DocWindow::findAgainForward);

				searchLineEdit	=	new	QLineEdit;
				searchLineEdit->setPlaceholderText("Search	Pattern");
				hbox->addWidget(searchLineEdit);
				connect(searchLineEdit,	SIGNAL(returnPressed()),	this,	SLOT(find()));

				QVBoxLayout	*vbox	=	new	QVBoxLayout();

				//	Build	the	Table	Widget	that	displays	the	variable	names,	types,	and	units.
				varTable	=	new	VarTableWidget(this);

				int	recordCount	=	datalog->parameterCount();
				for	(int	ii	=	0;	ii	<	recordCount;	ii++)	{
								varTable->addRecord(	Qt::Checked,	
																													datalog->parameterName(ii),
																													datalog->parameterType(ii),
																													datalog->parameterUnits(ii));
				}

				vbox->addWidget(varTable);
				vbox->addLayout(hbox);

				QWidget	*window	=	new	QWidget;
				window->setLayout(vbox);

				setCentralWidget(window);
}

void	DocWindow::load()	{

				QString	newFileName;	

				newFileName	=	QFileDialog::getOpenFileName(this,
				tr("Open	Data	File"),	".",	tr("Data	Files	(*.trk)"));

				QFileInfo	trkFileInfo(	newFileName);
				TRK_DataLog*	newdatalog	=	new	TRK_DataLog(	trkFileInfo.absoluteFilePath().toStdString());

				if	(!newFileName.isEmpty())	{
								DocWindow*	w	=	new	DocWindow(newdatalog);
								w->setWindowTitle(newFileName);
								w->resize(800,	500);
								w->show();
				}
}

void	DocWindow::formattedSave(LogFormatter	&formatter)	{
					
				QFileInfo	trkFileInfo(	datalog->getFileName().c_str());

				QString	outFileName	=	trkFileInfo.canonicalPath();
				outFileName	+=	"/";
				outFileName	+=	trkFileInfo.completeBaseName();
				outFileName	+=	formatter.extension();

				QTextStream	out(stdout);
				out	<<	"outFileName	=	\""	<<	outFileName	<<	"\""	<<	endl;

				int	count	=	varTable->recordCount();
				for	(int	index=0	;	index<count	;	index++)	{
								if	(varTable->isChecked(index))	{
												datalog->selectParameter(index);
								}	else	{
												datalog->deselectParameter(index);
								}									
				}

				FILE	*fp;
				if	((	fp	=	fopen(outFileName.toStdString().c_str(),	"w")	)	!=	NULL)	{
								datalog->formattedWrite(fp,	&formatter);
				}
}

void	DocWindow::saveAsCSV()	{
				CSV_Formatter	csv_formatter;
				formattedSave(csv_formatter);
}

void	DocWindow::saveAsVarList()	{
				Varlist_Formatter	varlist_formatter;
				formattedSave(varlist_formatter);
}

void	DocWindow::checkAll()	{
				varTable->checkAll();
}

void	DocWindow::unCheckAll()	{
				varTable->unCheckAll();
}

void	DocWindow::findAgain(int	direction)	{
				if	(foundItemIndex	>=	0)	{
								varTable->unHighLightRecord(foundItemIndex);						
								int	count	=	varTable->recordCount();
								int	startIndex	=	(count	+	foundItemIndex	+	direction)	%	count;
								if	((foundItemIndex	=	textSearch(searchPattern,	startIndex,	direction))	>=	0)	{
												varTable->scrollToRecord(foundItemIndex);						
												varTable->highLightRecord(foundItemIndex);						
								}
				}
}

void	DocWindow::findAgainForward()	{
				findAgain(1);
}

void	DocWindow::findAgainBackward()	{
				findAgain(-1);
}

void	DocWindow::find()	{
				searchPattern	=	searchLineEdit->text();

				if	(foundItemIndex	>=	0)	{
								varTable->unHighLightRecord(foundItemIndex);						
				}
				if	((foundItemIndex	=	textSearch(searchPattern,	0,	1))	>=	0)	{
								varTable->scrollToRecord(foundItemIndex);						
								varTable->highLightRecord(foundItemIndex);						
				}
}

int	DocWindow::textSearch(QString	pattern,	int	startIndex,	int	direction)	{

				QRegExp	rx(pattern);
				int	count	=	varTable->recordCount();
				int	foundIx	=	-1;
				for	(int	i=0	;	i<count	;	i++)	{
								int	index	=	(count	+	startIndex	+	direction	*	i)	%	count;
								if	((rx.indexIn(datalog->parameterName(index),0))	>=	0)	{
												foundIx	=	index;
												break;
								}
				}
				return	foundIx;
}
