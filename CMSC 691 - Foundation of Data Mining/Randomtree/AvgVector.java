/*
 * AvgVector.java
 *
 * Created on June 2, 2005, 6:01 PM
 */

package DDMT.Nodes;

import java.util.*;
/**
 *
 * @author  kanishk1
 */
public class AvgVector 
{
    public Point Avg;
    public int size;
    /** Creates a new instance of AvgVector */
    public AvgVector(int d) 
    {
        Avg = new Point(d);
        
        this.size = 0;
    }
    
    public AvgVector(Point a, int size, int d)
    {
        Avg = new Point(d);
        for (int i=0;i<a.d;i++)
            this.Avg.x[i] = a.x[i];
        this.size = size;
    }
    
    //compute the average for every features, column mean
    public AvgVector(Vector a, int d)
    {
        this.Avg = new Point(d);
        for (int i=0;i<a.size();i++)
        {
            Point temp = (Point)a.elementAt(i);
            for (int j=0;j<d;j++)
                this.Avg.x[j] += temp.x[j];
        }
        
        if (a.size()!=0)
        {
            for (int j=0;j<d;j++)
                this.Avg.x[j] /= a.size();
        }
        else
        {
            for (int j=0;j<d;j++)
                this.Avg.x[j] = 0.0;
        }
        this.size = a.size();
    }
}

