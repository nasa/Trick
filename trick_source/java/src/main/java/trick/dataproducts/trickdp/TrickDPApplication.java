
//========================================
//    Package
//========================================
package trick.dataproducts.trickdp;

//========================================
//    Imports
//========================================
import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Arrays;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.DropMode;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JToolBar;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.tree.DefaultMutableTreeNode;

import org.jdesktop.application.Application;

import trick.common.framework.BaseApplication;
import trick.common.TrickApplication;
import trick.common.ui.TrickFileFilter;
import trick.common.ui.UIUtils;
import trick.common.ui.panels.DataPanel;
import trick.common.ui.panels.ListPanel;
import trick.common.utils.SwingAction;
import trick.dataproducts.DataProductsApplication;
import trick.dataproducts.trickdp.utils.DPRemoteCallInterface;
import trick.dataproducts.trickdp.utils.DPRemoteCallInterfaceImpl;
import trick.dataproducts.trickdp.utils.PDFBooklet;
import trick.dataproducts.trickdp.utils.TrickDPActionController;
import trick.dataproducts.trickqp.utils.QPRemoteCallInterface;
import trick.dataproducts.utils.FileTreePanel;
import trick.dataproducts.utils.Session;
import trick.dataproducts.utils.SessionRun;
import trick.dataproducts.utils.SessionRunTransferHandler;
import trick.dataproducts.utils.SimDPTree;
import trick.dataproducts.utils.SimRunTree;


/**
 * Trick DP Application that extends {@link DataProductsApplication}.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class TrickDPApplication extends DataProductsApplication {

    //========================================
    //    Public data
    //========================================
    public FileTreePanel simRunTree;
    public FileTreePanel simDPTree;
    public ListPanel runList;
    public ListPanel dpList;

    public String rightClickedDP = null;

    //========================================
    //    Protected data
    //========================================


    //========================================
    //    Private Data
    //========================================
    private TrickDPActionController actionController = null;

    // the current dir where the app is started from
    private String currentDir = System.getProperty("user.dir");

    // to call functions in trick_qp
    private QPRemoteCallInterface qpRemoteCall;

    // to let other application to call this app (trick_dp)
    private DPRemoteCallInterface dpRemoteCall;

    // menu items for gnuplot terminal
    private JRadioButtonMenuItem[] gnuplotTerminalMenuItems;

    // menu items for device choices
    private JRadioButtonMenuItem[] deviceMenuItems;

    //========================================
    //    Constructors
    //========================================


    //========================================
    //    Actions
    //========================================
    @SwingAction
    public void newSession() {
        actionController.handleNewSession();
    }

    @SwingAction
    public void openSession() {
        actionController.handleOpenSession();
    }

    @SwingAction
    public void saveSession() {
        actionController.handleSaveSession();
    }

    @SwingAction
    public void refreshSession() {
        actionController.handleRefreshSession();
    }

    @SwingAction
    public void singlePlot() {
        setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.SIMPLE_PRESENTATION]);
        setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
        actionController.handleSinglePlot();
    }

    @SwingAction
    public void comparisonPlot() {
        setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.COMPARISON_PRESENTATION]);
        setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
        actionController.handleComparisonPlot();
    }

    @SwingAction
    public void errorPlot() {
        setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.DELTA_PRESENTATION]);
        setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
        actionController.handleErrorPlot();
    }

    @SwingAction
    public void contrastPlot() {
        setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.CONTRAST_PRESENTATION]);
        setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
        actionController.handleContrastPlot();
    }

    @SwingAction
    public void tabularData() {
        setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.SIMPLE_PRESENTATION]);
        setDisplayMode(Session.MODE_OPTIONS[Session.TABLE_MODE]);
        actionController.handleTabularData();
    }

    @SwingAction
    public void tabularErrorData() {
        setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.DELTA_PRESENTATION]);
        setDisplayMode(Session.MODE_OPTIONS[Session.TABLE_MODE]);
        actionController.handleTabularErrorData();
    }

    // user selected Import Sim Dir from Sims/Runs menu
    @SwingAction
    public void importSimDir() {
        actionController.handleImportSimDir();
    }

    // user selected Add Run Dir from Sims/Runs menu
    @SwingAction
    public void addRunDir() {
        actionController.handleAddRunDir();
    }

    @SwingAction
    public void gnuSinglePlot() {
        JOptionPane.showMessageDialog(getMainFrame(), "UNIMPLEMENTED FEATURE !\n" +
            "Gnu plot is not implemented yet.",
            "Error", JOptionPane.WARNING_MESSAGE);
    }

    @SwingAction
    public void gnuComparisonPlot() {
        JOptionPane.showMessageDialog(getMainFrame(), "UNIMPLEMENTED FEATURE !\n" +
            "Gnu plot is not implemented yet.",
            "Error", JOptionPane.WARNING_MESSAGE);
    }

    @SwingAction
    public void gnuErrorPlot() {
        JOptionPane.showMessageDialog(getMainFrame(), "UNIMPLEMENTED FEATURE !\n" +
            "Gnu plot is not implemented yet.",
            "Error", JOptionPane.WARNING_MESSAGE);
    }

    @SwingAction
    public void quickPlot() {
        actionController.launchQP(getSelectedRunPaths());
    }

    @SwingAction
    public void createPDF() {
        if (fileDevice == null) {
            fileDevice = new File(UIUtils.getTrickUserHome());
        }
        PDFBooklet.showDialog(getMainFrame(), "Create PDF", fileDevice);
    }

    @SwingAction
    public void addRuns() {
        actionController.handleAddRuns();

        // Need to modify trick_qp as well when runs are added, so
        // registering the remote call. Every time need to
        // register a new one as the other application could
        // be no longer exist.
        try {
            registerQPRemoteCall();
        } catch (RemoteException re) {

        } catch (MalformedURLException me) {

        } catch (NotBoundException nbe) {

        }

        // if trick_qp is not up, remote call won't be created successfully
        // and then there is no need to update trick_qp.
        if (qpRemoteCall != null) {
            if (runList.getAllData() != null) {
                Object[] allRuns = runList.getAllData();
                int len = allRuns.length;
                if (len > 0) {
                    final String[] dirList = new String[len];
                    for (int i = 0; i < len; i ++) {
                        if (allRuns[i] instanceof SessionRun) {
                            dirList[i] = ((SessionRun)allRuns[i]).getDir();
                        }
                    }
                    try {
                        // parsing the String array instead of an array of SessionRun
                        // so that we don't have to worry about the SessionRun objects serialization.
                        qpRemoteCall.updateRunList(dirList);
                    } catch (RemoteException re) {

                    }
                }
            }
        }
    }

    @SwingAction
    public void readDPList() {
        actionController.handleReadDPList();
    }

    @SwingAction
    public void openSelected() {
        simRunTree.expandSelectedNodes();
    }

    @SwingAction
    public void closeSelected() {
        simRunTree.collapseSelectedNodes();
    }

    @SwingAction
    public void removeSelectedNodes() {
        if (TrickApplication.getPopupInvoker() == DataPanel.SIM_RUN_TREE) {
            simRunTree.removeSelectedData();
        } else if (TrickApplication.getPopupInvoker() == DataPanel.SIM_DP_TREE) {
            simDPTree.removeSelectedData();
        }
    }

    @SwingAction
    public void removeSelectedItems() {
        if (TrickApplication.getPopupInvoker() == DataPanel.RUN_LIST) {
            runList.removeSelectedData();
        } else if (TrickApplication.getPopupInvoker() == DataPanel.DP_LIST) {
            dpList.removeSelectedData();
        }
    }

    @SwingAction
    public void removeAllItems() {
        if (TrickApplication.getPopupInvoker() == DataPanel.RUN_LIST) {
            runList.removeAllData();
        } else if (TrickApplication.getPopupInvoker() == DataPanel.DP_LIST) {
            dpList.removeAllData();
        }
    }

    @SwingAction
    public void addDPs() {
        dpList.addData(simDPTree.getSelectedItems().toArray());
    }

    // Add DP from Data Product menu
    @SwingAction
    public void addDP() {
        actionController.handleAddDP();
    }

    // Edit DP from Data Product menu
    @SwingAction
    public void editSelectedDP() {
        int selectedLen = dpList.getSelectedData().length;
        if (selectedLen < 1) {
            JOptionPane.showMessageDialog(getMainFrame(),
                                          "No DP_ file is selected from DP Selections!",
                                          "Error",
                                          JOptionPane.WARNING_MESSAGE);
        } else if (selectedLen > 1) {
            JOptionPane.showMessageDialog(getMainFrame(),
                                          "More than one DP_ file selected in DP Selections. Only one may be edited at a time!",
                                          "Error",
                                          JOptionPane.WARNING_MESSAGE);
        } else {
            actionController.handleEditDP(dpList.getSelectedFirstData().toString());
        }
    }

    // Edit DP... from the popup menu after right-clicking on a DP_ file
    @SwingAction
    public void editRightClickedDP() {
        actionController.handleEditDP(rightClickedDP);
    }


    // Filter from Data Product menu
    @SwingAction
    public void filterDP() {
        actionController.handleFilterDP();
    }

    @SwingAction
    public void runSim() {
        String simExe = "./S_main_" + UIUtils.getTrickHostCPU() + ".exe";
        for (String eachItem : simRunTree.getSelectedItems()) {
            File eachItemFile = new File(eachItem);
            File simFilePath = eachItemFile.getParentFile();
            // find out the SIM_ directory for the RUN
            while (!simFilePath.getName().startsWith("SIM_")) {
                simFilePath = simFilePath.getParentFile();
            }
            String simExeArg = eachItem+java.io.File.separator+"input.py";
            ProcessBuilder pb = new ProcessBuilder(simExe, simExeArg);
            pb.directory(simFilePath);
            printStatusMessage("cd " + simFilePath.getPath() + "\n");
            printStatusMessage(simExe + " " + simExeArg + "\n");
            Process process = null;
            try {
                process = pb.start();
            } catch (IOException e) {
                printStatusMessage(e.getMessage() + "\n");
            }
            if (process != null) {
                captureProcessMessage(process);
            }
        }
    }

    @SwingAction
    public void refreshSelected() {
         actionController.handleRefreshSelected();
    }

    @SwingAction
    public void plotDestination() {

    }

    @SwingAction
    public void selectFileDevice() {
        fileDevice = UIUtils.chooseSaveFile(UIUtils.getTrickUserHome(), "dp_out", null, getMainFrame());
        if (fileDevice != null) {
            plotDevice = Session.DEVICE_OPTIONS[Session.FILE_DEVICE];
        } else {
            // if no file is selected such as the Cancel button is clicked,
            // set the device to what it was before
            setDevice(plotDevice);
        }
    }

    //========================================
    //    Set/Get methods
    //========================================


    //========================================
    //    Methods
    //========================================
    /**
     * Main method for this application.
     * @param args command line arguments
     */
    public static void main(String[] args) {
        BaseApplication.launch(TrickDPApplication.class, args);
    }

    /**
     * Helper method for registering the remote call to call functions in trick_qp.
     */
    private void registerQPRemoteCall() throws RemoteException, MalformedURLException, NotBoundException {
        // TODO: may need to change to register by hostname and port number.
        // Here "localhost" is hard-coded and the default port 1099 is implied.
        Registry registry = LocateRegistry.getRegistry();
        qpRemoteCall = (QPRemoteCallInterface) registry.lookup("server.QPRemoteCallInterface");
    }

    /**
     * Starts things after everything else is ready.
     * This is called after startup.
     *
     * @see #initialize
     * @see #startup
     */
    @Override
    protected void ready() {
        super.ready();
        // TODO: may need to support different port instead of the default one
        // Create the registry in case any other application needs to
        // make the remote call to this application. Be sure to create
        // a different registry from trick_qp.
        try {
            LocateRegistry.createRegistry(1099);
        } catch (RemoteException re) {
            // registry already exists, do nothing
        }

        try {
            dpRemoteCall = new DPRemoteCallInterfaceImpl();
            Naming.rebind("server.DPRemoteCallInterface", dpRemoteCall);
        } catch (RemoteException re) {

        } catch (MalformedURLException me) {

        }

    }

    /**
     * Makes initialization as needed. This is called before startup().
     *
     * @see #startup
     */
    @Override
    protected void initialize(String[] args) {
        super.initialize(args);
        initProperties();
    }

    // Save all SIM directories for next time before shutting down
    @Override
    public void shutdown() {
        Vector<DefaultMutableTreeNode> rootChildren = simRunTree.getAllChildren(simRunTree.getRoot());
        if (rootChildren != null) {
            String dirs = "";
            for (DefaultMutableTreeNode eachChild: rootChildren) {
                if (!dirs.isEmpty()) {
                    dirs += ",";
                }
                dirs += eachChild.getUserObject().toString();
                trickProperties.setProperty("TRICK_DP_SIM_DIRS", dirs);
            }
        }
        super.shutdown();
    }

    /**
     * Creates an action controller for all actions.
     * This gets called after all gui is ready.
     *
     * Required by {@link DataProductsApplication}.
     */
    @Override
    protected void createActionController() {
        actionController = new TrickDPActionController();
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
    protected JComponent createLeftTop() {
        TrickFileFilter simRunFileFilter = new TrickFileFilter(TrickFileFilter.SIM_RUN);

        String simDirList = trickProperties.getProperty("TRICK_DP_SIM_DIRS");

        if (simDirList != null) {
            String[] simDirs = simDirList.split(",");
            /* FileTree will not add/display a folder icon if the directory
             * resides under an existing folder (e.g. $HOME, $HOME/trick_sims).
             * Reversing the loading order can bypass this filtering which
             * causes problems if sub-directory names (e.g. "trick_sims") do
             * not begin with "SIM" or "RUN"; our SIM/RUN filter will not allow
             * the user to open "trick_sims" from the $HOME folder because it
             * is hidden/filtered.
             * Fixed in FileTree.
             */
            //Arrays.sort( simdirs, Collections.reverseOrder() );

            for (int i = 0; i < simDirs.length; i ++) {
                File dir = new File(simDirs[i]);
                if (i == 0) {
                    simRunTree = new SimRunTree(dir, simRunFileFilter, 4);
                } else {
                    simRunTree.importDir(dir);
                }
            }

        } else {
            // after app properties initialization, simDirList should never be null
            // leave else here just in case!
            File dir = new File(UIUtils.getTrickUserHome());
            simRunTree = new SimRunTree(dir, simRunFileFilter, 4);
        }
        simRunTree.setPreferredSize(new Dimension(325, 375));

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        simRunTree.setBorder(BorderFactory.createTitledBorder(loweredetched, "Sims/Runs Tree"));

        simRunTree.setType(DataPanel.SIM_RUN_TREE);

        String[] popupMenuActions = {"refreshSelected", "openSelected", "closeSelected", "removeSelectedNodes"};
        simRunTree.setPopup(createPopupMenu(popupMenuActions), 0);
        popupMenuActions = new String[] { "addRuns", "readDPList", "refreshSelected", "openSelected", "closeSelected", "removeSelectedNodes" };
        simRunTree.setPopup(createPopupMenu(popupMenuActions), 1);
        popupMenuActions = new String[] {"addRuns"};
        simRunTree.setPopup(createPopupMenu(popupMenuActions), 2);
        popupMenuActions = new String[] {"addRuns", "quickPlot", "runSim"};
        simRunTree.setPopup(createPopupMenu(popupMenuActions), 3);

        expandFirstNodeOfTree(simRunTree);

        return simRunTree;
    }

    /**
     * Helper method for expanding the node at frist row of a tree.
     */
    private void expandFirstNodeOfTree(FileTreePanel treePanel) {
        treePanel.getTree().expandRow(0);
        treePanel.scrollToTreeTop();
    }


    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
    protected JComponent createLeftMiddle() {
        runList = new ListPanel();
        runList.setType(DataPanel.RUN_LIST);
        simRunTree.addObserver(runList);
        runList.addListMouseListener(new LocalMouseListener());
        runList.setPreferredSize(new Dimension(325, 225));

        runList.getJList().setTransferHandler(new SessionRunTransferHandler(runList.getJList()));
        runList.getJList().setDragEnabled(true);
        runList.getJList().setDropMode(DropMode.INSERT);

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        runList.setBorder(BorderFactory.createTitledBorder(loweredetched, "Run Selections"));
        runList.getJList().setBackground(Color.white);

        String[] popupMenuActions = { "removeSelectedItems", "removeAllItems", "quickPlot", "configureRunTimename" };
        runList.setPopup(createPopupMenu(popupMenuActions), 0);

        return runList;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
    protected JComponent createRightTop() {
        File dir = new File(UIUtils.getTrickUserHome());
        TrickFileFilter simDPFileFilter = new TrickFileFilter(TrickFileFilter.SIM_DP);
        simDPTree = new SimDPTree(dir, simDPFileFilter, 4);
        simDPTree.getTree().addMouseListener(new LocalMouseListener());
        simDPTree.setType(DataPanel.SIM_DP_TREE);
        simDPTree.setFinalPath("DP_Product");
        simDPTree.setPreferredSize(new Dimension(325, 375));

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        simDPTree.setBorder(BorderFactory.createTitledBorder(loweredetched, "DP Tree"));

        String[] popupMenuActions = { "addDPs", "refreshSelected", "removeSelectedNodes" };
        simDPTree.setPopup(createPopupMenu(popupMenuActions), 0);
        simDPTree.setPopup(createPopupMenu(popupMenuActions), 1);
        popupMenuActions= new String[] { "addDPs", "refreshSelected"};
        simDPTree.setPopup(createPopupMenu(popupMenuActions), 2);
        popupMenuActions = new String[] {"addDPs", "editRightClickedDP"};
        simDPTree.setPopup(createPopupMenu(popupMenuActions), 3);

        simRunTree.addObserver(simDPTree);

        return simDPTree;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
    protected JComponent createRightMiddle() {
        dpList = new ListPanel();
        dpList.setType(DataPanel.DP_LIST);
        simDPTree.addObserver(dpList);
        dpList.addListMouseListener(new LocalMouseListener());
        dpList.setPreferredSize(new Dimension(325, 225));

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        dpList.setBorder(BorderFactory.createTitledBorder(loweredetched, "DP Selections"));
        dpList.getJList().setBackground(Color.white);

        String[] popupMenuActions = { "editRightClickedDP", "removeSelectedItems", "removeAllItems" };
        dpList.setPopup(createPopupMenu(popupMenuActions), 0);

        return dpList;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
    protected JComponent createBottom() {
        JComponent Bottom = getCommonBottom();
        Bottom.setPreferredSize(new Dimension(800, 150));

        return Bottom;
    }

    /**
     * Creates the menu bar for the application.
     *
     * @return a {@link JMenuBar} for the application.
     */
    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = new JMenuBar();
        String[] sessionMenuActionNames = {
            "newSession",
            "openSession",
            //"convertSession",
            "saveSession",
            "refreshSession",
            //"propertiesSession",
            "---",
            "lookAndFeel",
            "---",
            "quit"
        };
        String[] simsRunsMenuActionNames = {
            "importSimDir",
            "addRunDir"
        };
        String[] dataProductMenuActionNames = {
                "addDP",
                "editSelectedDP",
                "filterDP"
        };

        String[] actionsMenuActionNames = {
                "singlePlot",
                "comparisonPlot",
                "errorPlot",
                "contrastPlot",
                "tabularData",
                "tabularErrorData",
                "---",
                "gnuSinglePlot",
                "gnuComparisonPlot",
                "gnuErrorPlot",
                "---",
                "quickPlot",
                "---",
                "createPDF"
        };
        String[] helpMenuActionNames = {
                "helpContents",
                "---",
                "showAboutBox"
        };

        JMenu sessionMenu = createMenu("sessionMenu", sessionMenuActionNames);
        sessionMenu.insert(confirmExitSelection, sessionMenuActionNames.length-1);
        menuBar.add(sessionMenu);

        menuBar.add(createMenu("simsRunsMenu",simsRunsMenuActionNames));
        menuBar.add(createMenu("dataProductMenu",dataProductMenuActionNames));

        menuBar.add(createSettingsMenu());

        menuBar.add(createMenu("actionsMenu",actionsMenuActionNames));

        menuBar.add(createHelpMenu("helpMenu", helpMenuActionNames, "Help.hs"));

        return menuBar;
    }

    /**
     * Creates the tool bar for the application.
     *
     * @return a {@link JToolBar} for the application.
     */
    @Override
    protected JToolBar createToolBar() {
        String[] toolbarActionNames = {
                "newSession",
                "openSession",
                "saveSession",
                "---",
                "singlePlot",
                "comparisonPlot",
                "errorPlot",
                "contrastPlot",
                "tabularData",
                "tabularErrorData",
                "---",
                "quickPlot",
                "---",
                "createPDF",
                "gnuSinglePlot",
                "gnuComparisonPlot",
                "gnuErrorPlot",
                "---",
                "toggleGnuplot"
            };
        JToolBar toolBar = new JToolBar();
        toolBar.setFloatable(false);
        for (String actionName : toolbarActionNames) {
            if (actionName.equals("---")) {
                toolBar.addSeparator();
            } else if (actionName.equals("toggleGnuplot")) {
                toolBar.add(gnuplotButton);
            } else {
                toolBar.add(createButton(actionName, false));
            }
        }
        toolBar.add(Box.createHorizontalGlue());
        toolBar.add(UIUtils.getSmallTrickIconLabel());
        return toolBar;
    }

    /**
     * Helper method for creating Settings menu.
     */
    private JMenu createSettingsMenu() {
        JMenu settingsMenu = new JMenu();
        settingsMenu.setName("settingsMenu");

        settingsMenu.add(new JLabel("Device"));

        deviceMenuItems = addRadioButtonMenuItems(settingsMenu, new String[]{"selectTerminalDevice", "selectPrinterDevice", "selectFileDevice"});
        settingsMenu.addSeparator();

        settingsMenu.add(new JLabel("Plot Utility"));
        if ( fermiExists ) {
            addRadioButtonMenuItems(settingsMenu, new JRadioButtonMenuItem[]{fermiRadioButton, javaRadioButton, gnuplotRadioButton});
        } else {
            addRadioButtonMenuItems(settingsMenu, new JRadioButtonMenuItem[]{javaRadioButton, gnuplotRadioButton});
        }

        settingsMenu.addSeparator();

        JMenu gnuplotTerminalMenu = new JMenu(getAction("selectGnuplotTerminal"));
        getAction("selectGnuplotTerminal").setEnabled(false);

        settingsMenu.add(gnuplotTerminalMenu);
        gnuplotTerminalMenuItems = addRadioButtonMenuItems(gnuplotTerminalMenu, new String[]{"selectX11", "selectPSColor", "selectPSBW", "selectPNG", "selectEPS", "selectAQUA"});
        return settingsMenu;
    }

    /**
     * Helper method for initializing properties as necessary.
     */
    private void initProperties() {

        setTrickDPSimDirsProperty();

        setTrickDPImportDirProperty();
    }

    /**
     * Helper method to set TRICK_DP_IMPORT_DIR property.
     */
    private void setTrickDPImportDirProperty() {
        String defaultImportSimDir = trickProperties.getProperty("TRICK_DP_IMPORT_DIR");
        // if the TRICK_DP_IMPORT_DIR doesn't exist, set TRICK_USER_HOME to the property
        if (defaultImportSimDir == null) {
            defaultImportSimDir = UIUtils.getTrickUserHome();
        }

        // if TRICK_USER_HOME doesn't exist, set current dir to the property
        if (defaultImportSimDir == null || !((new File(defaultImportSimDir)).exists())) {
            defaultImportSimDir = currentDir;
        }

        trickProperties.setProperty("TRICK_DP_IMPORT_DIR", defaultImportSimDir);
    }

    /**
     * Helper method to set TRICK_DP_SIM_DIRS property.
     *
     * The property string contains:
     * 1. current directory if it is in a SIM dir or it has SIM dirs
     * 2. TrickDPApplication.properties
     * 3. TRICK_USER_HOME if necessary
     */
    private void setTrickDPSimDirsProperty() {
        String simDirs = addCurrentDir();

        /*if (simDirs == null) {
            simDirs = UIUtils.getTrickUserHome();
            if (simDirs == null) {
                simDirs = "";
            }
        }*/

        simDirs = appendDirsFromPropertyFile(simDirs);

        // add TRICK_USER_HOME if it's not there
        if (!simDirs.contains(UIUtils.getTrickUserHome())) {
            if (simDirs != null && !simDirs.trim().isEmpty()) {
                simDirs = simDirs.concat("," + UIUtils.getTrickUserHome());
            } else {
                simDirs = UIUtils.getTrickUserHome();
            }
        }

        // the dir that is shown on the top and is always expanded when the gui is initially up.
        // this dir is either the current dir if it is in a SIM or has a SIM dir
        // or the TRICK_USER_HOME.
        trickProperties.setProperty("TRICK_DP_SIM_DIRS", simDirs);
    }


    /**
     * Helper method to initially set current dir to the dir string.
     *
     */
    private String addCurrentDir() {
        String simDirs = "";

        // if current dir is in a SIM dir, import its parent
        if ( currentDir.contains("/SIM") ) {
            try {
                simDirs = (new File("..")).getCanonicalPath();
            } catch (java.io.IOException ioe) {
                // shouldn't get here
                simDirs = currentDir.replaceAll("/SIM+\\S*", "");
            }
        } else {
            // Filter a list of returned files that are SIM directories
            // TODO: use TrickFileFilter
            FilenameFilter simFilter = new FilenameFilter() {
                public boolean accept(File path, String filename) {
                    File myFullPath = new File(path + java.io.File.separator + filename);
                    if ( myFullPath.isDirectory() && filename.contains("SIM") ) {
                        return true;
                    } else {
                        return false;
                    }
                }
            };
            String[] simsList = new File(currentDir).list(simFilter);
            if ( simsList != null && simsList.length > 0 ) {
                simDirs = currentDir;
            }
        }
        return simDirs;
    }


    /**
     * Helper method to append those dirs specified as TRICK_DP_SIM_DIRS property
     * in related .properties file without duplication to the specified dir string
     * separated by comma.
     *
     */
    private String appendDirsFromPropertyFile(String simDirs) {
        // prevent the duplicate ones
        File myDpPropFile = new File(propDirectory + java.io.File.separator + applicationName + ".properties");
        if ( myDpPropFile.exists() ) {
            String dpSimDirsProperty = trickProperties.getProperty("TRICK_DP_SIM_DIRS");
            // if the property doesn't exist, return the original string
            if (dpSimDirsProperty == null) {
                return simDirs;
            }
            String[] dpSimDirs = dpSimDirsProperty.split(",");

            // if the property doesn't have any value for some reason,
            // return the original string
            if (dpSimDirs == null || dpSimDirs.length < 1) {
                return simDirs;
            }

            Arrays.sort(dpSimDirs);

            for (String eachDir : dpSimDirs) {
                if (eachDir == null || eachDir.isEmpty()) {
                    continue;
                }
                boolean t1 = new File(eachDir).exists();
                boolean t2 = false;
                for (String myDir : simDirs.split(",")) {
                    if ( myDir.equals(eachDir) ) {
                        t2 = true;  //found duplicate directories
                        break;
                    }
                }
                // Keep the first one as the first one as it will be expanded automatically at beginning
                if ( t1 == true  &&  t2 == false ){
                    if (!simDirs.isEmpty()) {
                        simDirs = simDirs.concat("," + eachDir);
                    } else {
                        simDirs = eachDir;
                    }
                }
            }
        }
        return simDirs;
    }


    /**
     * Helper method for getting selected run paths.
     * @return list of selected run paths
     */
    public String[] getSelectedRunPaths() {
        String[] runPaths = null;
        if (runList.getAllData() != null && runList.getAllData().length>0) {
            Object[] runDirs = runList.getAllData();
            runPaths = new String[runDirs.length];
            for (int i = 0; i < runDirs.length; i++) {
                runPaths[i] = runDirs[i].toString();
            }
        }
        return runPaths;
    }

    /**
     * Sets the device for plotting.
     *
     * @param dc    The name of the device.
     */
    public void setDevice(String dc) {
        if (dc.equalsIgnoreCase(Session.DEVICE_OPTIONS[Session.PRINTER_DEVICE])) {
            deviceMenuItems[Session.PRINTER_DEVICE].doClick();
        } else if (dc.equalsIgnoreCase(Session.DEVICE_OPTIONS[Session.FILE_DEVICE])) {
            deviceMenuItems[Session.FILE_DEVICE].doClick();
        } else {
            deviceMenuItems[Session.TERMINAL_DEVICE].doClick(); // default
        }
    }

    /**
     * Sets the Gnuplot terminal.
     *
     * @param gt    The name of the Gnuplot terminal.
     */
    public void setGnuplotTerminal(String gt) {
        if (gt.equalsIgnoreCase(Session.GNUPLOT_TERMINAL_OPTIONS[Session.PS_COLOR_GNUPLOT_TERMINAL])) {
            gnuplotTerminalMenuItems[Session.PS_COLOR_GNUPLOT_TERMINAL].doClick();
        } else if (gt.equalsIgnoreCase(Session.GNUPLOT_TERMINAL_OPTIONS[Session.PS_BW_GNUPLOT_TERMINAL])) {
            gnuplotTerminalMenuItems[Session.PS_BW_GNUPLOT_TERMINAL].doClick();
        } else if (gt.equalsIgnoreCase(Session.GNUPLOT_TERMINAL_OPTIONS[Session.PNG_GNUPLOT_TERMINAL])) {
            gnuplotTerminalMenuItems[Session.PNG_GNUPLOT_TERMINAL].doClick();
        } else if (gt.equalsIgnoreCase(Session.GNUPLOT_TERMINAL_OPTIONS[Session.EPS_GNUPLOT_TERMINAL])) {
            gnuplotTerminalMenuItems[Session.EPS_GNUPLOT_TERMINAL].doClick();
        } else if (gt.equalsIgnoreCase(Session.GNUPLOT_TERMINAL_OPTIONS[Session.AQUA_GNUPLOT_TERMINAL])) {
            gnuplotTerminalMenuItems[Session.AQUA_GNUPLOT_TERMINAL].doClick();
        } else {
            gnuplotTerminalMenuItems[Session.X11_GNUPLOT_TERMINAL].doClick(); // default
        }
    }


    //========================================
    //    Inner classes
    //========================================
    /**
     * Localized mouse call handling.
     */
    private class LocalMouseListener extends MouseAdapter {

        //========================================
        //    MouseListener method(s)
        //========================================
        /**
         * Invoked when the mouse button has been clicked (pressed
         * and released) on a component.
         *
         * @param e MouseEvent sent from system.
         */
        @Override
        public void mouseClicked(MouseEvent e) {
            if (UIUtils.isRightMouseClick(e)) {
                if (e.getSource() == runList.getJList()) {
                    if (runList.getSelectedFirstData() != null &&
                        runList.getSelectedFirstData() instanceof SessionRun) {
                        runToConfigure = (SessionRun)runList.getSelectedFirstData();
                    }
                } else if (e.getSource() == dpList.getJList()) {
                    int index = dpList.getJList().locationToIndex(e.getPoint());
                    if (index > -1) {
                        Object clickedObj = dpList.getJList().getModel().getElementAt(index);
                        rightClickedDP = clickedObj.toString();
                    } else {
                        rightClickedDP = null;
                    }
                } else if (e.getSource() == simDPTree.getTree()) {
                    if (simDPTree.getRightClickedTreeUserObj() != null) {
                        rightClickedDP = simDPTree.getRightClickedTreeUserObj().getFile().getAbsolutePath();
                    } else {
                        rightClickedDP = null;
                    }
                }
                // common behaviors are handled in ListPanel
            } else if (UIUtils.isDoubleClick(e)) {
                if (e.getSource() == dpList.getJList()) {
                     dpList.removeSelectedData();
                } else if (e.getSource() == runList.getJList()) {
                    runList.removeSelectedData();
                }
            }
        }
    }

}
