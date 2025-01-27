package trick.tv;

import java.awt.Component;

import javax.swing.DefaultCellEditor;
import javax.swing.JOptionPane;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;

import org.jdesktop.swingx.JXTextField;

import trick.common.utils.vs.VSDouble;

@XmlRootElement
public class TVDouble extends VSDouble implements TrickViewFluent<TVDouble.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Decimal {
            public String format(double value) {
                return Double.toString(value);
            }
        },

        Hexadecimal {
            public String format(double value) {
                return Double.toHexString(value);
            }
        };

        public abstract String format(double value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new JXTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVDouble tvDouble;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvDouble = (TVDouble)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVDouble getCellEditorValue() {
            try {
                return new TVDouble(Double.parseDouble(
                  super.getCellEditorValue().toString().trim()));
            }
            catch (NumberFormatException numberFormatException) {
                JOptionPane.showMessageDialog(getComponent(), numberFormatException.toString(),
                  "Invalid Input", JOptionPane.ERROR_MESSAGE);
            }
            return tvDouble;
        }
    };

    Format format = Format.Decimal;

    public TVDouble() {}

    public TVDouble(double value) {
        super(value);
    }

    public Class<Format> getFormatClass() {
        return Format.class;
    }

    public Format getFormat() {
        return format;
    }

    public void setFormat(Format format) {
        this.format = format;
    }

    public TableCellRenderer getCellRenderer() {
        return tableCellRenderer;
    }

    public TableCellEditor getCellEditor() {
        return tableCellEditor;
    }

    @Override
    public void setPrecision(String precision) {
        this.precision = precision;
    }

    @Override
    public String getPrecision() {
        return precision;
    }

    @Override
    public String toString() {
        return format.format(getValue());
    }

}
