
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.Component;
import java.awt.Container;
import java.beans.BeanInfo;
import java.beans.IntrospectionException;
import java.beans.Introspector;
import java.beans.PropertyDescriptor;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.logging.Logger;

import javax.swing.ActionMap;
import javax.swing.JMenu;

import trick.common.TrickApplication;
import trick.common.framework.BaseApplication;
import trick.common.framework.Session;
import trick.common.framework.task.TaskService;
import trick.common.utils.SwingAction;
import trick.common.utils.TrickAction;
import trick.common.utils.TrickResources;

public class AppContext {
	
	private final Class appClass;
	private final HashMap<Object, ActionMap> actionMaps;
	private TrickResources resources = null;
	private TaskService service;
	private Session session;
	private static final Logger logger = Logger.getLogger(AppContext.class.getName());

	public AppContext(Class<? extends BaseApplication> appClass) {
		this.appClass = appClass;
		session = new Session();
		service = new TaskService("default");
		actionMaps = new HashMap<Object, ActionMap>();
	}

	public Class getApplicationClass() {
		return appClass;
	}

	public ActionMap getActionMap(Class c, Object o) {
		if(c == null) {
			throw new IllegalArgumentException("Null Class");
		}

		if(o == null) {
			throw new IllegalArgumentException("Null Object");
		}

		if(!c.isAssignableFrom(o.getClass())) {
			throw new IllegalArgumentException("Given Object isn't an instance of the given Class");
		}

    	ActionMap actionMap = actionMaps.get(o);

		if(actionMap == null) {
			actionMap = new ActionMap();
    	
			for (Method man : c.getMethods()) {
				if (isSwingAction(man)) {
					String name = man.getName();
					TrickResources actProp = TrickAction.extractProperties(getResourceMap(), name);
					actionMap.put(man.getName(), new TrickAction(actProp, o, man));
				}
			}

			actionMaps.put(o, actionMap);
		}

		return actionMap;
	}

	public TrickResources getResourceMap() {
		if (resources == null)
			parseResources();
		
		return resources;
	}

	public Session getSessionStorage() {
		return session;
	}

	public Session getLocalStorage() {
		return session;
	}

	public TaskService getTaskService() {
		return service;
	}

	/**
	 * Add the properties of the given component and any of its children to the resources
	 */
	public void injectComponents(Component root) {
		if (root == null) {
			throw new IllegalArgumentException("null component. can't inject");
		}

		injectProperties(root);

		if(root instanceof JMenu) 
			injectComponents(((JMenu) root).getMenuComponents());
		else if(root instanceof Container)
			injectComponents(((Container) root).getComponents());
	}

	private void injectComponents(Component[] components) {
		for (Component component : components) {
			injectComponents(component);
		}
	}

	private void injectProperties(Component component) {
		if (component == null) {
			throw new IllegalArgumentException("null component. can't inject properties");
		}
		String compName = component.getName();

		if(compName != null && resourceExists(compName)) {
			PropertyDescriptor[] pds = getComponentProperties(component);
			if (pds != null && pds.length > 0) {
				for (String key : resources.stringPropertyNames()) {
					int index = key.lastIndexOf(".");
					String keyComponent = (index < 0) ? null : key.substring(0, index);

					if (compName.equals(keyComponent)) {
						index++;

						if (index == key.length()) {
							String msg = "component resource does not have a property name";
							logger.warning(msg);
						} else {
							String propertyName = key.substring(index);
							
							for (PropertyDescriptor pd : pds) {
								String pd_name = pd.getName();
								if (pd_name.equals(propertyName)) {
									injectProperty(component, pd, key);
									return;
								}
							}

							String msg = String.format("Component '%s' does not have a Property '%s'.", compName, propertyName);
							logger.warning(msg);
						}
					}
				}
			}
		}
	}

	private void injectProperty(Component comp, PropertyDescriptor pd, String key) {
		// System.out.println("Injecting: " + key + " with " + resources.getString(key));
		TrickAction.configureMnemonic(comp, resources.getString(key));
	}

	private PropertyDescriptor[] getComponentProperties(Component comp) {
		try {
			BeanInfo info = Introspector.getBeanInfo(comp.getClass());
			return info.getPropertyDescriptors();
		} catch(IntrospectionException ie) {
			String msg = "introspection failed";
			System.err.println(msg);
			return null;
		}
	}

	private boolean resourceExists(String name) {
		for(String key : resources.stringPropertyNames()) {
			int index = key.lastIndexOf(".");
			if(index >= 0 && name.equals(key.substring(0, index)))
				return true;
		}

		return false;
	}
    
    private TrickResources parseResources(Class<? extends BaseApplication> app) throws IOException, FileNotFoundException {
		TrickResources prop;
		File resource;
		String path;
		
    	if(app.getSimpleName().equals("TrickApplication")) {
    		prop = new TrickResources();
    	} else {
    		Class superApp = app.getSuperclass();
    		prop = new TrickResources(parseResources(superApp));
		}
    		
    	path = TrickApplication.getResourcePath(app);
    	resource = new File(path);
    	prop.loadProperties(resource);
    	
    	int filePos = path.lastIndexOf("/") + 1;
    	path = path.substring(0, filePos);
    	if(prop.getProperty("PATH") != null)
	    	path += ";" + prop.getProperty("PATH");
    	
    	prop.setProperty("PATH", path);
    	
    	return prop;
    }

    private void parseResources() {
    	try {
	    	resources = parseResources(appClass);
    	} catch(IOException ioe) {
    		System.err.println(ioe.getMessage());
    	}
    }

	private boolean isSwingAction(Method man) { 
    	return man.getAnnotation(SwingAction.class) != null;
    }
}
