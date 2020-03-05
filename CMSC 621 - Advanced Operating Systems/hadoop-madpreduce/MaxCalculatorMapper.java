/*-----------------------------------------------------------------
					Sandipan Dey
					CSEE - 621
-----------------------------------------------------------------*/


package Calculator;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * AvgCalculatorMapper
 *
 * Maps the max to a float.
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MaxCalculatorMapper extends MapReduceBase
    implements Mapper<LongWritable, Text, Text, FloatWritable> 
{
  float max = Float.NEGATIVE_INFINITY;
  Text maxKey = new Text("Max"); 	
  
  public MaxCalculatorMapper() { }
    
  public void map(LongWritable key, Text value, OutputCollector<Text, FloatWritable> output,
      Reporter reporter) throws IOException, NumberFormatException 
  {
    StringTokenizer tokenizer = new StringTokenizer(value.toString());
    while (tokenizer.hasMoreTokens()) 
    {
      String tok = tokenizer.nextToken();
      float f = Float.parseFloat(tok);
      if (f > max)
      {
    	  max = f;
      }
    }
    output.collect(maxKey, new FloatWritable(max));
  }
}
