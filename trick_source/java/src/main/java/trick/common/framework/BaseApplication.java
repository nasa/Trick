
//========================================
//	Package
//========================================
package trick.common.framework;

import java.awt.event.ActionEvent;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.HierarchyEvent;
import java.awt.event.HierarchyListener;
import java.awt.event.PaintEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.ActiveEvent;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Frame;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.Window;

import java.beans.Beans;
import java.beans.PropertyChangeSupport;
import java.io.IOException;
import java.lang.IllegalAccessException;
import java.lang.InstantiationException;
import java.lang.NoSuchMethodException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.security.InvalidParameterException;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.EventObject;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JRootPane;
import javax.swing.RootPaneContainer;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;
import javax.swing.event.SwingPropertyChangeSupport;

import trick.common.framework.AppContext;
import trick.common.framework.SwingToolbox;
import trick.common.framework.task.Task;
import trick.common.framework.View;

public abstract class BaseApplication {

	//========================================
	//	Attributes
	//========================================
	
	// -- PUBLIC -----------------------------

	// -- PRIVATE ----------------------------
	private View mainView;
	private List<ExitListener> exitListeners;

	private final AppContext context;
	private final SetUpCloseOperations MAIN_FRAME_SETUP = (Container c) -> initMainFrameClose();
	private final SetUpCloseOperations WINDOW_SETUP = (Container c) -> initWindowClose(c);
	
	private static BaseApplication application = null;

	private static final Logger logger = Logger.getLogger(BaseApplication.class.getName());
	private static final String INITIALIZATION_MARKER="BaseApplication.initRootPaneContainer";
	private static final String WINDOW_STATE_NORMAL_BOUNDS = "WindowState.normalBounds";

	// -- PROTECTED --------------------------
    protected boolean ready;
	protected PropertyChangeSupport changeSupport;

	//========================================
	//	Constructors
	//========================================
	protected BaseApplication() {
		exitListeners = new CopyOnWriteArrayList<ExitListener>();
		context = new AppContext(getClass());
		changeSupport = new SwingPropertyChangeSupport(this, true);
	}

	//========================================
	//	Public Methods
	//========================================

	public static void launch(Class<? extends BaseApplication> c, String[] args) {
		Runnable doCreateAndShowGUI = new GUIDisplayRunner(c, args);
		SwingUtilities.invokeLater(doCreateAndShowGUI);
	}

	public void show(JDialog d) {
		if(d != null) {
			initRootPaneContainer(d);
			d.setVisible(true);
		}
	}

	public void show(View v) {
		RootPaneContainer comp = (RootPaneContainer) v.getRootPane().getParent();
		initRootPaneContainer(comp);
		((Window) comp).setVisible(true);
	}

	public void exit() {  exit(null);  }

	public void exit(final EventObject event) {
		Runnable exitHandler = new ExitHandler(event);

		if (SwingUtilities.isEventDispatchThread()) {
			exitHandler.run();
		} else {
			try { SwingUtilities.invokeAndWait(exitHandler); }
			catch (Exception ignore) {}
		}
	}

	public void addExitListener(ExitListener listener) {
		exitListeners.add(listener);
	}

	public void removeExitListener(ExitListener listener) {
		exitListeners.remove(listener);
	}

	public void saveSession(Window win) {
		String filename = SwingToolbox.getSessionFileName(win);
		Session storage = context.getLocalStorage();
		
		if(filename == null) throw new InvalidParameterException("Null Window");

		try {  
			storage.save(win, filename);
		} catch (IOException e) {
			String msg = "couldnt save session to '" + filename + "'";
			logger.log(Level.WARNING, msg, e);
		}

	}

	//========================================
	//	Protected Methods
	//========================================
	
	protected void configureWindow(Window root) {
		context.injectComponents(root);
	}

	protected void end() {
		Runtime.getRuntime().exit(0);
	}

	//========================================
	//	Private Methods
	//========================================

	private static <T extends BaseApplication> T create(Class<T> appClass) {
		// AppContext ctx;

		// TODO: Set up System property
		/*	if (!Beans.isDesignTime()) {
					
				try {
					System.setProperty("java.net.useSystemProxies", "true");
				} catch (SecurityException ignoreException) {
					// Unsigned apps can't set this property. 
				}
			}
		*/

		// Create an instance of <T>
		T app;

		try {
			app = createInstanceOf(appClass);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			return null;
		}

		AppContext ctx = app.getContext();

		// TODO: Add platform info to resource map
		/*	ResourceMap appResourceMap = ctx.getResourceMap();
			final PlatformType platform = AppHelper.getPlatform();
			appResourceMap.putResource(ResourceMap.KEY_PLATFORM, platform);
		*/

		//TODO: Generic registration with the Mac OS X application menu
		/*	if (PlatformType.OS_X.equals(platform)) {
				try {
					OSXAdapter.setQuitHandler(application, Application.class.getDeclaredMethod("handleQuit", (Class[])null));
				} catch (Exception e) {
					logger.log(Level.SEVERE, "Cannot set Mac Os X specific handler for Quit event", e);
				}
			}
		*/
		
		// Set up Look and Feel
		String lnfKey = "Application.lookAndFeel",
			   lnfVal, lnfName;

		if (!Beans.isDesignTime()) {
			lnfVal = ctx.getResourceMap().getString(lnfKey);
			lnfVal = (lnfVal == null) ? "system" : lnfVal;
			lnfName = lnfVal;

			try {
				if (lnfVal.equalsIgnoreCase("system")) {
					lnfName = UIManager.getSystemLookAndFeelClassName();
				} else if (lnfVal.equalsIgnoreCase("nimbus")) {    
					for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
						if ("Nimbus".equals(info.getName())) {
							lnfName = info.getClassName();
							break;
						}
					}
				}
				
				if (!lnfVal.equalsIgnoreCase("default"))
					UIManager.setLookAndFeel(lnfName);
				
			} catch (Exception e) {
				String s = "Couldn't set LookandFeel " + lnfKey + " = \"" + lnfVal + "\"";
				logger.log(Level.WARNING, s, e);
			}
		}
		

		return app;
	}

	private void initRootPaneContainer(RootPaneContainer c) {
		SetUpCloseOperations closeOps = WINDOW_SETUP;
		if(c == getMainFrame())  closeOps = MAIN_FRAME_SETUP;

		Container root = c.getRootPane().getParent();
		Window window = safe_cast(root, Window.class);

		try {  initializeOnce(c);  }
		catch(IllegalArgumentException ignored) {  return;  }

		closeOps.setupClose(root);
		handleContainerBounds(root);

		if (window != null) {
			configureWindow(window);
			reloadContainerState(window);
			centerWindow(window);
		}
		
		
	}

	//========================================
	//	Helper Methods
	//========================================

	private static <T> T createInstanceOf(Class<T> cls)  throws NoSuchMethodException, 
																InstantiationException, 
																IllegalAccessException, 
																InvocationTargetException {
		Constructor<T> clsCtor = cls.getDeclaredConstructor();

		if(!clsCtor.isAccessible()) {
			try { clsCtor.setAccessible(true); }
			catch (SecurityException UNUSED) { }
		}

		return clsCtor.newInstance();
	}

	private static boolean appIsLaunched() {  return application != null;  }

	private void initializeOnce(RootPaneContainer c) {
		JComponent rootPane = c.getRootPane();
		Object prop = rootPane.getClientProperty(INITIALIZATION_MARKER);
		
		if (prop != null)
			throw new IllegalArgumentException("Container already initialized.");
		
		rootPane.putClientProperty(rootPane, Boolean.TRUE);
	}

	private <T> T safe_cast(Object o, Class<T> c) {
		if(c.isInstance(o)) return c.cast(o);
		else return null;
	}

	private void initMainFrameClose() {
		JFrame mf = getMainFrame();

		mf.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
				exit(e);
			}	
		});

		mf.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
	}

	private void initWindowClose(Component c) {
		Window window = safe_cast(c, Window.class);
		window.addHierarchyListener(new HierarchyListener() {
			@Override
			public void hierarchyChanged(HierarchyEvent event) {
				boolean showing_changed = (event.getChangeFlags() & HierarchyEvent.SHOWING_CHANGED) != 0;
				if (showing_changed) {
					Window secondaryWindow = safe_cast(event.getSource(), Window.class);
					if(secondaryWindow != null && !secondaryWindow.isShowing()) {
						saveSession(secondaryWindow);
					}
				}
			}
		});
	}

	private void handleContainerBounds(Container comp) {
		Window wind = safe_cast(comp, Window.class);

		if(safe_cast(comp, JFrame.class) != null) {
			handleJFrameBounds(comp);
		}

		if (wind != null) {
			boolean bounds_not_set =  !comp.isValid() 
									|| comp.getWidth() == 0 
									|| comp.getHeight() == 0;
			String filename = SwingToolbox.getSessionFileName(wind);

			if (bounds_not_set)  wind.pack();

			try {
				if(filename != null) {
					context.getSessionStorage().restore(comp, filename);
				}
			} catch (Exception e) {
				// TODO: handle exception
			}
		}
	}

	private void handleJFrameBounds(Container comp) {
		comp.addComponentListener(new ComponentListener() {
			@Override
			public void componentResized(ComponentEvent e) {
				JFrame frame = safe_cast(e.getComponent(), JFrame.class);
				boolean both_not_maxxed = (frame.getExtendedState() & Frame.MAXIMIZED_BOTH) == 0;
				if (both_not_maxxed) {
					JRootPane rootPane = frame.getRootPane();
					rootPane.putClientProperty(WINDOW_STATE_NORMAL_BOUNDS, frame.getBounds());
				}
			}

			@Override
			public void componentMoved(ComponentEvent e) {}

			@Override
			public void componentHidden(ComponentEvent e) {}

			@Override
			public void componentShown(ComponentEvent e) {}
		});
	}

	private void reloadContainerState(Window w) {
		String filename;
		
		if((filename = SwingToolbox.getSessionFileName(w)) != null) {
			try {
				Session session = context.getSessionStorage();
				session.restore(w, filename);
			} catch (Exception e) {
				String msg = "Failed to restore session [%s]";
				logger.log(Level.WARNING, String.format(msg, filename), e);
			}
		}
	}

	private void centerWindow(Window w) {
		Point defLoc = SwingToolbox.defaultLocation(w),
			  actLoc = w.getLocation();

		if (!w.isLocationByPlatform() && actLoc.equals(defLoc)) {
			Dimension screenDim = w.getToolkit().getScreenSize(),
					  windowDim = w.getSize();
			double widthRatio = screenDim.getWidth() / windowDim.getWidth(),
				   heightRatio = screenDim.getHeight() / windowDim.getHeight();

			if(widthRatio > 1.25 && heightRatio > 1.25) {
				Component owner = w.getOwner();

				if (owner == null && getMainFrame() != w) {
					owner = getMainFrame();
				}

				w.setLocationRelativeTo(owner);
			}
		}
	}

	private void waitForReady() {
		new WaitTillFree().execute();
	}

	//========================================
	//	Getters/Setters
	//========================================

	public static BaseApplication getInstance() throws IllegalStateException {
		return getInstance(BaseApplication.class);
	}

	public static <T extends BaseApplication> T getInstance(Class<T> appClass) throws IllegalStateException {
		if (Beans.isDesignTime() && application == null) {
			application = create(appClass);
		}

		if (appIsLaunched()) {
			return appClass.cast(application);
		} else {
			throw new IllegalStateException("Application did not launch...");
		}
	}

	public JFrame getMainFrame() {
		return getMainView().getFrame();
	}

	public void setMainFrame(JFrame nFrame) {
		if(mainView != null)
			mainView.setFrame(nFrame);
	}

	public View getMainView() {
		if (mainView == null) {
			mainView = new View(this);
		}
		return mainView;
	}

	public void setMainView(View nView) {
		mainView = nView;
	}

	public final AppContext getContext() {
		return context;
	}

	public ExitListener[] getExitListeners() {
		int size = exitListeners.size();
		ExitListener[] el_array = exitListeners.toArray(new ExitListener[size]);

		return el_array;
	}

	//========================================
	//	Undefined Methods
	//========================================

	protected void initialize(String[] args) {}
	protected void ready() {}
	protected void shutdown() {}

	protected abstract void startup();

	//========================================
	//	Inner Classes
	//========================================

	public abstract class ExitListener {
		public abstract boolean canExit(EventObject e);
		public abstract void willExit(EventObject e);
	}

	private interface SetUpCloseOperations {
		public void setupClose(Container c);
	}

	private static class GUIDisplayRunner implements Runnable {
		private final Class appClass;
		private final String[] args;

		public GUIDisplayRunner(Class c, String[] args) {
			this.appClass = c;
			this.args = args;
		}

		@Override
		public void run() {
			try {
				application = create(appClass);
				application.initialize(args);
				application.startup();
				application.waitForReady();
			} catch (Exception e) {
				String msg = appClass.getName() + " failed to launch";
				logger.log(Level.SEVERE, msg, e);
				throw new Error(msg, e);
			}
		}
	}

	private class ExitHandler implements Runnable {
		private final EventObject event;

		public ExitHandler(EventObject ev) {
			event = ev;
		}

		@Override 
		public void run() {
			if(allowedToExit()) {
				try {
					exitAll();
					shutdown();
				} catch (Exception e) {
					logger.log(Level.WARNING, "Unexpected shutdown error", e);
				} finally {
					end();
				}
			}
		}

		private boolean allowedToExit() {
			for (ExitListener exitListener : exitListeners)
				if (!exitListener.canExit(event))
					return false;
			
			return true;
		}

		private void exitAll() {
			for (ExitListener exitListener : exitListeners) {
				try {
					exitListener.willExit(event);
				} catch (Exception e) {
					logger.log(Level.WARNING, "ExitListener.willExit() failed", e);
				}
			}
		}
	}

	private void waitForEmptyQueue(JPanel panel) {
		EventQueue Q = Toolkit.getDefaultToolkit().getSystemEventQueue();
		boolean emptyQ = false;

		while(!emptyQ) {
			QueueEvent e = new QueueEvent(panel);

			Q.postEvent(e);

			synchronized(e) {
				while(!e.isDispatched()) {
					try { e.wait(); }
					catch (InterruptedException ignored) {}
				}

				emptyQ = e.isQueueEmpty();
			}
		}
	}

	private class WaitTillFree extends Task<Void, Void> {
		// private boolean emptyQ = false;
		private final JPanel panel;

		WaitTillFree() {
			super(BaseApplication.this);

			panel = new JPanel();
		}

		@Override
        protected Void doInBackground() {
			waitForEmptyQueue(panel);
			return null;
		}

		@Override
		protected void finished() {
			ready = true;
			ready();
		}

		
	}

	private class QueueEvent extends PaintEvent implements ActiveEvent {
		
		private boolean dispatched = false;
		private boolean empty = false;

		QueueEvent(Component c) {
			super(c, PaintEvent.UPDATE, null);
		}

		@Override
		public void dispatch() {
			EventQueue Q = Toolkit.getDefaultToolkit().getSystemEventQueue();
			synchronized(this) {
				empty = Q.peekEvent() == null;
				dispatched = true;
				notifyAll();
			}
		}

		synchronized boolean isDispatched() { return dispatched; }
		synchronized boolean isQueueEmpty() { return empty; }
	}
}
