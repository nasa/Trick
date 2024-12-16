//========================================
//    Package
//========================================
package trick.rtperf;

//========================================
//    Imports
//========================================
import java.awt.Color;
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
import java.util.HashMap;
import java.util.List;
import java.util.Map;
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
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JSplitPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
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
//import trick.rtperf.rtPerf.JobExecutionEvent;
import trick.simcontrol.utils.SimControlActionController;
import trick.simcontrol.utils.SimState;

import org.jfree.chart.*;
import org.jfree.data.general.DefaultPieDataset;

import java.util.Timer;
import java.util.TimerTask;

public class rtPerf extends TrickApplication implements PropertyChangeListener
{
    
    //========================================
    //    Public data
    //========================================


    //========================================
    //    Protected data
    //========================================


    //========================================
    //    Private Data
    //========================================
    private int modeIndex = -1;
    private int debug_flag ;
    private int debug_present ;
    private int overrun_present ;
    private int message_present ;
    private int message_port ;
    
    /** whether automatically exit when sim is done/killed. */
    private static boolean isAutoExitOn;

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

    /*
     *  The action controller that performs actions for such as clicking button, selection a menu item and etc.
     */
    private SimControlActionController actionController;

    // The animation image player panel
    private AnimationPlayer logoImagePanel;

    // VariableServerConnection for sending/receiving Variable Server commands.
    private VariableServerConnection commandSimcom;
    // VariableServerConnection for receiving Sim state from Variable Server.
    private VariableServerConnection statusSimcom;

    // Socket for receiving health and status messages
    private SocketChannel healthStatusSocketChannel ;
   
    private JComboBox runningSimList;
    private static String host;
    private static int port = -1;
    private static boolean isRestartOptionOn;
    //True if an error was encountered during the attempt to connect to Variable Server during intialize()
    private boolean errOnInitConnect = false;
    //Time out when attempting to establish connection with Variable Server in milliseconds
    private int varServerTimeout = 5000;
    
    // The object of SimState that has Sim state data.
    private SimState simState;
    private String customizedCheckpointObjects;

    private static Charset charset;
    

    final private static String LOCALHOST = "localhost";

	final private Dimension FULL_SIZE = new Dimension(680, 640);
	final private Dimension LITE_SIZE = new Dimension(340, 360);

    private DefaultPieDataset dataset;
    private JFreeChart chart;
    private ChartPanel chartPanel;
    private JPanel mainPanel;

    /**
     * Connects to the variable server if {@link VariableServerConnection} is able to be created successfully and
     * starts the communication server for sim health status messages.
     */
    @Action
    public void connect()
    {
        System.err.println("Host is " + host + " and port is " + port);
        // get host and port for selected sim  	
//        if (runningSimList != null && runningSimList.getSelectedItem() != null)
//        {
//            System.err.println("Line 1\n");
//        	String selectedStr = runningSimList.getSelectedItem().toString();
//        	// remove the run info if it is shown
//        	int leftPre = selectedStr.indexOf("(");
//        	if (leftPre != -1)
//            {
//                System.err.println("Line 2\n");
//        		selectedStr = selectedStr.substring(0, leftPre);
//        	}
//        	// can be separated either by : or whitespace
//        	String[] elements = selectedStr.split(":");
//        	
//        	if (elements == null || elements.length < 2)
//            {
//                System.err.println("Line 3\n");
//        		elements = selectedStr.split("\\s+");
//        	}
//        	
//        	if (elements == null || elements.length < 2)
//            { 
//                System.err.println("Line 4\n");
//				String errMsg = "Can't connect! Please provide valid host name and port number separated by : or whitespace!";		
//                printErrorMessage(errMsg);
//        		return;
//        	}
//
//            System.err.println("Line 5\n");
//        	host = elements[0].trim();
//        	
//            try
//            {
//        	    port = Integer.parseInt(elements[1].trim());
//        	}
//            catch (NumberFormatException nfe)
//            {
//                System.err.println("Line 6\n");
//				String errMsg = elements[1] + " is not a valid port number!";
//        		printErrorMessage(errMsg);
//        		return;
//        	}
//        }

        getInitializationPacket();
        
        if (commandSimcom == null)
        {
            System.err.println("Line 7\n");
			String errMsg = "Sorry, can't connect. Please make sure the availability of both server and port!";
			printErrorMessage(errMsg);
            return;
        }
        else
        {
            System.err.println("Line 8\n");
            Object[] keys = actionMap.allKeys();
            // If there is a server connection established, enable all actions.
            for (int i = 0; i < keys.length; i++)
            {
                String theKey = (String)keys[i];
                getAction(theKey).setEnabled(true);
            }
        }

        scheduleGetSimState();

    }

    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Gets the initialization packet from Variable Server if it is up.
     */
    public void getInitializationPacket()
    {    	
        String simRunDir = null;
        String[] results = null;      
        try
        {
			String errMsg = "Error: RealTimePerformanceApplication:getInitializationPacket()";
            try
            {
            	if (host != null && port != -1)
                {
            		commandSimcom = new VariableServerConnection(host, port, varServerTimeout);
            	}
                else
                {
            		commandSimcom = null;
            	}
            }
            catch (UnknownHostException host_exception)
            {
                /** The IP address of the host could not be determined. */
                errMsg += "\n Unknown host \""+host+"\"";
                errMsg += "\n Please use a valid host name (e.g. localhost)";
                errOnInitConnect = true;   
		printErrorMessage(errMsg); 
            }
            catch (SocketTimeoutException ste)
            {
                /** Connection attempt timed out. */
                errMsg += "\n Connection Timeout \""+host+"\"";
                errMsg += "\n Please try a different host name (e.g. localhost)";
                errOnInitConnect = true;
                printErrorMessage(errMsg);           
            }
            catch (IOException ioe)
            {
                /** Port number is unavailable, or there is no connection, etc. */
                errMsg += "\n Invalid TCP/IP port number \""+port+"\"";
                errMsg += "\n Please check the server and enter a proper port number!";
                errMsg += "\n IOException ..." + ioe;
                errMsg += "\n If there is no connection, please make sure SIM is up running properly!";
                errOnInitConnect = true;
		        printErrorMessage(errMsg);
            } 
            
            if (commandSimcom == null)
            {
            	(new RetrieveHostPortTask()).execute();
            	return;
            }
                       
            actionController.setVariableServerConnection(commandSimcom);

//            simState = new SimState();

            // Sends commands to the variable server to add several variables for retrieving simulation details.
            commandSimcom.put("trick.var_set_client_tag(\"rtPerf\")\n");
            commandSimcom.put("trick.var_add(\"trick_frame_log.frame_log.target_job_array.frame_time_seconds\")\n"); 
            commandSimcom.put("trick.var_send()\n");

            results = commandSimcom.get().split("\t");
            System.out.println(results[0] + " and " + results[1]);
//            if (results != null && results.length > 0)
//            {
//                execTimeTicValue = Double.parseDouble(results[3]);
//                simStartTime = Double.parseDouble(results[1]);
//                long terminateTime = Long.parseLong(results[2]);                
//                if (terminateTime >= Long.MAX_VALUE - 1)
//                {
//                	enableProgressBar = false;
//                }
//                
//                // need to minus the sim start time as it could be a number other than 0.0
//                simStopTime = terminateTime/execTimeTicValue - simStartTime;
//            }
//
//
//            simRunDirField = new JTextField[slaveCount+1];
//            overrunField = new JTextField[slaveCount+1];
//
//            for (int i = 0; i < simRunDirField.length; i++)
//            {
//                if (i==0)
//                {
//                    simRunDirField[i] = new JTextField(results[4] + java.io.File.separator + results[5] + " " + results[6]);
//                }
//                else
//                {
//                    simRunDirField[i] = new JTextField();
//                }
//                overrunField[i] = new JTextField("    ");
//                overrunField[i].setPreferredSize( new Dimension(60, overrunField[i].getHeight()) );
//            }
//            simRunDir = results[7];
//            simRunDir = results[4] + java.io.File.separator + simRunDir;
//
//            simState.setRunPath(simRunDir);
//
//            
//            for (int i = 1; i < simRunDirField.length; i++)
//            {
//            	simRunDirField[i].setText("Slave " + i);
//            }
//        
//
//            if ( message_present == 1 )
//            {
//                commandSimcom.put("trick.var_add(\"trick_message.mdevice.port\") \n" +
//                                  "trick.var_send() \n" +
//                                  "trick.var_clear() \n");
//                results = commandSimcom.get().split("\t");
//                message_port = Integer.parseInt(results[1]) ;
//
//            }
        }
        catch (NumberFormatException nfe)
        {

        }
        catch (IOException e)
        {

        }
        catch (NullPointerException npe)
        {
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
    public void propertyChange(PropertyChangeEvent evt)
    {
        if ("progress" == evt.getPropertyName())
        {
          int progress = (Integer) evt.getNewValue();
          progressBar.setValue(progress);
        }

    }

    /**
     * Cleans up the socket communication before exiting the application.
     */
    @Override
    protected void shutdown()
    {
        super.shutdown();
        try
        {
            if (commandSimcom != null)
            {
                commandSimcom.close();
            }
            if (statusSimcom != null)
            {
                statusSimcom.close();
            }
            if (healthStatusSocketChannel != null)
            {
            healthStatusSocketChannel.close() ;
            }
        }
        catch (java.io.IOException ioe) { }
    }

    /**
     * Makes initialization as needed. This is called before startup().
     *
     * @see #startup
     */
    @Override
    protected void initialize(String[] args)
    {
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
    protected void ready()
    {
    	super.ready();

    	//logoImagePanel.start();

    	// The following code was commented out and moved to the end of startup()
    	// due to framework having issues with certain Java versions. In certain Java
    	// version such as JDK1.6.0_20, ready() never gets called???
    	// 05-24-2011, found out that if there was any SwingTask or Thread started
    	// before ready() in application framework, JDK1.6.0_20 would fail to realize
    	// the startup() is done. That's why ready() never gets called even though startup()
    	// is done. So modified the code to start the logo animation player after startup()
    	// and moved the following code back to where it should be.
        if (commandSimcom == null)
        {
        	//logoImagePanel.pause();
            Object[] keys = actionMap.allKeys();
            // If there is no server connection, disable all actions except connect and quit.
            for (int i = 0; i < keys.length; i++)
            {
                String theKey = (String)keys[i];
                if (!(theKey.equals("connect") || theKey.equals("quit")))
                {
                    getAction(theKey).setEnabled(false);
                }
            }
            String errMsg = "No server connection. Please connect!";
			printErrorMessage(errMsg);
            return;
        }
        
        if (isRestartOptionOn)
        {
        	printSendHS();
        }
        
        scheduleGetSimState();

    }

    /**
     * Starts building GUI. This is called after initialize.
     * Once startup() is done, ready() is called.
     *
     * @see #initialize
     * @see #ready
     */
    @Override
    protected void startup()
    {
        super.startup();

        // dont't want to the confirmation dialog for sim control panel
    	removeExitListener(exitListener);

        View view = getMainView();
        view.setComponent(createMainPanel());
        view.setMenuBar(createMenuBar());
        view.setToolBar(createToolBar());
        view.setStatusBar(createStatusBar());

        if(errOnInitConnect && (runningSimList != null) )
        {
            runningSimList.addItem("localhost : " + port);  
        } 

        show(view);
    }
    

    

    /**
	 * Prints an error message to the status message pane. In the event there is an error with it, a JOptionPane will pop up.
	 * @param err
	 */
	protected void printErrorMessage(String err)
    {
		try
        {
			// Get the document attached to the Status Message Pane 
			Document doc = statusMsgPane.getDocument();

			// Set the font color to red and the background to black
			StyleContext sc = new StyleContext();               
			Style redStyle = sc.addStyle("Red", null);

			// Add the error message to the bottom of the message pane
			doc.insertString(doc.getLength(), err + "\n", redStyle);

			// If Lite mode is engaged, or the window is small enough 
			// to obscure the message pane, create a popup for the error as well.
			if (liteButton.isSelected() || getMainFrame().getSize().height <= LITE_SIZE.height + 50)
            {
				JOptionPane.showMessageDialog(getMainFrame(), err, "Sim Control Panel Error", JOptionPane.ERROR_MESSAGE);
			}
		}
        catch (BadLocationException ble)
        {
			JOptionPane.showMessageDialog(getMainFrame(), 
										  "Status Message Pane had an issue when printing: " + err, 
										  "Status Message Pane Error", 
										  JOptionPane.ERROR_MESSAGE);
		}
        catch (NullPointerException npe)
        {
			System.err.println( "Sim Control Error at Initialization: \n" + err);
		}
        return;
	}


    /**
     * Helper method to print the send_hs file to the statusMsgPane.
     */
    private void printSendHS()
    {
    	if (simState != null)
        {
    		File sendHS = new File(simState.getRunPath() + java.io.File.separator + "send_hs");
    		if (!sendHS.exists())
            {
    			return;
    		}
    		
    		String lineText = null;
    		
    		Document doc = statusMsgPane.getDocument();
            StyleContext sc = new StyleContext();
            
            // normal style is white on black
            Style defaultStyle = sc.addStyle("Default", null);
            
            BufferedReader reader = null;
    		try
            {   			
    			reader = new BufferedReader(new FileReader(sendHS));
    			while ((lineText = reader.readLine()) != null)
                {
    				doc.insertString(doc.getLength(), lineText+System.getProperty("line.separator"), defaultStyle);
    			}   				
    		}
            catch (FileNotFoundException fnfe)
            {
    			// do nothing
    		}
            catch (IOException ioe)
            {
    			// do nothing
    		}
            catch (BadLocationException ble)
            {
    			// do nothing
    		}
    		
    		finally
            {
    			try
                {
    				if (reader != null)
                    {
    					reader.close();
    				}
    			}
                catch (IOException ioe)
                {
    				
    			}
    		}
    	}
    }

    /**
     * Adds all the variables to variable server for getting SIM states.
     */
    private void scheduleGetSimState()
    {
        try
        {
            statusSimcom = new VariableServerConnection(host, port);
            statusSimcom.put("trick.var_set_client_tag(\"SimControl2\")\n");

            // whenever there is data in statusSimcom socket, do something
            String status_vars ;

            status_vars = "trick.var_add(\"trick_sys.sched.time_tics\") \n" +
                          "trick.var_add(\"trick_sys.sched.mode\") \n" +
                          "trick.var_add(\"trick_real_time.rt_sync.actual_run_ratio\") \n" +
                          "trick.var_add(\"trick_real_time.rt_sync.active\") \n";

            if ( debug_present != 0 )
            {
                status_vars += "trick.var_add(\"trick_instruments.debug_pause.debug_pause_flag\")\n" ;
            }
            if ( overrun_present != 0 )
            {
                status_vars += "trick.var_add(\"trick_real_time.rt_sync.total_overrun\")\n" ;
            }
            statusSimcom.put(status_vars) ;

            statusSimcom.put("trick.var_cycle(0.25)\n");

            getAction("connect").setEnabled(false);         
            //runningSimList.setEnabled(false);
        }
        catch (NumberFormatException nfe)
        {

        }
        catch (IOException e)
        {
            statusLabel.setText("Not Ready");
            statusLabel.setEnabled(false);
            statusSimcom = null;
            getAction("connect").setEnabled(true);           
            //runningSimList.setEnabled(true);
            getAction("startSim").setEnabled(false);
        }
    }

    //========================================
    //    Inner Classes
    //========================================
    private class RetrieveHostPortTask extends SwingWorker<Void, Void>
    {
    	
    	private MulticastSocket multicastSocket = null;
    	
    	@Override
        public Void doInBackground()
        {
    		while (getAction("connect").isEnabled())
            {
    			retrieveHostPort();
    		}
            return null;
        }
    	
    	@Override
    	public void done()
        {
    		if (multicastSocket != null)
            {
                multicastSocket.close();
            }
    	}
    	
    	/**
    	 * Helper method for retrieving the host and its listening ports.
    	 * 
    	 */
    	//for Java 7, the type of elements of JComboBox needs to be specified to avoid the warning and it's not supported in Java 6
    	@SuppressWarnings("unchecked")
    	private void retrieveHostPort()
        {
    	    try
            {
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
    	        if (info[7] != null)
                {	    	        
	    	        if (runningSimList != null)
                    {
	    	        	String hostPort = info[0] + " : " + info[1] + " (" + info[5] + " " + info[6] + ")";
	    	        	if (!UIUtils.comboBoxContains((DefaultComboBoxModel)runningSimList.getModel(), hostPort))
                        {
	    	        		// only show localhost's resource
	    	        		// TODO: may want to have whole network resource
	    	        		if (InetAddress.getLocalHost().equals(InetAddress.getByName(info[0])))
                            {
	    	        			runningSimList.addItem(hostPort);
	    	        			runningSimList.setSelectedItem(hostPort);	    	        			
	    	        		}
	    	        	}	    	        	
	    	        }
    	        }
    	    }
            catch (IOException ioe)
            {
    	    	// do nothing
    	    }    		
    	}   	
    }


    /**
     * Convenient method for setting the state of specified actions.
     *
     * @param actsStr    All actions that need setting state. Each action is separated by ",".
     * @param flag        The state is set to for the actions.
     */
    private void setActionsEnabled(String actsStr, boolean flag)
    {
        if (actsStr != null)
        {
            String[] acts = actsStr.split(",");
            setActionsEnabled(acts, flag);
        }
    }

    // Ask why we need both of these functions since if I only have the one above I get this error "incompatible types: java.lang.String[] cannot be converted to java.lang.String"

    /**
     * Convenient method for setting the state of specified actions.
     *
     * @param acts        The array of all the actions.
     * @param flag        The state is set to for the actions.
     */
    private void setActionsEnabled(String[] acts, boolean flag)
    {
        if (acts != null)
        {
            for (int i = 0; i < acts.length; i++)
            {
                if (getAction(acts[i].trim()) != null)
                {
                    getAction(acts[i].trim()).setEnabled(flag);
                }
            }
        }
    }

    /**
     * Creates the main panel for this application.
     */
    @Override
    protected JComponent createMainPanel()
    {
        DefaultPieDataset dataset = new DefaultPieDataset();
        dataset.setValue("Completed", 60);
        dataset.setValue("In Progress", 30);
        dataset.setValue("Failed", 10);
        
        JFreeChart chart = ChartFactory.createPieChart("Job Execution Status", dataset, true, true, false); // Chart title, dataset, include legend, tooltips, urls

        // Create the chart panel to display the chart
        ChartPanel chartPanel = new ChartPanel(chart);
        
        // Create the main panel to hold the chart panel
        JPanel mainPanel = new JPanel();
        mainPanel.add(chartPanel);

        return mainPanel;
    }

    
    /**
     * Inner class for the task of monitoring health status.
     */
    private class MonitorHealthStatusTask extends Task<Void, Void>
    {
        public MonitorHealthStatusTask(Application app)
        {
            super(app);
        }
        
        /*
         * Main task. Executed in background thread.
         */
        @Override
        public Void doInBackground()
        { 
            return null;
        }

        @Override
        protected void finished()
        {
            try
            {
                if (healthStatusSocketChannel != null)
                {
                    healthStatusSocketChannel.close() ;
                }
            }
            catch (java.io.IOException ioe)
            {

            }
        }
    }

    /**
     * Inner class for the task of monitoring the status of the currently running simulation.
     *
     */
    private class MonitorSimStatusTask extends Task<Void, Void>
    {
        /**
         * Constructor with specified {@link Application}.
         *
         * @param app    The specified {@link Application} that needs Sim status monitoring.
         */
        public MonitorSimStatusTask(Application app)
        {
            super(app);
        }

        @Override
        protected Void doInBackground()
        {
            String results[] = null;
            int ii ;

            while (true)
            {

                try
                {

                    if (statusSimcom != null)
                    {

                        String resultsStr = statusSimcom.get();
                        if (resultsStr == null) 
                            break;

                        results = resultsStr.split("\t");
                        ii = 1 ;

                        // whenever there is data in statusSimcom socket, do something
                        if (results != null && results[0].equals("0"))
                        {
                            // "trick_sys.sched.time_tics"
                            if (results[ii] != null && results[ii] != "")
                            {
                                double time_tics = Double.parseDouble(results[ii]);                                
                                double execOutTime = time_tics/execTimeTicValue;
                                simState.setExecOutTime(execOutTime);
                                ii++ ;
                            }

                            // "trick_sys.sched.mode"
                            if (results.length > ii && results[ii] != null && results[ii] != "")
                            {
                                modeIndex = Integer.parseInt(results[ii]);
                                simState.setMode(modeIndex);
                                switch (modeIndex)
                                {
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
                            if (results.length > ii && results[ii] != null && results[ii] != "")
                            {
                                simState.setSimRealtimeRatio(Float.parseFloat(results[ii]));
                                ii++ ;
                            }

                            // "real_time.rt_sync.active"
                            if (results.length > ii && results[ii] != null && results[ii] != "")
                            {
                                simState.setRealtimeActive(Integer.parseInt(results[ii]));
                                ii++;
                            }

                            // "instruments.debug_pause.debug_pause_flag"
                            if (debug_present == 1 && results.length > ii && results[ii] != null && results[ii] != "")
                            {
                                debug_flag = Integer.parseInt(results[ii]);
                                if ( debug_flag == 1 )
                                {
                                    simState.setMode(SimState.DEBUG_STEPPING_MODE);
                                }
                                ii++ ;
                            }

                            // "real_time.rt_sync.total_overrun"
                            if (overrun_present == 1 && results.length > ii && results[ii] != null && results[ii] != "")
                            {
                                simState.setOverruns(Integer.parseInt(results[ii]));
                                ii++ ;
                            }
                        }
                        else
                        {
                            // break the while (true) loop
                            break;
                        }
                    }
                }
                catch (Exception e)
                {
                    break;
                }
            } // end while (true)
            return null;
        }

        @Override
        protected void succeeded(Void ignored)
        {
            simState.setMode(SimState.COMPLETE_MODE);
        }

        @Override
        protected void finished()
        {
            try
            {
                if (commandSimcom != null)
                {
                    commandSimcom.close();
                }
                if (statusSimcom != null)
                {
                    statusSimcom.close();
                }
                if (isAutoExitOn)
                {
                	exit();
                }
            }
            catch (IOException e)
            {
            }
        }
    }

    public static void main(String[] args)
    {
        Application.launch(rtPerf.class, args);

        // Arrays.toString(args) converts such as localhost 7000 -r to [localhost, 7000, -r],
        // so need to remove [, ] and all white spaces.
        String commandLine = (Arrays.toString(args)).replace("[","").replace("]", "").replaceAll("\\s+", "");

        // check to see if -r or -restart is used
        Pattern restartOptionPattern = Pattern.compile("(\\-r|\\-restart)(,|$)");
        Matcher matcher = restartOptionPattern.matcher(commandLine);

        // if -r | -restart is used, set the flag and then remove it from the command line 
        if (matcher.find())
        {
            isRestartOptionOn = true;
            commandLine = matcher.replaceAll("");
        }

        // check to see if -auto_exit is used
        Pattern autoExitOptionPattern = Pattern.compile("(\\-auto\\_exit)(,|$)");
        Matcher autoExitMatcher = autoExitOptionPattern.matcher(commandLine);

        if (autoExitMatcher.find())
        {
            isAutoExitOn = true;
            commandLine = autoExitMatcher.replaceAll("");            
        } 
        
        Scanner commandScanner = new Scanner(commandLine).useDelimiter(",");
        // now need to figure out host and port, if not specified, available host&port will be listed
        if (commandScanner.hasNextInt())
        {
        	port = commandScanner.nextInt();
        	if (commandScanner.hasNext())
            {
        		host = commandScanner.next();
        	}
        }
        else
        {
        	if (commandScanner.hasNext())
            {
        		host = commandScanner.next();
        		if (commandScanner.hasNextInt())
                {
        			port = commandScanner.nextInt();
        		}
        	}
        }      
        
        if (commandScanner != null)
        {
        	commandScanner.close();
        }
    }
}
