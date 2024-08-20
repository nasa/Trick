
//========================================
//	Package
//========================================
package trick.common.framework.task.utils;

import java.awt.Component;

import javax.swing.Action;

import trick.common.framework.task.Task;
import trick.common.framework.task.TaskEvent;
import trick.common.framework.task.TaskListener;
import trick.common.framework.BaseApplication;
import trick.common.utils.TrickAction;
import trick.common.utils.TrickResources;

public class InputBlocker {
    private final Task task;
    private final BlockingTarget target;
    private final TrickAction action;

    public InputBlocker(Task t, BlockingTarget b, TrickAction a) {
        validateTask(t);
    
        task = t;
        target = b;
        action = a;
    }

    // TODO: Implement this
    public boolean willBlock() {
        return target != null;
    }

    public void block() {
        target.setBlocked(true);
    }

    public void unblock() {
        target.setBlocked(false);
        
    }
    
    private static void validateTask(Task t) {
        if (t == null)
            throw new IllegalArgumentException("task null");
        else if (t.getTaskService() != null)
            throw new IllegalStateException("task currently being executed");
    }
    
}

