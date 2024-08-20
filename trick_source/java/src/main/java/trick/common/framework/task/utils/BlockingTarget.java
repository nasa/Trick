
//========================================
//	Package
//========================================
package trick.common.framework.task.utils;

import java.awt.Component;

import javax.swing.Action;

public abstract class BlockingTarget {
    private final Object target;
    
    public BlockingTarget(Object t) { target = t; }
    
    public abstract void setBlocked(boolean state);
    
    public enum Scope {
        NONE,
        ACTION,
        COMPONENT,
        WINDOW,
        APPLICATION
    }

    private static void validateScope(Scope s, Object o) {
        switch (s) {
            case ACTION:
                if (!(o instanceof Action)) 
                    throw new IllegalArgumentException("target isn't an Action");
                break;
            
            case COMPONENT:
            case WINDOW:
                if (!(o instanceof Component)) 
                    throw new IllegalArgumentException("target isn't a Component");
                break;
            
            default: 
        }
    }

    public static BlockingTarget ScopedTarget(Object target, Scope scope) {
        validateScope(scope, target);
        
        switch (scope) {
            case ACTION:
                return new BlockingTarget(target) {
                    @Override
                    public void setBlocked(boolean state) {
                        Action act = (Action) target;
                        act.setEnabled(!state);
                    }
                };
            
            case COMPONENT:
                return new BlockingTarget(target) {
                    @Override
                    public void setBlocked(boolean state) {
                        Component comp = (Component) target;
                        comp.setEnabled(!state);
                    }
                };

            case WINDOW:
            case APPLICATION:
                return new BlockingTarget(target) {
                    @Override
                    public void setBlocked(boolean state) {
                        // showBusyGlassPane
                        // showBlockingDialog
                    }
                };
        
            default:
                return null;
        }
    }
}