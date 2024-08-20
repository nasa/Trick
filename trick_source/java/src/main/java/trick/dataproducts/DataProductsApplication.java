
//========================================
//    Package
//========================================
package trick.dataproducts;

//========================================
//    Imports
//========================================
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.ComponentOrientation;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;

import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.SwingWorker;

import org.jdesktop.swingx.JXMultiSplitPane;
import org.jdesktop.swingx.MultiSplitLayout;

import trick.common.framework.View;
import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.common.ui.components.NumberTextField;
import trick.common.utils.SwingAction;
import trick.dataproducts.trickqp.TrickQPApplication;
import trick.dataproducts.utils.Session;
import trick.dataproducts.utils.SessionRun;
import trick.dataproducts.utils.SessionXMLCreator;



/**
 * Data Products Application that extends {@link TrickApplication}.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public abstract class DataProductsApplication extends TrickApplication {

    //========================================
    //  Public data
    //========================================
    public JTextField versionField;
    public JTextField titleField;
    public NumberTextField startField;
    public NumberTextField stopField;
    public NumberTextField freqField;

    public JTextArea statusArea;

    public SessionRun runToConfigure;

    public String sessionFile;

    public File fileDevice;

    //========================================
    //  Protected data
    //========================================
    protected JLabel versionLabel;
    protected JLabel titleLabel;
    protected JLabel startLabel;
    protected JLabel stopLabel;
    protected JLabel freqLabel;

    // radio buttons for plot utility
    protected ButtonGroup radioButtonGroup;
    protected JRadioButtonMenuItem fermiRadioButton;
    protected JRadioButtonMenuItem javaRadioButton;
    protected JRadioButtonMenuItem gnuplotRadioButton;

    protected JToggleButton gnuplotButton;

    protected String plotDevice = Session.DEVICE_OPTIONS[Session.TERMINAL_DEVICE];
    protected String gnuplotTerminal = Session.GNUPLOT_TERMINAL_OPTIONS[Session.X11_GNUPLOT_TERMINAL];


    protected static String TEMP_DP_FILE         = "/tmp/DP_" + System.getenv("USER") + ".xml";
    protected static String TEMP_SESSION_FILE    = "/tmp/Session_" + System.getenv("USER") + ".xml";

    protected boolean fermiExists ;

    //========================================
    //  Private Data
    //========================================
    private String plotCommand;

    // Options are: "Simple", "Comparison", "Delta", "Contrast"
    private String preferredPresentation;

    // Options are: "Plot", "Table".
    private String displayMode;


    //========================================
    //  Constructors
    //========================================


    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets the preferred presentation.
     * @param pt the presentation string
     */
    public void setPreferredPresentation(String pt) {
        preferredPresentation = pt;
    }

    /**
     * Gets the preferred presentation.
     * @return the preferred presentation
     */
    public String getPreferredPresentation() {
        return preferredPresentation;
    }

    /**
     * Sets preferred display mode.
     * @param md the mode
     */
    public void setDisplayMode(String md) {
        displayMode = md;
    }

    /**
     * Gets preferred display mode.
     * @return the mode of the preferred display
     */
    public String getDisplayMode() {
        return displayMode;
    }

    /**
     * Gets the common bottom component if this is what you want.
     *
     * @return    An instance of {@link JComponent}.
     */
    protected JComponent getCommonBottom() {
        JPanel panel = new JPanel();
        panel.setPreferredSize(new Dimension(600, 180));

        panel.setLayout(new BorderLayout());
        JPanel fieldsPanel = new JPanel();
        FlowLayout flowLayout = new FlowLayout();
        fieldsPanel.setLayout(flowLayout);
        flowLayout.setAlignment(FlowLayout.LEADING);
        fieldsPanel.setComponentOrientation(ComponentOrientation.LEFT_TO_RIGHT);

        if (DataProductsApplication.getInstance() instanceof TrickQPApplication) {
            titleLabel = new JLabel("Title:");
            titleField = new JTextField(15);
            fieldsPanel.add(titleLabel);
            fieldsPanel.add(titleField);
            fieldsPanel.add(Box.createHorizontalStrut(10));
        }

        versionLabel = new JLabel("Version:");
        versionField = new JTextField("1.0", 2);
        fieldsPanel.add(versionLabel);
        fieldsPanel.add(versionField);
        fieldsPanel.add(Box.createHorizontalStrut(10));

        startLabel = new JLabel("Start:");
        startField = new NumberTextField("-1.0e20", 5);
        fieldsPanel.add(startLabel);
        fieldsPanel.add(startField);
        fieldsPanel.add(Box.createHorizontalStrut(10));

        stopLabel = new JLabel("Stop:");
        stopField = new NumberTextField("1.0e20", 5);
        fieldsPanel.add(stopLabel);
        fieldsPanel.add(stopField);
        fieldsPanel.add(Box.createHorizontalStrut(10));

        freqLabel = new JLabel("Freq:");
        freqField = new NumberTextField("0.0", 5);
        fieldsPanel.add(freqLabel);
        fieldsPanel.add(freqField);
        fieldsPanel.add(Box.createHorizontalStrut(10));

        panel.add(fieldsPanel, BorderLayout.NORTH);

        statusArea = new JTextArea();
        statusArea.setBackground(new Color(65, 65, 65));
        statusArea.setForeground(Color.white);
        panel.add(new JScrollPane(statusArea), BorderLayout.CENTER);

        return panel;
    }


    //========================================
    //    Methods
    //========================================
    @SwingAction
    public void configureRunTimename() {
        String msgStr1 = "Please specify the time name for the selected run\n";
        String msgStr2 = "\n\n";

        if (runToConfigure != null) {
            msgStr2 = runToConfigure.getDir() + msgStr2 ;
            //String timeName = resourceMap.getString("default.timename");
            String timeName = DEFAULT_TIME_NAME;
            if (runToConfigure.getTimename() != null) {
                timeName = runToConfigure.getTimename();
            }
            Object inputValue = JOptionPane.showInputDialog(getMainFrame(), msgStr1+msgStr2, timeName);
            if (inputValue != null) {
                runToConfigure.setTimename(inputValue.toString());
            }
        } else {
            JOptionPane.showMessageDialog(getMainFrame(), "Please select a Run first!", "No Run Selected", JOptionPane.WARNING_MESSAGE);
            return;
        }
    }

    @SwingAction
    public void toggleGnuplot() {
        if (gnuplotButton.isSelected()) {
            gnuplotButton.setIcon(resourceMap.getIcon("gnuplot.on.icon"));
            gnuplotRadioButton.setSelected(true);
            getAction("selectGnuplotTerminal").setEnabled(true);
        } else {
            gnuplotButton.setIcon(resourceMap.getIcon("gnuplot.off.icon"));
            if (gnuplotRadioButton.isSelected()) {
                if ( fermiExists ) {
                    fermiRadioButton.setSelected(true);
                } else {
                    javaRadioButton.setSelected(true);
                }
            }
            getAction("selectGnuplotTerminal").setEnabled(false);
        }
    }

    @SwingAction
    public void selectFermi() {
        gnuplotButton.setSelected(false);
        getAction("selectGnuplotTerminal").setEnabled(false);
        toggleGnuplot();
    }

    @SwingAction
    public void selectGnuplot() {
        gnuplotButton.setSelected(true);
        getAction("selectGnuplotTerminal").setEnabled(true);
        toggleGnuplot();
    }

    @SwingAction
    public void selectJavaPlot() {
        gnuplotButton.setSelected(false);
        getAction("selectGnuplotTerminal").setEnabled(false);
        toggleGnuplot();
    }

    @SwingAction
    public void selectTerminalDevice() {
        setDevice(Session.TERMINAL_DEVICE);
    }

    @SwingAction
    public void selectPrinterDevice() {
        setDevice(Session.PRINTER_DEVICE);
    }

    @SwingAction
    public void selectGnuplotTerminal() {
        // do nothing
    }

    @SwingAction
    public void selectX11() {
        setGnuplotTerminal(Session.X11_GNUPLOT_TERMINAL);
    }

    @SwingAction
    public void selectPSColor() {
        setGnuplotTerminal(Session.PS_COLOR_GNUPLOT_TERMINAL);
    }

    @SwingAction
    public void selectPSBW() {
        setGnuplotTerminal(Session.PS_BW_GNUPLOT_TERMINAL);
    }

    @SwingAction
    public void selectPNG() {
        setGnuplotTerminal(Session.PNG_GNUPLOT_TERMINAL);
    }

    @SwingAction
    public void selectEPS() {
        setGnuplotTerminal(Session.EPS_GNUPLOT_TERMINAL);
    }

    @SwingAction
    public void selectAQUA() {
        setGnuplotTerminal(Session.AQUA_GNUPLOT_TERMINAL);
    }

    /**
     * Cleans up the socket communication before exiting the application.
     */
    @Override
    protected void shutdown() {
        super.shutdown();
    }


    /**
     * Makes initialization as needed. This is called before startup().
     *
     * @see #startup
     */
    @Override
    protected void initialize(String[] args) {
        super.initialize(args);
    }

    /**
     * Starts things such as establishing socket communication, and starting monitor tasks.
     * This is called after startup.
     *
     * @see #initialize
     * @see #startup
     */
    @Override
    protected void ready() {
        createActionController();
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

        gnuplotButton = new JToggleButton();
        gnuplotButton.setAction(getAction("toggleGnuplot"));
        gnuplotButton.setText(null);
        gnuplotButton.setFocusable(false);

        fermiRadioButton = new JRadioButtonMenuItem();
        fermiRadioButton.setAction(getAction("selectFermi"));

        javaRadioButton = new JRadioButtonMenuItem(getAction("selectJavaPlot"));

        String fermi_exe = UIUtils.getTrickHome() + "/bin/trick-fxplot" ;
        File f = new File(fermi_exe) ;
        fermiExists = f.exists() ;

        if ( fermiExists ) {
            fermiRadioButton.setSelected(true);
        } else {
            javaRadioButton.setSelected(true);
        }

        gnuplotRadioButton = new JRadioButtonMenuItem();
        gnuplotRadioButton.setAction(getAction("selectGnuplot"));

        radioButtonGroup = new ButtonGroup();
        if ( fermiExists ) {
            radioButtonGroup.add(fermiRadioButton);
        }
        radioButtonGroup.add(javaRadioButton);
        radioButtonGroup.add(gnuplotRadioButton);


        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());
        view.setToolBar(createToolBar());

        show(view);
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
            "(COLUMN " +
                "(ROW weight=0.8 " +
                    "(COLUMN weight=0.44  (LEAF name=left.top weight=0.55) " +
                    "                     (LEAF name=left.middle weight=0.45) " +
                    ") " +
                    "(COLUMN weight=0.56  (LEAF name=right.top weight=0.55) " +
                    "                     (LEAF name=right.middle weight=0.45) " +
                    ") " +
                ")" +
                "(LEAF name=bottom weight=0.2) " +
            ")";
        MultiSplitLayout.Node modelRoot = MultiSplitLayout.parseModel( layoutDef );

        msp.getMultiSplitLayout().setModel( modelRoot );

        msp.add( createLeftTop(), "left.top" );
        msp.add( createLeftMiddle(), "left.middle" );
        msp.add( createRightTop(), "right.top" );
        msp.add( createRightMiddle(), "right.middle");
        msp.add( createBottom(), "bottom" );

        msp.validate();

        return msp;
    }


    /**
     * Creates the component for left top.
     * @return left top component
     */
    protected abstract JComponent createLeftTop();

    /**
     * Creates the component for left middle.
     * @return left middle component
     */
    protected abstract JComponent createLeftMiddle();

    /**
     * Creates the component for right top.
     * @return right top component
     */
    protected abstract JComponent createRightTop();

    /**
     * Creates the component for right middle.
     * @return right middle component
     */
    protected abstract JComponent createRightMiddle();

    /**
     * Creates the component for the bottom.
     * @return bottom component
     */
    protected abstract JComponent createBottom();


    /**
     * Helper method for setting gnuplot terminal.
     *
     * @param index    One of these: Session.X11_GNUPLOT_TERMINAL,
     *                            Session.PS_COLOR_GNUPLOT_TERMINAL,
     *                            Session.PW_BW_GNUPLOT_TERMINAL,
     *                            Session.PNG_GNUPLOT_TERMINAL,
     *                            Session.EPS_GNUPLOT_TERMINAL,
     *                            Session.AQUA_GNUPLOT_TERMINAL.
     */
    private void setGnuplotTerminal(int index) {
        if (index >= 0 && index<Session.GNUPLOT_TERMINAL_OPTIONS.length) {
            gnuplotTerminal = Session.GNUPLOT_TERMINAL_OPTIONS[index];
        } else {
            gnuplotTerminal = Session.GNUPLOT_TERMINAL_OPTIONS[Session.X11_GNUPLOT_TERMINAL];
        }
    }

    /**
     * Helper method for setting plot device.
     *
     * @param index    One of these: Session.TERMINAL_DEVICE,
     *                            Session.PRINTER_DEVICE,
     *                            Session.FILE_DEVICE.
     */
    private void setDevice(int index) {
        if (index >= 0 && index<Session.DEVICE_OPTIONS.length) {
            plotDevice = Session.DEVICE_OPTIONS[index];
        } else {
            plotDevice = Session.DEVICE_OPTIONS[Session.TERMINAL_DEVICE];
        }
    }

    /**
     * Resets all commond fields if available.
     */
    public void resetCommonFields() {
        if (titleField != null) {
            titleField.setText("");
        }
        versionField.setText("1.0");
        startField.setText("-1.0e20");
        stopField.setText("1.0e20");
        freqField.setText("0.0");
    }

    /**
     * Sets the action controller for the application.
     */
    protected abstract void createActionController();


    /**
     * Saves {@link Session} related data to a specified file.
     *
     * @param session            An instance of {@link Session}.
     * @param fileName            The name of a file.
     * @param runData            A list for runs.
     * @param productFilesData    A list of dp files.
     */
    public void saveSessionToFile(Session session, String fileName, Object[] runData, Object[] productFilesData) {
        if (fileName == null) {
            // User clicked a plot button, so create temporary session file
            sessionFile = TEMP_SESSION_FILE;
        } else {
            sessionFile = fileName;
        }

        SessionXMLCreator xmlCreator = new SessionXMLCreator();
        xmlCreator.setXMLFile(sessionFile);
        xmlCreator.setDTDFile("Session.dtd");
        xmlCreator.setPublicID("-//Tricklab//DTD Session V1.0//EN");

        session.setVersion(versionField.getText());
        // session file doesn't have title, add this back if it needs it.
        //if (!titleField.getText().isEmpty()) {
            //session.setTitle(titleField.getText());
        //}
        session.setDevice(plotDevice);
        if (gnuplotButton.isSelected()) {
            session.setGnuplotTerminal(gnuplotTerminal);
        }

        session.setTStart((Double)startField.getValue());
        session.setTStop((Double)stopField.getValue());
        session.setFrequency((Double)freqField.getValue());
        xmlCreator.setRoot(session);

        for (int i = 0; i < runData.length; i++) {
            SessionRun sr = null;

            if (runData[i] instanceof SessionRun) {
                sr = (SessionRun)runData[i];
            } else {
                sr = new SessionRun(runData[i].toString());
            }

            session.addRun(sr);
        }
        for (int i = 0; i < productFilesData.length; i++) {
            session.addProductFile(productFilesData[i].toString());
        }
        xmlCreator.runCreator();
    }

    /**
     * Launches the external program for plotting.
     *
     * @param sessionFile    The session used for plotting.
     */
    public void launchPlotProgram(String sessionFile) {
        if (fermiRadioButton.isSelected()) {
            plotCommand = resourceMap.getString("fxplot.command");
        } else if (javaRadioButton.isSelected()) {
            plotCommand = resourceMap.getString("jxplot.command");
        } else if (gnuplotRadioButton.isSelected()) {
            plotCommand = resourceMap.getString("gxplot.command");
        }

        plotCommand = UIUtils.getTrickBin() + File.separator + plotCommand;
        (new LaunchPlotProcessTask(plotCommand, sessionFile)).execute();
    }

    /**
     * Launches a process with specified parameters.
     *
     * @param command     The operating system program and arguments.
     * @throws Exception Exception
     */
    public void launchPlotProcess(String... command) throws Exception{
        if (command == null || command.length < 0) {
            printStatusMessage("No plotting command specified!\n");
            return;
        }
        if (plotDevice.equals(Session.DEVICE_OPTIONS[Session.FILE_DEVICE])) {
            printStatusMessage("Generating postscript file(s) ...\n");
        } else {
            printStatusMessage("===>>>Launching " + command[0] + " " + command[1] + "<<<===\n");
        }
        ProcessBuilder pb = new ProcessBuilder(command);
        pb.redirectErrorStream(true);
        captureProcessMessage(pb.start());
    }

    /**
     * Redirects runtime process messages from screen to GUI status area.
     *
     * @param runtimeProcess The runtime process from which screen messages is generated.
     */
    public void captureProcessMessage(Process runtimeProcess) {
        try {
            if (runtimeProcess == null) {
                return;
            }

            BufferedReader stdInput = new BufferedReader(new InputStreamReader(runtimeProcess.getInputStream()));

            String s = null;

            // read the output from the command
            while ((s = stdInput.readLine()) != null) {
                printStatusMessage(s + "\n");
            }
        } catch (IOException ioe) {
            printStatusMessage(ioe.getMessage() + "\n");
        }
    }

    /**
     * Prints status message to the status text area.
     * @param msg the string to use
     */
    public void printStatusMessage(String msg) {
        statusArea.append(msg);
    }


    private class LaunchPlotProcessTask extends SwingWorker<Void, Void> {
        private String[] processCommand;
        public LaunchPlotProcessTask(String... command) {
            this.processCommand = command;
        }

        @Override
        public Void doInBackground() {
            try {
                launchPlotProcess(processCommand);
            } catch (Exception e) {
                printStatusMessage("Error launching plotting process!\n");
            }
            return null;
        }

        @Override
        public void done() {
        }
    }

}
