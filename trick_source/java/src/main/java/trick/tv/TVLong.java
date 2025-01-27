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

import trick.common.utils.vs.VSLong;

@XmlRootElement
public class TVLong extends VSLong implements TrickViewFluent<TVLong.Format> {

    private String precision;

    @XmlType(name = "")
    public enum Format {

        Binary {
            public String format(long value, boolean unsigned) {
                String result = Long.toBinaryString(value);
                return "0000000000000000000000000000000000000000000000000000000000000000"
                    .substring(result.length()) + result;
            }

            public long parse(String value) {
                return new BigInteger(value, 2).longValue();
            }
        },

        Decimal {
            public String format(long value, boolean unsigned) {
                if (unsigned && value < 0) {
                    return new BigInteger(Long.toString(value)).add(
                      new BigInteger("18446744073709551616")).toString();
                }
                else {
                    return Long.toString(value);
                }
            }

            public long parse(String value) {
                return new BigInteger(value, 10).longValue();
            }
        },

        Octal {
            public String format(long value, boolean unsigned) {
                return Long.toOctalString(value);
            }

            public long parse(String value) {
                return new BigInteger(value, 8).longValue();
            }
        },

        Hexadecimal {
            public String format(long value, boolean unsigned) {
                return Long.toHexString(value);
            }

            public long parse(String value) {
                return new BigInteger(value, 16).longValue();
            }
        },

        BinaryReverseBytes {
            public String format(long value, boolean unsigned) {
                return Long.toBinaryString(Long.reverseBytes(value));
            }

            public long parse(String value) {
                return Long.reverseBytes(new BigInteger(value, 2).longValue());
            }
        },

        DecimalReverseBytes {
            public String format(long value, boolean unsigned) {
                value = Long.reverseBytes(value);
                if (unsigned && value < 0) {
                    return new BigInteger(Long.toString(value)).add(
                      new BigInteger("18446744073709551616")).toString();
                }
                else {
                    return Long.toString(value);
                }
            }

            public long parse(String value) {
                return Long.reverseBytes(new BigInteger(value, 10).longValue());
            }
        },

        OctalReverseBytes {
            public String format(long value, boolean unsigned) {
                return Long.toOctalString(Long.reverseBytes(value));
            }

            public long parse(String value) {
                return Long.reverseBytes(new BigInteger(value, 8).longValue());
            }
        },

        HexadecimalReverseBytes {
            public String format(long value, boolean unsigned) {
                return Long.toHexString(Long.reverseBytes(value));
            }

            public long parse(String value) {
                return Long.reverseBytes(new BigInteger(value, 16).longValue());
            }
        };

        public abstract String format(long value, boolean unsigned);
        public abstract long parse(String value);

    }

    static TableCellRenderer tableCellRenderer = new DefaultTableCellRenderer();

    static TableCellEditor tableCellEditor = new DefaultCellEditor(new javax.swing.JTextField() {{
        setBorder(new javax.swing.border.EmptyBorder(0, 0, 0, 0));
    }}) {

        TVLong tvLong;

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value,
          boolean isSelected, int row, int column) {
            tvLong = (TVLong)value;
            return super.getTableCellEditorComponent(table, value, isSelected, row, column);
        }

        @Override
        public TVLong getCellEditorValue() {
            try {
                return new TVLong(tvLong.getFormat().parse(
                  super.getCellEditorValue().toString().trim()));
            }
            catch (NumberFormatException numberFormatException) {
                JOptionPane.showMessageDialog(getComponent(), numberFormatException.toString(),
                  "Invalid Input", JOptionPane.ERROR_MESSAGE);
            }
            return tvLong;
        }
    };

    public final boolean unsigned;

    Format format = Format.Decimal;

    public TVLong() {
        this(false);
    }

    public TVLong(boolean unsigned) {
        this.unsigned = unsigned;
    }

    public TVLong(long value) {
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
