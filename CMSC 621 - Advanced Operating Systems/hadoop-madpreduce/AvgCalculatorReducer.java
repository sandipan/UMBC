/*-----------------------------------------------------------------
						Sandipan Dey
 						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

/**
 * MinMaxCalculatorReducer
 *
 * Calculates the Avg / Sum / Count
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class AvgCalculatorReducer extends MapReduceBase
	implements Reducer<Text, Text, Text, Text> 
{
	float sum = 0;
	long count = 0;
	
	public void reduce(Text key, Iterator<Text> values,
			OutputCollector<Text, Text> output, Reporter reporter) throws IOException 
	{
		while (values.hasNext()) 
		{
			String sum_count = values.next().toString();
			String[] args = sum_count.split("_");
			sum += Float.parseFloat(args[0]);
			count += Long.parseLong(args[1]);
		}
		output.collect(new Text("Sum"), new Text(Float.toString(sum)));
		output.collect(new Text("Count"), new Text(Long.toString(count)));
		output.collect(new Text("Avg"), new Text(Float.toString(sum / count)));
	}
}
