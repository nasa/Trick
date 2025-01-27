package trick.tv;

import java.awt.*;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.EnumSet;
import java.util.HashSet;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;


import org.jdesktop.swingx.JXTable;
import org.jdesktop.swingx.decorator.ColorHighlighter;
import org.jdesktop.swingx.decorator.HighlighterFactory;
import org.jdesktop.swingx.decorator.PatternPredicate;

import trick.common.utils.VariableServerConnection;
import trick.common.utils.vs.Variable;
import trick.common.utils.vs.VariableServerFluent;

/**
 * displays a table of variables
 *
 * @author Derek Bankieris
 */
public class VariableTable extends JXTable {

    /** position at which to insert new variables */
    public enum Position {Top, Before, After, Bottom};

    /** variables being tracked */
    ArrayList<Variable<? extends TrickViewFluent>> variables = new ArrayList<>();

    /** variables being tracked */
    HashSet<Variable<? extends TrickViewFluent>> variablesHashSet = new HashSet<>(variables.size());

    /** variable server to which to send values changes */
    VariableServerConnection variableServerConnection;

    /** the table model */
    VariableTableModel model;

    /** the precision column visibility state */
    boolean maxPrecisionVisible;

    /** the name change action */
    Action nameChangeAction = new AbstractAction() {
        public void actionPerformed(ActionEvent actionEvent) {}
    };

    /**
     * constructor
     */
    public VariableTable() {
        this(null);
    }

    /**
     * constructor
     *
     * @param variableServerConnection the variable server to which to send value changes
     */
    public VariableTable(VariableServerConnection variableServerConnection) {

        this.variableServerConnection = variableServerConnection;

        // Set the model.
        model = new VariableTableModel();
        setModel(model);

        // Enable column controls.
        setColumnControlVisible(true);

        // Highlight invalid references in red.
        setHighlighters(
          HighlighterFactory.createSimpleStriping(),
          new ColorHighlighter(new PatternPredicate("<Invalid Reference>",
            getColumnModel().getColumnIndex("Value")),
            null, Color.red, null, Color.red));

        // Add keyboard support.
        getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0), "VariableTable.delete");

        // Add drag and drop support.
        setFillsViewportHeight(true);
        setDragEnabled(true);
        setDropMode(DropMode.INSERT);
        setTransferHandler(new TransferHandler() {

            DataFlavor variableDataFlavor = new DataFlavor(Variable.class,
              "Variable<? extends TrickViewFluent>");

            @Override
            public int getSourceActions(JComponent component) {
                return COPY_OR_MOVE;
            }

            @Override
            protected Transferable createTransferable(JComponent component) {
                return new VariableSelection(getSelectedRows());
            }

            @Override
            public boolean canImport(TransferSupport transferSupport) {
                return transferSupport.isDrop() &&
                  transferSupport.isDataFlavorSupported(variableDataFlavor) &&
                  ((JXTable.DropLocation)transferSupport.getDropLocation()).isInsertRow();
            }

            @Override
            public boolean importData(TransferSupport transferSupport) {

                if (!canImport(transferSupport)) {
                    return false;
                }

                try {
                    int[] indices = (int[])transferSupport.getTransferable().
                      getTransferData(variableDataFlavor);

                    // Convert from view to model indices.
                    for (int i = 0; i < indices.length; i++) {
                        indices[i] = convertRowIndexToModel(indices[i]);
                    }
                    Arrays.sort(indices);


                    // If dropping after the end of the table, the drop index will be equal to
                    // the row count, which is not a legally convertable index.
                    int dropIndex = ((JXTable.DropLocation)transferSupport.getDropLocation()).getRow();
                    if (dropIndex < model.getRowCount()) {
                        dropIndex = convertRowIndexToModel(dropIndex);
                    }

                    ArrayList<Variable<? extends TrickViewFluent>> movingVariables =
                      new ArrayList<Variable<? extends TrickViewFluent>>();

                    // Remove in reverse order to avoid index corruption.
                    for (int i = indices.length - 1; i >= 0;  --i) {
                        movingVariables.add(variables.remove(indices[i]));
                    }

                    // The removed variables have reduced the table's row count. The index at which
                    // we will begin re-inserting them must be decremented for each removed variable
                    // that was positioned above the drop index.
                    int index = dropIndex;
                    for (int i : indices) {
                        if (i < dropIndex) {
                            --index;
                        }
                        else {
                            break;
                        }
                    }

                    // The variables were removed in reverse order, so re-insert them
                    // with a constant index.
                    for (Variable<? extends TrickViewFluent> variable : movingVariables) {
                        variables.add(index, variable);
                    }

                    // Update all affected rows.
                    model.fireTableRowsUpdated(Math.min(dropIndex, indices[0]),
                      Math.max(Math.min(dropIndex, model.getRowCount() - 1), indices[indices.length - 1]));

                    // Revert to unsorted mode.
                    getRowSorter().setSortKeys(null);

                    // Select the re-inserted variables.
                    setRowSelectionInterval(convertRowIndexToView(index),
                      convertRowIndexToView(index + indices.length - 1));
                }
                catch (Exception exception) {
                    exception.printStackTrace();
                }

                return true;
            }

            /**
             * provides for dragging of rows
             */
            class VariableSelection implements Transferable {

                /** indices of the selected rows */
                int[] indices;

                /** textual representation of the selected rows */
                String string = "";

                /**
                 * constructor
                 *
                 * @param indices indices of the selected rows
                 */
                public VariableSelection(int[] indices) {
                    this.indices = indices;
                    for (int row : indices) {
                        for (int column = 0; column < getColumnCount(); ++column) {
                            string += getStringAt(row, column) + "\t";
                        }
                        string = string.trim() + "\n";
                    }
                }

                @Override
                public Object getTransferData(DataFlavor dataFlavor)
                  throws UnsupportedFlavorException {
                    if (!isDataFlavorSupported(dataFlavor)) {
                        throw new UnsupportedFlavorException(dataFlavor);
                    }

                    if (dataFlavor == variableDataFlavor) {
                        return indices;
                    }
                    return string;
                }

                @Override
                public DataFlavor[] getTransferDataFlavors() {
                    return new DataFlavor[] {variableDataFlavor, DataFlavor.stringFlavor};
                }

                @Override
                public boolean isDataFlavorSupported(DataFlavor dataFlavor) {
                    return dataFlavor == variableDataFlavor ||
                           dataFlavor == DataFlavor.stringFlavor;
                }

            }

        });
    }

    /**
     * sets the variable server connection
     *
     * @param variableServerConnection the variable server connection to use
     */
    public void setVariableServerConnection(VariableServerConnection variableServerConnection) {
        this.variableServerConnection = variableServerConnection;
    }

    /**
     * sets the precision column visibility and updates the (Variable Table) model
     *
     * @param maxPrecisionVisible
     */
    public void setMaxPrecisionVisible(boolean maxPrecisionVisible) {
        this.maxPrecisionVisible = maxPrecisionVisible;
        this.model.setPrecisionColumnVisibility(maxPrecisionVisible);
        this.model.fireTableStructureChanged();
    }

    /**
     * sets the delete action
     *
     * @param action the action to take on deletion
     */
    public void setDeleteAction(Action action) {
        getActionMap().put("VariableTable.delete", action);
    }

    /**
     * sets the name change action
     *
     * @param action the action to take on a name change
     */
    public void setNameChangeAction(Action action) {
        nameChangeAction = action;
    }

    /**
     * adds a variable
     *
     * @param variable the variable to add
     */
    public void add(Variable<? extends TrickViewFluent> variable) {
        add(variable, Position.Bottom);
    }

    /**
     * adds the <code>variable</code> at <code>position</code>
     *
     * @param variable the variable to add
     * @param position the position at which to add the variable
     */
    public void add(Variable<? extends TrickViewFluent> variable, Position position) {
        if (!variablesHashSet.contains(variable)) {
            int index = 0;
            int selectedRow = getSelectedRow();

            switch (position) {
                case Top:
                    break;
                case Before:
                    if (selectedRow != -1) {
                        index = convertRowIndexToModel(selectedRow);
                    }
                    break;
                case After:
                    if (selectedRow == -1) {
                        index = model.getRowCount();
                    }
                    else {
                        index = convertRowIndexToModel(selectedRow) + 1;
                    }
                    break;
                case Bottom:
                    index = model.getRowCount();
                    break;
            }

            variables.add(index, variable);
            variablesHashSet.add(variable);
            model.fireTableRowsInserted(index, index);
        }
    }

    /**
     * removes the variable at index <code>i</code>
     *
     * @param i the index
     *
     * @return the removed variable, or <code>null</code> if none exists
     */
    public Variable<? extends TrickViewFluent> removeVariable(int i) {
        variablesHashSet.remove(variables.get(i));
        Variable<? extends TrickViewFluent> variable = variables.remove(i);
        model.fireTableRowsDeleted(i, i);
        return variable;
    }

    /**
     * return the selected variables
     *
     * @return the selected variables
     */
    public ArrayList<Variable<? extends TrickViewFluent>> getSelectedVariables() {
        ArrayList<Variable<? extends TrickViewFluent>> selectedVariables = new ArrayList<>();

        int[] rowIndices = getSelectedRows();
        for (int i = 0; i < rowIndices.length; ++i) {
            rowIndices[i] = convertRowIndexToModel(rowIndices[i]);
        }

        for (int i : rowIndices) {
            selectedVariables.add(variables.get(i));
        }

        return selectedVariables;
    }

    /**
     * removes all variables
     */
    public void clear() {
        int rowCount = model.getRowCount();
        if (rowCount > 0) {
            variablesHashSet.clear();
            variables.clear();
            model.fireTableRowsDeleted(0, rowCount - 1);
        }
    }

    /**
     * updates all rows
     */
    public void update() {
        int rowCount = model.getRowCount();
        if (rowCount > 0) {
            model.fireTableRowsUpdated(0, rowCount - 1);
        }
    }

    /**
     * returns all variables in the table
     *
     * @return all variables in the table
     */
    public ArrayList<Variable<? extends TrickViewFluent>> getVariables() {
        return variables;
    }

    @Override
    public TableCellRenderer getCellRenderer(int row, int column) {
        if (convertColumnIndexToModel(column) == 1 && !variables.isEmpty()) {
            row = convertRowIndexToModel(row);
            if (row < variables.size()) {
                Variable<? extends TrickViewFluent> variable = variables.get(row);
                if (variable.getState() == Variable.State.Valid) {
                    if (variable.getValue().getPrecision() != null
                            && !variable.getValue().getPrecision().contains("--")
                            && isFractional(variable)
                            && this.maxPrecisionVisible) {
                        // Sets rounded value of variable by its precision
                        BigDecimal bd = new BigDecimal(variable.getValue().toString());
                        bd = bd.setScale(Integer.parseInt(variable.getValue().getPrecision().toString()), RoundingMode.HALF_UP);
                        variable.setValue(bd.toString());
                    }
                    return variable.getValue().getCellRenderer();
                }
            }
        }
        return getDefaultRenderer(String.class);
    }

    @Override
    @SuppressWarnings("unchecked")
    public TableCellEditor getCellEditor(int row, int column) {
        final Variable<? extends TrickViewFluent> variable = variables.get(convertRowIndexToModel(row));
        final TrickViewFluent value = variable.getValue();
        switch(convertColumnIndexToModel(column)) {
            default:
                return getDefaultEditor(String.class);
            case 1:
                return value.getCellEditor();
            case 2:
                return getDefaultEditor(String.class);
            case 3:
                return new DefaultCellEditor(new JComboBox(EnumSet.allOf(value.getFormatClass()).toArray()) {{
                    setSelectedItem(value.getFormat());
                }});
            case 4:
                String[] precisionArr;
                if (isFractional(variable)) {
                    precisionArr = java.util.stream.IntStream.rangeClosed(1, 15).mapToObj(Integer::toString).toArray(String[]::new);
                } else {
                    precisionArr = new String[]{"--"};
                }
                return new DefaultCellEditor(new JComboBox(precisionArr) {{
                    setSelectedItem(variable.getValue().getPrecision());
                }});
        }
    }

    /**
     * This function checks if variable has valid precision and it is decimal that type of TVDouble or TVFloat
     * Since only double and float numbers are roundable
     *
     * @param variable
     * @return
     */
    private boolean isFractional(Variable<? extends TrickViewFluent> variable) {
        return  variable.getValue().getFormat().toString().contains("Decimal")
                && (variable.getValue().getFormatClass().toString().contains("TVDouble")
                || variable.getValue().getFormatClass().toString().contains("TVFloat"));
    }


    /**
     * sets the units of all <code>variables</code> to the result of <code>units.toString()</code>.
     * Illegal conversions are silently ignored.
     *
     * @param variables the variables whose units to set
     * @param units the new units
     */

    private void setUnits(ArrayList<? extends Variable> variables, Object units) {
        for (Variable variable : variables) {
                try {
                    variable.sendUnitsToVariableServer(units.toString(),
                            variableServerConnection);
                }
                catch (IOException ioException) {
                    System.err.println("Failed to set variable \"" +
                            variable + "\" units to \"" + units + "\"");
                    ioException.printStackTrace(System.err);
                }
        }
    }


    /**
     * the model for this table
     */
    class VariableTableModel extends AbstractTableModel {
        private boolean precisionColumnVisibility = false;

        @Override
        public int getRowCount() {
            return variables.size();
        }

        @Override
        public int getColumnCount() {
            return getPrecisionColumnVisibility() ? 5 : 4;
        }

        @Override
        public String getColumnName(int columnIndex) {
            switch (columnIndex) {
                case 0:
                    return "Variable";
                case 1:
                    return "Value";
                case 2:
                    return "Units";
                case 4:
                    return "Precision";
                default:
                    return "Format";
            }
        }

        @Override
        public boolean isCellEditable(int rowIndex, int columnIndex) {
            return columnIndex == 0 || variables.get(rowIndex).getState() == Variable.State.Valid;
        }

        @Override
        public Object getValueAt(int rowIndex, int columnIndex) {
            Variable<? extends TrickViewFluent> variable = variables.get(rowIndex);
            switch (columnIndex) {
                case 0:
                    return variable.name;
                case 1:
                    switch (variable.getState()) {
                        case Invalid:
                            return "<Invalid Reference>";
                        case Valid:
                            return variable.getValue();
                        default:
                            return "<Unknown>";
                    }
                case 2:
                    return variable.getUnits();
                case 4:
                    return variable.getValue().getPrecision();
                default:
                    return variable.getValue().getFormat();
            }
        }

        @Override
        public void setValueAt(Object value, int rowIndex, int columnIndex) {
            switch (columnIndex) {
                case 0:
                    nameChangeAction.actionPerformed(new ActionEvent(this, 0, value.toString()));
                    break;
                case 1:
                    setValues(getSelectedVariables(), value);
                    break;
                case 2:
                    setUnits(getSelectedVariables(), value);
                    break;
                case 3:
                    setFormat(getSelectedVariables(), value);
                    for (int row : getSelectedRows()) {
                        int index = convertRowIndexToModel(row);
                        model.fireTableRowsUpdated(index, index);
                    }
                    break;
                case 4:
                    setPrecision(getSelectedVariables(), value);
                    break;
            }
        }

        private void setValues(ArrayList<? extends Variable<? extends VariableServerFluent>> variables,
                               Object value) {
            for (Variable<? extends VariableServerFluent> variable : variables) {
                setValue(variable, value);
            }
        }

        @SuppressWarnings("unchecked")
        private <T extends VariableServerFluent> void setValue(Variable<T> variable, Object value) {
            try {
                variable.sendValueToVariableServer((T) value, variableServerConnection);
            } catch (IOException ioException) {
                System.err.println("Failed to set variable \"" + variable + "\" to \"" + value + "\"");
                ioException.printStackTrace(System.err);
            }
        }

        @SuppressWarnings("unchecked")
        private void setFormat(ArrayList<Variable<? extends TrickViewFluent>> variables, Object format) {
            for (Variable<? extends TrickViewFluent> variable : variables) {
                TrickViewFluent value = variable.getValue();
                try {
                    value.setFormat(Enum.valueOf(value.getFormatClass(), format.toString()));
                } catch (IllegalArgumentException illegalArgumentException) {
                    // The format is not applicable to the current value; ignore it.
                }
            }
        }

        /**
         * sets visible(shown) precision of the <code>Variable</code>'s value
         * only on TV Table, does not affect the value in simulation.
         *
         * @param variables
         * @param precision the value after dot(.) to be set
         */
        private void setPrecision(ArrayList<Variable<? extends TrickViewFluent>> variables, Object precision) {
            try {
                for (Variable<? extends TrickViewFluent> variable : variables) {
                    variable.getValue().setPrecision(precision.toString());
                }
            } catch (NullPointerException nullPointerException) {
                System.err.println("Failed to set precision value \"" + precision + "\"");
                nullPointerException.printStackTrace(System.err);
            }
        }

        public void setPrecisionColumnVisibility(boolean precisionColumnVisibility) {
            this.precisionColumnVisibility = precisionColumnVisibility;
        }

        public boolean getPrecisionColumnVisibility() {
            return this.precisionColumnVisibility;
        }
    }

}
