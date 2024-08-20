/*
 * MtvView.java: The MTV gui
 * @author dstrauss
 * This gui was designed using NetBeans, so it contains automatically generated code!
 */

package trick.mtv;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Vector;

import javax.swing.ButtonGroup;
import javax.swing.DefaultCellEditor;
import javax.swing.Icon;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.Timer;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.DefaultTableCellRenderer;

import trick.common.framework.BaseApplication.ExitListener;
import trick.common.framework.BaseApplication;
import trick.common.framework.task.TaskMonitor;
import trick.common.framework.View;
import trick.common.TrickApplication;
import trick.common.utils.SwingAction;
import trick.common.utils.TrickResources;

/**
 * The application's main frame.
 */
public class MtvView extends View {

    public MtvView(TrickApplication app) {
        super(app);

        //if user closes the window, call exit
        getFrame().setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        getFrame().addWindowListener(new WindowAdapter() {
            @Override
	    public void windowClosing(WindowEvent e) {
                //System.out.println("Close action.");
                MtvApp.getApplication().exit();
            }
        });
        
        try { 
        	resourceMap = parseResources(app.resourceMap); 
        } catch(IOException ioe) {
	        resourceMap = new TrickResources();
        	System.err.println(ioe.getMessage());
        }

        initComponents();
        
        edit_new_event();

        // status bar initialization - message timeout, idle icon and busy animation, etc
        int messageTimeout = resourceMap.getInteger("StatusBar.messageTimeout");
        messageTimer = new Timer(messageTimeout, new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                statusMessageLabel.setText("");
            }
        });
        messageTimer.setRepeats(false);
        int busyAnimationRate = resourceMap.getInteger("StatusBar.busyAnimationRate");
        for (int i = 0; i < busyIcons.length; i++) {
            busyIcons[i] = resourceMap.getIcon("StatusBar.busyIcons[" + i + "]");
        }
        busyIconTimer = new Timer(busyAnimationRate, new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                busyIconIndex = (busyIconIndex + 1) % busyIcons.length;
                statusAnimationLabel.setIcon(busyIcons[busyIconIndex]);
            }
        });
        idleIcon = resourceMap.getIcon("StatusBar.idleIcon");
        statusAnimationLabel.setIcon(idleIcon);
        progressBar.setVisible(false);

        // connecting action tasks to status bar via TaskMonitor
        TaskMonitor taskMonitor = new TaskMonitor(getApplication().getContext());
        taskMonitor.addPropertyChangeListener(new java.beans.PropertyChangeListener() {
            @Override
            public void propertyChange(java.beans.PropertyChangeEvent evt) {
                String propertyName = evt.getPropertyName();
                if ("started".equals(propertyName)) {
                    if (!busyIconTimer.isRunning()) {
                        statusAnimationLabel.setIcon(busyIcons[0]);
                        busyIconIndex = 0;
                        busyIconTimer.start();
                    }
                    progressBar.setVisible(true);
                    progressBar.setIndeterminate(true);
                } else if ("done".equals(propertyName)) {
                    busyIconTimer.stop();
                    statusAnimationLabel.setIcon(idleIcon);
                    progressBar.setVisible(false);
                    progressBar.setValue(0);
                } else if ("message".equals(propertyName)) {
                    String text = (String)(evt.getNewValue());
                    statusMessageLabel.setText((text == null) ? "" : text);
                    messageTimer.restart();
                } else if ("progress".equals(propertyName)) {
                    int value = (Integer)(evt.getNewValue());
                    progressBar.setVisible(true);
                    progressBar.setIndeterminate(false);
                    progressBar.setValue(value);
                }
            }

        });

    } // end MtvView constructor
    
    private TrickResources parseResources(TrickResources base) throws IOException, FileNotFoundException {
		TrickResources prop = new TrickResources(base);
		File resource;
		String path;
    		
    	path = TrickApplication.getResourcePath(this.getClass());
    	System.out.println("PATH: " + path);
    	resource = new File(path);
    	prop.loadProperties(resource);
    	
    	int filePos = path.lastIndexOf("/") + 1;
    	path = path.substring(0, filePos);
    	if(prop.getProperty("PATH") != null)
	    	path += ";" + prop.getProperty("PATH");
    	
    	prop.setProperty("PATH", path);
    	
    	return prop;
    }

    // ================================== General Functions =======================================

    // user clicked Show Exit Confirmation Prompt from file menu
    @SwingAction
    public void show_exit_prompt() {
        MtvApp.confirmExit = confirmExitMenuItem.isSelected();
        if (MtvApp.confirmExit) {
            MtvApp.getApplication().add_my_exit_listener();
        } else {
            for (ExitListener exitListener : MtvApp.getApplication().getExitListeners()) {
                MtvApp.getApplication().removeExitListener(exitListener);    
            }
        }
    }    
    // user clicked Exit from file menu
    @SwingAction
    public void quit() {
        //System.out.println("Quit action.");
        MtvApp.getApplication().exit();
    }

    // user clicked Connect button, or hit return in host field or port field
    @SwingAction
    public void connect() {
        if (!connect_button.isEnabled()) {
            JOptionPane.showMessageDialog(viewPanel, "Already connected.",
                    "Error", JOptionPane.WARNING_MESSAGE);
        } else {
            // get host and port from user text fields
            MtvApp.host = host_field.getText();
            MtvApp.port = Integer.parseInt(port_field.getText());
            MtvApp.getApplication().update();
        }
    }

    // ================================== View Tab Functions =======================================

    // DANNY: NO LONGER USED user clicked Hide Unnamed Events toggle in edit menu
    //@SwingAction
    //public void hide_nonames() {
    //    MtvApp.getApplication().hide_nonames = hideNonamesMenuItem.getState();
    //    if (MtvApp.vscom != null) {
    //        for (int ii=0; ii<trick.mtv.MtvApp.mtv_list_count; ii++) {
    //            if (trick.mtv.MtvApp.mtv_events.get(ii).name.equals("no_name_specified")) {
    //                trick.mtv.MtvApp.mtv_events.get(ii).show = !MtvApp.getApplication().hide_nonames;
    //            }
    //        }
    //        // force update() call in doInBackground task
    //        MtvApp.getApplication().need_update = true;
    //    }
   // }

    // user clicked Customize Event Display... in edit menu
    @SwingAction
    public void customize_event_display() {

        if (connect_button.isEnabled()) {
            JOptionPane.showMessageDialog(viewPanel, "You must be connected to a sim.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
            return;
        }
        final int mtv_list_count = trick.mtv.MtvApp.mtv_list_count;
        // create buttons to select/clear certain kinds of events
        JPanel selectPanel = new JPanel();
        final int select_choices = 5;
        final JRadioButton[] selectbutts = new JRadioButton[select_choices];
        selectbutts[0] = new JRadioButton("Select All", false);
        selectbutts[1] = new JRadioButton("Select Added Events", false);
        selectbutts[2] = new JRadioButton("Select Active Events", false);
        selectbutts[3] = new JRadioButton("Select Unnamed Events", false);
        selectbutts[4] = new JRadioButton("Clear All", false);
        // create scrolled pane to contain event buttons
        final JTabbedPane tabpane = new JTabbedPane(JTabbedPane.TOP, JTabbedPane.WRAP_TAB_LAYOUT);
        class Tab {
            // each tab contains the following:
            JPanel panel;
            JScrollPane scrollPane;
            //JRadioButton butt;
            boolean custom; // keep track when user selects event buttons
            boolean[] select_butt_state;
        }
        final Tab[] tabs = new Tab[20];
        
        // select buttons handler
        ActionListener select_butt_listen = new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                // get the current tab
                int sel = tabpane.getSelectedIndex();
                if (e.getSource().equals(selectbutts[0])) { // SELECT ALL
                    selectbutts[0].setSelected(true);
                    selectbutts[1].setSelected(true);
                    selectbutts[2].setSelected(true);
                    selectbutts[3].setSelected(true);
                    selectbutts[4].setSelected(false);
                    for (int ii=0; ii<mtv_list_count; ii++) {
                        if (trick.mtv.MtvApp.mtv_events.get(ii).butt.isShowing()) {
                            trick.mtv.MtvApp.mtv_events.get(ii).butt.setSelected(true);
                        }
                    }
                    tabs[sel].custom = false;
                }
                if (e.getSource().equals(selectbutts[1]) || // SELECT ADDED
                    e.getSource().equals(selectbutts[2])) { // SELECT ACTIVE
                    selectbutts[4].setSelected(false);
                    for (int ii=0; ii<mtv_list_count; ii++) {
                        if (trick.mtv.MtvApp.mtv_events.get(ii).butt.isShowing()) {
                            trick.mtv.MtvApp.mtv_events.get(ii).butt.setSelected(
                                (selectbutts[1].isSelected() && trick.mtv.MtvApp.mtv_events.get(ii).added)
                             || (selectbutts[2].isSelected() && trick.mtv.MtvApp.mtv_events.get(ii).active));
                        }
                    }
                    tabs[sel].custom = false;
                }
                if (e.getSource().equals(selectbutts[3])) { // SELECT UNNAMED
                    if (!selectbutts[3].isSelected()) {
                        selectbutts[4].setSelected(false);
                    }
                    for (int ii=0; ii<mtv_list_count; ii++) {
                        if (trick.mtv.MtvApp.mtv_events.get(ii).butt.isShowing() &&
                            trick.mtv.MtvApp.mtv_events.get(ii).name.equals("no_name_specified") ) {
                            trick.mtv.MtvApp.mtv_events.get(ii).butt.setSelected(selectbutts[3].isSelected());
                        }
                    }
                    tabs[sel].custom &= true;
                }    

                if (e.getSource().equals(selectbutts[4])) { // CLEAR ALL
                    selectbutts[0].setSelected(false);
                    selectbutts[1].setSelected(false);
                    selectbutts[2].setSelected(false);
                    selectbutts[3].setSelected(false);
                    selectbutts[4].setSelected(true);
                    for (int ii=0; ii<mtv_list_count; ii++) {
                        if (trick.mtv.MtvApp.mtv_events.get(ii).butt.isShowing()) {
                            trick.mtv.MtvApp.mtv_events.get(ii).butt.setSelected(false);
                        }
                    }
                    tabs[sel].custom = false;
                    
                }
                if (!tabs[sel].custom) {
                    // set SELECT ALL when all other SELECTS are set
                    selectbutts[0].setSelected(
                        selectbutts[1].isSelected() &&
                        selectbutts[2].isSelected() &&
                        selectbutts[3].isSelected());
                    // set CLEAR ALL when all other SELECTS are not set
                    selectbutts[4].setSelected(
                        !selectbutts[1].isSelected() &&
                        !selectbutts[2].isSelected() &&
                        !selectbutts[3].isSelected());
                    if (selectbutts[4].isSelected()) {
                        selectbutts[3].setSelected(false);
                    }
                }
                // save the select button states for each tab
                for (int ii=0; ii<select_choices; ii++) {
                    tabs[sel].select_butt_state[ii] = selectbutts[ii].isSelected();
                }
            }
        };
        // selecting event buttons will clear the select buttons
        ActionListener event_butt_listen = new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                tabs[tabpane.getSelectedIndex()].custom = true;
                for (int ii=0; ii<select_choices; ii++) {
                    selectbutts[ii].setSelected(false);
                }
            }
        };
        // add the select buttons
        selectPanel.setLayout(new GridLayout(select_choices,1));
        for (int ii=0; ii<select_choices; ii++) {
            selectbutts[ii].addActionListener(select_butt_listen);
            selectPanel.add(selectbutts[ii]);
        }
        // selecting a tab will restore the select button states for that tab
        tabpane.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                JTabbedPane p = (JTabbedPane)e.getSource();
                int sel = p.getSelectedIndex();
                for (int ii=0; ii<select_choices; ii++) {
                    selectbutts[ii].setSelected(tabs[sel].select_butt_state[ii]);
                }
            }
        });
        
        // create tab 0 to contain ungrouped events (any subsequent group will get its own tab)
        tabs[0] = new Tab();
        tabs[0].panel = new JPanel();
        tabs[0].panel.setName("Events");
        tabs[0].scrollPane = new JScrollPane(tabs[0].panel);
        //tabs[0].butt = new JRadioButton();
        tabs[0].custom = true;
        tabs[0].select_butt_state = new boolean[select_choices];
        int tabcount = 1;
        int tabnum;
        HashMap<String,Integer> tabmap = new HashMap<String,Integer>(); // map group names to tab number
        String eventname[] = null;
        // add all event buttons
        for (int ii=0; ii<mtv_list_count; ii++) {
            // don't show buttons for deleted events
            if (!trick.mtv.MtvApp.mtv_events.get(ii).deleted) {
                if (!trick.mtv.MtvApp.mtv_events.get(ii).added ||
                    !trick.mtv.MtvApp.mtv_events.get(ii).active) {
                    trick.mtv.MtvApp.mtv_events.get(ii).butt.setForeground(Color.GRAY);
                }
                trick.mtv.MtvApp.mtv_events.get(ii).butt.addActionListener(event_butt_listen);
                eventname = trick.mtv.MtvApp.mtv_events.get(ii).name.split("\\.");
                // a grouped event name has format <groupname>.<eventname>
                // a non-grouped event name is simply <eventname> with no period
                if (eventname.length > 1) {
                    // put grouped events on their own tab
                    if (!tabmap.containsKey(eventname[0])) {
                        tabmap.put(eventname[0], tabcount);
                        tabs[tabcount] = new Tab();
                        tabs[tabcount].panel = new JPanel();
                        tabs[tabcount].panel.setName(eventname[0]);
                        tabs[tabcount].scrollPane = new JScrollPane(tabs[tabcount].panel);
                        //tabs[tabcount].butt = new JRadioButton();
                        tabs[tabcount].custom = true;
                        tabs[tabcount].select_butt_state = new boolean[select_choices];
                        tabcount++;
                    }
                    tabnum = Integer.parseInt(tabmap.get(eventname[0]).toString());
                } else {
                    // non-grouped events
                    tabnum = 0;
                }
                tabs[tabnum].panel.add(trick.mtv.MtvApp.mtv_events.get(ii).butt);
            }
        }
        // add all tabs
        for (int ii=0; ii<tabcount; ii++) {
            tabs[ii].scrollPane.setPreferredSize(new Dimension(450,250));
            tabs[ii].panel.setLayout(new GridLayout(mtv_list_count,1));
            tabpane.addTab(tabs[ii].panel.getName(), tabs[ii].scrollPane);
            // try putting a radio button in the tab
            //JPanel tablabel = new JPanel();
            //tablabel.setOpaque(false);
            //tablabel.add(new JLabel(tabs[ii].panel.getName()));
            //tablabel.add(tabs[ii].butt);
            //tabpane.setTabComponentAt(ii, tablabel);
        }
        
        // popup and get choices
        Object[] options = {
            new JLabel("Select events you want to display:"),
            selectPanel,
            tabpane };
        int ret = JOptionPane.showConfirmDialog(viewPanel, options, "Customize Event Display",
                                                JOptionPane.OK_CANCEL_OPTION);
        if (ret == JOptionPane.OK_OPTION) {
            for (int ii=0; ii<mtv_list_count; ii++) {
                trick.mtv.MtvApp.mtv_events.get(ii).show = trick.mtv.MtvApp.mtv_events.get(ii).butt.isSelected();
            }
            // force update() call in doInBackground task
            MtvApp.getApplication().need_update = true;
        }
    }

    // user clicked a Cycle from cycle menu
    @SwingAction
    public void set_cycle() {
        MtvApp.getApplication().var_cycle = Double.parseDouble(cycle_group.getSelection().getActionCommand());
        if (MtvApp.vscom != null) {
            try {
                MtvApp.vscom.put("trick.var_cycle(" + MtvApp.getApplication().var_cycle + ")");
            } catch (IOException ioe) {
                System.out.println("MTV set_cycle() " +ioe.toString() +".");
            }
        }
    }

    // get one shot of data while paused
    public void get_one_shot_data() {
        MtvApp.getApplication().one_shot = true;
        try {
            MtvApp.vscom.put("trick.var_send()");
        } catch (IOException ioe) {
            System.out.println("MTV get_one_shot_data() " +ioe.toString() +".");
            MtvApp.getApplication().one_shot = false;
        }
        // wait for data to be read in doInBackground (it clears one_shot when read)
        while (MtvApp.getApplication().one_shot) {}
    }


    // user clicked Load Event from file menu
    @SwingAction
    public void load_event() {
        FileReader fr = null;
        BufferedReader br = null;
        String parse_string = "";
        String results[] = null;

        if (current_tab==1) {
        // Edit
            JOptionPane.showMessageDialog(editPanel, "You can only Load an event from a file for viewing from the View tab.\n" +
                                                     "The ability to Load an event from a file for editting is not implemented yet.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
            return;
        } else if (connect_button.isEnabled()) {
            JOptionPane.showMessageDialog(viewPanel, "You must be connected to a sim before loading.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
            return;
        }

        JFileChooser fc = new JFileChooser();
        fc.setDialogTitle("Load Event(s) from a Python file");
        fc.setFileSelectionMode(0);

        fc.setCurrentDirectory(new File(MtvApp.default_event_dir));
        int ret = fc.showOpenDialog(viewPanel);
        if (ret == JFileChooser.APPROVE_OPTION) {
            String dirname = fc.getCurrentDirectory().getPath();
            String filename = fc.getSelectedFile().getName();
            //System.out.println("Load event: " + dirname + "/" + filename);
            MtvApp.default_event_dir = dirname;
            MtvApp.getApplication().trickProperties.setProperty("TRICK_MTV_DIR", dirname);
            try {
                MtvApp.vscom.pause();
                try {
                    fr = new FileReader(dirname + "/" + filename);
                    br = new BufferedReader(fr);
                    String line = br.readLine();
                    while (line != null) {
                        // escape quotes in the string
                        parse_string = parse_string + line.replaceAll("\"", "\\\\\"") + "\n";
                        line = br.readLine();
                    }
                    br.close();
                } catch (FileNotFoundException fnf) {
                    System.out.println("MTV load_event() " +fnf.toString() +".");
                } catch (IOException ioe) {
                    System.out.println("MTV load_event() " +ioe.toString() +".");
                }
                MtvApp.vscom.put("trick_ip.ip.parse(\"\"\"\n" + parse_string + "\n\"\"\")");
                MtvApp.vscom.unpause();
                // force update() call in doInBackground task
                MtvApp.getApplication().need_update = true;
            } catch (IOException ioe) {
                System.out.println("MTV load_event() " +ioe.toString() +".");
            }
        }
    }

    // user clicked Delete Event from edit menu
    @SwingAction
    public void delete_event() {
        Object[] options = { "Yes, DELETE it!", "Cancel" };
        String results[] = null;

        int row = event_table.getSelectedRow();
        if ((row == -1) || (rowtype[row] != RowType.EVENT)) {
            JOptionPane.showMessageDialog(viewPanel, "You must select an event to be deleted.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
        } else if (!canEditRow[row]) {
            JOptionPane.showMessageDialog(viewPanel, "You cannot delete an unnamed event.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
        } else {
            String name = (trick.mtv.MtvApp.mtv_events.get(eventmap[row])).name;
            int answer = JOptionPane.showOptionDialog(viewPanel,
                             "Are you sure you want to permanently delete event " + name + " from the sim ?", "Delete Event",
                             JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[1]);
            if (answer == JOptionPane.OK_OPTION) {
                try {
                    //MtvApp.vscom.pause();
                    trick.mtv.MtvApp.mtv_events.get(eventmap[row]).deleted = true;
                    trick.mtv.MtvApp.mtv_events.get(eventmap[row]).show = false;
                    MtvApp.vscom.put("trick.delete_event(" + name + ")");
                    // force update() call in doInBackground task
                    MtvApp.getApplication().need_update = true;
                    //MtvApp.vscom.unpause();
                } catch (IOException ioe) {
                    System.out.println("MTV delete_event() " +ioe.toString() +".");
                }
            }
        }
    }

    public class EventTableRenderer extends DefaultTableCellRenderer {
        // Set the Fired/Ran Time cell background color when fired; dim the inactive rows; color the manual mode rows
        JCheckBox checkbox = new JCheckBox();

        public EventTableRenderer() {
            
        }
        @Override
        public Component getTableCellRendererComponent(JTable table, Object value,
                                                       boolean isSelected, boolean hasFocus, int row, int column) {
            Color bg, fg;
            Color manual_color = Color.BLUE;
            Color job_color = new Color(120,45,0); // brown
            Color run_color = new Color(80,190,80); // dark green
            Color runlite_color = new Color(180,210,180); // light green

            // default row color
            bg = Color.WHITE;
            fg = Color.BLACK;
            // shade each event row
            if (rowtype[row] == RowType.EVENT) {
                bg = Color.LIGHT_GRAY;
            }
            // user selected row
            if (isSelected) {
                bg = event_table.getSelectionBackground();
                fg = event_table.getSelectionForeground();
            }
            // if event is added before/after a job, make all of its rows brown
            if (trick.mtv.MtvApp.mtv_events.get(eventmap[row]).before_after > 0) {
                fg = job_color;
            }
            // if event is in manual mode, make all of its rows text blue
            //if ((mode[row] != Mode.NORMAL) || (mode[trick.mtv.MtvApp.mtv_events.get(eventmap[row]).row] != Mode.NORMAL)) {
            if (mode[trick.mtv.MtvApp.mtv_events.get(eventmap[row]).row] != Mode.NORMAL) {
                fg = manual_color;
            }
            
            // dim each inactive/disabled row's text; if event is inactive, dim its condition/action rows too
            if ((!active[row] || !active[trick.mtv.MtvApp.mtv_events.get(eventmap[row]).row] || 
                  !trick.mtv.MtvApp.mtv_events.get(eventmap[row]).added)
               && !(value instanceof Boolean)) {
                if ((mode[trick.mtv.MtvApp.mtv_events.get(eventmap[row]).row] != Mode.NORMAL) && (rowtype[row] != RowType.ACTION)) {
                    // only actions can become active/inactive in manual mode
                } else {
                    fg = Color.LIGHT_GRAY;
                    if (rowtype[row] == RowType.EVENT) {
                        fg = Color.WHITE;
                    }
                }
            }
            // Hold button only valid for conditions, so hide it for event and action rows
            if ((rowtype[row] != RowType.CONDITION) && (column==6)) {
                fg = bg;
            }
            // Added button only valid for events, so hide it for cond and action rows
            if ((rowtype[row] != RowType.EVENT) && (column==8)) {
                fg = bg;
            }

            // hidden
            if (fg==bg) {
                setBackground(bg);
                setForeground(fg);
                return this;
            }
            // checkbox
            if (value instanceof Boolean) {
                checkbox.setSelected(((Boolean) value).booleanValue());
                checkbox.setHorizontalAlignment(JLabel.CENTER);
                checkbox.setBackground(bg);
                // in manual mode, the Active box for event and conditions are dead, so dim them
                if ((fg==manual_color) && (column==0) && (rowtype[row]!=RowType.ACTION)) {
                    checkbox.setEnabled(false);
                } else {
                    checkbox.setEnabled(true);
                }
                return checkbox;
            // normal
            } else {
                super.getTableCellRendererComponent(table,value,isSelected,hasFocus,row,column);  
                if ((is_red[row] && (rowtype[row] == RowType.EVENT) && (column==2))) { // Fired Event
                    setBackground(Color.RED);   
                    setForeground(Color.WHITE);
                } else if ((is_green[row] && (rowtype[row] == RowType.EVENT) && (column==4))) { // Ran Event
                    setBackground(run_color);
                    setForeground(Color.WHITE);
                } else if ((is_red[row] && (rowtype[row] == RowType.CONDITION) && (column==2))) { // Fired Condition
                    setBackground(Color.PINK);
                    setForeground(Color.WHITE);
                } else if ((is_green[row] && (rowtype[row] == RowType.ACTION) && (column==4))) { // Ran Action
                    setBackground(runlite_color);
                    setForeground(Color.WHITE);
                } else {
                    setBackground(bg);
                    setForeground(fg);
                }
                return this;
            }
            
        }

    }
    
    // ================================== Edit Tab Functions =======================================

    // Starting up MTV, or user clicked Create New Event from edit menu
    @SwingAction
    public void edit_new_event() {

        if (edit_not_saved) {
            Object[] options = { "Yes, Forget current edits!", "Cancel" };
            int answer = JOptionPane.showOptionDialog(editPanel,
                        "You have not saved the event to a file. Do you wish to lose the current edits you've made?", "Current Edit Not Saved",
                         JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[1]);
            if (answer != JOptionPane.OK_OPTION) {
               return;
           }
        }

        edit_table_rows.removeAllElements();
        edit_condition_count = 1;
        edit_action_count = 1;
        edit_event_name = "put_new_event_name_here";
        eventTextField.setText(edit_event_name);

        Vector<Object> row;
        row = new Vector<Object>(Arrays.asList("### Event Configuration", "", "", ""));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList(edit_event_name+ " = trick.new_event", "(\"", edit_event_name, "\")"));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList(edit_event_name+ ".cycle = ", "", "0.5", ""));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList(edit_event_name+ ".activate","(", "", " )"));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList("trick.add_event","(", edit_event_name, " )"));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList("### Conditions", "", "", ""));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList(edit_event_name+ ".condition","(0,\"\"\"","False", "\"\"\")"));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList("### Actions", "", "", ""));
        edit_table_rows.addElement(row);
        row = new Vector<Object>(Arrays.asList(edit_event_name+ ".action","(0,\"\"\"","print \"ACTION0\"", "\"\"\")"));
        edit_table_rows.addElement(row);
        // disable table and edit menu until user sets event name
        edit_table.setEnabled(false);
        editMenu.setEnabled(false);
        // redraw table so rows are shown grayed out
        TableModelEvent tme = new TableModelEvent(edit_table.getModel());
        edit_table.tableChanged(tme);
    }

    // user clicked Save in file menu
    @SwingAction
    public void save_event() {
        FileWriter fw = null;
        BufferedWriter bw = null;
        boolean exists, done;
        int ret;
        String line, stmt, col;

        if (current_tab==0) {
        // View
            JOptionPane.showMessageDialog(editPanel, "You can only Save an event to a file from the Edit tab.\n",
                                          "Error", JOptionPane.WARNING_MESSAGE);
            return;
        }

        JFileChooser fc = new JFileChooser();
        fc.setDialogTitle("Save Event to a file");
        fc.setFileSelectionMode(0);

        fc.setCurrentDirectory(new File(MtvApp.default_event_dir));
        done = false;
        while (!done) {
            ret = fc.showSaveDialog(editPanel);
            if (ret == JFileChooser.APPROVE_OPTION) { // User cllicked YES
                String dirname = fc.getCurrentDirectory().getPath();
                String filename = fc.getSelectedFile().getName();
                String path = dirname + "/" + filename;
                MtvApp.default_event_dir = dirname;
                exists = (new File(path).exists());
                if (exists) {
                    Object[] options = { "Yes, REPLACE it!", "Cancel" };
                    int answer = JOptionPane.showOptionDialog(editPanel,
                                "Replace existing file: " + filename + " ?", "Replace File",
                                 JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[1]);
                    if (answer != JOptionPane.OK_OPTION) {
                       continue;
                   }
                }
                // Save it
                try {
                    fw = new FileWriter(path);
                    bw = new BufferedWriter(fw);
                    stmt = "";
                    for (int ii=0; ii<edit_table_rows.size(); ii++) {
                        line = "";
                        for (int jj=0; jj<4; jj++) {
                            col = (String) edit_table.getValueAt(ii,jj);
                            if ((jj==0) && !col.isEmpty()) {
                                stmt = col;
                                System.out.println("stmt= " +stmt);
                            }
                            // add newline before/after each action or condition string?
                            if (stmt.endsWith("action") && (jj==1) && (col.endsWith("\"\"\"")) ) {
                                col = col + "\n";
                            } else if (stmt.endsWith("action") && (jj==3) && (col.startsWith("\"\"\"")) ) {
                                col = "\n" + col;
                            }
                            line += col;
                        }
                        bw.write(line +"\n");
                    }
                    bw.close();
                    edit_not_saved = false;
                } catch (FileNotFoundException fnf) {
                    System.out.println("MTV save_event() " +fnf.toString() +".");
                } catch (IOException ioe) {
                    System.out.println("MTV save_event() " +ioe.toString() +".");
                }
                done = true;
            } else { // User clicked CANCEL
                done = true;
            }
        } // end while !done
    }

    // user clicked Send To Sim button
    @SwingAction
    public void send_to_sim() {
        String line, stmt, col;
        String parse_string = "";
        String results[] = null;

        try {
            MtvApp.vscom.pause();
            stmt = "";
            for (int ii=0; ii<edit_table_rows.size(); ii++) {
                line = "";
                for (int jj=0; jj<4; jj++) {
                    col = (String) edit_table.getValueAt(ii,jj);
                    if ((jj==0) && !col.isEmpty()) {
                        stmt = col;
                    }
                    // add newline before/after each action or condition string?
                    if (stmt.endsWith("action") && (jj==1) && (col.endsWith("\"\"\"")) ) {
                        col = col + "\n";
                    } else if (stmt.endsWith("action") && (jj==3) && (col.startsWith("\"\"\"")) ) {
                        col = "\n" + col;
                    }
                    line += col;
                }
                // escape quotes in the string
                parse_string = parse_string + line.replaceAll("\"", "\\\\\"") + "\n";
            }
            //System.out.println(parse_string);
            MtvApp.vscom.put("trick_ip.ip.parse(\"\"\"\n" + parse_string + "\n\"\"\")");
            MtvApp.vscom.unpause();
            // force update() call in doInBackground task
            MtvApp.getApplication().need_update = true;
        } catch (IOException ioe) {
            System.out.println("MTV send_to_sim() " +ioe.toString() +".");
        }
    }

    // The event is laid out like this in the table:
    // ### Event Configuration
    // <name> = trick.sim_services.IPPythonEvent("<name>")
    // <name>.cycle = 0.5
    // <name>.activate()
    // <name>.add_event()
    // <<< all other statements besides a condition or action go here >>>
    // ### Conditions
    // <name>.condition(0, "False")
    // ### Actions
    // <name>.action(0, "print \"ACTION0\"")

    // find where to insert new stmt from add_line and insert it into table
    public void add_new_statement(String command, Vector new_row) {
        int row=0, cond_start=0, act_start=0;
        int start, end;
        for (int ii=0; ii<edit_table_rows.size(); ii++) {
            if (edit_table.getValueAt(ii, 0).equals("### Conditions")) {
                cond_start = ii;
            }
            if (edit_table.getValueAt(ii, 0).equals("### Actions")) {
                act_start = ii;
                break;
            }
        }
        if (command.equals("condition") || command.equals("condition_job") ||
            command.equals("condition_var")) {
            start = cond_start+1;
            end = act_start;
        } else if (command.equals("action") || command.equals("action_job") ||
            command.equals("action_job_on") || command.equals("action_job_off")) {
            start = act_start+1;
            end = edit_table_rows.size();
        } else { // all other stmts go under Event Configuration
            start = 1;
            end = cond_start;
        }
        for (int ii=start; ii<end; ii++) {
            if (edit_table_rows.get(ii).equals(new_row)) {
                JOptionPane.showMessageDialog(editPanel, "Statement already exists.",
                                      "Error", JOptionPane.WARNING_MESSAGE);
                edit_table.setRowSelectionInterval(ii, ii); // select the duplicate line
                return;
            }
            row = ii;
        }
        // insert line *after* the appropriate line
        edit_table_rows.insertElementAt(new_row, row+1);
        // redraw table so all rows are shown
        TableModelEvent tme = new TableModelEvent(edit_table.getModel());
        edit_table.tableChanged(tme);
        edit_table.setRowSelectionInterval(row+1, row+1); // select the new line
        edit_not_saved = true;
    }

    // user clicked Add Line or Add Statement from edit menu
    @SwingAction
    public void add_line(ActionEvent e) {
        String stmt, command, starting, data, ending;
        int num;
        Vector<Object> new_row;

        int row = edit_table.getSelectedRow();

        // if Add Line, must be adding to condtion or action
        if (e.getActionCommand().equals("Add Line to condition/action")) {
            if (row == -1) {
                JOptionPane.showMessageDialog(editPanel, "You must select a line where new line is to be added.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
                return;
            }
            stmt = (String) edit_table.getValueAt(row, 0);
            starting = (String) edit_table.getValueAt(row, 1);
            ending = (String) edit_table.getValueAt(row, 3);
            if (!stmt.isEmpty()) {
                System.out.println("stmt= <" + stmt +">");
                if (!stmt.endsWith("condition") && !stmt.endsWith("condition_job") && !stmt.endsWith("condition_var") &&
                    !stmt.endsWith("action") && !stmt.endsWith("action_job") && !stmt.endsWith("action_job_on") &&
                    !stmt.endsWith("action_job_off")) {
                    JOptionPane.showMessageDialog(editPanel, "You can only add a new line to a condition or action.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
                    return;
                }
            }
            edit_table.setValueAt("", row, 3); // remove ending paren from current line
            new_row = new Vector<Object>(Arrays.asList("", "", "", ending));
            // insert new line *after* the selected line
            edit_table_rows.insertElementAt(new_row, row+1);
            // redraw table so all rows are shown
            TableModelEvent tme = new TableModelEvent(edit_table.getModel());
            edit_table.tableChanged(tme);
            edit_table.setRowSelectionInterval(row+1, row+1); // select the new line
            edit_not_saved = true;
        // Add Statement: <whatever>
        } else {
            command = e.getActionCommand();
            data = "";
            if (command.equals("action_disable")) {
                num = 0;
                if (edit_action_count > 1) {
                    if (row == -1) {
                        JOptionPane.showMessageDialog(editPanel, "You must select an action statement to apply " +command + " to.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
                        return;
                    } else {
                        stmt = (String) edit_table.getValueAt(row, 0);
                        if (stmt.isEmpty() || (!stmt.endsWith("action") && !stmt.endsWith("action_job") &&
                                               !stmt.endsWith("action_job_on") && !stmt.endsWith("action_job_off")) ) {
                            JOptionPane.showMessageDialog(editPanel, "You must select an action statement to apply " +command + " to.",
                                              "Error", JOptionPane.WARNING_MESSAGE);
                            return;
                        }
                        starting = (String) edit_table.getValueAt(row, 1);
                        num = Integer.parseInt(starting.substring(1, starting.indexOf(',')));
                    }
                }
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(", ""+num, " )"));
                add_new_statement(command, new_row);
            }
            if (command.equals("condition_disable") ||
                command.equals("condition_hold_on") || command.equals("condition_hold_off")) {
                num = 0;
                if (edit_condition_count > 1) {
                    if (row == -1) {
                        JOptionPane.showMessageDialog(editPanel, "You must select a condition statement to apply " +command + " to.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
                        return;
                    } else {
                        stmt = (String) edit_table.getValueAt(row, 0);
                        if (stmt.isEmpty() || (!stmt.endsWith("condition") && !stmt.endsWith("condition_job") &&
                                               !stmt.endsWith("condition_var")) ) {
                            JOptionPane.showMessageDialog(editPanel, "You must select a condition statement to apply " +command + " to.",
                                              "Error", JOptionPane.WARNING_MESSAGE);
                            return;
                        }
                        starting = (String) edit_table.getValueAt(row, 1);
                        num = Integer.parseInt(starting.substring(1, starting.indexOf(',')));
                    }
                }
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(", ""+num, " )"));
                add_new_statement(command, new_row);
            }
            if (command.equals("activate")) {
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(", "", " )"));
                add_new_statement(command, new_row);
            }
            if (command.equals("add_event")) {
                stmt = "trick." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(", edit_event_name, " )"));
                add_new_statement(command, new_row);
            }
            if (command.equals("add_event_after") || command.equals("add_event_before")) {
                // TODO need to implement a job chooser
                stmt = "trick." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(", edit_event_name + ", \"INSERT_JOBNAME_HERE\"", " )"));
                add_new_statement(command, new_row);
            }
            if (command.equals("condition_all")) {
                stmt = edit_event_name +"." + command + " = ";
                new_row = new Vector<Object>(Arrays.asList(stmt, "", "True", ""));
                add_new_statement(command, new_row);
            }
            if (command.equals("cycle")) {
                stmt = edit_event_name +"." + command + " = ";
                new_row = new Vector<Object>(Arrays.asList(stmt, "", "0.5", ""));
                add_new_statement(command, new_row);
            }
            if (command.equals("condition")) {
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(" +edit_condition_count +",\"\"\"", "False", "\"\"\")"));
                add_new_statement(command, new_row);
                edit_condition_count++;
            }
            if (command.equals("condition_job")) {
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(" +edit_condition_count +",", "\"INSERT_JOBNAME_HERE\"", " )"));
                add_new_statement(command, new_row);
                edit_condition_count++;
            }
            if (command.equals("condition_var")) {
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(" +edit_condition_count +",", "\"INSERT_VARNAME_HERE\"" , " )"));
                add_new_statement(command, new_row);
                edit_condition_count++;
            }
            if (command.equals("action")) {
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(" +edit_action_count +",\"\"\"", "print \"ACTION" +edit_action_count +"\"", "\"\"\")"));
                add_new_statement(command, new_row);
                edit_action_count++;
            }
            if (command.startsWith("action_job")) {
                stmt = edit_event_name +"." + command;
                new_row = new Vector<Object>(Arrays.asList(stmt, "(" +edit_action_count +",", "\"INSERT_JOBNAME_HERE\"" , " )"));
                add_new_statement(command, new_row);
                edit_action_count++;
            }

        }

    }

    // user clicked Delete Line from edit menu
    @SwingAction
    public void delete_line() {
        String stmt, starting, ending;
        int count, num, num_to_delete;

        int row = edit_table.getSelectedRow();
        if (row == -1) {
            JOptionPane.showMessageDialog(editPanel, "You must select a line to delete.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
            return;
        }
        if (row == 1) {
            JOptionPane.showMessageDialog(editPanel, "You cannot delete the event constructor.",
                                          "Error", JOptionPane.WARNING_MESSAGE);
            return;
        }
        stmt = (String) edit_table.getValueAt(row, 0);
        starting = (String) edit_table.getValueAt(row, 1);
        ending = (String) edit_table.getValueAt(row, 3);
        count = 1; // number of rows to delete
        // if it's a statement line, ask user if they are sure, handle multiple rows in stmt
        if (! stmt.isEmpty()) {
            if (! starting.isEmpty()) {
                while (ending.isEmpty()) {
                    ending = (String) edit_table.getValueAt(row+count, 3);
                    count++;
                }
            }
            Object[] options = { "Yes, DELETE it!", "Cancel" };
            int answer = JOptionPane.showOptionDialog(editPanel,
                             "Are you sure you want to delete statement: " + stmt + " ?", "Delete Statement",
                             JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, options, options[1]);
            if (answer != JOptionPane.OK_OPTION) {
                return;
            }
            // when deleting a condtion/action, update other condition/action numbers in the event
            if (stmt.endsWith("condition") || stmt.endsWith("condition_job") ||
                stmt.endsWith("condition_var")) {
                num_to_delete = Integer.parseInt(starting.substring(1, starting.indexOf(',')));
                for (int ii=0; ii<edit_table_rows.size(); ii++) {
                    if ( ((String) edit_table.getValueAt(ii, 0)).endsWith("condition") ||
                         ((String) edit_table.getValueAt(ii, 0)).endsWith("condition_job") ||
                         ((String) edit_table.getValueAt(ii, 0)).endsWith("condition_var")) {
                        starting = (String) edit_table.getValueAt(ii, 1);
                        num = Integer.parseInt(starting.substring(1, starting.indexOf(',')));
                        if (num > num_to_delete) {
                            edit_table.setValueAt("(" +(num-1) +",\"", ii, 1);
                        }
                    }
                }
                edit_condition_count--;
            }
            if (stmt.endsWith("action") || stmt.endsWith("action_job") ||
                stmt.endsWith("action_job_on") || stmt.endsWith("action_job_off")) {
                num_to_delete = Integer.parseInt(starting.substring(1, starting.indexOf(',')));
                for (int ii=0; ii<edit_table_rows.size(); ii++) {
                    if ( ((String) edit_table.getValueAt(ii, 0)).endsWith("action") ||
                         ((String) edit_table.getValueAt(ii, 0)).endsWith("action_job") ||
                         ((String) edit_table.getValueAt(ii, 0)).endsWith("action_job_on") ||
                         ((String) edit_table.getValueAt(ii, 0)).endsWith("action_job_off")) {
                        starting = (String) edit_table.getValueAt(ii, 1);
                        num = Integer.parseInt(starting.substring(1, starting.indexOf(',')));
                        if (num > num_to_delete) {
                            edit_table.setValueAt("(" +(num-1) +",\"", ii, 1);
                        }
                    }
                }
                edit_action_count--;
            }
        }
        for (int ii=0; ii<count; ii++) {
            edit_table_rows.remove(row);
        }
        // if we only deleted part of a stmt, move ending paren up one row
        if (stmt.isEmpty()) {
            edit_table.setValueAt(ending, row-1, 3);
        }
        // redraw table so all rows are shown
        TableModelEvent tme = new TableModelEvent(edit_table.getModel());
        edit_table.tableChanged(tme);
    }

    public class EditTableRenderer extends DefaultTableCellRenderer {
        //

        public EditTableRenderer() {

        }
        @Override
        public Component getTableCellRendererComponent(JTable table, Object value,
                                                       boolean isSelected, boolean hasFocus, int row, int column) {
            Color bg, fg;
            Color manual_color = Color.BLUE;
            Color run_color = new Color(80,190,80); // dark green
            Color runlite_color = new Color(180,210,180); // light green

            // default row color
            bg = Color.WHITE;
            fg = Color.BLACK;

            // show comments as light gray (or everything if table is disabled)
            if ( ((column==0) && ( ((String)value).startsWith("#")) ) || !edit_table.isEnabled() ) {
                fg = Color.LIGHT_GRAY;
            }

            // user selected row
            if (isSelected) {
                // don't allow user to select our comments
                if (((String)value).equals("### Event Configuration") ||
                    ((String)value).equals("### Conditions") ||
                    ((String)value).equals("### Actions") ) {
                    edit_table.clearSelection();
                } else {
                    bg = edit_table.getSelectionBackground();
                    fg = edit_table.getSelectionForeground();
                }
            }

            // hidden
            if (fg==bg) {
                setBackground(bg);
                setForeground(fg);
                return this;
            }

            super.getTableCellRendererComponent(table,value,isSelected,hasFocus,row,column);
            setBackground(bg);
            setForeground(fg);
            return this;

        }

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        menuBar = new javax.swing.JMenuBar();
        javax.swing.JMenu fileMenu = new javax.swing.JMenu();
        loadMenuItem = new javax.swing.JMenuItem();
        saveMenuItem = new javax.swing.JMenuItem();
        jSeparator4 = new javax.swing.JPopupMenu.Separator();
        confirmExitMenuItem = new javax.swing.JCheckBoxMenuItem();
        javax.swing.JMenuItem exitMenuItem = new javax.swing.JMenuItem();
        viewMenu = new javax.swing.JMenu();
        deleteMenuItem = new javax.swing.JMenuItem();
        customizeMenuItem = new javax.swing.JMenuItem();
        editMenu = new javax.swing.JMenu();
        createMenuItem = new javax.swing.JMenuItem();
        edit_deleteMenuItem = new javax.swing.JMenuItem();
        edit_addlineMenuItem = new javax.swing.JMenuItem();
        edit_addstmtMenu = new javax.swing.JMenu();
        edit_actionMenuItem = new javax.swing.JMenuItem();
        edit_action_disableMenuItem = new javax.swing.JMenuItem();
        jMenuItem1 = new javax.swing.JMenuItem();
        jMenuItem2 = new javax.swing.JMenuItem();
        jMenuItem3 = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JPopupMenu.Separator();
        edit_activateMenuItem = new javax.swing.JMenuItem();
        edit_add_eventMenuItem = new javax.swing.JMenuItem();
        edit_add_event_afterMenuItem = new javax.swing.JMenuItem();
        edit_add_event_beforeMenuItem = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JPopupMenu.Separator();
        edit_conditionMenuItem = new javax.swing.JMenuItem();
        edit_condition_allMenuItem = new javax.swing.JMenuItem();
        edit_condition_disableMenuItem = new javax.swing.JMenuItem();
        edit_condition_hold_onMenuItem = new javax.swing.JMenuItem();
        jMenuItem4 = new javax.swing.JMenuItem();
        jMenuItem5 = new javax.swing.JMenuItem();
        jSeparator3 = new javax.swing.JPopupMenu.Separator();
        edit_cycleMenuItem = new javax.swing.JMenuItem();
        cycleMenu = new javax.swing.JMenu();
        cycleMenuItem05 = new javax.swing.JRadioButtonMenuItem();
        cycleMenuItem10 = new javax.swing.JRadioButtonMenuItem();
        cycleMenuItem25 = new javax.swing.JRadioButtonMenuItem();
        cycleMenuItem50 = new javax.swing.JRadioButtonMenuItem();
        cycleMenuItem75 = new javax.swing.JRadioButtonMenuItem();
        cycleMenuItem100 = new javax.swing.JRadioButtonMenuItem();
        javax.swing.JMenuItem aboutMenuItem = new javax.swing.JMenuItem();
        statusPanel = new javax.swing.JPanel();
        status_label = new javax.swing.JLabel();
        javax.swing.JSeparator statusPanelSeparator = new javax.swing.JSeparator();
        statusMessageLabel = new javax.swing.JLabel();
        host_label = new javax.swing.JLabel();
        host_field = new javax.swing.JTextField();
        port_label = new javax.swing.JLabel();
        port_field = new javax.swing.JTextField();
        statusAnimationLabel = new javax.swing.JLabel();
        progressBar = new javax.swing.JProgressBar();
        connect_button = new javax.swing.JButton();
        jTabbedPane1 = new javax.swing.JTabbedPane();
        viewPanel = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        // Mode combobox
        comboBox = new javax.swing.JComboBox();
        comboBox.addItem("Normal");
        comboBox.addItem("Manual FIRE");
        comboBox.addItem("Manual ON");
        comboBox.addItem("Manual OFF");
        mode_selected = false;

        event_table_rows = new Vector<Vector>();
        event_table_cols = new Vector<String>();
        String[] col_names = {"Active", "Name", "Fired Time", "Fired Count", "Ran Time", "Ran Count", "Hold", "Mode", "Added"};
        for (int ii=0; ii<col_names.length; ii++) {
            event_table_cols.addElement((String) col_names[ii]);
        }
        event_table = new javax.swing.JTable();
        toolBar = new javax.swing.JToolBar();
        tool_label = new javax.swing.JLabel();
        simtime_label = new javax.swing.JLabel();
        simtime_text = new javax.swing.JTextField();
        trick_icon = new trick.common.ui.panels.SmallTrickIconLabel();
        editPanel = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        edit_table = new javax.swing.JTable();
        toolBar2 = new javax.swing.JToolBar();
        jLabel1 = new javax.swing.JLabel();
        eventTextField = new javax.swing.JTextField();
        send_button = new javax.swing.JButton();
        trick_icon2 = new trick.common.ui.panels.SmallTrickIconLabel();

        menuBar.setName("menuBar"); // NOI18N

        fileMenu.setText(resourceMap.getString("fileMenu.text")); // NOI18N
        fileMenu.setName("fileMenu"); // NOI18N

        javax.swing.ActionMap actionMap = BaseApplication.getInstance().getContext().getActionMap(MtvView.class, this);
        loadMenuItem.setAction(actionMap.get("load_event")); // NOI18N
        loadMenuItem.setIcon(resourceMap.getIcon("loadMenuItem.icon")); // NOI18N
        loadMenuItem.setText(resourceMap.getString("loadMenuItem.text")); // NOI18N
        loadMenuItem.setName("loadMenuItem"); // NOI18N
        fileMenu.add(loadMenuItem);

        saveMenuItem.setAction(actionMap.get("save_event")); // NOI18N
        saveMenuItem.setIcon(resourceMap.getIcon("saveMenuItem.icon")); // NOI18N
        saveMenuItem.setText(resourceMap.getString("saveMenuItem.text")); // NOI18N
        saveMenuItem.setName("saveMenuItem"); // NOI18N
        fileMenu.add(saveMenuItem);

        jSeparator4.setName("jSeparator4"); // NOI18N
        fileMenu.add(jSeparator4);

        confirmExitMenuItem.setAction(actionMap.get("show_exit_prompt")); // NOI18N
        confirmExitMenuItem.setSelected(true);
        confirmExitMenuItem.setText(resourceMap.getString("confirmExitMenuItem.text")); // NOI18N
        confirmExitMenuItem.setName("confirmExitMenuItem"); // NOI18N
        confirmExitMenuItem.setSelected(MtvApp.confirmExit);
        fileMenu.add(confirmExitMenuItem);

        exitMenuItem.setAction(actionMap.get("quit")); // NOI18N
        exitMenuItem.setName("exitMenuItem"); // NOI18N
        fileMenu.add(exitMenuItem);

        menuBar.add(fileMenu);

        viewMenu.setText(resourceMap.getString("viewMenu.text")); // NOI18N
        viewMenu.setName("viewMenu"); // NOI18N

        deleteMenuItem.setAction(actionMap.get("delete_event")); // NOI18N
        deleteMenuItem.setIcon(resourceMap.getIcon("deleteMenuItem.icon")); // NOI18N
        deleteMenuItem.setText(resourceMap.getString("deleteMenuItem.text")); // NOI18N
        deleteMenuItem.setName("deleteMenuItem"); // NOI18N
        viewMenu.add(deleteMenuItem);

        customizeMenuItem.setAction(actionMap.get("customize_event_display")); // NOI18N
        customizeMenuItem.setText(resourceMap.getString("customizeMenuItem.text")); // NOI18N
        customizeMenuItem.setName("customizeMenuItem"); // NOI18N
        customizeMenuItem.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                customizeMenuItemActionPerformed(evt);
            }
        });
        viewMenu.add(customizeMenuItem);

        menuBar.add(viewMenu);

        editMenu.setAction(actionMap.get("edit_new_event")); // NOI18N
        editMenu.setText(resourceMap.getString("editMenu.text")); // NOI18N
        editMenu.setName("editMenu"); // NOI18N

        createMenuItem.setAction(actionMap.get("edit_new_event")); // NOI18N
        createMenuItem.setText(resourceMap.getString("createMenuItem.text")); // NOI18N
        createMenuItem.setName("createMenuItem"); // NOI18N
        editMenu.add(createMenuItem);

        edit_deleteMenuItem.setAction(actionMap.get("delete_line")); // NOI18N
        edit_deleteMenuItem.setText(resourceMap.getString("edit_deleteMenuItem.text")); // NOI18N
        edit_deleteMenuItem.setName("edit_deleteMenuItem"); // NOI18N
        editMenu.add(edit_deleteMenuItem);

        edit_addlineMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_addlineMenuItem.setText(resourceMap.getString("edit_addlineMenuItem.text")); // NOI18N
        edit_addlineMenuItem.setName("edit_addlineMenuItem"); // NOI18N
        editMenu.add(edit_addlineMenuItem);

        edit_addstmtMenu.setText(resourceMap.getString("edit_addstmtMenu.text")); // NOI18N
        edit_addstmtMenu.setName("edit_addstmtMenu"); // NOI18N

        edit_actionMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_actionMenuItem.setText(resourceMap.getString("edit_actionMenuItem.text")); // NOI18N
        edit_actionMenuItem.setName("edit_actionMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_actionMenuItem);

        edit_action_disableMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_action_disableMenuItem.setText(resourceMap.getString("edit_action_disableMenuItem.text")); // NOI18N
        edit_action_disableMenuItem.setName("edit_action_disableMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_action_disableMenuItem);

        jMenuItem1.setAction(actionMap.get("add_line")); // NOI18N
        jMenuItem1.setText(resourceMap.getString("jMenuItem1.text")); // NOI18N
        jMenuItem1.setName("jMenuItem1"); // NOI18N
        edit_addstmtMenu.add(jMenuItem1);

        jMenuItem2.setAction(actionMap.get("add_line")); // NOI18N
        jMenuItem2.setText(resourceMap.getString("jMenuItem2.text")); // NOI18N
        jMenuItem2.setName("jMenuItem2"); // NOI18N
        edit_addstmtMenu.add(jMenuItem2);

        jMenuItem3.setAction(actionMap.get("add_line")); // NOI18N
        jMenuItem3.setText(resourceMap.getString("jMenuItem3.text")); // NOI18N
        jMenuItem3.setName("jMenuItem3"); // NOI18N
        edit_addstmtMenu.add(jMenuItem3);

        jSeparator1.setName("jSeparator1"); // NOI18N
        edit_addstmtMenu.add(jSeparator1);

        edit_activateMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_activateMenuItem.setText(resourceMap.getString("edit_activateMenuItem.text")); // NOI18N
        edit_activateMenuItem.setName("edit_activateMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_activateMenuItem);

        edit_add_eventMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_add_eventMenuItem.setText(resourceMap.getString("edit_add_eventMenuItem.text")); // NOI18N
        edit_add_eventMenuItem.setName("edit_add_eventMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_add_eventMenuItem);

        edit_add_event_afterMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_add_event_afterMenuItem.setText(resourceMap.getString("edit_add_event_afterMenuItem.text")); // NOI18N
        edit_add_event_afterMenuItem.setName("edit_add_event_afterMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_add_event_afterMenuItem);

        edit_add_event_beforeMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_add_event_beforeMenuItem.setText(resourceMap.getString("edit_add_event_beforeMenuItem.text")); // NOI18N
        edit_add_event_beforeMenuItem.setName("edit_add_event_beforeMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_add_event_beforeMenuItem);

        jSeparator2.setName("jSeparator2"); // NOI18N
        edit_addstmtMenu.add(jSeparator2);

        edit_conditionMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_conditionMenuItem.setText(resourceMap.getString("edit_conditionMenuItem.text")); // NOI18N
        edit_conditionMenuItem.setName("edit_conditionMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_conditionMenuItem);

        edit_condition_allMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_condition_allMenuItem.setText(resourceMap.getString("edit_condition_allMenuItem.text")); // NOI18N
        edit_condition_allMenuItem.setName("edit_condition_allMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_condition_allMenuItem);

        edit_condition_disableMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_condition_disableMenuItem.setText(resourceMap.getString("edit_condition_disableMenuItem.text")); // NOI18N
        edit_condition_disableMenuItem.setName("edit_condition_disableMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_condition_disableMenuItem);

        edit_condition_hold_onMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_condition_hold_onMenuItem.setText(resourceMap.getString("edit_condition_hold_onMenuItem.text")); // NOI18N
        edit_condition_hold_onMenuItem.setName("edit_condition_hold_onMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_condition_hold_onMenuItem);

        jMenuItem4.setAction(actionMap.get("add_line")); // NOI18N
        jMenuItem4.setText(resourceMap.getString("jMenuItem4.text")); // NOI18N
        jMenuItem4.setName("jMenuItem4"); // NOI18N
        edit_addstmtMenu.add(jMenuItem4);

        jMenuItem5.setAction(actionMap.get("add_line")); // NOI18N
        jMenuItem5.setText(resourceMap.getString("jMenuItem5.text")); // NOI18N
        jMenuItem5.setName("jMenuItem5"); // NOI18N
        edit_addstmtMenu.add(jMenuItem5);

        jSeparator3.setName("jSeparator3"); // NOI18N
        edit_addstmtMenu.add(jSeparator3);

        edit_cycleMenuItem.setAction(actionMap.get("add_line")); // NOI18N
        edit_cycleMenuItem.setText(resourceMap.getString("edit_cycleMenuItem.text")); // NOI18N
        edit_cycleMenuItem.setName("edit_cycleMenuItem"); // NOI18N
        edit_addstmtMenu.add(edit_cycleMenuItem);

        editMenu.add(edit_addstmtMenu);

        menuBar.add(editMenu);

        cycleMenu.setText(resourceMap.getString("cycleMenu.text")); // NOI18N
        cycleMenu.setName("cycleMenu"); // NOI18N

        cycleMenuItem05.setAction(actionMap.get("set_cycle")); // NOI18N
        cycleMenuItem05.setSelected(true);
        cycleMenuItem05.setText(resourceMap.getString("cycleMenuItem05.text")); // NOI18N
        cycleMenuItem05.setName("cycleMenuItem05"); // NOI18N
        cycleMenu.add(cycleMenuItem05);

        cycleMenuItem10.setAction(actionMap.get("set_cycle")); // NOI18N
        cycleMenuItem10.setSelected(true);
        cycleMenuItem10.setText(resourceMap.getString("cycleMenuItem10.text")); // NOI18N
        cycleMenuItem10.setName("cycleMenuItem10"); // NOI18N
        cycleMenu.add(cycleMenuItem10);

        cycleMenuItem25.setAction(actionMap.get("set_cycle")); // NOI18N
        cycleMenuItem25.setSelected(true);
        cycleMenuItem25.setText(resourceMap.getString("cycleMenuItem25.text")); // NOI18N
        cycleMenuItem25.setName("cycleMenuItem25"); // NOI18N
        cycleMenu.add(cycleMenuItem25);

        cycleMenuItem50.setAction(actionMap.get("set_cycle")); // NOI18N
        cycleMenuItem50.setSelected(true);
        cycleMenuItem50.setText(resourceMap.getString("cycleMenuItem50.text")); // NOI18N
        cycleMenuItem50.setName("cycleMenuItem50"); // NOI18N
        cycleMenu.add(cycleMenuItem50);

        cycleMenuItem75.setAction(actionMap.get("set_cycle")); // NOI18N
        cycleMenuItem75.setSelected(true);
        cycleMenuItem75.setText(resourceMap.getString("cycleMenuItem75.text")); // NOI18N
        cycleMenuItem75.setName("cycleMenuItem75"); // NOI18N
        cycleMenu.add(cycleMenuItem75);

        cycleMenuItem100.setAction(actionMap.get("set_cycle")); // NOI18N
        cycleMenuItem100.setSelected(true);
        cycleMenuItem100.setText(resourceMap.getString("cycleMenuItem100.text")); // NOI18N
        cycleMenuItem100.setName("cycleMenuItem100"); // NOI18N
        cycleMenu.add(cycleMenuItem100);
        //DANNY set 0.50 as default
        cycle_group = new ButtonGroup();
        cycleMenuItem05.setActionCommand("0.05");
        cycle_group.add(cycleMenuItem05);
        cycleMenuItem10.setActionCommand("0.10");
        cycle_group.add(cycleMenuItem10);
        cycleMenuItem25.setActionCommand("0.25");
        cycle_group.add(cycleMenuItem25);
        cycleMenuItem50.setActionCommand("0.50");
        cycle_group.add(cycleMenuItem50);
        cycleMenuItem75.setActionCommand("0.75");
        cycle_group.add(cycleMenuItem75);
        cycleMenuItem100.setActionCommand("1.00");
        cycle_group.add(cycleMenuItem100);

        cycleMenuItem50.setSelected(true);

        menuBar.add(cycleMenu);

        statusPanel.setName("statusPanel"); // NOI18N
        statusPanel.setLayout(new javax.swing.BoxLayout(statusPanel, javax.swing.BoxLayout.LINE_AXIS));

        status_label.setText(resourceMap.getString("status_label.text")); // NOI18N
        status_label.setMaximumSize(new java.awt.Dimension(550, 20));
        status_label.setName("status_label"); // NOI18N
        status_label.setPreferredSize(new java.awt.Dimension(750, 20));
        statusPanel.add(status_label);

        statusPanelSeparator.setName("statusPanelSeparator"); // NOI18N
        statusPanel.add(statusPanelSeparator);

        statusMessageLabel.setName("statusMessageLabel"); // NOI18N
        statusPanel.add(statusMessageLabel);

        host_label.setText(resourceMap.getString("host_label.text")); // NOI18N
        host_label.setName("host_label"); // NOI18N
        statusPanel.add(host_label);

        host_field.setText(resourceMap.getString("host_field.text")); // NOI18N
        host_field.setAction(actionMap.get("connect")); // NOI18N
        host_field.setName("host_field"); // NOI18N
        host_field.setPreferredSize(new java.awt.Dimension(450, 28));
        statusPanel.add(host_field);

        port_label.setText(resourceMap.getString("port_label.text")); // NOI18N
        port_label.setName("port_label"); // NOI18N
        statusPanel.add(port_label);

        port_field.setText(resourceMap.getString("port_field.text")); // NOI18N
        port_field.setAction(actionMap.get("connect")); // NOI18N
        port_field.setName("port_field"); // NOI18N
        port_field.setPreferredSize(new java.awt.Dimension(100, 28));
        statusPanel.add(port_field);

        statusAnimationLabel.setHorizontalAlignment(javax.swing.SwingConstants.LEFT);
        statusAnimationLabel.setName("statusAnimationLabel"); // NOI18N
        statusPanel.add(statusAnimationLabel);

        progressBar.setMaximumSize(new java.awt.Dimension(0, 0));
        progressBar.setName("progressBar"); // NOI18N
        progressBar.setPreferredSize(new java.awt.Dimension(0, 0));
        statusPanel.add(progressBar);

        connect_button.setAction(actionMap.get("connect")); // NOI18N
        connect_button.setText(resourceMap.getString("connect_button.text")); // NOI18N
        connect_button.setName("connect_button"); // NOI18N
        statusPanel.add(connect_button);

        jTabbedPane1.setName("jTabbedPane1"); // NOI18N
        // enable/diable appropriate menus when a tab is selected
        jTabbedPane1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
                current_tab = ((JTabbedPane)evt.getSource()).getSelectedIndex();
                if (current_tab==0) {
                    // View
                    viewMenu.setEnabled(true);
                    editMenu.setEnabled(false);
                    cycleMenu.setEnabled(true);
                } else {
                    // Edit
                    viewMenu.setEnabled(false);
                    if (!edit_event_name.equals("put_new_event_name_here")) {
                        editMenu.setEnabled(true);
                    }
                    cycleMenu.setEnabled(false);
                }
            }
        });

        viewPanel.setName("viewPanel"); // NOI18N
        viewPanel.setLayout(new java.awt.BorderLayout());

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        event_table.setModel(
            new javax.swing.table.DefaultTableModel(
                /***
                new Object [][] {
                    {null, null, null, null, null, null, null, null, null}
                },
                new String [] {
                    "Active", "Name", "Fired Time", "Fired Count", "Ran Time", "Ran Count", "Hold", "Enabled", "Mode", "Added"
                }
                ***/
                event_table_rows, event_table_cols
            ) {
                Class[] types = new Class [] {
                    java.lang.Boolean.class, java.lang.String.class,
                    java.lang.Double.class, java.lang.Integer.class,
                    java.lang.Double.class, java.lang.Integer.class,
                    java.lang.Boolean.class, java.lang.Object.class,
                    java.lang.Boolean.class
                    //java.lang.String.class
                }; //DANNY added hidden column on end?

                public Class getColumnClass(int columnIndex) {
                    return types [columnIndex];
                }

                public boolean isCellEditable(int rowIndex, int columnIndex) {
                    if (canEditRow[rowIndex]) {
                        return canEdit[rowIndex][columnIndex];
                    }
                    return false;
                }

            });
            for (int ii=0; ii<col_names.length; ii++) {
                event_table.getColumnModel().getColumn(ii).setCellRenderer(new EventTableRenderer());
            }
            event_table.getColumnModel().getColumn(7).setCellEditor(new DefaultCellEditor(comboBox));

            event_table.getModel().addTableModelListener(new TableModelListener() {
                public void tableChanged(TableModelEvent tme) {
                    if (tme.getType() == TableModelEvent.UPDATE) {
                        String name, modestr, command;
                        int row = tme.getFirstRow();
                        int column = tme.getColumn();
                        //System.out.println("LISTEN " + row + ", " + column + " changed."
                            //+ " The new value: " + event_table.getValueAt(row, column));
                        if (mode_selected) { // Mode
                            // mousePressed detected the user click in a row's Mode combo box
                            try {
                                name = trick.mtv.MtvApp.mtv_events.get(eventmap[row]).name;
                                modestr = (String)event_table.getValueAt(row, column);
                                //System.out.println("   mode= " + modestr);
                                if (modestr == "Manual FIRE") {
                                    command = ".manual_fire()";
                                    mode[row] = Mode.MANUALON;
                                } else if (modestr == "Manual ON") {
                                    command = ".manual_on()";
                                    mode[row] = Mode.MANUALON;
                                } else if (modestr == "Manual OFF") {
                                    command = ".manual_off()";
                                    mode[row] = Mode.MANUALOFF;
                                } else { // Normal
                                    command = ".manual_done()";
                                    mode[row] = Mode.NORMAL;
                                }
                                //System.out.println("   cmd= " + name + command);
                                MtvApp.vscom.put(name + command);
                                // paused when user pressed mouse, so unpause after choice is made
                                //System.out.println("=============== unpause ==============");
                                MtvApp.vscom.unpause();
                            } catch (IOException ioe) {
                                System.out.println("MTV tableChanged() " +ioe.toString() +".");
                            }
                            mode_selected = false;
                        }
                    }
                }
            });
            event_table.addMouseListener(new MouseAdapter() {

                public void mousePressed(MouseEvent e) {
                    if (event_table.isEnabled()) {
                        //System.out.println("<press>");
                        // Handle user choosing Mode here
                        JTable target = (JTable)e.getSource();
                        int row = target.getSelectedRow();
                        int column = target.getSelectedColumn();
                        String mode, name, command;
                        // handle the user click in Active / Hold / Mode column
                        if (event_table.isCellEditable(row,column)) {
                            //System.out.println("PRESS " + row + ", " + column);
                            name = MtvApp.mtv_events.get(eventmap[row]).name;
                            command = "";
                            if (column==7) { // Mode
                                // can't do the command here because user has to click again to make selection
                                // so set a flag that will make the tableChanged function handle it
                                mode_selected = true;
                                command = "<mode>...";
                                // pause while user makes choice
                                //System.out.println("===============+ pause +==============");
                                try {
                                    MtvApp.vscom.pause();
                                } catch (IOException ioe) {
                                    System.out.println("MTV mousePressed() " +ioe.toString() +".");
                                }
                            }
                            //System.out.println("   cmd= " + name + command);
                        } else {
                            // user clicked in non-edittable cell
                        }
                    } // end isEnabled
                }

                public void mouseReleased(MouseEvent e) {
                    if (event_table.isEnabled()) {
                        //System.out.println("<release>");
                        // Handle user clicking Active/Hold checkboxes here
                        JTable target = (JTable)e.getSource();
                        int row = target.getSelectedRow();
                        int column = target.getSelectedColumn();
                        String mode, name, command;
                        boolean checked;
                        if (event_table.isCellEditable(row,column)) {
                            //System.out.println("RELEASE " + row + ", " + column);
                            name = MtvApp.mtv_events.get(eventmap[row]).name;
                            command = "";
                            if (column==0) { // Active
                                checked = (Boolean)event_table.getValueAt(row, column);
                                active[row] = checked;
                                // if this is an event row, make sure its conditions/actions rows are set to same active color
                                if (rowtype[row]== RowType.EVENT) {
                                    int ii = row+1;
                                    while (MtvApp.mtv_events.get(eventmap[ii]).row==row) {
                                        // set its value (to the current value) to make the cell renderer get called
                                        event_table.setValueAt(event_table.getValueAt(ii,0),ii,0);
                                        ii++;
                                        if (ii==event_table_rows.size()) {
                                            break;
                                        }
                                    }
                                }
                                //System.out.println("   active= " + checked);
                                if (checked) {
                                    if (rowtype[row]== RowType.EVENT) {
                                        command = name + ".activate()";
                                    } else if (rowtype[row]== RowType.CONDITION) {
                                        command = name + ".condition_enable(" + condactnum[row] + ")";
                                    } else { // ACTION
                                        command = name + ".action_enable(" + condactnum[row] + ")";
                                    }
                                } else { // not checked
                                    if (rowtype[row]== RowType.EVENT) {
                                        command = name + ".deactivate()";
                                    } else if (rowtype[row]== RowType.CONDITION) {
                                        command = name + ".condition_disable(" + condactnum[row] + ")";
                                    } else { // ACTION
                                        command = name + ".action_disable(" + condactnum[row] + ")";
                                    }
                                }
                            }
                            if (column==6) { // Hold
                                checked = (Boolean)event_table.getValueAt(row, column);
                                //System.out.println("   hold= " + checked);
                                if (checked) {
                                    command = name + ".condition_hold_on(" + condactnum[row] + ")";
                                } else { // not checked
                                    command = name + ".condition_hold_off(" + condactnum[row] + ")";
                                }
                            }
                            if (column==8) { // Added
                                checked = (Boolean)event_table.getValueAt(row, column);
                                //System.out.println("   added= " + checked);
                                // make sure its conditions/actions rows are set to same active color
                                int ii = row+1;
                                while (MtvApp.mtv_events.get(eventmap[ii]).row==row) {
                                    // simply set any row value (to the current value) to make the cell renderer get called
                                    event_table.setValueAt(event_table.getValueAt(ii,0),ii,0);
                                    ii++;
                                    if (ii==event_table_rows.size()) {
                                        break;
                                    }
                                }
                                MtvApp.mtv_events.get(eventmap[row]).added = checked;
                                if (checked) {
                                    // if it has a target job, insert it before/after that job
                                    int before_after = MtvApp.mtv_events.get(eventmap[row]).before_after;
                                    if (before_after==1) {
                                        command = "trick.add_event_before(" + name +", \""
                                        +MtvApp.mtv_events.get(eventmap[row]).target_job+ "\")";
                                    } else if (before_after==2) {
                                        command = "trick.add_event_after(" + name +", \""
                                        +MtvApp.mtv_events.get(eventmap[row]).target_job+ "\")";
                                    } else {
                                        command = "trick.add_event(" + name + ")";
                                    }
                                } else { // not checked
                                    command = "trick.remove_event(" + name + ")";
                                }
                            }
                            if (column != 7) {
                                //System.out.println("   cmd= " + command);
                                try {
                                    MtvApp.vscom.put(command);
                                } catch (IOException ioe) {
                                    System.out.println("MTV mouseReleased() " +ioe.toString() +".");
                                }
                            }
                            // deselect the row after click is done
                            event_table.clearSelection();
                        } else {
                            // user clicked in non-edittable cell
                        }
                    } // end isEnabled
                }
            });
            event_table.setGridColor(resourceMap.getColor("event_table.gridColor")); // NOI18N
            event_table.setName("event_table"); // NOI18N
            event_table.setShowGrid(true);
            jScrollPane1.setViewportView(event_table);
            event_table.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);

            viewPanel.add(jScrollPane1, java.awt.BorderLayout.CENTER);

            toolBar.setRollover(true);
            toolBar.setBorderPainted(false);
            toolBar.setName("toolBar"); // NOI18N
            //DANNY
            toolBar.setLayout(new javax.swing.BoxLayout(toolBar, javax.swing.BoxLayout.LINE_AXIS));

            tool_label.setText(resourceMap.getString("tool_label.text")); // NOI18N
            tool_label.setMaximumSize(new java.awt.Dimension(550, 16));
            tool_label.setName("tool_label"); // NOI18N
            tool_label.setPreferredSize(new java.awt.Dimension(550, 16));
            toolBar.add(tool_label);

            simtime_label.setText(resourceMap.getString("simtime_label.text")); // NOI18N
            simtime_label.setName("simtime_label"); // NOI18N
            toolBar.add(simtime_label);

            simtime_text.setBackground(resourceMap.getColor("simtime_text.background")); // NOI18N
            simtime_text.setEditable(false);
            simtime_text.setText(resourceMap.getString("simtime_text.text")); // NOI18N
            simtime_text.setBorder(null);
            simtime_text.setFocusable(false);
            simtime_text.setMaximumSize(new java.awt.Dimension(80, 28));
            simtime_text.setName("simtime_text"); // NOI18N
            simtime_text.setPreferredSize(new java.awt.Dimension(80, 28));
            toolBar.add(simtime_text);

            trick_icon.setName("trick_icon"); // NOI18N
            toolBar.add(trick_icon);

            viewPanel.add(toolBar, java.awt.BorderLayout.PAGE_START);

            jTabbedPane1.addTab(resourceMap.getString("viewPanel.TabConstraints.tabTitle"), viewPanel); // NOI18N

            editPanel.setName("editPanel"); // NOI18N
            editPanel.setLayout(new java.awt.BorderLayout());

            jScrollPane2.setName("jScrollPane2"); // NOI18N

            edit_table_rows = new Vector<Vector>();
            edit_table_cols = new Vector<String>();
            user_text = new JTextArea();
            String[] col_names2 = {"Event Syntax", "", "User Data", ""};
            for (int ii=0; ii<col_names2.length; ii++) {
                edit_table_cols.addElement((String) col_names2[ii]);
            }
            model = new javax.swing.table.DefaultTableModel(
                edit_table_rows, edit_table_cols
                /***
                new Object [][] {
                    {event_name+ " = trick.sim_services.IPPythonEvent", "(\"", event_name, "\")"} // row0
                }
                new String [] {
                    "Event Syntax", "", "User Data", ""
                }***/
            ) {
                Class[] types = new Class [] {
                    java.lang.String.class, java.lang.String.class, java.lang.String.class, java.lang.String.class
                };
                boolean[] canEdit = new boolean [] {
                    false, false, true, false
                };

                public Class getColumnClass(int columnIndex) {
                    return types [columnIndex];
                }

                public boolean isCellEditable(int rowIndex, int columnIndex) {
                    return canEdit [columnIndex];
                }
            };
            edit_table.setModel(model);
            edit_table.setName("edit_table"); // NOI18N
            for (int ii=0; ii<col_names2.length; ii++) {
                edit_table.getColumnModel().getColumn(ii).setCellRenderer(new EditTableRenderer());
            }
            //DANNY set column widths
            edit_table.getColumnModel().getColumn(0).setMinWidth(50);
            edit_table.getColumnModel().getColumn(0).setPreferredWidth(150);
            edit_table.getColumnModel().getColumn(0).setMaxWidth(500);
            edit_table.getColumnModel().getColumn(0).setHeaderValue("Event Syntax");
            edit_table.getColumnModel().getColumn(1).setMinWidth(40);
            edit_table.getColumnModel().getColumn(1).setPreferredWidth(40);
            edit_table.getColumnModel().getColumn(1).setMaxWidth(40);
            edit_table.getColumnModel().getColumn(1).setHeaderValue("");
            edit_table.getColumnModel().getColumn(2).setMinWidth(50);
            edit_table.getColumnModel().getColumn(2).setPreferredWidth(300);
            edit_table.getColumnModel().getColumn(2).setMaxWidth(500);
            edit_table.getColumnModel().getColumn(2).setHeaderValue("User Data");
            edit_table.getColumnModel().getColumn(3).setMinWidth(40);
            edit_table.getColumnModel().getColumn(3).setPreferredWidth(40);
            edit_table.getColumnModel().getColumn(3).setMaxWidth(40);
            edit_table.getColumnModel().getColumn(3).setHeaderValue("");
            jScrollPane2.setViewportView(edit_table);

            editPanel.add(jScrollPane2, java.awt.BorderLayout.CENTER);

            toolBar2.setRollover(true);
            toolBar2.setBorderPainted(false);
            toolBar2.setName("toolBar2"); // NOI18N
            //DANNY
            toolBar2.setLayout(new javax.swing.BoxLayout(toolBar2, javax.swing.BoxLayout.LINE_AXIS));

            jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
            jLabel1.setName("jLabel1"); // NOI18N
            toolBar2.add(jLabel1);

            eventTextField.setText(edit_event_name);
            eventTextField.setMaximumSize(new java.awt.Dimension(540, 28));
            eventTextField.setName("eventTextField"); // NOI18N
            eventTextField.setPreferredSize(new java.awt.Dimension(540, 28));
            eventTextField.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    eventTextFieldActionPerformed(evt);
                }
            });
            toolBar2.add(eventTextField);

            send_button.setAction(actionMap.get("send_to_sim")); // NOI18N
            send_button.setText(resourceMap.getString("send_button.text")); // NOI18N
            send_button.setToolTipText(resourceMap.getString("send_button.toolTipText")); // NOI18N
            send_button.setFocusable(false);
            send_button.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
            send_button.setName("send_button"); // NOI18N
            send_button.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
            toolBar2.add(send_button);

            trick_icon2.setName("trick_icon2"); // NOI18N
            toolBar2.add(trick_icon2);

            editPanel.add(toolBar2, java.awt.BorderLayout.PAGE_START);

            jTabbedPane1.addTab(resourceMap.getString("editPanel.TabConstraints.tabTitle"), editPanel); // NOI18N

            setComponent(jTabbedPane1);
            setMenuBar(menuBar);
            setStatusBar(statusPanel);
        }// </editor-fold>//GEN-END:initComponents

    private void eventTextFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_eventTextFieldActionPerformed
        String new_name = evt.getActionCommand();
        String data;
        // replace any occurrences of the old event name with new name in the table
        for (int ii=0; ii<edit_table_rows.size(); ii++) {
            data = (String) edit_table.getValueAt(ii, 0);
            edit_table.setValueAt(data.replaceAll(edit_event_name, new_name), ii, 0);
            data = (String) edit_table.getValueAt(ii, 2);
            edit_table.setValueAt(data.replaceAll(edit_event_name, new_name), ii, 2);
        }
        edit_event_name = new_name;
        edit_table.setEnabled(true);
        // enable table and menus after user sets event name
        edit_table.setEnabled(true);
        editMenu.setEnabled(true);
        if (!connect_button.isEnabled()) {
            send_button.setEnabled(true);
        }
        edit_not_saved = true;
}//GEN-LAST:event_eventTextFieldActionPerformed

    private void customizeMenuItemActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_customizeMenuItemActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_customizeMenuItemActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JCheckBoxMenuItem confirmExitMenuItem;
    public javax.swing.JButton connect_button;
    private javax.swing.JMenuItem createMenuItem;
    private javax.swing.JMenuItem customizeMenuItem;
    private javax.swing.JMenu cycleMenu;
    private javax.swing.JRadioButtonMenuItem cycleMenuItem05;
    private javax.swing.JRadioButtonMenuItem cycleMenuItem10;
    private javax.swing.JRadioButtonMenuItem cycleMenuItem100;
    private javax.swing.JRadioButtonMenuItem cycleMenuItem25;
    private javax.swing.JRadioButtonMenuItem cycleMenuItem50;
    private javax.swing.JRadioButtonMenuItem cycleMenuItem75;
    private javax.swing.JMenuItem deleteMenuItem;
    private javax.swing.JMenu editMenu;
    public javax.swing.JPanel editPanel;
    private javax.swing.JMenuItem edit_actionMenuItem;
    private javax.swing.JMenuItem edit_action_disableMenuItem;
    private javax.swing.JMenuItem edit_activateMenuItem;
    private javax.swing.JMenuItem edit_add_eventMenuItem;
    private javax.swing.JMenuItem edit_add_event_afterMenuItem;
    private javax.swing.JMenuItem edit_add_event_beforeMenuItem;
    private javax.swing.JMenuItem edit_addlineMenuItem;
    private javax.swing.JMenu edit_addstmtMenu;
    private javax.swing.JMenuItem edit_conditionMenuItem;
    private javax.swing.JMenuItem edit_condition_allMenuItem;
    private javax.swing.JMenuItem edit_condition_disableMenuItem;
    private javax.swing.JMenuItem edit_condition_hold_onMenuItem;
    private javax.swing.JMenuItem edit_cycleMenuItem;
    private javax.swing.JMenuItem edit_deleteMenuItem;
    private javax.swing.JTable edit_table;
    private javax.swing.table.DefaultTableModel model;
    private JTextArea user_text;
    public Vector<Vector> edit_table_rows;
    private Vector<String> edit_table_cols;
    private javax.swing.JTextField eventTextField;
    public ButtonGroup cycle_group;
    public javax.swing.JComboBox comboBox; // Mode
    public javax.swing.JTable event_table;
    public Vector<Vector> event_table_rows;
    public Vector<String> event_table_cols;
    public boolean mode_selected; // user clicked a row's Mode combobox
    public javax.swing.JTextField host_field;
    private javax.swing.JLabel host_label;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JMenuItem jMenuItem1;
    private javax.swing.JMenuItem jMenuItem2;
    private javax.swing.JMenuItem jMenuItem3;
    private javax.swing.JMenuItem jMenuItem4;
    private javax.swing.JMenuItem jMenuItem5;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JPopupMenu.Separator jSeparator1;
    private javax.swing.JPopupMenu.Separator jSeparator2;
    private javax.swing.JPopupMenu.Separator jSeparator3;
    private javax.swing.JPopupMenu.Separator jSeparator4;
    public javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JMenuItem loadMenuItem;
    private javax.swing.JMenuBar menuBar;
    public javax.swing.JTextField port_field;
    private javax.swing.JLabel port_label;
    private javax.swing.JProgressBar progressBar;
    private javax.swing.JMenuItem saveMenuItem;
    public javax.swing.JButton send_button;
    private javax.swing.JLabel simtime_label;
    public javax.swing.JTextField simtime_text;
    private javax.swing.JLabel statusAnimationLabel;
    private javax.swing.JLabel statusMessageLabel;
    private javax.swing.JPanel statusPanel;
    public javax.swing.JLabel status_label;
    private javax.swing.JToolBar toolBar;
    private javax.swing.JToolBar toolBar2;
    private javax.swing.JLabel tool_label;
    private trick.common.ui.panels.SmallTrickIconLabel trick_icon;
    private trick.common.ui.panels.SmallTrickIconLabel trick_icon2;
    private javax.swing.JMenu viewMenu;
    public javax.swing.JPanel viewPanel;
    // End of variables declaration//GEN-END:variables

    public int current_tab;       // 0 = View tab, 1 = Edit tab
    public enum RowType { EVENT, CONDITION, ACTION }
    public enum Mode { NORMAL, MANUALON, MANUALOFF }
    public RowType rowtype[];     // what type is each row
    public int eventmap[];        // which event (index in mtv_events) does each row belong to
    public int condactnum[];      // which condition/action index does this row belong to
    public boolean is_red[];      // true when event/condition fired and is colored red
    public boolean is_green[];    // true when action ran and is colored green
    public boolean active[];      // is this row (event/condition/action) Active/Enabled
    public Mode mode[];           // is this row (event/condition/action) mode Normal, or Manual On/Off
    public boolean canEditRow[];  // whole row is not editable when user does not specify event name
    public boolean canEdit[][];   // if canEditRow, is this particular cell editable (Active/Enable/Hold/Mode)    
    public TrickResources resourceMap;

    private final Timer messageTimer;
    private final Timer busyIconTimer;
    private final Icon idleIcon;
    private final Icon[] busyIcons = new Icon[15];
    private int busyIconIndex = 0;
    private JDialog aboutBox;     // the about MTV popup

    public String edit_event_name;
    private int edit_condition_count;
    private int edit_action_count;
    private boolean edit_not_saved = false;
}
