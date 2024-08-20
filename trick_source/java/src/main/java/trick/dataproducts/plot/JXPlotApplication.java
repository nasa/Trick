
package trick.dataproducts.plot;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.FieldPosition;
import java.util.ArrayList;

import javax.swing.DefaultListModel;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JToolBar;
import javax.swing.ListModel;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.xml.parsers.ParserConfigurationException;

import org.jdesktop.application.Application;
import org.jdesktop.swingx.JXMultiSplitPane;
import org.jdesktop.swingx.JXTree;
import org.jdesktop.swingx.MultiSplitLayout;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.title.TextTitle;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.xml.sax.SAXException;

import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.PDPage;
import org.apache.pdfbox.pdmodel.PDPageContentStream;
import org.apache.pdfbox.pdmodel.common.PDRectangle;
import org.apache.pdfbox.pdmodel.font.PDFont;
import org.apache.pdfbox.pdmodel.font.PDType1Font;
import org.apache.pdfbox.pdmodel.graphics.image.JPEGFactory;
import org.apache.pdfbox.pdmodel.graphics.image.PDImageXObject;

import trick.common.framework.BaseApplication;
import trick.common.framework.View;
import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.common.utils.SwingAction;
import trick.dataproducts.plot.utils.PlotUtils;
import trick.dataproducts.plot.utils.TrickChart;
import trick.dataproducts.plot.utils.TrickChartFrame;
import trick.dataproducts.plot.utils.TrickFrame;
import trick.dataproducts.plot.utils.TrickTableFrame;
import trick.dataproducts.plot.utils.TrickXYLineAndShapeRenderer;
import trick.dataproducts.plot.utils.TrickXYPlot;
import trick.dataproducts.plot.utils.TrickXYSeries;
import trick.dataproducts.trickqp.utils.Product;
import trick.dataproducts.trickqp.utils.ProductCurve;
import trick.dataproducts.trickqp.utils.ProductDataPanel;
import trick.dataproducts.trickqp.utils.ProductPage;
import trick.dataproducts.trickqp.utils.ProductPlot;
import trick.dataproducts.trickqp.utils.ProductTable;
import trick.dataproducts.trickqp.utils.ProductVar;
import trick.dataproducts.trickqp.utils.ProductVarcase;
import trick.dataproducts.trickqp.utils.ProductDomParser;
import trick.dataproducts.utils.Session;
import trick.dataproducts.utils.SessionRun;
import trick.dataproducts.utils.SessionDomParser;

/**
 * Plotting using Java.
 *
 * @author Hong Chen
 * @since Trick 13
 */
public class JXPlotApplication extends TrickApplication {

  //========================================
    //  Public data
    //========================================


    //========================================
    //  Protected data
    //========================================


    private JXTree productTree;
    private DefaultMutableTreeNode productTreeRoot;

    @SuppressWarnings("rawtypes")
    private JList tableList;

    private static int chartInitialLocX = 50;
    private static int chartInitialLocY = 50;
    private Session sessionObject;

    //========================================
    //  Constructors
    //========================================


    //========================================
    //    Actions
    //========================================
    @SwingAction
    public void saveAllToPDF() {
        File file = UIUtils.chooseSaveFile(null, "plot_", "pdf", getMainFrame());
        if (file == null) {
            return;
        }
        try {
            int dpFileCount = productTreeRoot.getChildCount();
            if (dpFileCount > 0) {
                PDDocument document = new PDDocument();
                for (int i = 0; i < dpFileCount; i++) {
                    DefaultMutableTreeNode eachDPFile = (DefaultMutableTreeNode)productTree.getModel().getChild(productTreeRoot, i);
                    int pageCount = eachDPFile.getChildCount();
                    for (int j = 0; j < pageCount; j++) {
                        DefaultMutableTreeNode eachPage = (DefaultMutableTreeNode)productTree.getModel().getChild(eachDPFile, j);
                        if (eachPage.getUserObject() instanceof TrickChartFrame) {
                            TrickChartFrame theFrame = (TrickChartFrame)eachPage.getUserObject();
                            theFrame.writePDFPage(document);
                        }
                    }
                }
                // document needs to be closed after the PDF file is saved
                document.save(file);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    //========================================
    //  Set/Get methods
    //========================================



    //========================================
    //  Methods
    //========================================
    /**
     * Main method for this application.
     * @param args command line arguments
     */
    public static void main(String[] args) {
        BaseApplication.launch(JXPlotApplication.class, args);
    }

    /**
     * Makes initialization as needed. This is called before startup().
     *
     * @see #startup
     */
    @SuppressWarnings({ "rawtypes", "unchecked" })
    @Override
    protected void initialize(String[] args) {
        super.initialize(args);

        // TODO: make it as a tree
        final ListModel listModel = new DefaultListModel();
        tableList = new JList(listModel);
        tableList.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                int index = tableList.locationToIndex(e.getPoint());
                if (index != -1 && listModel.getElementAt(index) instanceof TrickFrame) {
                    TrickFrame theFrame = (TrickFrame)listModel.getElementAt(index);
                    if (!theFrame.isVisible()) {
                        theFrame.setVisible(true);
                    } else {
                        theFrame.requestFocus();
                        theFrame.toFront();
                    }
                }
            }
        });

        productTreeRoot = new DefaultMutableTreeNode("Root");
        DefaultTreeModel treeModel = new DefaultTreeModel(productTreeRoot);
        productTree = new JXTree(treeModel);
        productTree.setRootVisible(false);
        productTree.setShowsRootHandles(true);
        productTree.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                TreePath clickedPath = null;
                DefaultMutableTreeNode clickedNode = null;

                // when nothing is viewable, do nothing and return immediately 
                if (productTree.getClosestPathForLocation(e.getX(), e.getY()) == null) {
                    return;
                }
                clickedPath = productTree.getClosestPathForLocation(e.getX(), e.getY());
                if (clickedPath.getLastPathComponent() instanceof DefaultMutableTreeNode) {
                    clickedNode = (DefaultMutableTreeNode)clickedPath.getLastPathComponent();
                }

                if (clickedNode.getUserObject() instanceof TrickFrame) {
                    TrickFrame theFrame = (TrickFrame)clickedNode.getUserObject();
                    if (!theFrame.isVisible()) {
                        theFrame.setVisible(true);
                    } else {
                        theFrame.requestFocus();
                        theFrame.toFront();
                    }
                }
            }
        });

        if (args.length > 0) {
            openSessionFile(args[0]);
        }
    }


    /**
     * Opens a session file for plotting.
     */
    private void openSessionFile(String sessionFile) {
        File file = new File(sessionFile);
        if (file != null) {
            try {
                sessionObject = SessionDomParser.parse(file);
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (ParserConfigurationException e) {
                e.printStackTrace();
            } catch (SAXException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }

            for (String dpFile : sessionObject.getProductFiles()) {
                // if parsing is failed, stop!
                try {
                    Product productObject = ProductDomParser.parse(new File(dpFile)) ;

                    if (productObject != null) {
                        // if the mode is not defined or is set to PLOT_MODE, plot the data.
                        if (sessionObject.getMode() == null || sessionObject.getMode().equals(Session.MODE_OPTIONS[Session.PLOT_MODE])) {
                            DefaultMutableTreeNode dpFileNode = new DefaultMutableTreeNode(dpFile);
                            productTreeRoot.add(dpFileNode);
                            ((DefaultTreeModel) productTree.getModel()).reload();
                            // if there is any page
                            if (productObject.getPageList() != null && productObject.getPageList().size() > 0) {
                                for (ProductPage eachPage : productObject.getPageList()) {
                                    if (eachPage != null) {
                                        createAndShowCharts(dpFileNode, eachPage);
                                    }
                                }
                            }
                        } else if (sessionObject.getMode().equals(Session.MODE_OPTIONS[Session.TABLE_MODE])) {
                            if (productObject.getTableList() != null && productObject.getTableList().size() > 0) {
                                for (ProductTable eachTable : productObject.getTableList()) {
                                    createAndShowTable(tableList, productObject.getTitle(), eachTable);
                                }
                            }
                        }
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    System.out.println("\nERROR parsing " + dpFile + " !!!\n");
                }
            }
        }
    }

    /**
     * Helper method for creating table.
     */
    @SuppressWarnings({ "rawtypes" })
    private void createAndShowTable(JList list, String title, ProductTable theTable) {
        showTable(theTable);
    }

    /**
     * Helper method for creating charts for a page.
     */
    private void createAndShowCharts(DefaultMutableTreeNode dpFileNode, ProductPage thePage) {
        ArrayList<TrickChart> chartList = new ArrayList<TrickChart>();

        if (sessionObject.getPresentation().equals(Session.PRESENTATION_OPTIONS[Session.SIMPLE_PRESENTATION])) {
            createChartsForSingle(thePage, chartList);
        } else if (sessionObject.getPresentation().equals(Session.PRESENTATION_OPTIONS[Session.COMPARISON_PRESENTATION])) {
            createChartsForComparison(thePage, chartList);
        } else if (sessionObject.getPresentation().equals(Session.PRESENTATION_OPTIONS[Session.DELTA_PRESENTATION])) {
            createChartsForDelta(thePage, chartList, false);
        } else if (sessionObject.getPresentation().equals(Session.PRESENTATION_OPTIONS[Session.CONTRAST_PRESENTATION])) {
            createChartsForContrast(thePage, chartList);
        }
        showPagePlots(dpFileNode, thePage, chartList);
    }

    /**
     * Helper method for creating all charts using contrast plotting for a particular {@link ProductPage} 
     * and adding the charts to a list. Contrast plotting shows both comparison and delta plots on the 
     * same page.
     */
    private void createChartsForContrast(ProductPage thePage, ArrayList<TrickChart> chartList) {
        createChartsForComparison(thePage, chartList);
        createChartsForDelta(thePage, chartList, true);
    }

    /**
     * Helper method for creating all charts using delta plotting for a particular {@link ProductPage} 
     * and adding the charts to a list.
     */
    private void createChartsForDelta(ProductPage thePage, ArrayList<TrickChart> chartList, boolean isContrast) {
        for (ProductPlot eachPlot : thePage.getPlotList()) {
            XYSeriesCollection dataset = null;
            int totalRuns = sessionObject.getRuns().size();
            if (totalRuns == 1) {
                dataset = new XYSeriesCollection();
                for (ProductCurve eachCurve : eachPlot.getCurveList()) {
                    //dataset.addSeries(getXYSeriesForCurve(sessionObject.getRuns().get(0), thePage, eachPlot, eachCurve));
                    addSeriesToDataset(dataset, getXYSeriesForCurve(sessionObject.getRuns().get(0), thePage, eachPlot, eachCurve));
                }
                createAndAddChart("(" + sessionObject.getRuns().get(0).getDirName() +")", chartList, thePage, eachPlot, dataset, false);
            } else {
                if (isContrast) {
                    dataset = new XYSeriesCollection();
                }
                for (int i = 0; i < totalRuns - 1; i++) {
                    for (int j = i + 1; j < totalRuns; j++) {
                        if (!isContrast) {
                            dataset = new XYSeriesCollection();
                        }
                        for (ProductCurve eachCurve : eachPlot.getCurveList()) {
                            TrickXYSeries series1 = getXYSeriesForCurve(sessionObject.getRuns().get(i), thePage, eachPlot, eachCurve);
                            TrickXYSeries series2 = getXYSeriesForCurve(sessionObject.getRuns().get(j), thePage, eachPlot, eachCurve);
                            if (series1 == null || series2 == null) {
                                continue;
                            }
                            int totalCount = series1.getItemCount() < series2.getItemCount() ? series1.getItemCount() : series2.getItemCount();

                            TrickXYSeries deltaSeries = new TrickXYSeries("DELTA: "
                                    + series1.getDescription().substring(0, series1.getDescription().lastIndexOf("["))
                                    + " ["
                                    + sessionObject.getRuns().get(i).getDirName()
                                    + " - "
                                    + sessionObject.getRuns().get(j).getDirName() + "]", false, true);
                            for (int k = 0; k < totalCount; k++) {
                                deltaSeries.add(series1.getX(k).doubleValue(), series1.getY(k).doubleValue() - series2.getY(k).doubleValue());
                            }
                            //dataset.addSeries(deltaSeries);
                            addSeriesToDataset(dataset, deltaSeries);
                            deltaSeries.setXVar(series1.getXVar());
                            deltaSeries.setYVar(series1.getYVar());
                        }
                        if (!isContrast) {
                            createAndAddChart("(Difference)", chartList, thePage, eachPlot, dataset, isContrast);
                        }
                    }
                }
                if (isContrast) {
                    createAndAddChart("(Difference)", chartList, thePage, eachPlot, dataset, true);
                }
            }
        }
    }

    /**
     * Helper method for creating all charts using comparison plotting for a particular {@link ProductPage}
     * and adding the charts to a list.
     */
    private void createChartsForComparison(ProductPage thePage, ArrayList<TrickChart> chartList) {
        for (ProductPlot eachPlot : thePage.getPlotList()) {
            XYSeriesCollection dataset = getComparisonPlotDataset(thePage, eachPlot);
            createAndAddChart("(Compare)" , chartList, thePage, eachPlot, dataset, false);
        }
    }

    /**
     * Helper method for creating all charts using single plotting for a particular {@link ProductPage}
     * and adding the charts to a list.
     */
    private void createChartsForSingle(ProductPage thePage, ArrayList<TrickChart> chartList) {
        for (ProductPlot eachPlot : thePage.getPlotList()) {
            for (SessionRun eachRun : sessionObject.getRuns()) {
                XYSeriesCollection dataset = getSinglePlotDataset(eachRun, thePage, eachPlot);
                createAndAddChart("("+eachRun.getDirName()+")", chartList, thePage, eachPlot, dataset, false);
            }
        }
    }

    /**
     * Creates a chart for the specified {@link XYSeriesCollection} and {@link ProductPlot} and then
     * adds to the list.
     */
    private void createAndAddChart(String subTitle, ArrayList<TrickChart> chartList, ProductPage aPage, ProductPlot aPlot, XYSeriesCollection dataset, boolean isContrast) {
        XYPlot plot = createXYPlot(aPage, aPlot, dataset);

        TrickChart chart = new TrickChart(aPlot.getTitle(), TrickChart.TRICK_DEFAULT_TITLE_FONT, plot, true);
        //TrickChartTheme chartTheme = new TrickChartTheme("TrickChart");

        //chartTheme.apply(chart);
        configureChart(subTitle, chart, aPage, aPlot, isContrast);
        chartList.add(chart);
    }

    /**
     * Helper method for creating a {@link XYPlot} for the specified {@link ProductPlot}
     * and logged data.
     */
    private XYPlot createXYPlot(ProductPage aPage, ProductPlot aPlot, XYSeriesCollection dataset) {
        DecimalFormat newFormat = new DecimalScientificFormat();

        NumberAxis xAxis = new NumberAxis(null);
        xAxis.setNumberFormatOverride(newFormat);
        NumberAxis yAxis = new NumberAxis(null);
        yAxis.setNumberFormatOverride(newFormat);

        TrickXYLineAndShapeRenderer renderer = new TrickXYLineAndShapeRenderer();
        return new TrickXYPlot(aPage, aPlot, dataset, xAxis, yAxis, renderer);
    }

    /**
     * Configures the specified chart.
     */
    private void configureChart(String subTitleText, TrickChart chart, ProductPage aPage, ProductPlot aPlot, boolean isContrast) {
        // TODO: will see if need to remove the legend for contrast plotting
        //if (isContrast) {
        //    chart.removeLegend();
        //} else {
        if (aPlot.getForegroundColor() != null) {
            chart.getLegend().setItemPaint(aPlot.getForegroundColor());
        } else if (aPage.getForegroundColor() != null) {
            chart.getLegend().setItemPaint(aPage.getForegroundColor());
        }
        if (aPlot.getBackgroundColor() != null) {
            chart.getLegend().setBackgroundPaint(aPlot.getBackgroundColor());
        } else if (aPage.getBackgroundColor() != null) {
            chart.getLegend().setBackgroundPaint(aPage.getBackgroundColor());
        }
        //}

        TextTitle subTitle = null;
        if (subTitleText != null && !subTitleText.isEmpty()) {
            subTitle = new TextTitle(subTitleText, TrickChart.TRICK_DEFAULT_SUBTITLE_FONT);
        }

        // Font
        if (aPlot.getFont() != null && !(aPlot.getFont().isEmpty())) {
            Font plotFont = ProductDataPanel.getFontFromText(aPlot.getFont());
            chart.getTitle().setFont(plotFont);
            //if (!isContrast) {
                chart.getLegend().setItemFont(plotFont);
            //}
            if (subTitleText != null && !subTitleText.isEmpty()) {
                subTitle.setFont(plotFont);
            }
        }

        if (subTitle != null) {
            chart.addSubtitle(subTitle);
        }

    }

    /**
     * Shows all plots on a page.
     */
    private void showPagePlots(DefaultMutableTreeNode dpFileNode, ProductPage thePage, ArrayList<TrickChart> chartList) {
        if (chartList.size() > 0) {
            TrickChartFrame chartFrame = new TrickChartFrame(thePage.getTitle(), getMainFrame().getIconImage(), thePage, chartList);

            dpFileNode.add(new DefaultMutableTreeNode(chartFrame));
            ((DefaultTreeModel)productTree.getModel()).reload();

            locateFrame(chartFrame);
        }
    }

    private void locateFrame(TrickFrame frame) {
        frame.setLocation(chartInitialLocX, chartInitialLocY);
        chartInitialLocX += 30;
        chartInitialLocY += 30;
        frame.pack();
        frame.setVisible(true);
    }

    /**
     * Display the table.
     */
    @SuppressWarnings({ "rawtypes", "unchecked" })
    private void showTable(ProductTable theTable) {
        if (theTable == null || theTable.getColumnList() == null || theTable.getColumnList().size() < 1) {
            return;
        }

        TrickTableFrame tableFrame = new TrickTableFrame(theTable.getTitle(), getMainFrame().getIconImage(), sessionObject, theTable);
        tableFrame.setLocation(this.getMainFrame().getLocation());
        if (tableList.getModel() instanceof DefaultListModel) {
            DefaultListModel listModel = (DefaultListModel)tableList.getModel();
            listModel.addElement(tableFrame);
        }
        locateFrame(tableFrame);
    }


    /**
     * Gets the dataset for single plotting. Note that for single plotting,
     * data from different runs is showing on different plots.
     * @param page TODO
     */
    private XYSeriesCollection getSinglePlotDataset(SessionRun theRun, ProductPage page, ProductPlot plot) {
        XYSeriesCollection dataset = new XYSeriesCollection();

        for (ProductCurve eachCurve : plot.getCurveList()) {
            if (eachCurve != null && eachCurve.isValid()) {
                TrickXYSeries series = getXYSeriesForCurve(theRun, page, plot, eachCurve);
                if (series != null) {
                    //dataset.addSeries(series);
                    addSeriesToDataset(dataset, series);
                }
            }
        }
        return dataset;
    }

    /**
     * Helper method for getting {@link XYSeries} for the specified {@link ProductCurve}.
     * @param page TODO
     */
    private TrickXYSeries getXYSeriesForCurve(SessionRun theRun, ProductPage page, ProductPlot plot, ProductCurve eachCurve) {
        TrickXYSeries series = null;
        ProductVar theXVar = null;
        ProductVar theYVar = null;

        // a curve either has varcase or X & Y and doesn't have both at the same time
        if (eachCurve.hasVarcase()) {
            for (ProductVarcase eachVarcase : eachCurve.getVarcaseList()) {
                if (eachVarcase.isValid()) {
                    theXVar = eachVarcase.getX();
                    theYVar = eachVarcase.getY();

                    series = PlotUtils.getXYVarSeries(theRun, page, plot, theXVar, theYVar);

                    // if XYSeries for this varcase can be found, skip the rest. otherwise, keep checking the next varcase
                    if (series != null) {
                        break;
                    }
                }
            }
        } else {
            theXVar = eachCurve.getX();
            theYVar = eachCurve.getY();

            series = PlotUtils.getXYVarSeries(theRun, page, plot, theXVar, theYVar);
        }
        return series;
    }


    /**
     * Helper method for setting X and Y variable scales.
     * If the variable itself doesn't specify the scale, the plot scale is used if available.
     * If the variable does have the scale specified, the plot scale is ignored.
     */
    /*private void setXYVarScale(ProductPlot plot, ProductVar theXVar, ProductVar theYVar) {
        if (theXVar.getScale() == null) {
            if (plot.getXScale() != null && !plot.getXScale().isEmpty()) {
                try {
                    theXVar.setScale(Double.parseDouble(plot.getXScale()));
                } catch (Exception e) {
                    // if there is an exception, just simply don't set the scale.
                }
            }
        }
        if (theYVar.getScale() == null) {
            if (plot.getYScale() != null && !plot.getYScale().isEmpty()) {
                try {
                    theYVar.setScale(Double.parseDouble(plot.getYScale()));
                } catch (Exception e) {
                    // if there is an exception, just simply don't set the scale.
                }
            }
        }
    }*/

    /**
     * Gets the dataset for comparison plotting. Note that for comparison plotting,
     * data from different runs is showing on the same plot.
     * @param page TODO
     */
    private XYSeriesCollection getComparisonPlotDataset(ProductPage page, ProductPlot plot) {
        XYSeriesCollection dataset = new XYSeriesCollection();
        for (ProductCurve eachCurve : plot.getCurveList()) {
            if (eachCurve != null) {
                for (SessionRun eachRun : sessionObject.getRuns()) {
                    TrickXYSeries series = getXYSeriesForCurve(eachRun, page, plot, eachCurve);
                    if (series != null) {
                        addSeriesToDataset(dataset, series);
                    }
                }
            }
        }
        return dataset;
    }

    /**
     * Helper method for adding the data series index as the prefix for the key of a series
     * so that the key is guaranteed to be unique.
     */
    private void addSeriesToDataset(XYSeriesCollection dataset, TrickXYSeries series) {
        String theKey = series.getKey().toString();
        theKey = "Series " + dataset.getSeriesCount() + ": " + theKey.toString();
        series.setKey(theKey);
        dataset.addSeries(series);
    }

    /**
     * Starts building GUI. This is called after initialize.
     * Once startup() is done, ready() is called.
     * 
     * @see #initialize
     * @see #ready
     */
    @Override
    protected void startup() {
        super.startup();

        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());
        //view.setToolBar(createToolBar());
        show(view);

        //getMainFrame().setLocation(0, 0);

    }

    @Override
    protected void ready() {
        super.ready();
    }

    @Override
    protected void shutdown() {
        try {
            super.shutdown();
        } catch (Exception e) {
            // add this catch here in order not to show the warning
            // exception when try to exit the application while the
            // plotting frame is maximized. this is due to the problem
            // with storing session of the framework. here basically,
            // the plotting frame session is not saved which we don't
            // need it anyway.
            // do nothing
        }
    }

    /**
     * Creates the main panel. This is required by TrickApplication.
     * 
     * @return a {@link JComponent} as the main panel.
     */
    @Override
    protected JComponent createMainPanel() {

        JXMultiSplitPane msp = new JXMultiSplitPane();

        String layoutDef =
            "(COLUMN (LEAF name=top weight=0.7)" +
            //"        (LEAF name=middle weight=0.2)" +
            "        (LEAF name=bottom weight=0.3)" +
            ")";
        MultiSplitLayout.Node modelRoot = MultiSplitLayout.parseModel( layoutDef );

        msp.getMultiSplitLayout().setModel( modelRoot );


        JPanel productsPanel = UIUtils.createSearchableTitledPanel("Products & Pages", productTree, null);
        productsPanel.setMinimumSize(new Dimension(250, 50));
        productsPanel.setPreferredSize(new Dimension(500, 150));

        JPanel tablesPanel = UIUtils.createSearchableTitledPanel("Tables", tableList, null);
        tablesPanel.setMinimumSize(new Dimension(250, 50));
        //pagesPanel.setPreferredSize(new Dimension(400, 50));

        msp.add(productsPanel, "top" );
        //msp.add(pagesPanel, "middle" );
        msp.add(tablesPanel, "bottom" );

        return msp;
    }



    /**
     * Create the JMenuBar for this application.
     */
    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();

        JMenu menu = menuBar.getMenu(0);
        menu.add(new JSeparator(), 0);
        //menu.add(new JMenuItem(getAction("saveALLToPDF")), 0);
        menu.add(createMenuItem("saveAllToPDF"), 0);
        return menuBar;
    }


    /**
     * Creates the tool bar for the application.
     * 
     * @return a {@link JToolBar} for the application.
     */
    @Override
    protected JToolBar createToolBar() {
        return new JToolBar();
    }

    /*
    TODO: may want to add feature for righ_clicking a series to have
          a pop up menu for making it visible or invisible.
    @Override
    public void actionPerformed(ActionEvent e) {
        int series = -1;
        if (e.getActionCommand().equals("S1")) {
            series = 0;
        }
        else if (e.getActionCommand().equals("S2")) {
            series = 1;
        }
        else if (e.getActionCommand().equals("S3")) {
            series = 2;
        }
        if (series >= 0) {
            boolean visible = this.renderer.getItemVisible(series, 0);
            this.renderer.setSeriesVisible(series, new Boolean(!visible));
        }

    }*/

    //========================================
    //    Inner classes
    //========================================
    public class DecimalScientificFormat extends DecimalFormat {

        private static final long serialVersionUID = -7709354031636993724L;

        private  DecimalFormat normalFormat = new DecimalFormat("0.#####");

        private DecimalFormat scientificFormat = new DecimalFormat("0.#####E0");

        @Override
        public StringBuffer format(double number, StringBuffer result, FieldPosition fieldPosition) {
            String decimalFormat = normalFormat.format(number);
            String numberString = Double.toString(number);

            boolean needScientificNotion = false;
            int dotLoc = numberString.indexOf(".");

            if (numberString.indexOf("E") != -1) {
                needScientificNotion = true;
            } else if (dotLoc == -1) {
                if (numberString.length() > 5) {
                    needScientificNotion = true;
                }
            } else {
                if (dotLoc >= 5) {
                    needScientificNotion = true;
                } else if (numberString.length() - dotLoc >= 6) {
                    needScientificNotion = true;
                }
            }

            if (number != 0.0 && needScientificNotion) {
                return scientificFormat.format(number, result, fieldPosition);
            } else {
                return result.append(decimalFormat);
            }
        }
    }
}
