
//========================================
//	Package
//========================================
package trick.common.framework.task;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.lang.Thread.State;
import java.security.InvalidParameterException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutionException;

import javax.swing.Action;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;

import trick.common.framework.task.TaskEvent;
import trick.common.framework.task.TaskListener;
import trick.common.framework.task.utils.BlockingTarget;
import trick.common.framework.task.utils.InputBlocker;
import trick.common.framework.BaseApplication;
import trick.common.utils.TrickAction;
import trick.common.utils.TrickResources;

public abstract class Task <T, V> extends SwingWorker <T, V>{

	public static final String PROP_DESCRIPTION = "description";
    public static final String PROP_INPUTBLOCKER = "inputBlocker";
    public static final String PROP_MESSAGE = "message";
    public static final String PROP_TASKSERVICE = "taskService";
    public static final String PROP_TITLE = "title";
    public static final String PROP_USERCANCANCEL = "userCanCancel";
    public static final String PROP_COMPLETED = "completed";
    public static final String PROP_DONE = "done";
    public static final String PROP_STARTED = "started";

    private static final Logger logger = Logger.getLogger(Task.class.getName());

	private final BaseApplication application;
	private final TrickResources resourceMap;

	private List<TaskListener<T,V>> taskListeners;
	private long    messageTime, taskStart, taskDone;
	private String  title, description, message;
	private boolean progressProperty;
	private InputBlocker blocker;
	private TaskService service;

	public Task(BaseApplication app) {
		this.application = app;
		this.resourceMap = application.getContext().getResourceMap();
		taskListeners = new CopyOnWriteArrayList<TaskListener<T, V>>();

        initTask();
	}
	
	public boolean isPending() {
		return getState() == StateValue.PENDING;
	}

	public TaskService getTaskService() {
		return service;
	}

	public void setTaskService(TaskService serv) {
		service = serv;
	}

	public InputBlocker getInputBlocker() {
		return blocker;
	}

	public void setInputBlocker(InputBlocker block) {
		blocker = block;
	}

	// protected abstract Void doInBackground();
	protected abstract void finished();
	protected void cancelled() {}
	protected void succeeded(T result) {}
	protected void interrupted(InterruptedException e) {}

    protected void failed(Throwable cause) {
        String msg = String.format("%s failed: %s", this, cause);
        logger.log(Level.SEVERE, msg, cause);
    }

	protected final void setProgress(float percentage) {
		if (percentage < 0.0f || percentage > 1.0f) {
			String msg = String.format("'%f' is not a valid percentage", percentage);
			throw new InvalidParameterException(msg);
		}

		setProgress(Math.round(percentage * 100.0f));

	}

	// TODO: Re-Write This
	private void initTask() {
        if (resourceMap != null) {
            title = resourceMap.getString(PROP_TITLE);
            description = resourceMap.getString(PROP_DESCRIPTION);
            message = resourceMap.getString(PROP_MESSAGE);
            if (message != null) {
                messageTime = System.currentTimeMillis();
            }
        }
        addPropertyChangeListener(new StatePCL());
        // taskListeners = new CopyOnWriteArrayList<TaskListener<T, V>>();
    }

	// TODO: Rewrite this
	private void fireDoInBackgroundListeners() {
        TaskEvent<Void> event = new TaskEvent<Void>(this, null);
        for (TaskListener<T, V> listener : taskListeners) {
            listener.doInBackground(event);
        }
    }

	// TODO: Rewrite this
    private void fireSucceededListeners(T result) {
        TaskEvent<T> event = new TaskEvent<T>(this, result);
        for (TaskListener<T, V> listener : taskListeners) {
            listener.succeeded(event);
        }
    }

	// TODO: Rewrite this
    private void fireCancelledListeners() {
        TaskEvent<Void> event = new TaskEvent<Void>(this, null);
        for (TaskListener<T, V> listener : taskListeners) {
            listener.cancelled(event);
        }
    }

	// TODO: Rewrite this
    private void fireInterruptedListeners(InterruptedException e) {
        TaskEvent<InterruptedException> event = new TaskEvent<InterruptedException>(this, e);
        for (TaskListener<T, V> listener : taskListeners) {
            listener.interrupted(event);
        }
    }

	// TODO: Rewrite this
    private void fireFailedListeners(Throwable e) {
        TaskEvent<Throwable> event = new TaskEvent<Throwable>(this, e);
        for (TaskListener<T, V> listener : taskListeners) {
            listener.failed(event);
        }
    }

	// TODO: Rewrite this
    private void fireFinishedListeners() {
        TaskEvent<Void> event = new TaskEvent<Void>(this, null);
        for (TaskListener<T, V> listener : taskListeners) {
            listener.finished(event);
        }
    }

	// TODO: Rewrite this
    private void fireCompletionListeners() {
        try {
            if (isCancelled()) {
                fireCancelledListeners();
            } else {
                try {
                    fireSucceededListeners(get());
                } catch (InterruptedException e) {
                    fireInterruptedListeners(e);
                } catch (ExecutionException e) {
                    fireFailedListeners(e.getCause());
                }
            }
        } finally {
            fireFinishedListeners();
        }
    }

	// TODO: Rewrite this
	private class StatePCL implements PropertyChangeListener {
		@Override
		public void propertyChange(PropertyChangeEvent e) {
			if(eventIs(e, "state")) {
				StateValue state = (StateValue) e.getNewValue();

				switch (state) {
					case STARTED: taskBegun(); break;
					case    DONE: taskEnded(); break;
					default: break;
				}
			} else if(eventIs(e, "progress")) {
				synchronized (Task.this) {
					progressProperty = true;
				}
			}
		}

		private boolean eventIs(PropertyChangeEvent event, String name) {
			return name.equals(event.getPropertyName());
		}

		private void taskBegun() {
			synchronized (Task.this) {
                taskStart = System.currentTimeMillis();
            }
            firePropertyChange(PROP_STARTED, false, true);
            fireDoInBackgroundListeners();
		}

		private void taskEnded() {
			synchronized (Task.this) {
                taskDone = System.currentTimeMillis();
            }

			try {
				removePropertyChangeListener(this);
				firePropertyChange(PROP_DONE, false, true);
			} finally {
				SwingUtilities.invokeLater(new TaskCleanup());
			}
		}
	}

	// TODO: Re-Write this too
	private class TaskCleanup implements Runnable {
		@Override
		public void run () {
			try {
				handleEndState();
			} finally {
				handleFinish();
			}
		}

		private void handleEndState() {
			if(isCancelled()) {
				cancelled();
			} else try {
				succeeded(get());
			} catch (InterruptedException e) {
				interrupted(e);
			} catch (ExecutionException e) {
				failed(e.getCause());
			}
		}

		private void handleFinish() {
			finished();
			try {
				fireCompletionListeners();
			} finally {
				firePropertyChange(PROP_COMPLETED, false, true);
			}
		}
	}
	
}
