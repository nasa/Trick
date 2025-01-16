package trick.tv;


import org.jdesktop.application.Application;


public class MockTVApplication extends TVApplication {
    private static Object lock = new Object(); 
    
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
    
    public static void main(String[] args) {
        synchronized(lock) {   		
    		Application.launch(MockTVApplication.class, args);
    		while(true) {
    			try {
    				lock.wait();
    			}
    			catch (InterruptedException e) {
    				System.err.println("launchAndWait interrupted!");
    				break;
    			}
    			Application app = Application.getInstance(MockTVApplication.class);
    			if (app instanceof MockTVApplication) {
    				if (((MockTVApplication) app).isReady()) {  
    					break;
    				}
    			}
    		}
    	}
	}
}