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

class JobExecutionEvent {
    public String id;
    public boolean isEOF;
    public boolean isTOF;
    public double start;
    public double stop;

    public JobExecutionEvent(String identifier, boolean isTopOfFrame, boolean isEndOfFrame, double start_time, double stop_time) {
        id = identifier;
        isEOF = isEndOfFrame;
        isTOF = isTopOfFrame;
        start = start_time;
        stop = stop_time;
    }

    public String toString() {
        return ( "JobExecutionEvent: " + id + "," + start + "," + stop );
    }
}

class KeyedColorMap {
    private Map<String, Color> colorMap;
    int minColorIntensity;

    public KeyedColorMap() {
        colorMap = new HashMap<String, Color>();
        minColorIntensity = 100;
    }

    private Color generateColor () {
        Random rand = new Random();
        boolean found = false;
        int R = 0;
        int G = 0;
        int B = 0;

        while (!found) {
            R = rand.nextInt(256);
            G = rand.nextInt(256);
            B = rand.nextInt(256);
            found = true;
            if ((R < minColorIntensity) && (G < minColorIntensity) && (B < minColorIntensity)) {
                found = false;
            }
        }
        return new Color( R,G,B);
    }

    public void addKey( String identifier ) {
        if (!colorMap.containsKey(identifier)) {
            colorMap.put(identifier, generateColor());
        }
    }

    // Given the key, return the color.
    public Color getColor(String identifier) {
        return colorMap.get(identifier);
    }

    // Given the color, return the key.
    public String getKeyOfColor(Color search_color) {
        for (Map.Entry<String, Color> entry : colorMap.entrySet()) {
            String id = entry.getKey();
            Color color = entry.getValue();
            if (color.getRGB() == search_color.getRGB()) {
                return id;
            }
        }
        return null;
    }

    public void readFile(String fileName) throws IOException {
        try {
            BufferedReader in = new BufferedReader( new FileReader(fileName) );
            String line;
            String field[];

            while( (line = in.readLine()) !=null) {
                field   = line.split(",");
                String id    = field[0];
                int R = Integer.parseInt( field[1]);
                int G = Integer.parseInt( field[2]);
                int B = Integer.parseInt( field[3]);
                colorMap.put(id, new Color(R,G,B));
            }
            in.close();
        } catch ( java.io.FileNotFoundException e ) {
           System.out.println("File \"" + fileName + "\" not found.\n");
        }
    }

    public void writeFile(String fileName) throws IOException {
        BufferedWriter out = new BufferedWriter( new FileWriter(fileName) );
        for (Map.Entry<String, Color> entry : colorMap.entrySet()) {
            String id = entry.getKey();
            Color color = entry.getValue();
            String line = String.format(id + "," + color.getRed() +
                                             "," + color.getGreen() +
                                             "," + color.getBlue() + "\n");
            out.write(line, 0, line.length());
        }
        out.flush();
        out.close();
    }
} // KeyedColorMap

class TraceViewCanvas extends JPanel {

    public static final int MIN_TRACE_WIDTH = 4;
    public static final int DEFAULT_TRACE_WIDTH = 10;
    public static final int MAX_TRACE_WIDTH = 30;
    public static final int LEFT_MARGIN = 100;
    public static final int RIGHT_MARGIN = 100;
    public static final int TOP_MARGIN = 20;
    public static final int BOTTOM_MARGIN = 20;

    private int traceWidth;
    private double frameDuration;
    private List<JobExecutionEvent> jobExecList;
    private KeyedColorMap idToColorMap;
    private BufferedImage image;
    private TraceViewOutputToolBar sToolBar;
    private Cursor crossHairCursor;
    private Cursor defaultCursor;

    public TraceViewCanvas( ArrayList<JobExecutionEvent> jobExecEvtList, TraceViewOutputToolBar outputToolBar ) {

        traceWidth = DEFAULT_TRACE_WIDTH;
        frameDuration = 1.0;
        image = null;
        sToolBar = outputToolBar;
        jobExecList = jobExecEvtList;
        crossHairCursor = new Cursor( Cursor.CROSSHAIR_CURSOR );
        defaultCursor = new Cursor( Cursor.DEFAULT_CURSOR );
        double smallestStart =  Double.MAX_VALUE;
        double largestStop   = -Double.MAX_VALUE;

        try {
           idToColorMap = new KeyedColorMap();
           idToColorMap.readFile("IdToColors.txt");

           boolean wasTOF = false;
           double startOfFrame = 0.0;
           double lastStartOfFrame = 0.0;
           double frameSizeSum = 0.0;
           int frameNumber = 0;
           int frameSizeCount = 0;

           for (JobExecutionEvent jobExec : jobExecList ) {
                if (jobExec.start < smallestStart) smallestStart = jobExec.start;
                if (jobExec.stop  > largestStop)    largestStop  = jobExec.stop;
                // Calculate the average frame size.
                if (!wasTOF && jobExec.isTOF) {
                    startOfFrame = jobExec.start;
                    if (frameNumber > 0) {
                        double frameSize = (startOfFrame - lastStartOfFrame);
                        frameSizeSum += frameSize;
                        frameSizeCount ++;
                    }
                    lastStartOfFrame = startOfFrame;
                    frameNumber++;
                }
                wasTOF = jobExec.isTOF;
                idToColorMap.addKey(jobExec.id);
            }

            // Calculate the average frame size.
            frameDuration = frameSizeSum / frameSizeCount;
            idToColorMap.writeFile("IdToColors.txt");

           System.out.println("File loaded.\n");
        } catch ( java.io.FileNotFoundException e ) {
           System.out.println("File not found.\n");
           System.exit(0);
        } catch ( java.io.IOException e ) {
           System.out.println("IO Exception.\n");
           System.exit(0);
        }

        int preferredHeight = traceWidth * (int)((largestStop - smallestStart) / frameDuration) + TOP_MARGIN;
        setPreferredSize(new Dimension(500, preferredHeight));

        ViewListener viewListener = new ViewListener();
         addMouseListener(viewListener);
         addMouseMotionListener(viewListener);
    }

    public double getFrameDuration() {
        return frameDuration;
    }

    public void setFrameDuration(double duration) {
        frameDuration = duration;
        repaint();
    }

    public void increaseTraceWidth() {
        if (traceWidth < MAX_TRACE_WIDTH) {
            traceWidth ++;
            repaint();
        }
    }

    public void decreaseTraceWidth() {
        if (traceWidth > MIN_TRACE_WIDTH) {
            traceWidth --;
            repaint();
        }
    }

    private boolean traceRectContains(int x, int y) {
        int traceRectXMax = getWidth() - RIGHT_MARGIN;
        if ( x < (LEFT_MARGIN)) return false;
        if ( x > (traceRectXMax)) return false;
        if ( y < TOP_MARGIN) return false;
        return true;
    }

    private boolean timeRectContains(int x, int y) {
        int timeRectXMin = 30;
        int timeRectXMax = LEFT_MARGIN;
        if ( x < 30 ) return false;
        if ( x > LEFT_MARGIN) return false;
        if ( y < TOP_MARGIN) return false;
        return true;
    }

    private class ViewListener extends MouseInputAdapter {
        public void mouseReleased(MouseEvent e) {
            int x = e.getX();
            int y = e.getY();
            Color color = new Color ( image.getRGB(x,y) );

            String id = idToColorMap.getKeyOfColor( color );
            sToolBar.setJobID(id);

            if ( y > TOP_MARGIN) {
                int frameNumber = (y - TOP_MARGIN) / traceWidth;
                sToolBar.setFrameNumber(frameNumber);
            }
            if ( traceRectContains(x, y)) {
                double pixelsPerSecond = (double)calcTraceRectWidth() / frameDuration;
                double subFrameTime = (x - LEFT_MARGIN) / pixelsPerSecond;
                sToolBar.setSubFrameTime(subFrameTime);
            }
        }

        public void mouseMoved(MouseEvent e) {
            int x = e.getX();
            int y = e.getY();
            if ( traceRectContains(x, y)) {
                setCursor(crossHairCursor);
            } else {
                setCursor(defaultCursor);
            }
        }
    }

    private int calcTraceRectHeight() {
        return ( getHeight() - TOP_MARGIN - BOTTOM_MARGIN);
    }

    private int calcTraceRectWidth() {
        return ( getWidth() - LEFT_MARGIN - RIGHT_MARGIN);
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
               RenderingHints.VALUE_RENDER_QUALITY);

        int traceRectHeight = calcTraceRectHeight();
        int traceRectWidth = calcTraceRectWidth();
        double pixelsPerSecond = (double)traceRectWidth / frameDuration;

        // Panel Background Color Fill
        g2d.setPaint(Color.WHITE);
        g2d.fillRect(0, 0, getWidth(), getHeight());

        // Frame Trace Rectangle Fill
        g2d.setPaint(Color.BLACK);
        g2d.fillRect(LEFT_MARGIN, TOP_MARGIN, traceRectWidth, traceRectHeight);

        boolean wasEOF = false;
        boolean wasTOF = false;
        double startOfFrame = 0.0;
        int frameNumber = 0;

        for (JobExecutionEvent jobExec : jobExecList ) {

            if (!wasTOF && jobExec.isTOF) {
                startOfFrame = jobExec.start;
                frameNumber ++;
            }

            wasTOF = jobExec.isTOF;
            wasEOF = jobExec.isEOF;

            int jobY = TOP_MARGIN + frameNumber * traceWidth;
            int jobStartX = LEFT_MARGIN + (int)((jobExec.start - startOfFrame) * pixelsPerSecond);
            int jobWidth  = (int)( (jobExec.stop - jobExec.start) * pixelsPerSecond);

            g2d.setPaint(Color.BLACK);
            g2d.drawString ( String.format("%8.3f", startOfFrame), 30, jobY + traceWidth/2);
            g2d.setPaint( idToColorMap.getColor( jobExec.id ) );
            g2d.fillRect(jobStartX, jobY, jobWidth, traceWidth-2);

        } // for
    } // doDrawing

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        image = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_RGB);
        Graphics2D g2 = image.createGraphics();
        doDrawing(g2);
        g.drawImage(image, 0, 0, this);
        g2.dispose();
    }
} // class TraceViewCanvas

class TraceViewInputToolBar extends JToolBar implements ActionListener {

    private TraceViewCanvas traceView;
    private JTextField frameDurationField;

    public TraceViewInputToolBar (TraceViewCanvas tv) {
        traceView = tv;
        add( new JLabel(" Frame Size: "));
        frameDurationField = new JTextField(15);
        frameDurationField.setText( String.format("%8.4f", traceView.getFrameDuration()) );
        add(frameDurationField);

        JButton setButton = new JButton("Set");
        setButton.addActionListener(this);
        setButton.setActionCommand("setFrameSize");
        setButton.setToolTipText("Set frame size in seconds.");
        add(setButton);
    }
    public void actionPerformed(ActionEvent event) {
        String s = event.getActionCommand();
        switch (s) {
            case "setFrameSize":
                double newFrameSize = 0.0;
                try {
                    newFrameSize = Double.parseDouble( frameDurationField.getText() );
                } catch ( NumberFormatException e) {
                    frameDurationField.setText( String.format("%8.4f", traceView.getFrameDuration()) );
                }
                if ( newFrameSize > 0.0) {
                    traceView.setFrameDuration( newFrameSize );
                }
            break;
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }
} // class TraceViewInputToolBar

class TraceViewOutputToolBar extends JToolBar {
    private JTextField IDField;
    private JTextField frameNumberField;
    private JTextField subFrameTimeField;

    public TraceViewOutputToolBar () {

        add( new JLabel(" Job ID: "));
        IDField = new JTextField(15);
        IDField.setEditable(false);
        IDField.setText( "");
        add(IDField);

        add( new JLabel(" Frame Number: "));
        frameNumberField = new JTextField(15);
        frameNumberField.setEditable(false);
        frameNumberField.setText( "0");
        add(frameNumberField);

        add( new JLabel(" Subframe Time: "));
        subFrameTimeField = new JTextField(15);
        subFrameTimeField.setEditable(false);
        subFrameTimeField.setText( "0.00");
        add(subFrameTimeField);
    }
    public void setJobID(String id) {
        IDField.setText( id );
    }
    public void setFrameNumber(int fn) {
        frameNumberField.setText( String.format("%d", fn));
    }
    public void setSubFrameTime(double time) {
        subFrameTimeField.setText( String.format("%8.4f", time));
    }
} // class TraceViewOutputToolBar

class TraceViewMenuBar extends JMenuBar implements ActionListener {

    private TraceViewCanvas traceView;

    public TraceViewMenuBar(TraceViewCanvas tv) {
        traceView = tv;

        JMenu fileMenu = new JMenu("File");
        JMenuItem fileMenuExit = new JMenuItem("Exit");
        fileMenuExit.setActionCommand("exit");
        fileMenuExit.addActionListener(this);
        fileMenu.add(fileMenuExit);
        add(fileMenu);

        JMenu optionsMenu = new JMenu("Options");
        JMenu traceSizeMenu = new JMenu("TraceSize");
        JMenuItem traceSizeMenuIncrease = new JMenuItem("Increase Trace Width");
        traceSizeMenuIncrease.setActionCommand("increase-trace_width");
        KeyStroke ctrlPlus  = KeyStroke.getKeyStroke('P', InputEvent.CTRL_MASK );
        traceSizeMenuIncrease.setAccelerator(ctrlPlus);
        traceSizeMenuIncrease.addActionListener(this);
        traceSizeMenu.add(traceSizeMenuIncrease);
        JMenuItem traceSizeMenuDecrease = new JMenuItem("Decrease Trace Width");
        traceSizeMenuDecrease.setActionCommand("decrease-trace_width");
        KeyStroke ctrlMinus = KeyStroke.getKeyStroke('-', InputEvent.CTRL_MASK);
        traceSizeMenuDecrease.setAccelerator(ctrlMinus);
        traceSizeMenuDecrease.addActionListener(this);
        traceSizeMenu.add(traceSizeMenuDecrease);
        optionsMenu.add(traceSizeMenu);
        add(optionsMenu);

    }
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increase-trace_width":
                traceView.increaseTraceWidth();
            break;
            case "decrease-trace_width":
                traceView.decreaseTraceWidth();
            break;
            case "exit":
                System.exit(0);
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }
} // class TraceViewMenuBar

class TraceViewWindow extends JFrame {

    public TraceViewWindow( ArrayList<JobExecutionEvent> jobExecList ) {
        TraceViewOutputToolBar outputToolBar = new TraceViewOutputToolBar();
        TraceViewCanvas traceView = new TraceViewCanvas( jobExecList, outputToolBar);

        TraceViewMenuBar menuBar = new TraceViewMenuBar(traceView);
        setJMenuBar(menuBar);

        TraceViewInputToolBar nToolBar = new TraceViewInputToolBar( traceView );
        add(nToolBar, BorderLayout.NORTH);

        JScrollPane scrollPane = new JScrollPane( traceView );
        scrollPane.setPreferredSize(new Dimension(800, 400));

        JPanel tracePanel = new JPanel();
        tracePanel.setPreferredSize(new Dimension(800, 400));
        tracePanel.add(scrollPane);
        tracePanel.setLayout(new BoxLayout(tracePanel, BoxLayout.X_AXIS));

        JPanel mainPanel  = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.add(tracePanel);

        add(outputToolBar, BorderLayout.SOUTH);

        setTitle("JobPerf");
        setSize(800, 500);
        add(mainPanel);
        pack();
        setVisible(true);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);
        setVisible(true);

        traceView.repaint();
    }
} // class TraceViewWindow

class Interval {
    public double start;
    public double stop;
    public Interval( double begin, double end) {
        start = begin;
        stop  = end;
    }
    public double getDuration() {
        return stop - start;
    }
} // Interval

class RunRegistry {
    ArrayList<Interval> intervalList;

    public RunRegistry() {
        intervalList = new ArrayList<Interval>();
    }
    void addSample(double start, double stop) {
        Interval interval = new Interval(start, stop);
        intervalList.add(interval);
    }
    double getMeanDuration() {
        double mean = 0.0;
        int N = intervalList.size();
        if (N > 0) {
            double sum = 0.0;
            for (Interval interval : intervalList ) {
                sum += interval.getDuration();
            }
            mean = sum / N;
        }
        return mean;
    }
    double getStdDev() {
        double stddev = 0.0;
        int N = intervalList.size();
        if (N > 0) {
            double sum = 0.0;
            double mean = getMeanDuration();
            for (Interval interval : intervalList ) {
                double duration = interval.getDuration();
                double difference = duration - mean;
                sum += difference * difference;
            }
            stddev = Math.sqrt( sum / N );
        }
        return stddev;
    }
    double getMaxDuration() {
        double maxDuration = Double.MIN_VALUE;
        for (Interval interval : intervalList ) {
            double duration = interval.getDuration();
            if (duration > maxDuration) {
                maxDuration = duration;
            }
        }
        return maxDuration;
    }
    double getMinDuration() {
        double minDuration = Double.MAX_VALUE;
        for (Interval interval : intervalList ) {
            double duration = interval.getDuration();
            if (duration < minDuration) {
                minDuration = duration;
            }
        }
        return minDuration;
    }
} // RunRegistry

class StatisticsRecord {
    public String id;
    public double meanValue;
    public double stddev;
    public double maxValue;
    public double minValue;
    public StatisticsRecord( String s, double mean, double sd, double min, double max) {
        id = s;
        meanValue = mean;
        stddev = sd;
        minValue = min;
        maxValue = max;
    }
} // StatisticsRecord

class CompareByID implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        return a.id.compareTo(b.id);
    }
} // CompareByID

class CompareByMeanValue implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.meanValue < b.meanValue) return -1;
        if ( a.meanValue > b.meanValue) return  1;
        return 0;
    }
} // CompareByMeanValue

class CompareByStdDev implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.stddev < b.stddev) return -1;
        if ( a.stddev > b.stddev) return  1;
        return 0;
    }
} // CompareByStdDev

class CompareByMaxDuration implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.maxValue < b.maxValue) return -1;
        if ( a.maxValue > b.maxValue) return  1;
        return 0;
    }
} // CompareByMaxDuration

class CompareByMinDuration implements Comparator<StatisticsRecord> {
    public int compare(StatisticsRecord a, StatisticsRecord b) {
        if ( a.minValue < b.minValue) return -1;
        if ( a.minValue > b.minValue) return  1;
        return 0;
    }
} // CompareByMinDuration

class JobStats {

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

    SortCriterion currentSortCriterion = SortCriterion.MEAN;
    ArrayList<StatisticsRecord> jobStatisticsList;

    public JobStats( ArrayList<JobExecutionEvent> jobExecList ) {

        Map<String, RunRegistry> runRegistryMap
            = new HashMap<String, RunRegistry>();

        for (JobExecutionEvent jobExec : jobExecList ) {
            RunRegistry runRegistry = runRegistryMap.get(jobExec.id);
            if (runRegistry != null) {
                runRegistry.addSample(jobExec.start, jobExec.stop);
            } else {
                runRegistry = new RunRegistry();
                runRegistry.addSample(jobExec.start, jobExec.stop);
                runRegistryMap.put(jobExec.id, runRegistry);
            }
        }

        jobStatisticsList = new ArrayList<StatisticsRecord>();

        for (Map.Entry<String, RunRegistry> entry : runRegistryMap.entrySet()) {
            String id = entry.getKey();
            RunRegistry register = entry.getValue();
            double mean   = register.getMeanDuration();
            double stddev = register.getStdDev();
            double min    = register.getMinDuration();
            double max    = register.getMaxDuration();

            jobStatisticsList.add( new StatisticsRecord(id, mean, stddev, min, max));
        }
    }

    // Sort by MeanDuration in descending order.
    public void SortByID() {
       Collections.sort( jobStatisticsList, new CompareByID());
       currentSortCriterion = SortCriterion.ID;
    }

    // Sort by MeanDuration in descending order.
    public void SortByMeanValue() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByMeanValue()));
        currentSortCriterion = SortCriterion.MEAN;
    }

    // Sort by Standard Deviation of duration in descending order.
    public void SortByStdDev() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByStdDev()));
        currentSortCriterion = SortCriterion.STDDEV;
    }

    // Sort by MaxDuration in descending order.
    public void SortByMaxValue() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByMaxDuration()));
        currentSortCriterion = SortCriterion.MAX;
    }

    // Sort by MinDuration in descending order.
    public void SortByMinValue() {
        Collections.sort( jobStatisticsList, Collections.reverseOrder( new CompareByMinDuration()));
        currentSortCriterion = SortCriterion.MIN;
    }

    public void write() {
        System.out.println("           [Job Duration Statistics Sorted by " + currentSortCriterion +"]");
        System.out.println("----------------------------------------------------------------------");
        System.out.println("  Job Id   Mean Duration     Std Dev      Min Duration   Max Duration");
        System.out.println("---------- -------------- -------------- -------------- --------------");
           for (StatisticsRecord jobStatisticsRecord : jobStatisticsList ) {
               System.out.println( String.format("%10s %14.6f %14.6f %14.6f %14.6f",
                   jobStatisticsRecord.id,
                   jobStatisticsRecord.meanValue,
                   jobStatisticsRecord.stddev,
                   jobStatisticsRecord.minValue,
                   jobStatisticsRecord.maxValue));
        }
    }
} // JobStats

public class JobPerf extends JFrame {
    ArrayList<JobExecutionEvent> jobExecEvtList;
    JobStats jobStats;

    public JobPerf( String[] args ) {
        TraceViewWindow traceViewWindow;
        boolean interactive = true;
        boolean printReport = false;
        JobStats.SortCriterion sortOrder = JobStats.SortCriterion.MEAN;
        String fileName = "in.csv";

        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-h" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                case "-x" :
                case "--nogui" : {
                    interactive = false;
                } break;
                case "-p" :
                case "--report" : {
                    printReport = true;
                } break;
                case "-s0" :
                case "--sort=id" : {
                    sortOrder = JobStats.SortCriterion.ID;
                } break;
                case "-s1" :
                case "--sort=mean" : {
                    sortOrder = JobStats.SortCriterion.MEAN;
                } break;
                case "-s2" :
                case "--sort=stddev" : {
                    sortOrder = JobStats.SortCriterion.STDDEV;
                } break;
                case "-s3" :
                case "--sort=max" : {
                    sortOrder = JobStats.SortCriterion.MAX;
                } break;
                case "-s4" :
                case "--sort=min" : {
                    sortOrder = JobStats.SortCriterion.MIN;
                } break;
                default : {
                    fileName = args[ii];
                } break;
            } //switch
            ++ii;
        } // while

        jobExecEvtList = JobExecutionEventList(fileName);
        jobStats = new JobStats(jobExecEvtList);
        if (printReport) {
            if (sortOrder == JobStats.SortCriterion.ID ) jobStats.SortByID();
            if (sortOrder == JobStats.SortCriterion.MEAN ) jobStats.SortByMeanValue();
            if (sortOrder == JobStats.SortCriterion.STDDEV ) jobStats.SortByStdDev();
            if (sortOrder == JobStats.SortCriterion.MAX ) jobStats.SortByMaxValue();
            if (sortOrder == JobStats.SortCriterion.MIN ) jobStats.SortByMinValue();
            jobStats.write();
        }
        if (interactive) {
            traceViewWindow = new TraceViewWindow(jobExecEvtList);
        }
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
            + "usage: trick-jperf [options] <file-name>\n"
            + "options: \n"
            + "-h, --help\n"
            + "    Print this help text and exit."
            + "-x, --nogui\n"
            + "    Don't run as a GUI application. Command line only."
            + "-p, --report\n"
            + "    Write sorted job statics report to the terminal."
            + "-s0, --sort=id\n"
            + "    Sort job statistics by identifier."
            + "-s1, --sort=mean   [default]\n"
            + "    Sort job statistics by mean duration."
            + "-s2, --sort=stddev\n"
            + "    Sort job statistics by standard deviation of duration."
            + "-s3, --sort=min\n"
            + "    Sort job statistics by minimum duration."
            + "-s4, --sort=max\n"
            + "    Sort job statistics by maximum duration."
            + "----------------------------------------------------------------------\n"
          );
    }

    // Read the timeline file.
    private ArrayList<JobExecutionEvent> JobExecutionEventList( String fileName ) {
        String line;
        String field[];

        ArrayList<JobExecutionEvent> jobExecEvtList = new ArrayList<JobExecutionEvent>();
        try {
            BufferedReader in = new BufferedReader( new FileReader(fileName) );

            // Strip the header off the CSV file.
            line = in.readLine();
            while( (line = in.readLine()) !=null) {
                 field   = line.split(",");
                 String id    = field[0];
                 boolean isTOF = false;
                 if (Integer.parseInt(field[1]) == 1) isTOF = true;
                 boolean isEOF = false;
                 if (Integer.parseInt(field[2]) == 1) isEOF = true;
                 double start = Double.parseDouble( field[3]);
                 double stop  = Double.parseDouble( field[4]);

                 if (start < stop) {
                     JobExecutionEvent evt = new JobExecutionEvent(id, isTOF, isEOF, start, stop);
                     jobExecEvtList.add( evt);
                 }
             }
         } catch ( java.io.FileNotFoundException e ) {
             System.out.println("File \"" + fileName + "\" not found.\n");
             System.exit(0);
         } catch ( java.io.IOException e ) {
             System.out.println("IO Exception.\n");
             System.exit(0);
         }
         return jobExecEvtList;
    }

    public static void main(String[] args) {
        JobPerf jobPerf = new JobPerf( args );
    } // main

} // class JobPerf
