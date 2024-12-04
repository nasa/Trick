package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

/**
 * Capabilites That Need To Be Added
 * - a way to filter the data to be within a user specified sub time period
 *   within the data set.
 */

/**
* Class JobPerf is an application that renders time-line data from a Trick based
  simulation. It also generates run-time statistics reports for the simulation
  jobs. It can be run with or without a GUI.
*/
public class JobPerf {
    ArrayList<JobExecutionEvent> jobExecEvtList;
    JobStats jobStats;

    /**
     * Constructor
     * @param args the command line arguments.
     */
    public JobPerf( String[] args ) {
        TraceViewWindow traceViewWindow;
        boolean interactive = true;
        boolean printReport = false;
        JobStats.SortCriterion sortOrder = JobStats.SortCriterion.MEAN;
        String fileName = "in.csv";

        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-h" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                case "-x" :
                case "--nogui" : {
                    interactive = false;
                } break;
                case "-p" :
                case "--report" : {
                    printReport = true;
                } break;
                case "-s0" :
                case "--sort=id" : {
                    sortOrder = JobStats.SortCriterion.ID;
                } break;
                case "-s1" :
                case "--sort=mean" : {
                    sortOrder = JobStats.SortCriterion.MEAN;
                } break;
                case "-s2" :
                case "--sort=stddev" : {
                    sortOrder = JobStats.SortCriterion.STDDEV;
                } break;
                case "-s3" :
                case "--sort=max" : {
                    sortOrder = JobStats.SortCriterion.MAX;
                } break;
                case "-s4" :
                case "--sort=min" : {
                    sortOrder = JobStats.SortCriterion.MIN;
                } break;
                default : {
                    fileName = args[ii];
                } break;
            } //switch
            ++ii;
        } // while

        jobExecEvtList = getJobExecutionEventList(fileName);
        jobStats = new JobStats(jobExecEvtList);
        if (printReport) {
            if (sortOrder == JobStats.SortCriterion.ID ) jobStats.SortByID();
            if (sortOrder == JobStats.SortCriterion.MEAN ) jobStats.SortByMeanValue();
            if (sortOrder == JobStats.SortCriterion.STDDEV ) jobStats.SortByStdDev();
            if (sortOrder == JobStats.SortCriterion.MAX ) jobStats.SortByMaxValue();
            if (sortOrder == JobStats.SortCriterion.MIN ) jobStats.SortByMinValue();
            jobStats.write();
        }
        if (interactive) {
            traceViewWindow = new TraceViewWindow(jobExecEvtList);
        }
    }

    /**
     * Print the usage instructions to the terminal.
     */
    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
            + "usage: trick-jperf [options] <file-name>\n\n"
            + "options: \n"
            + "-h, --help\n"
            + "    Print this help text and exit.\n"
            + "-x, --nogui\n"
            + "    Don't run as a GUI application. Command line only.\n"
            + "-p, --report\n"
            + "    Write sorted job statics report to the terminal.\n"
            + "-s0, --sort=id\n"
            + "    Sort job statistics by identifier.\n"
            + "-s1, --sort=mean   [default]\n"
            + "    Sort job statistics by mean duration.\n"
            + "-s2, --sort=stddev\n"
            + "    Sort job statistics by standard deviation of duration.\n"
            + "-s3, --sort=min\n"
            + "    Sort job statistics by minimum duration.\n"
            + "-s4, --sort=max\n"
            + "    Sort job statistics by maximum duration.\n"
            + "----------------------------------------------------------------------\n"
          );
    }

    /**
     * Read the timeline file, resulting in a ArrayList<JobExecutionEvent>.
     */
    private ArrayList<JobExecutionEvent> getJobExecutionEventList( String fileName ) {
        String line;
        String field[];

        ArrayList<JobExecutionEvent> jobExecEvtList = new ArrayList<JobExecutionEvent>();
        try {
            BufferedReader in = new BufferedReader( new FileReader(fileName) );

            // Strip the header off the CSV file.
            line = in.readLine();
            while( (line = in.readLine()) !=null) {
                 field   = line.split(",");
                 String id    = field[0];
                 boolean isTOF = false;
                 if (Integer.parseInt(field[1]) == 1) isTOF = true;
                 boolean isEOF = false;
                 if (Integer.parseInt(field[2]) == 1) isEOF = true;
                 double start = Double.parseDouble( field[3]);
                 double stop  = Double.parseDouble( field[4]);

                 if (start < stop) {
                     JobExecutionEvent evt = new JobExecutionEvent(id, isTOF, isEOF, start, stop);
                     jobExecEvtList.add( evt);
                 }
             }
         } catch ( java.io.FileNotFoundException e ) {
             System.out.println("File \"" + fileName + "\" not found.\n");
             System.exit(0);
         } catch ( java.io.IOException e ) {
             System.out.println("IO Exception.\n");
             System.exit(0);
         }
         return jobExecEvtList;
    }

    /**
     * Entry point for the Java application.
     */
    public static void main(String[] args) {
        JobPerf jobPerf = new JobPerf( args );
    }
}
