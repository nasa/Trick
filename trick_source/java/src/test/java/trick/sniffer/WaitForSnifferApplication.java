package trick.sniffer;


import org.jdesktop.application.Application;


public class WaitForSnifferApplication extends SimSnifferApplication {
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
     * Launch the specified subclsas of SnifferApplication and block
     * (wait) until it's startup() method has run.
     */
    public static void launchAndWait(Class<? extends WaitForSnifferApplication> applicationClass) {
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
    			Application app = Application.getInstance(WaitForSnifferApplication.class);
    			if (app instanceof WaitForSnifferApplication) {
    				if (((WaitForSnifferApplication)app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
    }
}