
//========================================
//	Package
//========================================
package trick.common.ui.components;

//========================================
//	Imports
//========================================
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GraphicsEnvironment;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.border.EtchedBorder;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

/**
 * Defines font chooser dialog.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class FontChooser extends JDialog implements ActionListener, ListSelectionListener {

	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	
	private static final long serialVersionUID = 5290768624262666696L;
	
	private final static String OK_OPTION		= "Ok";
	private final static String CANCEL_OPTION	= "Cancel";
	private final static String BOLD_OPTION		= "Bold";
	private final static String ITALIC_OPTION	= "Italic";
	
	private String[] fonts;
	private Integer[] sizes;
	private JList fontList;
	private JList sizeList;
	private JCheckBox boldBox;
	private JCheckBox italicBox;
	
	private Font currentFont;
	
	private JButton okButton;
	private JButton cancelButton;
	
	private JLabel sampleLabel;
	
	private int closeOption = JOptionPane.CLOSED_OPTION;
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Custructor with specified owner.
	 */
	private FontChooser(Frame parent, String title, Font font) {
		super(parent, title, true);
		fonts = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();
		// the size list was obtained from tcl code, do we or can we support more?
		sizes = new Integer[]{4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
		buildGUI(parent);
		setLocationRelativeTo(parent);
		setCurrentFont(font);
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Sets the font for the dialog.
	 * 
	 * @param font	An instance of {@link Font} that needs to be set for the dialog.
	 */
	public void setCurrentFont(Font font) {
		currentFont = font;
		if (font != null) {
			fontList.setSelectedValue(font.getName(), true);
			sizeList.setSelectedValue(font.getSize(), true);
			if (font.getStyle() == Font.PLAIN) {
				boldBox.setSelected(false);
				italicBox.setSelected(false);
			} else {
				if (font.getStyle() == (Font.BOLD | Font.ITALIC)) {
					boldBox.setSelected(true);
					italicBox.setSelected(true);
				} else {
					if (font.getStyle() == Font.BOLD) {
						boldBox.setSelected(true);
					} else if (font.getStyle() == Font.ITALIC) {
						italicBox.setSelected(true);
					}
				}
			}
			sampleLabel.setFont(font);
			sampleLabel.setText(getCurrentFontText());
		}
	}
	
	/**
	 * Gets the current font of the dialog. 
	 * 
	 * @return An instance of {@link Font}. <code>null</code> is returned if Ok is not selected.
	 */
	public Font getCurrentFont() {
		if (closeOption == JOptionPane.CLOSED_OPTION || closeOption == JOptionPane.CANCEL_OPTION) {
			return null;
		}
		return currentFont;
	}
	
	/**
	 * Gets the currently selected font in text.
	 *
	 * @return	The currently selected font in preferred text format.
	 */
	public String getCurrentFontText() {
		return currentFont.getFamily() + ":" + currentFont.getStyle() + ":" + currentFont.getSize();
	}
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Handles clicks on the buttons or check box selections.
	 * 
	 * Required by {@link ActionListener}.
	 */
	public void actionPerformed(ActionEvent e) {
		if (OK_OPTION.equals(e.getActionCommand())) {
			updateFont();
			closeOption = JOptionPane.OK_OPTION;
			setVisible(false);
			dispose();
		} else if (CANCEL_OPTION.equals(e.getActionCommand())) {
			closeOption = JOptionPane.CANCEL_OPTION;
			setVisible(false);	
			dispose();
		} else {
			updateFont();
		} 
	}
	
	/**
	 * Handles selections on the list.
	 * 
	 * Required by {@link ListSelectionListener}.
	 */
	public void valueChanged(ListSelectionEvent e) { 		
		updateFont();
	}

	
	/**
     * Set up and show the font chooser dialog.
     * 
     * @param parent	The parent component for the dialog.
     * @param title		The title of the dialog.
     * @param font		The initial font.
     * @return Font Object
     */
    public static Font showDialog(Component parent, String title, Font font) {
        Frame frame = JOptionPane.getFrameForComponent(parent);
        FontChooser dialog = new FontChooser(frame, title, font);
        dialog.setVisible(true);  
        return dialog.getCurrentFont();
    }
    
    /**
     * Helper method for updating sample lable font for the purpose of showing
     * the selected font.
     */
    private void updateFont() {
    	// if during gui set up, don't need to update
    	if (fontList == null || fontList.getSelectedValue() == null ||
    	    sizeList == null || sizeList.getSelectedValue() == null ||
    	    boldBox == null || italicBox == null) {
    		return;
    	}
    	
		int currentStyle = Font.PLAIN;
    	if (boldBox.isSelected()) {    		
    		currentStyle += Font.BOLD;
    	} 
    	if (italicBox.isSelected()) {    		
    		currentStyle += Font.ITALIC;
    	}
			
    	currentFont = new Font(fontList.getSelectedValue().toString(), currentStyle, ((Integer)sizeList.getSelectedValue()).intValue());
    	sampleLabel.setFont(currentFont);
    	sampleLabel.setText(getCurrentFontText());
    	sampleLabel.repaint();
    }

	/**
	 * Helper method for building the gui.
	 * 
	 */
    //for Java 7, the type of elements of JComboBox needs to be specified to avoid the warning and it's not supported in Java 6
    @SuppressWarnings("unchecked")
	private void buildGUI(Frame parent) {
		JPanel selectionPanel = buildSelectionPanel(),
			   samplePanel    = buildSamplePanel(),
			   buttonPanel    = buildButtonPanel();
		
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));

		getContentPane().add(selectionPanel);
		getContentPane().add(Box.createVerticalStrut(15));

		getContentPane().add(samplePanel);
		getContentPane().add(buttonPanel);
		getContentPane().add(Box.createVerticalStrut(5));

		pack();
	}
	
	/**
	 * Gets a panel that contains a list and associated title.
	 * 
	 * @param labelText		The title label for the list.
	 * @param list			An instance of {@link JList}.
	 */
	private JPanel getListPanel(String labelText, JList list) {
		JPanel ret = new JPanel();
		ret.setName(labelText);
		list.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
		JScrollPane listScroller = new JScrollPane(list);
        listScroller.setAlignmentX(LEFT_ALIGNMENT);
        
        JLabel label = new JLabel(labelText);
        ret.setLayout(new BoxLayout(ret, BoxLayout.Y_AXIS));
        label.setLabelFor(list);
        ret.add(label);
        ret.add(Box.createRigidArea(new Dimension(0,5)));
        ret.add(listScroller);
        ret.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
        
        return ret;
	}
	
	/**
	 * Builds the font selection panel.
	 * 
	 * @return the built panel
	 */
	private JPanel buildSelectionPanel() {
		JPanel selectionPanel = new JPanel();

		fontList = new JList(fonts);
		fontList.getSelectionModel().addListSelectionListener(this);
		sizeList = new JList(sizes);
		sizeList.getSelectionModel().addListSelectionListener(this);

		JPanel fontPanel  = getListPanel("Font", fontList),
			   sizePanel  = getListPanel("Size", sizeList),
			   stylePanel = buildStylePanel(fontPanel.getPreferredSize().height);

		selectionPanel.add(fontPanel);
		selectionPanel.add(sizePanel);
		selectionPanel.add(stylePanel);

		// selectionPanel.setMinimumSize(selectionPanel.getPreferredSize());

		return selectionPanel;
	}

	/**
	 * Builds the style selection panel.
	 * 
	 * @param height the preferred height of the built style panel. 
	 * 				 Should be the preferred height of the font panel.
	 * @return the built panel
	 */
	private JPanel buildStylePanel(int height) {
		JPanel stylePanel = new JPanel();
		stylePanel.setLayout(new BoxLayout(stylePanel, BoxLayout.Y_AXIS));

		String name = "Style";
		JLabel styleLabel = new JLabel(name);

		boldBox = new JCheckBox(BOLD_OPTION);
		boldBox.setName("BoldCheck");
		boldBox.addActionListener(this);

		italicBox = new JCheckBox(ITALIC_OPTION);
		italicBox.setName("ItalicCheck");
		italicBox.addActionListener(this);

		stylePanel.add(styleLabel);
		stylePanel.add(Box.createRigidArea(new Dimension(0,5)));
		stylePanel.add(boldBox);
		stylePanel.add(italicBox);
		stylePanel.setBorder(BorderFactory.createEmptyBorder(10,10,10,10));
		stylePanel.setPreferredSize(new Dimension(stylePanel.getPreferredSize().width, height));
		stylePanel.setName(name);

		return stylePanel;
	}

	/**
	 * Builds the font preview panel.
	 * 
	 * @return the built panel
	 */
	private JPanel buildSamplePanel() {
		JPanel samplePanel = new JPanel(new BorderLayout());
		String title = "Preview";
		samplePanel.setName(title);
		samplePanel.setBorder(new TitledBorder(new EtchedBorder(), title));
		
		sampleLabel = new JLabel("", JLabel.CENTER);
		sampleLabel.setBackground(Color.white);
		sampleLabel.setBorder(new LineBorder(Color.black));
		sampleLabel.setOpaque(true);
		sampleLabel.setPreferredSize(new Dimension(120, 60));
		
		samplePanel.add(sampleLabel, BorderLayout.CENTER);
		
		return samplePanel;
	}

	/**
	 * Builds the button panel to accept the new font or cancel the operation.
	 * 
	 * @return the built panel
	 */
	private JPanel buildButtonPanel() {
		JPanel buttonPanel = new JPanel();

		okButton = new JButton(OK_OPTION);
		okButton.addActionListener(this);
		cancelButton = new JButton(CANCEL_OPTION);
		cancelButton.addActionListener(this);	

		buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));
		buttonPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
		buttonPanel.setName("Buttons");

		buttonPanel.add(Box.createHorizontalStrut(30));
		buttonPanel.add(okButton);
		buttonPanel.add(Box.createRigidArea(new Dimension(10, 0)));
		buttonPanel.add(cancelButton);

		return buttonPanel;
	}
	
	//========================================
	//	Inner Classes
	//========================================
	
}
