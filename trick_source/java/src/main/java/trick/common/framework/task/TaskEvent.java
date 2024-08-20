
//========================================
//	Package
//========================================
package trick.common.framework.task;

import java.util.EventObject;

public class TaskEvent<T> extends EventObject {
	private final T value;

	public final T getValue() {
		return value;
	}

	public TaskEvent(Task src, T val) {
		super(src);
		value = val;
	}
}
