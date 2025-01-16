package trick.simcontrol;


import org.jdesktop.application.Application;

import trick.simcontrol.utils.SimControlActionController;

import static trick.simcontrol.SimControlApplication.host;
import static trick.simcontrol.SimControlApplication.isAutoExitOn;
import static trick.simcontrol.SimControlApplication.isRestartOptionOn;
import static trick.simcontrol.SimControlApplication.port;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Stack;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.Action;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.AbstractAction;


public class StubbedSimControlApplication extends SimControlApplication {

	public static Stack<ActionID> ActionRecord;
	private static StubbedSimControlApplication the_stub = null;

	public StubbedSimControlApplication() {
		this("", -1);
	}

    public StubbedSimControlApplication(String hostname, int portNum) {
		host = hostname;
        port = portNum;
		actionController = new SimControlActionController();
		ActionRecord = new Stack<ActionID>();

		the_stub = this;
	}

	@Override
	protected void updateGUI() { /* UNUSED */ }

	@Override
	protected void startStatusMonitors() { /* UNUSED */}

	@Override
	protected void ready() {
		super.ready();
		setEnabledAllActions(true); // enable all actions
	}
	
	@Override
	public void startSim() {
		ActionRecord.push(ActionID.START);
	}
	
	@Override
	public void freezeSim() {
		ActionRecord.push(ActionID.FREEZE);
	}

	@Override
	public void connect() {
		ActionRecord.push(ActionID.CONNECT);
	}

	@Override
    public void showStatusFont() {
        ActionRecord.push(ActionID.SET_FONT);
        super.showStatusFont();
    }
    
    @Override
    public void saveStatusMsgs() {
        ActionRecord.push(ActionID.SAVE_STATUS);
    }

    @Override
    public void clearStatusMsgs() {
        ActionRecord.push(ActionID.CLEAR_STATUS);
        super.clearStatusMsgs();
    }

    @Override
    public void startTV() {
        ActionRecord.push(ActionID.TV);
    }

    @Override
    public void startMTV() {
        ActionRecord.push(ActionID.MTV);
    }

    @Override
    public void freezeAt() {
        ActionRecord.push(ActionID.FREEZE_AT);
    }

    @Override
    public void freezeIn() {
        ActionRecord.push(ActionID.FREEZE_IN);
    }

    @Override
    public void checkpointObjects() {
        ActionRecord.push(ActionID.PART_CHKPNT);
    }

    @Override
    public void throttle() {
        ActionRecord.push(ActionID.THROTTLE);
    }

    @Override
    public void stepSim() {
        ActionRecord.push(ActionID.STEP);
    }

    @Override
    public void recordingSim() {
        ActionRecord.push(ActionID.RECORDING);
    }

    @Override
    public void realtime() {
        ActionRecord.push(ActionID.REALTIME);
    }

    @Override
    public void shutdownSim() {
        ActionRecord.push(ActionID.SHUTDOWN);
    }

    @Override
    public void dumpChkpntASCII() {
		ActionRecord.push(ActionID.DUMP_CHKPNT);
    }

    @Override
    public void loadChkpnt() {
        ActionRecord.push(ActionID.LOAD_CHKPNT);
    }

    @Override
    public void lite() {
    	ActionRecord.push(ActionID.LITE);
        super.lite();
    }

    @Override
    public void quit(ActionEvent e) {
        ActionRecord.push(ActionID.EXIT);
        // super.quit(e);
    }

	public static StubbedSimControlApplication getInstance() { 
		return the_stub;
	}

    /**
     * Main method for this application.
     * @param args command line arguments
     */
    public static void main(String[] args) {
        Application.launch(StubbedSimControlApplication.class, args);
        
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
}

enum ActionID {
	CONNECT,
	LITE,
	LOAD_CHKPNT,
	DUMP_CHKPNT,
	PART_CHKPNT,
	START,
	FREEZE,
	FREEZE_IN,
	FREEZE_AT,
	STEP,
	SHUTDOWN,
	REALTIME,
	RECORDING,
	THROTTLE,
	MTV,
	TV,
	SAVE_STATUS,
	CLEAR_STATUS,
	SET_FONT,
    EXIT
}