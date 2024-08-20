
//========================================
//	Package
//========================================
package trick.dre;

//========================================
//	Imports
//========================================

import org.jdesktop.application.Application;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXTextField;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import trick.common.framework.BaseApplication;
import trick.common.framework.View;
import trick.common.ui.UIUtils;
import trick.common.ui.components.NumberTextField;
import trick.common.ui.panels.ListPanel;
import trick.common.utils.SwingAction;
import trick.common.utils.vs.Variable;
import trick.common.TrickApplication;
import trick.sie.utils.*;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.*;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.util.List;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.*;
import javax.swing.tree.TreePath;
import javax.xml.parsers.ParserConfigurationException;

/**
 * Dre - data recording editor application.
 *
 * @author Hong Chen
 * @author Scott Fennell
 * @intern Marcus Rockwell
 * @since Trick 10
 * @since Trick 17
 */
public class DreApplication extends TrickApplication {
    //========================================
    //	Public data
    //========================================


    //========================================
    //	Protected data
    //========================================
    protected static String sieResourcePath = null;

    //========================================
    //	Private Data
    //========================================
    private String single_prec_only;
    private String frequency;
    private String format;
    private String buffering;
    private Vector<String> variables = new Vector<String>();

    /**
     * The menu check box for Single Precision.
     */
    private JCheckBoxMenuItem singlePrecisionCheckBox;

    /**
     * Popup window for clicking a tree node
     **/
    private JPopupMenu treePopup = null;

    /** S_sie.resource xml parser */

    /**
     * The sim objects top level instances
     */
    private Collection<SieTemplate> rootTemplates;

    /**
     * The variable (sim objects) tree
     */
    private SieVariableTree varTree;

    /**
     * Dialog for adding arrayed variables
     */
    private AddVariableDialog addVariableDialog;

    /**
     * The search panel for the variable tree
     */
    private SearchPanel searchPanel;

    /**
     * The selected variable list
     */
    private ListPanel selectedVarList;

    /**
     * The text field that contains the group name
     */
    private JTextField nameField;

    /**
     * The text field that contains the cycle frequency for recording
     */
    private NumberTextField cycleField;

    /**
     * The text field that contains the max file size for the group
     */
    private NumberTextField maxFileSizeField;

    private JComboBox<String> sizeUnitsBox;

    private JCheckBox unlimitedSizeBox;

    private JRadioButtonMenuItem DRAscii_item;
    private JRadioButtonMenuItem DRBinary_item;
    private JRadioButtonMenuItem DRHDF5_item;

    private JRadioButtonMenuItem DRBuffer_item;
    private JRadioButtonMenuItem DRNoBuffer_item;
    private JRadioButtonMenuItem DRRingBuffer_item;

    private JRadioButtonMenuItem DRAlways_item;
    private JRadioButtonMenuItem DRChanges_item;
    private JRadioButtonMenuItem DRStepChanges_item;

    private boolean isSinglePrecision;

    /**
     * Vectors to contain the information on the variable
     * being added to the recording list.
     */

    //========================================
    //	Constructors
    //========================================

    //========================================
    //    Actions
    //========================================
    @SwingAction
    public void openDR() {
        File file = UIUtils.chooseOpenFile(null, null, "dr", getMainFrame());
        if (file != null) {
            openFile(file);
        }
    }

    @SwingAction
    public void saveDR() {
        if (nameField.getText().trim().compareTo("") == 0) {
            JOptionPane.showMessageDialog(getMainFrame(), "A group name must be entered!",
                    "Error", JOptionPane.ERROR_MESSAGE);
            return;
        } else if (nameField.getText().trim().contains(" ")) {
            JOptionPane.showMessageDialog(getMainFrame(), "A group name can not have whitespace!",
                    "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        File file = UIUtils.chooseSaveFile(null, null, "dr", getMainFrame());
        if (file != null) {
            saveFile(file);
        }
    }

    @SwingAction
    public void selectDRBinary() {
        format = "DRBinary";
        DRAscii_item.setSelected(false);
        DRBinary_item.setSelected(true);
        DRHDF5_item.setSelected(false);
    }

    @SwingAction
    public void selectDRAscii() {
        format = "DRAscii";
        DRAscii_item.setSelected(true);
        DRBinary_item.setSelected(false);
        DRHDF5_item.setSelected(false);
    }

    @SwingAction
    public void selectDRHDF5() {
        format = "DRHDF5";
        DRAscii_item.setSelected(false);
        DRBinary_item.setSelected(false);
        DRHDF5_item.setSelected(true);
    }

    @SwingAction
    public void selectDRAlways() {
        frequency = "DR_Always";
        DRAlways_item.setSelected(true);
        DRChanges_item.setSelected(false);
        DRStepChanges_item.setSelected(false);
    }

    @SwingAction
    public void selectDRChanges() {
        frequency = "DR_Changes";
        DRAlways_item.setSelected(false);
        DRChanges_item.setSelected(true);
        DRStepChanges_item.setSelected(false);
    }

    @SwingAction
    public void selectDRStepChanges() {
        frequency = "DR_Step_Changes";
        DRAlways_item.setSelected(false);
        DRChanges_item.setSelected(false);
        DRStepChanges_item.setSelected(true);
    }

    @SwingAction
    public void toggleSinglePrecision() {
        isSinglePrecision = singlePrecisionCheckBox.getState();
        if (isSinglePrecision) {
            single_prec_only = "True";
        } else {
            single_prec_only = "False";
        }
    }

    @SwingAction
    public void selectDRBuffer() {
        buffering = "DR_Buffer";
        DRBuffer_item.setSelected(true);
        DRNoBuffer_item.setSelected(false);
        DRRingBuffer_item.setSelected(false);
    }

    @SwingAction
    public void selectDRNoBuffer() {
        buffering = "DR_No_Buffer";
        DRBuffer_item.setSelected(false);
        DRNoBuffer_item.setSelected(true);
        DRRingBuffer_item.setSelected(false);
    }

    @SwingAction
    public void selectDRRingBuffer() {
        buffering = "DR_Ring_Buffer";
        DRBuffer_item.setSelected(false);
        DRNoBuffer_item.setSelected(false);
        DRRingBuffer_item.setSelected(true);
    }

    @SwingAction
    public void selectDRThreadBuffer() {
        buffering = "DR_Thread_Buffer";
        DRBuffer_item.setSelected(false);
        DRNoBuffer_item.setSelected(false);
        DRRingBuffer_item.setSelected(false);
    }

    @SwingAction
    public void removeSelected() {
        Object[] values = selectedVarList.getSelectedData();
        for (int i = 0; i < values.length; i++) {
            variables.remove(values[i]);
        }
        selectedVarList.removeSelectedData();
    }

    @SwingAction
    public void removeAll() {
        selectedVarList.removeAllData();
        variables.clear();
    }

    @SwingAction
    public void addVariables() {
        for (SieTemplate thisValue : searchPanel.getSelectedValues()) {
            addVariable(thisValue.toString());
        }
    }


    //========================================
    //	Set/Get methods
    //========================================


    //========================================
    //	Methods
    //========================================

    /**
     * Main method for this application.
     *
     * @param args command line arguments
     */
    public static void main(String[] args) {
        BaseApplication.launch(DreApplication.class, args);
    }

    /**
     * Makes initialization as needed. This is called before startup().
     *
     * @see #startup
     */
    @Override
    protected void initialize(String[] args) {
        super.initialize(args);
        File resourceFile = null;
        if (sieResourcePath == null) {
            resourceFile = new File(resourceMap.getString("sie.resource.file"));
        } else {
            resourceFile = new File(sieResourcePath);
        }
        if (!resourceFile.exists()) {
            System.out.println(resourceFile.getName() + " file does not exist. Exit!!!");
            System.exit(0);
        }

        try {
            rootTemplates = SieResourceDomParser.parse(new InputSource(new FileInputStream(resourceFile)));
        } catch (ParserConfigurationException | SAXException | IOException e) {
            e.printStackTrace();
        }

        treePopup = new JPopupMenu();

        varTree = new SieVariableTree();
        varTree.setRootInstances(rootTemplates);

        varTree.setShowsRootHandles(true);
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

        addVariableDialog = new AddVariableDialog();

        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());
        view.setToolBar(createToolBar());
        getMainFrame().setMinimumSize(getMainFrame().getPreferredSize());
        show(view);


    }

    /**
     * Creates the main panel. This is required by TrickApplication.
     *
     * @return a {@link JComponent} as the main panel.
     */
    @Override
    protected JComponent createMainPanel() {
        JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        JSplitPane treeSplitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT);

        searchPanel = new SearchPanel();

        searchPanel.setAction(actionMap.get("addVariables"));

        searchPanel.setRootTemplates(rootTemplates);

        treeSplitPane.add(UIUtils.createSearchableTitledPanel("Variables", varTree, null));
        treeSplitPane.add(searchPanel);

        splitPane.add(treeSplitPane);
        splitPane.add(UIUtils.createSearchableTitledPanel("Selected Variables", createSelectedVariablesPanel(), null));

        // create a mouse listener and add to tree
        TreeMouseListener mouser = new TreeMouseListener();
        varTree.addMouseListener(mouser);

        return splitPane;
    }

    /**
     * Helper method for creating the variables tab.
     */
    private JComponent createSelectedVariablesPanel() {
        JPanel panel = new JPanel();
        panel.setLayout(new BorderLayout());
        panel.add(new JSeparator(), BorderLayout.NORTH);
        selectedVarList = new ListPanel();
        String[] popupMenuActions = {"removeSelected", "removeAll"};
        selectedVarList.setPopup(createPopupMenu(popupMenuActions), 0);
        panel.add(selectedVarList, BorderLayout.CENTER);
        return panel;
    }

    /**
     * Create the JMenuBar for this application.
     */
    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();
        JMenu menu = menuBar.getMenu(0);
        menu.add(new JSeparator(), 0);
        menu.add(new JMenuItem(getAction("saveDR")), 0);
        menu.add(new JMenuItem(getAction("openDR")), 0);

        menuBar.add(createOptionsMenu(), 1);

        return menuBar;
    }

    /**
     * Helper method for creating Options menu.
     */
    private JMenu createOptionsMenu() {
        JMenu optionsMenu = new JMenu();
        optionsMenu.setName("optionsMenu");

        optionsMenu.add(new JLabel("Format		"));

        DRBinary_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRBinary_item);
        DRBinary_item.setAction(getAction("selectDRBinary"));

        DRAscii_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRAscii_item);
        DRAscii_item.setAction(getAction("selectDRAscii"));

        DRHDF5_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRHDF5_item);
        DRHDF5_item.setAction(getAction("selectDRHDF5"));

        selectDRBinary(); // by default, DR_Binary      
        optionsMenu.addSeparator();

        optionsMenu.add(new JLabel("Freq"));

        DRAlways_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRAlways_item);
        DRAlways_item.setAction(getAction("selectDRAlways"));

        DRChanges_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRChanges_item);
        DRChanges_item.setAction(getAction("selectDRChanges"));

        DRStepChanges_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRStepChanges_item);
        DRStepChanges_item.setAction(getAction("selectDRStepChanges"));

        selectDRAlways(); // by default, DR_Always

        optionsMenu.addSeparator();

        singlePrecisionCheckBox = new JCheckBoxMenuItem(getAction("toggleSinglePrecision"));
        toggleSinglePrecision();

        optionsMenu.add(singlePrecisionCheckBox);
        optionsMenu.addSeparator();

        optionsMenu.add(new JXLabel("Buffering"));

        DRBuffer_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRBuffer_item);
        DRBuffer_item.setAction(getAction("selectDRBuffer"));

        DRNoBuffer_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRNoBuffer_item);
        DRNoBuffer_item.setAction(getAction("selectDRNoBuffer"));

        DRRingBuffer_item = new JRadioButtonMenuItem();
        optionsMenu.add(DRRingBuffer_item);
        DRRingBuffer_item.setAction(getAction("selectDRRingBuffer"));

        selectDRBuffer();

        return optionsMenu;
    }

    /**
     * Creates the tool bar for the application.
     *
     * @return a {@link JToolBar} for the application.
     */
    @Override
    protected JToolBar createToolBar() {

        JToolBar toolBar = new JToolBar();

        // add buttons
        String[] toolbarActionNames = {"openDR", "saveDR"};
        for (String actionName : toolbarActionNames) {
            if (actionName.equals("---")) {
                toolBar.addSeparator();
            } else {
                toolBar.add(createButton(actionName, false));
            }
        }

        nameFieldInit();
        cycleFieldInit();
        maxFileSizeFieldInit();
        sizeUnitsBoxInit();
        unlimitedSizeBoxInit();

        toolBar.addSeparator();
        toolBar.add(new JLabel("DR Name (NO SPACE):  "));
        toolBar.add(nameField);
        toolBar.add(Box.createHorizontalStrut(10));
        toolBar.add(new JLabel("DR Cycle:  "));
        toolBar.add(cycleField);
        toolBar.addSeparator();
        toolBar.add(new JLabel(" Max File Size: "));
        toolBar.add(maxFileSizeField);
        toolBar.add(sizeUnitsBox);
        toolBar.addSeparator();
        toolBar.add(unlimitedSizeBox);
        toolBar.setSize(toolBar.getPreferredSize());
        toolBar.addSeparator();

        return toolBar;
    }

    private void nameFieldInit() {
        nameField = new JTextField(15);
        nameField.setMinimumSize(nameField.getPreferredSize());
        nameField.setPreferredSize(nameField.getPreferredSize());
        nameField.setMaximumSize(nameField.getPreferredSize());
    }

    private void cycleFieldInit() {
        cycleField = new NumberTextField("0.1", 5);
        cycleField.setMinimumSize(cycleField.getPreferredSize());
    }

    private void maxFileSizeFieldInit() {
        maxFileSizeField = new NumberTextField("1", 10);
        maxFileSizeField.setMinimumSize((maxFileSizeField.getPreferredSize()));
    }

    private void sizeUnitsBoxInit() {
        String[] units = {"B", "KiB", "MiB", "GiB"};
        sizeUnitsBox = new JComboBox<>(units);
        sizeUnitsBox.setSelectedItem(sizeUnitsBox.getItemAt(3));
    }

    private void unlimitedSizeBoxInit() {
        unlimitedSizeBox = new JCheckBox("Unlimited File Size", false);
        unlimitedSizeBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                updateUnlimited();
            }
        });
    }

    /**
     * routine to read the Frequency from the opened file.
     *
     * @param string String the string read in from the opened file.
     */
    private void readFrequency(String string) {
        if (string.contains("trick.DR_Always")) {
            selectDRAlways();
        } else if (string.contains("trick.DR_Changes")) {
            selectDRChanges();
        } else if (string.contains("trick.DR_Step_Changes")) {
            selectDRStepChanges();
        } else {
            System.out.println("Frequency Type is not recognized, defaulting to DR_Always");
            selectDRAlways();
        }
    }

    /**
     * routine to read the Buffering from the opened file.
     *
     * @param string String the string read in from the opened file.
     */
    private void readBuffering(String string) {
        if (string.contains("trick.DR_Buffer")) {
            selectDRBuffer();
        } else if (string.contains("trick.DR_No_Buffer")) {
            selectDRNoBuffer();
        } else if (string.contains("trick.DR_Ring_Buffer")) {
            selectDRRingBuffer();
        } else if (string.contains("trick.DR_Thread_Buffer")) {
            selectDRThreadBuffer();
        } else {
            System.out.println("Buffering Type is not recognized, defaulting to DR_Buffer");
            selectDRBuffer();
        }
    }

    /**
     * routine to read the Format from the opened file.
     *
     * @param string String the string read in from the opened file.
     */
    private void readFormat(String string) {
        if (string.contains("DRBinary")) {
            selectDRBinary();
        } else if (string.contains("DRAscii")) {
            selectDRAscii();
        } else if (string.contains("DRHDF5")) {
            selectDRHDF5();
        } else {
            System.out.println("Format Type is not recognized, defaulting to DR_Binary");
            selectDRBinary();
        }
    }

    /**
     * routine to read the Max File Size from the opened file.
     *
     * @param subs String the string read in from the opened file.
     */
    private void readFileSize(String subs) {
        StringTokenizer st = new StringTokenizer(subs);
        String quantity = st.nextToken("*").trim();
        maxFileSizeField.setText(quantity);
        if (maxFileSizeField.getText().equals("0")) {
            unlimitedSizeBox.setSelected(true);
            updateUnlimited();
        } else {
            unlimitedSizeBox.setSelected(false);
            maxFileSizeField.setEnabled(true);
            sizeUnitsBox.setEnabled(true);
        }
        if (st.hasMoreElements()) {
            String shift = st.nextToken("*").trim();
            switch (shift) {
                case "1024":
                    sizeUnitsBox.setSelectedItem("KiB");
                    break;
                case "1048576":
                    sizeUnitsBox.setSelectedItem("MiB");
                    break;
                case "1073741824":
                    sizeUnitsBox.setSelectedItem("GiB");
                    break;
            }
        } else {
            sizeUnitsBox.setSelectedItem("B");
        }
    }

    /**
     * helper method to update GUI after unlimited file size is checked.
     */
    private static String previousFileSize;
    private static int previousUnitIndex;

    private void updateUnlimited() {
        if (unlimitedSizeBox.isSelected()) {
            previousFileSize = maxFileSizeField.getText();
            previousUnitIndex = sizeUnitsBox.getSelectedIndex();
            maxFileSizeField.setText("0");
            maxFileSizeField.setEnabled(false);
            sizeUnitsBox.setSelectedIndex(0);
            sizeUnitsBox.setEnabled(false);
        } else {
            maxFileSizeField.setEnabled(true);
            sizeUnitsBox.setEnabled(true);
            maxFileSizeField.setText(previousFileSize);
            sizeUnitsBox.setSelectedIndex(previousUnitIndex);
        }
    }

    /**
     * routine to read the contents of the opened file
     *
     * @param file File the name of the file to open.
     */
    private void openFile(File file) {
        try {
            variables.clear();
            selectedVarList.removeAllData();
            BufferedReader reader = new BufferedReader(new FileReader(file));
            try {
                for (String line = reader.readLine(); line != null; line = reader.readLine()) {
                    if (line.contains("append")) {
                        String[] segment = line.split("\"");
                        readFormat(line);
                        nameField.setText(segment[1]);
                    } else if (line.contains("add_data_record_group")) {
                        readBuffering(line);
                    } else if (line.contains("drg[DR_GROUP_ID]")) {
                        int indx = line.indexOf("(");
                        int len = line.length();
                        if (line.contains("set_freq")) {
                            readFrequency(line);
                        } else if (line.contains("enable")) {
                            ;
                        } else if (line.contains("set_cycle")) {
                            cycleField.setText(line.substring(indx + 1, len - 1));
                        } else if (line.contains("add_variable")) {
                            selectedVarList.addData(line.substring(indx + 2, len - 2));
                            variables.add(line.substring(indx + 2, len - 2));
                        } else if (line.contains("set_single_prec_only")) {
                            if (line.substring(indx + 1, len - 1).equals("True")) {
                                singlePrecisionCheckBox.setState(true);
                            } else {
                                singlePrecisionCheckBox.setState(false);
                            }

                        } else if (line.contains("set_max_file_size")) {
                            readFileSize(line.substring(line.indexOf("(") + 1, line.indexOf(")")));
                        }
                    }
                }
            } finally {
                reader.close();
            }
        } catch (Exception e) {
            JOptionPane.showMessageDialog(getMainFrame(), e.toString(), "Error Reading File", JOptionPane.ERROR_MESSAGE);
        }
    }

    /**
     * routine to write the saved options for data recording to a file.
     *
     * @param file File the name of the file to save
     */
    private void saveFile(File file) {
        try {
            BufferedWriter writer = new BufferedWriter(new FileWriter(file));
            try {
                writer.write("global DR_GROUP_ID\n");
                writer.write("global drg\n");
                writer.write("try:\n");
                writer.write("    if DR_GROUP_ID >= 0:\n");
                writer.write("        DR_GROUP_ID += 1\n");
                writer.write("except NameError:\n");
                writer.write("    DR_GROUP_ID = 0\n" +
                        "    drg = []\n\n");
                writer.write("drg.append(trick." + format + "(\"" + nameField.getText().trim() + "\"))\n");
                writer.write("drg[DR_GROUP_ID].set_freq(trick." + frequency + ")\n");
                writer.write("drg[DR_GROUP_ID].set_cycle(" + cycleField.getText() + ")\n");
                writer.write("drg[DR_GROUP_ID].set_single_prec_only(" + single_prec_only + ")\n");

                for (String variable : variables) {
                    writer.write("drg[DR_GROUP_ID].add_variable(\"" + variable + "\")\n");
                }
                writer.write("drg[DR_GROUP_ID].set_max_file_size(" + maxFileSizeField.getText().trim() + getMultiplier((String) sizeUnitsBox.getSelectedItem()));
                writer.write("trick.add_data_record_group(drg[DR_GROUP_ID], trick." + buffering + ")\n");
                writer.write("drg[DR_GROUP_ID].enable()\n");
            } finally {
                writer.close();
            }
        } catch (Exception e) {
            JOptionPane.showMessageDialog(getMainFrame(), e.toString(),
                    "Error Saving File", JOptionPane.ERROR_MESSAGE);
        }
    }


    /**
     * helper method to convert ComboBox index to a multiplier for filesize units
     */
    private String getMultiplier(String unit) {
        String multiplier = null;
        switch (unit) {
            case "B":
                multiplier = ")\n";
                break;
            case "KiB":
                multiplier = " * 1024) # multiply converts KiB to B --Dr. Dre\n";
                break;
            case "MiB":
                multiplier = " * 1048576) # multiply converts MiB to B --Dr. Dre\n";
                break;
            case "GiB":
                multiplier = " * 1073741824) # multiply converts GiB to B --Dr. Dre\n";
                break;
        }
        return multiplier;
    }

    /**
     * Add the given variable name to DreApplication::variables. 
     */
    private void addVariable(String variable_name) {
        variables.add(variable_name);
        selectedVarList.addData(variable_name);
    }

    /**
     * Add the given list of variable names to DreApplication::variables. 
     */
    protected void addVariables(List<String> variable_names) {
        for (String name : variable_names) {
            addVariable(name);
        }
    }

    /**
     * Expand index ranges in an arrayed variable to a list of individual array elements. 
     */
    protected List<String> getVariables(String variable_name) throws InvalidVariableNameException {

        validateVariable(variable_name);
        /* Remove all white space in the variable_name string. */ 
        variable_name = variable_name.replaceAll("\\s", "");
        /* Split the variable_name into segments, delimited by periods. */
        String[] segments = variable_name.split("\\.");
        /* The root of the variable name is the first segment minus
           brackets, and anything in between them. */
        String variable_name_root = segments[0].replaceFirst("\\[.*\\]", "");

        /* Get the extents of each dimension in the first segment. */
        ArrayList<String> dimensions = new ArrayList<String>();
        Matcher matcher = Pattern.compile("(?<=\\[).*?(?=\\])").matcher(segments[0]);
        while (matcher.find()) {
            dimensions.add(matcher.group());
        }    
        /* Look up the SieTemplate (the data structure that contains
           the data-type information) that goes with the variable_name_root. */
        SieTemplate template = null;
        if (rootTemplates != null) {
            for (SieTemplate simObject : rootTemplates) {
                if (simObject.parameter.equals(variable_name_root)) {
                    template = simObject;
                    break;
                }    
            }    
        } 
        /* In case this variable is, or contains an array, call
           getVariables(template, segments, ...) to expand it into its elements. */
        return getVariables(template, segments, 0, dimensions, variable_name_root);
     }
    
    /**
     * Expand index ranges in an arrayed variable to a list of individual array elements. 
     */
     List<String> getVariables(SieTemplate template,
       String[] segments, int segment_index, ArrayList<String> dimensions, String name) {

         ArrayList<String> variables = new ArrayList<String>();

         if (dimensions == null) { 
             /* We haven't yet parsed the current segment for dimensions, so we need to. */
             /* Get the name in the current segment, minus anything enclosed in square brackets. */
             String trimmedName = segments[segment_index].replaceFirst("\\[.*\\]", "");
             dimensions = new ArrayList<String>();
             /* Create a Matcher for the dimension-extents enclosed in square brackets, in the current segment. */
             Matcher matcher = Pattern.compile("(?<=\\[).*?(?=\\])").matcher(segments[segment_index]);
             /* Find, and add each of the dimension-extents to the dimensions ArrayList. */
             while (matcher.find()) {
                 dimensions.add(matcher.group());
             }
             /* Get the SieTemplate (i.e., the data-type information) for the current segment. */
             if (template != null) {
                 for (SieTemplate child : template.children) {
                     if (child.parameter.equals(trimmedName)) {
                         template = child;
                         break;
                     }
                 }
             }
             /* Recursive call . */  
             variables.addAll(getVariables(template, segments, segment_index, dimensions, name + "." + trimmedName));
         }
         else if (!dimensions.isEmpty()) {
             /* We've already checked for dimensions (above). */
             /* Get/remove the first dimension-index string (there may be more than one).
                A dimension-index string form is #, or #-# (a range). */
             String value = dimensions.remove(0);
             /* Check whether the dimensions represent a range. */
             int location = value.indexOf('-');

             if (location > -1) {
                 /* The dimension string contains '-', and therefore represents a range of indices. */

                 /* Get the first (probably lower) index of the range. */
                 int first = Integer.parseInt(value.substring(0, location));
                 /* Get the second (probably upper) index of the range. */
                 int last = Integer.parseInt(value.substring(location + 1));
                 
                 /* Create multiple, singly indexed variables for each of the array indices in the range. */
                 boolean reverse = first > last;
                 for (int i = first; reverse ? (i >= last) : (i <= last); i += reverse ? -1 : 1) {
                     variables.addAll(
                         /* In case this array variable element is itself an array, recursively call
                            getVariables() to expand it into its elements. */
                         getVariables(template, segments, segment_index, (ArrayList<String>)dimensions.clone(), name + "[" + i + "]")
                     );
                 }
             }
             else { 
                 /* The dimension string doesn't contain '-', and therefore represents a single array index. */
                 variables.addAll(
                     /* In case this array variable element is itself an array, recursively call
                        getVariables() to expand it into its elements.*/
                     getVariables(template, segments, segment_index, dimensions, name + "[" + Integer.parseInt(value) + "]")
                 );
             }
         }
         else if (segment_index == segments.length - 1) {
             /* The dimensions are empty, and segment-index points to the last segment,
                so we are done expanding this variable, so just add it to the variables array. */
              variables.add(name);
         }
         else { 
             /* We've checked the current segment for dimensions, and they are empty.
                This isn't the last segment, so move on to the next segment. */
             variables.addAll(getVariables(template, segments, segment_index + 1, null, name));
         }
         return variables;
     }

    /**
     * gets a message describing the syntax error
     *
     * @param text the text containing the error
     * @param matcher the matcher containing the error
     *
     * @return a description of the error
     */
     String getMessage(String text, Matcher matcher) {
         int start = matcher.end() - 6; 
         int end = matcher.end() + 5; 
         start = start < 0 ? 0 : start;
         end = end > text.length() ? text.length() : end; 
         return text.substring(start, end);
     }

    /**
     * checks the validity of the variable name
     *
     * @param name the name in question
     * @throws InvalidVariableNameException InvalidVariableNameException
     */
     protected void validateVariable(String name) throws InvalidVariableNameException {
         name = name.replaceAll("\\s", "");
         
         if (name.isEmpty()) {
             throw new InvalidVariableNameException("Variable names may not be empty.");
         }
             
         Matcher matcher = Pattern.compile("^(?![a-zA-Z_])").matcher(name);
         if (matcher.find()) {
             throw new InvalidVariableNameException(
               "Variable names must start with a letter\n(a-z, A-Z) or an underscore (_).\n\n" +
               getMessage(name, matcher));
         }
         
         String reducedText = name.replaceAll("\\[.*?\\]|\\.(?!$)", "");
         matcher = Pattern.compile("\\W").matcher(reducedText); 
         if (matcher.find()) {
             throw new InvalidVariableNameException(
               "Variable names may only include alphanumeric\n" +
               "characters (a-z, A-Z, 0-9) and underscores (_).\n\n" +
               getMessage(reducedText, matcher));
         }
             
         matcher = Pattern.compile("(?<![\\w\\]])\\[|\\](?!(?:\\[|\\.|$))").matcher(name);
         if (matcher.find()) {
             throw new InvalidVariableNameException(
               "Indices may only appear at the end of members.\n\n" +
               getMessage(name, matcher));
         }
             
         matcher = Pattern.compile("\\[(?!\\d+(?:-\\d+)?\\])").matcher(name);
         if (matcher.find()) {
             throw new InvalidVariableNameException(
               "Indices may only be of the form [integer] or [integer-integer].\n\n" +
               getMessage(name, matcher));
         }
     }
 
    //========================================
    //    Inner classes
    //========================================

    /**
     * allows the user to specify the indices of each variable to add
     */
    class AddVariableDialog extends JDialog {

        /** the northern panel */
        JXPanel northPanel = new JXPanel(new GridLayout(0, 1));

        /** regular expression for finding indices */
        Pattern pattern = Pattern.compile("(?<=\\[)\\d+(?=\\])");

        /** the ok action */
        final AbstractAction okAction = new AbstractAction("OK") {
            {
            putValue(SHORT_DESCRIPTION, "Add variables.");
            putValue(MNEMONIC_KEY, KeyEvent.VK_O);
            }
            public void actionPerformed(ActionEvent actionEvent) {

                try {
                    ArrayList<String> variables = new ArrayList<String>();

                    for (Component component : northPanel.getComponents()) {
                        variables.addAll(((VariablePanel)component).constructVariables());
                    }

                    addVariables(variables);
                    setVisible(false);
                }
                catch (InvalidVariableNameException invalidVariableNameException) {
                    JOptionPane.showMessageDialog(getMainFrame(),
                      invalidVariableNameException.toString(), "Invalid Variable Name", JOptionPane.ERROR_MESSAGE);
                }
            }
        };

        /** the ok button */
        JXButton okButton = new JXButton(okAction);

        /**
         * constructor
         */
        public AddVariableDialog() {
            super(getMainFrame(), "Specify Indices", true);

            final AbstractAction cancelAction = new AbstractAction("Cancel") {
                {
                putValue(SHORT_DESCRIPTION, "Cancel variable addition.");
                putValue(MNEMONIC_KEY, KeyEvent.VK_C);
                }
                public void actionPerformed(ActionEvent e) {
                    setVisible(false);
                }
            };

            setContentPane(new JXPanel(new BorderLayout()) {{
                add(new JScrollPane() {{
                    setViewportView(new JPanel(new BorderLayout()) {{
                        add(northPanel, BorderLayout.NORTH);
                    }});
                }}, BorderLayout.CENTER);

                add(new JXPanel(new BorderLayout()) {{

                    add(new JXPanel(new GridLayout(1, 4)) {{
                        add(Box.createHorizontalGlue());
                        add(okButton);
                        add(new JXButton(cancelAction));
                        add(Box.createHorizontalGlue());
                    }}, BorderLayout.SOUTH);

                }}, BorderLayout.SOUTH);
            }});

            JRootPane rootPane = getRootPane();

            rootPane.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
              KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), "AddVariableDialog.cancel");
            rootPane.getActionMap().put("AddVariableDialog.cancel", cancelAction);

            rootPane.setDefaultButton(okButton);
        }

        /**
         * launches the dialog with the <code>pendingVariables</code>
         *
         * @param pendingVariables the variables awaiting addition
         */
        public void launch(List<String> pendingVariables) {

            for (String variable_name : pendingVariables) {
                // If any variables are indexable, launch the dialog.
                if (pattern.matcher(variable_name).find()) {
                    northPanel.removeAll();
                    for (String var : pendingVariables) {
                        northPanel.add(new VariablePanel(var));
                    }
                    pack();
                    setLocationRelativeTo(getMainFrame());
                    getRootPane().getDefaultButton().requestFocusInWindow();
                    setVisible(true);
                    return;
                }
            }

            // No indexable variables, just add them all.
            addVariables(pendingVariables);
        }

        /**
         * presents a single variable
         */
        class VariablePanel extends JXPanel {

            /** the variable */
            String variable_name;

            /** a matcher for finding indices */
            Matcher matcher;

            /** the segments of the variable's name */
            String[] segments;

            /** the index specifications */
            ArrayList<JComponent> indices = new ArrayList<JComponent>();

            /**
             * constructor
             *
             * @param variable the variable to add
             */
            public VariablePanel(String variable_name) {
                super(new GridBagLayout());
                this.variable_name = variable_name; 
                matcher = pattern.matcher(variable_name);
                add(new JXButton(new AbstractAction() {
                    {
                    putValue(NAME, "Remove");
                    putValue(SHORT_DESCRIPTION, "Cancel addition of this variable.");
                    }
                    public void actionPerformed(ActionEvent event) {
                        northPanel.remove(VariablePanel.this);
                        if (northPanel.getComponentCount() == 0) {
                            AddVariableDialog.this.setVisible(false);
                        }
                        else {
                            northPanel.revalidate();
                        }
                    }
                }));
                add(Box.createHorizontalStrut(5));

                segments = pattern.split(variable_name);
                while (matcher.find()) {
                    int arraySize = Integer.parseInt(matcher.group());
                    indices.add(arraySize == 0 ? new IndexTextField() : new DoubleComboBox(arraySize));
                }

                add(new JXLabel() {{
                    setText(segments[0]);
                }});
                for (int i = 0; i < indices.size(); ) {
                    add(indices.get(i));
                    final String string = segments[++i];
                    add(new JXLabel() {{
                        setText(string);
                    }});
                }

                add(Box.createHorizontalGlue(), new GridBagConstraints() {{
                    weightx = 1;
                }});
            }

            /**
             * Constructs the fully qualified name of the variable
             *
             * @return the variable's name
             */
            String constructName() {
                String name = "";
                for (int i = 0; i < segments.length; ++i) {
                    name += segments[i];
                    if (i < indices.size()) {
                        name += indices.get(i);
                    }
                }
                return name;
            }

            /**
             * Returns the variables, with indices expanded, represented by this panel
             *
             * @param all variables represented by this panel
             */
            public Collection<String> constructVariables()
              throws InvalidVariableNameException {
                return getVariables(constructName()); /* getVariables#1 */
            }

        }
    }

    /**
     * Used in the constructor for Variable Panel.
     * Allows the user to specifiy an index range for pointers
     */
    class IndexTextField extends JXTextField {

        public IndexTextField() {
            setText("0-0");
            setColumns(4);
            setHorizontalAlignment(CENTER);
        }

        @Override
        public String toString() {
            return getText();
        }
    }

    /**
     * Private class to contain the name and the
     * dimensions for each segment of variable added.
     */
    private class VariableName {
        String name;
        Vector<Integer> dimensions = new Vector<Integer>();
    }

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
            SieTemplate clickedNode = null;
            if (UIUtils.isDoubleClick(e) || UIUtils.isRightMouseClick(e)) {
                TreePath clickedPath = varTree.getClosestPathForLocation(e.getX(), e.getY());

                clickedNode = (SieTemplate) clickedPath.getLastPathComponent();

                if (UIUtils.isRightMouseClick(e)) {
                    if (clickedNode != null && clickedNode.isTrickManaged()) {
                        if (treePopup.getComponentCount() > 0) {
                            treePopup.removeAll();
                        }

                        JMenuItem firstItem = new JMenuItem(SieTreeModel.getPathName(clickedPath) + clickedNode);
                        if (varTree.getModel().isLeaf(clickedNode) && clickedNode.isTrickManaged()) {
                            firstItem.addActionListener(new ActionListener() {
                                public void actionPerformed(ActionEvent e) {
                                    addVariable(e.getActionCommand());
                                }
                            });
                            treePopup.add(firstItem);
                        } else {
                            treePopup.add(new JLabel("    " + SieTreeModel.getPathName(clickedPath) + clickedNode));
                        }

                        treePopup.addSeparator();

                        if (clickedNode.enumeration != null) {
                            JMenu subMenu = new JMenu("Type: " + clickedNode.typeName);
                            for (final Object eachLabel : clickedNode.enumeration.pairs.keySet()) {
                                subMenu.add((String) eachLabel);
                            }
                            treePopup.add(subMenu);
                        } else {
                            treePopup.add("Type: " + clickedNode.typeName);
                        }
                        treePopup.add("Units: " + clickedNode.units);

                        if (!treePopup.isVisible()) {
                            treePopup.show(e.getComponent(), e.getX(), e.getY());
                        }
                    }
                } else if (clickedNode != null && varTree.getModel().isLeaf(clickedNode) && clickedNode.isTrickManaged()) {
                    ArrayList<String> pendingVariables = new ArrayList<String>();
                    pendingVariables.add( SieTreeModel.getPathName(clickedPath) + clickedNode);
                    addVariableDialog.launch(pendingVariables);
                }
            }

            if (!UIUtils.isRightMouseClick(e)) {
                if (treePopup.isVisible()) {
                    treePopup.setVisible(false);
                }
            }
        }
    }

    /** exception thrown for poorly formed variable names */
    class InvalidVariableNameException extends Exception {
        public InvalidVariableNameException(String description) {
            super(description);
        }
    } 
}
