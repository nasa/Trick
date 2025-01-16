package trick.common;


import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

import javax.swing.ActionMap;
import javax.swing.Icon;

import org.jdesktop.application.ResourceMap;

/**
 * A class for checking different properties of an {@link Application}.
 *
 * @author Hong Chen
 */
public abstract class CheckApplicationProperties{
	
	
    public static String getActionText(javax.swing.Action action) {
        return (String)action.getValue(javax.swing.Action.NAME);
    }

    public static String getActionShortDescription(javax.swing.Action action) {
        return (String)action.getValue(javax.swing.Action.SHORT_DESCRIPTION);
    }
    
    // check to see if the action exists (the related @Action code needs to be implemented)
    public static javax.swing.Action checkAction(ActionMap aMap, String actionKey) {
    	javax.swing.Action action = aMap.get(actionKey);
    	assertNotNull(actionMapGetString(actionKey), action);
    	return action;
    }
    
    // check to see if the action text exists
    public static void checkActionText(ActionMap aMap, String actionKey) {
    	javax.swing.Action action = checkAction(aMap, actionKey);
    	assertNotNull(actionMapGetString(actionKey) + ".getValue(javax.swing.Action.Name)", getActionText(action));
    }
    
    // check to see if the action short description exists
    public static void checkActionShortDescription(ActionMap aMap, String actionKey) {
    	javax.swing.Action action = checkAction(aMap, actionKey);
    	assertNotNull(actionMapGetString(actionKey) + ".getValue(javax.swing.Action.SHORT_DESCRIPTION)", getActionShortDescription(action));
    }
    
    // check to see if the action text is as expected
    public static void checkActionText(ActionMap aMap, String actionKey, String expectedValue) {
    	javax.swing.Action action = aMap.get(actionKey);
    	assertEquals(actionMapGetString(actionKey) + ".getValue(javax.swing.Action.Name)", expectedValue, (String)action.getValue(javax.swing.Action.NAME));
    }
    
    // check to see if the action short description is as expected
    public static void checkActionShortDescription(ActionMap aMap, String actionKey, String expectedValue) {
    	javax.swing.Action action = checkAction(aMap, actionKey);
    	assertEquals(actionMapGetString(actionKey) + ".getValue(javax.swing.Action.SHORT_DESCRIPTION)", expectedValue, getActionShortDescription(action));
    }
    
    // check to see if the action icon is specified
    public static void checkActionIcon(ActionMap aMap, String actionKey) {
    	javax.swing.Action action = checkAction(aMap, actionKey);
    	Icon icon = (Icon)(action.getValue(javax.swing.Action.SMALL_ICON));
    	assertNotNull(actionMapGetString(actionKey) + ".getValue(javax.swing.Action.SMALL_ICON)", icon);
    }

    // check to see if the text of a key is defined
    public static void checkKeyText(ResourceMap rMap, String key) {
    	assertNotNull(rMap.getString(key));
    }
    
    // check to see if the text of a key is as expected
    public static void checkKeyText(ResourceMap rMap, String key, String expectedValue) {
    	String keyText = rMap.getString(key);
    	assertNotNull(keyText);
    	if (expectedValue != null) {
    		assertEquals(resourceMapGetString(key), expectedValue, keyText);
    	}
    }
    
    private static String actionMapGetString(String actionKey) {
    	return  "ActionMap.get(\"" + actionKey + "\")";
    }
    
    private static String resourceMapGetString(String key) {
    	return  "ResourceMap.get(\"" + key + "\")";
    }
    
}