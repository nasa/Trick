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

import trick.common.utils.vs.VSShort;

@XmlRootElement
public class TVShort extends VSShort implements TrickViewFluent<TVShort.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Binary {
            public String format(short value, boolean unsigned) {
                String result = Integer.toBinaryString(value & 0xFFFF);
                return "0000000000000000".substring(result.length()) + result;
            }

            public short parse(String value) {
                return new BigInteger(value, 2).shortValue();
            }
        },

        Decimal {
            public String format(short value, boolean unsigned) {
                if (unsigned && value < 0) {
                    return Integer.toString(value + 65536);
                }
                else {
                    return Short.toString(value);
                }
            }

            public short parse(String value) {
                return new BigInteger(value, 10).shortValue();
            }
        },

        Octal {
            public String format(short value, boolean unsigned) {
                return Integer.toOctalString(value & 0xFFFF);
            }

            public short parse(String value) {
                return new BigInteger(value, 8).shortValue();
            }
        },

        Hexadecimal {
            public String format(short value, boolean unsigned) {
                return Integer.toHexString(value & 0xFFFF);
            }

            public short parse(String value) {
                return new BigInteger(value, 16).shortValue();
            }
        },

        BinaryReverseBytes {
            public String format(short value, boolean unsigned) {
                return Integer.toBinaryString(Short.reverseBytes(value) & 0xFFFF);
            }

            public short parse(String value) {
                return Short.reverseBytes(new BigInteger(value, 2).shortValue());
            }
        },

        DecimalReverseBytes {
            public String format(short value, boolean unsigned) {
                value = Short.reverseBytes(value);
                if (unsigned && value < 0) {
                    return Integer.toString(value + 65536);
                }
                else {
                    return Short.toString(value);
                }
            }

            public short parse(String value) {
                return Short.reverseBytes(new BigInteger(value, 10).shortValue());
            }
        },

        OctalReverseBytes {
            public String format(short value, boolean unsigned) {
                return Integer.toOctalString(Short.reverseBytes(value) & 0xFFFF);
            }

            public short parse(String value) {
                return Short.reverseBytes(new BigInteger(value, 8).shortValue());
            }
        },

        HexadecimalReverseBytes {
            public String format(short value, boolean unsigned) {
                return Integer.toHexString(Short.reverseBytes(value) & 0xFFFF);
            }

            public short parse(String value) {
                return Short.reverseBytes(new BigInteger(value, 16).shortValue());
            }
        };

        public abstract String format(short value, boolean unsigned);
        public abstract short parse(String value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new javax.swing.JTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVShort tvShort;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvShort = (TVShort)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVShort getCellEditorValue() {
            try {
                return new TVShort(tvShort.getFormat().parse(
                  super.getCellEditorValue().toString().trim()));
            }
            catch (NumberFormatException numberFormatException) {
                JOptionPane.showMessageDialog(getComponent(), numberFormatException.toString(),
                  "Invalid Input", JOptionPane.ERROR_MESSAGE);
            }
            return tvShort;
        }
    };

    public final boolean unsigned;

    Format format = Format.Decimal;

    public TVShort() {
        this(false);
    }

    public TVShort(boolean unsigned) {
        this.unsigned = unsigned;
    }

    public TVShort(short value) {
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
