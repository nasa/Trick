package trick.jobperf;

import java.lang.Math;
import java.util.*;

/**
* Class RunRegistry represents a list of timeSpan's on which we can calculate
* the average (mean), standard deviation, minimum, and maximum of the timeSpans
* in the list.
*/
public class RunRegistry {
    ArrayList<TimeSpan> timeSpanList;
    /*
    * Constructor
    */
    public RunRegistry() {
        timeSpanList = new ArrayList<TimeSpan>();
    }
    void addTimeSpan(double start, double stop) {
        TimeSpan timeSpan = new TimeSpan(start, stop);
        timeSpanList.add(timeSpan);
    }
    void addTimeSpan(TimeSpan timeSpan) {
        timeSpanList.add(timeSpan);
    }
    double getMeanDuration() {
        double mean = 0.0;
        int N = timeSpanList.size();
        if (N > 0) {
            double sum = 0.0;
            for (TimeSpan timeSpan : timeSpanList ) {
                sum += timeSpan.getDuration();
            }
            mean = sum / N;
        }
        return mean;
    }
    double getStdDev() {
        double stddev = 0.0;
        int N = timeSpanList.size();
        if (N > 0) {
            double sum = 0.0;
            double mean = getMeanDuration();
            for (TimeSpan timeSpan : timeSpanList ) {
                double duration = timeSpan.getDuration();
                double difference = duration - mean;
                sum += difference * difference;
            }
            stddev = Math.sqrt( sum / N );
        }
        return stddev;
    }
    double getMaxDuration() {
        double maxDuration = Double.MIN_VALUE;
        for (TimeSpan timeSpan : timeSpanList ) {
            double duration = timeSpan.getDuration();
            if (duration > maxDuration) {
                maxDuration = duration;
            }
        }
        return maxDuration;
    }
    double getMinDuration() {
        double minDuration = Double.MAX_VALUE;
        for (TimeSpan timeSpan : timeSpanList ) {
            double duration = timeSpan.getDuration();
            if (duration < minDuration) {
                minDuration = duration;
            }
        }
        return minDuration;
    }
}
