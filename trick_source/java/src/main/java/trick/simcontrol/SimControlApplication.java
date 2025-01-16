
//========================================
//    Package
//========================================
package trick.simcontrol;

//========================================
//    Imports
//========================================
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.Desktop;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.MulticastSocket;
import java.net.UnknownHostException;
import java.net.SocketTimeoutException;
import java.net.URI;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.channels.NotYetConnectedException;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.channels.UnresolvedAddressException;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JSplitPane;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;
import javax.swing.SwingConstants;
import javax.swing.SwingWorker;
import javax.swing.text.BadLocationException;
import javax.swing.text.DefaultStyledDocument;
import javax.swing.text.Document;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import javax.swing.text.StyledEditorKit;

import org.jdesktop.application.Action;
import org.jdesktop.application.Application;
import org.jdesktop.application.Task;
import org.jdesktop.application.View;
import org.jdesktop.swingx.JXEditorPane;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXStatusBar;
import org.jdesktop.swingx.JXTitledPanel;
import org.jdesktop.swingx.JXTitledSeparator;

import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.common.ui.components.FontChooser;
import trick.common.ui.panels.AnimationPlayer;
import trick.common.ui.panels.FindBar;
import trick.common.utils.VariableServerConnection;
import trick.simcontrol.utils.SimControlActionController;
import trick.simcontrol.utils.SimState;


/**
 * The Sim Control application class which is a sub class of {@link TrickApplication}.
 *
 * @since Trick 10
 */
public class SimControlApplication extends TrickApplication implements PropertyChangeListener {

    //========================================
    //    Public data
    //========================================
	final public static Dimension FULL_SIZE = new Dimension(680, 640);
	final public static Dimension LITE_SIZE = new Dimension(340, 360);


    //========================================
    //    Protected data
    //========================================
    protected static String host;
    protected static int port = -1;

    /** whether or not to print a send_hs file to the status message panel */
    protected static boolean isRestartOptionOn;

    /** whether automatically exit when sim is done/killed. */
    protected static boolean isAutoExitOn; 

    /** The action controller that performs actions for such as clicking button, selection a menu item and etc. */
    protected SimControlActionController actionController;


    //========================================
    //    Private Data
    //========================================
    private int modeIndex = -1;
    private int debug_flag ;
    private int debug_present ;
    private int overrun_present ;
    private int message_present ;
    private int message_port ;

    // The panel that displays the current sim state description as well as progress.
    private JXTitledPanel runtimeStatePanel;
    private String currentSimStatusDesc = "None";
    private JProgressBar progressBar;
    // Always enable progress bar unless the sim termination time is not defined
    private boolean enableProgressBar = true;

    private JTextField recTime;
    //private JTextField realtimeTime;
    //private JTextField metTime;
    //private JTextField gmtTime;
    private JTextField simRealtimeRatio;

    private JXTitledPanel simOverrunPanel;
    private JTextField[] simRunDirField;
    private JTextField[] overrunField;
    private int slaveCount;
    private double simStartTime;
    private double simStopTime;
    private double execTimeTicValue;

    private JToggleButton dataRecButton;
    private JToggleButton realtimeButton;
    private JToggleButton dumpChkpntASCIIButton;
    private JToggleButton loadChkpntButton;
    private JToggleButton liteButton;

    private JXEditorPane statusMsgPane;

    private JXLabel statusLabel;

    // The animation image player panel
    private AnimationPlayer logoImagePanel;

    // VariableServerConnection for sending/receiving Variable Server commands.
    private VariableServerConnection commandSimcom;
    // VariableServerConnection for receiving Sim state from Variable Server.
    private VariableServerConnection statusSimcom;

    // Socket for receiving health and status messages
    private SocketChannel healthStatusSocketChannel ;
   
    private JComboBox runningSimList;
    //True if an error was encountered during the attempt to connect to Variable Server during intialize()
    private boolean errOnInitConnect = false;
    //Time out when attempting to establish connection with Variable Server in milliseconds
    private int varServerTimeout = 5000;
    
    // The object of SimState that has Sim state data.
    private SimState simState;
    private String customizedCheckpointObjects;

    private static Charset charset;
    

    final private static String LOCALHOST = "localhost";

    //========================================
    //    Actions
    //========================================
    @Action
    public void showStatusFont() {
        Font font = FontChooser.showDialog(statusMsgPane, "Choose Font", statusMsgPane.getFont());
        if (font != null) {
            statusMsgPane.setFont(font);
        }
    }
    
    @Action
    public void saveStatusMsgs() {
        String initialName = "sim_msg_";
        File file = UIUtils.chooseSaveFile(simState.getRunPath(), initialName, null, getMainFrame());
        if (statusMsgPane != null && file != null) {
            Document doc = statusMsgPane.getDocument();
            try {
                if (doc != null) {
                    UIUtils.saveTextFile(doc.getText(0, doc.getLength()), file);
                }
            } catch (BadLocationException ble) {

            }
        }
    }

    @Action
    public void clearStatusMsgs() {
        if (statusMsgPane != null) {
            Document doc = statusMsgPane.getDocument();
            try {
                if (doc != null) {
                    doc.remove(0, doc.getLength());
                }
            } catch (BadLocationException ble) {

            }
        }
    }

    @Action
    public void startTV() {
        launchTrickApplication("tv",  "--host " + host + " --port " + port);
    }

    @Action
    public void startMTV() {
        launchTrickApplication("mtv",  host + " " + port);
    }

    @Action
    public void openWiki() {
        if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.BROWSE)) {
            try {
                Desktop.getDesktop().browse(new URI("https://nasa.github.io/trick/index"));
            }
            catch(Exception e) {
                System.out.println("Exception occurred while opening wiki: " + e.getMessage());
            }
        }
    }

    @Action
    public void freezeAt() {
        actionController.handleFreezeAt(simState.getExecOutTime(), getMainFrame());
    }

    @Action
    public void freezeIn() {
        actionController.handleFreezeIn(simState.getExecOutTime(), getMainFrame());
    }

    @Action
    public void checkpointObjects() {
        customizedCheckpointObjects = actionController.handleCheckpointObjects(getMainFrame(), customizedCheckpointObjects);
    }

    @Action
    public void throttle() {
        actionController.handleThrottle(getMainFrame());
    }

    @Action
    public void stepSim() {
        actionController.handleStep(debug_flag);
    }

    @Action
    public void recordingSim() {
        actionController.handleRecOnOff(dataRecButton.isSelected());
    }

    @Action
    public void startSim() {
        actionController.handleStartSim();
    }

    @Action
    public void realtime() {
        actionController.handleRealtime(realtimeButton.isSelected());
    }

    @Action
    public void freezeSim() {
        actionController.handleFreeze(debug_flag);
    }

    @Action
    public void shutdownSim() {
        actionController.handleShutdown();
    }

    @Action
    public void dumpChkpntASCII() {
 
        String fileName = "chkpnt_" + simState.getTwoFractionFormatted(simState.getExecOutTime());

        // if customizedCheckpointObjects are specified, remove all white spaces.
        if (customizedCheckpointObjects != null) {
                customizedCheckpointObjects = customizedCheckpointObjects.trim();
                Pattern pattern = Pattern.compile("\\s+");
                Matcher matcher = pattern.matcher(customizedCheckpointObjects);
                if (matcher.find()) {
                        // replace all white spaces with nothing, basically remove all white spaces
                        // so that server side doesn't need to trim the spaces for each sim object name.
                        customizedCheckpointObjects = matcher.replaceAll("");
                }
        }

        actionController.handleDumpChkpntASCII(customizedCheckpointObjects, simState.getRunPath(), fileName, getMainFrame());
        runtimeStatePanel.setTitle("Dumping ASCII Checkpoint");
        currentSimStatusDesc = "PreCheckpoint";
    }

    @Action
    public void loadChkpnt() {
        actionController.handleLoadChkpnt(simState.getRunPath(), getMainFrame());
        runtimeStatePanel.setTitle("Loading Checkpoint");
        currentSimStatusDesc = "PreCheckpoint";
    }

    @Action
    public void lite() {
    	if (liteButton.isSelected()) {
    		getMainFrame().setSize(LITE_SIZE);
    	} else {
    		getMainFrame().setSize(FULL_SIZE);
    	}
    }

    /**
     * Connects to the variable server if {@link VariableServerConnection} is able to be created successfully and
     * starts the communication server for sim health status messages.
     */
    @Action
    public void connect() {
        parseConnectionFromSimList(); // get host and port from running sim list
		getInitializationPacket(); // init variable server connection
		if (commandSimcom == null) {
			String errMsg = "Sorry, can't connect. Please make sure the availability of both server and port!";
			printErrorMessage(errMsg);
            return;
        } 
		setEnabledAllActions(true); // enable all actions
		scheduleGetSimState(); // set up the sim status variables
		startStatusMonitors(); // start monitors for sim and health status
    }

    
    /**
     * Helper method for starting monitors for sim status as well as health status.
     */
	protected void startStatusMonitors() {
		MonitorSimStatusTask monitorSimStatusTask = new MonitorSimStatusTask(this);
        monitorSimStatusTask.addPropertyChangeListener(this);
        getContext().getTaskService().execute(monitorSimStatusTask);

        // For receiving hs messages.
        getContext().getTaskService().execute(new MonitorHealthStatusTask(this));
	}

	/**
	 * Sets all actions as either enabled or disabled
	 * @param isEnabled the state to set each action as
	 */
	protected void setEnabledAllActions(boolean isEnabled) {
		if(actionMap == null)
			return;
		
		Object[] keys = actionMap.allKeys();
		// If there is a server connection established, enable all actions.
		for (int i = 0; i < keys.length; i++) {
			String theKey = (String)keys[i];
			getAction(theKey).setEnabled(isEnabled);
		}
	}
	
	/**
	 * Parse the host and port from the runningSimList object
	 */
	protected void parseConnectionFromSimList() {

		if (runningSimList != null && runningSimList.getSelectedItem() != null) {
        	String selectedStr = runningSimList.getSelectedItem().toString();
        	// remove the run info if it is shown
        	int leftPre = selectedStr.indexOf("(");
        	if (leftPre != -1) {
        		selectedStr = selectedStr.substring(0, leftPre);
        	}
        	// can be separated either by : or whitespace
        	String[] elements = selectedStr.split(":");
        	
        	if (elements == null || elements.length < 2) {
        		elements = selectedStr.split("\\s+");
        	}
        	
        	if (elements == null || elements.length < 2) {       
				String errMsg = "Can't connect! Please provide valid host name and port number separated by : or whitespace!";		
                printErrorMessage(errMsg);
        	} else {
				host = elements[0].trim();
				try {  port = Integer.parseInt(elements[1].trim());  }
				catch (NumberFormatException nfe) {
					String errMsg = elements[1] + " is not a valid port number!";
					printErrorMessage(errMsg);
				}
			}
        }
	}

    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Gets the initialization packet from Variable Server if it is up.
     */
    public void getInitializationPacket() {    	
        String simRunDir = null;
        String[] results = null;      
        boolean masterslave_enabled;
        try {
			String errMsg = "Error: SimControlApplication:getInitializationPacket()";
            try {
            	if (host != null && port != -1) {
            		commandSimcom = new VariableServerConnection(host, port, varServerTimeout);
            	} else {
            		commandSimcom = null;
            	}
            } catch (UnknownHostException host_exception) {
                /** The IP address of the host could not be determined. */
                errMsg += "\n Unknown host \""+host+"\"";
                errMsg += "\n Please use a valid host name (e.g. localhost)";
                errOnInitConnect = true;   
		printErrorMessage(errMsg); 
            } catch (SocketTimeoutException ste) {
                /** Connection attempt timed out. */
                errMsg += "\n Connection Timeout \""+host+"\"";
                errMsg += "\n Please try a different host name (e.g. localhost)";
                errOnInitConnect = true;
                printErrorMessage(errMsg);           
            } catch (IOException ioe) {
                /** Port number is unavailable, or there is no connection, etc. */
                errMsg += "\n Invalid TCP/IP port number \""+port+"\"";
                errMsg += "\n Please check the server and enter a proper port number!";
                errMsg += "\n IOException ..." + ioe;
                errMsg += "\n If there is no connection, please make sure SIM is up running properly!";
                errOnInitConnect = true;
		printErrorMessage(errMsg);
            } 
            
            if (commandSimcom == null) {
            	(new RetrieveHostPortTask()).execute();
            	return;
            }
            
            actionController.setVariableServerConnection(commandSimcom);

            simState = new SimState();

            commandSimcom.put("trick.var_exists(\"trick_master_slave.master.num_slaves\")");
            results = commandSimcom.get().split("\t");
            masterslave_enabled = results[1].equals("1");

            commandSimcom.put("trick.var_set_client_tag(\"SimControl\")\n");
            commandSimcom.put("trick.var_add(\"trick_sys.sched.sim_start\") \n" +
            		          "trick.var_add(\"trick_sys.sched.terminate_time\") \n" +
                              "trick.var_add(\"trick_sys.sched.time_tic_value\") \n" +
                              "trick.var_add(\"trick_cmd_args.cmd_args.default_dir\") \n" +
                              "trick.var_add(\"trick_cmd_args.cmd_args.cmdline_name\") \n" +
                              "trick.var_add(\"trick_cmd_args.cmd_args.input_file\") \n" +
                              "trick.var_add(\"trick_cmd_args.cmd_args.run_dir\") \n");
            
            if (masterslave_enabled) {
                commandSimcom.put("trick.var_add(\"trick_master_slave.master.num_slaves\") \n");
            }

            commandSimcom.put("trick.var_send() \n" +
                              "trick.var_clear() \n");

            results = commandSimcom.get().split("\t");
            if (results != null && results.length > 0) {
                execTimeTicValue = Double.parseDouble(results[3]);
                simStartTime = Double.parseDouble(results[1]);
                long terminateTime = Long.parseLong(results[2]);                
                if (terminateTime >= Long.MAX_VALUE - 1) {
                	enableProgressBar = false;
                }
                
                // need to minus the sim start time as it could be a number other than 0.0
                simStopTime = terminateTime/execTimeTicValue - simStartTime;
            }

            slaveCount = masterslave_enabled ? Integer.parseInt(results[8]) : 0;

            simRunDirField = new JTextField[slaveCount+1];
            overrunField = new JTextField[slaveCount+1];

            for (int i = 0; i < simRunDirField.length; i++) {
                if (i==0) {
                    simRunDirField[i] = new JTextField(results[4] + java.io.File.separator + results[5] + " " + results[6]);
                } else {
                    simRunDirField[i] = new JTextField();
                }
                overrunField[i] = new JTextField("    ");
                overrunField[i].setPreferredSize( new Dimension(60, overrunField[i].getHeight()) );
            }
            simRunDir = results[7];
            simRunDir = results[4] + java.io.File.separator + simRunDir;

            simState.setRunPath(simRunDir);

            for (int i = 1; i < simRunDirField.length; i++) {
            	/**
            	 * Commented out the following code as slaves is a vector and can't be accessed at this point.
                 * Uncomment the following code if we can in the future.
            	 */
                /*commandSimcom.put("trick.sim_services.var_add(\"master_slave.master.slaves[" + i + "].sim_path\") \n" +
                                 "trick.sim_services.var_add(\"master_slave.master.slaves[" + i + "].S_main_name\") \n ");
                                  "trick.sim_services.var_add(\"master_slave.master.slaves[" + i + "].run_input_file\") \n" +
                                  "trick.sim_serives.var_send( ) \n" +
                                  "trick.sim_services.var_clear( ) \n");
                results = commandSimcom.get().split("\t");
                simRunDirField[i].setText(results[1] + java.io.File.separator + results[2] + " " + results[2]);*/
            	simRunDirField[i].setText("Slave " + i);
            }
            
            commandSimcom.put("trick.var_exists(\"trick_instruments.debug_pause.debug_pause_flag\")\n") ;
            results = commandSimcom.get().split("\t");
            debug_present = Integer.parseInt(results[1]);

            commandSimcom.put("trick.var_exists(\"trick_real_time.rt_sync.total_overrun\")\n") ;
            results = commandSimcom.get().split("\t");
            overrun_present = Integer.parseInt(results[1]);

            commandSimcom.put("trick.var_exists(\"trick_message.mdevice.port\")\n") ;
            results = commandSimcom.get().split("\t");
            message_present = Integer.parseInt(results[1]);

            if ( message_present == 1 ) {
                commandSimcom.put("trick.var_add(\"trick_message.mdevice.port\") \n" +
                                  "trick.var_send() \n" +
                                  "trick.var_clear() \n");
                results = commandSimcom.get().split("\t");
                message_port = Integer.parseInt(results[1]) ;

            }
            // If simOverrunPanel is already created, meaning the GUI was setup without connecting to the server.
            // Now, the user hits the Connect button to connect. Therefore, we need to update this panel once
            // it gets connected.
            if (simOverrunPanel != null) {
                (new RebuildSimOverrunPanelTask()).execute();
            }
        }
        catch (NumberFormatException nfe) {

        }
        catch (IOException e) {

        }
        catch (NullPointerException npe) {
            npe.printStackTrace();
        }
    }

    //========================================
    //    Methods
    //========================================
    /**
     * Invoked when SimStatusMonitor task's progress property changes.
     * This is required by {@link PropertyChangeListener}.
     */
    public void propertyChange(PropertyChangeEvent evt) {
        if ("progress" == evt.getPropertyName()) {
          int progress = (Integer) evt.getNewValue();
          progressBar.setValue(progress);
        }

    }

    /**
     * Cleans up the socket communication before exiting the application.
     */
    @Override
    protected void shutdown() {
        super.shutdown();
        try {
            if (commandSimcom != null) {
                commandSimcom.close();
            }
            if (statusSimcom != null) {
                statusSimcom.close();
            }
            if (healthStatusSocketChannel != null) {
            healthStatusSocketChannel.close() ;
            }
        }
        catch (java.io.IOException ioe) {
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
        actionController = new SimControlActionController();

        getInitializationPacket();
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
    	super.ready();

    	logoImagePanel.start();

    	// The following code was commented out and moved to the end of startup()
    	// due to framework having issues with certain Java versions. In certain Java
    	// version such as JDK1.6.0_20, ready() never gets called???
    	// 05-24-2011, found out that if there was any SwingTask or Thread started
    	// before ready() in application framework, JDK1.6.0_20 would fail to realize
    	// the startup() is done. That's why ready() never gets called even though startup()
    	// is done. So modified the code to start the logo animation player after startup()
    	// and moved the following code back to where it should be.
        if (commandSimcom == null) {
        	logoImagePanel.pause();
            Object[] keys = actionMap.allKeys();
            // If there is no server connection, disable all actions except connect and quit.
            for (int i = 0; i < keys.length; i++) {
                String theKey = (String)keys[i];
                if (!(theKey.equals("connect") || theKey.equals("quit"))) {
                    getAction(theKey).setEnabled(false);
                }
            }
            String errMsg = "No server connection. Please connect!";
			printErrorMessage(errMsg);
            return;
        }
        
        if (isRestartOptionOn) {
        	printSendHS();
        }
        
        scheduleGetSimState();

        startStatusMonitors();
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

        // dont't want to the confirmation dialog for sim control panel
    	removeExitListener(exitListener);

        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());
        view.setToolBar(createToolBar());
        view.setStatusBar(createStatusBar());

        if(errOnInitConnect && (runningSimList != null) ) {
            runningSimList.addItem("localhost : " + port);  
        } 

        show(view);
    }

	/**
	 * Prints an error message to the status message pane. In the event there is an error with it, a JOptionPane will pop up.
	 * @param err
	 */
	protected void printErrorMessage(String err) {
		try {
			// Get the document attached to the Status Message Pane 
			Document doc = statusMsgPane.getDocument();

			// Set the font color to red and the background to black
			StyleContext sc = new StyleContext();               
			Style redStyle = sc.addStyle("Red", null);
			setColorStyleAttr(redStyle, Color.red, Color.black);

			// Add the error message to the bottom of the message pane
			doc.insertString(doc.getLength(), err + "\n", redStyle);

			// If Lite mode is engaged, or the window is small enough 
			// to obscure the message pane, create a popup for the error as well.
			if (liteButton.isSelected() || getMainFrame().getSize().height <= LITE_SIZE.height + 50) {
				JOptionPane.showMessageDialog(getMainFrame(), err, "Sim Control Panel Error", JOptionPane.ERROR_MESSAGE);
			}
		} catch (BadLocationException ble) {
			JOptionPane.showMessageDialog(getMainFrame(), 
										  "Status Message Pane had an issue when printing: " + err, 
										  "Status Message Pane Error", 
										  JOptionPane.ERROR_MESSAGE);
		} catch (NullPointerException npe) {
			System.err.println( "Sim Control Error at Initialization: \n" + err);
		}
	}

    /**
     * Main method for this application.
     * @param args command line arguments
     */
    public static void main(String[] args) {
        Application.launch(SimControlApplication.class, args);
        
        // Arrays.toString(args) converts such as localhost 7000 -r to [localhost, 7000, -r],
        // so need to remove [, ] and all white spaces.
        String commandLine = (Arrays.toString(args)).replace("[","").replace("]", "").replaceAll("\\s+", "");

        // check to see if -r or -restart is used
        Pattern restartOptionPattern = Pattern.compile("(\\-r|\\-restart)(,|$)");
        Matcher matcher = restartOptionPattern.matcher(commandLine);

        // if -r | -restart is used, set the flag and then remove it from the command line 
        if (matcher.find()) {
            isRestartOptionOn = true;
            commandLine = matcher.replaceAll("");
        }

        // check to see if -auto_exit is used
        Pattern autoExitOptionPattern = Pattern.compile("(\\-auto\\_exit)(,|$)");
        Matcher autoExitMatcher = autoExitOptionPattern.matcher(commandLine);

        if (autoExitMatcher.find()) {
            isAutoExitOn = true;
            commandLine = autoExitMatcher.replaceAll("");            
        } 
        
        Scanner commandScanner = new Scanner(commandLine).useDelimiter(",");
        // now need to figure out host and port, if not specified, available host&port will be listed
        if (commandScanner.hasNextInt()) {
        	port = commandScanner.nextInt();
        	if (commandScanner.hasNext()) {
        		host = commandScanner.next();
        	}
        } else {
        	if (commandScanner.hasNext()) {
        		host = commandScanner.next();
        		if (commandScanner.hasNextInt()) {
        			port = commandScanner.nextInt();
        		}
        	}
        }      
        
        if (commandScanner != null) {
        	commandScanner.close();
        }
    }

    /**
     * Helper method to print the send_hs file to the statusMsgPane.
     */
    private void printSendHS() {
    	if (simState != null) {
    		File sendHS = new File(simState.getRunPath() + java.io.File.separator + "send_hs");
    		if (!sendHS.exists()) {
    			return;
    		}
    		
    		String lineText = null;
    		
    		Document doc = statusMsgPane.getDocument();
            StyleContext sc = new StyleContext();
            
            // normal style is white on black
            Style defaultStyle = sc.addStyle("Default", null);
            setColorStyleAttr(defaultStyle, Color.white, Color.black);
            
            BufferedReader reader = null;
    		try {   			
    			reader = new BufferedReader(new FileReader(sendHS));
    			while ((lineText = reader.readLine()) != null) {
    				doc.insertString(doc.getLength(), lineText+System.getProperty("line.separator"), defaultStyle);
    			}   				
    		} catch (FileNotFoundException fnfe) {
    			// do nothing
    		} catch (IOException ioe) {
    			// do nothing
    		} catch (BadLocationException ble) {
    			// do nothing
    		}
    		
    		finally {
    			try {
    				if (reader != null) {
    					reader.close();
    				}
    			} catch (IOException ioe) {
    				
    			}
    		}
    	}
    }
    
    /**
     * Adds all the variables to variable server for getting SIM states.
     */
    private void scheduleGetSimState() {
        try {
            statusSimcom = new VariableServerConnection(host, port);
            statusSimcom.put("trick.var_set_client_tag(\"SimControl2\")\n");

            // whenever there is data in statusSimcom socket, do something
            String status_vars ;

            status_vars = "trick.var_add(\"trick_sys.sched.time_tics\") \n" +
                          "trick.var_add(\"trick_sys.sched.mode\") \n" +
                          "trick.var_add(\"trick_real_time.rt_sync.actual_run_ratio\") \n" +
                          "trick.var_add(\"trick_real_time.rt_sync.active\") \n";

            if ( debug_present != 0 ) {
                status_vars += "trick.var_add(\"trick_instruments.debug_pause.debug_pause_flag\")\n" ;
            }
            if ( overrun_present != 0 ) {
                status_vars += "trick.var_add(\"trick_real_time.rt_sync.total_overrun\")\n" ;
            }
            statusSimcom.put(status_vars) ;

            statusSimcom.put("trick.var_cycle(0.25)\n");
            setActionsEnabled("connect", false); 
            setGUIEnabled(runningSimList, false);
        }
        catch (NumberFormatException nfe) {

        }
        catch (IOException e) {
            statusLabel.setText("Not Ready");
            statusLabel.setEnabled(false);
            statusSimcom = null;
            getAction("connect").setEnabled(true);           
            runningSimList.setEnabled(true);
            getAction("startSim").setEnabled(false);
        }
    }

	private void setGUIEnabled(Component gui, boolean flag) {
		if(gui != null) gui.setEnabled(flag);
	}

	private void setGUIsEnabled(Component[] guis, boolean flag) {
		for(Component gui : guis)	setGUIEnabled(gui, flag);
	}

    /**
     * Convenient method for setting the state of specified actions.
     *
     * @param actsStr    All actions that need setting state. Each action is separated by ",".
     * @param flag        The state is set to for the actions.
     */
    private void setActionsEnabled(String actsStr, boolean flag) {
        if (actsStr != null) {
            String[] acts = actsStr.split(",");
            setActionsEnabled(acts, flag);
        }
    }

    /**
     * Convenient method for setting the state of specified actions.
     *
     * @param acts        The array of all the actions.
     * @param flag        The state is set to for the actions.
     */
    private void setActionsEnabled(String[] acts, boolean flag) {
        if (acts != null) {
            for (int i = 0; i < acts.length; i++) {
                if (getAction(acts[i].trim()) != null) {
                    getAction(acts[i].trim()).setEnabled(flag);
                }
            }
        }
    }


    /**
     * Creates the main panel for this application.
     */
    @Override
	protected JComponent createMainPanel() {
        GridBagConstraints gridBagConstraints = new GridBagConstraints();

        JXPanel litePanel = new JXPanel();
        litePanel.setLayout(new java.awt.GridBagLayout());
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.weightx = 0.5;
        gridBagConstraints.weighty = 0.1;
        runtimeStatePanel = (JXTitledPanel)createRuntimeStatePanel();
        litePanel.add(runtimeStatePanel, gridBagConstraints);

        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;

        gridBagConstraints.gridwidth = 1;
        JPanel commandsPanel = createCommandsPanel();
        litePanel.add(commandsPanel, gridBagConstraints);

        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth= 1;
        JPanel timePanel = createTimePanel();
        litePanel.add(timePanel, gridBagConstraints);

        // default trick logo
        String trickLogoName = resourceMap.getString("trick.logo");

        // user defined trick logo as specified by TRICK_LOGO, use it if it exists.
        if (UIUtils.getTrickLogo() != null && (new File(UIUtils.getTrickLogo())).exists()) {
        	trickLogoName = UIUtils.getTrickLogo();
        }

        logoImagePanel = new AnimationPlayer(trickLogoName);
        logoImagePanel.setToolTipText("Trick Version " + UIUtils.getTrickVersion());

        JSplitPane topPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, litePanel, logoImagePanel);
        topPane.setBorder(null);

        JXPanel bottomPanel = new JXPanel();
        bottomPanel.setLayout(new java.awt.BorderLayout());
        simOverrunPanel = (JXTitledPanel)createSimOverrunPanel();
        bottomPanel.add(simOverrunPanel, BorderLayout.NORTH);
        bottomPanel.add(createStatusMsgPanel(), BorderLayout.CENTER);

        JSplitPane mainPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT, topPane, bottomPanel);
        mainPane.setPreferredSize(new Dimension(800, 600));

        return mainPane;
    }

    /**
     * Create the JMenuBar for this application.  In addition
     * to the @Actions defined here, the menu bar menus include
     * the cut/copy/paste/delete and quit @Actions that are
     * inherited from the Application class.
     */
    @Override
	protected JMenuBar createMenuBar() {
        JMenuBar menuBar = new JMenuBar();
        String[] fileMenuActionNames = {
            "showStatusFont",
            "---",
            "saveStatusMsgs",
            "clearStatusMsgs",
            "---",
            "lookAndFeel",
            "---",
            "quit"
        };
        String[] actionsMenuActionNames = {
            "startTV",
            "startMTV",
            "---",
            "freezeAt",
            "freezeIn",
            "---",
            "checkpointObjects",
            "---",
            "throttle"
        };

        menuBar.add(createMenu("fileMenu", fileMenuActionNames));
        menuBar.add(createMenu("actionsMenu", actionsMenuActionNames));
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
                "startTV",
                "---",
                "startMTV",             
                "---",
                "throttle",             
                "---",
                "openWiki"
            };
        JToolBar toolBar = new JToolBar(); 
        for (String actionName : toolbarActionNames) {
            if (actionName.equals("---")) {
                toolBar.addSeparator();
            } else {
                toolBar.add(createButton(actionName, false));                
            }
        }
        toolBar.add(Box.createHorizontalGlue());
        toolBar.add(UIUtils.getSmallTrickIconLabel());
        return toolBar;
    }

    /**
     * Creates the panel for showing the sim running progress as well as its status text.
     */
    private JPanel createRuntimeStatePanel() {
        JXTitledPanel titledPanel = new JXTitledPanel();
        titledPanel.setBorder(BorderFactory.createTitledBorder(""));
        titledPanel.setTitle("Initial");
        progressBar = new JProgressBar(0, 100);
        progressBar.setValue(0);
        progressBar.setStringPainted(true);
        titledPanel.add(progressBar, BorderLayout.CENTER);
        progressBar.setEnabled(enableProgressBar);
        return titledPanel;
    }

    /**
     * Creates the panel for holding all the command buttons.
     */
    private JPanel createCommandsPanel() {
    	JXTitledPanel titledCommandsPanel = new JXTitledPanel();
    	titledCommandsPanel.setBorder(BorderFactory.createEmptyBorder(5,10,10,10));
    	titledCommandsPanel.setTitle("Commands");
        JXPanel commandsPanel = new JXPanel();
        titledCommandsPanel.setContentContainer(commandsPanel);
        
        // 2 columns and 5 rows, each component has the same width and height.
        GridLayout gridLayout = new GridLayout(5,2,2,4);
        
        commandsPanel.setLayout(gridLayout);

        commandsPanel.add(new JButton(getAction("stepSim")));

        dataRecButton = new JToggleButton(getAction("recordingSim"));
        dataRecButton.setToolTipText("Data Recording On/Off");
        // By default, Data Recording is On.
        dataRecButton.setSelected(true);

        dataRecButton.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent evt) {
                if (evt.getStateChange() == ItemEvent.SELECTED) {                   
                    dataRecButton.setText("Data Rec On");
                } else {                    
                     dataRecButton.setText("Data Rec Off");                  
                }
            }
        });

        realtimeButton = new JToggleButton(getAction("realtime"));
        realtimeButton.setSelected(true);
        
        commandsPanel.add(dataRecButton);
        commandsPanel.add(new JButton(getAction("startSim")));
        commandsPanel.add(realtimeButton);
        commandsPanel.add(new JButton(getAction("freezeSim")));

        dumpChkpntASCIIButton = new JToggleButton(getAction("dumpChkpntASCII"));
        commandsPanel.add(dumpChkpntASCIIButton);

        commandsPanel.add(new JButton(getAction("shutdownSim")));

        loadChkpntButton = new JToggleButton(getAction("loadChkpnt"));
        commandsPanel.add(loadChkpntButton);

        liteButton = new JToggleButton(getAction("lite"));
        liteButton.setSelected(false);
        liteButton.addItemListener(new ItemListener() {
            public void itemStateChanged(ItemEvent evt) {
                if (evt.getStateChange() == ItemEvent.SELECTED) {
                	liteButton.setText("Full");
                } else {
                	liteButton.setText("Lite");
                }
            }
        });

        commandsPanel.add(liteButton);

        commandsPanel.add(new JButton(getAction("quit")));

        return titledCommandsPanel;
    }

    /**
     * Creates the panel for all the time display.
     */
    private JPanel createTimePanel() {
        JXTitledPanel titledTimePanel = new JXTitledPanel();
        titledTimePanel.setBorder(BorderFactory.createEmptyBorder(5,10,10,10));

        titledTimePanel.setTitle("Time");
        JXPanel timePanel = new JXPanel();
        titledTimePanel.setContentContainer(timePanel);

        // 1 column and 4 rows, each component has the same width and height.
        GridLayout gridLayout = new GridLayout(4,1,2,6);

        timePanel.setLayout(gridLayout);

        JXLabel recLabel = new JXLabel("RET (sec)");
        timePanel.add(recLabel);

        recTime = new JTextField("0.00");
        //recTime.setPreferredSize(recTime.getMinimumSize());
        recTime.setEditable(false);
        timePanel.add(recTime);

        /*timePanel.add(new JXLabel("Real Time (sec)        "));

        realtimeTime = new JTextField("0.00");
        realtimeTime.setEditable(false);
        timePanel.add(realtimeTime);

        timePanel.add(new JXLabel("MET"));

        metTime = new JTextField("000:00:00:00");
        metTime.setEditable(false);
        timePanel.add(metTime);

        timePanel.add(new JXLabel("GMT"));


        gmtTime = new JTextField("001:00:00:00");
        gmtTime.setEditable(false);
        timePanel.add(gmtTime);*/

        // extra spaces are for when look and feel is changed to have enough spaces
        timePanel.add(new JXLabel("Sim / Real Time"));

        simRealtimeRatio = new JTextField("0.00");
        //simRealtimeRatio.setPreferredSize(simRealtimeRatio.getMinimumSize());
        simRealtimeRatio.setEditable(false);
        timePanel.add(simRealtimeRatio);

        return titledTimePanel;
    }

    /**
     * Creates the panel for displaying the sim run and overruns information.
     */
    private JPanel createSimOverrunPanel() {
        JXTitledPanel titledPanel = new JXTitledPanel();
        titledPanel.setBorder(BorderFactory.createTitledBorder(""));
        titledPanel.setTitle("Simulations/Overruns");

        addFieldsToSimOverrunPanel(titledPanel);
        return titledPanel;
    }

    /**
     * Convenient method for adding Sim run dir and over run fields to the
     * corresponding panel.
     */
    private void addFieldsToSimOverrunPanel(JXTitledPanel titledPanel) {
    	JPanel panel = new JPanel();
    	panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
    	
        JPanel simRunPanel = new JPanel();
        simRunPanel.setLayout(new BoxLayout(simRunPanel, BoxLayout.Y_AXIS));

        JPanel overRunPanel = new JPanel();
        overRunPanel.setLayout(new BoxLayout(overRunPanel, BoxLayout.Y_AXIS));

        if (simRunDirField == null) {
            simRunDirField = new JTextField[1];
            overrunField = new JTextField[1];
            for (int ii = 0; ii < simRunDirField.length; ii++) {
                simRunDirField[ii] = new JTextField();
                overrunField[ii] = new JTextField();
            }
        }

        for (int i = 0; i < simRunDirField.length; i++) {
            simRunPanel.add(simRunDirField[i]);
            overRunPanel.add(overrunField[i]);
        }
        
        panel.add(simRunPanel);
        panel.add(overRunPanel);

        titledPanel.add(panel, BorderLayout.CENTER);
    }


    /**
     * Creates the panel for displaying the sim health status messages.
     */
    private JPanel createStatusMsgPanel() {
        statusMsgPane = new JXEditorPane();
        statusMsgPane.setDocument(new DefaultStyledDocument());
        statusMsgPane.setEditorKit(new StyledEditorKit());    
        statusMsgPane.putClientProperty(JXEditorPane.HONOR_DISPLAY_PROPERTIES, true);
        statusMsgPane.setBackground(Color.black);

        int curr_font_size = statusMsgPane.getFont().getSize() ;
        Font font = new Font("Monospaced", Font.PLAIN, curr_font_size);
        statusMsgPane.setFont(font);

        JPanel statusMsgPanel = UIUtils.createSearchableTitledPanel("Status Messages", statusMsgPane, new FindBar(statusMsgPane.getSearchable()));
        return statusMsgPanel;
    }

    /**
     * Creates the status bar that displays connection status such as host, port.
     *      
     * @return a {@link JXStatusBar}
     */
    @Override
    //for Java 7, the type of elements of JComboBox needs to be specified to avoid the warning and it's not supported in Java 6
    @SuppressWarnings("unchecked")
	protected JComponent createStatusBar() {
        JXStatusBar statusBar = new JXStatusBar();
        statusLabel = new JXLabel("");
        
        statusBar.add(statusLabel);

        JPanel hostPortPanel = new JPanel();
        hostPortPanel.setLayout(new BorderLayout());
        JXTitledSeparator hostPortHeader = new JXTitledSeparator();
        hostPortHeader.setHorizontalAlignment(SwingConstants.CENTER);
        hostPortHeader.setTitle("Host : Port (Run Info)");
      
        runningSimList = new JComboBox();
        runningSimList.setEditable(true);
        runningSimList.getEditor().addActionListener(new ActionListener(){
        	public void actionPerformed(ActionEvent ae) {
        		connect();
        	}
        });
        String hostPort = null;
        if (host != null && port != -1) {
        	hostPort = host + " : " + port;
        }
        // if host is parsed in as an argument, add it to the hostList
        if (hostPort != null && !UIUtils.comboBoxContains((DefaultComboBoxModel)runningSimList.getModel(), hostPort)) {
    		runningSimList.addItem(hostPort);
    		runningSimList.setSelectedItem(hostPort);
    	}
        
        runningSimList.setToolTipText("<html>Sims that are running and can be connected to. The manual input format is <host><:|whitespace><port>.</html>");
        hostPortPanel.add(hostPortHeader, BorderLayout.NORTH);
        hostPortPanel.add(runningSimList, BorderLayout.CENTER);
               
        statusBar.add(hostPortPanel, JXStatusBar.Constraint.ResizeBehavior.FILL);
        JButton connectBtn = new JButton(getAction("connect"));
        
        statusBar.add(connectBtn);

        return statusBar;
    }

    /**
     * Updates the GUI as needed if SIM states are changed.
     */
    protected void updateGUI() {
        String newStatusDesc = SimState.SIM_MODE_DESCRIPTION[simState.getMode()];

        recTime.setText(simState.getTwoFractionFormatted(simState.getExecOutTime()));

        if (simState.getRealtimeActive() == 1) {
        	if (realtimeButton.getText().equals("RealTime Off")) {
        		realtimeButton.setSelected(true);
        		realtimeButton.setText("RealTime On");
        		getAction("stepSim").setEnabled(true);
        	}
        } else {
        	if (realtimeButton.getText().equals("RealTime On")) {
        		realtimeButton.setSelected(false);
        		realtimeButton.setText("RealTime Off");
        		getAction("stepSim").setEnabled(false);
        	}
        }

        simRealtimeRatio.setText(simState.getTwoFractionFormatted(simState.getSimRealtimeRatio()));

        // Track Master sim overruns
        overrunField[0].setText(Integer.toString(simState.getOverruns()));
        if ( simState.getOverruns() > 0 ) {
            overrunField[0].setForeground(new Color(205, 0, 0));  // red3
        } else {
            overrunField[0].setForeground(Color.getColor("#000000"));
        }
        simOverrunPanel.revalidate();

        // Update the GUI when that status is changed.
        if ( !newStatusDesc.equals(currentSimStatusDesc) ) {

            switch ( simState.getMode() ) {

                case SimState.INITIALIZATION_MODE:
                    enableAllCommands();
                    setActionsEnabled("startSim,freezeSim,recordingSim,realtime,quit", false);
                    logoImagePanel.resume();
                    break;

                case SimState.FREEZE_MODE:
                    if ( currentSimStatusDesc.equals("PreCheckpoint") ) {
                        ;/* Skip a cycle so the checkpoint status has time to display briefly */
                    } else {
                        enableAllCommands();
                        setActionsEnabled("freezeSim,quit", false);
                    }
                    logoImagePanel.pause();
                    break;

                case SimState.DEBUG_STEPPING_MODE:
                case SimState.RUN_MODE:
                    disableAllCommands();
                    setActionsEnabled("freezeSim,lite", true);
                    if (debug_flag != 0) {
                        setActionsEnabled("stepSim,dumpChkpntASCII", true);
                    }
                    logoImagePanel.resume();
                    break;

                case SimState.EXIT_MODE:
                case SimState.COMPLETE_MODE:
                    disableAllCommands();
                    setActionsEnabled( "lite,quit", true );
                    statusLabel.setText("Done");
                    statusLabel.setEnabled(false);
                    logoImagePanel.stop();
                    break;
            }

            runtimeStatePanel.setTitle(newStatusDesc);
            currentSimStatusDesc = runtimeStatePanel.getTitle();
        }
    }

    /**
     * Enable all buttons on the Commands panel.
     */
    private void enableAllCommands() {
        for ( String btn : getAllCommandActions() ) {
            setActionsEnabled( btn, true );
        }
    }

    /**
     * Disable all buttons on the Commands panel.
     */
    private void disableAllCommands() {
        for ( String btn : getAllCommandActions() ) {
            setActionsEnabled( btn, false );
        }
    }

    /**
     * Returns all {@link Action} names associated with the Commands panel.
     */
    private String[] getAllCommandActions() {
        ArrayList<String> actions = new ArrayList<String>();

        actions.add("stepSim,recordingSim,startSim,realtime,freezeSim," +
        		"dumpChkpntASCII,shutdownSim,loadChkpnt,lite,quit");
        return actions.toArray(new String[0]);
    }

    /**
     * Creates a {@link ServerSocketChannel} if possible for echoing the sim health
     * status messages.
     *
     * @param port    The port number the can be used for the newly created channel.
     *
     * @return    An instance of {@link ServerSocketChannel}.
     *
     */
    /*private ServerSocketChannel createServerSocketChannel(int port) {
        if (port < 7200 || port > 7400) {
            throw new IllegalArgumentException("Invalid port: " + port);
        }
        ServerSocketChannel ssChannel = null;
        try {
            // create a non-blocking server socket channel on specified port
            ssChannel = ServerSocketChannel.open();
            ssChannel.configureBlocking(false);
            ssChannel.socket().bind(new InetSocketAddress(port));
        } catch (IOException e) {

        }
        return ssChannel;

    }*/


    //========================================
    //    Inner Classes
    //========================================
    private class RetrieveHostPortTask extends SwingWorker<Void, Void> {
    	
    	private MulticastSocket multicastSocket = null;
    	
    	@Override
        public Void doInBackground() {
    		while (getAction("connect").isEnabled()) {
    			retrieveHostPort();
    		}
            return null;
        }
    	
    	@Override
    	public void done() {
    		if (multicastSocket != null) {
                multicastSocket.close();
            }
    	}
    	
    	/**
    	 * Helper method for retrieving the host and its listening ports.
    	 * 
    	 */
    	//for Java 7, the type of elements of JComboBox needs to be specified to avoid the warning and it's not supported in Java 6
    	@SuppressWarnings("unchecked")
    	private void retrieveHostPort() {
    	    try {
    	    	multicastSocket = new MulticastSocket(9265);
    	        InetAddress group = InetAddress.getByName("239.3.14.15");
    	        multicastSocket.joinGroup(group);
    	         
    	        byte[] buffer = new byte[1024];
    	        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
    	        multicastSocket.receive(packet);

    	        // Remove the trailing newline, and split the tab-delimitted message.
    	        String[] info = new String(packet.getData(), packet.getOffset(), packet.getLength()).replace("\n", "").split("\t");
    	        // Reset the packet length or future messages will be clipped.
    	        packet.setLength(buffer.length);
    	        // version Trick 10 or later	           	       
    	        if (info[7] != null) {	    	        
	    	        if (runningSimList != null) {
	    	        	String hostPort = info[0] + " : " + info[1] + " (" + info[5] + " " + info[6] + ")";
	    	        	if (!UIUtils.comboBoxContains((DefaultComboBoxModel)runningSimList.getModel(), hostPort)) {
	    	        		// only show localhost's resource
	    	        		// TODO: may want to have whole network resource
	    	        		if (InetAddress.getLocalHost().equals(InetAddress.getByName(info[0]))) {
	    	        			runningSimList.addItem(hostPort);
	    	        			runningSimList.setSelectedItem(hostPort);	    	        			
	    	        		}
	    	        	}	    	        	
	    	        }
    	        }
    	    } catch (IOException ioe) {
    	    	// do nothing
    	    }    		
    	}   	
    }
        
    /**
     * Inner class for the task of rebuiding those fields for Sim run & overrun info.
     * This is used when the user launch Sim Control before starting the server.
     */
    private class RebuildSimOverrunPanelTask extends SwingWorker<Void, Void> {
        @Override
        public Void doInBackground() {
            for(int i=2; i<simOverrunPanel.getComponentCount(); i++) {
                simOverrunPanel.remove(simOverrunPanel.getComponent(i));
            }

            addFieldsToSimOverrunPanel(simOverrunPanel);
            return null;
        }
        @Override
		public void done() {
            simOverrunPanel.validate();
        }
    }
    
    /**
     * Helper method for setting style attribute.
     */
    private void setColorStyleAttr(Style st, Color foreground, Color background) {
        st.addAttribute(StyleConstants.Foreground, foreground);
        st.addAttribute(StyleConstants.Background, background);
        st.addAttribute(StyleConstants.Alignment, StyleConstants.ALIGN_LEFT);
    }

    /**
     * Inner class for the task of monitoring health status.
     */
    private class MonitorHealthStatusTask extends Task<Void, Void> {
        public MonitorHealthStatusTask(Application app) {
            super(app);
        }
        
        /*
         * Main task. Executed in background thread.
         */
        @Override
        public Void doInBackground() {
            charset = Charset.forName("ISO-8859-1");
            CharsetDecoder decoder = charset.newDecoder();
            ByteBuffer byteBuffer = ByteBuffer.allocateDirect(1024 * 1024);

            CharBuffer charBuffer = null;
            Document doc = statusMsgPane.getDocument();
            StyleContext sc = new StyleContext();
            
            // normal style is white on black
            Style defaultStyle = sc.addStyle("Default", null);
            
            setColorStyleAttr(defaultStyle, Color.white, Color.black);
            
            // green - is there any reason not using the Color.green?
            Color greenColor = new Color(150,230,30);
            Style greenStyle = sc.addStyle("Green", null);
            setColorStyleAttr(greenStyle, greenColor, Color.black);
            
            // yellow
            Style yellowStyle = sc.addStyle("Yellow", null);
            setColorStyleAttr(yellowStyle, Color.yellow, Color.black);
            
            // red
            Style redStyle = sc.addStyle("Red", null);
            setColorStyleAttr(redStyle, Color.red, Color.black);
            
            // cyan
            Style cyanStyle = sc.addStyle("Cyan", null);
            setColorStyleAttr(cyanStyle, Color.cyan, Color.black);
            
            try {
                healthStatusSocketChannel = SocketChannel.open() ;
                healthStatusSocketChannel.configureBlocking(true) ;

                healthStatusSocketChannel.connect(new InetSocketAddress(host, message_port)) ;
            } catch (IOException e) {
            } catch (UnresolvedAddressException uae) {
                /** Connection to variable server is not working with the
                 *  current hostname, but it is working if localhost is
                 *  substituted as the hostname. So change it now
                 *  for future use when TV and MTV are launched.
                 */
                host = LOCALHOST;
            }

            while (true) {
                try {
                    if (healthStatusSocketChannel != null) {
                        try {
                            byteBuffer.clear();
                            int numBytesRead = healthStatusSocketChannel.read(byteBuffer);
                            if (numBytesRead == -1) {
                                continue;
                            } else {
                                byteBuffer.flip();
                                try {
                                    charBuffer = decoder.decode(byteBuffer);

                                    String charStr = charBuffer.toString();

                                    int returnCounts = statusMsgPane.getText().split("\n", -1).length;

                                    if (returnCounts > 99999) {
                                    	doc.remove(0, doc.getLength());
                                    }

                                    // interpret ansi escape color sequences
                                    String tokens[] = charStr.split("\033") ;
                                    // ansi escape is 4 characters: [NNm where NN is 2 digit color number
                                    for (String token : tokens) {
                                        int ansicolor = 0 ;
                                        String coloredpart = token;
                                        if (token.charAt(0) == '\133') { // open square bracket
                                        	// get the 1st m locaction
                                        	int mLoc = token.indexOf('m');
                                        	if (mLoc != -1) {
                                        		try {
                                        			ansicolor = Integer.parseInt(token.substring(1, mLoc));
                                        			coloredpart = token.substring(mLoc+1);
                                        		} catch (Exception ex) {
                                        			// do nothing, coloredpart is printed in normal
                                        		}
                                        	}
                                        }
                                        if (coloredpart.length() == 0) {
                                            continue;
                                        }
                                        switch (ansicolor) {
                                            case 0 :  // normal
                                                doc.insertString(doc.getLength(), coloredpart, defaultStyle) ;
                                                break;
                                            case 32 : // green
                                                doc.insertString(doc.getLength(), coloredpart, greenStyle) ;
                                                break;
                                            case 33 : // yellow
                                                doc.insertString(doc.getLength(), coloredpart, yellowStyle) ;
                                                break;
                                            case 31 : // red
                                                doc.insertString(doc.getLength(), coloredpart, redStyle) ;
                                                break;
                                            case 36 : // cyan
                                                doc.insertString(doc.getLength(), coloredpart, cyanStyle) ;
                                                break;
                                            default : // normal
                                                doc.insertString(doc.getLength(), coloredpart, defaultStyle) ;
                                                break;
                                        }
                                    }
                                    // Always scroll to the end
                                    statusMsgPane.setCaretPosition(doc.getLength());
                                    statusMsgPane.validate();

                                } catch (CharacterCodingException e) {
                                    continue;
                                } catch (BadLocationException ble) {
                                    continue;
                                }
                                continue;
                            }
                        } catch (IOException e) {
                            break;
                        } catch (NotYetConnectedException nc) {
                        }
                    }
                } finally {
                    if (runtimeStatePanel.getTitle() == "Sim Complete") {
                        // Always scroll to the end
                        statusMsgPane.setCaretPosition(statusMsgPane.getDocument().getLength());
                        break;
                    }
                }
            }
            return null;
        }

        @Override
        protected void finished() {
            try {
                if (healthStatusSocketChannel != null) {
                    healthStatusSocketChannel.close() ;
                }
            } catch (java.io.IOException ioe) {

            }
        }
    }

    /**
     * Inner class for the task of monitoring the status of the currently running simulation.
     *
     */
    private class MonitorSimStatusTask extends Task<Void, Void> {
        /**
         * Constructor with specified {@link Application}.
         *
         * @param app    The specified {@link Application} that needs Sim status monitoring.
         */
        public MonitorSimStatusTask(Application app) {
            super(app);
        }

        @Override
        protected Void doInBackground() {
            String results[] = null;
            int ii ;

            while (true) {

                try {

                    if (statusSimcom != null) {

                        String resultsStr = statusSimcom.get();
                        if (resultsStr == null) 
                            break;

                        results = resultsStr.split("\t");
                        ii = 1 ;

                        // whenever there is data in statusSimcom socket, do something
                        if (results != null && results[0].equals("0")) {
                            // "trick_sys.sched.time_tics"
                            if (results[ii] != null && results[ii] != "") {
                                double time_tics = Double.parseDouble(results[ii]);                                
                                double execOutTime = time_tics/execTimeTicValue;
                                // TODO: test to see if the commented code needed
                                //if (execOutTime < 0 && simStartTime >= 0) {
                                //	simStartTime = execOutTime;
                                //}
                                simState.setExecOutTime(execOutTime);
                                ii++ ;
                            }

                            // "trick_sys.sched.mode"
                            if (results.length > ii && results[ii] != null && results[ii] != "") {
                                modeIndex = Integer.parseInt(results[ii]);
                                simState.setMode(modeIndex);
                                switch (modeIndex) {
                                    case SimState.INITIALIZATION_MODE:
                                    	//currentSimStatusDesc = "Ready to Run";
                                        //setSimStateDesc(currentSimStatusDesc);
                                        //break;
                                    case SimState.DEBUG_STEPPING_MODE:
                                    case SimState.EXIT_MODE:
                                        break;
                                    // need to setProgress for FREEZE_MODE because a checkpoint could be loaded
                                    // during this mode and that could have a new elapsed time.
                                    case SimState.FREEZE_MODE:
                                    case SimState.RUN_MODE:
                                    	// need to minus the sim start time as it could be a negative number
                                    	setProgress(Math.abs((float)((simState.getExecOutTime()-simStartTime)/simStopTime)));                                    	
                                        break;
                                }
                                ii++ ;
                            }

                            // "real_time.rt_sync.actual_run_ratio"
                            if (results.length > ii && results[ii] != null && results[ii] != "") {
                                simState.setSimRealtimeRatio(Float.parseFloat(results[ii]));
                                ii++ ;
                            }

                            // "real_time.rt_sync.active"
                            if (results.length > ii && results[ii] != null && results[ii] != "") {
                                simState.setRealtimeActive(Integer.parseInt(results[ii]));
                                ii++;
                            }

                            // "instruments.debug_pause.debug_pause_flag"
                            if (debug_present == 1 && results.length > ii && results[ii] != null && results[ii] != "") {
                                debug_flag = Integer.parseInt(results[ii]);
                                if ( debug_flag == 1 ) {
                                    simState.setMode(SimState.DEBUG_STEPPING_MODE);
                                }
                                ii++ ;
                            }

                            // "real_time.rt_sync.total_overrun"
                            if (overrun_present == 1 && results.length > ii && results[ii] != null && results[ii] != "") {
                                simState.setOverruns(Integer.parseInt(results[ii]));
                                ii++ ;
                            }
                            updateGUI();
                        } else {
                            // break the while (true) loop
                            break;
                        }
                    }
                } catch (Exception e) {
                    break;
                }
            } // end while (true)
            return null;
        }

        @Override
        protected void succeeded(Void ignored) {
            simState.setMode(SimState.COMPLETE_MODE);

            // Commented out the following code so that the sim time won't be set unnecessary.
            // However, sometimes you probably will see the sim time is not updated
            // to the stop time when it's completed due to the reason as stated following.
            // Trick 7 seems to have this issue too and I guess people are ok with it.
            /*if (modeIndex != SimState.FREEZE_MODE) {
                // due to the delay on client side, when the server is done and closes the connection,
                // the client might have not received the latest time. so making sure to show
                // the stop time at the end.
                simState.setExecOutTime(simStopTime);
                setProgress(100);
            }*/
            updateGUI();
        }

        @Override
        protected void finished() {
            try {
                if (commandSimcom != null) {
                    commandSimcom.close();
                }
                if (statusSimcom != null) {
                    statusSimcom.close();
                }
                if (isAutoExitOn) {
                	exit();
                }
            }
            catch (IOException e) {
            }
        }
    }
}
