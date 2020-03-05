/*-----------------------------------------------------------------
						Sandipan Dey
						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.Date;
import java.util.Properties;

import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.SequenceFile;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.SequenceFile.CompressionType;
import org.apache.hadoop.mapred.JobConf;

@SuppressWarnings("deprecation")
public class Util
{
    // hdfs input dir
    public static final String hdfsInDir = "input";
    public static final String hdfsInFile = "file";
	
    // hdfs output dir
    public static final String hdfsOutDir = "output";
    
    //private static Random r = new Random();
    private static final String [] start = {"\33[31m", "\33[32m", "\33[33m", "\33[34m", "\33[35m", "\33[36m"};
    private static final String end = "\33[0m";
    private static int cin = 0;
    private static int clen = start.length;
    
    static void distributeInputFile(String inFile, int numMaps, int numReduces, JobConf conf)  throws IOException
    {
    	final FileSystem hdfs = FileSystem.get(conf);
        
    	System.out.println();
    	
    	printLine();
    	println("local input file path: " + inFile);
    	printFileSize(new File(inFile).length());
    	println("hdfs input path: " + hdfsInDir);
    	println("hdfs output path: " + hdfsOutDir);
    	System.out.println();
    	
    	// delete hdfs input and the output directories if they exist already
    	println("deleting existing i/o directories from hdfs...");
	    hdfs.delete(new Path(hdfsInDir), true);
    	hdfs.delete(new Path(hdfsOutDir), true);
    	System.out.println();
    	
    	// System.out.println("#map: " + conf.getNumMapTasks());
    	// System.out.println("#reduce: " + conf.getNumReduceTasks());
    	// set number of map tasks
    	if (numMaps > 0)	// otherwise ignore
    	{
        	println("setting number of map / reduce tasks...");
    		conf.setNumMapTasks(numMaps);
        	conf.setNumReduceTasks(numReduces);
    	}
    	println("number of || map tasks: " + conf.getNumMapTasks());
    	println("number of || reduce tasks: " + conf.getNumReduceTasks());
    	System.out.println();
    	
    	// copy the input file from local fs to hdfs
    	println("copying the local input file to hdfs...");
		Date startTime = new Date();
		hdfs.copyFromLocalFile(new Path(inFile), new Path(hdfsInDir + Path.SEPARATOR + hdfsInFile));
		Date endTime = new Date();
		println("done copying.");
	    println("Time spent to copy the input file to hdfs: " 
				+ (int) (endTime.getTime() - startTime.getTime()) + "ms");
		printLine();
    	
    	System.out.println();
    }

    static void splitInputFile(String inFile, int numMaps, JobConf conf)  throws IOException
    {
    	final FileSystem hdfs = FileSystem.get(conf);
        
    	System.out.println();
    	
    	printLine();
    	println("local input file path: " + inFile);
    	printFileSize(new File(inFile).length());
    	println("hdfs input path: " + hdfsInDir);
    	println("hdfs output path: " + hdfsOutDir);
    	printLine();
    	System.out.println();
    	
    	// delete hdfs input and the output directories if they exist already
    	println("deleting existing i/o directories from hdfs...");
	    hdfs.delete(new Path(hdfsInDir), true);
    	hdfs.delete(new Path(hdfsOutDir), true);
    	System.out.println();
    	
    	// copy and split the input file from local fs to hdfs
    	println("splitting the local input file to hdfs... (may take several minutes)");
		System.out.println();
    	Date startTime = new Date();
		try
		{
			File f = new File(inFile);
			long length = f.length();
			FileReader fr = new FileReader(inFile);
			BufferedReader br = new BufferedReader(fr);
			
			// find approximate size of each file
			long splitLen = (long)Math.ceil((length * 1.0) / numMaps);
						
			if (splitLen <= 0)
			{
				splitLen = 1;
			}
			
			String s;
			long len = 0;
			int index = 0;

			Path file = null;
			SequenceFile.Writer writer = null;
			while((s = br.readLine()) != null)
			{
				// generate an input file for each map task
				if (len == 0)
				{
					file = new Path(hdfsInDir, "part-000" + index);
					writer = SequenceFile.createWriter(hdfs, conf, file,
							Text.class, NullWritable.class, CompressionType.NONE);
			    }

				len += s.length() + 1;
				
				writer.append(new Text(s), NullWritable.get());
		        
				if (len > splitLen)
				{
					println("wrote input for map #" + index);
 					len = 0;
					++index;
					writer.close();
				}				
		    }

			println("wrote input for map #" + index);
			writer.close();

			fr.close();
 		}
		catch (IOException e)
		{
		}	    
		
		Date endTime = new Date();
		System.out.println();
		println("done copying.");
	    System.out.println();
		printLine();
		int ms = (int) (endTime.getTime() - startTime.getTime());
	    println("time spent to copy the input file to hdfs: " 
				+ ((ms > 1000) ? (ms / 1000)  + "sec" : ms + "ms"));
		printLine();
    	
    	System.out.println();
    }

    public static void println(String msg)
    {    	
    	System.out.println(start[cin] + msg + end);		// r.nextInt(start.length)
    	cin = (cin + 1) % clen;
    }

    public static void printErr(String msg)
    {    	
    	System.out.println(start[0] + msg + end);		
    }
    
    public static void print(String msg)
    {    	
    	System.out.print(start[cin] + msg + end);		// r.nextInt(start.length)
    	cin = (cin + 1) % clen;
    }
 
    public static void printUsage()
    {    	
    	System.out.println();
    	printLine();
    	System.out.println(start[cin] + "usage: " + end);
       	cin = (cin + 1) % clen;
    	printLine();
        System.out.print(start[cin] + "(1) ./run.sh " + end);
    	cin = (cin + 1) % clen;
        System.out.print(start[cin] + "<file.properties>" + end);
    	cin = (cin + 1) % clen;
    	System.out.println();
    	System.out.print(start[cin] + "(2) ./run.sh " + end);
    	cin = (cin + 1) % clen;
        System.out.print(start[cin] + "-console" + end);
    	cin = (cin + 1) % clen;    	
        System.out.println();        
        printLine();
        System.out.println();        
    }
    
    public static void printFileSize(long len)
    {
    	String slen = "";
    	long oneKB = 1 << 10, oneMB = oneKB << 10, oneGB = oneMB << 10; 
    	if (len < oneKB)
    	{
    		slen += len + "B";
    	}
    	else if (len < oneMB)
    	{
    		len >>= 10;
 			slen += len + "KB";
 	    }
    	else if (len < oneGB)
    	{
    		len >>= 20;
 			slen += len + "MB";
 	    }
       	else
    	{
    		len >>= 30;
 			slen += len + "GB";
 	    }
    	println("input file size: " + slen);
    }
    
    public static void printLine()
    {
       	System.out.println("---------------------------------------------------------------------------------");           	
    }

    public static void printSignature()
    {
    	System.out.println();
       	printLine();
       	println("Sandipan Dey");
       	println("CSEE - 621");
       	printLine();
       	System.out.println();
    }
    
    public static void clear() throws IOException
    {
    	String clearScreenCommand = null;
    	if( System.getProperty( "os.name" ).startsWith( "Window" ) )
    	{
    	   clearScreenCommand = "cls";
    	}
    	else
    	{
    	   clearScreenCommand = "clear";
    	}
    	Runtime.getRuntime().exec( clearScreenCommand );
    }
    
    public static Properties readPropertiesFile(String propertiesFile)
    {
        // read properties file.
        Properties properties = new Properties();
        try 
        {
            properties.load(new FileInputStream(propertiesFile));
        } 
        catch (IOException e) 
        {
        }
        return properties;
    }
}
