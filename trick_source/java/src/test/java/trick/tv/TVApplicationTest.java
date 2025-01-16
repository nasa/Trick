package trick.tv;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.launcher.ApplicationLauncher.application;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.ApplicationTest;
import trick.common.TestUtils;
import trick.tv.fixtures.TVFixture;

/**
 * 
 * Test TVApplication life cycle.
 * 
 * @author hchen
 *
 */
public class TVApplicationTest extends ApplicationTest {
	private static final int PORT = getOpenPort();
	private static final String SIM_NAME = "SIM_test_dr",
								GUI_TEST_INPUT = TestUtils.getTrickHome()
													+ "/trick_source/java"
													+ "/src/test/resources/RUN_gui_test/input.py";
	private static Process sim_process;

	private TVFixture tv_fix;
	private MockTVApplication tv_app;

	/**
	 * Before Running the Test Suite, prior to the constructor being called:
	 * Compiles and runs SIM_test_dr in the trick/test/ directory. 
	 *   - Change String SIM_NAME to compile & run another simulation in the 
	 * 	   trick/test/ folder
	 */
    @BeforeClass
    public static void onSetUpBeforeClass() {
		assumeThat(TestUtils.compileTestSim(SIM_NAME))
			.withFailMessage("Simulation \"%s\" Did Not Compile\n", SIM_NAME)
			.isTrue();
		
		sim_process = TestUtils.runTestSim(SIM_NAME, GUI_TEST_INPUT);
		assumeThat(sim_process)
			.withFailMessage("Simulation \"%s\" Did Not Start\n", SIM_NAME)
			.isNotNull();
    }

	/**
	 * After Running the Test Suite
	 * Kill the running simulation
	 */
	@AfterClass
	public static void onCleanUpAfterClass() {
		if(sim_process != null && sim_process.isAlive()) {
			sim_process.destroy();
		}
	}

	/**
	 * Set Up Before Each Test
	 * Start the application using the arguments to provide a host name and port number.
	 * Save the application instance in a variable
	 * Create a test fixture using the instance
	 */
    @Override
    protected void onSetUp() {        
        application(MockTVApplication.class)
			.withArgs("--host", "localhost",
					  "--port", "" + PORT)
			.start();
			
		sleep(2500);

		tv_app = MockTVApplication.getInstance(MockTVApplication.class);
		tv_fix = new TVFixture(robot(), tv_app);
    }

	/**
	 * Purpose: Select variables and verify that the expected values are in the table
	 * Procedures:
	 *   1) Select each of the mentioned variables
	 *   2) Compare the table values to the benchmark matrix SEL_VARS
	 */
    @Test
    public void testSelectVars() {
        // ARRANGE
        final String[][] SEL_VARS = { {	"drx.drt.a",			"97",	"1", "Decimal" },
									  {	"drx.drt.b",			"98",	"1", "Decimal" },
									  {	"drx.drt.c",			"-1234","1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"128",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"63",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"2",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"4",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"127",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"3",	"1", "Decimal" } };
        String[][] res_vars;

        // ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

        res_vars = tv_fix.getSelectedVars();

        // ASSERT
        assumeThat(res_vars.length)
			.withFailMessage("Unexpected number of rows.")
			.isEqualTo(SEL_VARS.length);

		for (int i = 0; i < res_vars.length; i++) {
			assumeThat(res_vars[i].length)
				.withFailMessage("Unexpected number of columns.")
				.isEqualTo(SEL_VARS[i].length);

			for (int j = 0; j < res_vars[0].length; j++) {
				assertThat(res_vars[i][j])
					.isEqualTo(SEL_VARS[i][j]);
			}
		}
    }

	/**
	 * Purpose: Do a basic test of the search panel to verify that it is enabled
	 * Procedure:
	 *   1) Use the search panel to find any variables that have 'var1' in their name.
	 *   2) Compare the search results with the benchmark SEARCH_VARS
	 */
    @Test
    public void testSearchVars() {
        // ARRANGE
        final String[] SEARCH_VARS = {  "drx.drt.uintB.var1"  , "drx.drt.intB.var1",
                                        "drx.drt.ucharB.var1" , "drx.drt.charB.var1",
                                        "drx.drt.ushortB.var1", "drx.drt.shortB.var1",
                                        "drx.drt.mixB.var1"  };
        String[] found_vars;
        assumeThat(tv_fix.getSearchResults()).isNull();

        // ACT
        tv_fix.enterQuery("var1\n");
        sleep(50);

        found_vars = tv_fix.getSearchResults();

        // ASSERT
        assertThat(found_vars.length)
            .withFailMessage("Unexpected number of selected variables.")
            .isEqualTo(SEARCH_VARS.length);

        for(int i = 0; i < SEARCH_VARS.length; i++)
            assertThat(found_vars[i]).isEqualTo(SEARCH_VARS[i]);
    }

	/**
	 * Purpose: Validate that variable values can be changed by editing the table
	 * Procedures:
	 *   1) Select each of the variables within SEL_VARS
	 *   2) Change the values of each of the selected variables
	 *   3) Verify that the table contains the expected values
	 *   4) Return the variables back to their original values
	 */
	@Test 
	public void testEditTable() {
		// ARRANGE
		final String[][] SEL_VARS = { {	"drx.drt.a",			"88",	"1", "Decimal" },
									  {	"drx.drt.b",			"88",	"1", "Decimal" },
									  {	"drx.drt.c",			"88",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"1",	"1", "Decimal" } };

		String[][] res_vars;
		String[] inital_values = new String[SEL_VARS.length];

		// ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

        for(int i = 0; i < SEL_VARS.length; i++){
			inital_values[i] = tv_fix.getCellValue(i, 1);
			tv_fix.editVariableTable(i, 1, SEL_VARS[i][1] + "\n");
			sleep(500);
        }

		res_vars = tv_fix.getSelectedVars();
		
		// ASSERT
        assertThat2DArraysAreEqual(res_vars, SEL_VARS);

		// CLEANUP
		for(int i = 0; i < inital_values.length; i++) {
			tv_fix.editVariableTable(i, 1, inital_values[i]);
			sleep(500);
		}
	}

	/**
	 * Purpose: Test the save functionality. Specifically that tied to the menu item.
	 * Procedures:
	 *   1) Select the list of variables
	 *   2) Click the Save menu item within the File menu
	 *   3) Enter the path where the new file will be saved to (see File SAVED_OUTPUT)
	 *   4) Compare the contents of the saved file to the benchmark file (see File EXPECTED_OUTPUT)
	 *      - If the contents aren't the same, copy the saved file to the FAILED_TEST file for debugging
	 */
    @Test
    public void testSave() {
        // ARRANGE
		final File EXPECTED_OUTPUT = new File("src/test/java/trick/tv/resources/tv_save_test.tv"),
				   SAVED_OUTPUT    = new File("test_results.tv"),
				   FAILED_TEST     = new File("failed_test.tv");
        final String[] SEL_VARS = { "drx.drt.a",	 "drx.drt.b",	  "drx.drt.c",
									"drx.drt.uintB.var1",     "drx.drt.intB.var1",
									"drx.drt.ucharB.var1",   "drx.drt.charB.var1",
									"drx.drt.ushortB.var1", "drx.drt.shortB.var1",
									"drx.drt.mixB.var1" };
		boolean file_written, file_expected;

		SAVED_OUTPUT.deleteOnExit();

		assumeThat(EXPECTED_OUTPUT).isNotNull();
		assumeThat(EXPECTED_OUTPUT.exists()).isTrue();

        // ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i]);
            sleep(250);
        }

        file_written = tv_fix.saveMenuItem(SAVED_OUTPUT);

        // ASSERT
		file_expected = sameFileContent(SAVED_OUTPUT, EXPECTED_OUTPUT);

        assertThat(file_written)
			.withFailMessage("Save operation failed. File not written\n")
			.isTrue();

        if (!file_expected) {
            try{
                Files.copy( SAVED_OUTPUT.toPath(), 
                            FAILED_TEST.toPath(), 
                            StandardCopyOption.REPLACE_EXISTING );
            } catch(Exception IGNORED) {}
        }

		assertThat(file_expected)
			.withFailMessage("Generated File doesn't match Benchmark. Failed .tv file is in java directory\n")
			.isTrue();
    }

	/**
	 * Purpose: Test the Open functionality. It should just add in the variables without overwriting the values
	 * Procedures:
	 *   1) Click the Open menu item
	 *   2) Enter the path where the file will be loaded from (see File SAVED_FILE)
	 *   3) Compare the table after loading to the expected in SEL_VARS.
	 */
    @Test
    public void testLoadVariables() {
        // ARRANGE
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_load_test.tv");
        final String[][] SEL_VARS = { {	"drx.drt.a",			"97",	"1", "Decimal" },
									  {	"drx.drt.b",			"98",	"1", "Decimal" },
									  {	"drx.drt.c",			"-1234","1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"128",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"63",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"2",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"4",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"127",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"3",	"1", "Decimal" } };

		String[][] res_vars;

        // ACT
		tv_fix.openMenuItem(SAVED_FILE);

		sleep(500);

		res_vars = tv_fix.getSelectedVars();

        // ASSERT
        assertThat2DArraysAreEqual(res_vars, SEL_VARS);
    }

	/**
	 * Purpose: Test the Open & Set menu item. It should update the selected variables and their values
	 * Procedures:
	 *   1) Click the Open & Set menu item
	 *   2) Open the file specified by SAVED_FILE
	 *   3) Compare the table after loading to the expected in SEL_VARS. 
	 * 		- The variable values will be different from the default simulation values
	 *   4) Use the Open & Set menu item to reload the orignal values using the ORIGINAL_VALS_FILE variable
	 */
    @Test
    public void testLoadVariables_SetValues() {
        // ARRANGE
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_load_test.tv"),
				   ORIGINAL_VALS_FILE = new File("src/test/java/trick/tv/resources/tv_save_test.tv");
		final String[][] SEL_VARS = { {	"drx.drt.a",			"88",	"1", "Decimal" },
									  {	"drx.drt.b",			"88",	"1", "Decimal" },
									  {	"drx.drt.c",			"88",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"1",	"1", "Decimal" } };

		String[][] res_vars;

        // ACT
		tv_fix.openSetMenuItem(SAVED_FILE);
		sleep(500);

		res_vars = tv_fix.getSelectedVars();

        // ASSERT
        assertThat2DArraysAreEqual(res_vars, SEL_VARS);

		// CLEANUP
		tv_fix.openSetMenuItem(ORIGINAL_VALS_FILE);
		sleep(500);
    }

	/**
	 * Purpose: Test the Set menu item. It should set variable values without changing the selected variables
	 * Procedures:
	 *   1) Click the Set menu item
	 *   2) Use SAVED_FILE to load the values
	 *   3) Record the state of the variable table immediately after loading the file (res_vars1)
	 *   4) Select the variables within SEL_VARS
	 *   5) Verify that res_vars1 is empty
	 *   6) Verify that the variable values have changed to the expected
	 */
    @Test
    public void testSetValues() {
        // ARRANGE
		final File SAVED_FILE = new File("src/test/java/trick/tv/resources/tv_load_test.tv"),
				   ORIGINAL_VALS_FILE = new File("src/test/java/trick/tv/resources/tv_save_test.tv");
		final String[][] SEL_VARS = { {	"drx.drt.a",			"88",	"1", "Decimal" },
									  {	"drx.drt.b",			"88",	"1", "Decimal" },
									  {	"drx.drt.c",			"88",	"1", "Decimal" },
                                      {	"drx.drt.uintB.var1",	"28",	"1", "Decimal" },
									  {	"drx.drt.intB.var1",	"28",	"1", "Decimal" },
                                      {	"drx.drt.ucharB.var1",	"3",	"1", "Decimal" },
									  {	"drx.drt.charB.var1",	"-1",	"1", "Decimal" },
                                      {	"drx.drt.ushortB.var1",	"14",	"1", "Decimal" },
									  {	"drx.drt.shortB.var1",	"78",	"1", "Decimal" },
                                      {	"drx.drt.mixB.var1",	"1",	"1", "Decimal" } };

		String[][] res_vars1, res_vars2;

        // ACT
		tv_fix.setValMenuItem(SAVED_FILE);
		sleep(500);

		res_vars1 = tv_fix.getSelectedVars();

        for(int i = 0; i < SEL_VARS.length; i++){
            tv_fix.selectVar(SEL_VARS[i][0]);
            sleep(250);
        }

		res_vars2 = tv_fix.getSelectedVars();

        // ASSERT
		assertThat(res_vars1.length)
			.withFailMessage("Variable Table has unexpected Entries: %s\n", Arrays.deepToString(res_vars1))
			.isEqualTo(0);
			
        assertThat2DArraysAreEqual(res_vars2, SEL_VARS);

		// CLEANUP
		tv_fix.setValMenuItem(ORIGINAL_VALS_FILE);
		sleep(500);
    }

	/**
	 * Purpose: Test the toggle for turning monitoring on and off
	 * Procedures: 
	 *   1) Toggle monitoring off
	 *   2) Select variable "drx.drt.a"
	 *   3) Save the variable entry in the array var_no_monitor
	 *   4) Toggle monitoring back on
	 *   5) Save the updated entry in the array var_monitor
	 *   6) compare both arrays to their expected benchmarks
	 */
	@Test 
	public void testMonitorToggle() {
		// ARRANGE 
		final String[] EXP_VAR_MONITOR   = { "drx.drt.a", "97", "1", "Decimal" },
					   EXP_VAR_NO_MONITOR = { "drx.drt.a", "<Unknown>", "1", "Decimal" };
		String[] var_no_monitor, var_monitor;

		// ACT 
		tv_fix.clickMonitorToggleMenuItem();
		tv_fix.selectVar(EXP_VAR_MONITOR[0]);
		sleep(250);

		var_no_monitor = tv_fix.getSelectedVars()[0];
		tv_fix.clickMonitorToggleMenuItem();
		sleep(500);

		var_monitor = tv_fix.getSelectedVars()[0];

		// ASSERT
		assertThat(var_monitor.length).isEqualTo(EXP_VAR_MONITOR.length);
		assertThat(var_no_monitor.length).isEqualTo(EXP_VAR_NO_MONITOR.length);

		for(int i = 0; i < EXP_VAR_MONITOR.length; i++) {
			assertThat(var_no_monitor[i]).isEqualTo(EXP_VAR_NO_MONITOR[i]);
			assertThat(var_monitor[i]).isEqualTo(EXP_VAR_MONITOR[i]);
		}

	}

	/**
	 * Find a free port, write the number to a file called port.info
	 * within the RUN_gui_test directory. Return the chosen port
	 */
	public static int getOpenPort() {
		String port = "39595";
		File port_info;
		ServerSocket temp = null;
		PrintWriter info_writer = null;
		
		try {
			temp = new ServerSocket(0);
			port_info = new File("src/test/resources/RUN_gui_test/port.info");
			info_writer = new PrintWriter(port_info, "UTF-8");

			port = "" + temp.getLocalPort();
			info_writer.write(port);
		} catch(IOException e) {
			System.err.println("Free Port Not Found");
		} finally {
			try {
				if(temp != null)
					temp.close();
				if(info_writer != null)
					info_writer.close();
			} catch(IOException e) {
				System.err.println("Issue with Cleanup.");
			}
		}

		return Integer.parseInt(port);
	}
}