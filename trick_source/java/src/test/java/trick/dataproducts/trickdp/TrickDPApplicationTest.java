package trick.dataproducts.trickdp;

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
 * Test TrickDPApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class TrickDPApplicationTest {
		
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
		WaitForTrickDPApplication.launchAndWait(WaitForTrickDPApplication.class);
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
		assertTrue("TrickDPApplicationTest is not ready yet!", application().isReady());
	}
	
	/**
     * Verify that all implemented actions exist as well as their text, shortDescription properties.
     */
    @Test
    public void testDefinedActions() {
        String[] actionNames = {"newSession", "openSession", "saveSession", "refreshSession", "importSimDir", "addRunDir",
        		                "addDP", "editSelectedDP", "editRightClickedDP", "filterDP", "gnuSinglePlot", "gnuComparisonPlot",
        		                "gnuErrorPlot", "quickPlot", "createPDF", "addRuns", "readDPList", "openSelected",
        		                "closeSelected", "removeSelectedNodes", "addDPs", "removeSelectedItems", "removeAllItems", "runSim",
        		                "refreshSelected"};
        String[] actionTexts = {"New...", "Open...", "Save...", "Refresh...", "Import Sim Dir...", "Add Run Dir...", "Add DP...", 
        		                "Edit DP...", "Edit DP...", "Filter...", "GNUplot Postscript Single Plot...",
        		                "GNUplot Postscript Comparison Plot...", "GNUplot Postscript Error Plot...", 
        		                "Quickplot...", "Create PDF Booklet...", "Add run(s)", "Read DP List", "Opentree",
        		                "Closetree", "Remove", "Add DPs", "Remove", "Remove All", "Run Sim", "Refresh"};
        String[] actionShortDescriptions = {"Start a New Session", "Open an Existing Session", "Save Current Session",
        		                            "Refresh Current Session", "Import Sim Directory to Current Session", 
        		                            "Add a Run Directory to Current Session", "Add a Data Product file to Current Session",
        		                            "Open Quickplot to edit the Selected Data Product from DP Selections", "Open Quickplot to edit the Selected Data Product file", 
        		                            "Filter the Displayed Data Product files", 
        		                            "Show GNU Postscript Single Plot", "Show GNU Postscript Comparison Plot",
        		                            "Show GNU Postscript Error Plot", "Quick Plot", "Create PDF Booklet...", "Selects run(s)",
        		                            "Add DP List in Selected SIM Directory", "Open Selected Tree Nodes", "Close Selected Tree Nodes",
        		                            "Remove Selected Node(s)", "Add Selected to DP List", "Remove Selected List Item(s)",
        		                            "Remove All List Items", "Run Selected Sim", "Refresh the Selected Nodes of the Tree"};
        
        for (int i = 0; i < actionNames.length; i++) {            
        	CheckApplicationProperties.checkAction(application().actionMap, actionNames[i]); 
        	CheckApplicationProperties.checkActionText(application().actionMap, actionNames[i], actionTexts[i]);
        	CheckApplicationProperties.checkActionShortDescription(application().actionMap, actionNames[i], actionShortDescriptions[i]);            
        }             
    }
    
    @Test
    public void testDefinedKeyText() {
    	String[] keyNames = {"fileMenu.text", "sessionMenu.text", "simsRunsMenu.text", "dataProductMenu.text",
    			             "settingsMenu.text", "actionsMenu.text", "helpMenu.text"};
    	String[] keyTexts = {"&File", "&Session", "Sims/&Runs", "&Data Product", "Se&ttings", "&Actions", "&Help"};
    	
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
	
	private static WaitForTrickDPApplication application() {
        return Application.getInstance(WaitForTrickDPApplication.class);
    }
}