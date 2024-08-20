
//========================================
//    Package
//========================================
package trick.common.utils;

import java.awt.Color;
import java.io.File;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.ImageIcon;

public class TrickResources extends Properties {
	public TrickResources() {
		super();
	}
	
	public TrickResources(TrickResources tr) {
		super(tr);
	}
	
	public String getString(String key) {
		return getResolvedProperty(key);
	}
	
	public ImageIcon getIcon(String key) {
    	String iconPath = getString(key);
    
		return new ImageIcon(iconPath);
    }
	
	public ImageIcon getImageIcon(String key) {    
		return getIcon(key);
    }
    
    public int getInteger(String key) {
    	return Integer.parseInt(getString(key));
    }
    
    public Color getColor(String key) {
    	String colorStr = getString(key);
    	String[] rgba;
    	if(colorStr != null && (rgba = colorStr.split(", ")).length >= 3) {
    		return getColorFromRGBA(rgba);
    	}
    	
    	return null;
    }
    
    private Color getColorFromRGBA(String[] code) {
    	if(code[0].indexOf(".") >= 0) {
    		float rgba[] = getRGBA_Float(code);
    		return new Color(rgba[0], rgba[1], rgba[2], rgba[3]);
    	} else {
    		int rgba[] = getRGBA_Integer(code);
    		return new Color(rgba[0], rgba[1], rgba[2], rgba[3]);
    	}
    }
    
    private float[] getRGBA_Float(String[] code) {
    		float[] rgba = { 1.0f, 1.0f, 1.0f, 1.0f };
    		rgba[0] = Float.parseFloat(code[0]);
    		rgba[1] = Float.parseFloat(code[1]);
    		rgba[2] = Float.parseFloat(code[2]);
    		
    		if(code.length == 4) {
    			rgba[3] = Float.parseFloat(code[3]);
    		}
    		
    		return rgba;
    }
    
    private int[] getRGBA_Integer(String[] code) {
    		int[] rgba = { 255, 255, 255, 255 };
    		rgba[0] = Integer.parseInt(code[0]);
    		rgba[1] = Integer.parseInt(code[1]);
    		rgba[2] = Integer.parseInt(code[2]);
    		
    		if(code.length == 4) {
    			rgba[3] = Integer.parseInt(code[3]);
    		}
    		
    		return rgba;
    
    }
    
    public void loadProperties(File resource) throws IOException, FileNotFoundException {
    	String icon, path = resource.getAbsolutePath();
    	int lastSlash = path.lastIndexOf("/");
    	path = path.substring(0, lastSlash + 1);
    	
    	load(new FileReader(resource));
    	
    	for(String key : stringPropertyNames()) {
    		if(isIcon(key)) {
    			icon = getString(key);
    			if(icon.indexOf("/") < 0) {
    				icon = path + icon;
    				setProperty(key, icon);
    			}
    		}
    	}
    }
    
    private boolean isIcon(String key) {
    	return  key.endsWith(".icon") ||
    			key.endsWith(".smallIcon") ||
    			key.endsWith(".largeIcon");
	}
	
	private String getResolvedProperty(String key) {
		String val = getProperty(key);
		
		if(val != null) {
			Pattern pat = Pattern.compile("\\$\\{.*?\\}");
			Matcher match = pat.matcher(val);
			
			while(match.find()) {
				String found = match.group(0);
				found = found.substring(2, found.length() - 1);
				
				String replace = getResolvedProperty(found);
				
				val = match.replaceFirst(replace);
				match = pat.matcher(val);
			}
		}
		
		return val;
	}
}
