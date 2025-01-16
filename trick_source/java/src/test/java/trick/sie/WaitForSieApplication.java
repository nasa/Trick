package trick.sie;


import org.jdesktop.application.Application;

import trick.sie.SieApplication;


public class WaitForSieApplication extends SieApplication {
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
     * Launch the specified subclsas of SieApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForSieApplication> applicationClass) {
    	synchronized(lock) {
			// Set path to S_sie.resource (src/test/resources/S_sie.resource)
			final String sep = java.io.File.separator;
			sieResourcePath = String.format("src%1$stest%1$sresources%1$sS_sie.resource", sep);
            
    		Application.launch(applicationClass, new String[]{});
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(WaitForSieApplication.class);
    			if (app instanceof WaitForSieApplication) {
    				if (((WaitForSieApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}