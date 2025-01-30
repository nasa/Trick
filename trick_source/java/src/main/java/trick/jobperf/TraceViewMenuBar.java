package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

/**
* Class TraceViewMenuBar represents the menu bar of the JobPerf application.
* It aggregates the following GUI components:
* JMenuBar [this]
*     JMenu [fileMenu]
*         JMenuItem [fileMenuExit], Action: Call System.exit(0);
*     JMenu [viewMenu]
*         JMenu [traceSizeMenu]
*             JMenuItem [traceSizeIncrease], Action: Call traceView.incrementTraceWidth().
*             JMenuItem [traceSizeDecrease], Action: Call traceView.decrementTraceWidth()
*/
public class TraceViewMenuBar extends JMenuBar implements ActionListener {

    private TraceViewCanvas traceView;

    /**
     * Constructor
     * @param tvc the TraceViewCanvas to be controlled by this menu bar.
     */
    public TraceViewMenuBar(TraceViewCanvas tvc) {
        traceView = tvc;

        JMenu fileMenu = new JMenu("File");
        JMenuItem fileMenuExit = new JMenuItem("Exit");
        fileMenuExit.setActionCommand("exit");
        KeyStroke ctrlQ  = KeyStroke.getKeyStroke('Q', InputEvent.CTRL_MASK );
        fileMenuExit.setAccelerator(ctrlQ);
        fileMenuExit.addActionListener(this);
        fileMenu.add(fileMenuExit);
        add(fileMenu);

        JMenu viewMenu = new JMenu("View");

        JMenuItem traceSizeIncrease = new JMenuItem("Increase Trace Width");
        traceSizeIncrease.setActionCommand("increase-trace_width");
        KeyStroke ctrlPlus  = KeyStroke.getKeyStroke('=', InputEvent.CTRL_MASK );
        traceSizeIncrease.setAccelerator(ctrlPlus);
        traceSizeIncrease.addActionListener(this);
        viewMenu.add(traceSizeIncrease);

        JMenuItem traceSizeDecrease = new JMenuItem("Decrease Trace Width");
        traceSizeDecrease.setActionCommand("decrease-trace_width");
        KeyStroke ctrlMinus = KeyStroke.getKeyStroke('-', InputEvent.CTRL_MASK);
        traceSizeDecrease.setAccelerator(ctrlMinus);
        traceSizeDecrease.addActionListener(this);
        viewMenu.add(traceSizeDecrease);

        viewMenu.addSeparator();

        JMenuItem showFrame = new JMenuItem("Frame Details ...");
        showFrame.setActionCommand("expand-selected-frame");
        KeyStroke ctrlF = KeyStroke.getKeyStroke('F', InputEvent.CTRL_MASK);
        showFrame.setAccelerator(ctrlF);
        showFrame.addActionListener(this);
        viewMenu.add(showFrame);

        JMenuItem showStats = new JMenuItem("Job Statistics ...");
        showStats.setActionCommand("show-job-stats");
        KeyStroke ctrlV = KeyStroke.getKeyStroke('V', InputEvent.CTRL_MASK);
        showStats.setAccelerator(ctrlV);
        showStats.addActionListener(this);
        viewMenu.add(showStats);

        add(viewMenu);

    }
    @Override
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increase-trace_width":
                traceView.incrementTraceWidth();
            break;
            case "decrease-trace_width":
                traceView.decrementTraceWidth();
            break;
            case "expand-selected-frame":
                traceView.displaySelectedFrame();
            break;
            case "show-job-stats":
                traceView.jobStats.SortByID();
                traceView.displayJobStatsWindow();
            break;
            case "exit":
                System.exit(0);
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }
}
