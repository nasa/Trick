
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.Component;
import java.awt.GraphicsConfiguration;
import java.awt.Insets;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Window;

import javax.swing.JPopupMenu;
import javax.swing.RootPaneContainer;

public final class SwingToolbox {

	public static Point defaultLocation(Window window) {
        GraphicsConfiguration gc = window.getGraphicsConfiguration();
        Rectangle bounds = gc.getBounds();
        Insets insets = window.getToolkit().getScreenInsets(gc);
        int x = bounds.x + insets.left;
        int y = bounds.y + insets.top;
        return new Point(x, y);
    }

	public static RootPaneContainer findRootPaneContainer(Component root) {
        while (root != null) {
            if (root instanceof RootPaneContainer) {
                return (RootPaneContainer) root;
            } else if (root instanceof JPopupMenu && root.getParent() == null) {
                root = ((JPopupMenu) root).getInvoker();
            } else {
                root = root.getParent();
            }
        }
        return null;
    }
	
	public static String getSessionFileName(Window w) {
		if(w == null) return null;
		
		return w.getName() + ".session.xml";
	}
}
