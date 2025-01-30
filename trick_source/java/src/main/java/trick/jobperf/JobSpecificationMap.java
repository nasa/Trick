package trick.jobperf;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.nio.file.*;

/**
* Class JobSpecificationMap associates identifiers with unique RGB colors.
*/
public class JobSpecificationMap {
    private Map<String, JobSpecification> jobSpecMap;

    /**
    * Constructor
    */
    public JobSpecificationMap( File file ) throws IOException, FileNotFoundException {
        jobSpecMap = new HashMap<String, JobSpecification>();
        System.out.println( "INSTANCIATING JobSpecificationMap("+ file.toString() +").");
        BufferedReader in = new BufferedReader( new FileReader( file.toString()) );
        String line;
        String field[];

        while( (line = in.readLine()) != null) {
            if ( line.matches("\\s+1 [|].*$") ) {
                field   = line.split("[|]");
                if (field.length == 9) {
                    String jobclass = field[2].trim();
                    int phase = Integer.parseInt( field[3].trim());
                    String id = String.format("%.2f", Double.parseDouble( field[7].trim()));
                    String name = field[8].trim();
                    jobSpecMap.put(id, new JobSpecification(name, jobclass, phase));
                    //System.out.println("JobSpec = " + id + "," + jobclass + "," + name);
                }
            }
        }
        in.close();
    }

    /**
    * Add an identifier, and a JobSpecification to the JobSpecificationMap.
    * @ param identifier  Specifies the key.
    */
    public void addKey( String identifier,  JobSpecification jobSpec) {
        if (!jobSpecMap.containsKey(identifier)) {
            jobSpecMap.put(identifier, jobSpec);
        }
    }

    /**
    * Given an identifier, return the corresponding JobSpecification.
    * @param identifier  the key.
    * @return the JobSpecification associated with the key.
    */
    public JobSpecification getJobSpecification(String identifier) {
        return jobSpecMap.get(identifier);
    }

} // class JobSpecificationMap
