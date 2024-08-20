package trick.montemonitor;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.Icon;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;
import javax.swing.SwingWorker;
import javax.swing.UIManager;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.DefaultTableModel;

import org.jdesktop.application.Application;
import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXTable;
import org.jdesktop.swingx.JXTextField;
import org.jdesktop.swingx.decorator.HighlighterFactory;
import org.jdesktop.swingx.renderer.CellContext;
import org.jdesktop.swingx.renderer.DefaultTableRenderer;
import org.jdesktop.swingx.renderer.LabelProvider;

import trick.common.framework.BaseApplication;
import trick.common.framework.View;
import trick.common.RunTimeTrickApplication;

/**
 * MonteMonitor displays information on a montle carlo simulation.
 *
 * @author Derek Bankieris
 */
public class MonteMonitorApplication extends RunTimeTrickApplication {

    // Active sims.
    private ArrayList<Slave> slaves = new ArrayList<Slave>();

    // Displays progress and number of runs.
    private JProgressBar progressBar;

    // Table for displaying simulation information.
    private JXTable slaveTable;

    // Provides for adding new slaves.
    private JXTextField newSlaveTextField;

    // This popup menu responds to right clicks in the slaveTable.
    private JPopupMenu popupMenu;

    /** runs all the slaves */
    AbstractAction runMonteAction = new AbstractAction("Start") {
        {
        putValue(MNEMONIC_KEY, KeyEvent.VK_S);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            for (Slave slave : slaves) {
                runSlave(slave);
            }
        }
    };

    /** stops all the slaves */
    AbstractAction stopMonteAction = new AbstractAction("Stop") {
        {
        putValue(MNEMONIC_KEY, KeyEvent.VK_P);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            for (Slave slave : slaves) {
                stopSlave(slave);
            }
        }
    };

    /** runs the selected slaves */
    AbstractAction runSlavesAction = new AbstractAction("Start") {
        {
        putValue(MNEMONIC_KEY, KeyEvent.VK_S);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            int[] selectionIndicies = slaveTable.getSelectedRows();
            for (int i = 0; i < selectionIndicies.length; ++i) {
                selectionIndicies[i] = slaveTable.convertRowIndexToModel(selectionIndicies[i]);
            }
            for (int index : selectionIndicies) {
                runSlave(slaves.get(index));
            }
        }
    };

    /** stops the selected slaves */
    AbstractAction stopSlavesAction = new AbstractAction("Stop") {
        {
        putValue(MNEMONIC_KEY, KeyEvent.VK_P);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            int[] selectionIndicies = slaveTable.getSelectedRows();
            for (int i = 0; i < selectionIndicies.length; ++i) {
                selectionIndicies[i] = slaveTable.convertRowIndexToModel(selectionIndicies[i]);
            }
            for (int index : selectionIndicies) {
                stopSlave(slaves.get(index));
            }
        }
    };

    /** adds a new slave */
    AbstractAction addSlaveAction = new AbstractAction("Add") {
        {
        putValue(MNEMONIC_KEY, KeyEvent.VK_A);
        }
        public void actionPerformed(ActionEvent actionEvent) {
            String text = newSlaveTextField.getText().replace(" ", "");
            newSlaveTextField.setText(text);
            if (!text.isEmpty()) {
                try {
                    variableServerConnection.put("trick_mc.mc.add_slave(\"" + text + "\")");
                }
                catch (Exception e) {}
            }
        }
    };

    @Override
    protected void connect() throws IOException {
        super.connect();

        variableServerConnection.pause();
        variableServerConnection.put("trick.var_set_client_tag(\"TRICK_Monte\")");
        variableServerConnection.add("trick_mc.mc.enabled");
        variableServerConnection.add("trick_mc.mc.slave_id");
        variableServerConnection.poll();

        String[] data = variableServerConnection.get().split("\t");
        try {
            if (Integer.parseInt(data[1]) == 1 && Integer.parseInt(data[2]) == 0) {
                // Clear any data from a previous connection.
                for (int i = slaves.size() - 1; i >= 0; --i) {
                    removeSlave(i);
                }

                // Start getting updates in an infinite backgound Worker thread.
                new Monitor().execute();
                variableServerConnection.clear();
                variableServerConnection.add("trick_mc.mc.actual_num_runs");
                variableServerConnection.add("trick_mc.mc.num_results");
                variableServerConnection.add("trick_mc.mc.num_slaves");
                sendCyclePeriod();
                variableServerConnection.unpause();
            }
            else {
                throw new NumberFormatException();
            }
        }
        catch (NumberFormatException numberFormatException) {
            throw new IOException("This simulation is not a Monte Carlo master.");
        }
    }

    @Override
    protected void disconnect(Exception exception) {
        try {
            super.disconnect(exception);
        }
        catch (IOException ioException) {}
    }

    /**
     * sets the various GUI elements and Action enabled states
     *
     * @param connected the connection state
     */
    protected void setConnectionState(boolean connected) {
        super.setConnectionState(connected);
        updateMonteActions();
        updateSlaveActions();
        addSlaveAction.setEnabled(connected);
        progressBar.setForeground(connected ? UIManager.getColor("ProgressBar.foreground") :
          progressBar.getValue() == progressBar.getMaximum() ? Color.GREEN : Color.RED);
    }

    /** sets the Monte Carlo Action enabled states */
    private void updateMonteActions() {
        runMonteAction.setEnabled(false);
        stopMonteAction.setEnabled(false);
        if (getConnectionState()) {
            for (Slave slave : slaves) {
                switch(slave.state) {
                    case READY:
                    case RUNNING:
                    case UNRESPONSIVE_RUNNING:
                        stopMonteAction.setEnabled(true);
                        break;
                    case STOPPING:
                    case UNRESPONSIVE_STOPPING:
                    case STOPPED:
                        runMonteAction.setEnabled(true);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    /** sets the slave Action enabled states */
    private void updateSlaveActions() {
        int[] selectionIndicies = slaveTable.getSelectedRows();
        runSlavesAction.setEnabled(false);
        stopSlavesAction.setEnabled(false);
        if (getConnectionState()) {
            for (int index : selectionIndicies) {
                switch(slaves.get(slaveTable.convertRowIndexToModel(index)).state) {
                    case READY:
                    case RUNNING:
                    case UNRESPONSIVE_RUNNING:
                        stopSlavesAction.setEnabled(true);
                        break;
                    case STOPPING:
                    case UNRESPONSIVE_STOPPING:
                    case STOPPED:
                        runSlavesAction.setEnabled(true);
                        break;
                }
            }
        }
    }

    /**
     * adds the specified slave
     *
     * @param slave the slave to be added
     */
    private void addSlave(Slave slave) {
        slaves.add(slave);
        ((DefaultTableModel)slaveTable.getModel()).fireTableRowsInserted(slaves.size() - 1, slaves.size() - 1);
    }

    /**
     * removes the slave at the specified index
     *
     * @param index the slave of the slave to be removed
     */
    private void removeSlave(int index) {
        slaves.remove(index);
        ((DefaultTableModel)slaveTable.getModel()).fireTableRowsDeleted(index, index);
    }

    /**
     * runs the specified slave
     *
     * @param slave the slave to be run
     */
    private void runSlave(Slave slave) {
        try {
            variableServerConnection.put("trick_mc.mc.start_slave(" + slave.id + ")");
        }
        catch (Exception e) {}
    }

    /**
     * stops the specified slave
     *
     * @param slave the slave to be stopped
     */
    private void stopSlave(Slave slave) {
        try {
            variableServerConnection.put("trick_mc.mc.stop_slave(" + slave.id + ")");
        }
        catch (Exception e) {}
    }

    @Override
    protected void startup() {
        super.startup();

        View view = getMainView();
        view.setMenuBar(createMenuBar());
        view.setToolBar(createToolBar());
        view.setComponent(createMainPanel());

        // Build the popup menu.
        popupMenu = new JPopupMenu() {{
            add(new JMenuItem(runSlavesAction));
            add(new JMenuItem(stopSlavesAction));
        }};

        setConnectionState(false);

        show(view);

        JFrame mainFrame = getMainFrame();
        Rectangle bounds = mainFrame.getBounds();
        mainFrame.setBounds(new Rectangle(
          Integer.parseInt(trickProperties.getProperty(xKey, Integer.toString(bounds.x))),
          Integer.parseInt(trickProperties.getProperty(yKey, Integer.toString(bounds.y))),
          Integer.parseInt(trickProperties.getProperty(widthKey, Integer.toString(bounds.width))),
          Integer.parseInt(trickProperties.getProperty(heightKey, Integer.toString(bounds.height)))));
    }

    @Override
    protected JComponent createMainPanel() {
        return new JXPanel(new BorderLayout()) {

            Integer noRunNumber = -1;

            {
            progressBar = new JProgressBar() {{
                setMinimum(0);
                setStringPainted(true);
            }};
            add(progressBar, BorderLayout.NORTH);

            slaveTable = new JXTable(new DefaultTableModel() {

                private final String[] columnNames = {"ID", "Status", "Machine Name", "Current Run", "Runs Completed"};

                @Override
                public int getRowCount() {
                    return slaves.size();
                }

                @Override
                public int getColumnCount() {
                    return columnNames.length;
                }

                @Override
                public String getColumnName(int column) {
                    return columnNames[column];
                }

                @Override
                public Class<?> getColumnClass(int column) {
                    switch(column) {
                        case 1:
                            return Slave.State.class;
                        case 2:
                            return String.class;
                        default:
                            return Integer.class;
                    }
                }

                @Override
                public Object getValueAt(int row, int column) {
                    Slave slave = slaves.get(row);
                    switch (column) {
                        case 0:
                            return slave.id;
                        case 1:
                            return slave.state;
                        case 2:
                            return slave.name;
                        case 3:
                            switch(slave.state) {
                                case UNINITIALIZED:
                                case INITIALIZING:
                                case READY:
                                case STOPPED:
                                case FINISHED:
                                    return noRunNumber;
                                default:
                                    return slave.currentRun;
                            }
                        case 4:
                            return slave.numResults;
                        default:
                            return "Update getValueAt method!";
                    }
                }

                @Override
                public boolean isCellEditable(int row, int column) {
                    return false;
                }

            }) {{
                getSelectionModel().addListSelectionListener(new ListSelectionListener() {
                    @Override
                    public void valueChanged(ListSelectionEvent e) {
                        updateSlaveActions();
                    }
                });

                ((DefaultTableModel)getModel()).addTableModelListener(new TableModelListener() {
                    @Override
                    public void tableChanged(TableModelEvent e) {
                        updateSlaveActions();
                        updateMonteActions();
                    }
                });

                setDefaultRenderer(Slave.State.class, new DefaultTableRenderer(new LabelProvider() {
                    @Override
                    protected Icon getValueAsIcon(CellContext context) {
                        return ((Slave.State)context.getValue()).icon;
                    }
                    @Override
                    protected String getValueAsString(CellContext context) {
                        return ((Slave.State)context.getValue()).toString();
                    }
                }));

                // GUI elements must be named if their session state is to be saved.
                setName("slaveTable");
                setColumnControlVisible(true);

                // Provide alternate row coloring.
                setHighlighters(HighlighterFactory.createSimpleStriping());

                // Add mouse support.
                addMouseListener(new MouseAdapter() {
                    public void mousePressed(MouseEvent e) {
                        if (e.getButton() == MouseEvent.BUTTON3) {
                            popupMenu.show(slaveTable, e.getX(), e.getY());
                        }
                    }
                });
            }};

            add(new JScrollPane(slaveTable), BorderLayout.CENTER);

            add(new JXPanel(new BorderLayout()) {{
                add(new JLabel("New Slave: "), BorderLayout.WEST);
                newSlaveTextField = new JXTextField() {{
                    setAction(addSlaveAction);
                }};
                add(newSlaveTextField, BorderLayout.CENTER);
                add(new JXButton(addSlaveAction), BorderLayout.EAST);
            }}, BorderLayout.SOUTH);
            }
        };
    }

    @Override
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = super.createMenuBar();

        menuBar.add(new JMenu("Slaves") {{
            setMnemonic(KeyEvent.VK_S);
            add(new JMenuItem(runSlavesAction));
            add(new JMenuItem(stopSlavesAction));
        }}, 1);

        menuBar.add(new JMenu("Monte Carlo") {{
            setMnemonic(KeyEvent.VK_M);
            add(new JMenuItem(runMonteAction));
            add(new JMenuItem(stopMonteAction));
        }}, 1);

        return menuBar;
    }

    /** main method for this application 
     * @param args command line arguments
     */
    public static void main(String[] args) {
        BaseApplication.launch(MonteMonitorApplication.class, args);
    }

    /** monitors incoming messages from the Variable Server and updates the display table with new information  */
    private class Monitor extends SwingWorker<Void, String[]> {

        int slaveCount = 0;

        /** In a background Worker thread, forever listen for incoming messages from the Variable Server. */
        @Override
        public Void doInBackground() throws IOException {
            while (true) {
                String line = variableServerConnection.get();
                if (line == null) {
                    throw new IOException("Reached end of stream.");
                }
                publish(line.split("\t", -1));
            }
        }

        /**
         * update the display table with the new information. Runs on the Event Dispatch Thread (where Swing components must
         * be modified) in response to calls to <code>publish</code>.
         */
        @Override
        protected void process(List<String[]> dataList) {
            try {
                String[] data = dataList.get(dataList.size() - 1);
                int dataIndex = 1;

                progressBar.setMaximum(Integer.parseInt(data[dataIndex++]));
                progressBar.setValue(Integer.parseInt(data[dataIndex++]));
                progressBar.setString(progressBar.getValue() + " / " + progressBar.getMaximum() + " (" +
                  String.format("%.2f", (double)progressBar.getValue() * 100 / progressBar.getMaximum()) + "%)");

                int numSlaves = Integer.parseInt(data[dataIndex++]);

                while(slaveCount < numSlaves) {
                    variableServerConnection.add("trick_mc.mc.slaves_head[" + slaveCount + "][0].id");
                    variableServerConnection.add("trick_mc.mc.slaves_head[" + slaveCount + "][0].state");
                    variableServerConnection.add("trick_mc.mc.slaves_head[" + slaveCount + "][0].machine_name");
                    variableServerConnection.add("trick_mc.mc.slaves_head[" + slaveCount + "][0].current_run[0].id");
                    variableServerConnection.add("trick_mc.mc.slaves_head[" + slaveCount + "][0].num_results");
                    ++slaveCount;
                }

                while(slaveCount > numSlaves) {
                    variableServerConnection.remove("trick_mc.mc.slaves_head[" + slaveCount + "][0].id");
                    variableServerConnection.remove("trick_mc.mc.slaves_head[" + slaveCount + "][0].state");
                    variableServerConnection.remove("trick_mc.mc.slaves_head[" + slaveCount + "][0].machine_name");
                    variableServerConnection.remove("trick_mc.mc.slaves_head[" + slaveCount + "][0].current_run[0].id");
                    variableServerConnection.remove("trick_mc.mc.slaves_head[" + slaveCount + "][0].num_results");
                    --slaveCount;
                }

                for (Slave slave : slaves) {
                    slave.valid = false;
                }

                while (dataIndex < data.length) {
                    String id = data[dataIndex++];
                    Slave slave = null;
                    for (Slave s : slaves) {
                        if (s.id.equals(id)) {
                            slave = s;
                            slave.valid = true;
                            break;
                        }
                    }
                    if (slave == null) {
                        slave = new Slave(id);
                        addSlave(slave);
                    }
                    switch (Integer.parseInt(data[dataIndex++])) {
                        case 0:
                            slave.state = Slave.State.UNINITIALIZED;
                            break;
                        case 1:
                            slave.state = Slave.State.INITIALIZING;
                            break;
                        case 2:
                            slave.state = Slave.State.READY;
                            break;
                        case 3:
                            slave.state = Slave.State.RUNNING;
                            break;
                        case 4:
                            slave.state = Slave.State.STOPPING;
                            break;
                        case 5:
                            slave.state = Slave.State.STOPPED;
                            break;
                        case 6:
                            slave.state = Slave.State.FINISHED;
                            break;
                        case 7:
                            slave.state = Slave.State.UNRESPONSIVE_RUNNING;
                            break;
                        case 8:
                            slave.state = Slave.State.UNRESPONSIVE_STOPPING;
                            break;
                        case 9:
                            slave.state = Slave.State.DISCONNECTED;
                            break;
                        default:
                            slave.state = Slave.State.UNKNOWN;
                            break;
                    }
                    slave.name = data[dataIndex++];
                    if (data[dataIndex++].equals("BAD_REF")) {
                        slave.currentRun = -1;
                    }
                    else {
                        slave.currentRun = Integer.valueOf(data[dataIndex - 1]);
                    }
                    slave.numResults = Integer.valueOf(data[dataIndex++]);
                }

                for (int i = slaves.size() - 1; i >= 0; --i) {
                    if (!slaves.get(i).valid) {
                        removeSlave(i);
                    }
                }

                if (!slaves.isEmpty()) {
                    ((DefaultTableModel)slaveTable.getModel()).fireTableRowsUpdated(0, slaves.size() - 1);
                }
            }
            catch (Exception e) {}
        }

        /** Executed on the Event Dispatch Thread after the doInBackground method is finished. */
        @Override
        protected void done() {
            try {
                // Check for any exception that occurred during execution by calling get().
                get();
            }
            catch (Exception exception) {
                disconnect(exception);
            }
        }

    }

}
