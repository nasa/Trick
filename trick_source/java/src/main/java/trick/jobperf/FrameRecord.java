package trick.jobperf;
import java.util.*;

/**
* Class CompareByDuration compares two JobExecutionEvent's by their duration.
*/
class CompareByDuration implements Comparator<JobExecutionEvent> {
    public int compare(JobExecutionEvent a, JobExecutionEvent b) {
        Double dur_a = a.stop - a.start;
        Double dur_b = b.stop - b.start;
        if ( dur_a > dur_b) return -1;
        if ( dur_a < dur_b) return  1;
        return 0;
    }
}

/**
* Class CompareByDuration compares two JobExecutionEvent's by their start time.
*/
class CompareByStartTime implements Comparator<JobExecutionEvent> {
    public int compare(JobExecutionEvent a, JobExecutionEvent b) {
        if ( a.start < b.start) return -1;
        if ( a.start > a.start) return  1;
        return 0;
    }
}

/**
* Class FrameRecord represents the set of jobs that have been executed during a
* frame.
*/
public class FrameRecord {
    public ArrayList<JobExecutionEvent> jobEvents;
    public double start;
    public double stop;
    /**
     * Constructor
     */
    public FrameRecord() {
        start = 0.0;
        stop  = 0.0;
        jobEvents = new ArrayList<JobExecutionEvent>();
    }

    /**
     * @return the stop time minus the start time.
     */
    public double getDuration() {
        return stop - start;
    }

    public void SortByJobEventDuration() {
        Collections.sort( jobEvents, new CompareByDuration());
    }

    public void SortByStartTime() {
        Collections.sort( jobEvents, new CompareByStartTime());
    }

    /**
     * For each jobEvent in the frame, record the number of times
     * its start time is contained within
     * another jobs stop/stop range.
     */
    public void CalculateJobContainment() {
        SortByJobEventDuration();
        int N = jobEvents.size();
        for (int i = 0 ; i < (N-1); i++) {
            for (int j = i+1 ; j < N; j++) {
                if ( jobEvents.get(i).contains( jobEvents.get(j) )) {
                    jobEvents.get(j).contained ++ ;
                }
            }
        }
    }
}
