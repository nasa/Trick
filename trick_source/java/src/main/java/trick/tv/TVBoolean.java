package trick.tv;

import java.awt.Component;

import javax.swing.DefaultCellEditor;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.UIManager;
import javax.swing.border.EmptyBorder;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;

import trick.common.utils.vs.VSBoolean;

@XmlRootElement
public class TVBoolean extends VSBoolean implements TrickViewFluent<TVBoolean.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Boolean {
            public String format(boolean value) {
                return value ? "1" : "0";
            }
        };

        public abstract String format(boolean value);

    }

    static TableCellRenderer tableCellRenderer = new BooleanRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new JCheckBox() {
            {
            setHorizontalAlignment(CENTER);
            addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent actionEvent) {
                    tableCellEditor.stopCellEditing();
                }
            });
            }
        }) {{
            final JCheckBox checkBox = (JCheckBox)editorComponent;
            checkBox.removeActionListener(delegate);
            delegate = new EditorDelegate() {
                @Override
                public void setValue(Object value) {
                    checkBox.setSelected(((TVBoolean)value).getValue());
                }
                @Override
                public TVBoolean getCellEditorValue() {
                    return new TVBoolean(((JCheckBox)editorComponent).isSelected());
                }
            };
    }};

    Format format = Format.Boolean;

    public TVBoolean() {
        this(false);
    }

    public TVBoolean(boolean value) {
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

    static class BooleanRenderer extends JCheckBox implements TableCellRenderer {

        static final EmptyBorder noFocusBorder = new EmptyBorder(1, 1, 1, 1);

	public BooleanRenderer() {
	    super();
	    setHorizontalAlignment(JLabel.CENTER);
            setBorderPainted(true);
            setOpaque(true);
	}

        public Component getTableCellRendererComponent(JTable table, Object value,
          boolean isSelected, boolean hasFocus, int row, int column) {
            if (value instanceof TVBoolean) {
                setSelected(((TVBoolean)value).getValue());
            }

	    if (isSelected) {
	        setForeground(table.getSelectionForeground());
	        setBackground(table.getSelectionBackground());
	    }
	    else {
	        setForeground(table.getForeground());
	        setBackground(table.getBackground());
	    }

            if (hasFocus) {
                setBorder(UIManager.getBorder("Table.focusCellHighlightBorder"));
            } else {
                setBorder(noFocusBorder);
            }

            return this;
        }
    }

}
