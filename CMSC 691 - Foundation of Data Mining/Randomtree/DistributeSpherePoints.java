/*
 * DistributeSpherePoints.java
 *
 * Created on October 19, 2005, 4:34 PM
 */

package DDMT.Nodes;
import java.util.Random;
import java.util.Vector;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
/**
 *
 * @author  kanishk1
 */
public class DistributeSpherePoints 
{
    Vector points = new Vector();
    int numPoints;      //number of points reqd to cover the sphere
    int d;              //dimension of the problem
    int counter = 0;
    int maxIter ;

    public DistributeSpherePoints(int n, int dimension, int maxIter) 
    {
        int minp1,minp2;
        double mind,maxd,dist;
        
        
        Random r = new Random();
        r.setSeed(1000);
        
        this.numPoints = n;
        this.d = dimension;
        this.maxIter = maxIter;
        
        for (int i=0;i<this.numPoints;i++)
        {
            Point p = new Point(this.d);
            for (int j=0;j<this.d;j++)
                p.x[j] = r.nextDouble();
            p.membership = -1;
            this.points.add(p);
        }
//        this.PrintPoints(this.points);
        this.points = this.Normalize(points);
//        this.PrintPoints(this.points);
        
        while (counter < this.maxIter) 
        {
        /* Find the closest two points */
            minp1 = 0;
            minp2 = 1;
            mind = Distance((Point)points.elementAt(minp1),(Point)points.elementAt(minp2));
            maxd = mind;
            for (int i=0;i<this.numPoints-1;i++) 
            {
                for (int j=i+1;j<this.numPoints;j++) 
                {
                    if ((dist = Distance((Point)points.elementAt(i),(Point)points.elementAt(j))) < mind) 
                    {
                       mind = dist;
                       minp1 = i;
                       minp2 = j;
                    }
                    if (dist > maxd)
                       maxd = dist;
                }
            }

            /*
               Move the two minimal points apart, in this case by 1%
               but should really vary this for refinement
            */
            Point p1 = (Point)points.elementAt(minp1);
            Point p2 = (Point)points.elementAt(minp2);
            
            Point t1 = new Point(this.d);
            Point t2 = new Point(this.d);
            
            for (int i=0;i<this.d;i++)
            {
                t1.x[i] = p1.x[i] + 4.01 * (p2.x[i] - p1.x[i]);
                t2.x[i] = p1.x[i] - 4.01 * (p2.x[i] - p1.x[i]);
            }
            
            
            
            points.setElementAt(t1, minp2);
            points.setElementAt(t2, minp1);
            counter++;
        }    
        this.Normalize(points);
//        this.PrintPoints(points);
    }
    
    public static void main(String[] args) 
    {
        int dimension = 1000;
        int numPCA = 10;
        int dbig = 5*dimension;
        int dsmall = 5*numPCA;
        //for bigger problem
        DistributeSpherePoints ds = new DistributeSpherePoints(dbig,dimension,20);
        WriteSpherePoints(ds.GetPoints(),1,dimension,numPCA);
        //for smaller problem
        ds = new DistributeSpherePoints(dsmall,numPCA,20);
        WriteSpherePoints(ds.GetPoints(),0,dimension,numPCA);
        System.out.println("Prog Ended");
    }
   
    public static void WriteSpherePoints(Vector Points,int size,int dimension,int numPCA)
    {
        if (size==1)
        {
            String Pts = "";
            for (int i=0;i<Points.size();i++)
            {
                Point p = new Point(dimension);
                p = (Point)Points.elementAt(i);
                for (int j=0;j<p.d;j++)
                    Pts = Pts.concat(""+p.x[j]+" ");
                Pts = Pts.concat("\n");
            }
            WriteFile("c:\\Data\\SpherePointsBig.txt", Pts);
        }
        else
        {
            String Pts = "";
            for (int i=0;i<Points.size();i++)
            {
                Point p = new Point(numPCA);
                p = (Point)Points.elementAt(i);
                for (int j=0;j<p.d;j++)
                    Pts = Pts.concat(""+p.x[j]+" ");
                Pts = Pts.concat("\n");
            }
            WriteFile("c:\\Data\\SpherePointsSmall.txt", Pts);
        }
    }
    
    public static void WriteFile(String filename,String fileData)
    {
        try
        {
            BufferedWriter out = new BufferedWriter(new FileWriter(filename));

            out.write(fileData);
            out.close();
        }
        catch (IOException e)
        {
            System.out.println("Writing exception");
        }
    }
    
    public Vector GetPoints()
    {
        return this.points;
    }
    
    public Vector Normalize (Vector Points)
    {
        for (int i=0;i<Points.size();i++)
        {
            double val = 0.0;
            Point temp = (Point)Points.elementAt(i);
            for (int j=0;j<temp.d;j++)
                val += temp.x[j]*temp.x[j];
            for (int j=0;j<temp.d;j++)
                temp.x[j]/=Math.sqrt(val);
            
            Points.setElementAt(temp, i);
        }
        return Points;
    }
    
    public double Distance(Point a, Point b)
    {
        double dist = 0.0;
        for (int i=0;i<a.d;i++)
            dist += Math.pow(a.x[i]-b.x[i],2);
        
        dist = Math.sqrt(dist);
        return dist;
    }
    
    public void PrintPoints(Vector v)
    {
        for (int i=0;i<v.size();i++)
        {
            Point p = (Point)v.elementAt(i);
            
            for (int j=0;j<p.d;j++)
                System.out.print(p.x[j]+" ");
            System.out.print("\n");
        }
        System.out.println("\n\n");
    }
}
