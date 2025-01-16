package trick.simcontrol.fixtures;

import java.awt.Font;
import java.awt.GraphicsEnvironment;

import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.DialogFixture;
import org.assertj.swing.fixture.JCheckBoxFixture;
import org.assertj.swing.fixture.JLabelFixture;
import org.assertj.swing.fixture.JListFixture;
import org.assertj.swing.fixture.JPanelFixture;

import trick.simcontrol.fixtures.FontChooserFixtureExtension;
import trick.common.ui.components.FontChooser;

public class FontChooserFixture extends DialogFixture {
    private JListFixture fonts, sizes;
    private JCheckBoxFixture bold, italic;
    private JLabelFixture preview;

    public static final String[] FONT_LIST = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();

    public FontChooserFixture(Robot robot, FontChooser target) {
        super(robot, target);

        JPanelFixture fontPanel = panel("Font"),
                      sizePanel = panel("Size"),
                      stylPanel = panel("Style"),
                      prevPanel = panel("Preview"),
                      bttnPanel = panel("Buttons");
        
        fonts   = fontPanel.list();
        sizes   = sizePanel.list();
        bold    = stylPanel.checkBox("BoldCheck");
        italic  = stylPanel.checkBox("ItalicCheck");
        preview = prevPanel.label();

        
    }

    public void selectFont(int index)   {  fonts.clickItem(index);  }
    public void selectFont(String name) {  fonts.clickItem(name);  }

    public void selectSize(int size) {
        if (size < 4)       size = 4;
        else if (size > 24) size = 24;

        sizes.clickItem("" + size);
    }

    public void setBold(boolean toggled) {
        if (toggled) bold.check();
        else         bold.uncheck();
    }

    public void setItalic(boolean toggled) {
        if (toggled) italic.check();
        else         italic.uncheck();
    }

    public String getPreviewText() {
        return preview.text();
    }

    public Font getPreviewFont() {
        return preview.font().target();
    }

    public static FontChooserFixtureExtension getExtension() {
        return new FontChooserFixtureExtension();
    }

}