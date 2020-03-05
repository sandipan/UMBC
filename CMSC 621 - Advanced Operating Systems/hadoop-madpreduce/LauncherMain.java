/*-----------------------------------------------------------------
						Sandipan Dey
 						CSEE - 621
-----------------------------------------------------------------*/

package Calculator;

import java.util.Properties;
import java.util.Scanner;
import java.io.File;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

/**
 * LauncherMain
 *
 * main launcher class
 *
 */
public class LauncherMain 
{
	  static int DefaultMaps = 10;
	  static int DefaultReduces = 1;
	  static String [] Algorithms = {"MinMax", "Max", "Avg", "Median", "Count"};
	                    
	  public static void main(String [] args) throws Exception 
	  {
		Util.clear();
		if (args.length == 0)
		{
			Util.printUsage();
			return;
		}
		
		Util.printSignature();
		
		String inFile = null;
		String outDir = null;
		int numMaps = DefaultMaps;
		int numReduces = DefaultReduces;
		boolean split = false; 
		String algo = null;
		boolean benchmark = false; 
		String ermsg = null;
		
		try
		{
			if (args[0].equalsIgnoreCase("-console"))
			{
			    System.out.println();
			    Scanner in = new Scanner(System.in);
			    Util.print("Enter input file path to read from:");
			    inFile = in.nextLine();
			    Util.print("Enter output directory to store output:");
			    outDir = in.nextLine();
			    Util.println("Enter map-reduce algo to run:");
			    for (int i = 0; i < Algorithms.length; ++i)
			    {
			    	Util.println("(" + (i + 1) + ")" + Algorithms[i]);			    
			    }
			    Util.print("Algorithm to run:");
			    algo = Algorithms[in.nextInt() - 1];
			    Util.print("Enter maximum number of || map tasks (defaults to 10) :");
			    numMaps = in.nextInt();
			    //Util.print("Enter number of || reduce tasks (defaults to 1):");
			    //numReduces = in.nextInt();
			    Util.print("Do you want the program to split & distribute the input file to hdfs [yes|no]:");
			    String sp = in.nextLine();
			    split = sp.equalsIgnoreCase("yes");
			    in.close();            
			    System.out.println();
		   	}
			else
			{
			    Properties p = Util.readPropertiesFile(args[0]);
			    inFile = p.getProperty("InputFile").trim();
			    outDir = p.getProperty("OutputDir").trim();
			    algo = p.getProperty("Algo").trim();
			    try
			    {
			    	String map = p.getProperty("MaxMaps");
			    	numMaps = map == null ? -1 : Integer.parseInt(map.trim());
			    	//numReduces = Integer.parseInt(p.getProperty("MaxReduces"));
			    }
			    catch (NumberFormatException e)
			    {
			    	Util.println("MaxMap must be a +ve integer");
			    	numMaps = -1;
			    }
			    String sp = p.getProperty("Split") ;
			    split = sp != null ? sp.trim().equalsIgnoreCase("yes") : false;
			    String bm = p.getProperty("Benchmark");
			    benchmark = bm != null ? bm.trim().equalsIgnoreCase("yes") : false;
			}

			// error handling
			if (inFile == null || !new File(inFile).exists())
			{
				ermsg = "Input file " + inFile + " does not exist!Quitting!";
			}
			else if (outDir == null || !new File(outDir).exists())
			{
				ermsg = "Output directory " + outDir + " does not exist!Quitting!";
			}
			else
			{
				boolean found = false;
				if (algo != null)
				{
					for (int i = 0; i < Algorithms.length; ++i)
					{
						if (algo.equalsIgnoreCase(Algorithms[i]))
						{
							found = true;
							break;
						}
					}
				}
				if (!found)
				{
					ermsg = "Invalid algorithm name! Quitting!";
				}
			}
		}
		catch (Exception e)
		{
			ermsg = "Error in input!Quitting!";
	        }

		if (ermsg != null)
		{
			System.out.println();
			Util.printLine();
			Util.printErr(ermsg);
			Util.printLine();
			System.out.println();
			return;
		}

		if (benchmark)
		{
			int [][] times = new int[Algorithms.length][10];
			Tool t = null;
			for (int i = 0; i < Algorithms.length; ++i)
			{
				algo = Algorithms[i];
				for (int j = 0; j < 10; ++j)
				{					
					if (algo.equalsIgnoreCase("MinMax"))
					{
						t = new MinMaxLauncher(inFile, outDir, (j + 1) * 1, 1, split);
						ToolRunner.run(t, args);
						times[i][j] = ((MinMaxLauncher)t).timeInSec;
					}
					else if (algo.equalsIgnoreCase("Max"))
					{
						t = new MaxLauncher(inFile, outDir, (j + 1) * 1, 1, split);
						ToolRunner.run(t, args);
						times[i][j] = ((MaxLauncher)t).timeInSec;
					}
					else if (algo.equalsIgnoreCase("Avg"))
					{
						t = new AvgLauncher(inFile, outDir, (j + 1) * 1, 1, split);
						ToolRunner.run(t, args);
						times[i][j] = ((AvgLauncher)t).timeInSec;
					}
					else if (algo.equalsIgnoreCase("Median"))
					{
						t = new MedianLauncher(inFile, outDir, (j + 1) * 1, 1, split);
						ToolRunner.run(t, args);
						times[i][j] = ((MedianLauncher)t).timeInSec;
					}
					else if (algo.equalsIgnoreCase("Count"))
					{
						t = new CountLauncher(inFile, outDir, (j + 1) * 1, 1, split);
						ToolRunner.run(t, args);
						times[i][j] = ((CountLauncher)t).timeInSec;
					}
				}						
			}
			Util.print("Algo");
			for (int j = 0; j < 10; ++j)
			{
				System.out.print('\t');
				Util.print(Integer.toString((j + 1) * 1));
			}
			System.out.println();
			for (int i = 0; i < Algorithms.length; ++i)
			{
				Util.print(Algorithms[i]);
				for (int j = 0; j < 10; ++j)
				{
					System.out.print('\t');
					System.out.print(times[i][j]);
				}
				System.out.println();;
			}
			return;
		}
		
		Tool t = null;
		
		if (algo.equalsIgnoreCase("MinMax"))
		{
			t = new MinMaxLauncher(inFile, outDir, numMaps, numReduces, split);
		}
		else if (algo.equalsIgnoreCase("Max"))
		{
			t = new MaxLauncher(inFile, outDir, numMaps, numReduces, split);
		}
		else if (algo.equalsIgnoreCase("Avg"))
		{
			t = new AvgLauncher(inFile, outDir, numMaps, numReduces, split);
		}
		else if (algo.equalsIgnoreCase("Median"))
		{
			t = new MedianLauncher(inFile, outDir, numMaps, numReduces, split);
		}
		else if (algo.equalsIgnoreCase("Count"))
		{
			t = new CountLauncher(inFile, outDir, numMaps, numReduces, split);
		}
		
		System.out.println();
	    	Util.println("starting algorithm: " + algo);
	    	System.out.println();
	    
		int ret = ToolRunner.run(t, args);
		System.exit(ret);
	  }
}
