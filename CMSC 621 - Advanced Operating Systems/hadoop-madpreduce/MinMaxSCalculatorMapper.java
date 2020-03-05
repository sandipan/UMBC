/*-----------------------------------------------------------------
						Sandipan Dey
						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.NullWritable;
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
public class MinMaxSCalculatorMapper extends MapReduceBase
    implements Mapper<Text, NullWritable, Text, FloatWritable> 
{
  float min = Float.POSITIVE_INFINITY;
  float max = Float.NEGATIVE_INFINITY;
 	
  public MinMaxSCalculatorMapper() { }

  public void map(Text key, NullWritable value, OutputCollector<Text, FloatWritable> output,
      Reporter reporter) throws IOException, NumberFormatException 
  {
    StringTokenizer tokenizer = new StringTokenizer(key.toString());
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
    output.collect(new Text("Min"), new FloatWritable(min));
    output.collect(new Text("Max"), new FloatWritable(max));
  }
}
