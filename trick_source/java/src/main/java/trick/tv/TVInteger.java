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

import trick.common.utils.vs.VSInteger;

@XmlRootElement
public class TVInteger extends VSInteger implements TrickViewFluent<TVInteger.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Binary {
            public String format(int value, boolean unsigned) {
                String result = Integer.toBinaryString(value);
                return "00000000000000000000000000000000".substring(result.length()) + result;
            }

            public int parse(String value) {
                return new BigInteger(value, 2).intValue();
            }
        },

        Decimal {
            public String format(int value, boolean unsigned) {
                if (unsigned && value < 0) {
                    return Long.toString(value + 4294967296L);
                }
                else {
                    return Integer.toString(value);
                }
            }

            public int parse(String value) {
                return new BigInteger(value, 10).intValue();
            }
        },

        Octal {
            public String format(int value, boolean unsigned) {
                return Integer.toOctalString(value);
            }

            public int parse(String value) {
                return new BigInteger(value, 8).intValue();
            }
        },

        Hexadecimal {
            public String format(int value, boolean unsigned) {
                return Integer.toHexString(value);
            }

            public int parse(String value) {
                return new BigInteger(value, 16).intValue();
            }
        },

        BinaryReverseBytes {
            public String format(int value, boolean unsigned) {
                return Integer.toBinaryString(Integer.reverseBytes(value));
            }

            public int parse(String value) {
                return Integer.reverseBytes(new BigInteger(value, 2).intValue());
            }
        },

        DecimalReverseBytes {
            public String format(int value, boolean unsigned) {
                value = Integer.reverseBytes(value);
                if (unsigned && value < 0) {
                    return Long.toString(value + 4294967296L);
                }
                else {
                    return Integer.toString(value);
                }
            }

            public int parse(String value) {
                return Integer.reverseBytes(new BigInteger(value, 10).intValue());
            }
        },

        OctalReverseBytes {
            public String format(int value, boolean unsigned) {
                return Integer.toOctalString(Integer.reverseBytes(value));
            }

            public int parse(String value) {
                return Integer.reverseBytes(new BigInteger(value, 8).intValue());
            }
        },

        HexadecimalReverseBytes {
            public String format(int value, boolean unsigned) {
                return Integer.toHexString(Integer.reverseBytes(value));
            }

            public int parse(String value) {
                return Integer.reverseBytes(new BigInteger(value, 16).intValue());
            }
        };

        public abstract String format(int value, boolean unsigned);
        public abstract int parse(String value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new javax.swing.JTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVInteger tvInteger;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvInteger = (TVInteger)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVInteger getCellEditorValue() {
            try {
                return new TVInteger(tvInteger.getFormat().parse(
                  super.getCellEditorValue().toString().trim()));
            }
            catch (NumberFormatException numberFormatException) {
                JOptionPane.showMessageDialog(getComponent(), numberFormatException.toString(),
                  "Invalid Input", JOptionPane.ERROR_MESSAGE);
            }
            return tvInteger;
        }
    };

    public final boolean unsigned;

    Format format = Format.Decimal;

    public TVInteger() {
        this(false);
    }

    public TVInteger(boolean unsigned) {
        this.unsigned = unsigned;
    }

    public TVInteger(int value) {
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
