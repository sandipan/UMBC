/*-----------------------------------------------------------------
						Sandipan Dey
 						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;

/**
 * MedianCalculatorReducer
 *
 * Calculates the median
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MedianCalculatorReducer extends MapReduceBase
	implements Reducer<FloatWritable, LongWritable, Text, FloatWritable> 
{
	long count = 0;
	long totalCount = -1;
	long medianCount = -1;
	float median = 0;
	Text medianKey = new Text("Median");
	
	public void reduce(FloatWritable key, Iterator<LongWritable> values,
			OutputCollector<Text, FloatWritable> output, Reporter reporter) throws IOException 
	{
		float f = key.get();
		
		if (totalCount == -1 && f == Float.NEGATIVE_INFINITY && values.hasNext())
		{	
			totalCount = values.next().get();
			medianCount = (totalCount % 2 == 1) ? (totalCount + 1) / 2 : totalCount / 2;
			//output.collect(new Text("MedianIndex"), new FloatWritable(medianCount));
		}
		else
		{
			while (values.hasNext()) 
			{
				//long oldCount = count;
				count += values.next().get();
				if (/*oldCount < medianCount &&*/ count == medianCount)
				{
					median += f;
					if (totalCount % 2 == 1)
					{
						output.collect(medianKey, new FloatWritable(median));
					}
				}
				else if (totalCount % 2 == 0 /*&& oldCount < medianCount*/ && count == medianCount + 1)
				{
					median += f;
					median /= 2.0;
					output.collect(medianKey, new FloatWritable(median));
				}
			}
			//output.collect(new Text(Long.toString(count)), new FloatWritable(f));
		}
	}
}
