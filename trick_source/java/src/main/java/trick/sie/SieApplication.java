
//========================================
//    Package
//========================================
package trick.sie;

//========================================
//    Imports
//========================================

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JComponent;
import javax.swing.JEditorPane;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;
import javax.swing.JTextPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.text.BadLocationException;
import javax.swing.text.StyledDocument;
import javax.swing.tree.TreePath;
import javax.xml.parsers.ParserConfigurationException;

import org.jdesktop.application.Application;
import org.jdesktop.swingx.JXEditorPane;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import trick.common.framework.BaseApplication;
import trick.common.framework.View;
import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.common.ui.panels.FindBar;
import trick.common.utils.SwingAction;
import trick.sie.utils.SearchPanel;
import trick.sie.utils.SieEnumeration;
import trick.sie.utils.SieResourceDomParser;
import trick.sie.utils.SieTemplate;
import trick.sie.utils.SieTreeModel;
import trick.sie.utils.SieVariableTree;


/**
 * SIE application.
 *
 * @since Trick 10
 */
public class SieApplication extends TrickApplication implements TreeSelectionListener {
    
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
    protected static String sieResourcePath = null;
    
    //========================================
    //    Private Data
    //========================================
    private enum TabTypes { 
        VAR_TREE, 
        ENUMERATED_TYPES;
             
        public String getTitle() {
            String title = "Var Tree";          
            switch (this) {
            case VAR_TREE:
                title = "Var Tree";
                break;           
            case ENUMERATED_TYPES:
                title = "Enumerated Types";
                break;
            }
            return title;
        };
    }
    
    /** The parser for SIE data */
    //private SieResourceXMLParser sieXMLParser;
    
    /** Popup window for clicking a tree node **/
    private JPopupMenu treePopup;
    
    /** Tabbed pane */
    private JTabbedPane tabbedPane;
        
    /** The variable (sim objects) tree */
    private SieVariableTree varTree;
    
    /** The sim objects top level instances */
    private Collection<SieTemplate> rootTemplates;
    
    private HashMap<String, SieEnumeration> simEnums;
    
    /** The search panel for the variable tree */
    private SearchPanel searchPanel;
          
    /** The reading pane for showing detailed info. of variables. */
    private JTextPane readingPaneForVars;
          
    private TabTypes tabType = TabTypes.VAR_TREE;
    
    
    //========================================
    //    Constructors
    //========================================
    
 

    //========================================
    //    Actions
    //========================================      
    @SwingAction
    public void saveVariableDetails() {     
        String initialName = "sie_var_";
        File file = UIUtils.chooseSaveFile(null, initialName, null, getMainFrame());
        if (file != null) {   
            if (tabbedPane.getSelectedIndex() == TabTypes.VAR_TREE.ordinal()) {
                UIUtils.saveTextFile(readingPaneForVars.getText(), file);
            }
        }       
    }
    
    @SwingAction
    public void showVariableDetails() {
        try {
            updateReadingPaneForSearch();
        } catch (BadLocationException e) {
            // Do nothing
        }
    }
    
    //========================================
    //    Set/Get methods
    //========================================
    
    
    //========================================
    //    Methods
    //========================================
    /** 
     * Required by TreeSelectionListener interface. 
     * Called whenever the value of the selection changes.
     * 
     * @param e the event that characterizes the change.
     */
    public void valueChanged(final TreeSelectionEvent e)  {     
        try {
            updateReadingPaneForVars();
        } catch (BadLocationException ble) {
            
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
               
        setAndRunParser();
        
        setupInitialGUIComponents();
    }
  
    /**
     * Helper method for setting the parse as well as run it.
     */
    private void setAndRunParser() {
        File resourceFile;
        if (sieResourcePath == null) {
            resourceFile = new File(resourceMap.getString("sie.resource.file"));
        } else {
            resourceFile = new File(sieResourcePath);
        }
        
        if (resourceFile != null && !resourceFile.exists()) {
            System.out.println(resourceFile.getName() + " file does not exist. Exit!!!");
            System.exit(0);
        }
    
        rootTemplates = getRootTemplates(resourceFile);
        try {
			simEnums = SieResourceDomParser.getSieEnumerationMap(new InputSource(new FileInputStream(resourceFile)));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
        
        /*try {
            sieXMLParser = new SieResourceXMLParser(resourceFile.getCanonicalPath());            
            sieXMLParser.runParser();
        } catch (Exception e) {
            System.out.println("Failed to parse " + resourceFile.getName() + "!");
            System.exit(0);
        }*/
    }

    /**
     * Helper method for getting all top level sim object templates.
     */
	private static Collection<SieTemplate> getRootTemplates(File resourceFile) {
		try {
			return SieResourceDomParser.parse(new InputSource(new FileInputStream(resourceFile)));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (ParserConfigurationException e) {
			e.printStackTrace();
		} catch (SAXException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}
    
    /**
     * Helper method for initializing GUI.
     */
    private void setupInitialGUIComponents() {
        treePopup = new JPopupMenu();
        
        varTree = new SieVariableTree();
        varTree.setRootInstances(rootTemplates);
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
        
        show(view);
    }

    @Override
    protected void ready() {
        super.ready();  
    }
    
   
    /**
     * Main method.
     * @param args command line arguments
     */
    public static void main(String[] args) {
        if (args.length == 1) {
            printMatchVarOnScreen(args[0]);
        } else {
            BaseApplication.launch(SieApplication.class, args);
        }
    }
    
    /**
     * Creates the main panel. This is required by TrickApplication.
     * 
     * @return a {@link JComponent} as the main panel.
     */
    @Override
    protected JComponent createMainPanel() {                     
        JComponent tabForTree = null;        
        JComponent tabForEnumerated = null;
               
        tabForTree = createTabForTree();        
        tabForEnumerated = createTabForEnumerated();
            
        tabbedPane = new JTabbedPane();        
        tabbedPane.addTab(TabTypes.VAR_TREE.getTitle(), tabForTree);
        tabbedPane.addTab(TabTypes.ENUMERATED_TYPES.getTitle(), tabForEnumerated);
                     
        setReadyVariableTree();
        
        setReadyTabbedPane();
        
        return tabbedPane;
    }

    /**
     * Helper method for creating the tab for enumerated types. 
     * This pane displays all enumerated types in HTML format.
     */
    private JComponent createTabForEnumerated() {
        JComponent tabForEnumerated;
        
        if (simEnums!= null) {
            tabForEnumerated = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
            tabForEnumerated.setPreferredSize(new Dimension(600, 500));
            setupForViewingEnumeratedTypes(tabForEnumerated, simEnums.values());            
        } else {
            tabForEnumerated= new JTextArea();
        }
        return tabForEnumerated;
    }
    
    /**
     * Helper method for creating the tab for variable tree.
     * @return
     */
    private JComponent createTabForTree() {
        JComponent tabForTree = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        
        JComponent treeAndSearch = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);
        
        searchPanel = new SearchPanel();
        
        searchPanel.setAction(actionMap.get("showVariableDetails"));
        
        searchPanel.setRootTemplates(rootTemplates);
        
        treeAndSearch.add(UIUtils.createSearchableTitledPanel("Variable Tree", varTree, null));
        treeAndSearch.add(searchPanel);
        readingPaneForVars = UIUtils.createReadingTextPane();
        
        tabForTree.add(treeAndSearch);
        tabForTree.add(UIUtils.createSearchableTitledPanel("Selected Variable(s) Details", readingPaneForVars, null));                 
        return tabForTree;
    } 
    
    /**
     * Helper method for getting tab pane ready.
     */
    private void setReadyTabbedPane() {
        tabbedPane.setSelectedIndex(tabType.ordinal());
        
        tabbedPane.addChangeListener(new ChangeListener() {
            // This method is called whenever the selected tab changes
            public void stateChanged(ChangeEvent evt) {
                JTabbedPane pane = (JTabbedPane)evt.getSource();               
                // Get current tab
                int sel = pane.getSelectedIndex();
                if (sel == TabTypes.ENUMERATED_TYPES.ordinal()) {
                    getAction("saveVariableDetails").setEnabled(false);
                } else {
                    getAction("saveVariableDetails").setEnabled(true);
                }               
                tabType = TabTypes.values()[sel];
            }
        });
    }

    /**
     * Helper method for getting variable tree ready.
     */
    private void setReadyVariableTree() {
        // create a mouse listener and add to tree
        TreeMouseListener mouser = new TreeMouseListener();
        varTree.addMouseListener(mouser);               
        varTree.setShowsRootHandles(true);
        varTree.addTreeSelectionListener(this);
        varTree.setVisibleRowCount(20);
        // scroll to the selected node if applicable
        varTree.scrollPathToVisible(varTree.getSelectionPath());
    }
    
    
    /**
     * Create the JMenuBar for this application. 
     */
    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();
        JMenu menu = menuBar.getMenu(0);
        menu.add(new JSeparator(), 0);
        menu.add(new JMenuItem(getAction("saveVariableDetails")), 0);
        return menuBar;
    }

    
    /**
     * Sets up the components for displaying and browsing all the enumerated types.
     * 
     * @param comp the component to add other components to.
     * @param enums the list of all SieEnumeration objects.
     */
    private void setupForViewingEnumeratedTypes(JComponent comp, Collection<SieEnumeration> enums) {
        // The area to display the links of first letters of all enumerated types.
        final JEditorPane headerPane = new JEditorPane();
        // The area to display all enumerated types in details. 
        final JXEditorPane contentPane = new JXEditorPane();
        
        JScrollPane headerScroll = new JScrollPane(headerPane);
        headerScroll.setPreferredSize(new Dimension(600, 100));
        
        JPanel contentPanel = new JPanel();
        contentPanel.setLayout(new BorderLayout());
        
        JScrollPane contentScroll = new JScrollPane(contentPane);
        contentScroll.setPreferredSize(new Dimension(600, 400));
        contentPanel.add(contentScroll, BorderLayout.CENTER);
        
        headerPane.setEditable(false);               
        headerPane.setContentType("text/html"); 
            
        contentPane.setEditable(false);
        contentPane.setContentType("text/html");
            
        FindBar findBar = new FindBar(contentPane.getSearchable());
        contentPanel.add(findBar, BorderLayout.SOUTH);
            
        // Add all the desired components to the comp
        comp.add(headerScroll);    
        comp.add(contentPanel);
           
        headerPane.setText(generateEnumeratedTypesHeaderLinks(enums));
        contentPane.setText(generateEnumeratedTypesContent(enums));
            
        //Set up the JEditorPane to handle clicks on hyperlinks
        headerPane.addHyperlinkListener(new HyperlinkListener() {
            public void hyperlinkUpdate(HyperlinkEvent e) {
                // Handle clicks; ignore mouseovers and other link-related events
                if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
                    // Get the HREF of the link and display it.
                    contentPane.scrollToReference(e.getDescription());
                }
            }
        });
    }
           
    /**
     * Prints all matched variables on screen.
     */
    private static void printMatchVarOnScreen(String matchingText) {
        File resourceFile = null;
        //SieResourceXMLParser parser = null;
        resourceFile = new File("S_sie.resource");
        
        if (resourceFile != null && !resourceFile.exists()) {
            System.out.println(resourceFile.getName() + " file does not exist. Exit!!!");
            System.exit(0);
        }
        System.out.println("Initializing search...");
        System.out.println("Finding " + matchingText + "...");
        /*try {
            parser = new SieResourceXMLParser(resourceFile.getCanonicalPath());           
            parser.runParser();
        } catch (Exception e) {
           System.out.println("Failed to parse " + resourceFile.getName() + "!");
            System.exit(0);
        }*/
        Collection<SieTemplate> rootTemplates = getRootTemplates(resourceFile);
        boolean found = false;
        
        for (SieTemplate eachTemplate : rootTemplates) {
            for (SieTemplate variable : eachTemplate) {
                if (variable.parameter.contains(matchingText)) {                    
                        found = true;
                        System.out.println(getPathStringWithDimExpanded(variable.toString()));
                        System.out.println(getData(variable));
                    
                }
            }
        }        
        
        if (!found) {
            System.out.println("No matched variables found!");              
        }
    }

    /**
     * Generates the content in html format for all the enumerated types.
     * 
     * @param enums the list of all SieEnumeration objects.
     * 
     * @return the String of all information of the enumerated types in html format.
     */
    private String generateEnumeratedTypesContent(Collection<SieEnumeration> enums) {
        HashSet<String> sectionLetterSet = new HashSet<String>();
        StringWriter sout = new StringWriter();
        PrintWriter out = new PrintWriter(sout);
        
        if (enums != null && enums.size() > 0) {
                // sort
                ArrayList<SieEnumeration> sortedEnums = new ArrayList<SieEnumeration>(enums);
                Collections.sort(sortedEnums);
                SieEnumeration item = null;
                         
                for (Object obj : sortedEnums) {
                    if (obj instanceof SieEnumeration) {
                        item = (SieEnumeration)obj;
                        String firstLetter = (item.name.substring(0,1));
                        
                        // Only adds the section name if it is not created. 
                        // The section name is "_" + first letter of the type.
                        if (sectionLetterSet.contains(firstLetter)) {
                            out.println("<H3>" + item.name + "</H3>");                           
                        } else {    
                            out.println("<H3><A NAME=\"_" + firstLetter + "\">" + item.name + "</A></H3>");
                            sectionLetterSet.add(firstLetter);
                        }
        
                        //int valSize = item.pairs.keySet().size();
                        // Lists all the values of this enumerated type.
                        if (item.pairs != null && item.pairs.keySet().size() > 0) {
                            out.println("<DL>");
                            for (Object eachLabel : item.pairs.keySet()) {
                                out.println("<DD>" + eachLabel.toString());
                            }
                            out.println("</DL>");
                            out.println("<HR");
                        }
                    }
                }
        }
        out.close();
        
        return sout.toString();
    }
    
    /**
     * Generates the links for all the enumerated types based on their first letters.
     * 
     * @param enums the list of all SieEnumeration objects.
     * 
     * @return the String of all the unique letter (case senstive) links based on 
     *        the first letters of all enumerated types.
     */
    private String generateEnumeratedTypesHeaderLinks(Collection<SieEnumeration> enums) {
        HashSet<String> letterSet = new HashSet<String>();
        StringWriter sout = new StringWriter();
        PrintWriter out = new PrintWriter(sout);
        out.println("<H2>" + "All Enumerated Types" + "</H2>");
        
        // Set up the letterSet without duplicate letters.
        // All letters are case senstive. e.g. the links could have both "R" and "r".
        if (enums != null && enums.size() > 0) {
            SieEnumeration item = null;
            for (Object obj : enums) {
                item = (SieEnumeration)obj;
                String firstLetter = (item.name.substring(0, 1));
                if (!letterSet.contains(firstLetter)) {
                    letterSet.add(firstLetter);
                }
            }
        } 

        if (!letterSet.isEmpty()) {
            Object[] letters = letterSet.toArray();
            // All links are in alphabet order and case senstive.
            Arrays.sort(letters);
            for (int i = 0; i < letters.length; i ++) {
                out.println("<A HREF=\"_" + letters[i].toString() + "\">" + letters[i].toString() + "</A>");
            }
        }
        out.close();
        return sout.toString();
    }
    
    /**
     * Updates the reading pane for selected items from search results.
     * 
     * @throws BadLocationException BadLocationException
     */
    public void updateReadingPaneForSearch() throws BadLocationException {        
        // if the reading pane is not ready, do nothing
        if (readingPaneForVars == null) {
            return;
        }
        
        StyledDocument readingContent = readingPaneForVars.getStyledDocument();
        eraseReadingContent(readingContent);
        for (SieTemplate thisValue : searchPanel.getSelectedValues()) {
            insertTitleToReadingContent(readingContent, getPathStringWithDimExpanded(thisValue.toString()));
            insertDataToReadingContent(readingContent, thisValue);
        }
    }
    
    
    /**
     * Updates the reading pane for showing details of variables.
     * 
     * @throws BadLocationException BadLocationException
     */
    public void updateReadingPaneForVars() throws BadLocationException {
        // if the reading pane is not ready, do nothing
        if (readingPaneForVars == null) {
            return;
        }
        
        TreePath[] selectedPaths = varTree.getSelectionPaths();
        if (selectedPaths == null || selectedPaths.length == 0) {
            return;
        }

        StyledDocument readingContent = readingPaneForVars.getStyledDocument();
        eraseReadingContent(readingContent);
        
        for (TreePath path : selectedPaths) {
            SieTemplate template = (SieTemplate)path.getLastPathComponent();
            if (!template.isTrickManaged()) {
                continue;
            }
            insertTitleToReadingContent(readingContent, getPathStringWithDimExpanded(SieTreeModel.getPathName(path) + template));
            insertDataToReadingContent(readingContent, template);
        }
    }

    /**
	 * If the variable path that contains the dim number such as
	 * ball.obj.force.output.force[2], 2 is actually the size of the array
	 * and users are used to see it shown as ball.obj.force.output.force[0-1].
	 * 
	 * @return a String that has all tree node names separated by "." with
	 *         [num] expanded if applicable.
	 */
	private static String getPathStringWithDimExpanded(String originalPath) {		
		String patternExpression = "\\[([1-9][0-9]*)\\]";
		Pattern pattern = Pattern.compile(patternExpression);
		Matcher m = pattern.matcher(originalPath);
		StringBuffer sb = new StringBuffer();
		while (m.find()) {
			String replacement = "[0-" + (Integer.valueOf(originalPath.substring(m.start()+1, m.end()-1)) - 1) + "]";
			m.appendReplacement(sb, replacement);
		}
		m.appendTail(sb);
		return sb.toString();
	}
    
    /**
     * Helper method for inserting some data info to the reading content.
     */
    private void insertDataToReadingContent(StyledDocument readingContent, SieTemplate sieObj) throws BadLocationException {
        if (sieObj != null) {    
            readingContent.insertString(readingContent.getLength(), getData(sieObj) + "\n", readingContent.getStyle(UIUtils.DOCUMENT_STYLE_REGULAR));                
        }
    }

    /**
     * Helper method for inserting a string served as a title to the reading content.
     */
    private void insertTitleToReadingContent(StyledDocument readingContent, String pathStr) throws BadLocationException {
        if (pathStr != null) {
            readingContent.insertString(readingContent.getLength(), pathStr + "\n", readingContent.getStyle(UIUtils.DOCUMENT_STYLE_REGULAR_LARGE_BOLD_BLUE));    
        }
    }
    
    /**
     * Helper method for erasing all contents of a {@link StyledDocument}.
     */
    private void eraseReadingContent(StyledDocument readingContent) {
        // clear reading pane
        try {
            readingContent.remove(0, readingContent.getLength());
        } catch (BadLocationException ble) {
            // do nothing
        }
    }

    private static String getData(SieTemplate template) {
        StringBuffer ret = new StringBuffer();
        if (template.description == null || template.description.isEmpty()) {
            ret.append("\tNot Specified\n");
        } else {
            ret.append("\t" + template.description + "\n");
        }   
        ret.append("\t" + template.typeName + "    " + template.ioType + "    " + template.units);
        return ret.toString();
    }
    
    //========================================
    //    Inner classes
    //========================================
    /**
     * Using an inner class to define MouseListener to help organize code better.
     * The goal of this class is to handle mouse calls and forward them
     * to the interested parties.
     */
    private class TreeMouseListener extends MouseAdapter {
        
        //========================================
        //    MouseListener methods
        //========================================
        /**
         * Invoked when the mouse button has been clicked (pressed
         * and released) on a component.
         *
         * @param e MouseEvent sent from system.
         */
        @Override
		public void mouseClicked(MouseEvent e) {
            if( UIUtils.isRightMouseClick(e) ) {
                TreePath rightClickedPath = varTree.getClosestPathForLocation(e.getX(), e.getY());
                
                SieTemplate rightClickedNode = null;
                
                if (rightClickedPath.getLastPathComponent() instanceof SieTemplate) {
                    rightClickedNode = (SieTemplate)rightClickedPath.getLastPathComponent();
                }
                if (rightClickedNode != null && rightClickedNode.isTrickManaged()) {                    
                    if (treePopup.getComponentCount() > 0) {
                        treePopup.removeAll();
                    }
                    
                    updateTreePopupContents(rightClickedPath);
                    
                    if (!treePopup.isVisible()) {    
                        treePopup.show(e.getComponent(), e.getX(), e.getY());
                    }
                }
            } else {
                if (treePopup.isVisible()) {
                    treePopup.setVisible(false);
                }               
            }
        }

        /**
         * Helper method for updating the contents related to the right-clicked node for popup display.
         */
        private void updateTreePopupContents(TreePath rightClickedNode) {
            SieTemplate clickedObj = (SieTemplate)rightClickedNode.getLastPathComponent();
            treePopup.add(SieTreeModel.getPathName(rightClickedNode) + clickedObj);                                     
            treePopup.addSeparator();
            
            if (clickedObj.isEnumerated()) {               
                JMenu subMenu = new JMenu("Type: " + clickedObj.typeName);
                if (clickedObj.enumeration != null) {
                    for (final Object eachLabel : clickedObj.enumeration.pairs.keySet()) {
                        subMenu.add((String)eachLabel);
                    }
                }
                treePopup.add(subMenu);
            } else {
                treePopup.add("Type: " + clickedObj.typeName);
            }
            treePopup.add("Units: " + clickedObj.units);
        }
    }
    
}
