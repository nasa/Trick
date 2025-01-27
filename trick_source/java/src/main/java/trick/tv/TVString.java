package trick.tv;

import java.awt.Component;

import javax.swing.DefaultCellEditor;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;

import org.jdesktop.swingx.JXTextField;

import trick.common.utils.vs.VSString;

@XmlRootElement
public class TVString extends VSString implements TrickViewFluent<TVString.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        String {
            public String format(String value) {
                return value;
            }
        };

        public abstract String format(String value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new JXTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVString tvString;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvString = (TVString)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVString getCellEditorValue() {
            return new TVString(tvString.getFormat().format(
              super.getCellEditorValue().toString()));
        }
    };

    Format format = Format.String;

    public TVString() {}

    public TVString(String value) {
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
