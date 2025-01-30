package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

class SortButtonsToolBar extends JToolBar implements ActionListener {
    JobStatsViewCanvas statsViewCanvas;
    private JButton sortByIDButton;
    private JButton sortByMean;
    private JButton sortByStdDev;
    private JButton sortByMin;
    private JButton sortByMax;

    public SortButtonsToolBar( JobStatsViewCanvas statsViewCanvas ) {
        this.statsViewCanvas = statsViewCanvas;

        add( new JLabel("Sort by : "));
        sortByIDButton = new JButton("ID");
        sortByIDButton.addActionListener(this);
        sortByIDButton.setActionCommand("sort-by-ID");
        sortByIDButton.setToolTipText("Sort by Job ID");
        add(sortByIDButton);

        sortByMean = new JButton("Mean");
        sortByMean.addActionListener(this);
        sortByMean.setActionCommand("sort-by-mean");
        sortByMean.setToolTipText("Sort by Mean Job Run Duration");
        add(sortByMean);

        sortByStdDev = new JButton("Std Dev");
        sortByStdDev.addActionListener(this);
        sortByStdDev.setActionCommand("sort-by-std-dev");
        sortByStdDev.setToolTipText("Sort by Std Deviation of Job Run Duration");
        add(sortByStdDev);

        sortByMin = new JButton("Min");
        sortByMin.addActionListener(this);
        sortByMin.setActionCommand("sort-by-min");
        sortByMin.setToolTipText("Sort by Minimum Job Run Duration");
        add(sortByMin);

        sortByMax = new JButton("Max");
        sortByMax.addActionListener(this);
        sortByMax.setActionCommand("sort-by-max");
        sortByMax.setToolTipText("Sort by Maximum Job Run Duration");
        add(sortByMax);
    }

    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "sort-by-ID":
                statsViewCanvas.jobStats.SortByID();
                statsViewCanvas.repaint();
                break;
            case "sort-by-mean":
                statsViewCanvas.jobStats.SortByMeanValue();
                statsViewCanvas.repaint();
                break;
            case "sort-by-std-dev":
                statsViewCanvas.jobStats.SortByStdDev();
                statsViewCanvas.repaint();
                break;
            case "sort-by-min":
                statsViewCanvas.jobStats.SortByMinValue();
                statsViewCanvas.repaint();
            break;
            case "sort-by-max":
                statsViewCanvas.jobStats.SortByMaxValue();
                statsViewCanvas.repaint();
            break;
            default:
                System.out.println("Unknown Action Command:" + s);
                break;
        }
    }
}

public class JobStatsViewWindow extends JFrame {

    public JobStatsViewWindow( JobStats jobStats, JobSpecificationMap jobSpecificationMap ) {

        JobStatsViewCanvas statsViewCanvas = new JobStatsViewCanvas( jobStats, jobSpecificationMap);

        JScrollPane scrollPane = new JScrollPane( statsViewCanvas );
        scrollPane.setPreferredSize(new Dimension(800, 400));
        scrollPane.getVerticalScrollBar().setUnitIncrement( 20 );

        SortButtonsToolBar toolbar = new SortButtonsToolBar( statsViewCanvas);

        JPanel scrollingJobStatsCanvas = new JPanel();
        scrollingJobStatsCanvas.setPreferredSize(new Dimension(800, 400));
        scrollingJobStatsCanvas.add(toolbar);
        scrollingJobStatsCanvas.add(scrollPane);

        scrollingJobStatsCanvas.setLayout( new BoxLayout(scrollingJobStatsCanvas, BoxLayout.Y_AXIS));

        setTitle("Job Statistics");
        // setSize(800, 500);
        setPreferredSize(new Dimension(1200, 500));
        add(scrollingJobStatsCanvas);
        pack();
        setVisible(true);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setFocusable(true);
        setVisible(true);

        statsViewCanvas.repaint();
    }
}
