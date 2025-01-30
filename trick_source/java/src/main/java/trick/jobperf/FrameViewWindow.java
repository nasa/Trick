package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

public class FrameViewWindow extends JFrame {
    public FrameViewWindow( TraceViewCanvas tvc, FrameRecord frame, int frameNumber ) {

        FrameViewCanvas frameViewCanvas = new FrameViewCanvas(tvc, frame);

        JScrollPane scrollPane = new JScrollPane( frameViewCanvas );
        scrollPane.getVerticalScrollBar().setUnitIncrement( 20 );

        JPanel scrollingFrameViewCanvas = new JPanel();
        scrollingFrameViewCanvas.add(scrollPane);
        scrollingFrameViewCanvas.setLayout(new BoxLayout(scrollingFrameViewCanvas, BoxLayout.X_AXIS));

        setTitle("Frame " + frameNumber);
        setPreferredSize(new Dimension(1200, 400));
        add(scrollingFrameViewCanvas);
        pack();
        setVisible(true);
        setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        setFocusable(true);
        setVisible(true);

        frameViewCanvas.repaint();
    }
}
