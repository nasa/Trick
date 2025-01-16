package trick.dre;


import java.io.File;
import java.io.StringWriter;

import static org.assertj.core.api.Assumptions.assumeThat;
import org.jdesktop.application.Application;

import trick.common.TestUtils;

public class MockDreApplication extends DreApplication {

	public MockDreApplication() {
		super();
	}

    public String[] getSettingsOutput() {
        StringWriter strWrt = new StringWriter();
        try{ writeGroupSettings(strWrt); }
		catch(Exception IGNORED) {}

        return strWrt.toString().split("\n");
    }

    public String[] getFooterOutput() {
        StringWriter strWrt = new StringWriter();
        try{ writeFileFooter(strWrt); }
		catch(Exception IGNORED) {}

        return strWrt.toString().split("\n");
    }

    public String[] getVariableOutput() {
        StringWriter strWrt = new StringWriter();
        try{ writeVariables(strWrt); }
		catch(Exception IGNORED) {}

        return strWrt.toString().split("\n");
    }
	
    public static void main(String[] args) {
		File sie;

		sieResourcePath = TEST_DR_SIE;
		sie = new File(sieResourcePath);

		assumeThat(sie.exists()).withFailMessage("Test Sie Resource File Not Found!\n")
								.isTrue();
		
        Application.launch(MockDreApplication.class, args);
    }

	/* Relative path from trick home to test_dr_sie.src, an S_sie.resource file copied from SIM_test_dr */
	private final static String TEST_DR_SIE = TestUtils.getTrickHome() +
		"/trick_source/java/src/test/resources/test_dr_sie.src";
		
	/* Relative path from trick home to cannon_sie.src, an S_sie.resource file copied from SIM_cannon_numeric */
	private final static String  CANNON_SIE = TestUtils.getTrickHome() +
		"/trick_source/java/src/test/resources/cannon_sie.src";
}