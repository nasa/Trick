package trick.dre;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.launcher.ApplicationLauncher.application;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.ApplicationTest;
import trick.dre.fixtures.DreFixture;
import static trick.dre.fixtures.DreFixture.ALWAYS;
import static trick.dre.fixtures.DreFixture.ASCII;
import static trick.dre.fixtures.DreFixture.BINARY;
import static trick.dre.fixtures.DreFixture.BUFFER;
import static trick.dre.fixtures.DreFixture.CHANGES;
import static trick.dre.fixtures.DreFixture.HDF5;
import static trick.dre.fixtures.DreFixture.NO_BUFFER;
import static trick.dre.fixtures.DreFixture.RING_BUFFER;
import static trick.dre.fixtures.DreFixture.SINGLE_PREC_OFF;
import static trick.dre.fixtures.DreFixture.SINGLE_PREC_ON;
import static trick.dre.fixtures.DreFixture.STEP;
import trick.dre.fixtures.DreFixture.Size;

/**
 * 
 * Test DreApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class DreApplicationTest extends ApplicationTest {

    private DreFixture dre_fix;
    private MockDreApplication dre_app;

    @BeforeClass
    public static void onSetUpBeforeClass() {

    }

    @Override
    protected void onSetUp() {       
        application(MockDreApplication.class).start();

        sleep(2000);

        dre_app = MockDreApplication.getInstance(MockDreApplication.class);
        dre_fix = new DreFixture(robot(), dre_app);
    }

    /**
     * Purpose: Test changing the selected format 
     * Procedures:
     *   1) Select ASCII and save the selected options within ascii_sel
     *   2) Select HDF5 and save the selected options within hdf5_sel
     *   3) Select Binary and save the selected options within binary_sel
     *   4) Verify that each of the saved selected options equals the 
     *      expected benchmarks (BINARY_EXP, ASCII_EXP, HDF5_EXP)
     */
    @Test
    public void testFormatOptions() {
        // ARRANGE
        final int BINARY_EXP = BINARY | ALWAYS | BUFFER | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  ASCII_EXP  = ASCII  | ALWAYS | BUFFER | SINGLE_PREC_OFF,
                  HDF5_EXP   = HDF5   | ALWAYS | BUFFER | SINGLE_PREC_OFF;

        int init_opts = dre_fix.getSelectedOptions(),
            binary_sel, ascii_sel, hdf5_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(BINARY_EXP);
        
        // ACT
        dre_fix.setOptions(ASCII);
        ascii_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(HDF5);
        hdf5_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(BINARY);
        binary_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(binary_sel).isEqualTo(BINARY_EXP);
        assertThat(ascii_sel).isEqualTo(ASCII_EXP);
        assertThat(hdf5_sel).isEqualTo(HDF5_EXP);
    }

    /**
     * Purpose: Test changing the selected frequency 
     * Procedures:
     *   1) Select Changes and save the selected options within changes_sel
     *   2) Select Step and save the selected options within step_sel
     *   3) Select Always and save the selected options within always_sel
     *   4) Verify that each of the saved selected options equals the 
     *      expected benchmarks (ALWAYS_EXP, CHANGES_EXP, STEP_EXP)
     */
    @Test
    public void testFrequencyOptions() {
        // ARRANGE
        final int ALWAYS_EXP  = BINARY | ALWAYS  | BUFFER | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  CHANGES_EXP = BINARY | CHANGES | BUFFER | SINGLE_PREC_OFF,
                  STEP_EXP    = BINARY | STEP    | BUFFER | SINGLE_PREC_OFF;

        int init_opts = dre_fix.getSelectedOptions(),
            always_sel, changes_sel, step_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(ALWAYS_EXP);
        
        // ACT
        dre_fix.setOptions(CHANGES);
        changes_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(STEP);
        step_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(ALWAYS);
        always_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(always_sel).isEqualTo(ALWAYS_EXP);
        assertThat(changes_sel).isEqualTo(CHANGES_EXP);
        assertThat(step_sel).isEqualTo(STEP_EXP);
    }

    /**
     * Purpose: Test changing the selected buffer type 
     * Procedures:
     *   1) Select No Buffer and save the selected options within no_buff_sel
     *   2) Select Ring Buffer and save the selected options within ring_sel
     *   3) Select Buffer and save the selected options within buffer_sel
     *   4) Verify that each of the saved selected options equals the 
     *      expected benchmarks (BUFFER_EXP, NO_BUFF_EXP, RING_EXP)
     */
    @Test
    public void testBufferOptions() {
        // ARRANGE
        final int BUFFER_EXP  = BINARY | ALWAYS | BUFFER      | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  NO_BUFF_EXP = BINARY | ALWAYS | NO_BUFFER   | SINGLE_PREC_OFF,
                  RING_EXP    = BINARY | ALWAYS | RING_BUFFER | SINGLE_PREC_OFF;

        int init_opts = dre_fix.getSelectedOptions(),
            buffer_sel, no_buff_sel, ring_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(BUFFER_EXP);
        
        // ACT
        dre_fix.setOptions(NO_BUFFER);
        no_buff_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(RING_BUFFER);
        ring_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(BUFFER);
        buffer_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(buffer_sel).isEqualTo(BUFFER_EXP);
        assertThat(no_buff_sel).isEqualTo(NO_BUFF_EXP);
        assertThat(ring_sel).isEqualTo(RING_EXP);
    }

    /**
     * Purpose: Test toggling single precision on/off 
     * Procedures:
     *   1) Toggle single precision on and save the selected options within sp_on_sel
     *   2) Toggle single precision off and save the selected options within sp_off_sel
     *   3) Verify that each of the saved selected options equals the 
     *      expected benchmarks (SP_OFF_EXP, SP_ON_EXP)
     */
    @Test
    public void testSinglePrecOptions() {
        // ARRANGE
        final int SP_OFF_EXP = BINARY | ALWAYS | BUFFER | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  SP_ON_EXP  = BINARY | ALWAYS | BUFFER | SINGLE_PREC_ON;

        int init_opts = dre_fix.getSelectedOptions(),
            sp_off_sel, sp_on_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(SP_OFF_EXP);
        
        // ACT
        dre_fix.setOptions(SINGLE_PREC_ON);
        sp_on_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(SINGLE_PREC_OFF);
        sp_off_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(sp_off_sel).isEqualTo(SP_OFF_EXP);
        assertThat(sp_on_sel).isEqualTo(SP_ON_EXP);
    }

	/**
	 * Purpose: Select variables and verify that the Selected Variable
     *          Panel shows the expected output
	 * Procedures:
	 *   1) Select each contents of the Selected Variables panel to the
     *      SEL_VARS array
	 */
    @Test
    public void testSelectVars() {
        // ARRANGE
        final String[] SEL_VARS = { "drx.drt.a", "drx.drt.b", "drx.drt.c", 
                                    "drx.drt.uintB.var1"  , "drx.drt.intB.var1",
                                    "drx.drt.ucharB.var1" , "drx.drt.charB.var1",
                                    "drx.drt.ushortB.var1", "drx.drt.shortB.var1",
                                    "drx.drt.mixB.var1"};
        String[] res_vars;

        // ACT
        for(int i = 0; i < SEL_VARS.length; i++){
            dre_fix.selectVar(SEL_VARS[i]);
            sleep(250);
        }

        res_vars = dre_fix.getSelectedVars();

        // ASSERT
        assertThat(res_vars.length)
            .withFailMessage("Unexpected number of selected variables.")
            .isEqualTo(SEL_VARS.length);

        for(int i = 0; i < SEL_VARS.length; i++)
            assertThat(res_vars[i]).isEqualTo(SEL_VARS[i]);
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
        assumeThat(dre_fix.getSearchResults()).isNull();

        // ACT
        dre_fix.enterQuery("var1\n");
        sleep(50);

        found_vars = dre_fix.getSearchResults();

        // ASSERT
        assertThat(found_vars.length)
            .withFailMessage("Unexpected number of selected variables.")
            .isEqualTo(SEARCH_VARS.length);

        for(int i = 0; i < SEARCH_VARS.length; i++)
            assertThat(found_vars[i]).isEqualTo(SEARCH_VARS[i]);
    }

    /**
     * Purpose: Test inputing information into the toolbar
     * Procedures:
     *   1) Enter a group name
     *   2) Enter a cycle
     *   3) Set the maximum file size and the unit of measurement
     *   4) Parse the settings output to verify the correct information was processed
     */
    @Test
    public void testToolbar() {
        // ARRANGE
        final String GRP_NAME  = "TestingToolbar",
                     GRP_CYCLE = "5.2",
                     GRP_SIZE  = "12";
        final Size   GRP_UNIT  = Size.MB;
        final String[] EXP_LINES = { "drg.append(trick.DRBinary(\"" + GRP_NAME + "\"))",
                                     "drg[DR_GROUP_ID].set_cycle(" + GRP_CYCLE + ")",
                                     "drg[DR_GROUP_ID].set_max_file_size(" + GRP_SIZE + GRP_UNIT.TAG + ")" };

        String[] output;
        boolean setGrpName = false,
                setCycle = false,
                setSize = false;

        // ACT
        dre_fix.setGroupName(GRP_NAME);
        dre_fix.setCycle(GRP_CYCLE);
        dre_fix.setMaxFileSize(GRP_SIZE, GRP_UNIT);

        output = dre_app.getSettingsOutput();

        // Assert
        for(String line : output) {
            if (line.contains("drg.append")) {
                setGrpName = true;
                assertThat(line).isEqualTo(EXP_LINES[0]);
            } else if (line.contains("set_cycle")) {
                setCycle = true;
                assertThat(line).isEqualTo(EXP_LINES[1]);
            } else if(line.contains("set_max_file_size")) {
                setSize = true;
                line = line.split(" # ")[0];
                assertThat(line).isEqualTo(EXP_LINES[2]);
            }
        }

        assertThat(setGrpName)
                .withFailMessage("Group Name was not set...\n")
                .isTrue();

        assertThat(setCycle)
                .withFailMessage("Cycle was not set...\n")
                .isTrue();

        assertThat(setSize)
                .withFailMessage("Max File Size was not set...\n")
                .isTrue();
    }

    /**
     * Purpose: Test the save functionality
     * Procedures:
     *   1) Enter a group name
     *   2) Enter a cycle
     *   3) Set the maximum file size and the unit of measurement
     *   4) Set the the format as ASCII, the frequency as Changes,
     *      the buffer as No Buffer, and Single Precision on
     *   5) Select the various different variable types (drx.drt.charB,
     *      drx.drt.intB, drx.drt.shortB, drx.drt.ucharB, drx.drt.uintB, 
     *      drx.drt.ushortB, drx.drt.mixB)
     *   6) Make sure there isn't any remaining test result file from 
     *      previous test runs
     *   7) Click the Save menu item
     *   8) Save the .dr file to test_result.dr
     *   9) Compare test_result.dr to the presaved benchmark dre_test_ascii.dr
     */
    @Test
    public void testSaveDataRecordGroup() {
        // ARRANGE
        final File TEST_DR = new File("src/test/java/trick/dre/resources/dre_test_ascii.dr");
        final String NAME = "DR_Test_ASCII",
                     CYCLE = "0.2",
                     SIZE = "3",
                     VAR_PREFIX = "drx.drt.",
                     VAR_POSTFIX = ".var";
        final Size   UNIT = Size.MB;
        final String[] VAR_TYPES = {"charB", "intB", "shortB", "ucharB", "uintB", "ushortB", "mixB"};
        String var;
        File result = new File("test_result.dr");
        boolean file_as_expected = false;

        dre_fix.setGroupName(NAME);
        dre_fix.setCycle(CYCLE);
        dre_fix.setMaxFileSize(SIZE, UNIT);
        dre_fix.setOptions(ASCII | CHANGES | SINGLE_PREC_ON | NO_BUFFER);

        for(int i = 0; i < VAR_TYPES.length; i++) {
            for(int j = 1; j <= 4; j++) {
                var = VAR_PREFIX + VAR_TYPES[i] + VAR_POSTFIX + j;
                dre_fix.selectVar(var);
                sleep(250);
            }
        }

        if (result != null && result.exists()) {
            result.delete();
        }
        result.deleteOnExit();
        
        assumeThat(result != null && !result.exists()).isTrue();
        assumeThat(TEST_DR != null && TEST_DR.exists())
                .withFailMessage("Benchmark File Not Found!")
                .isTrue();
        
        //ACT
        dre_fix.saveMenuItem("test_result");
        file_as_expected = sameFileContent(TEST_DR, result);
        
        //ASSERT
        if (!file_as_expected) {
            try{
                Files.copy( result.toPath(), 
                            new File("failed_test.dr").toPath(), 
                            StandardCopyOption.REPLACE_EXISTING );
            } catch(Exception IGNORED) {}

            assertThat(file_as_expected)
                    .withFailMessage("Generated File doesn't match Benchmark. Failed .dr file is in java directory\n")
                    .isTrue();
        }
    }

    /**
     * Purpose: Test opening .dr files
     * Procedures:
     *   1) Click the Open menu item
     *   2) Get the selected options and variables and store them in 
     *      sel_opts and sel_vars, respectively
     *   3) Get the settings output and store it in the settings array
     *   4) Verify that the expected options are selected by comparing
     *      sel_opts with the benchmark EXP_OPTIONS
     *   5) Parse the settings array and ensure that name, cycle, and 
     *      max file size are as expected.
     *   6) Verify that the selected variables within sel_vars is equal
     *      to the expected within SEL_VARS
     */
    @Test
    public void testLoadDataRecordGroup() {
        // ARRANGE
        final File TEST_DR = new File("src/test/java/trick/dre/resources/dre_test_ascii.dr");
        final int EXP_OPTIONS = ASCII | CHANGES | SINGLE_PREC_ON | NO_BUFFER;
        final String EXP_NAME = "DR_Test_ASCII",
                     EXP_CYCLE = "0.2",
                     EXP_SIZE = "3",
                     VAR_PREFIX = "drx.drt.",
                     VAR_POSTFIX = ".var";
        final Size   EXP_UNIT = Size.MB;
        final String[] VAR_TYPES = {"charB", "intB", "shortB", "ucharB", "uintB", "ushortB", "mixB"};

        int sel_opts;
        String[] sel_vars, settings;
        
        //ACT
        dre_fix.openMenuItem(TEST_DR.getAbsolutePath());
        
        sel_opts = dre_fix.getSelectedOptions();
        sel_vars = dre_fix.getSelectedVars();
        settings = dre_app.getSettingsOutput();
        
        //ASSERT
        assumeThat(sel_vars.length).isEqualTo(VAR_TYPES.length * 4);

        assertThat(sel_opts)
            .withFailMessage("Expected options did not load correctly.\n")
            .isEqualTo(EXP_OPTIONS);

        for(String line : settings) {
            if (line.contains("drg.append")) {
                assertThat(line).contains(EXP_NAME);
            } else if (line.contains("set_cycle")) {
                assertThat(line).contains(EXP_CYCLE);
            } else if(line.contains("set_max_file_size")) {
                assertThat(line).contains(EXP_SIZE)
                                .contains(EXP_UNIT.TAG);
            }
        }

        for(int i = 0; i < VAR_TYPES.length; i++) {
            int base = (i * 4) - 1;
            for(int j = 1; j <= 4; j++) {
                String varName = VAR_PREFIX + VAR_TYPES[i] + VAR_POSTFIX + j;
                int adj_index = base + j;
                
                assertThat(sel_vars[adj_index])
                    .isEqualTo(varName);
            }
        }
    }
}