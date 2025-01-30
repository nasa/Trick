package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

public class FrameViewCanvas extends JPanel {
    private FrameRecord frame;
    private TraceViewCanvas tvc;
    private Font headingsFont;
    private Font dataFont;

    public FrameViewCanvas( TraceViewCanvas tvc, FrameRecord frame ) {
        this.tvc = tvc;
        this.frame = frame;
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

        // TITLE
        g2d.setFont(headingsFont);
        g2d.setPaint( Color.RED );
        g2d.drawString("Frame Details", 100, 50);

        // Column Headings
        g2d.setFont(headingsFont);
        g2d.setPaint( Color.BLUE );
        g2d.drawString("Job-ID", 100, 80);
        g2d.drawString("Job-Class", 180, 80);
        g2d.drawString("Start-Time", 420, 80);
        g2d.drawString("Stop-Time", 520, 80);
        g2d.drawString("Duration", 620, 80);
        g2d.drawString("Job-Name", 740, 80);

        frame.SortByStartTime();

        // For each job in the frame.
        int jobY = 100;
        for (JobExecutionEvent jobExec : frame.jobEvents) {
            g2d.setPaint( tvc.idToColorMap.getColor( jobExec.id ) );
            g2d.fillRect(50, jobY, 20, 20);
            g2d.setPaint( Color.BLACK );
            jobY += 20;
            double duration = jobExec.stop - jobExec.start;

            g2d.setFont(dataFont);
            g2d.drawString(jobExec.id, 100, jobY);
            g2d.drawString( String.format("%12.6f", jobExec.start), 420, jobY);
            g2d.drawString( String.format("%12.6f", jobExec.stop), 520, jobY);
            g2d.drawString( String.format("%12.6f", duration), 620, jobY);

            JobSpecification jobSpec = tvc.jobSpecificationMap.getJobSpecification(jobExec.id);
            if ( jobSpec == null) {
                g2d.setPaint( Color.RED );
                g2d.drawString("UNKNOWN", 180, jobY);
                g2d.drawString("UNKNOWN", 740, jobY);
            } else {
                g2d.drawString(jobSpec.jobClass, 180, jobY);
                g2d.drawString(jobSpec.name, 740, jobY);
            }
        }
        frame.SortByJobEventDuration();
    }

    /**
     * Calculate the height of the FrameViewCanvas (JPanel) needed to render the
     * jobs in the frame.
     */
    private int neededPanelHeight() {
        return 20 * frame.jobEvents.size() + 100;
    }

    /**
     * This function paints the FrameViewCanvas (i.e, JPanel) when required.
     */
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }

}
