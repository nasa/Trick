
//========================================
//    Package
//========================================
package trick.dataproducts.trickqp;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.File;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.DropMode;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.tree.DefaultMutableTreeNode;

import org.jdesktop.application.Application;

import trick.common.framework.BaseApplication;
import trick.common.ui.UIUtils;
import trick.common.ui.components.CommonTreeNode;
import trick.common.ui.panels.DataPanel;
import trick.common.ui.panels.ListPanel;
import trick.common.utils.LogVar;
import trick.common.utils.LogVar.DisplayType;
import trick.common.utils.SwingAction;
import trick.common.utils.XMLCreator;
import trick.dataproducts.DataProductsApplication;
import trick.dataproducts.trickdp.utils.DPRemoteCallInterface;
import trick.dataproducts.trickqp.utils.DataTransferHandler;
import trick.dataproducts.trickqp.utils.Product;
import trick.dataproducts.trickqp.utils.ProductDataPanel;
import trick.dataproducts.trickqp.utils.ProductExternalFunction;
import trick.dataproducts.trickqp.utils.ProductPage;
import trick.dataproducts.trickqp.utils.ProductTable;
import trick.dataproducts.trickqp.utils.ProductTree;
import trick.dataproducts.trickqp.utils.ProductXMLCreator;
import trick.dataproducts.trickqp.utils.QPRemoteCallInterface;
import trick.dataproducts.trickqp.utils.QPRemoteCallInterfaceImpl;
import trick.dataproducts.trickqp.utils.TrickQPActionController;
import trick.dataproducts.trickqp.utils.VarListPanel;
import trick.dataproducts.utils.Session;
import trick.dataproducts.utils.SessionRun;
import trick.dataproducts.utils.SessionRunTransferHandler;


/**
 * Trick QP Application that extends {@link DataProductsApplication}.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class TrickQPApplication extends DataProductsApplication {

    //========================================
    //    Public data
    //========================================
    // Made all these GUI components public so that the action controller can access them.
    public ProductTree productTree;
    public JTabbedPane propertyPane;
    public VarListPanel varList;
    public ListPanel runList;

    public DefaultMutableTreeNode plotsNode;
    public DefaultMutableTreeNode tablesNode;
    public DefaultMutableTreeNode programsNode;
    
    public JTextField searchField;
    
    public static enum QPTabType {
    	QP_PAGE_TAB("Page"), 
    	QP_PLOT_TAB("Plot"), 
    	QP_X_VAR_TAB("X Var"),
    	QP_Y_VAR_TAB("Y Var"),
    	QP_TABLE_TAB("Table"),
    	QP_COLUMN_TAB("Column"),
    	QP_TABLE_VAR_TAB("Table Var"),
    	QP_PROGRAM_TAB("Program"),
    	QP_IN_TAB("In"),
    	QP_OUT_TAB("Out"); 
        
    	private String title;
    	private QPTabType(String title) {
    		this.title = title;
    	}
    	
		public String getTabTitle() {
			return title;
		}
    }
    
    //========================================
    //    Protected data
    //========================================


    //========================================
    //    Private Data
    //========================================
    private TrickQPActionController actionController = null;

    private ArrayList<ProductDataPanel> dataPanels;

    private static String[] initialArgs;
    
    // to let other application to call this app (trick_qp)
    private QPRemoteCallInterface qpRemoteCall;
    
    // to call functions in trick_dp
    private DPRemoteCallInterface dpRemoteCall;
    
    private JPopupMenu functionPopup;
    
    private JButton functionButton;
    
    //========================================
    //    Constructors
    //========================================


    //========================================
    //    Actions
    //========================================

    @SwingAction
    public void singlePlot() {
    	setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
    	setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.SIMPLE_PRESENTATION]);
        if (saveDPToFile(TEMP_DP_FILE)) {
        	launchPlot();
        }
    }

    @SwingAction
    public void comparisonPlot() {
    	setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
    	setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.COMPARISON_PRESENTATION]);
        if (saveDPToFile(TEMP_DP_FILE)) {
        	launchPlot();
        }
    }

    @SwingAction
    public void errorPlot() {
    	setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
    	setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.DELTA_PRESENTATION]);
        if (saveDPToFile(TEMP_DP_FILE)) {
        	launchPlot();
        }
    }

    @SwingAction
    public void contrastPlot() {
    	setDisplayMode(Session.MODE_OPTIONS[Session.PLOT_MODE]);
    	setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.CONTRAST_PRESENTATION]);
        if (saveDPToFile(TEMP_DP_FILE)) {
        	launchPlot();
        }
    }

    @SwingAction
    public void tabularData() {
    	setDisplayMode(Session.MODE_OPTIONS[Session.TABLE_MODE]);
    	setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.SIMPLE_PRESENTATION]);
        if (saveDPToFile(TEMP_DP_FILE)) {
        	launchPlot();
        }
    }

    @SwingAction
    public void tabularErrorData() {
    	setDisplayMode(Session.MODE_OPTIONS[Session.TABLE_MODE]);
    	setPreferredPresentation(Session.PRESENTATION_OPTIONS[Session.DELTA_PRESENTATION]);
        if (saveDPToFile(TEMP_DP_FILE)) {
        	launchPlot();
        }
    }

    /**
     * Helper method for setting up the session data and calling the plot program launching.
     */
    private void launchPlot() {
        Session session = new Session(versionField.getText(), getPreferredPresentation());
        session.setMode(getDisplayMode());
        saveSessionToFile(session, null, runList.getAllData(), new String[]{TEMP_DP_FILE});
        launchPlotProgram(sessionFile);
    }

    @SwingAction
    public void newDP() {
        actionController.handleNewDP();
    }

    @SwingAction
    public void openDP() {
        actionController.handleOpenDP();
    }

    //@SwingAction
    //public void convertDP() {
        //TODO
    //}

    @SwingAction
    public void refreshDP() {
    	actionController.handleRefreshDP();
    }

    // Return true if file is actually saved, false otherwise such as cancel selection.
    @SwingAction
    public boolean saveDP() {
        return actionController.handleSaveDP();
    }

    // Return true if file is actually saved, false otherwise such as cancel selection.
    @SwingAction
    public boolean saveAsDP() {
        return actionController.handleSaveAsDP();
    }

    //@SwingAction
    //public void propertiesDP() {

    //}
    
    @SwingAction
    public void searchVar() {
    	actionController.handleSearchVar();
    }
    
    @SwingAction
    public void addVar() {
        actionController.handleAddVar();
    }


    @SwingAction
    public void expandVar() {
        actionController.expandVar();
    }

    @SwingAction
    public void contractVar() {
        actionController.contractVar();
    }

    @SwingAction
    public void changeUnits() {
        actionController.changeUnits();
    }

    @SwingAction
    public void addRun() {
        actionController.handleAddRun();
    }

    @SwingAction
    public void removeRun() {
        actionController.handleRemoveRun();
    }

    @SwingAction
    public void newPage() {
        actionController.handleNewPage();
    }

    @SwingAction
    public void removeAllPages() {
        plotsNode.removeAllChildren();
        productTree.getDefaultModel().nodeStructureChanged(plotsNode);
    }

    @SwingAction
    public void newPlot() {
        actionController.handleNewPlot();
    }

    @SwingAction
    public void newCurve() {
        actionController.handleNewCurve();
    }

    @SwingAction
    public void newVarcase() {
        actionController.handleNewVarcase();
    }

    @SwingAction
    public void removeSelectedNode() {
        if (productTree != null) {
            productTree.removeClickedNode();
        }
    }

    @SwingAction
    public void newTable() {
        actionController.handleNewTable();
    }

    @SwingAction
    public void removeTable() {
        tablesNode.removeAllChildren();
        productTree.getDefaultModel().nodeStructureChanged(tablesNode);
    }

    @SwingAction
    public void newColumn() {
        actionController.handleNewColumn();
    }
    
    @SwingAction
    public void newProgram() {
    	if (programsNode.getChildCount() >= 1) {
    		JOptionPane.showMessageDialog(getMainFrame(), 
                                          "A Program already exists. Multiple programs not supported !\n",
                                          "Warning", 
                                          JOptionPane.WARNING_MESSAGE);
    	} else {
    		actionController.handleNewProgram();
    	}
    }
    
    @SwingAction
    public void removeAllPrograms() {
    	programsNode.removeAllChildren();
        productTree.getDefaultModel().nodeStructureChanged(programsNode);
    }
    
    @SwingAction
    public void newProgramOutput() {
    	if (programsNode.getChildCount() < 1) {
    		JOptionPane.showMessageDialog(getMainFrame(), 
                                          "A Program must exist in DP to add an output for it !\n",
                                          "Warning", 
                                          JOptionPane.WARNING_MESSAGE);
    	} else {
    	    String response = JOptionPane.showInputDialog(getMainFrame(), 
    			                                          "Enter Name for Output:",
    			                                          "Add Output",
    			                                          JOptionPane.QUESTION_MESSAGE);
    	    if (response != null && !response.trim().isEmpty()) {    	    	
    	    	actionController.handleNewProgramOutput(response);
    	    }
    	}
    }
    
    @SwingAction
    public void selectFunction() {
    	// handled in LocalListMouseListener#MouseClicked
    	// this empty action method is for gui display
    }
    
    @SwingAction
    public void subtractFunction() {
    	if (varList == null || varList.getSelectedData() == null || varList.getSelectedData().length != 2) {
    		JOptionPane.showMessageDialog(getMainFrame(), 
    			                          "Exactly two vars must be selected for subtraction !\n",
                                          "Error", 
                                          JOptionPane.ERROR_MESSAGE);
    	} else {
    		actionController.handleSubtractFunction((LogVar)varList.getSelectedData()[0], (LogVar)varList.getSelectedData()[1]);
    	}
    }
    
    @SwingAction
    public void reverseSubtractFunction() {
    	if (varList == null || varList.getSelectedData() == null || varList.getSelectedData().length != 2) {
    		JOptionPane.showMessageDialog(getMainFrame(), 
    			                          "Exactly two vars must be selected for subtraction !\n",
                                          "Error", 
                                          JOptionPane.ERROR_MESSAGE);
    	} else {
    		actionController.handleSubtractFunction((LogVar)varList.getSelectedData()[1], (LogVar)varList.getSelectedData()[0]);
    	}
    }
    
    @SwingAction
    public void cancelFunction() {
    	// do nothing as if it is selected, the related popup will be closed and this is what is desired.
    }

    @Override
	@SwingAction
    public void quit(ActionEvent e) {
    	if ((productTree.getAllChildren(plotsNode) != null && productTree.getAllChildren(plotsNode).size() > 0) ||
            (productTree.getAllChildren(tablesNode) != null && productTree.getAllChildren(tablesNode).size() > 0)) {
    		if (productTree.isChanged()) {
    		    int choice = JOptionPane.showConfirmDialog(getMainFrame(), 
					                                       "Save changes to a DP_ file?", 
                                                           "Save DP_ File?", 
                                                           JOptionPane.YES_NO_CANCEL_OPTION);
    		    if (choice == JOptionPane.CANCEL_OPTION) {
    			    return;
    		    } else if (choice == JOptionPane.YES_OPTION) {
    			    if (!saveDP()) {
    				    return;
    			    }
    		    }
    		}
    	}
    	
    	super.quit(e);
    }
    
    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Sets preferred presentation.
     */
    @Override
	public void setPreferredPresentation(String pt) {
    	super.setPreferredPresentation(pt);
    	
        // Every time need to register a new one remote class as the other application
        // could be no longer exist.
        try {
            registerDPRemoteCall();
        } catch (RemoteException re) {
                        
        } catch (MalformedURLException me) {
                        
        } catch (NotBoundException nbe) {
                        
        }
        if (dpRemoteCall != null) {
        	try {
        		dpRemoteCall.setPresentation(pt);
        	} catch (RemoteException re) {
        		// do nothing
        	}
        }
    }
    
    /**
     * Sets preferred display mode.
     */
    @Override
	public void setDisplayMode(String md) {
    	super.setDisplayMode(md);
    	
    	// Every time need to register a new one remote class as the other application
        // could be no longer exist.
        try {
            registerDPRemoteCall();
        } catch (RemoteException re) {
                        
        } catch (MalformedURLException me) {
                        
        } catch (NotBoundException nbe) {
                        
        }
        if (dpRemoteCall != null) {
            try {
                dpRemoteCall.setMode(md);
            } catch (RemoteException re) {     
            	// do nothing
            }
        }
    }
    
    /**
     * Gets the tab type for a specified node type.
     *
     * @param nodeType    The tree node type which we need to get tab type for.
     *
     * @return     A {@link QPTabType}, null if no tab is corresponded to this type of node.
     *
     */
    public static QPTabType getTabTypeForNode(int nodeType) {
    	QPTabType tabType = null;
    	switch (nodeType) {
	        case CommonTreeNode.PAGE_NODE:
	        	tabType = QPTabType.QP_PAGE_TAB;
	            break;
	        case CommonTreeNode.PLOT_NODE:
	        	tabType = QPTabType.QP_PLOT_TAB;
	            break;
	        case CommonTreeNode.TABLE_NODE:
	        	tabType = QPTabType.QP_TABLE_TAB;
	            break;
	        case CommonTreeNode.COLUMN_NODE:
	        	tabType = QPTabType.QP_COLUMN_TAB;
	            break;
	        case CommonTreeNode.VAR_NODE:
	        	tabType = QPTabType.QP_TABLE_VAR_TAB;
	            // TODO: for input
	            break;
	        case CommonTreeNode.X_NODE:
	        	tabType = QPTabType.QP_X_VAR_TAB;
	            break;
	        case CommonTreeNode.Y_NODE:
	        	tabType = QPTabType.QP_Y_VAR_TAB;
	            break;
	        //case CommonTreeNode.Z_NODE:
	        //    tabIndex = TrickQPApplication.VAR_TAB;
	        //    break;
	        case CommonTreeNode.PROGRAM_NODE:
	        	tabType = QPTabType.QP_PROGRAM_TAB;
	        	break;
	        case CommonTreeNode.INPUT_VAR_NODE:
	        	tabType = QPTabType.QP_IN_TAB;
	        	break;
	        case CommonTreeNode.OUTPUT_VAR_NODE:
	        	tabType = QPTabType.QP_OUT_TAB;
	        	break;
    	}
        return tabType;
    }
    
    /**
     * Gets the instance of the action controller.
     * @return the action controller
     */
    public TrickQPActionController getActionController() {
        return actionController;
    }

    //========================================
    //    Methods
    //========================================
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
    	// make the remote call to this application.
    	try {
    		LocateRegistry.createRegistry(1099);
    	} catch (RemoteException re) {
    		// registry already exists, do nothing
    	}
    	
    	try {
            qpRemoteCall = new QPRemoteCallInterfaceImpl();
            Naming.rebind("server.QPRemoteCallInterface", qpRemoteCall);
    	} catch (RemoteException re) {
    		
    	} catch (MalformedURLException me) {
    		
    	}
    }
    
    /**
     * Main method for this application.
     * @param args command line arguments
     * @throws RemoteException RemoteException
     * @throws MalformedURLException MalformedURLException
     */
    public static void main(String[] args) throws RemoteException, MalformedURLException {
        BaseApplication.launch(TrickQPApplication.class, args);

        // save the arguments which will be used for launching the application
        if (args != null && args.length > 0) {
            initialArgs = new String[args.length];
            System.arraycopy(args, 0, initialArgs, 0, args.length);
        }
    }

    
    /** 
     * Helper method for registering the remote call to call functions in trick_dp.
     */
    private void registerDPRemoteCall() throws RemoteException, MalformedURLException, NotBoundException {
    	// TODO: may need to change to register by hostname and port number. 
    	// Here "localhost" is hardcoded and the default port 1099 is implied.
    	Registry registry = LocateRegistry.getRegistry();
        dpRemoteCall = (DPRemoteCallInterface) registry.lookup("server.DPRemoteCallInterface");
    }
    
    /**
     * Adds specified runs to the application.
     *
     * @param runs    An array of runs that need added to the list as well as associated variables
     */
    public void addRuns(String[] runs) {
        if (runs != null && runs.length > 0) {
            for (int i = 0; i < runs.length; i++) {
                runList.addData(new SessionRun(runs[i]));
            }
            if (actionController == null) {
                actionController = new TrickQPActionController();
            }
            refreshDP();
        }
    }


    /**
     * Required by {@link DataProductsApplication}.
     * This gets called after all gui is ready.
     */
    @Override
	protected void createActionController() {
        actionController = new TrickQPActionController();
        // initialArgs could have both run and dp file
        if (initialArgs != null && initialArgs.length > 0) {
            // get the run list out of the initialArgs
            ArrayList<String> runArgList = new ArrayList<String>();
            String dpFileName = null;
            for (int i = 0; i < initialArgs.length; i++) {
                // if there are more than one dp files specified, only that last one counts
                if (initialArgs[i].endsWith(".xml")) {
                    dpFileName = initialArgs[i];
                } else {
                    // need the full path of the specified run if available
                    File f = new File(initialArgs[i]);
                    if (f.exists()) {
                        runArgList.add(f.getAbsolutePath());
                    } else {
                        System.out.println("Warning: " + initialArgs[i] + " does not exist!");
                    }
                }
            }

            if (dpFileName != null) {
                actionController.openDP(dpFileName);
            }
            if (runArgList.size() > 0) {
                String[] runArray = new String[runArgList.size()];
                runArgList.toArray(runArray);
                addRuns(runArray);
            }
        }

    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
	protected JComponent createLeftTop() {
        varList = new VarListPanel();
        varList.setType(DataPanel.VAR_LIST);
        varList.addListMouseListener(new LocalListMouseListener());
        varList.setPreferredSize(new Dimension(325, 300));

        varList.getJList().setTransferHandler(new DataTransferHandler());
        varList.getJList().setDragEnabled(true);

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        varList.setBorder(BorderFactory.createTitledBorder(loweredetched, "Vars"));
        varList.getJList().setBackground(Color.white);

        String[] popupMenuActions = { "addVar", "expandVar", "contractVar", "changeUnits" };
        varList.setPopup(createPopupMenu(popupMenuActions), 0);

        return varList;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
	protected JComponent createLeftMiddle() {
        runList = new ListPanel();
        runList.setType(DataPanel.RUN_LIST);
        runList.addListMouseListener(new LocalListMouseListener());
        runList.setPreferredSize(new Dimension(325, 350));

        runList.getJList().setTransferHandler(new SessionRunTransferHandler(runList.getJList()));
        runList.getJList().setDragEnabled(true);
        runList.getJList().setDropMode(DropMode.INSERT);
        
        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        runList.setBorder(BorderFactory.createTitledBorder(loweredetched, "Runs"));
        runList.getJList().setBackground(Color.white);

        String[] popupMenuActions = { "removeRun", "configureRunTimename" };
        runList.setPopup(createPopupMenu(popupMenuActions), 0);

        return runList;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
	protected JComponent createRightTop() {
        productTree = new ProductTree();
        productTree.setPreferredSize(new Dimension(425, 300));

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        productTree.setBorder(BorderFactory.createTitledBorder(loweredetched, "DP Content"));

        plotsNode = new CommonTreeNode("Plots");
        ((CommonTreeNode)plotsNode).setNodeType(CommonTreeNode.PLOTS_NODE);

        tablesNode = new CommonTreeNode("Tables");
        ((CommonTreeNode)tablesNode).setNodeType(CommonTreeNode.TABLES_NODE);

        programsNode = new CommonTreeNode("Programs");
        ((CommonTreeNode)programsNode).setNodeType(CommonTreeNode.PROGRAMS_NODE);

        productTree.addNode(null, plotsNode, true);
        productTree.addNode(null, tablesNode, true);
        productTree.addNode(null, programsNode, true);

        String[] popupMenuActions = new String[] { "newPage", "removeAllPages" };
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.PLOTS_POP);

        popupMenuActions = new String[] { "newPlot", "removeSelectedNode" };
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.PAGE_POP);

        popupMenuActions= new String[] { "newCurve", "removeSelectedNode"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.PLOT_POP);

        popupMenuActions = new String[] { "addVar", "removeSelectedNode", "---", "newVarcase"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.CURVE_POP);

        popupMenuActions = new String[] { "addVar", "removeSelectedNode"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.VARCASE_POP);

        popupMenuActions = new String[] {"removeSelectedNode"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.VAR_POP);

        popupMenuActions = new String[] { "newTable", "removeTable" };
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.TABLES_POP);

        popupMenuActions = new String[] { "addVar", "removeSelectedNode"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.TABLE_POP);

        popupMenuActions = new String[] { "addVar", "removeSelectedNode" };
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.COLUMN_POP);
        
        popupMenuActions = new String[] { "newProgram", "removeAllPrograms"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.PROGRAMS_POP);
        
        popupMenuActions = new String[] { "removeSelectedNode"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.PROGRAM_POP);
        
        popupMenuActions = new String[] { "newProgramOutput"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.OUTPUT_POP);
        
        popupMenuActions = new String[] { "addVar"};
        productTree.setPopup(createPopupMenu(popupMenuActions), ProductTree.INPUT_POP);


        productTree.getTree().setTransferHandler(new DataTransferHandler());
        productTree.getTree().setDragEnabled(true);
        //productTree.getTree().setDropMode(DropMode.ON_OR_INSERT);

        return productTree;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
	protected JComponent createRightMiddle() {
        propertyPane = new JTabbedPane();
        propertyPane.setPreferredSize(new Dimension(425, 350));

        Border loweredetched = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
        propertyPane.setBorder(BorderFactory.createTitledBorder(loweredetched, "Property Notebook"));

        for (int i = 0; i < QPTabType.values().length; i++) {
            ProductDataPanel dataPanel = new ProductDataPanel();
            dataPanel.buildGUIRepresentation(QPTabType.values()[i]);
            if (dataPanels == null) {
                dataPanels = new ArrayList<ProductDataPanel>();
            }
            dataPanels.add(dataPanel);
            JScrollPane scrollPane = new JScrollPane(dataPanel);
            propertyPane.add(QPTabType.values()[i].getTabTitle(), scrollPane);
            propertyPane.setEnabledAt(i, false);
        }

        // at beginning, no tabbed pane is selected
        setSelectedTab(-1);

        return propertyPane;
    }

    /**
     * Required by {@link DataProductsApplication}.
     */
    @Override
	protected JComponent createBottom() {
        JComponent Bottom = getCommonBottom();
        Bottom.setPreferredSize(new Dimension(900, 100));

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

        String[] fileMenuActionNames = {
            "newDP",
            "openDP",
            //"convertDP",
            "refreshDP",
            "saveDP",
            "saveAsDP",
            //"propertiesDP",
            "---",
            "lookAndFeel",
            "---",
            "quit"
        };
        String[] varsMenuActionNames = {
            "addVar",
            "expandVar",
            "contractVar",
            "changeUnits"
        };
        String[] runsMenuActionNames = {
                "addRun",
                "removeRun"
        };
        String[] plotsMenuActionNames = {
                "newPage",
                "removeAllPages",
                "---",
                "newPlot",
                "---",
                "newCurve",
                "---",
                "newVarcase"
        };
        String[] tablesMenuActionNames = {
                "newTable",
                "removeTable",
                "---",
                "newColumn"
        };
        String[] programsMenuActionNames = {
        		"newProgram",
        		"removeAllPrograms",
        		"newProgramOutput"
        };
        String[] actionsMenuActionNames = {
                "singlePlot",
                "comparisonPlot",
                "errorPlot",
                "contrastPlot",
                "tabularData",
                "tabularErrorData"               
        };
        String[] helpMenuActionNames = {
        		"helpContents",
                "---",
                "showAboutBox"
        };
        
        JMenu fileMenu = createMenu("fileMenu", fileMenuActionNames);
        fileMenu.insert(confirmExitSelection, fileMenuActionNames.length-1);
        menuBar.add(fileMenu);        
        menuBar.add(createMenu("varsMenu", varsMenuActionNames));
        menuBar.add(createMenu("runsMenu", runsMenuActionNames));
        menuBar.add(createMenu("plotsMenu", plotsMenuActionNames));
        menuBar.add(createMenu("tablesMenu", tablesMenuActionNames));
        menuBar.add(createMenu("programsMenu", programsMenuActionNames));
        menuBar.add(createSettingsMenu());
        menuBar.add(createMenu("actionsMenu", actionsMenuActionNames));
        menuBar.add(createHelpMenu("helpMenu", helpMenuActionNames, "Help.hs"));
      
        return menuBar;
    }

    /**
     * Helper method for creating Settings menu.
     */
    private JMenu createSettingsMenu() {
        JMenu settingsMenu = new JMenu();
        settingsMenu.setName("settingsMenu");

        settingsMenu.add(new JLabel("Plot Utility"));

        addRadioButtonMenuItems(settingsMenu, new JRadioButtonMenuItem[]{fermiRadioButton, javaRadioButton, gnuplotRadioButton});

        return settingsMenu;
    }

    /**
     * Creates the tool bar for the application.
     *
     * @return a {@link JToolBar} for the application.
     */
    @Override
    protected JToolBar createToolBar() {
        String[] toolbarActionNames = {
                "newDP",
                "openDP",
                "saveDP",
                "---",
                "singlePlot",
                "comparisonPlot",
                "errorPlot",
                "contrastPlot",
                "tabularData",
                "tabularErrorData",
                "---",
                "toggleGnuplot",
                "---",
                "selectFunction",
                "---"
            };
        JToolBar toolBar = new JToolBar();
        toolBar.setFloatable(false);
        
        for (String actionName : toolbarActionNames) {
            if (actionName.equals("---")) {
                toolBar.addSeparator();
            } else if (actionName.equals("selectFunction")) {
            	functionButton = createButton("selectFunction", false);
            	functionButton.addMouseListener(new LocalListMouseListener());
            	toolBar.add(functionButton);
            } else if (actionName.equals("toggleGnuplot")) {
                toolBar.add(gnuplotButton);
            } else {
                toolBar.add(createButton(actionName, false));
            }
        }
        
        String[] popupMenuActions = new String[] { "subtractFunction", "reverseSubtractFunction", "cancelFunction" };
        functionPopup = createPopupMenu(popupMenuActions);
        
        toolBar.add(createButton("searchVar", true));
        searchField = new JTextField(10);
        searchField.addActionListener(new ActionListener() {
        	public void actionPerformed(ActionEvent e) {
        		searchVar();
        	}
        });
        toolBar.add(searchField);
        // set the minimum size for searchField so that the following added trick icon can be seen 
        searchField.setMinimumSize(searchField.getPreferredSize());
        toolBar.add(Box.createHorizontalGlue());
        toolBar.add(UIUtils.getSmallTrickIconLabel());
        return toolBar;
    }


    /**
     * Selects a tab.
     *
     * @param index    The index number of the tab that needs selecting.
     *
     */
    public void setSelectedTab(int index) {
        setSelectedTab(index, null);
    }

    /**
     * Selects one tab and disables all other tabs.
     *
     * @param index   The index number of the tab that needs selecting.
     * @param node    The {@link CommonTreeNode} that causes the tab is selected.
     */
    public void setSelectedTab(int index, CommonTreeNode node) {
        propertyPane.setSelectedIndex(index);
        for (int i = 0; i < QPTabType.values().length; i ++) {
            if (i == index) {
                propertyPane.setEnabledAt(i, true);
            } else {
                propertyPane.setEnabledAt(i, false);
            }
        }

        if (node != null) {
        	try {
        		dataPanels.get(index).setRelatedNode(node);
        	} catch (IndexOutOfBoundsException e) {
        		// do nothing
        	}
        }
    }
    
    /**
     * Saves the current gui session to a file. 
     * 
     * @param fullPath      The file name which the data is saved to.
     * @param updateTitle    True - updating the Trick QP GUI title with the fullPath info; false otherwise.
     * 
     */
    public void saveDPToFile(String fullPath, boolean updateTitle) {
        saveDPToFile(fullPath);
        
        if (updateTitle) {
            getMainFrame().setTitle(fullPath + " - " + resourceMap.getString("mainFrame.title"));
        }
    }
    
    /**
     * Saves the current gui session to a file. 
     * 
     * @param fullPath      The file name which the data is saved to.
     * 
     * @return True if the file is created successfully, false otherwise.
     */
    public boolean saveDPToFile(String fullPath) {
        XMLCreator xmlCreator = new ProductXMLCreator();
        xmlCreator.setCallerFrame(getMainFrame());
        xmlCreator.setXMLFile(fullPath);
        xmlCreator.setDTDFile("Product.dtd");
        xmlCreator.setPublicID("-//Tricklab//DTD Product V1.0//EN");

        Product product = new Product("1.0");

        if (versionField.getText() != null && !("".equals(versionField.getText().trim()))) {
            product.setVersion(versionField.getText());
        }
        xmlCreator.setRoot(product);

        product.setTitle(titleField.getText());
        product.setTStart((Double)startField.getValue());
        product.setTStop((Double)stopField.getValue());
        product.setFrequency((Double)freqField.getValue());

        // We need to add all pages, tables and extfn to Product object for the case of saving a DP file.
        // Otherwise, when it's the TMP_DP_FILE, meaning it's for displaying either plot or table. 
        // In other words, no pages are displayed if TABLE mode and no tables are displayed if PLOT mode.
        // TODO: Programs?
        // Since fxplot can show either plot or table or both based on the mode attibute in Session
        // xml file, go back to add all pages and let fxplot handle it.
        ArrayList<DefaultMutableTreeNode> allPages = null;
        ArrayList<DefaultMutableTreeNode> allTables = null;
        ArrayList<DefaultMutableTreeNode> allExtFunctions = null;
                
        allPages = productTree.getAllChildren(plotsNode);
        allTables = productTree.getAllChildren(tablesNode);
        allExtFunctions = productTree.getAllChildren(programsNode);
                
        // for pages
        if (allPages != null && allPages.size() > 0) {
            for (int i = 0; i < allPages.size(); i++) {
                CommonTreeNode thisPageNode = (CommonTreeNode)allPages.get(i);
                ProductPage thisPage = (ProductPage)thisPageNode.getUserObject();
                product.addPage(thisPage);
            }
        } // done with all pages

        // for tables
        if (allTables != null && allTables.size() > 0) {
            for (int i = 0; i < allTables.size(); i++) {
                CommonTreeNode thisTableNode = (CommonTreeNode)allTables.get(i);
                ProductTable thisTable = (ProductTable)thisTableNode.getUserObject();
                product.addTable(thisTable);
            }
        }

        // for extfn
        if (allExtFunctions != null && allExtFunctions.size() > 0) {
            for (int i = 0; i < allExtFunctions.size(); i++) {
                CommonTreeNode thisFunctionNode = (CommonTreeNode)allExtFunctions.get(i);
                ProductExternalFunction thisFunction= (ProductExternalFunction)thisFunctionNode.getUserObject();
                product.addExternalFunction(thisFunction);
            }
        }

        if (xmlCreator.runCreator()) {
        	return true;
        }
        return false;
    }

    //========================================
    //    Inner classes
    //========================================
    /**
     * In addition to the {@link MouseListener} implementation in {@link ListPanel}.
     */
    private class LocalListMouseListener implements MouseListener
    {
        /**
         * Default constructor.
         */
        public LocalListMouseListener() {
        }

        //========================================
        //    MouseListener methods
        //========================================
        /**
         * Invoked when the mouse button has been clicked (pressed
         * and released) on a component.
         *
         * @param e MouseEvent sent from system.
         */
        public void mouseClicked(MouseEvent e)
        {
        	if (e.getSource() == functionButton) {
        		functionPopup.show(e.getComponent(), e.getX(), e.getY());
        	} else if (UIUtils.isRightMouseClick(e)) {
                if (e.getSource() == runList.getJList()) {
                    runToConfigure = (SessionRun)runList.getSelectedFirstData();
                } else if (e.getSource() == varList.getJList()) {
                	if (varList.getSelectedData() != null && varList.getSelectedData().length > 0) {
                		getAction("addVar").setEnabled(true);
                		if (varList.getSelectedData().length > 1) {
                			getAction("changeUnits").setEnabled(false);
                		} else if (varList.getSelectedData().length == 1) {
                			getAction("changeUnits").setEnabled(true);
                		} 		
                	} else {
                		getAction("addVar").setEnabled(false);
                		getAction("changeUnits").setEnabled(false);
                	}
                }
                // common behaviors are handled in ListPanel
            } else if (UIUtils.isDoubleClick(e)) {
                if (e.getSource() == varList.getJList()) {                    
                    try {
                    	LogVar clickedVar = (LogVar)varList.getSelectedFirstData();
                    	boolean needToAdd = true;
                    	if (clickedVar.getDisplay() == DisplayType.CONTRACTED) {                    		
                    		expandVar();
                    	} else if (clickedVar.getDisplay() == DisplayType.EXPANDED) {
                    		needToAdd = false;
                    		contractVar();
                    	} else if (clickedVar.getDisplay() == DisplayType.HIDDEN) {
                    		needToAdd = false;
                    	}
                    	if (needToAdd) {
                    		addVar();
                    	}
                    } catch (Exception ex) {
                    	// for some reason, if there is an exception, do nothing.
                    }
                } else if (e.getSource() == runList.getJList()){
                    removeRun();
                }
            }
        }

        /**
         * Invoked when the mouse enters a component.
         *
         * @param e MouseEvent sent from system.
         */
        public void mouseEntered(MouseEvent e) {
            // TODO:
        }

        /**
         * Invoked when the mouse exits a component.
         *
         * @param e MouseEvent sent from system.
         */
        public void mouseExited(MouseEvent e) {
            // TODO:
        }

        /**
         * Invoked when a mouse button has been pressed on a component.
         *
         * @param e MouseEvent sent from system.
         */
        public void mousePressed(MouseEvent e) {
             // TODO:
        }

        /**
         * Invoked when a mouse button has been released on a component.
         *
         * @param e MouseEvent sent from system.
         */
        public void mouseReleased(MouseEvent e) {
             // TODO:
        }

    }


}
