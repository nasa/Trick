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

import trick.common.utils.vs.VSFloat;

@XmlRootElement
public class TVFloat extends VSFloat implements TrickViewFluent<TVFloat.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Decimal {
            public String format(float value) {
                return Float.toString(value);
            }
        },

        Hexadecimal {
            public String format(float value) {
                return Float.toHexString(value);
            }
        };

        public abstract String format(float value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new JXTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVFloat tvFloat;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvFloat = (TVFloat)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVFloat getCellEditorValue() {
            try {
                return new TVFloat(Float.parseFloat(
                  super.getCellEditorValue().toString().trim()));
            }
            catch (NumberFormatException numberFormatException) {
                JOptionPane.showMessageDialog(getComponent(), numberFormatException.toString(),
                  "Invalid Input", JOptionPane.ERROR_MESSAGE);
            }
            return tvFloat;
        }
    };

    Format format = Format.Decimal;

    public TVFloat() {}

    public TVFloat(float value) {
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
