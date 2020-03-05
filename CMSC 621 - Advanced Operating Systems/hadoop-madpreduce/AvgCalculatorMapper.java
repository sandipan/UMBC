/*-----------------------------------------------------------------
						Sandipan Dey
 						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * AvgCalculatorMapper
 *
 * Maps (key, Text) -> (Text, Text).
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class AvgCalculatorMapper extends MapReduceBase
    implements Mapper<LongWritable, Text, Text, Text> 
{
  public AvgCalculatorMapper() { }

  public void map(LongWritable key, Text value, OutputCollector<Text, Text> output,
      Reporter reporter) throws IOException, NumberFormatException 
  {
	float sum = 0;
	long count = 0;
	StringTokenizer tokenizer = new StringTokenizer(value.toString());
    
	while (tokenizer.hasMoreTokens()) 
    {
      String tok = tokenizer.nextToken();
      sum += Float.parseFloat(tok);
      ++count;
    }
    output.collect(new Text("Avg"), new Text(Float.toString(sum) + "_" + Long.toString(count)));
  }
}
