
//========================================
//    Package
//========================================
package trick.common;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Desktop;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Desktop.Action;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.lang.reflect.Method;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.EventObject;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

import javax.swing.ActionMap;
import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.border.EmptyBorder;

import trick.common.framework.BaseApplication;
import trick.common.framework.PropertySupport;
import trick.common.framework.View;
import trick.common.ui.UIUtils;
import trick.common.utils.TrickAction;
import trick.common.utils.TrickResources;
import trick.common.utils.SwingAction;

/**
 * The parent class which all other Trick Java GUI applications extend.
 * This intends to define common properties to all other Trick Java applications.
 * It is a subclass of {@link SingleFrameApplication}.
 *
 *
 * @author Hong Chen
 * @since Trick 10
 */
public abstract class TrickApplication extends BaseApplication implements PropertySupport {

    //========================================
    //    Public data
    //========================================
    /** User settable properties, such as default directories, etc. */
    public Properties trickProperties;
    //public Properties appProperties;
    //public TrickResources appProperties;

    /** The resource map for the application. */
    public TrickResources resourceMap;

    /** The action map for the application. */
    public ActionMap actionMap;

    /** The exit listener for all applications */
    public ExitListener exitListener;

    public static String DEFAULT_TIME_NAME = "sys.exec.out.time";

    //========================================
    //    Protected data
    //========================================
    /** This application's name. */
    protected String applicationName;

    /** The exit listener for all applications */
    protected JCheckBoxMenuItem confirmExitSelection;

    /** The property directory for all applications ($HOME/.trick) */
    protected static String propDirectory;

    /** The look and feel short name and its class name map */
    protected static Map<String, String> lafMap;

    /** The look and feel short name list */
    protected static String[] lafShortNames;
    
    protected static String SOURCE_PATH;
    protected static String RESOURCE_PATH;


    //========================================
    //    Private Data
    //========================================    
    private static int popupInvokerType;
    private static TrickApplication the_trick_app;

    // if we want to put the properties into a different location, change here.
    static {
        try {
            propDirectory = System.getProperty("user.home") + java.io.File.separator + ".trick";
        } catch (Exception e) {
            propDirectory = System.getenv("TRICK_USER_HOME") + java.io.File.separator + ".trick";
        }
    }

    static {
        UIManager.LookAndFeelInfo looks[] = UIManager.getInstalledLookAndFeels();
        if (looks != null && looks.length > 0) {
            lafMap = new HashMap<String, String>();
            lafShortNames = new String[looks.length+1];
            for (int i = 0; i < looks.length; i++) {
                if (looks[i].getName().equals("GTK+")) {
                    lafMap.put("GTK", looks[i].getClassName());
                    lafShortNames[i] = "GTK";
                } else {
                    lafMap.put(looks[i].getName(), looks[i].getClassName());
                    lafShortNames[i] = looks[i].getName();
                }
            }
        } else {
            lafShortNames = new String[1];
        }
        lafShortNames[lafShortNames.length-1] = "Default";
    }

    private JDialog aboutBox = null;
//stop in trick.common.framework.BaseApplication$GUIDisplayRunner.run()
    //========================================
    //    Constructors
    //========================================
	protected TrickApplication() {
		String trick_home = System.getenv("TRICK_HOME");
		if (trick_home == null || trick_home.isEmpty()) {
			trick_home = System.getProperty("user.home") + java.io.File.separator + "trick";
		}
		
		SOURCE_PATH = trick_home + "/trick_source/java/src/main/java";
		RESOURCE_PATH = trick_home + "/trick_source/java/src/main/resources";
		the_trick_app = this;
	}
	
    //========================================
    //    Actions
    //========================================
    /**
     * closes the application. Called when the "Exit" menu item is clicked.
     */
    @SwingAction
    public void exitConfirmation() {
        if (confirmExitSelection.isSelected()) {
            addExitListener(exitListener);
        }
        else {
            removeExitListener(exitListener);
        }
    }
    
    @SwingAction
    public void helpContents() {

    }

    /**
     * Show the about box dialog.
     */
	@SwingAction
    public void showAboutBox() {
        if (aboutBox == null) {
            aboutBox = createAboutBox();
        }
        // all those components configured in this application resource
        // property file are injected through this call.
        show(aboutBox);
    }

    /**
     * Close the about box dialog.
     */
	@SwingAction
    public void closeAboutBox() {
        if (aboutBox != null) {
            aboutBox.setVisible(false);
            aboutBox = null;
        }
    }

	@SwingAction
    public void lookAndFeel() {
        if (lafMap == null) {
            JOptionPane.showMessageDialog(getMainFrame(),
                                          "Currently, the system doesn't have any Look and Feel installed!",
                                          "Look and Feel Not Found",
                                          JOptionPane.WARNING_MESSAGE);
        } else {
            int choice = -1;
            if (trickProperties.getProperty(applicationName + ".lookAndFeel") == null) {
                choice = JOptionPane.showOptionDialog(getMainFrame(), "Change default to:",
                                                      "Change Look and Feel",
                                                      JOptionPane.YES_NO_OPTION,
                                                      JOptionPane.QUESTION_MESSAGE,
                                                      null,
                                                      lafShortNames,
                                                      "Default");
            } else {
                String laf = UIManager.getLookAndFeel().getName();
                // due to the inconsistence between the name of LookAndFeel and UIManager.LookAndFeelInfo for GTK
                if (laf.startsWith("GTK")) {
                    laf = "GTK";
                }
                choice = JOptionPane.showOptionDialog(getMainFrame(), "Change " + laf + " to:",
                                                      "Change Look and Feel",
                                                      JOptionPane.YES_NO_OPTION,
                                                      JOptionPane.QUESTION_MESSAGE,
                                                      null,
                                                      lafShortNames,
                                                      laf);
            }
            if (choice > -1 && choice < lafShortNames.length) {
                if (lafMap.get(lafShortNames[choice]) != null) {
                    changeLookAndFeel(lafMap.get(lafShortNames[choice]));
                    trickProperties.setProperty(applicationName + ".lookAndFeel", lafShortNames[choice]);
                } else {
                    // if using default, the lookandfeel property doesn't need storing
                    if (lafShortNames[choice].equals("Default")) {
                        changeLookAndFeel(UIManager.getSystemLookAndFeelClassName());
                        trickProperties.remove(applicationName + ".lookAndFeel");
                    }
                }
            }
        }
    }

	@SwingAction
	public void quit(ActionEvent e) {
		super.exit(e);
	}

	@SwingAction
    public void quit() {
        quit(null);
    }

    //========================================
    //    Set/Get methods
    //========================================
    public static void setPopupInvoker(int type) {
        popupInvokerType = type;
    }

    public static int getPopupInvoker() {
        return popupInvokerType;
    }
    
    public static TrickApplication getInstance() { return the_trick_app; }

    //========================================
    //    Methods
    //========================================
    @Override
    public Object getSessionState(Component component) {
        if (component instanceof JToggleButton) {
            return ((JToggleButton) component).isSelected();
        }
        return null;
    }

    @Override
    public void setSessionState(Component component, Object state) {
        if (component instanceof JToggleButton && state instanceof Boolean) {
            ((JToggleButton)component).setSelected(((Boolean)state).booleanValue());
        }
    }

    /**
     * Helper method for changing the look and feel for the application.
     * @param lafClassName string of look and feel to use.
     */
    protected void changeLookAndFeel(String lafClassName) {
        if (lafClassName != null) {
            final String finalLafClassName = lafClassName;
            Runnable runnable = new Runnable() {
                public void run() {
                    try {
                        UIManager.setLookAndFeel(finalLafClassName);
                        SwingUtilities.updateComponentTreeUI(getMainFrame());
                    } catch (Exception exception) {
                        JOptionPane.showMessageDialog(getMainFrame(),
                                                      "Can't change look and feel",
                                                      "Invalid Look and Feel",
                                                      JOptionPane.ERROR_MESSAGE);
                    }
                }
            };
            SwingUtilities.invokeLater(runnable);
        }
    }

    /**
     * Concatenates all of the strings in an array and each separated by the specified separator.
     *
     * @param    arr          The string array.
     * @param    separator    The separator for separating each string in the array.
     *
     * @return   The concatenated string for the string array separated by the separator.
     */
    public static String arrayToString(String[] arr, String separator) {
        StringBuffer result = new StringBuffer();
        if (arr.length > 0) {
            result.append(arr[0]);
            for (int i=1; i<arr.length; i++) {
                result.append(separator);
                result.append(arr[i]);
            }
        }
        return result.toString();
    }
    
    public static String getResourcePath(Class app) {
    	String canon = app.getCanonicalName();
    	canon = "/" + canon.replace(".", "/") + ".properties";
    	
    	int filePos = canon.lastIndexOf("/");
    	if (filePos >= 0 && filePos < canon.length()) {
    		String tail = canon.substring(0, filePos) + "/resources";
    		tail += canon.substring(filePos);
    		return RESOURCE_PATH + tail;
    	} else {
	    	return "";
    	}
    }


    /**
     * Makes initialization as needed. This is called before startup().
     *
     * @see #startup
     */
    @Override
    protected void initialize(String[] args) {
        // the name of this application
        applicationName = getContext().getApplicationClass().getSimpleName();

        // set directory for session storage
        getContext().getLocalStorage().setDirectory(new File(propDirectory + java.io.File.separator + "." + applicationName));

        // register property for JToggleButton class so that its state can be saved
        getContext().getSessionStorage().putProperty(JToggleButton.class, this);
		
        resourceMap = getContext().getResourceMap();
		actionMap = getContext().getActionMap(this.getClass(), this);

        // Load any saved user settable properties from properties file
        trickProperties = new Properties();
        try {
            FileInputStream in = new FileInputStream(propDirectory + java.io.File.separator + applicationName + ".properties");
            trickProperties.load(in);
            in.close();
        }
        catch (IOException e) {}

        exitListener = new ExitListener() {
            @Override
            public boolean canExit(EventObject e) {
                return (JOptionPane.showConfirmDialog(getMainFrame(), "Do you really want to exit?", "Confirm Exit",
                          JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION);
            }
            @Override
            public void willExit(EventObject e) {}
        };

        // Check look and feel property
        String laf = trickProperties.getProperty(applicationName + ".lookAndFeel");
        if (laf != null) {
            changeLookAndFeel(lafMap.get(laf));
        }
    }

    /**
     * Starts building GUI. This is called after initialize.
     * Once startup() is done, ready() is called.
     *
     * @see #initialize
     * @see #ready
     */
    @Override
    protected void startup() {
        String defaultValue       = Boolean.toString(false);
        boolean savedExitProperty = Boolean.valueOf(trickProperties.getProperty(
                "confirmExit", defaultValue));
        if (( savedExitProperty == true ) && (getExitListeners().length==0)) {
            // initialize gets called again if you reconnect, so don't add another exitlistener
            addExitListener(exitListener);
        }
        confirmExitSelection = createCheckBoxMenuItem("exitConfirmation", savedExitProperty);
    }

    @Override
    protected void shutdown() {
        super.shutdown();
        boolean confirmExit = false;
        if (getExitListeners().length > 0) {
            confirmExit = true;
        }
        trickProperties.setProperty("confirmExit", Boolean.toString(confirmExit));

        // Save any user settable properties to properties file in user's home directory in .trick dir
        try {
            FileOutputStream out = new FileOutputStream(propDirectory + java.io.File.separator + applicationName +".properties");
            trickProperties.store(out, "--- Trick User Properties ---");
            out.close();
        } catch (IOException e) {
        }
    }

    /**
     * Creates the main {@link JComponent} for the application main {@link View}.
     * Subclasses must override it.
     *
     * @return a {@link JComponent}.
     */
    protected abstract JComponent createMainPanel();


    /**
     * Creates a {@link JMenuBar} for the application.
     * The default menu bar includes a File menu with an Exit menu item and a check box menu item allowing the user to
     * disable the exit confirmation prompt.
     *
     * @return a {@link JMenuBar}.
     */
    protected JMenuBar createMenuBar() {
        JMenuBar menuBar = new JMenuBar();
        JMenu menu = new JMenu();
        menu.setName("fileMenu");
        menu.add(createMenuItem("lookAndFeel"));
        menu.add(new JSeparator());
        menu.add(confirmExitSelection);
        menu.add(createMenuItem("quit"));
        menuBar.add(menu);
        menuBar.add(Box.createHorizontalGlue());
        menuBar.add(UIUtils.getSmallTrickIconLabel());
        return menuBar;
    }

    /**
     * Creates a {@link JToolBar} for the application.
     * Subclasses need to override this if it needs to create a tool bar.
     *
     * @return a {@link JToolBar}.
     */

    protected JToolBar createToolBar() {
        return null;
    }

    /**
     * Creates a status bar for the application.
     * Subclasses need to override this if it needs to create a status bar.
     *
     * @return a {@link JComponent}.
     */
    protected JComponent createStatusBar() {
        return null;
    }

    /**
     * Gets the action by its name.
     *
     * @param actionName    The text name of the action.
     *
     * @return    A {@link javax.swing.Action} of the specified action name.
     */
    protected javax.swing.Action getAction(String actionName) {
        return actionMap.get(actionName);
    }

    /**
     * Creates a button based on the specified action name with showing
     * the text for the button optionally.
     *
     * @param actionName    The name of the action.
     * @param showText        <code>true</code> if text of the button needs showing,
     *                         <code>false</code> otherwise.
     *
     * @return An instance of {@link JButton}.
     */
    protected JButton createButton(String actionName, boolean showText) {
        JButton button = new JButton();
        button.setVerticalTextPosition(JButton.BOTTOM);
        button.setHorizontalTextPosition(JButton.CENTER);
        button.setAction(getAction(actionName));
        button.setFocusable(false);
        if (!showText) {
            button.setText(null);
            // if button text is not showing, the mnemonic is not necessary either
            button.setMnemonic(-1);
        }
        return button;
    }

    /**
     * Help method. Returns a JMenu named menuName that contains a JMenuItem
     * for each of the specified action names (see #getAction above).
     * Actions named "---" are turned into JSeparators.
     *
     * @param menuName        The name for the {@link JMenu}.
     * @param actionNames    An array of action names.
     *
     * @return    A {@link JMenu} with specified name and action names for its menu items.
     */
    protected JMenu createMenu(String menuName, String[] actionNames) {
        JMenu menu = new JMenu();
        menu.setName(menuName);
        buildMenu(menu, actionNames);
        return menu;
    }

    /**
     * Adds radio button menu items to a specified menu for the specified actions,
     * and always selects the first button in the group by default.
     *
     * @param menu        An instance of {@link JMenu} which needs adding radio button menu itmes for.
     * @param actions    A list of actions that are for menu items.
     * @return the menu items created
     */
    protected JRadioButtonMenuItem[] addRadioButtonMenuItems(JMenu menu, String[] actions) {
        JRadioButtonMenuItem[] menuItems = new JRadioButtonMenuItem[actions.length];
        ButtonGroup group = new ButtonGroup();
        for (int i = 0; i < actions.length; i++) {
            menuItems[i] = new JRadioButtonMenuItem();
            group.add(menuItems[i]);
            menuItems[i].setAction(getAction(actions[i]));
            menuItems[i].setName(actions[i] + "MenuItem");
            if (i == 0) {
                menuItems[i].setSelected(true);
            }
            menu.add(menuItems[i]);
        }
        return menuItems;
    }

    /**
     * Adds a group of radio button menu items to a specified menu.
     *
     * @param menu        An instance of {@link JMenu} which needs adding radio button menu items for.
     * @param items       A group of radio button menu items that need adding to a menu.
     */
    protected void addRadioButtonMenuItems(JMenu menu, JRadioButtonMenuItem[] items) {
        ButtonGroup group = new ButtonGroup();
        for (JRadioButtonMenuItem item : items) {
            group.add(item);
            menu.add(item);
        }
    }

    /**
     * Help method. Returns a JPopupMenu that contains a JMenuItem
     * for each of the specified action names (see #getAction above).
     * Actions named "---" are turned into JSeparators.
     *
     * @param actionNames    An array of action names.
     *
     * @return    A {@link JPopupMenu} with specified action names for its menu items.
     */
    protected JPopupMenu createPopupMenu(String[] actionNames) {
        JPopupMenu menu = new JPopupMenu();
        buildMenu(menu, actionNames);
        return menu;
    }

    /**
     * Helper method for setting up the menu with specified action names.
     */
    private void buildMenu(JComponent menu, String[] actionNames) {
        for (String actionName : actionNames) {
            if (actionName.equals("---")) {
                menu.add(new JSeparator());
            }
            else {
                menu.add(createMenuItem(actionName));
            }
        }
    }

    /**
     * Creates a menu item via the specified action name
     *
     * @param actionName the name of the action
     *
     * @return the menu item
     */
    protected JMenuItem createMenuItem(String actionName) {
        JMenuItem menuItem = new JMenuItem(getAction(actionName));
        menuItem.setName(actionName + "MenuItem");
        // Don't set it to null so that the icon specified with the action would be honored
        //menuItem.setIcon(null);
        return menuItem;
    }

    /**
     * Creates a checkbox menu item via the specified action name
     *
     * @param actionName the name of the action
     * @param b true or false
     *
     * @return the checkbox menu item
     */
    protected JCheckBoxMenuItem createCheckBoxMenuItem(String actionName, boolean b) {
        JCheckBoxMenuItem menuItem = new JCheckBoxMenuItem(getAction(actionName));
        menuItem.setName(actionName + "CheckBoxMenuItem");
        menuItem.setSelected(b);
        // Don't set it to null so that the icon specified with the action would be honored
        //menuItem.setIcon(null);
        return menuItem;
    }

    /**
     * Creates a simple about box {@link JDialog} that displays the standard
     * Application resources, like {@code Application.title} and
     * {@code Application.description}. The about box's labels and fields
     * are configured by resources that are injected when the about box
     * is shown. All {@code Application.version}, {@code Application.vender}, 
     * and {@code Application.homepage} are defiend in {@link TrickApplication}
     * resource property file from which any sub-class can inherit. All sub-class
     * needs to have its own {@code Application.id}, {@code Application.title}, and
     * {@code Application.description} to distinguish it from others.
     * @return the about box
     */
    protected JDialog createAboutBox() {
        JPanel panel = new JPanel(new GridBagLayout());
        panel.setBorder(new EmptyBorder(0, 28, 16, 28)); // top, left, bottom, right
        JLabel titleLabel = new JLabel();
        titleLabel.setName("aboutTitleLabel");
        GridBagConstraints c = new GridBagConstraints();
        initGridBagConstraints(c);
        c.anchor = GridBagConstraints.WEST;
        c.gridwidth = GridBagConstraints.REMAINDER;
        c.fill = GridBagConstraints.HORIZONTAL;
        c.ipady = 32;
        c.weightx = 1.0;
        panel.add(titleLabel, c);
        String[] fields = {"description", "version", "vendor", "homepage"};
        for(String field : fields) {
            JLabel label = new JLabel();
            label.setName(field + "Label");
            JComponent textField = new JTextField();
            ((JTextField)textField).setHorizontalAlignment(JTextField.RIGHT);
            textField.setName(field + "TextField");
            ((JTextField)textField).setEditable(false);
            // if for homepage, using JButton instead of JTextField so that
            // the user can click the homepage link.
            if ("homepage".equals(field)) {
                // gets the text from the resource file for homepageTextField
                final String uriStr = resourceMap.getString("homepageTextField.text");
                textField = new JButton("<html><body><a href='" + uriStr + "'>" + uriStr + "</a></body></html>");
                try {
                    final URI uri = new URI(uriStr);
                    ((JButton)textField).addActionListener(new ActionListener() {
                        public void actionPerformed(ActionEvent e) {
                            open(uri);
                        }
                    });
                } catch (URISyntaxException ue) {
                    ((JButton)textField).setText(uriStr);
                }
            }

            initGridBagConstraints(c);
            c.anchor = GridBagConstraints.BASELINE_TRAILING; //1.6 ONLY
            c.anchor = GridBagConstraints.EAST;
            panel.add(label, c);
            initGridBagConstraints(c);
            c.weightx = 1.0;
            c.gridwidth = GridBagConstraints.REMAINDER;
            c.fill = GridBagConstraints.HORIZONTAL;
            panel.add(textField, c);
        }
        JButton closeAboutButton = new JButton();
        closeAboutButton.setAction(getAction("closeAboutBox"));
        initGridBagConstraints(c);
        c.anchor = GridBagConstraints.EAST;
        c.gridx = 1;
        panel.add(closeAboutButton, c);
        JDialog dialog = new JDialog(getMainFrame());
        dialog.setName("aboutDialog");
        dialog.add(panel, BorderLayout.CENTER);
        return dialog;
    }

    /**
     * Helper method for launching the user-default browser to show the specified {@link URI}.
     */
    private void open(URI uri) {
        if (Desktop.isDesktopSupported()) {
            Desktop desktop = Desktop.getDesktop();
            try {
                desktop.browse(uri);
            } catch (IOException e) {
                JOptionPane.showMessageDialog(getMainFrame(),
                                              "There is no registered application for opening \n" + uri.toString(),
                                              "Warning", JOptionPane.WARNING_MESSAGE);
            }
        } else {
            JOptionPane.showMessageDialog(getMainFrame(),
                                          "This desktop is not supported to open \n" + uri.toString(),
                                          "Warning", JOptionPane.WARNING_MESSAGE);
        }
    }

    /**
     * Helper method for same {@link GridBagConstraints} settings.
     */
    private void initGridBagConstraints(GridBagConstraints c) {
        c.anchor = GridBagConstraints.CENTER;
        c.fill = GridBagConstraints.NONE;
        c.gridwidth = 1;
        c.gridheight = 1;
        c.gridx = GridBagConstraints.RELATIVE;
        c.gridy = GridBagConstraints.RELATIVE;
        c.insets = new Insets(0,0,0,0);
        c.ipadx = 4; // not the usual default
        c.ipady = 4; // not the usual default
        c.weightx = 0.0;
        c.weighty = 0.0;
    }

    /**
     * Creates a help menu item via action names related Java help set file name.
     *
     * @param menuName        the name of the menu
     * @param actionNames    all the action names for this help menu
     * @param helpSetName    the Jave help set file name
     *
     * @return the help menu
     *
     */
    protected JMenu createHelpMenu(String menuName, String[] actionNames, String helpSetName) {
        JMenu helpMenu = createMenu(menuName, actionNames);
        return helpMenu;
    }

    /**
     * prepends <code>${TRICK_HOME}/bin</code> to <code>executable</code> and
     * runs the command in a new process with the given <code>arguments</code>.
     * Redirects the new process's standard and error outputs to the
     * corresponding parent steams.
     *
     * @param executable the trick application script to run
     * @param arguments the parameters to pass to the application
     */
    protected static void launchTrickApplication(String executable, String arguments) {
        if ( System.getProperty("os.name").startsWith("Windows") ) {
            executable = executable + ".bat" ;
        }
        try {
            ProcessBuilder process = new ProcessBuilder(UIUtils.getTrickBin() +
              File.separator + "trick-" + executable,  arguments);
            process.redirectOutput(ProcessBuilder.Redirect.INHERIT);
            process.redirectError(ProcessBuilder.Redirect.INHERIT);
            process.start();
        } catch (IOException ioException) {
            ioException.printStackTrace();
        }
    }

}
