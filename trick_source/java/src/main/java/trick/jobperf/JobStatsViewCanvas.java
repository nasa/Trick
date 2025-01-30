package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

public class JobStatsViewCanvas extends JPanel {

    private Font headingsFont;
    private Font dataFont;
    JobStats jobStats;
    JobSpecificationMap jobSpecificationMap;

    public JobStatsViewCanvas( JobStats jobStats,
                               JobSpecificationMap jobSpecificationMap ) {
        this.jobStats = jobStats;
        this.jobSpecificationMap = jobSpecificationMap;

        dataFont      = new Font("Arial", Font.PLAIN, 18);
        headingsFont  = new Font("Arial", Font.BOLD, 18);

        setPreferredSize(new Dimension(800, neededPanelHeight()));
    }

    private void doDrawing(Graphics g) {

        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
               RenderingHints.VALUE_RENDER_QUALITY);

        // Panel Background Color Fill
        g2d.setPaint(Color.WHITE);
        g2d.fillRect(0, 0, getWidth(), getHeight());

        // Title
        g2d.setFont(headingsFont);
        g2d.setPaint( Color.RED );
        g2d.drawString("Jobs Duration Statistics Sorted by " + jobStats.currentSortCriterion, 100, 50);

        // Column Headings
        g2d.setFont(headingsFont);

        g2d.setPaint( Color.BLUE );
        g2d.drawString("Job-ID", 100, 80);
        g2d.drawString("Mean Dur", 200, 80);
        g2d.drawString("Std Dev", 300, 80);
        g2d.drawString("Min Dur", 400, 80);
        g2d.drawString("Max Dur", 500, 80);
        g2d.drawString("Job-Name", 600, 80);

        // For each record
        int jobY = 100;
        for (StatisticsRecord jobStatisticsRecord : jobStats.jobStatisticsList ) {

            g2d.setFont(dataFont);
            g2d.setPaint( Color.BLACK );

            g2d.drawString(jobStatisticsRecord.id, 100, jobY);
            g2d.drawString( String.format("%14.6f", jobStatisticsRecord.meanValue), 180, jobY);
            g2d.drawString( String.format("%14.6f", jobStatisticsRecord.stddev), 280, jobY);
            g2d.drawString( String.format("%14.6f", jobStatisticsRecord.minValue), 380, jobY);
            g2d.drawString( String.format("%14.6f", jobStatisticsRecord.maxValue), 480, jobY);

            JobSpecification jobSpec = jobSpecificationMap.getJobSpecification( jobStatisticsRecord.id);
            if (jobSpec != null) {
                g2d.drawString(jobSpec.name, 600, jobY);
            } else {
                g2d.setPaint( Color.RED );
                g2d.drawString("UNKNOWN", 600, jobY);
            }

            jobY += 20;
        }
    }

    /**
     * Calculate the height of the JobStatsCanvas (JPanel) needed to render the
     * jobs in the frame.
     */
    private int neededPanelHeight() {
        return 20 * jobStats.jobStatisticsList.size() + 100;
    }

    /**
     * This function paints the JobStatsCanvas (i.e, JPanel) when required.
     */
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }

}
