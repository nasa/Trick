package trick.dataproducts.trickqp;


import org.jdesktop.application.Application;


public class WaitForTrickQPApplication extends TrickQPApplication {
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
     * Launch the specified subclass of TrickDPApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForTrickQPApplication> applicationClass) {
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
    			Application app = Application.getInstance(WaitForTrickQPApplication.class);
    			if (app instanceof WaitForTrickQPApplication) {
    				if (((WaitForTrickQPApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}