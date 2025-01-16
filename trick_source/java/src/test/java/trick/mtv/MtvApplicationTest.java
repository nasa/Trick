package trick.mtv;

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
 * Test MtvApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class MtvApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForMtvApplication.launchAndWait(WaitForMtvApplication.class);
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
		assertTrue("MtvApplicationTest is not ready yet!", application().isReady());
	}
	
	
    @Test
    public void testDefinedKeyText() {
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "fileMenu.text", "&File");
    	CheckApplicationProperties.checkKeyText(application().resourceMap, "default_event_directory", "./Modified_data");
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForMtvApplication application() {
        return Application.getInstance(WaitForMtvApplication.class);
    }
}