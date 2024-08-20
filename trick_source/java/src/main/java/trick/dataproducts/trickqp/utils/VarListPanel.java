
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//Imports
//========================================
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;

import javax.swing.Box.Filler;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.ListCellRenderer;

import org.jdesktop.swingx.JXList;

import trick.common.TrickApplication;
import trick.common.ui.panels.ListPanel;
import trick.common.utils.LogVar;
import trick.common.utils.LogVar.DisplayType;
import trick.common.utils.SortedListModel;
import trick.common.utils.TrickResources;


/**
 * A class that extends {@link ListPanel} for displaying a list of {@link LogVar}
 * using {@link JXList}. All contents are sorted and unique.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class VarListPanel extends ListPanel {
   

	//========================================
    //    Public data
    //========================================


    //========================================
    //    Protected data
    //========================================

	
	//========================================
    //    Private Data
    //========================================
	
	private static final long serialVersionUID = 5858532680813373236L;

    
    //========================================
    //    Constructors
    //========================================
    /**
     *    Default constructor.
     */
	@SuppressWarnings("unchecked")
    public VarListPanel() {
    	super(new SortedListModel());
        ListCellRenderer renderer = new DataListCellRenderer();
        getJList().setCellRenderer(renderer);
    }

    //========================================
    //    Set/Get methods
    //========================================
    

    //========================================
    //    Methods
    //========================================
    /**
     * Adds one data to the list.
     *
     * @param item    The {@link Object} that needs adding to the list.
     */
    @Override
    public void addData(Object item) {
        if (item == null) {
            return;
        } else if (item instanceof Integer) {
            // 1st line in variable list is the RUN dir count
            LogVar cnt = new LogVar("@@@");
            ((SortedListModel)dataListModel).removeElement(cnt);
            cnt.setCount((Integer)item);
            cnt.setUnits(item.toString());
            cnt.setDisplay(DisplayType.HIDDEN);
            ((SortedListModel)dataListModel).add(cnt);
            return;
        } else if (((SortedListModel)dataListModel).contains(item)) {
            if (item instanceof LogVar) {
                // add to variable's RUN dir count
                LogVar varObj = (LogVar)getElement(item);
                varObj.setCount(varObj.getCount()+1);
            }
            return;
        }
        ((SortedListModel)dataListModel).add(item);
    }

    /**
     * Removes one data from the list.
     *
     * @param item    The specified data that needs removing from the list.
     */
    @Override
    public void removeData(Object item) {
        if (item==null) {
            return;
        }
        // if variable, don't actually remove it if it still belongs to a RUNdir
        if (item instanceof LogVar) {
            LogVar varObj = (LogVar)((SortedListModel)dataListModel).getElement(item);
            if (varObj==null) {
                return;
            }
            varObj.setCount(varObj.getCount()-1);
            if (varObj.getCount()==0) {
            	((SortedListModel)dataListModel).removeElement(varObj);
            }
        }
    }
    
    /**
     * Gets the data with same name as given object from the list.
     * @param obj object go get
     * @return the object
     */
    public Object getElement(Object obj) {
        return ((SortedListModel)dataListModel).getElement(obj);
    }

    /**
     * Refreshes the displayed list -- need this when hiding rows.
     */
    public void refreshData() {
    	((SortedListModel)dataListModel).refresh();
    }

    //========================================
    //    Inner classes
    //========================================
    
    // our own renderer to color a line (red when variable in varlist not in all RUN dirs)
    private class DataListCellRenderer implements ListCellRenderer {
        protected DefaultListCellRenderer defaultRenderer = new DefaultListCellRenderer();

        @Override
        public Component getListCellRendererComponent(JList list, Object value,
                         int index, boolean isSelected, boolean cellHasFocus) {
            Color bg, fg;
            new Color(0,0,0x80);
            Color red = new Color(0xb2, 0x22, 0x22);
            JLabel renderer = (JLabel) defaultRenderer.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);

            // default row color
            bg = Color.WHITE;
            fg = Color.BLACK;
            // color varlist row red if it is not contained in all RUN dirs
            if (value instanceof LogVar) {
                LogVar varObj = (LogVar)value;
                LogVar runcnt = (LogVar)list.getModel().getElementAt(0);
                if (varObj.getCount() < runcnt.getCount()) {
                    fg = red;
                }
                
                TrickApplication app = TrickApplication.getInstance();
                TrickResources resourceMap = app.getContext().getResourceMap();
                
                if (varObj.getDisplay()==DisplayType.HIDDEN) {
                    // Hide these using a blank filler component:
                    // 1st line of varlist is the RUN dir count (it's name is "@@@"); and contracted array components
                    Dimension nothing = new Dimension(0,0);
                    Filler hidden = new Filler(nothing, nothing, nothing);
                    return(hidden);
                } else {
                    renderer.setIcon(resourceMap.getIcon("treeNodeLeaf.icon"));
                    // give arrayed variables a folder icon
                    if (varObj.getDisplay()==DisplayType.EXPANDED) {
                        renderer.setIcon(resourceMap.getIcon("treeNodeOpen.icon"));
                    }
                    if (varObj.getDisplay()==DisplayType.CONTRACTED) {
                        renderer.setIcon(resourceMap.getIcon("treeNodeClosed.icon"));
                    }
                    // indent expanded array variables
                    // expand level 0 is normal variable, expand level 1 is 1st expansion, level 2 next expansion...
                    renderer.setIconTextGap(varObj.getLevel()*12);
                }
            } else {

            }
            // selected color
            if (isSelected) {
                fg = list.getSelectionForeground();
                bg = list.getSelectionBackground();
            }

            renderer.setForeground(fg);
            renderer.setBackground(bg);
            return renderer;
        }
    }

}
