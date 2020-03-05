/*-----------------------------------------------------------------
					Sandipan Dey
					CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Date;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.FloatWritable;
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
 * MinMaxLauncher
 *
 * Finds Minimum & Maximum of Files
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MaxLauncher extends Configured implements Tool 
{
	  String inFile;
	  String outDir;
	  int numMaps;
	  int numReduces;
	  boolean split = true;
	  int timeInSec = 0;

	  public MaxLauncher(String inFile, String outDir, int numMaps, int numReduces, boolean split)
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
    JobConf conf = new JobConf(getConf(), MaxLauncher.class);
    conf.setJobName("Calculator"); 

    if (split)
    {
    	Util.splitInputFile(inFile, numMaps, conf);
    }
    else
    {
    	Util.distributeInputFile(inFile, numMaps, numReduces, conf);        	
    }
    
    FileInputFormat.addInputPath(conf, new Path(Util.hdfsInDir));
    FileOutputFormat.setOutputPath(conf, new Path(Util.hdfsOutDir));

    conf.setMapOutputKeyClass(Text.class);
    conf.setMapOutputValueClass(FloatWritable.class);
    conf.setOutputKeyClass(Text.class);
    conf.setOutputValueClass(FloatWritable.class);

    conf.setMapperClass(split ? MaxSCalculatorMapper.class : MaxCalculatorMapper.class);
    //conf.setCombinerClass(MaxCalculatorReducer.class); 
    conf.setReducerClass(MaxCalculatorReducer.class);

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
    Util.println("job finished in " + time + "sec");
    timeInSec += time;
    Util.printLine();
    System.out.println();

    if (split)
    {
	    // read outputs
	    Path inFile = new Path(Util.hdfsOutDir, "part-00000");
	    Text key = new Text();
	    FloatWritable value = new FloatWritable();
	    final FileSystem hdfs = FileSystem.get(conf);
	
	    System.out.println();
	    Util.printLine();
	    Util.println("results:");
	    Util.printLine();
	    SequenceFile.Reader reader = new SequenceFile.Reader(hdfs, inFile, conf);
	    try 
	    {
	      while (reader.next(key, value))
	      {
	    	  Util.println(key.toString() + ": " + value.get());
	      }
	    } 
	    finally 
	    {
	      reader.close();
	    }
	    Util.printLine();
	    System.out.println();    
    }
    else 	// if (!split)
    {
	    // copy the output file from hdfs output to local dir
	    FileSystem hdfs = FileSystem.get(conf);
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

