package trick.jobperf;

import java.io.*;
import java.util.*;

/**
* Class CompareByID compares two StatisticsRecord's by id.
*/
class CompareByID implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        return a.id.compareTo(b.id);
    }
}
/**
* Class CompareByMeanValue compares two StatisticsRecord's by meanValue.
*/
class CompareByMeanValue implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.meanValue < b.meanValue) return -1;
        if ( a.meanValue > b.meanValue) return  1;
        return 0;
    }
}
/**
* Class CompareByStdDev compares two StatisticsRecord's by stddev.
*/
class CompareByStdDev implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.stddev < b.stddev) return -1;
        if ( a.stddev > b.stddev) return  1;
        return 0;
    }
}
/**
* Class CompareByMaxDuration compares two StatisticsRecord's by maxValue.
*/
class CompareByMaxDuration implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.maxValue < b.maxValue) return -1;
        if ( a.maxValue > b.maxValue) return  1;
        return 0;
    }
}
/**
* Class CompareByMinDuration compares two StatisticsRecord's by minValue.
*/
class CompareByMinDuration implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.minValue > b.minValue) return -1;
        if ( a.minValue < b.minValue) return  1;
        return 0;
    }
}

/**
* Class JobStats represents the statistics, i.e., mean, std deviation, max value,
* and min value of the run-duration of each of the Trick jobs in jobExecList. The
* statistic records can be sorted by any of the statistics, and by the job id,
* prior to being written as a report.
*/
public class JobStats {

    /**
     * Enum SortCriterion enumerates the valid ways that JobStats records can be
     * sorted.
     */
    enum SortCriterion {
        ID {
            @Override
            public String toString() { return "Identifier"; }
        },
        MEAN {
            @Override
            public String toString() { return "Mean Value"; }
        },
        STDDEV {
            @Override
            public String toString() { return "Standard Deviation"; }
        },
        MAX {
            @Override
            public String toString() { return "Maximum Value"; }
        },
        MIN {
            @Override
            public String toString() { return "Minimum Value"; }
        }
    }

    public SortCriterion currentSortCriterion = SortCriterion.MEAN;
    public ArrayList<StatisticsRecord> jobStatisticsList;

    /**
     * Constructor
     * @param jobExecList - the timeline data.
     */
    public JobStats( ArrayList<JobExecutionEvent> jobExecList ) {

        Map<String, RunRegistry> runRegistryMap
            = new HashMap<String, RunRegistry>();

        for (JobExecutionEvent jobExec : jobExecList ) {
            RunRegistry runRegistry = runRegistryMap.get(jobExec.id);
            if (runRegistry != null) {
                runRegistry.addTimeSpan(jobExec.start, jobExec.stop);
            } else {
                runRegistry = new RunRegistry();
                runRegistry.addTimeSpan(jobExec.start, jobExec.stop);
                runRegistryMap.put(jobExec.id, runRegistry);
            }
        }

        jobStatisticsList = new ArrayList<StatisticsRecord>();

        for (Map.Entry<String, RunRegistry> entry : runRegistryMap.entrySet()) {
            String id = entry.getKey();
            RunRegistry runRegistry = entry.getValue();
            double mean   = runRegistry.getMeanDuration();
            double stddev = runRegistry.getStdDev();
            double min    = runRegistry.getMinDuration();
            double max    = runRegistry.getMaxDuration();

            jobStatisticsList.add( new StatisticsRecord(id, mean, stddev, min, max));
        }
        SortByMeanValue();
    }

    /**
     * Sort by mean duration in descending order.
     */
    public void SortByID() {
       Collections.sort( jobStatisticsList, new CompareByID());
       currentSortCriterion = SortCriterion.ID;
    }

    /**
     * Sort by mean duration in descending order.
     */
    public void SortByMeanValue() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByMeanValue()));
        currentSortCriterion = SortCriterion.MEAN;
    }

    /**
     * Sort by standard deviation of duration in descending order.
     */
    public void SortByStdDev() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByStdDev()));
        currentSortCriterion = SortCriterion.STDDEV;
    }

    /**
     * Sort by maximum duration in descending order.
     */
    public void SortByMaxValue() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByMaxDuration()));
        currentSortCriterion = SortCriterion.MAX;
    }

    /**
     * Sort by minimum duration in descending order.
     */
    public void SortByMinValue() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByMinDuration()));
        currentSortCriterion = SortCriterion.MIN;
    }

    /**
    Write a text report to System.out.
    */
    public void write( JobSpecificationMap jobSpecificationMap ) {

        System.out.println("           [Job Duration Statistics Sorted by " + currentSortCriterion +"]");
        System.out.println("--------------------------------------------------------------------------------------------------");
        System.out.println("  Job Id   Mean Duration     Std Dev      Min Duration   Max Duration  Name");
        System.out.println("---------- -------------- -------------- -------------- -------------- ---------------------------");

        for (StatisticsRecord jobStatisticsRecord : jobStatisticsList ) {

           JobSpecification jobSpec = jobSpecificationMap.getJobSpecification( jobStatisticsRecord.id);
           String jobName = null;
           if (jobSpec != null) {
               jobName = jobSpec.name;
           } else {
               jobName = "UNKNOWN";
           }
           System.out.println( String.format("%10s %14.6f %14.6f %14.6f %14.6f %s",
                                               jobStatisticsRecord.id,
                                               jobStatisticsRecord.meanValue,
                                               jobStatisticsRecord.stddev,
                                               jobStatisticsRecord.minValue,
                                               jobStatisticsRecord.maxValue,
                                               jobName
                                            )
                             );
        }
    }
}
