package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.lang.Math;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.*;
import java.net.URL;

/**
* Class JobExecutionEvent represents one execution/run of a Trick job.
* <id> identifies the job. <start> and <stop> specify the
* clock times at which the job started and finished.
* <isTOF> indicates whether the job was run as
* an "top-of-frame" job.
*/
class JobExecutionEvent {
    public String id;
    public boolean isTOF;
    public boolean isEOF;
    public double start;
    public double stop;
    public int contained;

    /**
     * @param identifier identifies the relavant Trick job.
     * @param isTopOfFrame true if the job is a "top-of-frame" job, otherwise false.
     * @param isEndOfFrame true if the job is a "end-of-frame" job, otherwise false.
     * @param start_time the start time (seconds) of the identified job.
     * @param stop_time the stop time (seconds) of the identified job.
     */
    public JobExecutionEvent(String id, boolean isTOF, boolean isEOF, double start, double stop) {
        this.id = id;
        this.isTOF = isTOF;
        this.isEOF = isEOF;
        this.start = start;
        this.stop = stop;
        contained = 1;
    }

    /**
    * Determine whether a job's start time is contained
    * within another jobs stop/stop range.
    */
    public boolean contains( JobExecutionEvent other ) {
        if ((other.start > this.start) &&
            (other.start < this.stop)) {
                return true;
            }
        return false;
    }

    /**
     * Create a String representation of an object of this class.
     */
     @Override
    public String toString() {
        return ( "JobExecutionEvent: " + id + "," + start + "," + stop );
    }
}
