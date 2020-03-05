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
 * Calculates the Min and Max
 *
 * @author(Sandipan)
 */
@SuppressWarnings("deprecation")
public class MinMaxCalculatorReducer extends MapReduceBase
	implements Reducer<Text, FloatWritable, Text, FloatWritable> 
{

	float min = Float.POSITIVE_INFINITY;
	float max = Float.NEGATIVE_INFINITY;

	public void reduce(Text key, Iterator<FloatWritable> values,
			OutputCollector<Text, FloatWritable> output, Reporter reporter) throws IOException 
	{
		String op = key.toString(); 		
		if (op.equals("Min"))
		{
			while (values.hasNext()) 
			{
				float f = values.next().get();
				if (f < min)
				{
					min = f;
				}
			}
			output.collect(new Text("Min"), new FloatWritable(min));
		}
		else if (op.equals("Max"))
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
	
	/**
     * Reduce task done, write output to a file.
     */
	/*
    @Override
    public void cleanup(Context context) throws IOException {
      //write output to a file
      Path outDir = new Path(TMP_DIR, "out");
      Path outFile = new Path(outDir, "reduce-out");
      Configuration conf = context.getConfiguration();
      FileSystem fileSys = FileSystem.get(conf);
      SequenceFile.Writer writer = SequenceFile.createWriter(fileSys, conf,
          outFile, LongWritable.class, LongWritable.class, 
          CompressionType.NONE);
      writer.append(new LongWritable(numInside), new LongWritable(numOutside));
      writer.close();
    } */

}
