/*-----------------------------------------------------------------
					Sandipan Dey
					CSEE - 621
-----------------------------------------------------------------*/


package Calculator;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * CountCalculatorMapper
 *
 * Count Mapper with splitting
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class CountSCalculatorMapper extends MapReduceBase
    implements Mapper<Text, NullWritable, Text, LongWritable> 
{
	private Text countKey = new Text("Count");
	
	public CountSCalculatorMapper() { }

    public void map(Text key, NullWritable value, OutputCollector<Text, LongWritable> output,
      Reporter reporter) throws IOException 
    {
      output.collect(countKey, new LongWritable(new StringTokenizer(key.toString()).countTokens()));
    }
}
