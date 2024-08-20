
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.Component;

public interface PropertySupport {
	public abstract Object getSessionState(Component c);
	public abstract void setSessionState(Component c, Object state);
}
