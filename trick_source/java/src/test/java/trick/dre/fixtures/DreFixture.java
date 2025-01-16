package trick.dre.fixtures;

import java.awt.Font;
import java.awt.GraphicsEnvironment;
import java.io.File;
import java.io.StringWriter;

import javax.swing.JCheckBoxMenuItem;
import javax.swing.JOptionPane;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.DialogFixture;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JComboBoxFixture;
import org.assertj.swing.fixture.JFileChooserFixture;
import org.assertj.swing.fixture.JLabelFixture;
import org.assertj.swing.fixture.JListFixture;
import org.assertj.swing.fixture.JMenuItemFixture;
import org.assertj.swing.fixture.JOptionPaneFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JScrollPaneFixture;
import org.assertj.swing.fixture.JTextComponentFixture;
import org.assertj.swing.fixture.JTreeFixture;

import org.jdesktop.swingx.JXTitledPanel;

import trick.dre.DreApplication;
import trick.common.ui.components.FontChooser;
import trick.sie.utils.SearchPanel;

import static org.assertj.swing.timing.Timeout.timeout;

public class DreFixture extends FrameFixture {
    private JPanelFixture varTreePanel,
                          varSelectedPanel,
                          varSearchPanel;

    private JTextComponentFixture nameField,
                                  cycleField,
                                  fileSizeField,
                                  searchBar;

    private JCheckBoxFixture sizeLimitCheckBox,
                             caseSCheckBox,
                             regexCheckBox;

    private JScrollPaneFixture searchResults;
    private JComboBoxFixture sizeUnitComboBox;
    private JTreeFixture varTree;

    public DreFixture(Robot robot, DreApplication target) {
        super(robot, target.getMainFrame());

        varTreePanel = panel(new JXTitledPanelMatcher("Variables"));
        varSelectedPanel = panel(new JXTitledPanelMatcher("Selected Variables"));
        varSearchPanel = panel(
            new GenericTypeMatcher<SearchPanel>(SearchPanel.class) {
                @Override
                protected boolean isMatching(SearchPanel panel) {
                    return true;
                }
            });

        nameField     = textBox("groupNameField");
        cycleField    = textBox("cycleField");
        fileSizeField = textBox("fileSizeField");

        sizeLimitCheckBox = checkBox("sizeLimitToggle");
        sizeUnitComboBox  = comboBox("unitsComboBox");

        varTree = varTreePanel.tree();

        searchBar     = varSearchPanel.textBox();
        searchResults = varSearchPanel.scrollPane();
        caseSCheckBox = varSearchPanel.checkBox("caseSensitiveCheckBox");
        regexCheckBox = varSearchPanel.checkBox("regularExpressionCheckBox");
    }

    //---------------------------
    //      Fixture Methods      
    //---------------------------

    public void selectVar(String varPath) {
        varPath = varPath.replace(".", "/");
        varTree.doubleClickPath(varPath);
    }

    public void setOptions(int opts) {
        switch (opts & 0b11100000000) {
            case BINARY: selectOption("Binary"); break;
            case ASCII:  selectOption("Ascii"); break;
            case HDF5:   selectOption("HDF5"); break;
            default: break;
        }

        switch (opts & 0b00011100000) {
            case ALWAYS:  selectOption("Always"); break;
            case CHANGES: selectOption("Changes"); break;
            case STEP:    selectOption("StepChanges"); break;
            default: break;
        }

        switch (opts & 0b00000011100) {
            case BUFFER:      selectOption("Buffer"); break;
            case NO_BUFFER:   selectOption("NoBuffer"); break;
            case RING_BUFFER: selectOption("RingBuffer"); break;
            default: break;
        }

        switch (opts & 0b00000000011) {
            case SINGLE_PREC_ON:  setSinglePrec(true);  break;
            case SINGLE_PREC_OFF: setSinglePrec(false); break;
            default: break;
        }
    }

    public void saveMenuItem(String path) {
        menuItem("saveDRMenuItem").click();

        JFileChooserFixture fc = fileChooser(timeout(1500));
        JTextComponentFixture fileEntryTextBox = fc.fileNameTextBox();

        fileEntryTextBox.deleteText()
                        .enterText(path);
        fc.approve();
    }

    public void openMenuItem(String path) {
        menuItem("openDRMenuItem").click();

        JFileChooserFixture fc = fileChooser(timeout(1500));
        JTextComponentFixture fileEntryTextBox = fc.fileNameTextBox();

        fileEntryTextBox.deleteText()
                        .enterText(path);
        fc.approve();
    }

    public void setGroupName(String name) {
        nameField.deleteText()
                 .enterText(name);
    }

    public void setCycle(String cycle) {
        cycleField.deleteText()
                  .enterText(cycle);
    }

    public void setMaxFileSize(String fileSize, Size unit) {
        fileSizeField.deleteText()
                     .enterText(fileSize);
        switch(unit) {
            case B:  sizeUnitComboBox.selectItem("B");   break;
            case KB: sizeUnitComboBox.selectItem("KiB"); break;
            case MB: sizeUnitComboBox.selectItem("MiB"); break;
            case GB: sizeUnitComboBox.selectItem("GiB"); break;
            case UNLIMITED: sizeLimitCheckBox.check();   break;
        }
    }    

    public void enterQuery(String query) {
        varSearchPanel.textBox()
                      .deleteText()
                      .enterText(query);
    }

    public JOptionPaneFixture getErrorPopupFixture() {
        JOptionPaneFixture opt = optionPane(timeout(1500));
        if(opt.target().getMessageType() == JOptionPane.ERROR_MESSAGE)
            return opt;
        else return null;

    }

    public int getSelectedOptions() {
        int opts = 0b00000000000;

        if (getOptionState("Binary")) 
            opts |= BINARY;
        else if(getOptionState("Ascii"))
            opts |= ASCII;
        else if (getOptionState("HDF5"))
            opts |= HDF5;

        if (getOptionState("Always")) 
            opts |= ALWAYS;
        else if(getOptionState("Changes"))
            opts |= CHANGES;
        else if (getOptionState("StepChanges"))
            opts |= STEP;

        if (getOptionState("Buffer")) 
            opts |= BUFFER;
        else if(getOptionState("NoBuffer"))
            opts |= NO_BUFFER;
        else if (getOptionState("RingBuffer"))
            opts |= RING_BUFFER;

        if(isSinglePrec())
            opts |= SINGLE_PREC_ON;
        else
            opts |= SINGLE_PREC_OFF;

        return opts;
    }

    public String[] getSelectedVars() {
        return varSelectedPanel.list().contents();
    }

    public String[] getSearchResults() {
        String[] list = varSearchPanel.list().contents();
        return list.length > 0 ? list : null;
    }

    private boolean getOptionState(String ID) {
        String menuItemName = "selectDR" + ID + "MenuItem";
        return menuItem(menuItemName).target().isSelected();
    }

    private void selectOption(String ID) {
        String menuItemName = "selectDR" + ID + "MenuItem";
        menuItem(menuItemName).click();
    }

    private boolean isSinglePrec() {
        String singlePrecName = "toggleSinglePrecisionCheckBoxMenuItem";
        return menuItem(singlePrecName).target().isSelected();
    }

    private void setSinglePrec(boolean state) {
        String singlePrecName = "toggleSinglePrecisionCheckBoxMenuItem";
        JMenuItemFixture singlePrecCB = menuItem(singlePrecName);
        JCheckBoxMenuItem singlePrecTarget = (JCheckBoxMenuItem) singlePrecCB.target();
        if(state ^ singlePrecTarget.getState()){
            singlePrecCB.click();
        }
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

    //----------------------------
    //        Enumerations        
    //----------------------------

    public enum Size {
        B(""), KB(" * 1024"), MB(" * 1048576"), GB(" * 1073741824"), UNLIMITED("");

        public final String TAG;

        Size(String val) { TAG = val; }
    }

    //----------------------------
    //     Constant Variables     
    //----------------------------

    public final static int BINARY          = 0b10000000000,
                            ASCII           = 0b01000000000,
                            HDF5            = 0b00100000000,
                            ALWAYS          = 0b00010000000,
                            CHANGES         = 0b00001000000,
                            STEP            = 0b00000100000,
                            BUFFER          = 0b00000010000,
                            NO_BUFFER       = 0b00000001000,
                            RING_BUFFER     = 0b00000000100,
                            SINGLE_PREC_ON  = 0b00000000010,
                            SINGLE_PREC_OFF = 0b00000000001;
}