package trick.tv;

import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;

import trick.common.utils.vs.VariableServerFluent;

/**
 * designates that a class is compatible with Trick View
 *
 * @author Derek Bankieris
 */
public interface TrickViewFluent<T extends Enum<T>> extends VariableServerFluent {

    /**
     * returns an enumeration of the displayable formats
     *
     * @return an enumeration of valid formats
     */
    public Class<T> getFormatClass();

    /**
     * sets the displayed format
     *
     * @param format the format to display in
     */
    public void setFormat(T format);

    /**
     * gets the format
     *
     * @return the format to display in
     */
    public T getFormat();

    /**
     * returns a renderer for use by tables
     *
     * @return a renderer for the value
     */
    public TableCellRenderer getCellRenderer();

    /**
     * returns an editor for use by tables. The editor's <code>getCellEditorValue()</code> method must
     * return an instance of the class implementing this interface.
     *
     * @return an editor for the value
     */
    public TableCellEditor getCellEditor();

    /**
     * sets visible(shown) precision of the <code>Variable</code>'s value
     * only on TV Table, does not affect the value in simulation.
     *
     * @param precision the value after dot(.) to be set
     */
    public void setPrecision(String precision);

    /**
     * returns the precision
     *
     * @return the precision
     */
    public String getPrecision();
}
