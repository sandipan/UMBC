/*-----------------------------------------------------------------
					Sandipan Dey
					CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.FloatWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;

/**
 * MedianCalculatorMapper with splitting 
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MedianSCalculatorMapper extends MapReduceBase
    implements Mapper<Text, NullWritable, FloatWritable, LongWritable> 
{
	private LongWritable one = new LongWritable(1);
	public MedianSCalculatorMapper() { }

    public void map(Text key, NullWritable value, OutputCollector<FloatWritable, LongWritable> output,
      Reporter reporter) throws IOException 
    {
    	StringTokenizer tokenizer = new StringTokenizer(key.toString());
        float f;
        
    	while (tokenizer.hasMoreTokens()) 
    	{
    		String tok = tokenizer.nextToken();
    		try
    		{
    			f = Float.parseFloat(tok);
    	    	output.collect(new FloatWritable(f), one);    	
    		}
    		catch (NumberFormatException e)
    		{
    			if (tok.charAt(0) == '#')
    			{
    				f = Float.NEGATIVE_INFINITY;
    		    	output.collect(new FloatWritable(f), 
    		    			new LongWritable(Long.parseLong(tok.substring(1))));    	
    			}
    		}
    	}
    }
}
