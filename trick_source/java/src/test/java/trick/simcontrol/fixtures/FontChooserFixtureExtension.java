package trick.simcontrol.fixtures;

import java.awt.Container;

import org.assertj.swing.core.Robot;
import org.assertj.swing.fixture.ComponentFixtureExtension;

import trick.common.ui.components.FontChooser;

public class FontChooserFixtureExtension extends ComponentFixtureExtension {
    @Override
    public FontChooserFixture createFixture(Robot robot, Container root) {
        FontChooser fontDialog = robot.finder().findByType(root, FontChooser.class, true);
        return new FontChooserFixture(robot, fontDialog);
    }
}
