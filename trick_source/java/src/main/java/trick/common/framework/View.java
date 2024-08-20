
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Image;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JPanel;
import javax.swing.JRootPane;
import javax.swing.JToolBar;
import javax.swing.border.Border;

import trick.common.framework.BaseApplication;
import trick.common.utils.TrickResources;

public class View {

	private BaseApplication app;

	private JFrame frame;
	private JComponent component, statusBar, toolBarPanel;
	private JMenuBar menuBar;
	private List<JToolBar> toolBars = Collections.emptyList();


	public View(BaseApplication app) {
		this.app = app;
	}

	public void setComponent(JComponent c) {
		JComponent old = component;
		component = c;

		replaceComponent(old, component, BorderLayout.CENTER);

		// TODO: Fire Property Change
	}

	public void setMenuBar(JMenuBar m) {
		// JMenuBar old = menuBar;
		menuBar = m;

		getRootPane().setJMenuBar(m);

		// TODO: Fire Property Change
	}

	public JMenuBar getMenuBar() {
		// JMenuBar old = menuBar;
		return menuBar;
	}

	public JToolBar getToolBar() {
		return (toolBars.size() == 0) ? null : toolBars.get(0);
	}

	public void setToolBar(JToolBar t) {
		JToolBar old = getToolBar();
		List<JToolBar> tools = null;

		if (t != null)
			tools = Collections.singletonList(t);
		
		setToolBars(tools);

		// TODO: Fire Property Change
	}

	public void setToolBars(List<JToolBar> l) {
		if (l == null)
			l = Collections.emptyList();
		
		List<JToolBar> old = toolBars;
		toolBars = Collections.unmodifiableList(new ArrayList(l));

		JComponent oldPanel = toolBarPanel, newPanel = null;
		if (toolBars.size() == 1) {
			newPanel = toolBars.get(0);
		} else if(toolBars.size() > 1) {
			newPanel = new JPanel();
			for (JComponent jToolBar : toolBars) {
				newPanel.add(jToolBar);
			}
		}

		replaceComponent(oldPanel, newPanel, BorderLayout.NORTH);

		// TODO: Fire Property Change
	}

	public void setStatusBar(JComponent c) {
		JComponent old = statusBar;
		statusBar = c;

		replaceComponent(old, statusBar, BorderLayout.SOUTH);

		// TODO: Fire Property Change
	}

	public JFrame getFrame() {
		if (frame == null) {
			TrickResources resources = app.getContext().getResourceMap();

			String title = resources.getString("Application.title");
			ImageIcon icon = resources.getImageIcon("Application.icon");

			frame = new JFrame(title);
			frame.setName("mainFrame");

			if (icon != null) {
				Image img = icon.getImage();
				frame.setIconImage(img);
			}
		}

		return frame;
	}

	public JRootPane getRootPane() {
		return getFrame().getRootPane();
	}

	public void setFrame(JFrame f) {
		if (f == null)
            throw new IllegalArgumentException("null JFrame");

        if (frame != null)
            throw new IllegalStateException("frame already set");

        frame = f;
	}

	public BaseApplication getApplication() {
		return app;
	}

	private void replaceComponent(JComponent oldC, JComponent newC, String constraint) {
		Container contentPane = getRootPane().getContentPane();

		if (oldC != null)  
			contentPane.remove(oldC);

		if (newC != null)  
			contentPane.add(newC, constraint);
	}
}
