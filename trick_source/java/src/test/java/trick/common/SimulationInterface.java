package trick.common;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.Scanner;

public class SimulationInterface {

	public static boolean cleanSim(String simPath) {
		File sim_dir = new File(simPath);
		if (sim_dir.exists())  
			return run_cmd(sim_dir, "make clean", true) != null;
		else  
			return false;
	}

	public static boolean compileSim(String simPath) {
		File sim_dir = new File(simPath);
		if (sim_dir.exists())  
			return run_cmd(sim_dir, "trick-CP", true) != null;
		else  
			return false;
	}

	public static Process startSim(String simPath, String ... args) {
		File sim_dir = new File(simPath);
		if (sim_dir.exists()) {
			File exe = find_exe(sim_dir);
			if(exe != null && exe.exists()) {
				String cmd = "./" + find_exe(sim_dir).getName();
				for(String arg : args) cmd += " " + arg;

				return run_cmd(sim_dir, cmd, false);
			}
		}
		return null;
	}

	private static Process run_cmd(File dir, String cmd, boolean block) {
		try {
			Process p = Runtime.getRuntime().exec(cmd, null, dir);
			while(p.isAlive() && block) {  continue;  }

			return p;
		} catch(IOException ioe) {
			System.err.println(ioe.getMessage());

			return null;
		}
	}

	private static Process run_cmd(File dir, String cmd) { return run_cmd(dir, cmd, false); }

	private static File find_exe(File dir) {
		File[] exe = dir.listFiles(new FilenameFilter() {
			public boolean accept(File d, String name) {
				return name.startsWith("S_main") && name.endsWith("exe");
			}
		});

		if (exe.length > 0) {
			if (exe.length > 1)
				System.out.printf("WARNING: Detected %d simulation executables. Selecting '%s'...\n",
								  exe.length,  exe[0].getName());
			return exe[0];
		} 
		
		return null;
	}
}