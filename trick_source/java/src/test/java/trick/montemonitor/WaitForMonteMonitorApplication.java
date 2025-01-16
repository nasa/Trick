package trick.montemonitor;


import org.jdesktop.application.Application;


public class WaitForMonteMonitorApplication extends MonteMonitorApplication {
    static Object lock = new Object(); 
    
    boolean isEnded;
    
    @Override
    protected void end() {
    	isEnded = true;
    }
  
    @Override
    protected void ready() {
    	super.ready();
    	synchronized(lock) {
    		lock.notifyAll();
    	}
    }
    

    /**
     * Launch the specified subclsas of MonteMonitorApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForMonteMonitorApplication> applicationClass) {
    	synchronized(lock) {   		
    		Application.launch(applicationClass, new String[]{});
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(WaitForMonteMonitorApplication.class);
    			if (app instanceof WaitForMonteMonitorApplication) {
    				if (((WaitForMonteMonitorApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}