package trick.dataproducts.trickqp;

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
 * Test TrickQPApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class TrickQPApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForTrickQPApplication.launchAndWait(WaitForTrickQPApplication.class);
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
		assertTrue("TrickQPApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    @Test
    public void testDefinedActions() {
        String[] actionNames = {"searchVar", "newDP", "openDP", "refreshDP", "saveDP", "saveAsDP",
        		                "addVar", "expandVar", "contractVar", "changeUnits", "addRun", "removeRun",
        		                "newPage", "removeAllPages", "newPlot", "newCurve", "newVarcase",
        		                "newTable", "newColumn", "removeTable"};
        String[] actionTexts = {"Search", "New DP...", "Open DP...", "Refresh...", "Save...", "Save As...", "Add Var", 
        		                "Expand Var", "Contract Var", "Change Units...", "Add Run...", "Remove Run",
        		                "New Page", "Remove All Pages", "New Plot", "New Curve", "New Varcase",
        		                "New Table", "New Column", "Remove All Tables"};
        String[] actionShortDescriptions = {"Search Variables (all variables return if leave it blank)", 
        						            "Start a New DP", "Open an Existing DP File", "Refresh DP",
        		                            "Save Current DP", "Save Current DP As", "Add Selected Variables",
        		                            "Expand Selected Variables / All", "Contract Selected Variables / All",
        		                            "Change Units", "Add Run Directories", "Remove Selected Run Directories",
        		                            "Add a New Page", "Remove All Pages", "Add a New Plot", "Add a New Curve",
        		                            "Add One Variable Group Option for the Curve", "Add a New Table",
        		                            "Add a New Column to Table", "Remove All Tables"};       
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]); 
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);            
        }
    }
    
    @Test
    public void testDefinedKeyText() {
    	String[] keyNames = {"varsMenu.text", "runsMenu.text", "plotsMenu.text", "tablesMenu.text",
    			             "programsMenu.text", "settingsMenu.text", "actionsMenu.text", "helpMenu.text"};
    	String[] keyTexts = {"&Vars", "&Runs", "&Plots", "&Tables", "Pro&grams", "&Settings", "&Actions", "&Help"};
    	
    	for (int i = 0; i < keyNames.length; i++) {
    		CheckApplicationProperties.checkKeyText(application().resourceMap, keyNames[i], keyTexts[i]);
    	}
    }
	
	@Test
	public void testExit() {
		application().removeExitListener(application().exitListener);
		application().exit();
		assertTrue(application().isEnded);
	}
	
	private static WaitForTrickQPApplication application() {
        return Application.getInstance(WaitForTrickQPApplication.class);
    }
}