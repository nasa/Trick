package trick.jobperf;

/**
* Class StatisticsRecord represents the statistics, i.e., mean, std deviation,
* max value, and min value of the run-duration of an identified Trick job.
*/
public class StatisticsRecord {
    public String id;
    public double meanValue;
    public double stddev;
    public double maxValue;
    public double minValue;
    /**
     * Constructor
     * @param id - the job identifier.
     * @param mean - the mean value of job duration.
     * @param stddev - the standard deviation of job duration.
     * @param min - the minimum value of job duration.
     * @param max - the maximum value of job duration.
     */
    public StatisticsRecord( String id, double mean, double stddev, double min, double max) {
        this.id = id;
        this.meanValue = mean;
        this.stddev = stddev;
        this.minValue = min;
        this.maxValue = max;
    }
}
