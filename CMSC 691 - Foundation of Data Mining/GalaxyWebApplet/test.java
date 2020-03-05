import java.io.*;
import java.lang.Math;
import java.math.BigDecimal;

class test 
{
 public static void main(String args[])
  {
  try{
  // Open the file that is the first 
  // command line parameter
  FileInputStream fstream = new FileInputStream("vec.txt");
  // Get the object of DataInputStream
  DataInputStream in = new DataInputStream(fstream);
  BufferedReader br = new BufferedReader(new InputStreamReader(in));
  String strLine;
  //Read File Line By Line
  while ((strLine = br.readLine()) != null)   {
  // Print the content on the console
	String [] coords = strLine.split("\\s+");
	double	d1 = 2000 * new BigDecimal(coords[1]).doubleValue();
	double d2 = 2000 * new BigDecimal(coords[2]).doubleValue();
	double d3 = 2000 * new BigDecimal(coords[3]).doubleValue();
	System.out.println(d1 +  " " + d2 + " " + d3);				  

  }
  //Close the input stream
  in.close();
    }catch (Exception e){//Catch exception if any
  System.err.println("Error: " + e.getMessage());
  }
  }
}