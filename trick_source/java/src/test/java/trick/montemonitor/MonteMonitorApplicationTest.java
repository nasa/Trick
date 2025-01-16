package trick.montemonitor;

import static org.junit.Assert.assertTrue;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.CheckApplicationProperties;

/**
 * 
 * Test MonteMonitorApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class MonteMonitorApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForMonteMonitorApplication.launchAndWait(WaitForMonteMonitorApplication.class);
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {		
	}

	@After
	public void tearDown() throws Exception {
	}
	
	@Test
	public void testReady() {		
		assertTrue("MonteMonitorApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    /*@Test
    public void testDefinedActions() {
        String[] actionNames = {"stopMonteCarlo", "runMonteCarlo", "runSelectedSlaves", "stopSelectedSlaves", "addNewSlave"};
        String[] actionTexts = {"Stop", "Start", "Start", "Stop", "Add"};
        String[] actionShortDescriptions = {"Dispatch no further runs to any slave. Allow current runs to complete.",
        		                            "Begin dispatching runs to all live slaves.",
        		                            "Begin dispatching runs to all live selected slaves.",
        		                            "Dispatch no further jobs to the selected slaves. Allow current runs to complete.",
        		                            "Add and start a new slave."};
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]);
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);
        }
    }*/
    
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");
    	//CheckApplicationProperties.checkKeyText(application().resourceMap, "monteMenu.text", "&Monte Carlo");
    	//CheckApplicationProperties.checkKeyText(application().resourceMap, "slavesMenu.text", "&Slaves");
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForMonteMonitorApplication application() {
        return Application.getInstance(WaitForMonteMonitorApplication.class);
    }
}