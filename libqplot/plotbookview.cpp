#include "plotbookview.h"
#include <QDebug>

PlotBookView::PlotBookView(PlotBookModel *plotModel, const QStringList &titles, QWidget *parent) :
    QAbstractItemView(parent),
    _titles(titles),
    _plotModel(plotModel),
    _monteModel(0),
    _isTabCloseRequested(false),
    _currSelectedRun(-1),
    _isShowCurveDiff(false)
{
    setModel(_plotModel);

    _bookFrame = new QFrame(parent);
    _bookGridLayout = new QGridLayout(_bookFrame);

    _buttonCloseAll = new QPushButton("Close All",_bookFrame);
    connect(_buttonCloseAll,SIGNAL(clicked()),
            this,SLOT(_closeAllPlots()));
    _bookGridLayout->addWidget(_buttonCloseAll,1,0);
    _buttonToggleDiff = new QPushButton("Toggle Diff",_bookFrame);
    connect(_buttonToggleDiff,SIGNAL(clicked()),
            this,SLOT(_toggleDiffPlots()));

     _buttonLayout = new QHBoxLayout;
     _buttonLayout->addWidget(_buttonCloseAll);
     _buttonLayout->addWidget(_buttonToggleDiff);
     _buttonFrame = new QFrame(_bookFrame);
     _buttonFrame->setLayout(_buttonLayout);
    _bookGridLayout->addWidget(_buttonFrame,1,0);
    _buttonToggleDiff->setHidden(true);


    this->setFocusPolicy(Qt::ClickFocus);
    _nb = new QTabWidget(_bookFrame);
    _nb->setTabsClosable(true);
    _nb->setFocusPolicy(Qt::StrongFocus);
    _nb->setMovable(true);
    connect(_nb,SIGNAL(tabCloseRequested(int)),
            this,SLOT(tabCloseRequested(int)));
    connect(_nb,SIGNAL(currentChanged(int)),
            this,SLOT(tabCurrentChanged(int)));
    _bookGridLayout->addWidget(_nb,0,0);
}

// This is private
PlotBookView::PlotBookView(QWidget *parent) :
    QAbstractItemView(parent),
    _plotModel(0),
    _monteModel(0),
    _isTabCloseRequested(false),
    _currSelectedRun(-1),
    _isShowCurveDiff(false)
{
}

PlotBookView::~PlotBookView()
{
    // I think, not sure, the grids, plots frames are deleted with notebook
    // so nothing to do
}

void PlotBookView::setData(MonteModel *monteModel)
{
    _monteModel = monteModel;
}

//
// This is bogus.  Untested.  And don't really know what it is.
// For now just return frame rect
//
QRect PlotBookView::visualRect(const QModelIndex &index) const
{
    QRect rect;
    if ( model() && index.isValid()) {
        QWidget* page = _idx2Page(index);
        if ( page )  {
            rect = page->rect();
        }
    }

    return rect;
}

void PlotBookView::scrollTo(const QModelIndex &index,
                            QAbstractItemView::ScrollHint hint)
{
    // TODO
    Q_UNUSED(index);
    Q_UNUSED(hint);
    update();
}

// For now, return index for a plot at point
// Point is relative to page
QModelIndex PlotBookView::indexAt(const QPoint &point) const
{
    if ( !model() ) return QModelIndex();

    QModelIndex idx;
    QWidget* page = _nb->currentWidget();
    QGridLayout* grid = _page2grid.value(page);

    for ( int i = 0; i < grid->count(); ++i) {
        QWidget* w = grid->itemAt(i)->widget();
        if ( ! w->isVisible() ) {
            continue;
        }
        if ( w->geometry().contains(point) ) {
            Plot* plot = static_cast<Plot*>(w);
            idx = _plot2Idx(plot);
            break;
        }
    }

    return idx;
}

void PlotBookView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    connect(selectionModel,
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(_plotSelectModelCurrentChanged(QModelIndex,QModelIndex)));
    connect(selectionModel,
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,
            SLOT(_plotSelectModelSelectChanged(QItemSelection,QItemSelection)));
    QAbstractItemView::setSelectionModel(selectionModel);
}

QModelIndex PlotBookView::currentPageIndex()
{
    QModelIndex pageIdx;
    QWidget* page = _nb->currentWidget();
    pageIdx = _page2Idx(page);
    return pageIdx;
}

void PlotBookView::setCurrentPage(const QModelIndex& pageIdx)
{
    int pageId = -1;
    QWidget* page = _idx2Page(pageIdx);
    for ( int i = 0 ; i < _nb->count(); ++i ) {
        QWidget* w = _nb->widget(i);
        if ( w == page ) {
            pageId = i;
            break;
        }
    }
    if ( pageId >= 0 ) {
        _nb->setCurrentIndex(pageId);
    }

}

//
// To speed up printing and reduce pdf size,
// instead of printing directly to the pdf,
// print pixmaps of curves to pdf
//
bool PlotBookView::savePdf(const QString &fileName)
{
    // Page size and title header size
    int ww = 1200;
    int hh = 900;

    // Save current widget
    QWidget* originalPage = _nb->currentWidget();

    // Header height
    int heightHeader = 0;
    for ( int i = 0 ; i < _nb->count(); ++i ) {
        QWidget* page = _nb->widget(i);
        _nb->setCurrentWidget(page);  // for layout when offscreen
        PageTitleWidget* pw = _page2pagewidget.value(page);
        if ( pw->sizeHint().height() > heightHeader ) {
            heightHeader = pw->height() ;
        }
    }

    // Setup pdf printer
    QPrinter pdfPrinter(QPrinter::ScreenResolution);
    pdfPrinter.setOutputFileName(fileName);
    pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
    pdfPrinter.setFullPage(true);
    pdfPrinter.setPaperSize(QSizeF(ww,hh),QPrinter::DevicePixel);

    // Setup Painter
    QCPPainter pdfPainter;
    pdfPainter.setMode(QCPPainter::pmVectorized);
    pdfPainter.setMode(QCPPainter::pmNoCaching);
    pdfPainter.setMode(QCPPainter::pmNonCosmetic,false);
    if (! pdfPainter.begin(&pdfPrinter)) {
        return false;
    }
    QRect paintRect = QRect(0,0,pdfPrinter.width(),pdfPrinter.height());
    pdfPainter.setWindow(paintRect);

    // Init pixmap for header
    QRect headerRect(0,0,ww,heightHeader);
    QPixmap pixmapHeader(ww,heightHeader);
    QCPPainter pixPainterHeader;
    pixPainterHeader.setMode(QCPPainter::pmNoCaching);
    pixPainterHeader.setMode(QCPPainter::pmNonCosmetic,false);
    if (! pixPainterHeader.begin(&pixmapHeader)) {
        pdfPainter.end();
        return false;
    }
    pixPainterHeader.setWindow(headerRect);

    // Init pixmap for plots
    QRect plotsRect(0,heightHeader,ww,hh-heightHeader);
    QPixmap pixmapPlots(ww,hh-heightHeader);
    QCPPainter pixPainterPlots;
    pixPainterPlots.setMode(QCPPainter::pmNoCaching);
    pixPainterPlots.setMode(QCPPainter::pmNonCosmetic,false);
    if (! pixPainterPlots.begin(&pixmapPlots)) {
        pdfPainter.end();
        pixPainterHeader.end();
        return false;
    }
    pixPainterPlots.setWindow(plotsRect);

    for ( int pageId = 0 ; pageId < _nb->count(); ++pageId ) {

        QWidget* page = _nb->widget(pageId);
        PageTitleWidget* pw = _page2pagewidget.value(page);

        QVector<Plot*> plots = _page2Plots.value(page);
        QVector<QRect> origPlotViewports;
        foreach (Plot* plot, plots ) {
            origPlotViewports.append(plot->viewport());
            plot->setViewport(plotsRect);
        }

        _layoutPdfPlots(plots);

        // Draw plots (and restore plot layout)
        pixmapPlots.fill(Qt::white);
        int plotId = 0 ;
        foreach (Plot* plot, plots ) {
            plot->drawMe(&pixPainterPlots);
            plot->setViewport(origPlotViewports.at(plotId));
            plot->plotLayout()->simplify();  // get rid of empty cells
            plotId++;
        }
        pdfPainter.drawPixmap(0,heightHeader,pixmapPlots);

        // Draw header (resize for offscreen rendering)
        pixmapHeader.fill(Qt::white);
        pixPainterHeader.end();
        pw->setFixedWidth(ww);
        pw->setFixedHeight(heightHeader);
        pw->render(&pixmapHeader);
        pw->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX));
        pdfPainter.drawPixmap(0,0,pixmapHeader);
        pixPainterHeader.begin(&pixmapHeader);

        // Insert new page in pdf booklet
        if ( pageId < _pages.size()-1 ) {
            pdfPrinter.newPage();
        }
    }

    pdfPainter.end();
    pixPainterHeader.end();
    pixPainterPlots.end();

    _nb->setCurrentWidget(originalPage);

    return true;
}

void PlotBookView::_layoutPdfPlots(const QVector<Plot*>& plots)
{
        // Plotbookview's page is a Qt QGrid of QCustomplot Widgets.
        // QCustomPlot can make a "page", but for interactive use,
        // I found it better to use Qt's QGrid.  When printing it is better
        // to use QCustomPlot's layout.  The following switch statement
        // adds empty cells, later restored, in each plot's internal grid layout.
        switch ( plots.size() )
        {
        case 1:
        {
            break;
        }
        case 2:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(0);
            break;
        }
        case 3:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);

            plots.at(1)->plotLayout()->insertRow(0);
            plots.at(1)->plotLayout()->insertRow(2);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(0);
            break;
        }
        case 4:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertColumn(0);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(0);
            plots.at(3)->plotLayout()->insertColumn(0);
            break;
        }
        case 5:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertColumn(0);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(2);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(0);
            plots.at(3)->plotLayout()->insertRow(2);
            plots.at(3)->plotLayout()->insertColumn(0);

            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(0);
            break;
        }
        case 6:
        {

            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(0);
            plots.at(1)->plotLayout()->insertRow(2);
            plots.at(1)->plotLayout()->insertColumn(1);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(1);
            plots.at(3)->plotLayout()->insertRow(1);
            plots.at(3)->plotLayout()->insertColumn(0);

            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(2);
            plots.at(4)->plotLayout()->insertColumn(0);

            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertColumn(0);
            break;
        }
        case 7:
        {
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertRow(1);
            plots.at(0)->plotLayout()->insertColumn(1);

            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertRow(1);
            plots.at(1)->plotLayout()->insertColumn(0);

            plots.at(2)->plotLayout()->insertRow(0);
            plots.at(2)->plotLayout()->insertRow(2);
            plots.at(2)->plotLayout()->insertRow(2);
            plots.at(2)->plotLayout()->insertColumn(1);

            plots.at(3)->plotLayout()->insertRow(0);
            plots.at(3)->plotLayout()->insertRow(2);
            plots.at(3)->plotLayout()->insertRow(2);
            plots.at(3)->plotLayout()->insertColumn(0);

            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(0);
            plots.at(4)->plotLayout()->insertRow(3);
            plots.at(4)->plotLayout()->insertColumn(1);

            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertRow(0);
            plots.at(5)->plotLayout()->insertRow(3);
            plots.at(5)->plotLayout()->insertColumn(0);

            plots.at(6)->plotLayout()->insertRow(0);
            plots.at(6)->plotLayout()->insertRow(0);
            plots.at(6)->plotLayout()->insertRow(0);
            break;
        }
        default:
        {
        }
        }
}

void PlotBookView::showCurveDiff(bool isShow)
{
    _isShowCurveDiff = isShow;
    _buttonToggleDiff->setHidden(false);

}

QModelIndex PlotBookView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers modifiers)
{
    // TODO
    Q_UNUSED(cursorAction);
    Q_UNUSED(modifiers);
    return QModelIndex();
}

int PlotBookView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int PlotBookView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

// TODO: Untested
bool PlotBookView::isIndexHidden(const QModelIndex &index) const
{
    bool isHidden = false;
    Plot* plot = _idx2Plot(index);
    if ( plot ) {
        isHidden = plot->isVisible();
    }
    return isHidden;
}

// TODO
void PlotBookView::setSelection(const QRect &rect,
                                QItemSelectionModel::SelectionFlags command)
{
    Q_UNUSED(rect);
    Q_UNUSED(command);
    qDebug() << "setSelection!!";
}

//
// This is really bogus right now
//
QRegion PlotBookView::visualRegionForSelection(
                               const QItemSelection &selection) const
{
    QModelIndexList idxs = selection.indexes();
    foreach ( QModelIndex idx, idxs ) {
        QWidget* page = _idx2Page(idx);
        if ( page ) {
            return QRegion(page->rect());
        }
        break; // do first one for now
    }
    return QRegion();
}

//
// This is really bogus right now
//
QItemSelectionModel::SelectionFlags PlotBookView::selectionCommand(
                           const QModelIndex &index, const QEvent *event) const
{
    Q_UNUSED(index);
    Q_UNUSED(event);
    return QItemSelectionModel::Select;
}

void PlotBookView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:_selectNextCurve(); break;
    case Qt::Key_Down: _selectPrevCurve();break;
    default: ; // do nothing
    }
    QAbstractItemView::keyPressEvent(event);
}

//
// This takes makes the plot at idx the only plot on the page
//
void PlotBookView::maximize(const QModelIndex &idx)
{
    if ( _isPlotIdx(idx) ) {

        QGridLayout* grid = _idx2Grid(idx);
        Plot* plot = _idx2Plot(idx);

        for ( int i = 0; i < grid->count(); ++i) {
            QWidget* w = grid->itemAt(i)->widget();
            if ( w != plot ) {
                w->setVisible(false);
            } else {
                w->setVisible(true);
            }
        }
    }
}

void PlotBookView::minimize(const QModelIndex &idx)
{
    if ( _isPlotIdx(idx) ) {
        QGridLayout* grid = _idx2Grid(idx);
        for ( int i = 0; i < grid->count(); ++i) {
            grid->itemAt(i)->widget()->setVisible(true);
        }
    }
}

void PlotBookView::dataChanged(const QModelIndex &topLeft,
                          const QModelIndex &bottomRight)
{

    if ( !topLeft.isValid() ) return;

    // See if session start time changed
    QModelIndex startIdx = _plotModel->sessionStartIdx();
    if ( topLeft == bottomRight && topLeft == startIdx ) {
        foreach ( QModelIndex pageIdx, _plotModel->pageIdxs() ) {
            QWidget* page = _idx2Page(pageIdx);
            foreach ( Plot* plot, _page2Plots.value(page) ) {
                plot->setStartTime(model()->data(topLeft).toDouble());
                plot->axisRect()->zoomToFit();
                plot->replot();
            }
        }
    }

    // See if session stop time changed
    QModelIndex stopIdx  = _plotModel->sessionStopIdx();
    if ( topLeft == bottomRight && topLeft == stopIdx ) {
        foreach ( QModelIndex pageIdx, _plotModel->pageIdxs() ) {
            QWidget* page = _idx2Page(pageIdx);
            foreach ( Plot* plot, _page2Plots.value(page) ) {
                plot->setStopTime(model()->data(topLeft).toDouble());
                plot->axisRect()->zoomToFit();
                plot->replot();
            }
        }
    }

    // See if page start/stop time changed
    if ( topLeft == bottomRight && !topLeft.parent().parent().isValid() ) {
        int row = topLeft.row();
        if ( row == 1 || row == 2 ) {
            QWidget* page = _idx2Page(topLeft);
            foreach ( Plot* plot, _page2Plots.value(page) ) {
                if ( row == 1 ) {
                    plot->setStartTime(model()->data(topLeft).toDouble());
                } else if ( row == 2 ) {
                    plot->setStopTime(model()->data(topLeft).toDouble());
                }
                plot->axisRect()->zoomToFit();
                plot->replot();
            }
        }
    }
    QAbstractItemView::dataChanged(topLeft, bottomRight);
    viewport()->update();
}

void PlotBookView::_plotSelectModelCurrentChanged(const QModelIndex &currIdx,
                                                 const QModelIndex &prevIdx)
{
    Q_UNUSED(prevIdx);
    if ( _isPageIdx(currIdx) ) {
        setCurrentPage(currIdx);
    }
}

void PlotBookView::selectRun(int runId)
{
    if ( !model() || !selectionModel() ) return ;
    if ( model()->rowCount() <= 0 ) return;
    QModelIndex page0Idx = _plotModel->pageIdxs().at(0);
    if ( model()->rowCount(page0Idx) <= 0 ) return ;

    // Build a selection of all curves that this RUN maps to
    QItemSelection runSelection;
    foreach ( QModelIndex pageIdx, _plotModel->pageIdxs() ) {
        foreach ( QModelIndex plotIdx, _plotModel->plotIdxs(pageIdx) ) {
            QModelIndex curvesIdx;
            for ( int i = 0; i < plotIdx.model()->rowCount(plotIdx); ++i) {
                QModelIndex idx = plotIdx.model()->index(i,0,plotIdx);
                if ( plotIdx.model()->data(idx).toString() == "Curves" ) {
                    curvesIdx = idx;
                    break;
                }
            }
            QModelIndex curveIdx = model()->index(runId,0,curvesIdx);
            QItemSelection curveSelection(curveIdx,curveIdx);
            runSelection.merge(curveSelection, QItemSelectionModel::Select);
        }
    }

    // Make the selection
    selectionModel()->select(runSelection,QItemSelectionModel::ClearAndSelect);
}

void PlotBookView::_plotSelectModelSelectChanged(const QItemSelection &curr,
                                    const QItemSelection &prev)
{
    if ( curr.indexes().size() > 0 ) {
         // when tab changes, noop
        if ( _isPageIdx(curr.indexes().at(0)) ) return;
    }

    foreach ( QModelIndex prevIdx, prev.indexes() ) {
        TrickCurve* prevCurve = _idx2Curve(prevIdx);
        if ( prevCurve ) {
            prevCurve->setSelected(false);
            if ( curr.indexes().size() == 0 ) {
                // If curr > 0, replot done in next block
                QModelIndex plotIdx = prevIdx.parent();
                Plot* plot = _idx2Plot(plotIdx);
                if ( plot->isVisible() ) {
                        plot->replot();
                }
            }
        }
    }

    foreach ( QModelIndex currIdx, curr.indexes() ) {
        TrickCurve* currCurve = _idx2Curve(currIdx);
        if ( currCurve ) {
            currCurve->setSelected(true);
            QModelIndex plotIdx = currIdx.parent();
            Plot* plot = _idx2Plot(plotIdx);

            // If plot is on current page (not hidden) replot
            if ( plot->isVisible() ) {
                plot->replot();
            }
        }
    }

}

void PlotBookView::_slotCurveClicked(TrickCurve *curve)
{
    if ( curve == 0 ) {
        // Deselect
        selectionModel()->clear();
    } else {
        QModelIndex curveIdx = _curve2Idx(curve);
        int runId = curveIdx.row();
        selectRun(runId);
    }
}

void PlotBookView::_selectNextCurve()
{
    if ( ! selectionModel() || !model() ) return ;

    QItemSelection currSel = selectionModel()->selection();
    if ( currSel.size() > 0 ) {
        QModelIndex currIdx = currSel.indexes().at(0);
        if ( _isCurveIdx(currIdx) ) {
            QModelIndex plotIdx = currIdx.parent();
            int currRow = currIdx.row();
            int nextRow = currRow+1;
            if ( nextRow >= model()->rowCount(plotIdx) ) {
                nextRow = currRow;
            }
            selectRun(nextRow);
        }
    }
}

void PlotBookView::_selectPrevCurve()
{
    if ( ! selectionModel() || !model() ) return ;

    QItemSelection currSel = selectionModel()->selection();
    if ( currSel.size() > 0 ) {
        QModelIndex currIdx = currSel.indexes().at(0);
        if ( _isCurveIdx(currIdx) ) {
            QModelIndex plotIdx = currIdx.parent();
            int currRow = currIdx.row();
            int prevRow = currRow-1;
            if ( prevRow < 0 ) {
                prevRow = 0;
            }
            selectRun(prevRow);
        }
    }
}

void PlotBookView::tabCloseRequested(int tabId)
{
    if ( model() == 0 ) return;

    _isTabCloseRequested = true;
    QWidget* page = _nb->widget(tabId);
    QModelIndex pageIdx = _page2Idx(page);
    if ( pageIdx.isValid() ) {
        int row = pageIdx.row();
        model()->removeRow(row);
    }
    _isTabCloseRequested = false;
}

void PlotBookView::tabCurrentChanged(int tabId)
{
    if ( !model() ) return;

    QWidget* page = _nb->widget(tabId);
    foreach (Plot* plot, _page2Plots.value(page) ) {
        if ( plot ) {
            plot->replot();
        }
    }
}

void PlotBookView::_closeAllPlots()
{
    int nTabs = _nb->count();
    for ( int i = nTabs ; i >= 0; --i ) {
        tabCloseRequested(i);
    }
}

void PlotBookView::_toggleDiffPlots()
{
    foreach ( QWidget* page, _pages ) {
        QVector<Plot*> plots = _page2Plots.value(page);
        foreach ( Plot* plot, plots ) {
            AxisRect* axisRect = (AxisRect*) plot->axisRect();
            axisRect->toggleCurveDiff();
        }
    }
}

void PlotBookView::doubleClick(QMouseEvent *event)
{
    if ( !model() ) return ;

    QPoint globalPageOrigin(_nb->mapToGlobal(QPoint(0,0)));
    QPoint pagePos = event->globalPos()-globalPageOrigin;
    QModelIndex plotIdx = indexAt(pagePos);
    if ( _isPlotIdx(plotIdx) ) {
        QModelIndex pageIdx = plotIdx.parent();
        bool isExpanded = false;
        foreach ( QModelIndex idx, _plotModel->plotIdxs(pageIdx) ) {
            if ( idx != plotIdx ) {
                Plot* plot = _idx2Plot(idx);
                if ( plot->isVisible() ) {
                    isExpanded = false;
                } else {
                    isExpanded = true;
                }
                break;
            }
        }

        if ( isExpanded ) {
            minimize(plotIdx);
        } else {
            maximize(plotIdx);
        }
    }
}


void PlotBookView::plotKeyPress(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Up:{
        _selectPrevCurve();
        break;
    }
    case Qt::Key_Down: {
        _selectNextCurve();
        break;
    }
    case Qt::Key_Space:{
        _toggleDiffPlots();
        break;
    }
    }
}

void PlotBookView::rowsInserted(const QModelIndex &pidx, int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);
    QModelIndex g2pidx = model()->parent(gpidx);
    QModelIndex g3pidx = model()->parent(g2pidx);
    QModelIndex g4pidx = model()->parent(g3pidx);

    for ( int row = start; row < end+1; ++row) {

        QModelIndex idx = model()->index(row,0,pidx);

        if ( _isPageIdx(idx) ) {
            // Page (below root idx)
            QString dpfile = model()->data(idx).toString();
            QFrame* page = new QFrame;
            QPalette pal(palette());
            pal.setColor(QPalette::Background, Qt::white);
            page->setAutoFillBackground(true);
            page->setPalette(pal);
            _pages.append(page);
            QGridLayout* grid = new QGridLayout(page);
            grid->setContentsMargins(0, 0, 0, 0);
            grid->setSpacing(0);

            PageTitleWidget* pw = new PageTitleWidget(page);
            _page2pagewidget.insert(page,pw);
            grid->addWidget(pw,0,0,1,100);

            _page2grid.insert(page,grid);
            _nb->addTab(page,QFileInfo(dpfile).baseName());
            int nbIdx = _nb->count()-1;
            _nb->setCurrentIndex(nbIdx);
            _nb->setAttribute(Qt::WA_AlwaysShowToolTips, true);
        } else if ( !pidx.isValid()  && (row == 0 || row == 1) ) {
            // session start/stop time
        } else if ( ! gpidx.isValid() && row == 0 ) {
            // Page title (row 0)
            QWidget* page = _idx2Page(pidx);
            PageTitleWidget* pw = _page2pagewidget.value(page);
            QString title = model()->data(idx).toString();
            if ( !_titles.at(0).isEmpty() ) {
                pw->setTitle1(_titles.at(0));
            } else if ( !title.startsWith("QP_")  && title != "Page" ) {
                pw->setTitle1(title);
            } else {
                pw->setTitle1("Snap Plot");
            }
            if ( !_titles.at(1).isEmpty() ) {
                    pw->setTitle2(_titles.at(1));
            }
            if ( !_titles.at(2).isEmpty() ) {
                pw->setTitle3(_titles.at(2));
            }
            if ( !_titles.at(3).isEmpty() ) {
                pw->setTitle4(_titles.at(3));
            }
        } else if ( ! gpidx.isValid() && row == 1 ) {
            // Page start time
            QWidget* page = _idx2Page(pidx);
            double pageStartTime = model()->data(idx).toDouble();
            _page2startTime.insert(page,pageStartTime);
        } else if ( ! gpidx.isValid() && row == 2 ) {
            // Page stop time
            QWidget* page = _idx2Page(pidx);
            double pageStopTime = model()->data(idx).toDouble();
            _page2stopTime.insert(page,pageStopTime);
        } else if ( ! gpidx.isValid() && row >= 3 ) {
            // Plot
            QWidget* page = _idx2Page(pidx);
            QGridLayout* grid = _page2grid.value(page);
            Plot* plot = new Plot(page);
            QModelIndex sIdx = _plotModel->sessionStartIdx();
            double sessionStartTime = _plotModel->data(sIdx).toDouble();
            plot->setStartTime(sessionStartTime);
            sIdx = _plotModel->sessionStopIdx();
            double sessionStopTime = _plotModel->data(sIdx).toDouble();
            plot->setStopTime(sessionStopTime);
            connect(plot,SIGNAL(mouseDoubleClick(QMouseEvent*)),
                    this,SLOT(doubleClick(QMouseEvent*)));
            connect(plot, SIGNAL(keyPress(QKeyEvent*)),
                    this, SLOT(plotKeyPress(QKeyEvent*)));
            connect(plot, SIGNAL(curveClicked(TrickCurve*)),
                    this, SLOT(_slotCurveClicked(TrickCurve*)));
            _page2Plots[page].append(plot);
            int nPlots = _page2Plots[page].size();
            switch ( nPlots ) {
            case 1: {
                grid->addWidget(plot,1,0);
                break;
            }
            case 2: {
                grid->addWidget(plot,2,0);
                break;
            }
            case 3: {
                grid->addWidget(plot,3,0);
                break;
            }
            case 4: {
                QWidget* w2 = grid->itemAtPosition(2,0)->widget();
                QWidget* w3 = grid->itemAtPosition(3,0)->widget();
                grid->removeWidget(w2);
                grid->removeWidget(w3);
                grid->addWidget(w2,1,1);
                grid->addWidget(w3,2,0);
                grid->addWidget(plot,2,1);
                break;
            }
            case 5: {
                grid->addWidget(plot,3,0,1,2);
                break;
            }
            case 6: {
                QWidget* w2 = grid->itemAtPosition(1,1)->widget();
                QWidget* w3 = grid->itemAtPosition(2,0)->widget();
                QWidget* w4 = grid->itemAtPosition(2,1)->widget();
                QWidget* w5 = grid->itemAtPosition(3,0)->widget();
                grid->removeWidget(w2);
                grid->removeWidget(w3);
                grid->removeWidget(w4);
                grid->removeWidget(w5);
                grid->addWidget(w2,2,0);
                grid->addWidget(w3,3,0);
                grid->addWidget(w4,1,1);
                grid->addWidget(w5,2,1);
                grid->addWidget(plot,3,1);
                break;
            }
            case 7: {
                grid->addWidget(plot,4,0,1,2);
                break;
            }
            default: {
                qDebug() << "snap limitation: 7 plots max on DP :(";
                qDebug() << "snap will probably crash now!";
            }
            }

        } else if ( ! g2pidx.isValid() ) {

            if ( idx.row() == 0 ) {
                // X axis label
                QString xAxisLabel = model()->data(idx).toString();
                Plot* plot = _idx2Plot(pidx);
                plot->setXAxisLabel(xAxisLabel);
            } else if ( idx.row() == 1 ) {
                // Y axis label
                QString yAxisLabel = model()->data(idx).toString();
                Plot* plot = _idx2Plot(pidx);
                plot->setYAxisLabel(yAxisLabel);
            } else if ( idx.row() == 2 ) {
                //Curves (do nothing)
            } else if ( idx.row() == 3 ) {
                // Plot Title
                QString title = model()->data(idx).toString();
                Plot* plot = _idx2Plot(pidx);
                plot->setTitle(title);
            } else if ( idx.row() == 4 ) {
                // Plot xMin
                double xMin = model()->data(idx).toDouble();
                Plot* plot = _idx2Plot(pidx);
                plot->setXMinRange(xMin);
            } else if ( idx.row() == 5 ) {
                // Plot xMax
                double xMax = model()->data(idx).toDouble();
                Plot* plot = _idx2Plot(pidx);
                plot->setXMaxRange(xMax);
            } else if ( idx.row() == 6 ) {
                // Plot yMin
                double yMin = model()->data(idx).toDouble();
                Plot* plot = _idx2Plot(pidx);
                plot->setYMinRange(yMin);
            } else if ( idx.row() == 7 ) {
                // Plot yMax
                double yMax = model()->data(idx).toDouble();
                Plot* plot = _idx2Plot(pidx);
                plot->setYMaxRange(yMax);
            } else if ( idx.row() == 8 ) {
                // Plot start time
                QWidget* page = _idx2Page(pidx);
                Plot* plot = _idx2Plot(pidx);
                QModelIndex sIdx = _plotModel->sessionStartIdx();
                double sessionStartTime = _plotModel->data(sIdx).toDouble();
                double plotStartTime = model()->data(idx).toDouble();
                double pageStartTime = _page2startTime.value(page);
                if ( sessionStartTime > -1.0e30 ) {
                    plot->setStartTime(sessionStartTime);
                } else if ( plotStartTime > -1.0e30 ) {
                    plot->setStartTime(plotStartTime);
                } else if ( pageStartTime > -1.0e30 ) {
                    plot->setStartTime(pageStartTime);
                }
            } else if ( idx.row() == 9 ) {
                // Plot stop time
                QWidget* page = _idx2Page(pidx);
                Plot* plot = _idx2Plot(pidx);
                QModelIndex sIdx = _plotModel->sessionStopIdx();
                double sessionStopTime = _plotModel->data(sIdx).toDouble();
                double plotStopTime = model()->data(idx).toDouble();
                double pageStopTime = _page2stopTime.value(page);
                if ( sessionStopTime < 1.0e30 ) {
                    plot->setStopTime(sessionStopTime);
                } else if ( plotStopTime < 1.0e30 ) {
                    plot->setStopTime(plotStopTime);
                } else if ( pageStopTime < 1.0e30 ) {
                    plot->setStopTime(pageStopTime);
                }
            } else {
                qDebug() << "snap [bad scoobies]: this should not happen.";
                qDebug() << "     montewindow.cpp creates a model ";
                qDebug() << "     in MonteWindow::_createDPPage().";
                qDebug() << "     which doesn't match this.";
                qDebug() << "     Sorry if the msg doesn't help!!!";
                exit(-1);
            }
        } else if ( ! g3pidx.isValid() ) {
            // Curve (do nothing)
        } else if ( ! g4pidx.isValid() ) {
            // t,x,y,tunit,xunit,yunit,run
            if ( idx.row() == 6 ) {   // 6 is the idx row the run id

                // Run
                int runID = model()->data(idx).toInt();
                QModelIndex xidx = model()->index(1,0,pidx);
                QString xparam = model()->data(xidx).toString();
                QModelIndex yidx = model()->index(2,0,pidx);
                QString yparam = model()->data(yidx).toString();

                TrickCurveModel* curveModel = _monteModel->curve(runID,
                                                                 xparam,
                                                                 yparam);

                //
                // xunit and calc x scale if DP unit not equal to model unit
                //
                bool isXScale = false;
                double xScaleFactor = 1.0;
                QString xunit = curveModel->headerData
                                       (1,Qt::Horizontal,Param::Unit).toString();
                QModelIndex xDPUnitIdx = model()->index(4,0,pidx);
                QString xDPUnit = model()->data(xDPUnitIdx).toString();
                if ( !xDPUnit.isEmpty() && xunit != xDPUnit &&
                     xDPUnit != "--" && xunit != "--" ) {
                    isXScale = true;
                    xScaleFactor = Unit::convert(1.0,
                                             xunit.toAscii().constData(),
                                             xDPUnit.toAscii().constData());
                    xunit = xDPUnit;
                }


                //
                // yunit and calc y scale if DP unit not equal to model unit
                //
                bool isYScale = false;
                double yScaleFactor = 1.0;
                QString yunit = curveModel->headerData
                                       (2,Qt::Horizontal,Param::Unit).toString();
                QModelIndex yDPUnitIdx = model()->index(5,0,pidx);
                QString yDPUnit = model()->data(yDPUnitIdx).toString();
                if ( !yDPUnit.isEmpty() &&
                     yunit != yDPUnit && yDPUnit != "--" && yunit != "--" ) {
                    isYScale = true;
                    yScaleFactor = Unit::convert(1.0,
                                             yunit.toAscii().constData(),
                                             yDPUnit.toAscii().constData());
                    yunit = yDPUnit;
                }

                // Set scale factor for either x or y units
                if ( isXScale || isYScale ) {
                    delete curveModel;
                    curveModel = _monteModel->curve(runID,
                                                    xparam, yparam,
                                                    xScaleFactor, yScaleFactor);
                }

                Plot* plot = _idx2Plot(gpidx);
                TrickCurve* curve = plot->axisRect()->addCurve(curveModel);
                plot->axisRect()->zoomToFit();
                _plot2Curves[plot].append(curve);

                QModelIndex xAxisLabelIdx = model()->index(0,0,g2pidx);
                QString xAxisLabel = _appendUnitToAxisLabel(xAxisLabelIdx,xunit);
                plot->setXAxisLabel(xAxisLabel);
                model()->setData(xAxisLabelIdx,xAxisLabel);

                QModelIndex yAxisLabelIdx = model()->index(1,0,g2pidx);
                QString yAxisLabel = _appendUnitToAxisLabel(yAxisLabelIdx,yunit);
                plot->setYAxisLabel(yAxisLabel);
                model()->setData(yAxisLabelIdx,yAxisLabel);

                if ( pidx.row() == 1 && _isShowCurveDiff ) {
                    plot->axisRect()->showCurveDiff();
                }
            }
        }
    }
}

void PlotBookView::rowsAboutToBeRemoved(const QModelIndex &pidx,
                                        int start, int end)
{
    QModelIndex gpidx = model()->parent(pidx);
    QModelIndex g2pidx = model()->parent(gpidx);
    QModelIndex g3pidx = model()->parent(g2pidx);
    QModelIndex g4pidx = model()->parent(g3pidx);

    for ( int row = start; row < end+1; ++row) {

        QModelIndex idx = model()->index(row,0,pidx);

        if ( _isPageIdx(idx) ) {
            // Page
            QWidget* page = _idx2Page(idx);
            int row = _plotModel->pageIdxs().indexOf(idx);
            _pages.remove(row);
            _page2grid.remove(page);
            _page2Plots.remove(page);
            _page2startTime.remove(page);
            _page2stopTime.remove(page);
            if ( ! _isTabCloseRequested ) {
                // Tab widget will remove its own tab
                _nb->removeTab(idx.row());
            }
            page->deleteLater();
        } else if ( ! gpidx.isValid() ) {
            // Plot
            QWidget* page = _idx2Page(pidx);
            QGridLayout* grid = static_cast<QGridLayout*>(page->layout());

            Plot* plot = _idx2Plot(idx);
            QVector<TrickCurve*> curves = _plot2Curves.value(plot);
            foreach ( TrickCurve* curve, curves ) {
                disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            }
            int plotId = _page2Plots[page].indexOf(plot);
            _page2Plots[page].remove(plotId);
            _plot2Curves.remove(plot);
            grid->removeWidget(plot);
            delete plot;

            QList<QWidget*> plots;
            int nPlots = grid->count();
            for ( int i = nPlots-1; i >= 0 ; --i ) {
                QWidget* plot = grid->takeAt(i)->widget();
                plots << plot;
            }

            switch (nPlots)
            {
            case 0:
            {
                break;
            }
            case 1:
            {
                grid->addWidget(plots.at(0),0,0);
                break;
            }
            case 2:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),1,0);
                break;
            }
            case 3:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),1,0);
                grid->addWidget(plots.at(2),2,0);
                break;
            }
            case 4:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                break;
            }
            case 5:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0,1,2);
                break;
            }
            case 6:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0);
                grid->addWidget(plots.at(5),2,1);
                break;
            }
            case 7:
            {
                grid->addWidget(plots.at(0),0,0);
                grid->addWidget(plots.at(1),0,1);
                grid->addWidget(plots.at(2),1,0);
                grid->addWidget(plots.at(3),1,1);
                grid->addWidget(plots.at(4),2,0);
                grid->addWidget(plots.at(5),2,1);
                grid->addWidget(plots.at(6),3,0,1,2);
                break;
            }
            default:
            {
                qDebug() << "snap [error]: can't handle more than 7 plots!";
            }
            }
        } else if ( ! g2pidx.isValid() ) {
            // 0:xAxisLabel, 1:yAxisLabel, 2:Curves
            if ( idx.row() == 2 ) {
                // TODO Curves: remove all curves
            }
        } else if ( ! g3pidx.isValid() ) {
            // Curve (TODO: untested)
            Plot* plot = _idx2Plot(pidx);
            TrickCurve* curve = _idx2Curve(idx);
            disconnect(curve,SIGNAL(selectionChanged(TrickCurve*)));
            plot->removePlottable(curve);
            _plot2Curves[plot].remove(idx.row());
            plot->replot();
        } else if ( ! g4pidx.isValid() ) {
            // t,x,y,run
        }
    }
}

QModelIndex PlotBookView::_curve2Idx(TrickCurve *curve)
{
    Plot* plot = static_cast<Plot*>(curve->parentPlot());
    QModelIndex plotIdx = _plot2Idx(plot);
    int rowCurve = _plot2Curves.value(plot).indexOf(curve);
    QModelIndex curvesIdx;
    for ( int i = 0; i < plotIdx.model()->rowCount(plotIdx); ++i) {
        QModelIndex idx = plotIdx.model()->index(i,0,plotIdx);
        if ( plotIdx.model()->data(idx).toString() == "Curves" ) {
            curvesIdx = idx;
            break;
        }
    }
    if ( !curvesIdx.isValid() ) {
        qDebug() << "snap [bad scoobies]: \"Curves\" item not found.";
        exit(-1);
    }
    QModelIndex curveIdx = model()->index(rowCurve,0,curvesIdx);
    return curveIdx;
}

QWidget *PlotBookView::_idx2Page(const QModelIndex &idx) const
{
    if ( !idx.isValid() ) return 0;

    // if this is start/stop time
    if ( !idx.parent().isValid() && !_plotModel->isPageIdx(idx) ) {
        return 0;
    }

    QModelIndex pageIdx(idx);
    while ( pageIdx.parent().isValid() ) {
        pageIdx = pageIdx.parent();
    }
    int r = _plotModel->pageIdxs().indexOf(pageIdx);
    QWidget* page = _pages.at(r);

    return page;
}

QGridLayout *PlotBookView::_idx2Grid(const QModelIndex &idx) const
{
    QWidget* page = _idx2Page(idx);
    QGridLayout* grid = _page2grid.value(page);
    return grid;
}

Plot *PlotBookView::_idx2Plot(const QModelIndex &idx) const
{
    if ( !idx.isValid() || !idx.parent().isValid() ) {
        // root or page
        return 0;
    }

    Plot* plot = 0 ;
    QWidget* page = _idx2Page(idx);
    if ( page ) {
        QModelIndex plotIdx(idx);
        while ( plotIdx.parent().parent().isValid() ) {
            plotIdx = plotIdx.parent();
        }
        QModelIndex pageIdx = plotIdx.parent();
        int iPlot = _plotModel->plotIdxs(pageIdx).indexOf(plotIdx);
        if ( iPlot >= 0 ) {
            plot = _page2Plots.value(page).at(iPlot);
        }
    }
    return plot;
}

TrickCurve *PlotBookView::_idx2Curve(const QModelIndex &idx) const
{
    TrickCurve* curve = 0 ;
    Plot* plot = _idx2Plot(idx);
    if ( plot ) {
        curve = _plot2Curves.value(plot).at(idx.row());
    }
    return curve;
}

QModelIndex PlotBookView::_page2Idx(QWidget *page) const
{
    if ( !model() || !page) return QModelIndex();

    QModelIndex idx;
    int row = _pages.indexOf(page);
    if ( row >= 0 ) {
        idx = _plotModel->pageIdxs().at(row);
    }
    return idx;
}

QModelIndex PlotBookView::_plot2Idx(Plot *plot) const
{
    if ( !model() || !plot ) return QModelIndex();

    QModelIndex plotIdx;
    QWidget* page = plot->parentWidget();
    int rowPage = _pages.indexOf(page);
    QModelIndex pageIdx = _plotModel->pageIdxs().at(rowPage);
    QModelIndexList plotIdxs = _plotModel->plotIdxs(pageIdx);
    int iPlot =  _page2Plots.value(page).indexOf(plot);
    if ( iPlot >= 0 ) {
        plotIdx = plotIdxs.at(iPlot);
    }
    return plotIdx;
}

bool PlotBookView::_isPageIdx(const QModelIndex &idx)
{
    return _plotModel->isPageIdx(idx);
}

bool PlotBookView::_isPlotIdx(const QModelIndex &idx)
{
    if ( idx.isValid() && idx.parent().isValid() &&
         !idx.parent().parent().isValid() ) {
        return true;
    } else {
        return false;
    }
}

bool PlotBookView::_isCurveIdx(const QModelIndex &idx)
{
    if ( model()->data(idx.parent()).toString() == "Curves" ) {
        return true;
    } else {
        return false;
    }
}

QString PlotBookView::_appendUnitToAxisLabel(const QModelIndex axisLabelIdx,
                                            const QString& unit ) const
{
    QString axisLabel =  model()->data(axisLabelIdx).toString();
    QString curveUnit = unit;

    if ( axisLabel.contains('(') ) {
        // assumes that label parens are for units
        QString labelUnit;
        int a = axisLabel.lastIndexOf('(');
        int b = axisLabel.lastIndexOf(')');
        if ( b > 0 ) {
            labelUnit = axisLabel.mid(a+1,b-a-1);
        }
        if ( labelUnit != curveUnit && labelUnit !="--" ) {
            // units not the same across curves, so make unit (--)
            axisLabel.replace(a+1,labelUnit.size(),"--");
        }
    } else {

        axisLabel += " (";
        axisLabel += curveUnit;
        axisLabel += ")";
    }

    return axisLabel;
}
