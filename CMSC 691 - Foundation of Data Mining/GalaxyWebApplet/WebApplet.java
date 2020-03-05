import java.applet.Applet;
import java.awt.Graphics;
import java.awt.Color;
import java.util.Vector;
import java.awt.*;
import java.net.*;
import java.io.*;
import java.lang.Math;
import java.math.BigDecimal;

import javax.swing.JOptionPane;

public class WebApplet extends Applet {

	final int X0 = 250;
	final int Y0 = 200;
	final int X0Z = 100;
	final int Y0Z = 300;
	final int X = 500;
	final int Y = 50;
	final double L = Math.sqrt((X0Z-X0)*(X0Z-X0)+(Y0Z-Y0)*(Y0Z-Y0));
	final double ZCF = 0.5;

	Image dBuffer, eBuffer;
	Graphics dContext;
	Vector<Data> data = new Vector<Data>();
	int width, height;
	
	public String error = "null";
	
	double xmax = -9999, xmin = 9999;
	double ymax = -9999, ymin = 9999;
	double zmax = -9999, zmin = 9999;
	int index = -1;
	
	public void init()
	{
		try
		{
			width = Integer.parseInt(getParameter("Width")) / 2;
			height = Integer.parseInt(getParameter("Height")) / 2;
			xmax = width / 2;
			ymax = height / 2; 
			xmin = -width / 2;
			ymin = -height / 2;
		}
		catch (Exception e)
		{
		}
		dBuffer = createImage(width * 2, height * 2);
		eBuffer = createImage(width * 2, height * 2);
		dContext = dBuffer.getGraphics();	
		
		/*Data d = new Data(); 
		String [] coords = null;
		try
		{
			 InputStream in = new URL(getCodeBase(), "vec.txt").openStream();
			  BufferedReader bf = new BufferedReader(new InputStreamReader(in));
			  String line;
			  if ((line = bf.readLine()) != null){
					coords = line.split("\\s+");
					d.x = 2000 * Double.parseDouble(coords[0]);
					d.y = 2000 * Double.parseDouble(coords[1]);
					d.z = 2000 * Double.parseDouble(coords[2]);
			  }
			  JOptionPane.showMessageDialog(this, coords.length, "" + d.x + " " + d.y + " " + d.z, JOptionPane.OK_OPTION);
		}
		catch(Exception e)
		{
		}*/
	}
	
	public void callWebMethod(String algorithm, String input)
	{
		WebServiceSimulationThread t = new WebServiceSimulationThread(this, algorithm, input); 
	    //JOptionPane.showMessageDialog(this, error, "Hi", JOptionPane.OK_OPTION);
	}
	
	void plot(double x,double y,double z)
	{
		double tx=X0-(y/L)*(X0-X0Z);
		double ty=Y0+y/L*(Y0Z-Y0);
		
		dContext.setColor(new Color((int)(255 * (x - xmin) / (xmax 	- xmin)), (int)(255 * (y - ymin) / (ymax - ymin)), (int)(255 * (z - zmin) / (zmax - zmin))));
		dContext.drawOval((int)(tx+x), (int)(ty-ZCF*z), 2, 2);
		//dContext.drawLine(X0, Y0, (int)(tx+x), (int)(ty-ZCF*z));
	}
	
	synchronized void notifyDataChanged()
	{
		for (int i = 0; i < data.size(); ++i)
		{
			Data d = data.get(i);
			if (d.z > zmax)
			{
				zmax = d.z;
			}
			else if (d.z < zmin)
			{
				zmin = d.z;
			}			
		}
		dataChanged = true;
		repaint();
	}
	
	boolean dataChanged = false;
	
	public class Data
	{
		public double x, y, z;
	}
	
    public void paint(Graphics g) 
	{
		if (dataChanged)
		{
			dBuffer.flush();
			dContext.drawImage(eBuffer, 0, 0, null);
			for (int i = 0; i < data.size(); ++i)
			{
				Data d = data.get(i);
				plot(d.x, d.y, d.z);
			}
			dataChanged = false;
			g.drawImage(dBuffer, 0, 0, this);
		}
		/*g.drawLine(X0,Y0,X,Y0);
		g.drawString("X",X,Y0);
		g.drawLine(X0,Y0,X0,Y);
		g.drawString("Y",X0Z,Y0Z);
		g.drawLine(X0,Y0,X0Z,Y0Z);
		g.drawString("Z",X0,Y);*/
		g.drawOval(X0, Y0, 2, 2);

    	g.drawRect(0, 0, width * 2 - 2, height * 2 - 2);
    }
}
