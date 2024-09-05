#include	<QTableWidgetItem>
#include	"VarTableWidget.h"

VarTableWidget::VarTableWidget	(QWidget*	parent)
		:	QTableWidget(parent)	{

		setRowCount(0);
		setColumnCount(4);

		QStringList	strList;
		strList	<<	""	<<	"Name"	<<	"Type"	<<	"Units";
		setHorizontalHeaderLabels(	strList);

		setColumnWidth(0,25);
		setColumnWidth(1,200);
		setColumnWidth(2,80);
		setColumnWidth(3,80);

		setShowGrid(false);
}

void	VarTableWidget::addRecord(	Qt::CheckState	checkState,	
																																std::string	nameStr,
																																std::string	typeStr,
																																std::string	unitsStr	)	{

//	Consider	cloning	prototype	items,	as	in	this	example:
//	https://stackoverflow.com/questions/15827886/set-default-alignment-for-cells-in-qtablewidget

				int	row	=	rowCount();
				insertRow(row);

				Qt::ItemFlags	flags;

				QTableWidgetItem	*	const	selectItem	=	new	QTableWidgetItem;	
				flags	=	selectItem->flags();
				flags	&=	~Qt::ItemIsSelectable	&	~Qt::ItemIsEditable;
				if	(row	!=	0)					//	FIXME:	This	doesn't	seem	to	work	as	expected.	Row	0	still	seems	to	be	user	editable.
								flags	|=	Qt::ItemIsUserCheckable;
				selectItem->setFlags(flags);
				selectItem->setCheckState(checkState);
				setItem(row,	0,	selectItem);

				QTableWidgetItem	*	const	nameItem			=	new	QTableWidgetItem;	
				flags	=	nameItem->flags();
				flags	&=	~Qt::ItemIsSelectable	&	~Qt::ItemIsEditable;
				nameItem->setFlags(flags);
				nameItem->setText(nameStr.c_str());
				setItem(row,	1,	nameItem);
	
				QTableWidgetItem	*	const	typeItem			=	new	QTableWidgetItem;	
				flags	=	typeItem->flags();
				flags	&=	~Qt::ItemIsSelectable	&	~Qt::ItemIsEditable;
				typeItem->setFlags(flags);
				typeItem->setTextAlignment(Qt::AlignCenter);
				typeItem->setText(typeStr.c_str());
				setItem(row,	2,	typeItem);

				QTableWidgetItem	*	const	unitsItem		=	new	QTableWidgetItem;	
				flags	=	unitsItem->flags();
				flags	&=	~Qt::ItemIsSelectable	&	~Qt::ItemIsEditable;
				unitsItem->setFlags(flags);
				unitsItem->setTextAlignment(Qt::AlignCenter);
				unitsItem->setText(unitsStr.c_str());
				setItem(row,	3,	unitsItem);

}

void	VarTableWidget::highLightRecord(int	index)	{
				QBrush	brush(Qt::yellow);
				item(index,	1)->setBackground(brush);	
}

void	VarTableWidget::unHighLightRecord(int	index)	{
				QBrush	brush(Qt::white);
				item(index,	1)->setBackground(brush);	
}

void	VarTableWidget::scrollToRecord(int	index)	{
				scrollToItem(item(index,	0));
}

bool	VarTableWidget::isChecked(int	index)	{
				return	(item(index,	0)->checkState()	==	Qt::Checked)	?	true	:	false;
}

int	VarTableWidget::recordCount()	{
				return	rowCount();
}

void	VarTableWidget::checkAll()	{
				int	num_rows	=	rowCount();
				for	(int	i=0;	i<num_rows;	i++)	{
								item(i,0)->setCheckState(Qt::Checked);
				}
}

void	VarTableWidget::unCheckAll()	{
				int	num_rows	=	rowCount();
				//	Note	that	we	don't	clear	sys.exec.out.time
				for	(int	i=1;	i<num_rows;	i++)	{
								item(i,0)->setCheckState(Qt::Unchecked);
				}
}
