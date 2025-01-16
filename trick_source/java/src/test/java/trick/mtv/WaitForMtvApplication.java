package trick.mtv;


import org.jdesktop.application.Application;


public class WaitForMtvApplication extends MtvApp {
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
     * Launch the specified subclsas of MtvApp and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForMtvApplication> applicationClass) {
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
    			Application app = Application.getInstance(WaitForMtvApplication.class);
    			if (app instanceof WaitForMtvApplication) {
    				if (((WaitForMtvApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}