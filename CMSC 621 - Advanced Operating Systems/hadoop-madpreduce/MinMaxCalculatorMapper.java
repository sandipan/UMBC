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
 * MinMaxCalculatorMapper
 *
 * Maps the min and max to a float.
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MinMaxCalculatorMapper extends MapReduceBase
    implements Mapper<LongWritable, Text, Text, FloatWritable> 
{
  float min = Float.POSITIVE_INFINITY;
  float max = Float.NEGATIVE_INFINITY;
  Text minKey = new Text("Min");
  Text maxKey = new Text("Max");
  
  public MinMaxCalculatorMapper() { }

  public void map(LongWritable key, Text value, OutputCollector<Text, FloatWritable> output,
      Reporter reporter) throws IOException, NumberFormatException 
  {
    StringTokenizer tokenizer = new StringTokenizer(value.toString());
    while (tokenizer.hasMoreTokens()) 
    {
      String tok = tokenizer.nextToken();
      float f = Float.parseFloat(tok);
      if (f < min)
      {
    	  min = f;
      }
      if (f > max)
      {
    	  max = f;
      }
    }
    output.collect(minKey, new FloatWritable(min));
    output.collect(maxKey, new FloatWritable(max));
  }
}
