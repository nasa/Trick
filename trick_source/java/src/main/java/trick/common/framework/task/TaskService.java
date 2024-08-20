
//========================================
//	Package
//========================================
package trick.common.framework.task;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import javax.swing.SwingUtilities;

import trick.common.framework.task.Task;
import trick.common.framework.task.utils.InputBlocker;

public class TaskService extends ThreadPoolExecutor {
	private String name;

	public TaskService(String name) {
		super(3,10,1L, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>());

		this.name = name;

	}

	// TODO: Rewrite this
	public void execute(Task task) {
		// Input Validation
		if (task == null) {
            throw new IllegalArgumentException("null task");
        }
        if (!task.isPending() || (task.getTaskService() != null)) {
            throw new IllegalArgumentException("task has already been executed");
        }
		
		// Setup for task execution
        task.setTaskService(this);
        // TBD: what if task has already been submitted?
        // task.addPropertyChangeListener(taskPCL);
		
		// TODO: Add Task to Queue
        // List<Task<?, ?>> oldTaskList, newTaskList;
        // synchronized (tasks) {
        //     oldTaskList = tasks.getList();
        //     tasks.addLast(task);
        //     newTaskList = tasks.getList();
        // }
        // firePropertyChange(new TaskPropertyChangeEvent(oldTaskList, newTaskList, null, task));
		
        maybeBlockTask(task);
        super.execute(task);
		
	}

	public final String getName() {  return name;  }

	// TODO: Implement this
	private void maybeBlockTask(Task task) {
		final InputBlocker blocker = task.getInputBlocker();

		if(blocker == null ^ !blocker.willBlock()) {
			return;
		}

		if(SwingUtilities.isEventDispatchThread()) {
			blocker.block();
		} else {
			Runnable blockerRunnable = new Runnable() {
				@Override
				public void run() { blocker.block(); }
			};

			SwingUtilities.invokeLater(blockerRunnable);
		}
	}
	
}
