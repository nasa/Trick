package trick.sniffer;

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
 * Test SnifferApplication life cycle.
 * 
 * @author hchen
 *
 */
public class SnifferApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForSnifferApplication.launchAndWait(WaitForSnifferApplication.class);
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
		assertTrue("SnifferApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
   /* @Test
    public void testDefinedActions() {
        String[] actionNames = {"launchSimControlPanel", "launchTrickView"};
        String[] actionTexts = {"Launch Sim Control Panel", "Launch Trick View"};
        String[] actionShortDescriptions = {"Launch the Sim Control Panel and connect it to the selected simulation.",
        		                            "Launch Trick View and connect it to the selected simulation."};
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]);
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);
        }
    }*/
    
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");    	
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForSnifferApplication application() {
        return Application.getInstance(WaitForSnifferApplication.class);
    }
}