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
 * CountCalculatorMapper
 *
 * Maps (key, Text) -> (Text, LongWritable).
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class CountCalculatorMapper extends MapReduceBase
    implements Mapper<LongWritable, Text, Text, LongWritable> 
{
  private Text countKey = new Text("Count");
	
  public CountCalculatorMapper() { }

  public void map(LongWritable key, Text value, OutputCollector<Text, LongWritable> output,
      Reporter reporter) throws IOException 
  {
      output.collect(countKey, new LongWritable(new StringTokenizer(value.toString()).countTokens()));
  }
}
