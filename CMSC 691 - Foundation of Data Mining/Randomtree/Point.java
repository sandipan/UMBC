/*
 * Point.java
 *
 * Created on May 31, 2005, 12:48 PM
 */

package DDMT.Nodes;

import java.util.*;
/**
 *
 * @author  kanishk1
 */
public class Point 
{
    public double[] x;
    public int d;  //dimension of the problem
    public int membership;
    
    public Point(int d) 
    {
        this.d = d;
        this.x = new double[this.d];
        
        for (int i=0;i<this.d;i++)
            x[i] = 0.0;
            
        this.membership = -1;
    }
    public Point(double a[], int mem)
    {
        this.d = a.length;
        this.x = new double[this.d];
        
        for (int i=0;i<a.length;i++)
            x[i] = a[i];
            
        this.membership = mem;
    }
    
    public double Norm()
    {
        double norm = 0.0;
        
        for (int i=0;i<this.d;i++)
            norm += this.x[i]*this.x[i];
        
        return Math.sqrt(norm);
        
    }
    public void print_point(int dim)
    {
    	System.out.println("Start to print one point:");
        for (int i = 0; i< dim ; i++)
        {
           System.out.println ("X["+i+"] = "+this.x[i]);
        }
    }
}
