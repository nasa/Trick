package trick.jobperf;

/**
* Class TimeSpan represents a span of time.
*/
public class TimeSpan {
    public double start;
    public double stop;
    /**
     * Constructor
     * @param begin the start time.
     * @param end the end time.
     */
    public TimeSpan( double begin, double end) {
        start = begin;
        stop  = end;
    }
    /**
     * @return the stop time minus the start time.
     */
    public double getDuration() {
        return stop - start;
    }
}
