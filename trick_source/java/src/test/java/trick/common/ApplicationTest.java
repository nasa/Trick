package trick.common;

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
import java.io.File;
import java.nio.file.Files;
import java.util.ArrayList;
import java.util.Arrays;

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

public abstract class ApplicationTest extends AssertJSwingJUnitTestCase {
    protected FrameFixture mainFrame;

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}

    public boolean sameFileContent(File a, File b) {
        try {
            byte[] a_bytes = Files.readAllBytes(a.toPath()),
                   b_bytes = Files.readAllBytes(b.toPath());
        
            return Arrays.equals(a_bytes, b_bytes);
        } catch(Exception e) {
            System.err.println(e.getMessage());
        }

        return false;
    }

    public <T> void assertThat2DArraysAreEqual(T[][] array_a, T[][] array_b) {
        assertThat(array_a.length)
			.withFailMessage("Unexpected number of rows.")
			.isEqualTo(array_b.length);

		for (int i = 0; i < array_a.length; i++) {
			assertThat(array_a[i].length)
				.withFailMessage("Unexpected number of columns.")
				.isEqualTo(array_b[i].length);

			for (int j = 0; j < array_a[i].length; j++) {
				assertThat(array_a[i][j])
					.isEqualTo(array_b[i][j]);
			}
		}
    }

    protected FrameFixture getFrameByTitle(String title) {
        FrameFixture frame = findFrame(new GenericTypeMatcher<Frame>(Frame.class) {
            protected boolean isMatching(Frame frame) {
                return title.equals(frame.getTitle()) && frame.isShowing();
            }
        }).using(robot());

        return frame;
    }

    protected JButtonFixture getButtonByText(ComponentContainerFixture container, String text) {
        JButtonFixture button;

        try {
            button = container.button(new GenericTypeMatcher<JButton>(JButton.class) {
                @Override
                protected boolean isMatching(JButton button) {
                    return text.equals(button.getText());
                }
            });
        } catch (ComponentLookupException e) {
            return null;
        }

        return button;
    }
	
    protected JButtonFixture getButtonByText(String text) {
        return getButtonByText(mainFrame, text);
    }

    protected JToggleButtonFixture getToggleButtonByText(ComponentContainerFixture container, String text) {
        JToggleButtonFixture button;

        try {
            button = container.toggleButton(new GenericTypeMatcher<JToggleButton>(JToggleButton.class) {
                @Override
                protected boolean isMatching(JToggleButton button) {
                    return text.equals(button.getText());
                }
            });
        } catch (ComponentLookupException e) {
            return null;
        }

        return button;
    }

    protected JToggleButtonFixture getToggleButtonByText(String text) {
        return getToggleButtonByText(mainFrame, text);
    }

}
