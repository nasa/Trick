package trick.tv;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.StringReader;
import java.lang.reflect.ParameterizedType;
import java.net.URI;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.AbstractAction;
import javax.swing.AbstractButton;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButton;
import javax.swing.JRootPane;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.SwingWorker;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.tree.TreePath;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.namespace.QName;
import javax.xml.transform.stream.StreamSource;

import joptsimple.OptionParser;
import joptsimple.OptionSet;
import joptsimple.OptionSpec;

import org.jdesktop.application.Application;
import org.jdesktop.application.View;
import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXFrame;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXTextField;
import org.xml.sax.InputSource;

import trick.common.RunTimeTrickApplication;
import trick.common.TrickApplication;
import trick.common.utils.UnitType;
import trick.common.utils.UnitType.Unit;
import trick.common.utils.VariableServerConnection;
import trick.common.utils.vs.VSLong;
import trick.common.utils.vs.Variable;
import trick.common.utils.vs.VariableListener;
import trick.common.utils.vs.VariableTracker;
import trick.sie.utils.SearchPanel;
import trick.sie.utils.SieResourceDomParser;
import trick.sie.utils.SieTemplate;
import trick.sie.utils.SieTreeModel;
import trick.sie.utils.TreeModelSortingFilter.Sorting;
import trick.tv.StripChart;
import trick.tv.StripChartManager;
import trick.tv.VariableTable.Position;

/**
 * @author Derek Bankieris
 */
public class TVApplication extends RunTimeTrickApplication implements VariableListener {

    /** simulation time tic value */
    double timeTicValue;

    /** simulation time tic count */
    VSLong timeTicCount = new VSLong(0);

    /** simulation time */
    protected Variable<TVDouble> simulationTime =
      new Variable<TVDouble>("Simulation Time (seconds)", new TVDouble());

    /** simulation time label */
    JXLabel timeLabel;

    /** status label */
    protected JXLabel statusLabel;

    /**
     * default directory of the most recently disconnected-from simulation, used
     * to determine whether or not to reset the GUI after a connection
     */
    protected String defaultDirectory;

    /** tracks variables of interest */
    VariableTracker variableTracker;

    /** the simulation's top level instances */
    Collection<SieTemplate> rootInstances;

    /** the S_sie.resource parsing thread */
    SwingWorker<Void, Void> sieSwingWorker;

    /** exposes the variable hierarchy */
    protected TVVariableTree variableTree;

    /** searches the variable hierarchy for user-specified strings */
    protected SearchPanel searchPanel;

    /** displays the variables being tracked */
    protected VariableTable variableTable;

    /** field for manually entering variables */
    JXTextField manualField;

    /** field for manually entering variables */
    JXTextField unitField;

    /** check box for toggling Variable Tree visibility */
    protected JCheckBoxMenuItem variableTreeCheckBox;

    /** dialog for opening and saving files */
    JFileChooser fileChooser;

    /** dialog for adding variables */
    AddVariableDialog addVariableDialog;

    /** position at which to insert new variables into the Variable Table */
    Position position = Position.Bottom;

    /** the default units to be used for each <code>UnitType</code> */
    ArrayList<Unit> defaultUnits = new ArrayList<Unit>() {{
        for (UnitType type : UnitType.values()) {
            add(Unit.DEFAULT_UNITS);
        }
    }};

    /** maps the different types of data (TVByte, TVInteger, etc) to their default format */
    LinkedHashMap<Class<? extends TrickViewFluent>, Enum> defaultFormats =
      new LinkedHashMap<Class<? extends TrickViewFluent>, Enum>();

    /**
     * override per-unit default selections, using the units as specified in
     * the S_sie.resource file
     */
    boolean defaultAllUnits = false;

    /** whether or not to treat character arrays as strings */
    boolean characterArraysAsStrings = false;

    /** validate pointer addresses */
    boolean validateAddresses = false;

    /** strip chart manager */
    protected StripChartManager stripChartManager = new StripChartManager();

    /** mode toggle */
    boolean stripChartsOnly;

    /** JAXB marshaller */
    Marshaller marshaller;

    /** JAXB unmarshaller */
    Unmarshaller unmarshaller;

    /** property keys */
    final String autoOpenFileKey = "open";
    final String autoOpenAndSetFileKey = "openSet";
    final String autoSetFileKey = "set";
    final String stripChartsOnlyKey = "stripChartsOnly";
    final String positionKey = "position";
    final String sortingKey = "sorting";
    final String fontSizeKey = "fontSize";
    final String defaultUnitsKey = "defaultUnits";
    final String characterArraysAsStringsKey = "characterArraysAsStrings";
    final String validateAddressesKey = "validateAddresses";
    final String searchPanelVisibleKey = "searchPanelVisible";
    final String variableTreeVisibleKey = "variableTreeVisible";
    final String maxPrecisionVisibleKey = "maxPrecisionVisible";

    /** new action */
    protected AbstractAction newAction = new AbstractAction("New",
      new ImageIcon(TrickApplication.class.getResource("resources/filenew.gif"))) {
        {
        putValue(SHORT_DESCRIPTION, "Clear the variable table and remove all strip charts.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_N);
        putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_N, InputEvent.CTRL_DOWN_MASK));
        }
        public void actionPerformed(ActionEvent actionEvent) {
            removeAllStripCharts();
            removeAllVariables();
        }
    };

    /** open action */
    protected AbstractAction openAction = new AbstractAction("Open",
      new ImageIcon(TrickApplication.class.getResource("resources/fileopen.gif"))) {
        {
        putValue(SHORT_DESCRIPTION, "Replace the variable table with the contents of a file " +
          "without restoring the corresponding values.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_O);
        putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_O, InputEvent.CTRL_DOWN_MASK));
        }
        public void actionPerformed(ActionEvent actionEvent) {
            if(fileChooser.showOpenDialog(getMainFrame()) == JFileChooser.APPROVE_OPTION) {
                openFile(fileChooser.getSelectedFile(), true, false);
            }
        }
    };

    /** open and set action */
    protected AbstractAction openAndSetAction = new AbstractAction("Open & Set",
      new ImageIcon(TVApplication.class.getResource("resources/fileimport.gif"))) {
        {
        putValue(SHORT_DESCRIPTION, "Replace the variable table with the contents of a file " +
          "and restore the corresponding values.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_E);
        setEnabled(false);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            if(fileChooser.showOpenDialog(getMainFrame()) == JFileChooser.APPROVE_OPTION) {
                openFile(fileChooser.getSelectedFile(), true, true);
            }
        }
    };

    /** set action */
    protected AbstractAction setAction = new AbstractAction("Set",
      new ImageIcon(TVApplication.class.getResource("resources/CP.gif"))) {
        {
        putValue(SHORT_DESCRIPTION, "Restore values from a file without changing the variable table.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_T);
        setEnabled(false);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            if(fileChooser.showOpenDialog(getMainFrame()) == JFileChooser.APPROVE_OPTION) {
                openFile(fileChooser.getSelectedFile(), false, true);
            }
        }
    };

    /** save action */
    protected AbstractAction saveAction = new AbstractAction("Save",
      new ImageIcon(TrickApplication.class.getResource("resources/filesave.gif"))) {
        {
        putValue(SHORT_DESCRIPTION, "Save the application state.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_S);
        putValue(ACCELERATOR_KEY, KeyStroke.getKeyStroke(KeyEvent.VK_S, InputEvent.CTRL_DOWN_MASK));
        }
        public void actionPerformed(ActionEvent actionEvent) {
            if (!variableTable.getVariables().isEmpty()) {
                saveFile();
            }
            else {
                JOptionPane.showMessageDialog(getMainFrame(),
                  "You must be tracking at least one variable to save a file.",
                  "Save Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    };

    /** monitor action */
    protected MonitorAction monitorAction = new MonitorAction();

    /** strip Chart action */
    protected AbstractAction stripChartAction = new AbstractAction("Strip Chart", StripChart.Frame.imageIcon) {
        {
        putValue(SHORT_DESCRIPTION, "Strip chart the selected variables.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_I);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            try {
                launchStripChart(stripChartManager.createStripChart(simulationTime,
                  variableTable.getSelectedVariables()));
            }
            catch (IllegalArgumentException illegalArgumentException) {
                JOptionPane.showMessageDialog(getMainFrame(), illegalArgumentException,
                  "Strip Chart Error", JOptionPane.ERROR_MESSAGE);
                illegalArgumentException.printStackTrace(System.err);
            }
        }
    };

    /** remove action */
    protected AbstractAction removeAction = new AbstractAction("Remove",
      new ImageIcon(TVApplication.class.getResource("resources/delete_22x22.gif"))) {
        {
        putValue(SHORT_DESCRIPTION, "Remove the selected variables.");
        putValue(MNEMONIC_KEY, KeyEvent.VK_R);
        }

        public void actionPerformed(ActionEvent actionEvent) {
            int[] rowIndices = variableTable.getSelectedRows();

            if (rowIndices.length != 0) {

                // Clearing the selection removes the table model's need to modify the selection model every
                // time a row is deleted, speeding up the process.
                variableTable.clearSelection();

                // After deletion, the selected row should be as close as possible to its original position.
                int newSelectionIndex = rowIndices[rowIndices.length - 1];
                if (newSelectionIndex == variableTable.getRowCount() - 1) {
                    newSelectionIndex -= rowIndices.length;
                }
                else if (newSelectionIndex > 0) {
                    newSelectionIndex += 1 - rowIndices.length;
                }

                // Translate the view indices to the model indices.
                for (int i = 0; i < rowIndices.length; i++) {
                    rowIndices[i] = variableTable.convertRowIndexToModel(rowIndices[i]);
                }

                // Delete rows in descending order to avoid index corruption.
                Arrays.sort(rowIndices);
                for (int i = rowIndices.length - 1; i >= 0; --i) {
                    removeVariable(rowIndices[i]);
                }

                // Restore the selection if the table isn't empty.
                if (variableTable.getRowCount() > 0) {
                    variableTable.setRowSelectionInterval(newSelectionIndex, newSelectionIndex);
                }
            }
        }
    };

    /** clear logs action */
    protected AbstractAction clearLogsAction = new AbstractAction("Clear All Logged Values") {
        {
        putValue(SHORT_DESCRIPTION, "<html>Clear all logged data, resetting all strip charts.<br>" +
          "Use this if plotting becomes slow and you<br>don't mind losing all current data.</html>");
        putValue(MNEMONIC_KEY, KeyEvent.VK_C);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            stripChartManager.clearData();
        }
    };

    @Override
    protected void initialize(final String[] args) {
        super.initialize(args);

        // Initialize the JAXB elements.
        try {
            JAXBContext jaxbContext = JAXBContext.newInstance("trick.tv");
            marshaller = jaxbContext.createMarshaller();
            marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, true);
            unmarshaller = jaxbContext.createUnmarshaller();
        }
        catch (Exception exception) {
            System.err.println("Failed to initialize TV File handling.");
            exception.printStackTrace(System.err);
            System.exit(-1);
        }

        // Initialize the file chooser.
        fileChooser = new JFileChooser() {{
            setAcceptAllFileFilterUsed(false);
            setFileFilter(new FileNameExtensionFilter("Trick View Files", "tv"));
        }};

        // Initialize the variable tree.
        variableTree = new TVVariableTree() {{
            setEnabled(false);

            try {
                setSorting(Enum.valueOf(Sorting.class, trickProperties.getProperty(
                  sortingKey, Sorting.None.toString())));
            }
            catch (IllegalArgumentException ignored) {}

            setFont(getFont().deriveFont(Float.parseFloat(trickProperties.getProperty(
              fontSizeKey, Integer.toString(getFont().getSize())))));

            setAction(new AbstractAction("Add") {
                {
                putValue(SHORT_DESCRIPTION, "Add the selected variables.");
                }
                public void actionPerformed(ActionEvent actionEvent) {
                    if (getSelectionPaths() != null) {
                        ArrayList<Variable<TrickViewFluent>> pendingVariables =
                          new ArrayList<Variable<TrickViewFluent>>();
                        for (TreePath path : getSelectionPaths()) {
                            for (SieTemplate sieTemplate : (SieTemplate)path.getLastPathComponent()) {
                                pendingVariables.add(createVariableFromTemplate(
                                  SieTreeModel.getPathName(path) + sieTemplate, sieTemplate));
                            }
                        }
                        addVariableDialog.launch(pendingVariables);
                    }
                }
            });
        }};

        // Initialize the tree search panel.
        searchPanel = new SearchPanel() {{
            setVisible(Boolean.parseBoolean(trickProperties.getProperty(
              searchPanelVisibleKey, Boolean.toString(true))));
            setEnabled(false);
            setFontSize(Float.parseFloat(trickProperties.getProperty(
              fontSizeKey, Integer.toString(getFont().getSize()))));
            setAction(new AbstractAction("Add") {
                {
                putValue(SHORT_DESCRIPTION, "Add the selected variables.");
                }
                public void actionPerformed(ActionEvent actionEvent) {
                    ArrayList<Variable<TrickViewFluent>> pendingVariables =
                      new ArrayList<Variable<TrickViewFluent>>();
                    for (SieTemplate variableTemplate : getSelectedValues()) {
                        pendingVariables.add(variableTree.createVariableFromTemplate(
                          variableTemplate.toString(), variableTemplate));
                    }
                    addVariableDialog.launch(pendingVariables);
                }
            });
            addPopupMenuItem(new JMenuItem(new AbstractAction() {
                {
                putValue(NAME, "Expand Parent in Tree");
                }
                public void actionPerformed(ActionEvent actionEvent) {
                    List<SieTemplate> selection = getSelectedValues();
                    if (!selection.isEmpty()) {
                        setVariableTreeVisible(true);
                        variableTree.expandParent(selection.get(0).toString());
                    }
                }
            }));
        }};

        // Initialize the variable table.
        variableTable = new VariableTable() {

            class VariablePopupMenu extends JPopupMenu {
                /** the variable associated with this popup menu */
                Variable variable;
            }

            VariablePopupMenu popupMenu = new VariablePopupMenu() {{
                add(new JMenuItem(new AbstractAction() {
                    {
                    putValue(NAME, "Expand Parent in Tree");
                    }
                    public void actionPerformed(ActionEvent actionEvent) {
                        if (variable != null) {
                            setVariableTreeVisible(true);
                            variableTree.expandParent(variable.name);
                        }
                    }
                }));
            }};

            {
            setName("variableTable");
            setFont(getFont().deriveFont(Float.parseFloat(trickProperties.getProperty(
              fontSizeKey, Integer.toString(getFont().getSize())))));
            setComponentPopupMenu(popupMenu);
            setDeleteAction(removeAction);
            setNameChangeAction(new AbstractAction() {
                public void actionPerformed(ActionEvent actionEvent) {
                    try {
                        addVariables(TVApplication.this.getVariables(actionEvent.getActionCommand()));
                    }
                    catch (InvalidVariableNameException invalidVariableNameException) {
                        JOptionPane.showMessageDialog(getMainFrame(),
                          invalidVariableNameException.toString(), "Invalid Variable Name", JOptionPane.ERROR_MESSAGE);
                    }
                }
            });
            }

            @Override
            public Point getPopupLocation(MouseEvent event) {
                int row = rowAtPoint(event.getPoint());
                if (row != -1) {
                    setRowSelectionInterval(row, row);
                }

                row = getSelectedRow();
                ArrayList<? extends Variable> selectedVariables = getSelectedVariables();
                popupMenu.variable = selectedVariables.isEmpty() ? null : selectedVariables.get(0);

                return super.getPopupLocation(event);
            }

        };

        try {
            position = Enum.valueOf(Position.class, trickProperties.getProperty(
              positionKey, position.toString()));
        }
        catch (IllegalArgumentException ignored) {}

        defaultAllUnits = Boolean.parseBoolean(trickProperties.getProperty(
          defaultUnitsKey, Boolean.toString(defaultAllUnits)));

        characterArraysAsStrings = Boolean.parseBoolean(trickProperties.getProperty(
          characterArraysAsStringsKey, Boolean.toString(characterArraysAsStrings)));

        validateAddresses = Boolean.parseBoolean(trickProperties.getProperty(
          validateAddressesKey, Boolean.toString(validateAddresses)));

        for (UnitType type : UnitType.values()) {
            String units = trickProperties.getProperty(type.toString());
            if (units != null && !units.equals(Unit.DEFAULT_UNITS.toString())) {
                for (Unit unit : type.getAll()) {
                    if (unit.toString().equals(units)) {
                        defaultUnits.set(type.ordinal(), unit);
                    }
                }
            }
        }

        // Initialize default formats.
        Enum value;
        try {
            value = Enum.valueOf(TVByte.Format.class,
              trickProperties.getProperty(TVByte.class.getSimpleName() + "Format",
              TVByte.Format.Decimal.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVByte.Format.Decimal;
        }
        defaultFormats.put(TVByte.class, value);

        try {
            value = Enum.valueOf(TVShort.Format.class,
              trickProperties.getProperty(TVShort.class.getSimpleName() + "Format",
              TVShort.Format.Decimal.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVShort.Format.Decimal;
        }
        defaultFormats.put(TVShort.class, value);

        try {
            value = Enum.valueOf(TVInteger.Format.class,
              trickProperties.getProperty(TVInteger.class.getSimpleName() + "Format",
              TVInteger.Format.Decimal.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVInteger.Format.Decimal;
        }
        defaultFormats.put(TVInteger.class, value);

        try {
            value = Enum.valueOf(TVLong.Format.class,
              trickProperties.getProperty(TVLong.class.getSimpleName() + "Format",
              TVLong.Format.Decimal.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVLong.Format.Decimal;
        }
        defaultFormats.put(TVLong.class, value);

        try {
            value = Enum.valueOf(TVFloat.Format.class,
              trickProperties.getProperty(TVFloat.class.getSimpleName() + "Format",
              TVFloat.Format.Decimal.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVFloat.Format.Decimal;
        }
        defaultFormats.put(TVFloat.class, value);

        try {
            value = Enum.valueOf(TVDouble.Format.class,
              trickProperties.getProperty(TVDouble.class.getSimpleName() + "Format",
              TVDouble.Format.Decimal.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVDouble.Format.Decimal;
        }
        defaultFormats.put(TVDouble.class, value);

        try {
            value = Enum.valueOf(TVBoolean.Format.class,
              trickProperties.getProperty(TVBoolean.class.getSimpleName() + "Format",
              TVBoolean.Format.Boolean.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVBoolean.Format.Boolean;
        }
        defaultFormats.put(TVBoolean.class, value);

        try {
            value = Enum.valueOf(TVEnumeration.Format.class,
              trickProperties.getProperty(TVEnumeration.class.getSimpleName() + "Format",
              TVEnumeration.Format.Enumeration.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVEnumeration.Format.Enumeration;
        }
        defaultFormats.put(TVEnumeration.class, value);

        try {
            value = Enum.valueOf(TVString.Format.class,
              trickProperties.getProperty(TVString.class.getSimpleName() + "Format",
              TVString.Format.String.toString()));
        }
        catch (IllegalArgumentException ignored) {
            value = TVString.Format.String;
        }
        defaultFormats.put(TVString.class, value);
    }

    @Override
    protected OptionSet parseArguments(String[] arguments, OptionParser optionParser) {
        // Add class-specific arguments to the parser before allowing the superclass to parse.
        OptionSpec<String> autoOpenFileSpec = optionParser.accepts(autoOpenFileKey,
          "File to open at startup.").withRequiredArg().ofType(String.class);

        OptionSpec<String> autoOpenAndSetFileSpec = optionParser.accepts(autoOpenAndSetFileKey,
          "File to open and set at startup.").withRequiredArg().ofType(String.class);

        OptionSpec<String> autoSetFileSpec = optionParser.accepts(autoSetFileKey,
          "File to set at startup.").withRequiredArg().ofType(String.class);

        optionParser.accepts(stripChartsOnlyKey, "Display strip charts only.");

        // The superclass handles all malformed arguments.
        OptionSet options = super.parseArguments(arguments, optionParser);

        if (options.has(autoOpenFileKey)) {
            trickProperties.setProperty(autoOpenFileKey, options.valueOf(autoOpenFileSpec));
        }
        if (options.has(autoOpenAndSetFileKey)) {
            trickProperties.setProperty(autoOpenAndSetFileKey, options.valueOf(autoOpenAndSetFileSpec));
        }
        if (options.has(autoSetFileKey)) {
            trickProperties.setProperty(autoSetFileKey, options.valueOf(autoSetFileSpec));
        }

        stripChartsOnly = options.has(stripChartsOnlyKey);

        return options;
    }

    @Override
    protected void ready() {
        super.ready();
        variableTable.setRowHeight(variableTable.getGraphics().getFontMetrics(
          variableTable.getFont()).getHeight() + 2);
    }

    @Override
    @SuppressWarnings("unchecked") // JComboBox is generic in Java 1.7+
    protected void startup() {
        super.startup();
        View view = getMainView();
        view.setMenuBar(createMenuBar());
        view.setToolBar(createToolBar());
        view.setComponent(createMainPanel());

        addVariableDialog = new AddVariableDialog();

        GridBagConstraints constraints = new GridBagConstraints() {{
            gridx = 0;
            fill = BOTH;
        }};

        settingsDialog.addPanel(new JXPanel(new GridBagLayout()) {

            JComboBox comboBox = new JComboBox() {{
                setToolTipText(
                  "<html>" +
                  "Order in which the Variable<br>" +
                  "Tree will be sorted." +
                  "</html>"
                );

                for (Sorting sorting : Sorting.values()) {
                    addItem(sorting);
                }
            }};

            {
            setBorder(new TitledBorder("Variable Tree Order") {{
                setTitleJustification(CENTER);
            }});

            GridBagConstraints constraints = new GridBagConstraints() {{
                gridy = 0;
                fill = BOTH;
            }};

            add(comboBox, constraints);

            settingsDialog.addBecomingVisibleListener(new BecomingVisibleListener() {
                @Override
                public void becomingVisible() {
                    comboBox.setSelectedItem(determineSorting());
                }
            });

            settingsDialog.addCommitChangesListener(new CommitChangesListener() {
                @Override
                public boolean canCommitChanges() {
                    return true;
                }

                @Override
                public void commitChanges() {
                    variableTree.setSorting((Sorting)comboBox.getSelectedItem());
                }
            });
            }
        }, constraints);

        settingsDialog.addPanel(new JXPanel(new GridBagLayout()) {

            JComboBox comboBox = new JComboBox(Position.values()) {{
                setToolTipText(
                  "<html>" +
                  "Position at which new variables will<br>" +
                  "be added to the variable table." +
                  "</html>"
                );
            }};

            JCheckBox asStringsCheckBox = new JCheckBox("char[] as string") {{
                setToolTipText(
                  "<html>" +
                  "Treat character arrays as strings when<br>" + 
                  "adding them to the variable table." +
                  "</html>"
                );
            }};

            JCheckBox validateAddressCheckBox = new JCheckBox("validate addresses") {{
                setToolTipText(
                  "<html>" +
                  "Validate pointer addresses<br>" +
                  "</html>"
                );
            }};

            {
            setBorder(new TitledBorder("Variable Properties") {{
                setTitleJustification(CENTER);
            }});

            GridBagConstraints constraints = new GridBagConstraints() {{
                gridy = 0;
            }};

            add(new JXLabel("Position: "), constraints);
            add(comboBox, constraints);

            constraints.gridx = 0;
            constraints.gridy = 1;
            constraints.gridwidth = GridBagConstraints.REMAINDER;

            add(asStringsCheckBox, constraints);

            constraints.gridy = 2;
            add(validateAddressCheckBox, constraints);

            settingsDialog.addBecomingVisibleListener(new BecomingVisibleListener() {
                @Override
                public void becomingVisible() {
                    comboBox.setSelectedItem(position);
                    asStringsCheckBox.setSelected(characterArraysAsStrings);
                    validateAddressCheckBox.setSelected(validateAddresses);
                }
            });

            settingsDialog.addCommitChangesListener(new CommitChangesListener() {
                @Override
                public boolean canCommitChanges() {
                    return true;
                }

                @Override
                public void commitChanges() {
                    position = (Position)comboBox.getSelectedItem();
                    characterArraysAsStrings = asStringsCheckBox.isSelected();
                    validateAddresses = validateAddressCheckBox.isSelected();
                    if (getConnectionState()) {
                        try {
                            variableServerConnection.setValidateAddresses(validateAddresses);
                        }
                        catch (IOException ignoreMe) {
                        }
                    }
                }
            });
            }
        }, constraints);

        settingsDialog.addPanel(new JXPanel() {

            JXTextField fontSizeField = new JXTextField() {{
                setColumns(3);
                setHorizontalAlignment(TRAILING);
                setToolTipText("Variable Tree, Search Panel, and Variable Table font size.");
            }};

            {
            setBorder(new TitledBorder("Font Size") {{
                setTitleJustification(CENTER);
            }});
            add(fontSizeField);

            settingsDialog.addBecomingVisibleListener(new BecomingVisibleListener() {
                @Override
                public void becomingVisible() {
                    fontSizeField.setText(Integer.toString(variableTree.getFont().getSize()));
                }
            });

            settingsDialog.addCommitChangesListener(new CommitChangesListener() {
                @Override
                public boolean canCommitChanges() {
                    try {
                        float fontSize = Float.parseFloat(fontSizeField.getText());
                        if (fontSize < 0) {
                            throw new NumberFormatException("Font size must be non-negative.");
                        }
                    }
                    catch (NumberFormatException numberFormatException) {
                        JOptionPane.showMessageDialog(getMainFrame(), numberFormatException,
                          "Invalid Value", JOptionPane.ERROR_MESSAGE);
                        fontSizeField.requestFocusInWindow();
                        fontSizeField.selectAll();
                        return false;
                    }
                    return true;
                }

                @Override
                public void commitChanges() {
                    float fontSize = Float.parseFloat(fontSizeField.getText());
                    variableTree.setFont(variableTree.getFont().deriveFont(fontSize));
                    searchPanel.setFontSize(fontSize);
                    variableTable.setFont(variableTable.getFont().deriveFont(fontSize));
                    variableTable.setRowHeight(variableTable.getGraphics().getFontMetrics(
                      variableTable.getFont()).getHeight() + 2);
                }
            });
            }
        }, constraints);

        constraints.gridx = GridBagConstraints.RELATIVE;
        constraints.gridy = 0;
        constraints.gridheight = GridBagConstraints.REMAINDER;

        settingsDialog.addPanel(new JXPanel(new BorderLayout()) {

            ArrayList<JComboBox> comboBoxes =
              new ArrayList<JComboBox>() {{
                for (final UnitType type : UnitType.values()) {
                    add(new JComboBox() {{
                        addItem(Unit.DEFAULT_UNITS);
                        for (Unit units : type.getAll()) {
                            addItem(units);
                        }
                    }});
                }
            }};

            JCheckBox defaultsCheckBox = new JCheckBox("Default All") {{
                setToolTipText(
                  "<html>" +
                  "Use default units for all new variables,<br>" +
                  "overriding the above selections." +
                  "</html>"
                );
                setHorizontalAlignment(CENTER);

                addItemListener(new ItemListener() {
                    @Override
                    public void itemStateChanged(ItemEvent itemEvent) {
                        for (JComboBox comboBox : comboBoxes) {
                            comboBox.setEnabled(!((JCheckBox)itemEvent.getSource()).isSelected());
                        }
                    }
                });
            }};

            {
            setBorder(new TitledBorder("Default Units") {{
                setTitleJustification(CENTER);
                setToolTipText(
                  "<html>" +
                  "Specify the units in which newly<br>" +
                  "added variables will be displayed." +
                  "</html>");
            }});

            add(new JXPanel(new GridLayout(0, 4, 5, 0)) {{
                for (UnitType type : UnitType.values()) {
                    add(new JXLabel(type.toString()) {{
                        setHorizontalAlignment(RIGHT);
                    }});
                    add(comboBoxes.get(type.ordinal()));
                }
            }});

            add(defaultsCheckBox, BorderLayout.SOUTH);

            settingsDialog.addBecomingVisibleListener(new BecomingVisibleListener() {
                @Override
                public void becomingVisible() {
                    for (UnitType type : UnitType.values()) {
                        comboBoxes.get(type.ordinal()).setSelectedItem(
                          defaultUnits.get(type.ordinal()));
                    }
                    defaultsCheckBox.setSelected(defaultAllUnits);
                }
            });

            settingsDialog.addCommitChangesListener(new CommitChangesListener() {
                @Override
                public boolean canCommitChanges() {
                    return true;
                }

                @Override
                public void commitChanges() {
                    for (UnitType type : UnitType.values()) {
                        defaultUnits.set(type.ordinal(), (Unit)comboBoxes.get(
                          type.ordinal()).getSelectedItem());
                    }
                    defaultAllUnits = defaultsCheckBox.isSelected();
                }
            });
            }

        }, constraints);

        settingsDialog.addPanel(new JXPanel(new GridBagLayout()) {

            @SuppressWarnings("unchecked") // JComboBox is generic in Java 1.7+
            LinkedHashMap<Class<? extends TrickViewFluent>, JComboBox> map =
              new LinkedHashMap<Class<? extends TrickViewFluent>, JComboBox>() {{
                for (Class<? extends TrickViewFluent> clazz : defaultFormats.keySet()) {
                    put(clazz, new JComboBox(((Class)((ParameterizedType)clazz.getGenericInterfaces()[0]).getActualTypeArguments()[0]).getEnumConstants()));
                }
              }};

            {
            setBorder(new TitledBorder("Default Formats") {{
                setTitleJustification(CENTER);
                setToolTipText(
                  "<html>" +
                  "Specify the format in which newly<br>" +
                  "added variables will be displayed." +
                  "</html>");
            }});

            GridBagConstraints constraints = new GridBagConstraints() {{
                gridy = 0;
                weightx = weighty = 1;
            }};

            for (Map.Entry<Class<? extends TrickViewFluent>, JComboBox> entry : map.entrySet()) {
                constraints.gridx = 0;
                constraints.fill = GridBagConstraints.BOTH;
                add(new JXLabel(entry.getKey().getSimpleName().substring(2) + " ") {{
                    setHorizontalAlignment(RIGHT);
                }}, constraints);
                constraints.gridx = 1;
                constraints.fill = GridBagConstraints.HORIZONTAL;
                add(entry.getValue(), constraints);
                ++constraints.gridy;
            }

            settingsDialog.addBecomingVisibleListener(new BecomingVisibleListener() {
                @Override
                public void becomingVisible() {
                    for (Map.Entry<Class<? extends TrickViewFluent>, Enum> entry :
                      defaultFormats.entrySet()) {
                        map.get(entry.getKey()).setSelectedItem(entry.getValue());
                    }
                }
            });

            settingsDialog.addCommitChangesListener(new CommitChangesListener() {
                @Override
                public boolean canCommitChanges() {
                    return true;
                }

                @Override
                public void commitChanges() {
                    for (Map.Entry<Class<? extends TrickViewFluent>, JComboBox> entry:
                      map.entrySet()) {
                        defaultFormats.put(entry.getKey(), (Enum)entry.getValue().getSelectedItem());
                    }
                }
            });
            }

        }, constraints);

        if (!stripChartsOnly) {
            show(view);
        }

        JFrame mainFrame = getMainFrame();
        Rectangle bounds = mainFrame.getBounds();
        mainFrame.setBounds(new Rectangle(
          Integer.parseInt(trickProperties.getProperty(xKey, Integer.toString(bounds.x))),
          Integer.parseInt(trickProperties.getProperty(yKey, Integer.toString(bounds.y))),
          Integer.parseInt(trickProperties.getProperty(widthKey, Integer.toString(bounds.width))),
          Integer.parseInt(trickProperties.getProperty(heightKey, Integer.toString(bounds.height)))));
    }

    @Override
    protected void connect() throws IOException {
        super.connect();

        //variableServerConnection.setDebugLevel(3);
        variableServerConnection.setClientTag("TRICK_TV_DATA_CHANNEL");

        variableTable.setVariableServerConnection(variableServerConnection);

        variableServerConnection.pause();
        variableServerConnection.add("trick_sys.sched.time_tic_value");
        variableServerConnection.poll();
        timeTicValue = Double.parseDouble(variableServerConnection.get().split("\t")[1]);

        variableServerConnection.clear();
        variableServerConnection.add("trick_cmd_args.cmd_args.default_dir");
        variableServerConnection.poll();
        String defaultDirectory = variableServerConnection.get().split("\t")[1];
        variableServerConnection.clear();

        // If we're connecting to a different simulation, reset everything.
        if (!defaultDirectory.equals(this.defaultDirectory)) {
            this.defaultDirectory = defaultDirectory;
            fileChooser.setCurrentDirectory(new File(defaultDirectory));
            resetState();
        }

        if (variableTree.isEmpty()) {
            processSieFile();
        }

        // Start the variable tracker.
        variableTracker = new VariableTracker(variableServerConnection) {{
            addVariableListener(TVApplication.this);
            execute();
            add(new Variable<VSLong>("trick_sys.sched.time_tics", timeTicCount));
        }};

        monitorAction.setActive(true);

        // Initialize the strip chart manager.
        stripChartManager.clearData();
        stripChartManager.addVariable(simulationTime);

        // If the variable table has contents, then we're reconneting to the same sim.
        // Add the variables to the new variable tracker.
        for (Variable<? extends TrickViewFluent> variable : variableTable.getVariables()) {
            addVariable(variable);
        }

        if (trickProperties.containsKey(autoOpenFileKey)) {
            openFile(new File(trickProperties.getProperty(autoOpenFileKey)), true, false);
            trickProperties.remove(autoOpenFileKey);
        }

        if (trickProperties.containsKey(autoOpenAndSetFileKey)) {
            openFile(new File(trickProperties.getProperty(autoOpenAndSetFileKey)), true, true);
            trickProperties.remove(autoOpenAndSetFileKey);
        }

        if (trickProperties.containsKey(autoSetFileKey)) {
            openFile(new File(trickProperties.getProperty(autoSetFileKey)), false, true);
            trickProperties.remove(autoSetFileKey);
        }

        if (stripChartsOnly && stripChartManager.getStripChartCount() == 0) {
            System.out.println("[Trick View] Strip-chart-only mode was specified, but no strip charts " +
              "could be opened. Provide a TV file containing strip chart information via -" +
              autoOpenFileKey + " or -" + autoOpenAndSetFileKey + ".");
            exit();
        }

        sendCyclePeriod();
        variableServerConnection.setValidateAddresses(validateAddresses);
    }

    /**
     * resets the state of all GUI elements
     */
    private void resetState() {
        // Disable searching.
        variableTree.setEnabled(false);
        searchPanel.setEnabled(false);

        // Clear the variable tree.
        variableTree.setRootInstances(null);

        // Clear the tree search panel.
        searchPanel.cancelSearch();
        searchPanel.setRootTemplates(null);
        searchPanel.clear();

        // Clear the variable table.
        variableTable.clear();

        // Dispose of all strip charts.
        removeAllStripCharts();

        // Clear the strip chart manager.
        stripChartManager.removeAllVariables();
    }

    /**
     * receives and parses the S_sie.resource file in a separate thread and
     * initializes the Variable Hierarchy Tree and Search Panel with the
     * results
     */
    private void processSieFile() {
        sieSwingWorker = new SwingWorker<Void, Void>() {

            {
            execute();
            }

            @Override
            public Void doInBackground() throws Exception {
                statusLabel.setText("Receiving S_sie.resource file");

                VariableServerConnection sieChannel = new VariableServerConnection(getHostName(), getPort());
                //sieChannel.setDebugLevel(3);
                sieChannel.setClientTag("TRICK_TV_SIE_CHANNEL");

                /**
                 *  Because Trick serializes incoming Variable Server commands
                 *  on ip_parse, sending a large S_sie.resource file will
                 *  prevent additional commands, even from other clients, from
                 *  being processed. Thus, we need to delay asking for that file
                 *  until we have loaded any specified files and begun receiving
                 *  variable values. However, with two threads on each side,
                 *  there's no guarantee which connection's commands will get
                 *  processed first. The best we can do is wait a bit and hope
                 *  the main thread's commands get processed first.
                 */
                try {
                    Thread.sleep(1000);
                }
                catch (InterruptedException ie) {}

                sieChannel.put("trick.send_sie_resource()");
                int fileSize = Integer.parseInt(sieChannel.get().split("\t")[1]);
                if (fileSize <= 0) {
                    throw new FileNotFoundException("Trick was unable to open S_sie.resource");
                }

                final StringBuilder sieData = new StringBuilder(fileSize);
                String line;
                do {
                    line = sieChannel.get();
                    sieData.append(line);
                } while (!line.equals("</sie>") && !isCancelled());
                sieChannel.close();

                if (!isCancelled()) {
                    statusLabel.setText("Parsing S_sie.resource file");
                    rootInstances = SieResourceDomParser.parse(new InputSource(new StringReader(sieData.toString())));
                }

                return null;
            }

            @Override
            protected void done() {
                statusLabel.setText(null);
                if (!isCancelled()) {
                    try {
                        get();
                        searchPanel.setRootTemplates(rootInstances);
                        searchPanel.setEnabled(true);
                        variableTree.setRootInstances(rootInstances);
                        variableTree.setEnabled(true);
                    }
                    catch (Exception exception) {
                        rootInstances = null;
                        searchPanel.setEnabled(false);
                        variableTree.setEnabled(false);
                        JOptionPane.showMessageDialog(getMainFrame(), exception,
                          "Variable Hierarchy Failure", JOptionPane.ERROR_MESSAGE);
                        exception.printStackTrace(System.err);
                    }
                }
            }

        };
    }

    @Override
    protected void setConnectionState(boolean connected) {
        super.setConnectionState(connected);
        openAndSetAction.setEnabled(connected);
        setAction.setEnabled(connected);
        monitorAction.setEnabled(connected);
    }

    /**
     * adds a variable, applying default units and format
     *
     * @param variable the variable to be added
     */
    protected void addVariable(Variable<? extends TrickViewFluent> variable) {
        addVariable(variable, true);
    }

    /**
     * adds a variable
     *
     * @param variable the variable to be added
     * @param applyDefaults whether or not to apply selections for default units and format
     */
    @SuppressWarnings("unchecked") // I know what I'm doing.
    protected void addVariable(Variable<? extends TrickViewFluent> variable, boolean applyDefaults) {
        // TV relies on time_tics being the first tracked variable, so don't let users manipulate it.
        if (!variable.name.equals("trick_sys.sched.time_tics")) {
            try {
                if (applyDefaults || (variable.getValue().getFormat() == null || variable.getValue().getPrecision() == null)) {
                    variable.getValue().setFormat(Enum.valueOf(variable.getValue().getFormatClass(),
                            defaultFormats.get(variable.getValue().getClass()).toString()));
                    variable.getValue().setPrecision("--"); // Default precision value
                }
                variableTable.add(variable, position);
                if (getConnectionState()) {
                    variableTracker.add(variable);
                    if (applyDefaults && !defaultAllUnits) {
                        String newUnits = "";
                        String units = variable.getUnits();
                        Matcher matcher = Pattern.compile("[a-zA-z\\-]+").matcher(units);
                        int endIndex = 0;
                        while(matcher.find()) {
                            newUnits += units.substring(endIndex, matcher.start());
                            UnitType unitType = UnitType.getType(matcher.group());
                            Unit unit = unitType == null ? Unit.DEFAULT_UNITS :
                              defaultUnits.get(unitType.ordinal());
                            newUnits += unit == Unit.DEFAULT_UNITS ? matcher.group() : unit;
                            endIndex = matcher.end();
                        }
                        newUnits += units.substring(endIndex);
                        variable.sendUnitsToVariableServer(newUnits, variableServerConnection);
                    }
                    else {
                        variable.sendUnitsToVariableServer(variableServerConnection);
                    }
                }
                stripChartManager.addVariable(variable);
            }
            catch (IOException ioException) {
                disconnect(ioException);
            }
            catch (IllegalArgumentException illegalArgumentException) {
                // Thrown by the strip chart manager for non-numeric variables.
            }
        }
    }

    /**
     * adds a list of variables
     *
     * @param variables the variables to be added
     */
    protected void addVariables(List<? extends Variable<? extends TrickViewFluent>> variables) {
        if (position == Position.Top || (position == Position.After && variableTable.getSelectedRow() != -1) ||
          position == Position.Before && variableTable.getSelectedRow() == -1) {
            Collections.reverse(variables);
        }
        for (Variable<? extends TrickViewFluent> variable : variables) {
            addVariable(variable);
        }
    }

    /**
     * removes all variables
     */
    protected void removeAllVariables() {
        // Delete rows in descending order to avoid index corruption.
        for (int i = variableTable.getVariables().size() - 1; i >= 0; --i) {
            removeVariable(i);
        }
    }

    /**
     * removes the variable corresponding to the <code>index</code>th variable
     * in the variable table
     *
     * @param index index of the variable to remove
     */
    protected void removeVariable(int index) {
        try {
            Variable variable = variableTable.removeVariable(index);
            stripChartManager.removeVariable(variable);
            if (getConnectionState()) {
                variableTracker.remove(variable);
            }
        }
        catch (IOException ioException) {
            disconnect(ioException);
        }
    }

    /**
     * removes all strip charts
     */
    protected void removeAllStripCharts() {
        for (StripChart stripChart : stripChartManager.getStripCharts()) {
            ((JXFrame)stripChart.getTopLevelAncestor()).dispose();
        }
    }

    /**
     * gets a message describing the sytax error
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
     * launches the <code>stripChart</code> in a new frame
     *
     * @param stripChart the strip chart to launch
     */
    void launchStripChart(final StripChart stripChart) {
        launchStripChart(stripChart, null);
    }

    /**
     * launches the <code>stripChart</code> in a new frame with the given <code>bounds</code>
     *
     * @param stripChart the strip chart to launch
     * @param bounds the bounds to use
     */
    void launchStripChart(final StripChart stripChart, final Rectangle bounds) {
        new StripChart.Frame(stripChart, bounds) {

            {
            JMenuBar menuBar = getJMenuBar();
            menuBar.add(new JMenu("Actions") {{
                setMnemonic(KeyEvent.VK_A);
                add(new JMenuItem(clearLogsAction));
            }});

            addWindowListener(new WindowAdapter() {
                @Override
                public void windowClosed(WindowEvent windowEvent) {
                    if (stripChartsOnly && stripChartManager.getStripChartCount() == 0) {
                        exit();
                    }
                }
            });

            if (bounds == null) {
                setLocationRelativeTo(getMainFrame());
            }

            setVisible(true);
            }

            @Override
            public void dispose() {
                stripChartManager.releaseStripChart(stripChart);
                super.dispose();
            }
        };
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

    /**
     * creates a <code>Variable</code> for every variable represented by <code>name</code>
     *
     * @param name the variable's name
     *
     * @return the list of generated variables
     * @throws InvalidVariableNameException InvalidVariableNameException
     */
    protected List<Variable<? extends TrickViewFluent>> getVariables(String name) throws InvalidVariableNameException {
        validateVariable(name);

        name = name.replaceAll("\\s", "");
        String[] segments = name.split("\\.");
        String trimmedName = segments[0].replaceFirst("\\[.*\\]", "");

        ArrayList<String> dimensions = new ArrayList<String>();
        Matcher matcher = Pattern.compile("(?<=\\[).*?(?=\\])").matcher(segments[0]);
        while (matcher.find()) {
            dimensions.add(matcher.group());
        }

        SieTemplate template = null;
        if (rootInstances != null) {
            for (SieTemplate simObject : rootInstances) {
                if (simObject.parameter.equals(trimmedName)) {
                    template = simObject;
                    break;
                }
            }
        }

        return getVariables(template, segments, 0, dimensions, trimmedName);
    }

    /**
     * helper function for generating variables from <code>name</code>
     *
     * @return the list of generated variables
     */
    @SuppressWarnings("unchecked") // clone returns an Object which must be cast
    List<Variable<? extends TrickViewFluent>> getVariables(SieTemplate template,
      String[] segments, int index, ArrayList<String> dimensions, String name) {

        ArrayList<Variable<? extends TrickViewFluent>> variables =
          new ArrayList<Variable<? extends TrickViewFluent>>();

        if (dimensions == null) {
            String trimmedName = segments[index].replaceFirst("\\[.*\\]", "");
            dimensions = new ArrayList<String>();
            Matcher matcher = Pattern.compile("(?<=\\[).*?(?=\\])").matcher(segments[index]);
            while (matcher.find()) {
                dimensions.add(matcher.group());
            }
            if (template != null) {
                for (SieTemplate child : template.children) {
                    if (child.parameter.equals(trimmedName)) {
                        template = child;
                        break;
                    }
                }
            }
            variables.addAll(getVariables(template, segments, index, dimensions, name + "." + trimmedName));
        }
        else if (!dimensions.isEmpty()) {
            String value = dimensions.remove(0);
            int location = value.indexOf('-');
            if (location > -1) {
                int first = Integer.parseInt(value.substring(0, location));
                int last = Integer.parseInt(value.substring(location + 1));
                boolean reverse = first > last;
                for (int i = first; reverse ? (i >= last) : (i <= last); i += reverse ? -1 : 1) {
                    variables.addAll(getVariables(template, segments, index,
                      (ArrayList<String>)dimensions.clone(), name + "[" + i + "]"));
                }
            }
            else {
                variables.addAll(getVariables(template, segments, index, dimensions,
                  name + "[" + Integer.parseInt(value) + "]"));
            }
        }
        else if (index == segments.length - 1) {
             variables.add(variableTree.createVariableFromTemplate(name, template));
        }
        else {
            variables.addAll(getVariables(template, segments, index + 1, null, name));
        }

        return variables;
    }

    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();

        JMenu menu = menuBar.getMenu(0);
        menu.add(new JSeparator(), 0);
        menu.add(new JMenuItem(saveAction), 0);
        menu.add(new JMenuItem(setAction), 0);
        menu.add(new JMenuItem(openAndSetAction), 0);
        menu.add(new JMenuItem(openAction), 0);
        menu.add(new JMenuItem(newAction), 0);

        menuBar.add(new JMenu("Help") {{
            setMnemonic(KeyEvent.VK_H);
            add(new JMenuItem("User's Guide") {{
                setToolTipText("Open the user's guide.");
                addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent actionEvent) {
                        String file = System.getenv().get("TRICK_HOME") + "/docs/html/_trick_view_page.html";
                        try {
                            Desktop.getDesktop().browse(new URI("file", file, null));
                        }
                        catch (Exception exception) {
                            JOptionPane.showMessageDialog(getMainFrame(), exception + "\n\n" +
                              "This operation may not be supported by your desktop.\n" +
                              "Please manually browse to file://" + file,
                              "Help Error", JOptionPane.ERROR_MESSAGE);
                        }
                    }
                });
            }});
            add(new JMenuItem("Submit Bug/Feedback") {{
                setToolTipText("Open a new email to the developer.");
                addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent actionEvent) {
                        String recipient = "derek.r.bankieris@nasa.gov";
                        try {
                            Desktop.getDesktop().mail(new URI("mailto",
                              recipient + "?subject=TrickView Feedback", null));
                        }
                        catch (Exception exception) {
                            JOptionPane.showMessageDialog(getMainFrame(), exception + "\n\n" +
                              "This operation may not be supported by your desktop.\n" +
                              "Please manually send an email to " + recipient + " regarding your feedback.",
                              "Help Error", JOptionPane.ERROR_MESSAGE);
                        }
                    }
                });
            }});
        }}, 1);

        menuBar.add(new JMenu("Actions") {{
            setMnemonic(KeyEvent.VK_A);
            add(new JMenuItem(monitorAction));
            add(new JMenuItem(stripChartAction));
            add(new JMenuItem(removeAction));
            add(new JMenuItem(clearLogsAction));
        }}, 1);

        menuBar.add(new JMenu("View") {{
            setMnemonic(KeyEvent.VK_V);
            variableTreeCheckBox = new JCheckBoxMenuItem("Variable Tree") {{
                setToolTipText("Toggle visibility of the Variable Tree.");
                setMnemonic(KeyEvent.VK_V);
                setSelected(Boolean.parseBoolean(trickProperties.getProperty(
                  variableTreeVisibleKey, Boolean.toString(true))));
                addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent actionEvent) {
                        setVariableTreeVisible(isSelected());
                    }
                });
            }};
            add(variableTreeCheckBox);

            add(new JCheckBoxMenuItem("Search Panel") {{
                setToolTipText("Toggle visibility of the Search Panel.");
                setMnemonic(KeyEvent.VK_S);
                setSelected(Boolean.parseBoolean(trickProperties.getProperty(
                  searchPanelVisibleKey, Boolean.toString(true))));
                addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent actionEvent) {
                        if (!isSelected()) {
                            searchPanel.setPreferredSize(searchPanel.getSize());
                        }
                        searchPanel.setVisible(isSelected());
                        ((JSplitPane)searchPanel.getParent()).resetToPreferredSizes();
                    }
                });
            }});

            // Max Precision Toggle
            add(new JCheckBoxMenuItem("Max Precision") {{
                setToolTipText("Toggle visibility of the Max Precision.");
                setMnemonic(KeyEvent.VK_M);
                setSelected(Boolean.parseBoolean(trickProperties.getProperty(
                        maxPrecisionVisibleKey, Boolean.toString(false))));
                addActionListener(new ActionListener() {
                    public void actionPerformed(ActionEvent actionEvent) {
                        // by default it is not selected. On selection, it shows Max Precision Column on Variable Table
                        if(isSelected()) {
                            variableTable.setMaxPrecisionVisible(true);
                        } else {
                            variableTable.setMaxPrecisionVisible(false);
                        }
                    }
                });
            }});


        }}, 1);

        return menuBar;
    }

    @Override
    protected JToolBar createToolBar() {
        return new JToolBar() {{
            add(newAction);
            add(openAction);
            add(openAndSetAction);
            add(setAction);
            add(saveAction);
            addSeparator();
            add(monitorAction);
            addSeparator();
            add(new JXLabel("Simulation Time: "));
            timeLabel = new JXLabel("unknown");
            add(timeLabel);
            add(Box.createHorizontalGlue());
            statusLabel = new JXLabel();
            add(statusLabel);
            add(Box.createHorizontalGlue());
            add(stripChartAction);
            add(removeAction);
        }};
    }

    @Override
    public JComponent createMainPanel() {
        return new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, new JScrollPane(variableTree) {{
            setPreferredSize(new Dimension(200, 0));
            setVisible(Boolean.parseBoolean(trickProperties.getProperty(
              variableTreeVisibleKey, Boolean.toString(true))));
        }}, new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, searchPanel, new JXPanel(new BorderLayout()) {{
            add(new JScrollPane(variableTable), BorderLayout.CENTER);

            JXPanel manualPanel = new JXPanel(new GridBagLayout()) {{

                GridBagConstraints constraints = new GridBagConstraints() {{
                    gridy = 0;
                    fill = BOTH;
                }};

                add(new JXLabel(" Manual Entry: "), constraints);

                manualField = new JXTextField() {{
                    setAction(new AbstractAction() {
                        {
                        putValue(SHORT_DESCRIPTION, "Manually add a variable by fully-qualified name.");
                        }
                        public void actionPerformed(ActionEvent actionEvent) {
                            try {
                                addVariables(getVariables(getText()));
                            }
                            catch (InvalidVariableNameException invalidVariableNameException) {
                                JOptionPane.showMessageDialog(getMainFrame(),
                                  invalidVariableNameException.toString(), "Invalid Variable Name",
                                  JOptionPane.ERROR_MESSAGE);
                            }
                        }
                    });
                }};


                constraints.weightx = 1;
                add(manualField, constraints);
                constraints.weightx = 0;

                add(new JXButton(new AbstractAction() {
                    {
                    putValue(NAME, "Purge");
                    putValue(SHORT_DESCRIPTION, "Remove all variables that failed to resolve to a valid address.");
                    putValue(MNEMONIC_KEY, KeyEvent.VK_P);
                    }
                    public void actionPerformed(ActionEvent actionEvent) {
                        ArrayList<Variable<? extends TrickViewFluent>> variables = variableTable.getVariables();
                        // Delete rows in descending order to avoid index corruption.
                        for (int i = variables.size() - 1; i >= 0; --i) {
                            if (variables.get(i).getState() == Variable.State.Invalid) {
                                removeVariable(i);
                            }
                        }
                    }
                }), constraints);
            }};


            add(manualPanel, BorderLayout.SOUTH);
        }}) {{
            setName("innerSplitPane");
        }}) {{
            setName("outerSplitPane");
            setBorder(new CompoundBorder(getBorder(), new EmptyBorder(5, 5, 0, 5)));
        }};
    }

    protected void setVariableTreeVisible(boolean visible) {
        Container container = variableTree.getParent().getParent();

        if (container.isVisible() == visible) {
            return;
        }

        if (!visible) {
            container.setPreferredSize(container.getSize());
        }

        container.setVisible(visible);
        variableTreeCheckBox.setSelected(visible);
        ((JSplitPane)container.getParent()).resetToPreferredSizes();

    }

    @Override
    public void variablesUpdated() {
        simulationTime.setValue(Double.toString(timeTicCount.getValue() / timeTicValue));
        timeLabel.setText(String.format("%.2f", simulationTime.getValue().getValue()));
        variableTable.update();
        stripChartManager.update();
    }

    @Override
    public void variableUpdateFailed(Exception exception) {
        disconnect(exception);
    }

    @Override
    protected void disconnect(Exception exception) {

        /**
         * Shut down the variable tracker, which will be null if the first
         * connection attempt after launch fails.
         */
        if (variableTracker != null) {
            variableTracker.cancel(true);
        }

        /**
         * For large sims, it's possible to manually disconnect before fully
         * receiving the S_sie.resource file. If this tread is already done,
         * cancelling it has no effect. sieSwingWorker will be null here if the
         * first connection attempt after launch fails.
         */
        if (sieSwingWorker != null) {
            sieSwingWorker.cancel(true);
        }

        try {
            super.disconnect(exception);
        }
        catch(IOException ignored) {}
    }

    /**
     * opens the specified file, displaying and setting the variables as specified
     *
     * @param file the file to be opened
     * @param display whether or not to replace the current variable table contents with those in the file
     * @param set whether or not to send the values in the file to the Variable Server
     */
    protected void openFile(File file, boolean display, boolean set) {
        try {
            restoreState((TVBean)unmarshaller.unmarshal(new StreamSource(file), TVBean.class).getValue(), display, set);
        }
        catch (Exception exception) {
            JOptionPane.showMessageDialog(getMainFrame(), exception,
              "Failed to Restore State", JOptionPane.ERROR_MESSAGE);
            exception.printStackTrace(System.err);
        }
    }

    /**
     * restores the state of the application
     *
     * @param tvBean the bean from which to restore the state
     * @param display whether or not to display the data
     * @param set whether or not to set the data's values
     */
    protected void restoreState(TVBean tvBean, boolean display, boolean set) {
        if (display) {
            removeAllStripCharts();
            removeAllVariables();
            for (Variable<? extends TrickViewFluent> variable : tvBean.variables) {
                addVariable(variable, false);
            }
            setCyclePeriod(tvBean.cyclePeriod);
        }

        if (set && getConnectionState()) {
            try {
                for (Variable variable : tvBean.variables) {
                    variable.sendValueToVariableServer(variableServerConnection);
                }
            }
            catch (IOException ioException) {
                disconnect(ioException);
            }
        }

        if (display) {
            try {
                for (TVBean.StripChartBean stripChartBean : tvBean.stripCharts) {

                    Variable domainVariable = simulationTime;
                    for (Variable variable : tvBean.variables) {
                        if (variable.name.equals(stripChartBean.domainVariable)) {
                            domainVariable = variable;
                            break;
                        }
                    }

                    ArrayList<Variable<?>> plottedVariables = new ArrayList<>();
                    for (String plottedVariable : stripChartBean.plottedVariables) {
                        for (Variable variable : tvBean.variables) {
                            if (variable.name.equals(plottedVariable)) {
                                plottedVariables.add(variable);
                                break;
                            }
                        }
                    }

                    StripChart stripChart = stripChartManager.createStripChart(
                      domainVariable, plottedVariables, stripChartBean.mode, stripChartBean.fixedAutoRange,
                      stripChartBean.areLinesVisible, stripChartBean.arePointsVisible, stripChartBean.isLegendVisible);
                    stripChartBean.restore(stripChart);

                    launchStripChart(stripChart, new Rectangle(
                      stripChartBean.bounds.x, stripChartBean.bounds.y,
                      stripChartBean.bounds.width, stripChartBean.bounds.height));
                }
            }
            catch (IllegalArgumentException illegalArgumentException) {
                JOptionPane.showMessageDialog(getMainFrame(), illegalArgumentException,
                  "Strip Chart Error", JOptionPane.ERROR_MESSAGE);
                illegalArgumentException.printStackTrace(System.err);
            }
        }
    }

    /**
     * saves the variable table and strip charts to a TV input file
     */
    void saveFile() {
        if (fileChooser.showSaveDialog(getMainFrame()) == JFileChooser.APPROVE_OPTION) {
            File file = fileChooser.getSelectedFile();
            if (!file.getName().endsWith(".tv")) {
                file = new File(file.getPath() + ".tv");
            }
            if (!file.exists() || (JOptionPane.showConfirmDialog(getMainFrame(),"Overwrite existing file?",
              "File Already Exists", JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION)) {
                try {
                    saveState(file);
                }
                catch (Exception exception) {
                    JOptionPane.showMessageDialog(getMainFrame(), exception,
                      "Failed to Save State", JOptionPane.ERROR_MESSAGE);
                    exception.printStackTrace(System.err);
                }
            }
            else {
                saveFile();
            }
        }
    }

    /**
     * saves the variable table and strip charts to <code>file</code>
     *
     * @param file the target
     * @throws JAXBException JAXBException
     */
    protected void saveState(File file) throws JAXBException {
        marshaller.marshal(new JAXBElement<TVBean>(new QName("trickView"), TVBean.class, new TVBean(this)), file);
    }

    /**
     * determines how the Variable Tree is being sorted
     *
     * @return the current Sorting, or <code>Sorting.None</code> if unknown
     */
    private Sorting determineSorting() {
        Comparator<Object> comparator = variableTree.getComparator();
        for (Sorting sorting : Sorting.values()) {
            if (comparator == sorting.comparator) {
                return sorting;
            }
        }
        return Sorting.None;
    }

    @Override
    protected void changeLookAndFeel(String lookAndFeelName) {
        // Some of TV's elements don't respond well to look and feel changes,
        // so I'm not supporting it for now.
    }

    @Override
    protected void shutdown() {
        trickProperties.setProperty(positionKey, position.toString());
        trickProperties.setProperty(sortingKey, determineSorting().name());
        trickProperties.setProperty(fontSizeKey, Integer.toString(variableTree.getFont().getSize()));
        trickProperties.setProperty(defaultUnitsKey, Boolean.toString(defaultAllUnits));
        trickProperties.setProperty(characterArraysAsStringsKey, Boolean.toString(characterArraysAsStrings));
        trickProperties.setProperty(validateAddressesKey, Boolean.toString(validateAddresses));
        trickProperties.setProperty(searchPanelVisibleKey, Boolean.toString(searchPanel.isVisible()));
        trickProperties.setProperty(variableTreeVisibleKey, Boolean.toString(variableTreeCheckBox.isSelected()));
        for (UnitType type : UnitType.values()) {
            trickProperties.setProperty(type.toString(),
              defaultUnits.get(type.ordinal()).toString());
        }
        for (Map.Entry<Class<? extends TrickViewFluent>, Enum> entry : defaultFormats.entrySet()) {
            trickProperties.setProperty(entry.getKey().getSimpleName() + "Format",
              entry.getValue().toString());
        }
        super.shutdown();
    }

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
                Enumeration<AbstractButton> buttons = buttonGroup.getElements();
                do {
                    AbstractButton abstractButton = buttons.nextElement();
                    if (abstractButton.isSelected()) {
                        position = Enum.valueOf(Position.class, abstractButton.getText());
                        break;
                    }
                } while (buttons.hasMoreElements());

                try {
                    ArrayList<Variable<? extends TrickViewFluent>> variables =
                      new ArrayList<Variable<? extends TrickViewFluent>>();

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

        /** button group for the variable addition position buttons */
        ButtonGroup buttonGroup = new ButtonGroup();

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
                    add (new JXPanel() {{
                        add(new JXLabel("Position: ") {{
                            setToolTipText("Position at which variables will be added to the table.");
                        }});

                        for (Position position : Position.values()) {
                            JRadioButton radioButton = new JRadioButton(position.toString());
                            add(radioButton);
                            buttonGroup.add(radioButton);
                        }
                    }}, BorderLayout.NORTH);

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
        public void launch(List<Variable<TrickViewFluent>> pendingVariables) {
            pendingVariables = new ArrayList<Variable<TrickViewFluent>>(
              new LinkedHashSet<Variable<TrickViewFluent>>(pendingVariables));

            for (Variable variable : pendingVariables) {
                // If any variables are indexable, launch the dialog.
                if (pattern.matcher(variable.name).find()) {
                    northPanel.removeAll();
                    for (Variable<TrickViewFluent> var : pendingVariables) {
                        northPanel.add(new VariablePanel(var));
                    }

                    Enumeration<AbstractButton> buttons = buttonGroup.getElements();
                    AbstractButton selectedButton = buttons.nextElement();
                    for (int i = 0; i < position.ordinal(); ++i) {
                        selectedButton = buttons.nextElement();
                    }
                    buttonGroup.setSelected(selectedButton.getModel(), true);

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
            Variable<TrickViewFluent> variable;

            /** a matcher for finding indices */
            Matcher matcher;

            /** the segments of the variable's name */
            String[] segments;

            /** the index specifications */
            ArrayList<JComponent> indices = new ArrayList<JComponent>();

            /** indication that the character array should be added as a string */
            JCheckBox asStringCheckBox = new JCheckBox("As String") {{
                addItemListener(new ItemListener() {
                    @Override
                    public void itemStateChanged(ItemEvent itemEvent) {
                        indices.get(indices.size() - 1).setEnabled(!isSelected());
                    }
                });
            }};

            /**
             * constructor
             *
             * @param variable the variable to add
             */
            public VariablePanel(Variable<TrickViewFluent> variable) {
                super(new GridBagLayout());
                this.variable = variable;
                matcher = pattern.matcher(variable.name);
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

                segments = pattern.split(variable.name);
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

                if (variable.getValue() instanceof TVByte && !indices.isEmpty()) {
                    add(asStringCheckBox);
                    asStringCheckBox.setSelected(characterArraysAsStrings);
                }
            }

            /**
             * constructs the fully qualified name of the variable
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

                // If we're treating this as a string, drop the last index.
                if (asStringCheckBox.isSelected()) {
                    return name.substring(0, name.lastIndexOf('['));
                }

                return name;
            }

            /**
             * returns the variables, with indices expanded, represented by this panel
             *
             * @param all variables represented by this panel
             */
            public Collection<Variable<? extends TrickViewFluent>> constructVariables()
              throws InvalidVariableNameException {
                return getVariables(constructName());
            }

        }

    }

    /**
     * allows the user to specifiy an index range for pointers
     */
    class IndexTextField extends JXTextField {

        /**
         * constructor
         */
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
     * the monitor action
     */
    class MonitorAction extends AbstractAction {

        /** monitor off icon */
        ImageIcon offIcon = new ImageIcon(TVApplication.class.getResource("resources/tv_off.png"));

        /** monitor on icon */
        ImageIcon onIcon = new ImageIcon(TVApplication.class.getResource("resources/tv_on.png"));

        /** state of the monitor */
        boolean active = false;

        /**
         * constructor
         */
        public MonitorAction() {
            super("Toggle Monitor");
            putValue(SHORT_DESCRIPTION, "Toggle monitoring of the variables.");
            putValue(MNEMONIC_KEY, KeyEvent.VK_M);
            putValue(SMALL_ICON, offIcon);
            setEnabled(false);
        }

        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            setActive(!active);
        }

        /**
         * sets the state of the monitor
         *
         * @param the state of the monitor
         */
        public void setActive(boolean active) {
            try {
                if (active) {
                    variableServerConnection.unpause();
                    putValue(SMALL_ICON, onIcon);
                }
                else {
                    variableServerConnection.pause();
                    putValue(SMALL_ICON, offIcon);
                }
                this.active = active;
            }
            catch (Exception exception) {
                disconnect(exception);
            }
        }
    }

    /** exception thrown for poorly formed variable names */
    class InvalidVariableNameException extends Exception {
        public InvalidVariableNameException(String description) {
            super(description);
        }
    }

    /**
     * launches the application
     *
     * @param args arguments
     */
    public static void main(String[] args) {
        Application.launch(TVApplication.class, args);
    }

}
