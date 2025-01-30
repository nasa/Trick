package trick.jobperf;

import java.awt.*;
import javax.swing.*;

/**
* Class TraceViewOutputToolBar displays information output from a
* TraceViewCanvas.
*/
class TraceViewOutputToolBar extends JToolBar {
    private JTextField IDField;
    private JTextField nameField;
    private JTextField classField;
    private JTextField startField;
    private JTextField stopField;
    // private JTextField frameNumberField;
    private JTextField durationField;

    /**
     * Constructor
     */
    public TraceViewOutputToolBar () {

        add( new JLabel(" Job ID: "));
        IDField = new JTextField(5);
        IDField.setEditable(false);
        IDField.setText( "");
        add(IDField);

        add( new JLabel(" Name: "));
        nameField = new JTextField(25);
        nameField.setEditable(false);
        nameField.setText( "");
        add(nameField);

        add( new JLabel(" Class: "));
        classField = new JTextField(12);
        classField.setEditable(false);
        classField.setText( "");
        add(classField);

        add( new JLabel(" Start: "));
        startField = new JTextField(6);
        startField.setEditable(false);
        startField.setText( "");
        add(startField);

        add( new JLabel(" Stop: "));
        stopField = new JTextField(6);
        stopField.setEditable(false);
        stopField.setText( "");
        add(stopField);

        add( new JLabel(" Duration: "));
        durationField = new JTextField(6);
        durationField.setEditable(false);
        durationField.setText( "");
        add(durationField);
    }

    /**
     * @param id job identifier to display.
     */
    public void setJobID(String id) {
        IDField.setText( id );
    }

    /**
     * @param id job identifier to display.
     */
    public void setJobName(String name) {
        nameField.setText(name);
    }

    /**
     * @param id job class to display.
     */
    public void setJobClass(String name) {
        classField.setText(name);
    }

    /**
     * @param time to be displayed in the job start field.
     */
    public void setJobTimes(Double start_time, Double stop_time) {
        startField.setText( String.format("%8.4f", start_time) );
        stopField.setText( String.format("%8.4f", stop_time) );
        Double duration = stop_time - start_time;
        durationField.setText( String.format("%8.4f", duration) );
    }

    /**
     * Clear the startField and stopField.
     */
    public void clearJobFields() {
        nameField.setText("");
        classField.setText("");
        startField.setText("");
        stopField.setText("");
        durationField.setText("");
        IDField.setText("");
    }

}
