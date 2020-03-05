/*-----------------------------------------------------------------
					Sandipan Dey
					CSEE - 621
-----------------------------------------------------------------*/


package Calculator;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * AvgCalculatorMapper
 *
 * Avg Mapper with splitting
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class AvgSCalculatorMapper extends MapReduceBase
    implements Mapper<Text, NullWritable, Text, Text> 
{
    public AvgSCalculatorMapper() { }

    public void map(Text key, NullWritable value, OutputCollector<Text, Text> output,
      Reporter reporter) throws IOException, NumberFormatException 
    {
      float sum = 0;
  	  long count = 0;
  	  StringTokenizer tokenizer = new StringTokenizer(key.toString());
   	  
	  while (tokenizer.hasMoreTokens()) 
	  {
	    String tok = tokenizer.nextToken();
	    sum += Float.parseFloat(tok);
	    ++count;
	  }
	  
	  output.collect(new Text("Avg"), new Text(Float.toString(sum) + "_" + Long.toString(count)));
    }
}
