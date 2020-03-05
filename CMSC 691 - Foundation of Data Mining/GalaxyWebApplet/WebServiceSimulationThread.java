import java.lang.Thread;
import java.util.Random;
import java.io.*;
import java.net.*;
import java.lang.Math;
import java.math.BigDecimal;

public class WebServiceSimulationThread extends Thread 
{
	WebApplet website;
	String algorithm;
	String input;
	Random randomGenerator;
    
	WebServiceSimulationThread(WebApplet website, String algorithm, String input)
	{
		this.website = website;
		this.algorithm = algorithm;
		this.input = input;
		randomGenerator = new Random();
		start();		
	}
	
	public void run() 
	{
		/*
		int interval = randomGenerator.nextInt(1000);
				
		try
		{
			// Do Processing
			Thread.sleep(interval);
		}
		catch (Exception e)
		{
			
		}
        */
		try
		{
			while (true)
			{
				setResult();
				Thread.sleep(2000);
			}
		}
		catch (InterruptedException e)
		{			
		}
    }

	synchronized void setResult()
	{
		int n = website.data.size();
		
		/* */
		for (double i = -125; i < 125; i += 0.25)
		for (double j = -125; j < 125; j += 0.25)
		{
			WebApplet.Data d = website. new Data();
			d.x = i;
			d.y = j;
			d.z = i * j * Math.exp((- i - j) / 10);//200 * Math.sin((i + j) / 10.0);//200 * Math.sin((i * i + j * j) / 500.0);//200 * Math.sin((i + j) / 10.0); //i * j * Math.exp((- i - j) / 10);//200 * Math.sin((i + j) / 10.0);//25 * i * j * Math.exp((- i - j) / 25);
			website.data.add(d);
			++n;
		}
		/* */

		/* *\/
		String line;
		URL url = null;
		try
		{
			url = new URL(website.getCodeBase(), "vec.txt");
			InputStream in = url.openStream();
			BufferedReader bf = new BufferedReader(new InputStreamReader(in));
			while((line = bf.readLine()) != null)
			{
				WebApplet.Data d = website. new Data();
				String [] coords = line.split("\\s+");
				d.x = 2000 * Double.parseDouble(coords[0]);
				d.y = 2000 * Double.parseDouble(coords[1]);
				d.z = 2000 * Double.parseDouble(coords[2]);
				website.data.add(d);
				++n;
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
		/* */
	
		website.notifyDataChanged();
	}
}