package trick.tv;

import java.awt.Component;
import java.math.BigInteger;

import javax.swing.DefaultCellEditor;
import javax.swing.JOptionPane;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlType;

import org.jdesktop.swingx.JXTextField;

import trick.common.utils.vs.VSByte;

@XmlRootElement
public class TVByte extends VSByte implements TrickViewFluent<TVByte.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Character {
            public String format(byte value, boolean unsigned) {
                return java.lang.Character.toString((char)value);
            }

            public byte parse(String value) {
                if (value.length() != 1) {
                    throw new NumberFormatException("This format requires exactly one character.");
                }
                return (byte)value.charAt(0);
            }
        },

        Binary {
            public String format(byte value, boolean unsigned) {
                String result = Integer.toBinaryString(value & 0xFF);
                return "00000000".substring(result.length()) + result;
            }

            public byte parse(String value) {
                return new BigInteger(value, 2).byteValue();
            }
        },

        Decimal {
            public String format(byte value, boolean unsigned) {
                if (unsigned && value < 0) {
                    return Integer.toString(value + 256);
                }
                else {
                    return Byte.toString(value);
                }
            }

            public byte parse(String value) {
                return new BigInteger(value, 10).byteValue();
            }
        },

        Octal {
            public String format(byte value, boolean unsigned) {
                return Integer.toOctalString(value & 0xFF);
            }

            public byte parse(String value) {
                return new BigInteger(value, 8).byteValue();
            }
        },

        Hexadecimal {
            public String format(byte value, boolean unsigned) {
                return Integer.toHexString(value & 0xFF);
            }

            public byte parse(String value) {
                return new BigInteger(value, 16).byteValue();
            }
        };

        public abstract String format(byte value, boolean unsigned);
        public abstract byte parse(String value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new JXTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVByte tvByte;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvByte = (TVByte)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVByte getCellEditorValue() {
            try {
                String value = super.getCellEditorValue().toString();
                Format format = tvByte.getFormat();
                if (format != Format.Character) {
                    value = value.trim();
                }
                return new TVByte(format.parse(value));
            }
            catch (NumberFormatException numberFormatException) {
                JOptionPane.showMessageDialog(getComponent(), numberFormatException.toString(),
                  "Invalid Input", JOptionPane.ERROR_MESSAGE);
            }
            return tvByte;
        }
    };

    public final boolean unsigned;

    Format format = Format.Decimal;

    public TVByte() {
        this(false);
    }

    public TVByte(boolean unsigned) {
        this.unsigned = unsigned;
    }

    public TVByte(byte value) {
        super(value);
        unsigned = false;
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
    public String toVariableServer() {
        return Format.Decimal.format(getValue(), unsigned);
    }

    @Override
    public String toString() {
        return format.format(getValue(), unsigned);
    }

}
