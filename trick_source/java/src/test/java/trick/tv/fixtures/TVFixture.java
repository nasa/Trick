package trick.tv.fixtures;

import java.io.File;

import javax.swing.JMenuItem;
import javax.swing.JOptionPane;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.Robot;
import org.assertj.swing.data.TableCell;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JFileChooserFixture;
import org.assertj.swing.fixture.JMenuItemFixture;
import org.assertj.swing.fixture.JOptionPaneFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JScrollPaneFixture;
import org.assertj.swing.fixture.JTableFixture;
import org.assertj.swing.fixture.JTextComponentFixture;
import org.assertj.swing.fixture.JTreeFixture;
import static org.assertj.swing.timing.Timeout.timeout;
import org.jdesktop.swingx.JXTitledPanel;

import trick.sie.utils.SearchPanel;
import trick.tv.TVApplication;

public class TVFixture extends FrameFixture {
    private JPanelFixture varSearchPanel;
    private JTableFixture varSelectedPanel;                          

    private JTextComponentFixture searchBar;

    private JCheckBoxFixture caseSCheckBox,
                             regexCheckBox,
                             greedCheckBox;

    private JScrollPaneFixture searchResults;
    private JTreeFixture varTree;

    public TVFixture(Robot robot, TVApplication target) {
        super(robot, target.getMainFrame());

        // varTreePanel = panel(new JXTitledPanelMatcher("Variables"));
        varSelectedPanel = table();
        varSearchPanel = panel(
            new GenericTypeMatcher<SearchPanel>(SearchPanel.class) {
                @Override
                protected boolean isMatching(SearchPanel panel) {
                    return true;
                }
            });

        varTree = tree();

        // searchBar     = varSearchPanel.textBox();
        // searchResults = varSearchPanel.scrollPane();
        caseSCheckBox = varSearchPanel.checkBox("caseSensitiveCheckBox");
        regexCheckBox = varSearchPanel.checkBox("regularExpressionCheckBox");
        greedCheckBox = varSearchPanel.checkBox("greedySearchCheckBox");
    }

    //---------------------------
    //      Fixture Methods      
    //---------------------------

    public void selectVar(String varPath) {
        varPath = varPath.replace(".", "/");
        varTree.doubleClickPath(varPath);
    }

    public boolean saveMenuItem(File file) {
        if (file != null && file.exists())   
            file.delete();

        return triggerFileChooserMenuItem(file, "Save");
    }

    public boolean openMenuItem(File file) {
        if (file != null && file.exists())
            return triggerFileChooserMenuItem(file, "Open");
        else
            return  false;
    }

    public boolean openSetMenuItem(File file) {
        if (file != null && file.exists())
            return triggerFileChooserMenuItem(file, "Open & Set");
        else
            return  false;
    } 

    public boolean setValMenuItem(File file) {
        if (file != null && file.exists())
            return triggerFileChooserMenuItem(file, "Set");
        else
            return  false;
    }

    public void clickMonitorToggleMenuItem() {
        menuItemByText("Toggle Monitor").click();
    }

    public void enterQuery(String query) {
        varSearchPanel.textBox()
                      .deleteText()
                      .enterText(query);
    }

    public void setSearchOptions(int opts) {
        boolean senseSel = (opts & CASE_SENSITIVE_ON) == CASE_SENSITIVE_ON,
                regexSel = (opts & REG_EXPRESSION_ON) == REG_EXPRESSION_ON,
                greedSel = (opts & GREEDY_SEARCH_ON ) == GREEDY_SEARCH_ON ;

        caseSCheckBox.check(senseSel);
        regexCheckBox.check(regexSel);
        greedCheckBox.check(greedSel);
    }

    public JOptionPaneFixture getErrorPopupFixture() {
        JOptionPaneFixture opt = optionPane(timeout(1500));
        if(opt.target().getMessageType() == JOptionPane.ERROR_MESSAGE)
            return opt;
        else return null;

    }

    public String[][] getSelectedVars() {
        return varSelectedPanel.contents();
    }

    public void editVariableTable(int row, int col, String entry) {
        TableCell cell = TableCell.row(row).column(col);
        varSelectedPanel.enterValue(cell, entry);
    }

    public String getCellValue(int row, int col) {
        TableCell cell = TableCell.row(row).column(col);
        return varSelectedPanel.valueAt(cell);
    }

    public String[] getSearchResults() {
        String[] list = varSearchPanel.list().contents();
        return list.length > 0 ? list : null;
    }

    private JMenuItemFixture menuItemByText(String text) {
        return menuItem(new GenericTypeMatcher<JMenuItem>(JMenuItem.class) {
            @Override
            protected  boolean  isMatching(JMenuItem item) {
                return item.getText().equals(text);
            };
        });
    }

    private boolean triggerFileChooserMenuItem(File file, String menuItemText) {
        if(file == null) return false;

        String path = file.getAbsolutePath();

        try {
            menuItemByText(menuItemText).click();

            JFileChooserFixture fc = fileChooser(timeout(1500));
            JTextComponentFixture fileEntryTextBox = fc.fileNameTextBox();

            fileEntryTextBox.deleteText()
                            .enterText(path);
            fc.approve();
        } catch(Exception e) {
            return false;
        }

        return true;
    }

    //---------------------------
    //       Inner Classes       
    //---------------------------

    private class JXTitledPanelMatcher extends GenericTypeMatcher<JXTitledPanel> {
        private final String title;

        public JXTitledPanelMatcher(String title) {
            super(JXTitledPanel.class);

            this.title = title;
        }

        @Override
        protected boolean isMatching(JXTitledPanel pane) {
            return pane.getTitle().equals(title);
        }
    }

    public static final int CASE_SENSITIVE_ON = 0b100,
                            REG_EXPRESSION_ON = 0b010,
                            GREEDY_SEARCH_ON  = 0b001;

    public static final int CASE_SENSITIVE_OFF = 0b000,
                            REG_EXPRESSION_OFF = 0b000,
                            GREEDY_SEARCH_OFF  = 0b000;
}