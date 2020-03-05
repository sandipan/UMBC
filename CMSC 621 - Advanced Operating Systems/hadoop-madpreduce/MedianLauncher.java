/*-----------------------------------------------------------------
					Sandipan Dey
					CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.Date;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.FileUtil;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.SequenceFile;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.SequenceFileInputFormat;
import org.apache.hadoop.mapred.SequenceFileOutputFormat;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.util.Tool;

/**
 * MedianLauncher
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MedianLauncher extends Configured implements Tool 
{
	String inFile;
	String outDir;
	int numMaps;
	int numReduces;
	boolean split = true;
	int timeInSec = 0;

	public MedianLauncher(String inFile, String outDir, int numMaps, int numReduces, boolean split)
	{
		  this.inFile = inFile;
		  this.outDir = outDir;
		  this.numMaps = numMaps;
		  this.numReduces = numReduces;
		  this.split = split;
	}
	  
/** Driver for the actual MapReduce process */
private void runJob() throws IOException 
  {
    JobConf conf;
    Path inPath = new Path(Util.hdfsInDir);
    Path outPath = new Path(Util.hdfsOutDir);
    
    // start 1st phase: count 
    System.out.println();
    Util.printLine();
    Util.println("starting 1st phase : counting...");
    Util.printLine();
    System.out.println();
       
    conf = new JobConf(getConf(), CountLauncher.class);
    conf.setJobName("Calculator-1"); 

    if (split)
    {
    	Util.splitInputFile(inFile, numMaps, conf);
    }
    else
    {
    	Util.distributeInputFile(inFile, numMaps, numReduces, conf);        	
    }
    
    FileInputFormat.addInputPath(conf, inPath);
    FileOutputFormat.setOutputPath(conf, outPath);

    conf.setMapOutputKeyClass(Text.class);
    conf.setMapOutputValueClass(LongWritable.class);
    conf.setOutputKeyClass(Text.class);
    conf.setOutputValueClass(NullWritable.class);

    conf.setMapperClass(split ? CountSCalculatorMapper.class : CountCalculatorMapper.class);
    //conf.setCombinerClass(CountCalculatorReducer.class); 
    conf.setReducerClass(CountCalculatorReducer.class);

    conf.setInputFormat(split ? SequenceFileInputFormat.class : TextInputFormat.class);  
    conf.setOutputFormat(split ? SequenceFileOutputFormat.class : TextOutputFormat.class);  
    
    // start a map/reduce job
    System.out.println();
    Util.println("starting map/reduce job...");
    System.out.println();
    //final long startTime = System.currentTimeMillis();
    Date startTime = new Date();
    JobClient.runJob(conf);
    //final double duration = (System.currentTimeMillis() - startTime)/1000.0;
    Date endTime = new Date();
    System.out.println();
    Util.printLine();
    int time = (int)(endTime.getTime() - startTime.getTime()) / 1000;
    Util.println("1st phase: job finished in " + time + "sec");
    timeInSec += time;
    Util.printLine();
    System.out.println();

    // copy the output file from hdfs output to hdfs input
    FileSystem hdfs = FileSystem.get(conf);
    Util.println("copying the 1st phase map/reduce output file to 2nd phase input...");
	startTime = new Date();
	Path op = new Path(Util.hdfsOutDir, "part-00000");
	Path ip = new Path(Util.hdfsInDir, "opart-00000");
	FileUtil.copy(op.getFileSystem(conf), op, ip.getFileSystem(conf), ip, 
					false, true, conf); 
	endTime = new Date();
	Util.println("done copying.");
	Util.println("Time spent to copy the input file to hdfs: " 
			+ (int) (endTime.getTime() - startTime.getTime()) + "ms");
	Util.printLine();
	System.out.println();

    // start 2nd phase: find median
    System.out.println();
    Util.printLine();
    Util.println("starting 2nd phase : finding median...");
    Util.printLine();
    System.out.println();
    
    conf = new JobConf(getConf(), MedianLauncher.class);
    conf.setJobName("Calculator-2"); 

    FileInputFormat.addInputPath(conf, inPath);
    FileOutputFormat.setOutputPath(conf, outPath);

    // delete hdfs output directory
    Util.println("deleting the output directory from hdfs...");
    hdfs.delete(outPath, true);
	System.out.println();
	
    conf.setMapOutputKeyClass(FloatWritable.class);
    conf.setMapOutputValueClass(LongWritable.class);
    conf.setOutputKeyClass(Text.class);
    conf.setOutputValueClass(FloatWritable.class);

    conf.setMapperClass(split ? MedianSCalculatorMapper.class : MedianCalculatorMapper.class);
    // conf.setCombinerClass(MedianCalculatorReducer.class); 
    conf.setReducerClass(MedianCalculatorReducer.class);

    conf.setInputFormat(split ? SequenceFileInputFormat.class : TextInputFormat.class);  
    conf.setOutputFormat(split ? SequenceFileOutputFormat.class : TextOutputFormat.class);  
    
    // start a map/reduce job
    System.out.println();
    Util.println("starting map/reduce job...");
    System.out.println();
    //final long startTime = System.currentTimeMillis();
    startTime = new Date();
    JobClient.runJob(conf);
    //final double duration = (System.currentTimeMillis() - startTime)/1000.0;
    endTime = new Date();
    System.out.println();
    Util.printLine();
    time = (int)(endTime.getTime() - startTime.getTime()) / 1000;
    Util.println("2nd phase: job finished in " + time + "sec");
    timeInSec += time;
    Util.printLine();
    System.out.println();
    Util.println("total time taken by both the phases: " + timeInSec + "sec");
    System.out.println();
    Util.printLine();
    System.out.println();

    if (split)
    {
	    // read outputs
	    Path inFile = new Path(Util.hdfsOutDir, "part-00000");
	    Text key = new Text();
	    FloatWritable value = new FloatWritable();
	    hdfs = FileSystem.get(conf);
	
	    System.out.println();
	    Util.printLine();
	    Util.println("results:");
	    Util.printLine();
	    SequenceFile.Reader reader = new SequenceFile.Reader(hdfs, inFile, conf);
		BufferedOutputStream bufStream = new BufferedOutputStream(
											new FileOutputStream(outDir + Path.SEPARATOR + "out"));
	    try 
	    {
	      while (reader.next(key, value))
	      {
	    	  String s1 = key.toString();
	    	  String s2 = Float.valueOf(value.get()).toString();
	    	  Util.println(s1 + ":" + s2);
	    	  for (int i = 0; i < s1.length(); ++i) 
			  {
					bufStream.write((byte)s1.charAt(i));
			  }
	    	  bufStream.write((byte)'\t');
	    	  for (int i = 0; i < s2.length(); ++i) 
			  {
					bufStream.write((byte)s2.charAt(i));
			  }
			  bufStream.flush();				
	      }
	    } 
	    finally 
	    {
	      reader.close();
	      bufStream.close();
	    }
	    Util.printLine();
	    System.out.println();    
    }
    else 	// if (!split)
    {
	    // copy the output file from hdfs output to local dir
	    hdfs = FileSystem.get(conf);
	    Util.println("copying the result to local dir...");
		startTime = new Date();
		hdfs.copyToLocalFile(new Path(Util.hdfsOutDir, "part-00000"), new Path(outDir, "out"));
		endTime = new Date();
		Util.println("done copying.");
		Util.println("Time spent to copy the result file from hdfs to local dir: " 
				+ (int) (endTime.getTime() - startTime.getTime()) + "ms");
		System.out.println();
		Util.printLine();
	    Util.println("results:");
	    Util.printLine();
	    FileReader fr = new FileReader(outDir + Path.SEPARATOR + "out");
		BufferedReader br = new BufferedReader(fr);
		String s;
		while((s = br.readLine()) != null)
		{	
			Util.println(s);
		}
		fr.close();
		Util.printLine();
		System.out.println();
				
    }
  }

  public int run(String [] args) throws IOException 
  {
    runJob();
    return 0;
  }
}

