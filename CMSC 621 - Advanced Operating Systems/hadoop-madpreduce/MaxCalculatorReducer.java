/*-----------------------------------------------------------------
						Sandipan Dey
						CSEE - 621
-----------------------------------------------------------------*/


package Calculator;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

/**
 * MinMaxCalculatorReducer
 *
 * Calculates the Max
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MaxCalculatorReducer extends MapReduceBase
	implements Reducer<Text, FloatWritable, Text, FloatWritable> 
{

	float max = Float.NEGATIVE_INFINITY;

	public void reduce(Text key, Iterator<FloatWritable> values,
			OutputCollector<Text, FloatWritable> output, Reporter reporter) throws IOException 
	{
		while (values.hasNext()) 
		{
			float f = values.next().get();
			if (f > max)
			{
				max = f;
			}
		}
		output.collect(new Text("Max"), new FloatWritable(max));
	}
}
