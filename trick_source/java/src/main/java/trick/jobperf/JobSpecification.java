package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;


/**
* Class JobSpecification represents ...
*/
class JobSpecification {
    public String name;
    public String jobClass;
    public int phase;

    /**
     * @param name identifies the relevant Trick job.
     * @param jobClass the Trick job class.
     * @param phase the Trick phase number of the Trick job.
     */
    public JobSpecification(String name, String jobClass, int phase) {
        this.name = name;
        this.jobClass = jobClass;
        this.phase = phase;
    }
    /**
     * Create a String representation of an object of this jobClass.
     */
     @Override
    public String toString() {
        return ( "JobSpecification: " + name + "," + jobClass + "," + phase );
    }
}
