package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;

/**
* Class KeyedColorMap associates identifiers with unique RGB colors.
*/
public class KeyedColorMap {
    private Map<String, Color> colorMap;
    int minLuminance;
    String fileName;

    /**
    * Constructor
    */
    public KeyedColorMap(String fileName) {
        this.fileName = fileName;
        colorMap = new HashMap<String, Color>();
        minLuminance = 30;
    }

    /**
    * Generate a random color, that's not too dark.
    * @ return the generated color.
    */
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
            // Reference: https://www.w3.org/TR/AERT/#color-contrast
            double luminance = (0.299*R + 0.587*G + 0.114*B);
            if (luminance < minLuminance ) found = false;
        }
        return new Color( R,G,B);
    }

    /**
    * Add an identifier, and a generated Color to the KeyedColorMap.
    * The Color will be generated randomly.
    * @ param identifier  Specifies the key for which a color will be generated.
    */
    public void addKey( String identifier ) {
        if (!colorMap.containsKey(identifier)) {
            colorMap.put(identifier, generateColor());
        }
    }

    /**
    * Given an identifier, return its color.
    * @param identifier  the key.
    * @return the Color associated with the key.
    */
    public Color getColor(String identifier) {
        return colorMap.get(identifier);
    }

    /**
    * Given a color, return the associated key, otherwise return null.
    * @param searchColor  the Color to search for.
    * @return the identifier associated with the searchColor.
    */
    public String getKeyOfColor(Color searchColor) {
        for (Map.Entry<String, Color> entry : colorMap.entrySet()) {
            String id = entry.getKey();
            Color color = entry.getValue();
            if (color.getRGB() == searchColor.getRGB()) {
                return id;
            }
        }
        return null;
    }

    /**
    * Write the identifier, color key/value pairs of the KeyedColorMap to a file.
    * @param fileName
    */
    public void writeFile() throws IOException {
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
    } // method writeFile

    /**
    * Read identifier, color key-value pairs into the KeyedColorMap from a file.
    * @param fileName
    */
    public void readFile() throws IOException {
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
    } // method readFile

} // class KeyedColorMap
