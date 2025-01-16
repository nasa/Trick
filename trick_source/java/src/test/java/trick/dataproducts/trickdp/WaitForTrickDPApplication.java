package trick.dataproducts.trickdp;


import org.jdesktop.application.Application;


public class WaitForTrickDPApplication extends TrickDPApplication {
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
    public static void launchAndWait(Class<? extends WaitForTrickDPApplication> applicationClass) {
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
    			Application app = Application.getInstance(WaitForTrickDPApplication.class);
    			if (app instanceof WaitForTrickDPApplication) {
    				if (((WaitForTrickDPApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}