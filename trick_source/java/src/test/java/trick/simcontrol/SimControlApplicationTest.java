package trick.simcontrol;

import trick.common.ApplicationTest;
import trick.simcontrol.fixtures.FontChooserFixture;
import trick.common.ui.components.FontChooser;
import trick.simcontrol.SimControlApplication;
import trick.simcontrol.StubbedSimControlApplication;

import java.awt.Font;
import java.awt.Frame;
import java.awt.Component;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.Dimension;
import java.awt.event.KeyEvent;
import java.awt.GraphicsEnvironment;
import java.awt.Toolkit;
import java.beans.Transient;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.text.JTextComponent;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;

import org.assertj.swing.core.GenericTypeMatcher;
import org.assertj.swing.core.KeyPressInfo;
import org.assertj.swing.exception.ComponentLookupException;
import org.assertj.swing.fixture.AbstractComponentFixture;
import org.assertj.swing.fixture.ComponentContainerFixture;
import org.assertj.swing.fixture.DialogFixture;
import org.assertj.swing.fixture.JButtonFixture;
import org.assertj.swing.fixture.JMenuItemFixture;
import org.assertj.swing.fixture.JOptionPaneFixture;
import org.assertj.swing.fixture.JPanelFixture;
import org.assertj.swing.fixture.JTextComponentFixture;
import org.assertj.swing.fixture.JToggleButtonFixture;
import org.assertj.swing.fixture.JToolBarFixture;
import org.assertj.swing.fixture.FrameFixture;
import org.assertj.swing.fixture.MouseInputSimulationFixture;
import org.assertj.swing.junit.testcase.AssertJSwingJUnitTestCase;

import org.jdesktop.swingx.JXFindBar;
import org.jdesktop.swingx.JXEditorPane;

import org.junit.BeforeClass;
import org.junit.Test;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.finder.WindowFinder.findFrame;
import static org.assertj.swing.launcher.ApplicationLauncher.application;

public class SimControlApplicationTest extends ApplicationTest {
    private JPanelFixture findPanel;
    private JTextComponentFixture editorFixture;
    private JToolBarFixture toolBarFixture;
    private StubbedSimControlApplication app = null;

    @BeforeClass
    public static void onSetUpBeforeClass() {

    }

    @Override
    protected void onSetUp() {       
        application(StubbedSimControlApplication.class).start();
        app = StubbedSimControlApplication.getInstance();

        mainFrame = getFrameByTitle("Sim Control");

        findPanel = mainFrame.panel(
            new GenericTypeMatcher<JXFindBar>(JXFindBar.class) {
                @Override
                protected boolean isMatching(JXFindBar pane) {
                    return true;
                }
            }
        );

        editorFixture = mainFrame.textBox(
            new GenericTypeMatcher<JXEditorPane>(JXEditorPane.class) {
                @Override
                protected boolean isMatching(JXEditorPane pane) {
                    return true;
                }
            }
        );

        toolBarFixture = mainFrame.toolBar();

        sleep(500);
    }

    //--------------------
    // JButton Tests
    //--------------------

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testConnectButton() {
        testJButton("Connect", ActionID.CONNECT);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStartButton() {
        testJButton("Start", ActionID.START);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testFreezeButton() {
        testJButton("Freeze", ActionID.FREEZE);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStepButton() {
        testJButton("Step", ActionID.STEP);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testShutdownButton() {
        testJButton("Shutdown", ActionID.SHUTDOWN);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testExitButton() {
        testJButton("Exit", ActionID.EXIT);
    }

    //-----------------------
    // JToggleButton Tests
    //-----------------------

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testDumpChkpntButton() {
        testButtonToggleTwice("Dump Chkpnt", ActionID.DUMP_CHKPNT);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testLoadChkpntButton() {
        testButtonToggleTwice("Load Chkpnt", ActionID.LOAD_CHKPNT);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testLiteToggle() {
        Frame frame = mainFrame.target();
        Dimension toggle1, toggle2; 

        testButtonToggleOnce("Lite", "Full", ActionID.LITE);
        toggle1 = frame.getSize();
        testButtonToggleOnce("Full", "Lite", ActionID.LITE);
        toggle2 = frame.getSize();

        assertThat(toggle1).isEqualTo(SimControlApplication.LITE_SIZE);
        assertThat(toggle2).isEqualTo(SimControlApplication.FULL_SIZE);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testDataRecToggle() {
        testButtonToggleTwice("Data Rec On", "Data Rec Off", ActionID.RECORDING);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testRealtimeToggle() {
        testButtonToggleTwice("RealTime On", ActionID.REALTIME);
    }

    //-----------------------------
    // Status Message Pane Tests
    //-----------------------------

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStatusMsgPane() {
        // ARRANGE
        final String msg = "How much wood can a woodchuck chuck if a woodchuck could chuck wood?";

        assumeThat(editorFixture).isNotNull();

        // ACT
        editorFixture.deleteText()
                     .enterText(msg);

        // ASSERT
        assertThat(editorFixture.target().getText());
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testFindPanelButtons() {
        // ARRANGE
        final String message  = "I must not fear.\n" + //
                                "Fear is the mind-killer.\n" + //
                                "Fear is the little-death that brings total obliteration.\n" + //
                                "I will face my fear.\n" + //
                                "I will permit it to pass over me and through me.\n" + //
                                "And when it has gone past, I will turn the inner eye to see its path.\n" + //
                                "Where the fear has gone there will be nothing. Only I will remain";
        final String outStencil = "%s@%d";
        final String[] expOutputNext = {"null@304", "fear@11",
                                        "Fear@17" , "Fear@42",
                                        "fear@114", "fear@249"},
                       expOutputPrev = {"fear@114", "Fear@42", 
                                        "Fear@17" , "fear@11"};
                                        
        String[] queryResults = new String[10];

        JXEditorPane editorPane = (JXEditorPane) editorFixture.target();
        JButtonFixture findNextButton = getButtonByText(findPanel, "Find Next"),
                       findPrevButton = getButtonByText(findPanel, "Find Previous");
        
        assumeThat(findNextButton).isNotNull();
        assumeThat(findPrevButton).isNotNull();
        
        setStatusMessage(message);
                              
        // ACT
        setFindText("Hello");
        queryResults[0] = String.format(outStencil, 
                                        editorPane.getSelectedText(), 
                                        editorPane.getSelectionStart());

        setFindText("fear");
        queryResults[1] = String.format(outStencil, 
                                        editorPane.getSelectedText(), 
                                        editorPane.getSelectionStart());

        for(int i = 2; i < 6; i++) {
            findNextButton.click();
            queryResults[i] = String.format(outStencil, 
                                            editorPane.getSelectedText(), 
                                            editorPane.getSelectionStart());
            sleep(100);
        }

        for(int i = 6; i < 10; i++) {
            findPrevButton.click();
            queryResults[i] = String.format(outStencil, 
                                            editorPane.getSelectedText(), 
                                            editorPane.getSelectionStart());
            sleep(100);
        }

        // ASSERT
        for(int i = 0; i < 6; i++) 
            assertThat(queryResults[i]).isEqualTo(expOutputNext[i]);

        for(int i = 0; i < 4; i++) 
            assertThat(queryResults[i + 6]).isEqualTo(expOutputPrev[i]);
    }

    //--------------------
    // File Menu Tests
    //--------------------

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testFontChange() {
        // ARRANGE
        FontChooserFixture fontDialogFixt;
        JButtonFixture okButton;

        JMenuItemFixture fontMenu = mainFrame.menuItem("showStatusFontMenuItem");
        final String[] fonts = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();

        assumeThat(fonts.length > 0).withFailMessage("No Available Fonts Installed...\n").isTrue();
        
        final int fontSize = 15;
        final String fontName = fonts[(int)(fonts.length / 2)],
                     expPreviewText = fontName + ":3:" + fontSize;
        final Font expFont = new Font(fontName, 3, fontSize);

        String actualPreviewText;
        Font actualFont, actualPreviewFont;
        
        fontMenu.click();
        fontDialogFixt = getFontChooserFixture();
        okButton = getButtonByText(fontDialogFixt, "Ok");

        assumeThat(fontDialogFixt).isNotNull();

        // ACT
        fontDialogFixt.selectFont(fontName);
        fontDialogFixt.selectSize(fontSize);
        fontDialogFixt.setBold(true);
        fontDialogFixt.setItalic(true);

        actualPreviewText = fontDialogFixt.getPreviewText();
        actualPreviewFont = fontDialogFixt.getPreviewFont();

        okButton.click();

        actualFont = editorFixture.font().target();

        // ASSERT
        assertThat(actualPreviewText).isEqualTo(expPreviewText);
        assertThat(actualPreviewFont).isEqualTo(expFont);
        assertThat(actualFont).isEqualTo(expFont);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testSaveStatusMessage() {
        // ARRANGE
        ActionID action;
        JMenuItemFixture saveMsg = mainFrame.menuItem("saveStatusMsgsMenuItem");

        assumeThat(saveMsg).isNotNull();

        // ACT
        saveMsg.click();
        action = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(action).isEqualTo(ActionID.SAVE_STATUS);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testClearStatusMessage() {
        // ARRANGE
        ActionID action;
        JMenuItemFixture clearMsg = mainFrame.menuItem("clearStatusMsgsMenuItem");

        editorFixture.enterText("MOOSE MOOSE MOOSE");

        assumeThat(clearMsg).isNotNull();

        // ACT
        clearMsg.click();
        action = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(action).isEqualTo(ActionID.CLEAR_STATUS);
        assertThat(editorFixture.text()).isEqualTo("");
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    /*  Not sure if I should test each of the LookAndFeel buttons  */
    @Test
    public void testLookAndFeel() {
        // ARRANGE
        JMenuItemFixture lafItem = mainFrame.menuItem("lookAndFeelMenuItem");
        UIManager.LookAndFeelInfo looks[] = UIManager.getInstalledLookAndFeels();
        
        assumeThat(looks.length > 0).withFailMessage("No LookAndFeel is installed...\n").isTrue();

        final UIManager.LookAndFeelInfo LAFI = looks[0];
        final String LAF_NAME  = LAFI.getName().equals("GTK+") ? "GTK" : LAFI.getName(),
                     LAF_CLASS = LAFI.getClassName();

        String lafClass;

        JOptionPaneFixture optFixt;

        // ACT
        lafItem.click();
        optFixt = mainFrame.optionPane();
        optFixt.buttonWithText(LAF_NAME).click();

        lafClass = UIManager.getLookAndFeel().getClass().getCanonicalName();
        
        // ASSERT
        assertThat(lafClass).isEqualTo(LAF_CLASS);

    }

    //--------------------
    // Action Menu Tests
    //--------------------

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStartTrickViewMenuItem() {
        // ARRANGE
        final JMenuItemFixture TV_ITEM = mainFrame.menuItem("startTVMenuItem");
        final ActionID TV_ACTION = ActionID.TV;

        ActionID loggedAction;

        // ACT
        TV_ITEM.click();
        loggedAction = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(loggedAction).isEqualTo(TV_ACTION);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStartMalfunctionTrickViewMenuItem() {
        // ARRANGE
        final JMenuItemFixture MTV_ITEM = mainFrame.menuItem("startMTVMenuItem");
        final ActionID MTV_ACTION = ActionID.MTV;

        ActionID loggedAction;

        // ACT
        MTV_ITEM.click();
        loggedAction = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(loggedAction).isEqualTo(MTV_ACTION);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testFreezeAtMenuItem() {
        // ARRANGE
        final JMenuItemFixture FREEZE_AT_ITEM = mainFrame.menuItem("freezeAtMenuItem");
        final ActionID FREEZE_AT_ACTION = ActionID.FREEZE_AT;

        ActionID loggedAction;

        // ACT
        FREEZE_AT_ITEM.click();
        loggedAction = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(loggedAction).isEqualTo(FREEZE_AT_ACTION);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testFreezeInMenuItem() {
        // ARRANGE
        final JMenuItemFixture FREEZE_IN_ITEM = mainFrame.menuItem("freezeInMenuItem");
        final ActionID FREEZE_IN_ACTION = ActionID.FREEZE_IN;

        ActionID loggedAction;

        // ACT
        FREEZE_IN_ITEM.click();
        loggedAction = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(loggedAction).isEqualTo(FREEZE_IN_ACTION);

    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testCheckpointMenuItem() {
        // ARRANGE
        final JMenuItemFixture CHKPNT_OBJ_ITEM = mainFrame.menuItem("checkpointObjectsMenuItem");
        final ActionID CHKPNT_OBJ_ACTION = ActionID.PART_CHKPNT;

        ActionID loggedAction;

        // ACT
        CHKPNT_OBJ_ITEM.click();
        loggedAction = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(loggedAction).isEqualTo(CHKPNT_OBJ_ACTION);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testThrottleMenuItem() {
        // ARRANGE
        final JMenuItemFixture THROTTLE_ITEM = mainFrame.menuItem("throttleMenuItem");
        final ActionID THROTTLE_ACTION = ActionID.THROTTLE;

        ActionID loggedAction;

        // ACT
        THROTTLE_ITEM.click();
        loggedAction = StubbedSimControlApplication.ActionRecord.peek();

        // ASSERT
        assertThat(loggedAction).isEqualTo(THROTTLE_ACTION);
    }

    //--------------------
    // Toolbar Tests
    //--------------------

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStartTrickViewButton() {
        final JButtonFixture TV_BUTTON = toolBarFixture.button("startTVButton");
        final ActionID TV_ACTION = ActionID.TV;

        testConnectedAction(TV_BUTTON, TV_ACTION);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testStartMalfunctionTrickViewButton() {
        final JButtonFixture MTV_BUTTON = toolBarFixture.button("startMTVButton");
        final ActionID MTV_ACTION = ActionID.MTV;

        testConnectedAction(MTV_BUTTON, MTV_ACTION);
    }

    /**
     * Purpose: 
     * Procedures:
     *   1) 
     */
    @Test
    public void testThrottleButton() {
        final JButtonFixture THROTTLE_BUTTON = toolBarFixture.button("throttleButton");
        final ActionID THROTTLE_ACTION = ActionID.THROTTLE;

        testConnectedAction(THROTTLE_BUTTON, THROTTLE_ACTION);
    }

    //--------------------
    // Helper Methods
    //--------------------

    private void setStatusMessage(String text) {
        editorFixture.deleteText()
                     .setText(text);
    }

    private void setFindText(String text) {
        JTextComponentFixture findTextField = findPanel.textBox(
            new GenericTypeMatcher<JTextField>(JTextField.class) {
                @Override
                protected boolean isMatching(JTextField pane) {
                    return true;
                }
            }
        );
        
        try {
            findTextField.deleteText()
                         .setText(text);
        } catch(Exception skipTest) {
            assumeThat(false).withFailMessage("Error with JXFindBar Implementation\n").isTrue();
        }
        
    }

    protected FontChooserFixture getFontChooserFixture() {
        try {
            AbstractComponentFixture abst = mainFrame.with(FontChooserFixture.getExtension());

            if (abst instanceof FontChooserFixture)
                return (FontChooserFixture) abst;
            else
                return null;
        } catch(Exception e) {
            return null;
        }
    }

    private void testJButton(String text, ActionID action) {
        JButtonFixture button = getButtonByText(text);
        assumeThat(button).withFailMessage("Button with text\"%s\" not found\n", text)
                          .isNotNull();

        testConnectedAction(button, action);
    }

    private void testConnectedAction(MouseInputSimulationFixture clickable, ActionID expected) {
        // ARRANGE
        ActionID actual;
        int initialLogSize, logSize;

        // ACT
        initialLogSize = StubbedSimControlApplication.ActionRecord.size();
        clickable.click();

        assumeThat(StubbedSimControlApplication.ActionRecord.size() > 0)
                    .withFailMessage("\"%s\" Action ID Not Registered", expected.name())
                    .isTrue();
        actual = StubbedSimControlApplication.ActionRecord.peek();
        logSize = StubbedSimControlApplication.ActionRecord.size();

        // ASSERT
        assertThat(actual).isEqualTo(expected);
        assertThat(logSize).isEqualTo(initialLogSize + 1);
    }

    private void testButtonToggleOnce(String initText, String toggleText, ActionID initAction) {
        // ARRANGE
        JToggleButtonFixture button = getToggleButtonByText(initText);
        String newButtonText;
        assumeThat(button).withFailMessage("Toggle Button with text\"%s\" not found\n", initText)
                          .isNotNull();

        // ACT
        testConnectedAction(button, initAction); // Toggle
        newButtonText = button.target().getText();

        // ASSERT
        assertThat(newButtonText).isEqualTo(toggleText);
    }

    private void testButtonToggleOnce(String text, ActionID action) { testButtonToggleOnce(text, text, action); }

    private void testButtonToggleTwice(String initText, ActionID initAction, String toggleText, ActionID toggleAction) {
        // ARRANGE
        JToggleButtonFixture button = getToggleButtonByText(initText);
        String buttonText, newButtonText;
        assumeThat(button).withFailMessage("Toggle Button with text\"%s\" not found\n", initText)
                          .isNotNull();

        // ACT
        testConnectedAction(button, initAction); // Toggle On
        newButtonText = button.target().getText();

        testConnectedAction(button, toggleAction); // Toggle Off
        buttonText = button.target().getText();

        // ASSERT
        assertThat(newButtonText).isEqualTo(toggleText);
        assertThat(buttonText).isEqualTo(initText);
    }

    private void testButtonToggleTwice(String text, ActionID action) { testButtonToggleTwice(text, action, text, action); }

    private void testButtonToggleTwice(String initText, String toggleText, ActionID action) { 
        testButtonToggleTwice(initText, action, toggleText, action); 
    }
}
