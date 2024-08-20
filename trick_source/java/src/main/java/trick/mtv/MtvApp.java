/*
 * The MTV main application
 * @author dstrauss
 */

package trick.mtv;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.EventObject;
import java.util.Vector;

import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;
import javax.swing.JRadioButton;
import javax.swing.event.TableModelEvent;

import org.jdesktop.application.Application;

import trick.common.framework.BaseApplication;
import trick.common.framework.task.Task;
import trick.common.TrickApplication;
import trick.common.utils.VariableServerConnection;
import trick.mtv.MtvView.Mode;
import trick.mtv.MtvView.RowType;

/**
 * The main class of the application.
 */
public class MtvApp extends TrickApplication {

    public static String host;
    public static int port;
    public static VariableServerConnection vscom; // VarClient for receiving info from variable server
    public MtvView mtv_view;                // The mtv gui
    public static int mtv_list_count=0;     // Number of events in mtv list from sim which includes deleted (dummies)
    public static int mtv_count;            // Number of events displayed in mtv gui
    private int map[];                      // Map table index (mtv_count) to mtv_events data (mtv_list_count)
    public static int mtv_var_count;        // Total number of variables to get from var server
    public double sim_time = 0.0;           //   1st var to get: Current sim time
    public static int last_update_ticker;   //   2nd var to get: Running count of mtv table added or deleted event(s)
    public final int time_tics = 1000000;   // For converting time tics to seconds
    //private boolean updating=false;       // True when mtv gui is being updated, so ignore var server data
    public static boolean confirmExit=true; // Do you want the "do you really want to exit" popup ??
    public boolean need_update=false;       // True when event data has changed and need to udpate display
    public boolean one_shot=false;          // True when paused and need to get one shot of data from var server
    public static String default_event_dir = ""; // Directory to use for event loads and saves
    public static int max_events_show = 500;// Limit number of events shown in table
    
                                            // Menu options: If you change default of the following, also change it in menu item
    public double var_cycle = 0.50;         //    cycle time to get event data default 0.50

    public class EventInfo {
        String name;       // event name
        int cond_count;    // number of conditions
        String[] cond;     // condition strings
        int act_count;     // number of actions
        String[] act;      // action strings
        int row;           // starting row index in mtv event table
        boolean show;      // show/hide this event in gui
        boolean active;    // event active button on
        boolean added;     // event added button on (it's in Trick event list)
        boolean deleted;   // event was permanently deleted using delete_event
        int before_after;  // job insertion (0=none, 1=before, 2=after)
        String target_job; // the job name the event is inserted before/after
        JRadioButton butt; // button used in customize_event_display
    }
    public static ArrayList<EventInfo> mtv_events = new ArrayList<EventInfo>(100);          // Save info about events in gui (mtv_count)

    /**
     * Main method launching the application.
     * @param args command line arguments
     */
    public static void main(String[] args) {
        launch(MtvApp.class, args);
        // Get host and port to connect to
        host = "localhost";
        port = 7000;
    	if (args.length > 1) {
            try {
                host = args[0];
		port = Integer.parseInt(args[1]);
            } catch (NumberFormatException nfe) {
		System.out.println("MTV Invalid <host> <port> arguments. Using localhost and 7000.");
		host = "localhost";
		port = 7000;
            }
	}

    }

    // Set default event directory for loads/saves:
    // 1) TRICK_MTV_DIR property or 2) resource default_event_directory
    // if 1) and 2) not set or does not exist, use current directory
    public void set_default_directory() {
        boolean exists;
        if (default_event_dir.isEmpty()) {
            default_event_dir = trickProperties.getProperty("TRICK_MTV_DIR");
            if (default_event_dir == null) {
                default_event_dir = resourceMap.getString("default_event_directory");
            }
            if (default_event_dir == null) {
                default_event_dir = ".";
            }
            exists = (new File(default_event_dir).exists());
            if (!exists) {
                // user current directory if specified default dir does not exist
                default_event_dir = ".";
            }
        }
        trickProperties.setProperty("TRICK_MTV_DIR", default_event_dir);
    }
    
    @Override
    protected void initialize(String[] args) {
        String event_vars = new String();
        String time_vars = new String();
        String results[] = null;
        int numvars, rindex, start, end;
        JFrame popup;
        JProgressBar bar;
        boolean max_reached = false; // only allow max_events_show number of events in table

        //System.out.println("Initialize.");
        // Connect to variable server and request event info
    	try {
            if (vscom == null) {
                super.initialize(args);
                set_default_directory();
                // connect at startup
                vscom = new VariableServerConnection(host, port);
                last_update_ticker = 0;
            } else {
                // already connected, so we must be updating because event data has changed
            }
            if (vscom != null) {
                //System.out.println("Connect OK.");
                vscom.pause();
                vscom.put("trick.var_set_client_tag(\"TRICK_MTV\")");
                //vscom.setAscii();
                //vscom.setBinary();
                vscom.setBinaryNoNames();
                // -------------------------- get preliminary event data ---------------------------
//vscom.put("trick.var_debug(3)") ;
                vscom.put("trick.var_cycle(0.01)"); // get preliminary event data at this rate
                vscom.put("trick.var_add(\"trick_ip.mtv.mtv_count\"); trick.var_send(); trick.var_clear();");                   
                mtv_list_count = Integer.parseInt(vscom.get());
//System.out.println("mtv_count=" + mtv_list_count);


                // create a popup progress bar to show while getting event data
                popup = new JFrame();
                popup.setTitle("MTV Getting Simulation Event Data");
                int x,y;
                if (mtv_view==null) {
                    x = Integer.valueOf(MtvApp.getApplication().trickProperties.getProperty("x"));
                    y = Integer.valueOf(MtvApp.getApplication().trickProperties.getProperty("y"));
                } else {
                    x = mtv_view.getFrame().getX();
                    y = mtv_view.getFrame().getY();  
                }
                popup.setLocation(x+100, y+100);
                bar = new JProgressBar(0,mtv_list_count);
                popup.add(bar);
                popup.setAlwaysOnTop(true);
                popup.setSize(400, 100);
                popup.setVisible(true);

                vscom.put("trick.mtv_send_event_data();"); // get all event data from Trick
                numvars = Integer.parseInt(vscom.get());
                results = vscom.get(numvars).split("\t");
                rindex = 0; // index into results
                start = 0;
                end = mtv_list_count;
                mtv_count = 0; // how many events are shown
                for (int ii=start; ii < end; ii++) {
                    // get each event name, active, added, number of conditions, number of actions, before/after indicator
                    if (ii == mtv_events.size()) {
                        // new event data since previous load
                        mtv_events.add(ii, new EventInfo());
                        mtv_events.get(ii).show = (Integer.parseInt(results[rindex+2])==1); // show it if it's added
                        mtv_events.get(ii).deleted = false;
                        mtv_events.get(ii).butt = new JRadioButton(results[rindex+0]); //name
                    }                    
                    mtv_events.get(ii).name = results[rindex+0];
                    mtv_events.get(ii).active = (Integer.parseInt(results[rindex+1])==1);
                    mtv_events.get(ii).added = (Integer.parseInt(results[rindex+2])==1);
                    mtv_events.get(ii).cond_count = Integer.parseInt(results[rindex+3]);
                    mtv_events.get(ii).act_count = Integer.parseInt(results[rindex+4]);
                    mtv_events.get(ii).before_after = Integer.parseInt(results[rindex+5]);
                    rindex+=6;
                    if (mtv_view != null) {
                        mtv_view.status_label.setText(" Updating " + mtv_events.get(ii).name);
                    }
                    // update the progress bar
                    bar.setValue(ii);
                    bar.setStringPainted(true);
                    popup.repaint();
                    popup.update(popup.getGraphics());
                    
                    // read events have no name, or user may not have specified an event name, default is do not show these
                    if (mtv_events.get(ii).name.equals("no_name_specified") && (mtv_view==null)) {
                        mtv_events.get(ii).show = false;
                    }
                    // trick gives deleted events a dummy name (they are still in mtv_list)
                    if (mtv_events.get(ii).name.equals("XXX_DELETED_EVENT")) {
                        mtv_events.get(ii).deleted = true;
                        mtv_events.get(ii).show = false;
                    }
                    if (mtv_count==max_events_show) {
                        // limit number of events shown - still need Active and Add for custom_event_list popup info
                        mtv_events.get(ii).show = false;
                        max_reached = true;
                    }
                    mtv_events.get(ii).butt.setSelected(mtv_events.get(ii).show); // for customize_event_display
                    if (!mtv_events.get(ii).show) {
                        // elected not to show this event via customize_event_display (or we are over 100 events)
                        rindex += (mtv_events.get(ii).cond_count + mtv_events.get(ii).act_count);
                        if (mtv_events.get(ii).before_after > 0) rindex++;
                        continue;
                    }                    
                    
                    mtv_events.get(ii).cond = new String[mtv_events.get(ii).cond_count] ;
                    for (int jj=0; jj<mtv_events.get(ii).cond_count; jj++) {
                        //vscom.put("trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].cond[" +jj+ "].comment\"); trick.var_send(); trick.var_clear();");
                        //mtv_events.get(ii).cond[jj] = vscom.get();
                        mtv_events.get(ii).cond[jj] = results[rindex+jj];
                    }
                    rindex += mtv_events.get(ii).cond_count;
                    mtv_events.get(ii).act = new String[mtv_events.get(ii).act_count] ;
                    for (int jj=0; jj<mtv_events.get(ii).act_count; jj++) {
                        //vscom.put("trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].act[" +jj+ "].comment\"); trick.var_send(); trick.var_clear();");
                        //mtv_events.get(ii).act[jj] = vscom.get();
                        mtv_events.get(ii).act[jj] = results[rindex+jj];
                    }
                    rindex += mtv_events.get(ii).act_count;
                    if (mtv_events.get(ii).before_after > 0) {
                        //vscom.put("trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].target_job\"); trick.var_send(); trick.var_clear();");
                        //mtv_events.get(ii).target_job = vscom.get();
                        mtv_events.get(ii).target_job = results[rindex+0];
                        rindex++;
                    }
                    mtv_count++;
                }
                // get last mtv update ticker
                vscom.put("trick.var_add(\"trick_ip.mtv.mtv_update_ticker\"); trick.var_send(); trick.var_clear();");
                last_update_ticker = Integer.parseInt(vscom.get());
                map = new int[mtv_count]; // this will map the gui event table index to the mtv_events data
                
                // -------------------------- get cyclical event data ---------------------------
                mtv_var_count = 0;
                vscom.put("trick.var_cycle(" + var_cycle + ")");
                // tell var server to get sim time and mtv_udpate_ticker cyclically
                time_vars = "trick.var_add(\"trick_sys.sched.time_tics\"); ";
                time_vars += "trick.var_add(\"trick_ip.mtv.mtv_update_ticker\"); ";
                //System.out.println(time_vars);
                vscom.put(time_vars);
                mtv_var_count += 2;

                mtv_count = 0;       // only counts events to be displayed in gui
                for (int ii=0; ii < mtv_list_count; ii++) {
                    if (!mtv_events.get(ii).show) {
                        continue;
                    }
                    // tell var server to get status info for each event that we will read cyclically
                    map[mtv_count] = ii;
                    event_vars  = "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].active\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].fired_time\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].fired_count\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].ran_time\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].ran_count\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].manual\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].manual_fired\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].hold\"); ";
                    event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].added\"); ";
                    mtv_var_count += 9;
                    for (int jj=0; jj < mtv_events.get(ii).cond_count; jj++) {
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].cond[" +jj+ "].enabled\"); ";
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].cond[" +jj+ "].fired_time\"); ";
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].cond[" +jj+ "].fired_count\"); ";
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].cond[" +jj+ "].hold\"); ";
                        mtv_var_count += 4;
                    }
                    for (int jj=0; jj < mtv_events.get(ii).act_count; jj++) {
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].act[" +jj+ "].enabled\"); ";
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].act[" +jj+ "].ran_time\"); ";
                        event_vars += "trick.var_add(\"trick_ip.mtv.mtv_list[" +ii+ "][0].act[" +jj+ "].ran_count\"); ";
                        mtv_var_count += 3;
                    }
                    //System.out.println(event_vars);
                    vscom.put(event_vars);
                    mtv_count++;
                } // end for ii
                //System.out.println("mtv_var_count=" + mtv_var_count);
                if (max_reached) {
                    JOptionPane.showMessageDialog(popup, 
                            "Number of events displayed is limited to a maximum of " + max_events_show + ".\n" +
                            "Use \"Customize Event Display\" under the View menu to display the ones you want.",
                    "Warning", JOptionPane.WARNING_MESSAGE);
                }
                popup.dispose();
                vscom.unpause();
            } // end if vscom != null
	} catch (NumberFormatException nfe) {
            System.out.println("MTV initialize() " +nfe.toString() +".");
    	} catch (IOException ioe) {
            System.out.println("MTV initialize() " +ioe.toString() +".");
            vscom = null;
	}

    }
    
    public void add_my_exit_listener() {
        // the super's exit dialog always pops up in middle of screen, so add our own exit listener
        // remove any current exit listener first
        removeExitListener(exitListener) ;
        exitListener = new BaseApplication.ExitListener() {
            public boolean canExit(EventObject e) {
                return JOptionPane.showConfirmDialog(mtv_view.viewPanel,
                        "Do you really want to exit?", "Confirm Exit", 0) == 0;
            }
            public void willExit(EventObject e) {}
        };
        addExitListener(exitListener) ;
    }
    
    /**
     * At startup create and show the main frame of the application.
     */
    @Override protected void startup() {
        Vector<Object> row;
        Vector<Object> default_row = new Vector<Object>(Arrays.asList(false,null,null,null,null,null,false,null,false));
        //Vector<Object> default_row = new Vector<Object>(Arrays.asList(false,null,null,null,null,null,false,null,null)); //DANNY add hidden column on end
        int rownum;
        boolean deadrow;

        //System.out.println("Startup.");
        if (mtv_view==null) {
            // set application icon ??
            getMainFrame().setIconImage(resourceMap.getImageIcon("Application.icon").getImage());
            confirmExit = Boolean.valueOf(MtvApp.getApplication().trickProperties.getProperty("confirmExit"));
            if (confirmExit) {
                add_my_exit_listener();
            }
            mtv_view = new MtvView(this);
        }
        if (!confirmExit) {
            //remove exitlistener that super possibly added
            removeExitListener(exitListener) ;
        }

        // Add a row to the table for each event to display
        for (int ii=0; ii < mtv_count; ii++) {
            row = new Vector<Object>(default_row);
            mtv_view.event_table_rows.addElement(row);
            rownum = mtv_view.event_table_rows.size()-1;
            mtv_events.get(map[ii]).row = rownum;
            // Add a row for each condition
            for (int jj=0; jj<mtv_events.get(map[ii]).cond_count; jj++ ) {
                row = new Vector<Object>(default_row);
                mtv_view.event_table_rows.addElement(row);
                //rownum = mtv_view.event_table_rows.size()-1;
                //mtv_view.event_table.setValueAt("   condition("+jj+")", rownum, 1);
            }
            // Add a row for each action
            for (int jj=0; jj<mtv_events.get(map[ii]).act_count; jj++ ) {
                row = new Vector<Object>(default_row);
                mtv_view.event_table_rows.addElement(row);
                //rownum = mtv_view.event_table_rows.size()-1;
                //mtv_view.event_table.setValueAt("      action("+jj+")", rownum, 1);
            }
        }

        // keep track of each row type of rows in event_table (EVENT, CONDITION, or ACTION)
        mtv_view.rowtype = new RowType[mtv_view.event_table_rows.size()];
        // keep track of which event each row belongs to and which condition/action index
        mtv_view.eventmap = new int[mtv_view.event_table_rows.size()];
        mtv_view.condactnum = new int[mtv_view.event_table_rows.size()];
        // keep track of manual on/off so row can be colored blue
        mtv_view.mode = new Mode[mtv_view.event_table_rows.size()];
        // make whole row not editable when user does not specify event name
        mtv_view.canEditRow = new boolean[mtv_view.event_table_rows.size()];
        // set Active, Hold, and Mode checkboxes editable where appropriate
        mtv_view.canEdit = new boolean[mtv_view.event_table_rows.size()][mtv_view.event_table_cols.size()];
        //mtv_view.canEdit = new boolean[mtv_view.event_table_rows.size()][mtv_view.event_table_cols.size()+1]; //DANNY
        // keep track of when fired column is set to red & when ran column is set to green
        mtv_view.is_red = new boolean[mtv_view.event_table_rows.size()];
        mtv_view.is_green = new boolean[mtv_view.event_table_rows.size()];
        // keep track of active/enabled rows so row can be dimmed
        mtv_view.active = new boolean[mtv_view.event_table_rows.size()];
        for (int ii=0; ii < mtv_count; ii++) {
            rownum = mtv_events.get(map[ii]).row;
            mtv_view.rowtype[rownum] = RowType.EVENT;
            mtv_view.mode[rownum] = Mode.NORMAL;
            mtv_view.eventmap[rownum] = map[ii];
            mtv_view.is_red[rownum] = false;
            mtv_view.is_green[rownum] = false;
            mtv_view.canEdit[rownum][0] = true; // Active
            mtv_view.canEdit[rownum][7] = true; // Mode
            mtv_view.canEdit[rownum][8] = true; // Added
            deadrow = (mtv_events.get(map[ii]).name.equals("no_name_specified"));
            mtv_view.canEditRow[rownum] = !deadrow;
            mtv_view.event_table.setValueAt(mtv_events.get(map[ii]).name, rownum, 1); // Name
            //mtv_view.event_table.setValueAt(mtv_events.get(map[ii]).name, rownum, 8); // Hidden Name DANNY
            for (int jj=0; jj<mtv_events.get(map[ii]).cond_count; jj++ ) {
                rownum++;
                mtv_view.rowtype[rownum] = RowType.CONDITION;
                mtv_view.mode[rownum] = Mode.NORMAL;
                mtv_view.eventmap[rownum] = map[ii];
                mtv_view.condactnum[rownum] = jj;
                mtv_view.is_red[rownum] = false;
                mtv_view.canEdit[rownum][0] = true; // Active
                mtv_view.canEdit[rownum][6] = true; // Hold
                mtv_view.canEdit[rownum][8] = false; // Added
                mtv_view.canEditRow[rownum] = !deadrow;
                mtv_view.event_table.setValueAt("_cond("+jj+"): " +mtv_events.get(map[ii]).cond[jj], rownum, 1); // Condition comment
                //mtv_view.event_table.setValueAt("   cond("+jj+") ", rownum, 1); // Condition indicator
                //mtv_view.event_table.setValueAt(mtv_events.get(map[ii]).name, rownum, 8); // Hidden Name DANNY
            }
            for (int jj=0; jj<mtv_events.get(map[ii]).act_count; jj++ ) {
                rownum++;
                mtv_view.rowtype[rownum] = RowType.ACTION;
                mtv_view.mode[rownum] = Mode.NORMAL;
                mtv_view.eventmap[rownum] = map[ii];
                mtv_view.condactnum[rownum] = jj;
                mtv_view.is_green[rownum] = false;
                mtv_view.canEdit[rownum][0] = true; // Active
                mtv_view.canEdit[rownum][8] = false; // Added
                mtv_view.canEditRow[rownum] = !deadrow;
                mtv_view.event_table.setValueAt("__act("+jj+"): " +mtv_events.get(map[ii]).act[jj], rownum, 1); // Action comment
                //mtv_view.event_table.setValueAt("      action("+jj+")", rownum, 1);       // Action indicator
                //mtv_view.event_table.setValueAt(mtv_events.get(map[ii]).name, rownum, 8); // Hidden Name DANNY
            }
        }
 /***
            //DANNY allow user to sort by columns
            TableRowSorter<DefaultTableModel> sorter =
                    new TableRowSorter<DefaultTableModel>((DefaultTableModel) mtv_view.event_table.getModel());
            mtv_view.event_table.setRowSorter(sorter);
            // sort by hidden event name column 8, then by name column 1 to keep each event's data together
            List <RowSorter.SortKey> sortKeys = new ArrayList<RowSorter.SortKey>();
            sortKeys.add(new RowSorter.SortKey(8, SortOrder.ASCENDING));
            sortKeys.add(new RowSorter.SortKey(1, SortOrder.ASCENDING));
            sorter.setSortKeys(sortKeys);
 ***/

        // manually set the initial column sizes
        //mtv_view.event_table.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_OFF);
        mtv_view.event_table.getColumnModel().getColumn(0).setPreferredWidth(20);  // Active
        mtv_view.event_table.getColumnModel().getColumn(1).setPreferredWidth(225); // Name
        mtv_view.event_table.getColumnModel().getColumn(2).setPreferredWidth(72);  // Fired Time
        mtv_view.event_table.getColumnModel().getColumn(3).setPreferredWidth(72);  // Fired Count
        mtv_view.event_table.getColumnModel().getColumn(4).setPreferredWidth(72);  // Ran Time
        mtv_view.event_table.getColumnModel().getColumn(5).setPreferredWidth(72);  // Ran Count
        mtv_view.event_table.getColumnModel().getColumn(6).setPreferredWidth(50);  // Hold
        mtv_view.event_table.getColumnModel().getColumn(7).setPreferredWidth(82);  // Mode
        mtv_view.event_table.getColumnModel().getColumn(8).setPreferredWidth(20);  // Added

        // force all rows to be redrawn when updating
        //    this does it: tell event table listener to forget previous state
        TableModelEvent tme = new TableModelEvent(mtv_view.event_table.getModel());
        mtv_view.event_table.tableChanged(tme);

        show(mtv_view);
        mtv_view.host_field.setText(host);
        mtv_view.port_field.setText(""+port);
        if (vscom != null) {
            mtv_view.connect_button.setEnabled(false);
            mtv_view.status_label.setText(" Connected");
            mtv_view.event_table.setEnabled(true);
            // don't allow Send to Sim in Edit tab until user gives event a name
            if (mtv_view.edit_event_name.equals("put_new_event_name_here")) {
                mtv_view.send_button.setEnabled(false);
            } else {
                mtv_view.send_button.setEnabled(true);
            }
        } else {
            mtv_view.connect_button.setEnabled(true);
            mtv_view.status_label.setText(" Could not connect to " +host + ":"+port);
            mtv_view.event_table.setEnabled(false);
            mtv_view.send_button.setEnabled(false);
        }
        //updating = false;

    }

    @Override
    protected void ready() {
        //System.out.println("Ready.");
        super.ready();
        if (vscom == null) {
	    return;
	} 
	MonitorEventsTask monitorEventsTask = new MonitorEventsTask(this);
	//monitorEventsTask.addPropertyChangeListener(this);
	getContext().getTaskService().execute(monitorEventsTask);    
    }

    public void update() {
        // delete all rows from event table then redo it
        boolean background_reader_is_up = false;
        //System.out.println("Update.");

        mtv_view.status_label.setText(" Updating...");
        //updating = true;
        if (vscom != null) {
            background_reader_is_up = true;
            try {
                vscom.put("trick.var_clear()");
                vscom.flushInput(); // haven't seen this flush anything yet, may not need
            } catch (IOException ioe) {
                System.out.println("MTV update() " +ioe.toString() +".");
            }
            /***/
            // do I need this delay?
            try {
                // wait for a little more than a cycle so var server can finish sending any data
                long wait_msecs = (long)(var_cycle * 1000.0 + 50.0);
                //System.out.println("...wait " + wait_msecs / 1000.0);
                Thread.sleep(wait_msecs);
                //System.out.println("...wait done.");
            } catch (InterruptedException ignore) {
            }
             /***/
        }

        mtv_view.event_table_rows.removeAllElements();
        initialize(null);
        need_update = false;
        if (vscom == null) {
            mtv_view.status_label.setText(" Could not connect to " +host + ":"+port);
            return;
        }
        startup();
        if (background_reader_is_up) {
            return;
        }
        ready();
        mtv_view.status_label.setText(" Connected");
        mtv_view.host_field.setText(host);
        mtv_view.port_field.setText(""+port);
        
    }

    /**
     * This method is to initialize the specified window by injecting resources.
     * Windows shown in our application come fully initialized from the GUI
     * builder, so this additional configuration is not needed.
     */
    @Override
    protected void configureWindow(java.awt.Window root) {
    }

    @Override
    protected JComponent createMainPanel() {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    @Override
    protected void shutdown() {
        //System.out.println("Shutdown.");
        if (vscom != null) {
            try {
                vscom.put("trick.var_clear()");
                vscom.close();
            } catch (IOException ioe) {
                System.out.println("MTV shutdown() " +ioe.toString() +".");
	    }
        }
        // save position
        trickProperties.setProperty("x", String.valueOf(mtv_view.getFrame().getX()) );
        trickProperties.setProperty("y", String.valueOf(mtv_view.getFrame().getY()) );
        super.shutdown();
    }

    /**
     * A convenient static getter for the application instance.
     * @return the instance of MtvApp
     */
    public static MtvApp getApplication() {
        return BaseApplication.getInstance(MtvApp.class);
    }

    // MonitorEventsTask =======================================================
    /**
     * Inner class for the task of monitoring events in the currently running simulation.
     */
    private class MonitorEventsTask extends Task<Void, Void> {

        /**
        * Constructor with specified {@link Application}.
        *
        * @param app	The specified {@link Application} that needs Sim status monitoring.
        */
        public MonitorEventsTask(BaseApplication app) {
            super(app);
        }

        @Override
        protected Void doInBackground() {
        // Periodically read event info from variable server
            int index, rownum, count;
            double time;
            int update_ticker=0;
            double color_duration = 1.0;
            boolean fired, ran, hold, remove_color, event_active_state_changed;
            String mode;
            String results[] = null;

            while (true) {
                // Cyclic data as read from var server:
                // Time info:          sim_time, mtv_update_ticker
                // Event info:
                // (for each event)    active, fired_time, fired_count, ran_time, ran_count, manual, manual_fired, hold, added
                // (for each condtion) enabled, fired_time, fired_count, hold
                // (for each action)   enabled, ran_time, ran_count

                try {                        
                    if (vscom != null) {
                        // Read data when available on the socket (don't read when paused unless getting one shot data)
                        if (vscom.isPaused() & !one_shot) {
                            continue;
                        }
                        if (need_update) {
                        // mtv forced an update (e.g. user deleted an event), need to udpate mtv event table before getting any more data
                            update();
                            continue;
                        }
	                results = vscom.get(mtv_var_count).split("\t");
                        if ((results == null) || (results.length != mtv_var_count)) {
                            System.out.println("Read unknown (length= " + results.length +
                                    " expected= " +mtv_var_count+ "), exitting.");
                            break;
                        }
                        // if this is a one shot read during pause, don't process any further
                        // whichever routine doing the one shot will process the results
                        if (one_shot) {
                            one_shot = false;
                            continue;
                        }
                        // Debug print out what you read
                        /***
                        System.out.print("Read results: length= " + results.length);
                        for (int ii=0; ii<results.length; ii++) {
                            System.out.print(" <" + results[ii] + ">");
                        }
                        System.out.println();
                        ***/

                        index = 0;  // index into results that we read
                        // 1st 2 variables are sim time and mtv update time; they are long long but get them as double??
                        if (Double.parseDouble(results[index+0]) / time_tics > sim_time) {
                            sim_time = Double.parseDouble(results[index+0]) / time_tics;
                            mtv_view.simtime_text.setText(" " + ((Double)sim_time).toString());
                            //System.out.println("simtime=" + sim_time);
                        }
                        update_ticker = Integer.parseInt(results[index+1]);
                        //System.out.println("update ticker=" +update_ticker);
                        if (update_ticker != last_update_ticker) {
                            // event(s) have been added or deleted in user code, time to update mtv event table
                            //System.out.println("last update ticker=" +last_update_ticker+ " update_ticker=" +update_ticker);
                            last_update_ticker = update_ticker;
                            update();
                            continue;
                        }
                        index += 2;

                        rownum = 0; // row number in table to update
                        for (int ii=0; ii<mtv_count; ii++) {                                                   // EVENT
                            event_active_state_changed = false;
                            mtv_events.get(map[ii]).active = Integer.parseInt(results[index+0])==1;
                            mtv_view.active[rownum] = mtv_events.get(map[ii]).active;
                            // set active value so that cell renderer is called which ensures all rows get displayed
                            if ( ! mtv_view.event_table.getValueAt(rownum, 0).equals( mtv_view.active[rownum] ) ) {
                                mtv_view.event_table.setValueAt(mtv_view.active[rownum], rownum, 0);           // Active
                                event_active_state_changed = true;
                            }
                            time = Double.parseDouble(results[index+1]);
                            count = Integer.parseInt(results[index+2]);
                            fired = (mtv_view.event_table.getValueAt(rownum, 3)==null)
                                  ? count > 0
                                  : count > (Integer)mtv_view.event_table.getValueAt(rownum, 3);
                            // set event fired column color red for a second
                            remove_color = (mtv_view.is_red[rownum]) && (sim_time >= time + color_duration);
                            if (fired || remove_color) {
                                mtv_view.is_red[rownum] = !remove_color;
                                mtv_view.event_table.setValueAt(time, rownum, 2);                              // Fired Time
                                mtv_view.event_table.setValueAt(count, rownum, 3);                             // Fired Count
                            }
                            time = Double.parseDouble(results[index+3]);
                            count = Integer.parseInt(results[index+4]);
                            ran =   (mtv_view.event_table.getValueAt(rownum, 5)==null)
                                ? count > 0
                                : count > (Integer)mtv_view.event_table.getValueAt(rownum, 5);
                            // set event ran column color green for a second
                            remove_color = (mtv_view.is_green[rownum]) && (sim_time >= time + color_duration);
                            if (ran || remove_color) {
                                mtv_view.is_green[rownum] = !remove_color;
                                mtv_view.event_table.setValueAt(time, rownum, 4);                              // Ran Time
                                mtv_view.event_table.setValueAt(count, rownum, 5);                             // Ran Count
                            }
                            if (Integer.parseInt(results[index+5])==1) {           // manual true              // Mode:
                                // disable event Active button in manual mode
                                mtv_view.canEdit[rownum][0] = false;
                                if (Integer.parseInt(results[index+6])==1) {       //   fired
                                    if (Integer.parseInt(results[index+7])==1) {   //   hold on
                                        mode = "Manual ON";                                                    // Manual ON
                                        mtv_view.mode[rownum] = Mode.MANUALON;
                                    } else {                                       //   hold off
                                        mode = "Manual FIRE";                                                  // Manual FIRED
                                        mtv_view.mode[rownum] = Mode.MANUALON;
                                    }
                                } else {                                           //   fired off
                                    mode = "Manual OFF";                                                       // Manual OFF
                                    mtv_view.mode[rownum] = Mode.MANUALOFF;
                                }
                            } else {                                               // manual false
                                mode = "Normal";                                                               // Normal
                                mtv_view.mode[rownum] = Mode.NORMAL;
                                mtv_view.canEdit[rownum][0] = true;
                            }
                            if (mode != mtv_view.event_table.getValueAt(rownum, 7)) {
                                mtv_view.event_table.setValueAt(mode, rownum, 7);
                            }
                            mtv_events.get(map[ii]).added = (Integer.parseInt(results[index+8])==1);           // Added
                            if ( ! mtv_view.event_table.getValueAt(rownum, 8).equals(mtv_events.get(map[ii]).added) ) {
                                mtv_view.event_table.setValueAt(mtv_events.get(map[ii]).added, rownum, 8);
                                event_active_state_changed = true;
                            }
                            index += 9;
                            rownum++;
                            for (int jj=0; jj<mtv_events.get(map[ii]).cond_count; jj++ ) {                              // CONDITION
                                if (event_active_state_changed) {
                                    // the event's Active or Added was clicked, so simply touch row so that it is rendered again
                                    mtv_view.event_table.setValueAt(mtv_view.event_table.getValueAt(rownum, 0), rownum, 0);
                                }
                                if (mtv_view.mode[mtv_events.get(mtv_view.eventmap[rownum]).row] != Mode.NORMAL) {
                                    // disable condition Active button in manual mode
                                    mtv_view.canEdit[rownum][0] = false;
                                } else {
                                    mtv_view.canEdit[rownum][0] = true;
                                }
                                mtv_view.active[rownum] = Integer.parseInt(results[index+0])==1;
                                if ( ! mtv_view.event_table.getValueAt(rownum, 0).equals( mtv_view.active[rownum] ) ) {
                                    mtv_view.event_table.setValueAt(mtv_view.active[rownum], rownum, 0);           // Enabled
                                }
                                time = Double.parseDouble(results[index+1]);
                                count = Integer.parseInt(results[index+2]);
                                fired = (mtv_view.event_table.getValueAt(rownum, 3)==null)
                                      ? count > 0
                                      : count > (Integer)mtv_view.event_table.getValueAt(rownum, 3);
                                // set condition fired column color red for a second
                                remove_color = (mtv_view.is_red[rownum]) && (sim_time >= time + color_duration);
                                if (fired || remove_color) {
                                    mtv_view.is_red[rownum] = !remove_color;
                                    mtv_view.event_table.setValueAt(time, rownum, 2);                              // Fired Time
                                    mtv_view.event_table.setValueAt(count, rownum, 3);                             // Fired Count
                                }
                                hold = Integer.parseInt(results[index+3])==1;
                                if ( ! mtv_view.event_table.getValueAt(rownum, 6).equals( hold ) ) {
                                    mtv_view.event_table.setValueAt(hold, rownum, 6);                              // Hold
                                }
                                index += 4;
                                rownum++;
                            }
                            for (int jj=0; jj<mtv_events.get(map[ii]).act_count; jj++ ) {                               // ACTION
                                if (event_active_state_changed) {
                                    // the event's Active or Added was clicked, so simply touch row so that it is rendered again
                                    mtv_view.event_table.setValueAt(mtv_view.event_table.getValueAt(rownum, 0), rownum, 0);
                                }
                                mtv_view.active[rownum] = Integer.parseInt(results[index+0])==1;
                                if ( ! mtv_view.event_table.getValueAt(rownum, 0).equals(mtv_view.active[rownum]) ) {
                                    mtv_view.event_table.setValueAt(mtv_view.active[rownum], rownum, 0);           // Enabled
                                }
                                time = Double.parseDouble(results[index+1]);
                                count = Integer.parseInt(results[index+2]);
                                ran = (mtv_view.event_table.getValueAt(rownum, 5)==null)
                                    ? count > 0
                                    : count > (Integer)mtv_view.event_table.getValueAt(rownum, 5);
                                // set action ran column color green for a second
                                remove_color = (mtv_view.is_green[rownum]) && (sim_time >= time + color_duration);
                                if (ran || remove_color) {
                                    mtv_view.is_green[rownum] = !remove_color;
                                    mtv_view.event_table.setValueAt(time, rownum, 4);                              // Ran Time
                                    mtv_view.event_table.setValueAt(count, rownum, 5);                             // Ran Count
                                }
                                index += 3;
                                rownum++;
                            }
                        } // end for ii

                    } else {
                        System.out.println("MTV doInBackground() VarClient is null.");
                        break;
                    }
                } catch (Exception ioe) {
                    System.out.println("MTV doInBackground() " +ioe.toString() +".");
                    break;
                }
            } // end whild true
            return null;
        }

        @Override
        protected void succeeded(Void ignored) {
            //System.out.println("Succeeded.");
        }

        @Override
        protected void finished() {
            //System.out.println("Finished.");
            if (vscom != null) {
                try {
                    vscom.put("trick.var_clear()");
                    vscom.close();
                } catch (IOException ioe) {  
                    System.out.println("MTV finished() " +ioe.toString() +".");
                } 
            }
            mtv_view.connect_button.setEnabled(true);
            mtv_view.status_label.setText(" Disconnected");
            mtv_view.event_table.setEnabled(false);
            mtv_view.send_button.setEnabled(false);
            mtv_list_count = 0;
            vscom = null;
        }
    } // End MonitorEventsTask =================================================

}
