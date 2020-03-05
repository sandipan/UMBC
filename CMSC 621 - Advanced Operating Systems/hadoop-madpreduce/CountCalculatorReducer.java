/*-----------------------------------------------------------------
						Sandipan Dey
 						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

/**
 * CountCalculatorReducer
 *
 * Calculates the # of elements
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class CountCalculatorReducer extends MapReduceBase
	implements Reducer<Text, LongWritable, Text, NullWritable> 
{
	long count = 0;
	
	public void reduce(Text key, Iterator<LongWritable> values,
			OutputCollector<Text, NullWritable> output, Reporter reporter) throws IOException 
	{
		while (values.hasNext()) 
		{
			count += values.next().get();
		}
		output.collect(new Text("#" + count), NullWritable.get());
	}
}
