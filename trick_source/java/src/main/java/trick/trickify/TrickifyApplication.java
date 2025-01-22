package trick.trickify;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Desktop;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.StringReader;
import java.lang.reflect.ParameterizedType;
import java.net.URI;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.*;
import javax.swing.AbstractAction;
import javax.swing.AbstractButton;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButton;
import javax.swing.JRootPane;
import javax.swing.JScrollPane;
import javax.swing.JSeparator;
import javax.swing.JSplitPane;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.SwingWorker;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.tree.TreePath;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.namespace.QName;
import javax.xml.transform.stream.StreamSource;

import joptsimple.OptionParser;
import joptsimple.OptionSet;
import joptsimple.OptionSpec;

import org.jdesktop.application.Application;
import org.jdesktop.application.View;
import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXFrame;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXTextField;
import org.xml.sax.InputSource;

import trick.common.RunTimeTrickApplication;
import trick.common.TrickApplication;
import trick.common.utils.UnitType;
import trick.common.utils.UnitType.Unit;
import trick.common.utils.VariableServerConnection;
import trick.common.utils.vs.VSLong;
import trick.common.utils.vs.Variable;
import trick.common.utils.vs.VariableListener;
import trick.common.utils.vs.VariableTracker;

public class TrickifyApplication extends RunTimeTrickApplication
{
    JXLabel timeLabel;
    String defaultDirectory;
    JXTextField manualField;
    JFileChooser fileChooser;

    protected AbstractAction newAction = new AbstractAction("New",
      new ImageIcon(TrickApplication.class.getResource("resources/filenew.gif"))) {
        {
        }
        public void actionPerformed(ActionEvent actionEvent) 
        {
        }
    };

    @Override
    @SuppressWarnings("unchecked")
    protected void startup() 
    {
        JFrame mainFrame = getMainFrame();
    }

    @Override
    @SuppressWarnings("unchecked")
    protected JComponent createMainPanel() 
    {
        return new JPanel();
    }

    public static void main(String[] args) 
    {
        JFrame frame = new JFrame();
        frame.setSize(500, 600);
        
        JButton src_dirs_button = new JButton();
        src_dirs_button.setBounds(0, 0, 50, 20);
        src_dirs_button.setText("Choose");
        src_dirs_button.addActionListener(new ActionListener() 
        { 
            public void actionPerformed(ActionEvent e) 
            { 
                JFileChooser src_dirs_chooser = new JFileChooser();
                src_dirs_chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                src_dirs_chooser.showSaveDialog(null);
            } 
        } );
        frame.add(src_dirs_button);

        int checkbox_x = 0;
        int checkbox_y = 300;
        int checkbox_width = 250;
        int checkbox_offset = 20;
        int checkbox_relative_offset = 0;

        JCheckBox full_build_box = new JCheckBox(" Full library build");
        full_build_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        checkbox_relative_offset += checkbox_offset;
        frame.add(full_build_box);

        JCheckBox no_clean_obj_box = new JCheckBox(" Don't rebuild object file list");
        no_clean_obj_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        checkbox_relative_offset += checkbox_offset;
        frame.add(no_clean_obj_box);
        
        JCheckBox no_clean_src_box = new JCheckBox(" Don't rebuild source file list");
        no_clean_src_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        checkbox_relative_offset += checkbox_offset;
        frame.add(no_clean_src_box);
        
        JCheckBox no_clean_s_source_box = new JCheckBox(" Don't rebuild S_source.hh");
        no_clean_s_source_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        checkbox_relative_offset += checkbox_offset;
        frame.add(no_clean_s_source_box);

        JCheckBox debug_box = new JCheckBox(" Debug info");
        debug_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        checkbox_relative_offset += checkbox_offset;
        frame.add(debug_box);


        frame.setLayout(null);
        frame.setVisible(true);
        //Application.launch(TrickifyApplication.class, args);
    }

}
