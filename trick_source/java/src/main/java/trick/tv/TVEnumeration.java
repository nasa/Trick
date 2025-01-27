package trick.tv;

import java.awt.Component;
import java.util.HashMap;

import javax.swing.DefaultCellEditor;
import javax.swing.JComboBox;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlIDREF;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;

import trick.common.utils.vs.VSValue;
import trick.sie.utils.SieEnumeration;

@XmlRootElement
public class TVEnumeration extends VSValue<String> implements TrickViewFluent<TVEnumeration.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Enumeration {
            public String format(String value, SieEnumeration enumeration) {
                for (String key : enumeration.pairs.keySet()) {
                    if (enumeration.pairs.get(key).equals(value)) {
                        return key;
                    }
                }
                return null;
            }
        };

        public abstract String format(String value, SieEnumeration enumeration);

    }

    @XmlElement(required = true)
    @XmlIDREF
    public final SieEnumeration enumeration;

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();
    
    static JComboBox comboBox = new JComboBox();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(comboBox) {

        TVEnumeration tvEnumeration;

        @Override
        @SuppressWarnings("unchecked")
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvEnumeration = (TVEnumeration)value;
            HashMap<String, String> hashSet = tvEnumeration.getEnumeration().pairs;
            comboBox.removeAllItems();
            for (String key : hashSet.keySet()) {
                comboBox.addItem(key);
                if (hashSet.get(key).equals(tvEnumeration.getValue())) {
                    comboBox.setSelectedItem(key);
                }
            }
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVEnumeration getCellEditorValue() {
            return new TVEnumeration(tvEnumeration.getEnumeration(),
              tvEnumeration.enumeration.pairs.get(super.getCellEditorValue().toString()));
        }
    };

    Format format = Format.Enumeration;

    private TVEnumeration() {
        enumeration = null;
    }

    public TVEnumeration(SieEnumeration enumeration) {
        this(enumeration, enumeration.pairs.values().iterator().next());
    }

    public TVEnumeration(SieEnumeration enumeration, String value) {
        this.enumeration = enumeration;
        setValue(value);
    }

    public void fromVariableServer(String string) {
        setValue(string);
    }

    public String toVariableServer() {
        return getValue();
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

    public SieEnumeration getEnumeration() {
        return enumeration;
    }

    @Override
    public String toString() {
        return format.format(getValue(), enumeration);
    }

}
