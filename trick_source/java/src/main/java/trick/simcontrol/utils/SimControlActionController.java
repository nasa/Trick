
//========================================
//	Package
//========================================
package trick.simcontrol.utils;

//========================================
//	Imports
//========================================
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

import javax.swing.Box;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextField;

import trick.common.ui.UIUtils;
import trick.common.ui.components.DoubleJSlider;
import trick.common.utils.VariableServerConnection;
import trick.simcontrol.SimControlApplication;

/**
 * The action controller class controls the actions that need to be handled 
 * for {@link SimControlApplication}. There are certain actions done directly 
 * in {@link SimControlApplication} if that way is more convenient.
 * 
 * @author Hong Chen
 * @since Trick 09
 */
public class SimControlActionController {
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	
	/* For sending or receiving data to/from variable server. */
	private VariableServerConnection simcom = null;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Default constructor.
	 */
	public SimControlActionController() {
		
	}
	
	public SimControlActionController(VariableServerConnection simcom) {
		this.simcom = simcom;
	}
	
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Sets VariableServerConnection for communicating with Variable Server.
	 * 
	 * @param simcom	The VariableServerConnection for Variable Server communication.
	 */
	public void setVariableServerConnection(VariableServerConnection simcom) {
		this.simcom = simcom;
	}
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Invoked when the user selects Freeze At ... from Actions menu. 
         * @param execOutTime time to freeze at
         * @param dialogParent dialog parent
	 */
	public void handleFreezeAt(double execOutTime, Component dialogParent) {
		freeze(execOutTime, 0, "Freeze sim at (enter RET):", "Freeze sim at specified time", dialogParent);
	}
	
	/** 
	 * Private helper method that serves both handleFreezeAt and handleFreezeIn.
	 * 
	 * @see #handleFreezeAt
	 * @see #handleFreezeIn
	 */
	private void freeze(double execOutTime, int isRelative, String dialogMsg, String dialogTitle, Component dialogParent) {
		double inputTime = 0.0;
		boolean isValid = true;
		do {
			String input = (String)JOptionPane.showInputDialog(dialogParent,
					   										   dialogMsg,
					   										   dialogTitle,
					   										   JOptionPane.INFORMATION_MESSAGE);
			if (input == null || input == "") {
				return;
			} else {
				try {		
					inputTime = Double.parseDouble(input);
					if (isRelative == 1) {
						inputTime = inputTime+execOutTime;
					}
					if (inputTime < execOutTime) {
						int choice = JOptionPane.showConfirmDialog(dialogParent,
																   "Can't freeze to time in past! Enter another time?",
																   "Freeze time error",
																   JOptionPane.YES_NO_OPTION);
						if (choice == JOptionPane.NO_OPTION) {
							return;
						} 
					} else {
						isValid = false;
					}
				} catch (NumberFormatException nfe) {
					
				}
			}
		} while (isValid);
		
		try {	
			simcom.put("trick.freeze("+ inputTime + ")\n");
		} catch (IOException e) {
			
		}
	}
	
	/**
	 * Invoked when the user selects Freeze In ... from Actions menu. 
         * @param execOutTime time to freeze in
         * @param dialogParent dialog parent
	 */
	public void handleFreezeIn(double execOutTime, Component dialogParent) {
		freeze(execOutTime, 1, "Freeze in this many seconds:", "Freeze sim in specified time", dialogParent);
	}
	
	/**
	 * Invoked when the user selects Checkpoint Objects ... from Actions menu.
         * @param dialogParent dialog parent
         * @param initObjects list of objects to checkpoint
         * @return list of objects to checkpoint
	 */
	public String handleCheckpointObjects(Component dialogParent, String initObjects) {
		return (String)JOptionPane.showInputDialog(dialogParent,
												   "Enter checkpoint objects (separated by ,):",
												   "Specify checkpoints objects",
												   JOptionPane.INFORMATION_MESSAGE,
												   null,
												   null,
												   initObjects);
	}
	
	/**
	 * Invoked when the user selects Throttle ... from Actions menu. 
         * @param dialogParent dialog parent
	 */
	public void handleThrottle(Component dialogParent) {
		double newRatio = -1.0;
		
		JOptionPane optionPane = new JOptionPane();
		double min = 0.1;
		double max = 10.0;
		final DoubleJSlider slider = new DoubleJSlider(min, max, 1.0, 10.0);
		slider.setMajorTickSpacing(10);
		slider.setMinorTickSpacing(1);
		slider.setPaintTicks(true);
		slider.setSnapToTicks(true);

		JLabel minLabel = new JLabel("Min:");
		final JTextField minField = new JTextField(Double.toString(min));
		
		minField.addActionListener( 
			new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					slider.setDoubleMinimum(Double.valueOf(minField.getText()));
				}
			});
		
		JLabel maxLabel = new JLabel("Max:");
		final JTextField maxField = new JTextField(Double.toString(max));
		maxField.addActionListener( 
				new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						// CTLaChance - 05-22-17
						// Cap the maximum possible value to 1000.
						if(Double.valueOf(maxField.getText()) > 1000) {
							maxField.setText(Double.toString(1000));
							slider.setDoubleMaximum(1000);
						}
						else {
							slider.setDoubleMaximum(Double.valueOf(maxField.getText()));
						}
					}
				});
		
		Box minMaxBox = Box.createHorizontalBox();
		minMaxBox.add(Box.createVerticalStrut(10));
		minMaxBox.add(minLabel);
		minMaxBox.add(minField);
		
		minMaxBox.add(Box.createHorizontalStrut(30));
		minMaxBox.add(maxLabel);
		minMaxBox.add(maxField);
		
		
		Object msg[] = {"Select a value:", minMaxBox, slider};
		optionPane.setMessage(msg);
		optionPane.setMessageType(JOptionPane.QUESTION_MESSAGE);
		optionPane.setOptionType(JOptionPane.OK_CANCEL_OPTION);
		JDialog dialog = optionPane.createDialog(dialogParent, "Select Value");
		dialog.setVisible(true);
		
		int choice = -1;
		if (optionPane.getValue() != null) {
			choice = ((Integer)optionPane.getValue()).intValue();
		}
		if (choice == JOptionPane.OK_OPTION) {
			newRatio = slider.getDoubleValue();
		}
		
		// If the user selects a new ratio
		if (newRatio != -1.0) {
			try {
				simcom.put("trick.real_time_set_rt_clock_ratio(" + newRatio +")\n");
			} catch (IOException e) {
				
			}
		}
	}

	/**
	 * Invoked when the users presses the Step button.
         * @param debug_flag single step the sim
	 */
    public void handleStep( int debug_flag ) {
        try {
            simcom.put("trick.exec_step()\n") ;
        } catch (IOException e) {}
    }
    
    /**
     * Invoked when the user presses the Data Rec On button.
     * @param recording true or false
     */
    public void handleRecOnOff(boolean recording) {
        
        try {
            if (recording) {
                simcom.put("trick.dr_enable()\n");
            } else {
                simcom.put("trick.dr_disable()\n");
            }
        } catch (IOException e) {
            
        }
    }
    
    /**
     * Invoked when the user presses the Start button.
     */
    public void handleStartSim() {
        try {
            simcom.put("trick.exec_run()\n");
        } catch (IOException e) {
            System.out.println("Put failed!");
        }
        
    }
    
    /**
     * Invoked when the user presses the Freeze button.
     * @param debug_flag value of debug 
     */
    public void handleFreeze( int debug_flag ) {
        try {               
            simcom.put("trick.exec_freeze()\n");
            if ( debug_flag == 1 ) {
                simcom.put("trick.debug_pause_off()\n" ) ;
            }
        } catch (IOException e) {
            System.out.println("Put failed! Exception: " + e.getMessage());
        }
    }
	
	/**
	 * Invoked when the user presses Real Time button.
         * @param isRealtime true or false
	 */
	public void handleRealtime(boolean isRealtime) {		
		try {
			if (isRealtime) {
                simcom.put("trick.real_time_enable() \n");
			} else {
				simcom.put("trick.real_time_disable() \n");
			}
		} catch (IOException e) {
			
		}
	}
	
	/**
	 * Invoked when the user presses Dump ASCII Chkpnt button.
         * @param checkpointObjects list of objects to checkpoint
         * @param dir directory to dump checkpoint
         * @param fileName name of checkpoint file
         * @param dialogParent dialogParent
	 */
	public void handleDumpChkpntASCII(String checkpointObjects, String dir, String fileName, Component dialogParent) {	
		if (dir == null) {
			dir = ".";
		}

		// The file name can be redefined by the user.
		File selectedFile = UIUtils.chooseSaveFile(dir, fileName, null, dialogParent);
		if (selectedFile != null) {			
			try {				
				// Make sure there are quotes for file name.			
				if (checkpointObjects == null || "".equals(checkpointObjects)) {					
					//Note: it seems that checkpoint command always put RUN_test in front of the file name,
					//so passing the file name instead of the whole path. 
					//TODO: if users don't like to be forced to save the checkpoing file in RUN_ dir,
					//      modify to use the whole path.
					simcom.put("trick.checkpoint(\"" + selectedFile.getName() + "\")\n");
				} else {					
					simcom.put("trick.checkpoint_objects(\"" + selectedFile.getName() + "\", \""+ checkpointObjects + "\")\n");
				}
				
			} catch (IOException e) {
					
			}
		}
		
	}
	
	/**
	 * Invoked when the user presses Shutdown button.
	 */
	public void handleShutdown() {
		try {	    	
			simcom.put("trick.stop()\n");
		} catch (IOException e) {
			
		}
   	}
	
	/**
	 * Invoked when the user presses Load ASCII Chkpnt button.
         * @param dir directory to load checkpoint
         * @param dialogParent dialogParent
	 */
	public void handleLoadChkpnt(String dir, Component dialogParent) {		
		// If the Sim Run dir is unknown, current dir will be used for the file chooser.
		if (dir == null) {
			dir = ".";
		}
		
		// The file name can be redefined by the user.
		File file = UIUtils.chooseOpenFile(dir, "chkpnt_", null, dialogParent);
		
		if (file != null) {
			try {
				//simcom.put("sys.exec.sim_com.filename = \"" + fileName + "\";");
				//simcom.put("sys.exec.sim_com.command = LdChckCmd ;");				
				simcom.put("trick.load_checkpoint(\"" + file.getAbsolutePath() + "\")\n");
			} catch (IOException e) {
				
			}
		}
	    
	}
	
	/**
	 * Invoked when the user presses Lite button.
	 */
	public void handleLite() {
		// TODO:
		System.out.println("lite...");
	}
	
}
