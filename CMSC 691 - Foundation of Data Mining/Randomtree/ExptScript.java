package DDMT.Nodes;

import java.io.BufferedInputStream;
import java.io.BufferedWriter;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.Vector;

/*
 * Created on Aug 13, 2005
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */

/**
 * @author Kanishka
 *
 * TODO To change the template for this generated type comment go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
public class ExptScript 
{
	int numNodes;
	int numClusters;
	
	public void NormCalc(int n, int dimension) throws IOException
	{
            double[] ref = new double[dimension];
            int refsize = 0;

            
            this.numNodes = n;
            String FileName = "c:\\Quality0.txt";
            File f = new File(FileName);
            
            FileReader fr = new FileReader(FileName);
            BufferedReader reader = new BufferedReader(fr);
           
            String lines = null;
            			
            for (int i=0;i<this.numNodes;i++)
            {
                lines = reader.readLine();
                StringTokenizer st = new StringTokenizer(lines," ,\n");	
                
                for (int j=0;j<dimension;j++)
                    ref[j] += Double.parseDouble(st.nextToken());
                refsize += Integer.parseInt(st.nextToken());
            }
            for (int j=0;j<dimension;j++)
                ref[j]/= refsize;
            
            double[] sum = new double[dimension];
            int size = refsize;
            for (int j=0;j<dimension;j++)
                sum[j] = ref[j];
            
            while ((lines = reader.readLine())!=null)
            {
                    StringTokenizer st = new StringTokenizer(lines," ,\n");
                    int time = Integer.parseInt(st.nextToken());
                    for (int j=0;j<dimension;j++)
                        sum[j] += Double.parseDouble(st.nextToken());
                    refsize += Integer.parseInt(st.nextToken());

                    double norm = 0.0;
                    String toWrite = "";
                    if (refsize!=0)
                    {
                        for (int j=0;j<dimension;j++)
                            norm += sum[j]*sum[j];
                        norm = Math.sqrt(norm)/refsize;
                        toWrite = ""+time+" "+String.valueOf(norm)+"\n";
                    }
                    else
                    {
                            norm = 0.0;
                            toWrite = ""+time+" "+String.valueOf(norm)+"\n";
                    }
                    this.AppendFile("c:\\Norm0.txt",toWrite);
                    System.out.println(time);
            }
	}
	
	public void CalculateGlobalMsg(int NumClusters)
        {
            this.numClusters = NumClusters;
            Vector Time = new Vector();
            int CurrTime = 0;
            try
            {
                int MaxTime = 0;
                String line = null;
              //  String FileName = new String("D:\\L2Expts\\OnlyL2\\DataSetChange\\1000_15_50_1000\\NoofMsg.txt");
                String FileName = new String("c:\\NoofMsg.txt");
            //    System.out.println("Reading File "+FileName);
                File file = new File(FileName);
                byte buffer[] = new byte[(int)file.length()];
                BufferedInputStream input = new BufferedInputStream(new FileInputStream(file));
                input.read(buffer,0,buffer.length);
                input.close();					 
                String fileData = new String(buffer);

                String[] result = fileData.split("\n");
                String[] temp1 = result[result.length-1].split(" ");
                MaxTime = Integer.parseInt(temp1[0]);

                int TimeOld = 0;
                int TimeNew = 0;

                System.out.println("MaxTime : "+MaxTime);
                Time = new Vector (MaxTime);
                String TimeString = "";
                int DataMsg = 0;
                int CtrlMsg[] = new int[NumClusters];


                for (int i=0;i<result.length;i++)
                {
                    System.out.println(result[i]);
                    String[] lineres = result[i].split(" ");
                    TimeNew = Integer.parseInt(lineres[0]);

                    if (TimeNew!=TimeOld)
                    {
                        TimeString =""+TimeNew+" ";
                        TimeString = TimeString.concat(String.valueOf(DataMsg)+" ");
                        for (int j=0;j<NumClusters;j++)
                        {
                            TimeString = TimeString.concat(String.valueOf(CtrlMsg[j])+" ");
                        }
                        TimeString = TimeString.concat("\n");
                        Time.add(TimeString);
                        DataMsg = 0;
                        for (int j=0;j<NumClusters;j++)
                        {
                            CtrlMsg[j]=0;
                        }
                        TimeOld = TimeNew;
                        i--;
                    }
                    else
                    {
                      //  System.out.println("Line : "+result[i]);

                        if (lineres.length==3)          //ctrl msg
                        {
                        //    System.out.println("Line : "+result[i]);
                            //this is control msg
                            int x  = Integer.valueOf(lineres[1]).intValue();
                            int y  = Integer.valueOf(lineres[2]).intValue();
                         //   System.out.println(x+" "+y);

                         //   int y  = Integer.parseInt(st.nextToken());
                            CtrlMsg[x]+=y;
                        }
                        else
                        {
                            //this is a data msg
                            int x = Integer.valueOf(lineres[1]).intValue();
                            DataMsg+=x;
                         //   System.out.println(st.nextToken());
                        }  
                    }
                }


                String t = "";
                int TotalCtrlMsg1  = 0;
                int TotalCtrlMsg2  = 0;
                int TotalCtrlMsg3  = 0;
                for (int i=0;i<Time.size();i++)
                {
                    String temp = (String)Time.elementAt(i);
                    StringTokenizer st = new StringTokenizer(temp," ");
                    int Instance = Integer.parseInt(st.nextToken());
                    int Throw = Integer.parseInt(st.nextToken());
                    
                    if (Instance>0 && Instance<40000)
                    {
                        TotalCtrlMsg1 += Integer.parseInt(st.nextToken());
                    }
                    
                    if (Instance>65000 && Instance < 75000)
                    {
                        TotalCtrlMsg2 += Integer.parseInt(st.nextToken());
                    }
                    
                    if (Instance>110000 && Instance < 120000)
                    {
                        TotalCtrlMsg3 += Integer.parseInt(st.nextToken());
                    }
                }
                
                System.out.println(TotalCtrlMsg1);
                System.out.println(TotalCtrlMsg2);
                System.out.println(TotalCtrlMsg3);
                

//                this.WriteFile("d:\\L2Expts\\1000Nodes\\Communications.txt",t);

//                t = "";
                for (int i=1;i<Time.size();i++)
                {
                    t ="";
                    String PrevTemp = (String)Time.elementAt(i-1);
                    String CurrTemp = (String)Time.elementAt(i);

                    String[] result1 = PrevTemp.split(" ");
                    String[] result2 = CurrTemp.split(" ");
                    System.out.println("Time :"+result1[0]);
                    for (int j=1;j<this.numClusters+2;j++)
                    {
                     //   System.out.println(result2[j]+" "+result1[j]);
                        int CumMsg = Integer.parseInt(result2[j])+Integer.parseInt(result1[j]);
                        result2[j] = String.valueOf(CumMsg);
                    //    System.out.println(result2[j]+" "+result1[j]);
                    }

                    for (int j=0;j<result2.length;j++)
                        t = t.concat(result2[j]+" ");
                   // System.out.println(t);
                    Time.set(i, t);
                }

                t = "";
                for (int i=0;i<Time.size();i++)
                {
                    String temp = (String)Time.elementAt(i);
                    t = t.concat(temp);
                }
                this.WriteFile("c:\\Communications.txt",t);
            }
            catch(FileNotFoundException e)
            {
                System.out.println("Writing Exception");
            }
            catch(IOException e)
            {

            }
        }
	
        //calculate the number of correct peers:
        public void CorrectNess(int numPeers)
        {
            
            int Zeros = 0;
            int Ones = 0;

            this.numNodes = numPeers;
            String FileName = "c:\\XiInstance.txt";
            File f = new File(FileName);
            
            try
            {
                FileReader fr = new FileReader(FileName);
                BufferedReader reader = new BufferedReader(fr);

                String lines = null;

                for (int i=0;i<this.numNodes;i++)
                {
                    lines = reader.readLine();
                    StringTokenizer st = new StringTokenizer(lines," ,\n");	
                    int time = Integer.parseInt(st.nextToken());
                    
                    if (Integer.parseInt(st.nextToken())==0)
                        Zeros++;
                    else
                        Ones++;
                    System.out.println(Zeros+" "+Ones);
                }

                
                double PercentZero = (double)Zeros/(double)this.numNodes*100.0;
                String toWrite = "1 "+String.valueOf(PercentZero)+"\n";
                this.AppendFile("c:\\CorrectNess.txt",toWrite);
                System.out.println(Zeros+" "+Ones);

                while ((lines = reader.readLine())!=null)
                {
                        StringTokenizer st = new StringTokenizer(lines," ,\n");
                        int time = Integer.parseInt(st.nextToken());
                        int output = Integer.parseInt(st.nextToken());
                        if (output==0)
                        {
                            Zeros++;
                            Ones--;
                        }
                        else
                        {
                            Ones++;
                            Zeros--;
                        }

                        PercentZero = (double)Zeros/(double)this.numNodes*100.0;

                        toWrite = ""+time+" "+String.valueOf(PercentZero)+"\n";
                        this.AppendFile("c:\\CorrectNess.txt",toWrite);
                        System.out.println(time+" "+Ones+" "+Zeros+" "+output);
                }
                
            }
            catch(FileNotFoundException e)
            {
                System.out.println("Writing Exception");
            }
            catch(IOException e)
            {

            }
        }
        
        public void PoBoxMessages()
        {
//            String FileName = new String("C:\\PoBoxMessages.txt");
            String FileName = new String("C:\\PoBoxL2Messages.txt");
            int factor = 100000; 
            try
            {
                File file = new File(FileName);
                byte buffer[] = new byte[(int)file.length()];
                BufferedInputStream input = new BufferedInputStream(new FileInputStream(file));
                input.read(buffer,0,buffer.length);
                input.close();					 
            
                String fileData = new String(buffer);

                String[] result = fileData.split("\n");

                int CtrlMsg1 = 0;
                int CtrlMsg2 = 0;
                int CtrlMsg3 = 0;
                int CtrlMsg4 = 0;
               
                for (int i=0;i<result.length;i++)
                {
                    StringTokenizer st = new StringTokenizer(result[i]," ,\n"); 
                    int Time = Integer.parseInt(st.nextToken());
                    String temp = st.nextToken();
//                    System.out.println("Time : "+Time+" "+temp);
                    
                    //for L2
                    if (Time < 10*factor)
                    {
                        int NumMessages = Integer.parseInt(temp.trim());
                        CtrlMsg1+= NumMessages;
                    }
                    
                    if ((Time > 6*factor && Time < 10*factor)||(Time > 1*factor && Time < 5*factor))
                    {
                        int NumMessages = Integer.parseInt(temp.trim());
                        CtrlMsg2+= NumMessages;
                    }
                        
                    //for others
//                    int NumMessages = Integer.parseInt(temp.trim());
//                    CtrlMsg1+= NumMessages;
                }
              
                System.out.println("Total Messages : "+CtrlMsg1);
                System.out.println("Stationary Messages : "+CtrlMsg2);
        
//                this.WriteFile("c:\\Communications.txt",result[0]+"\n");
//                String t = "";
//                for (int i=1;i<result.length;i++)
//                {
//                    t ="";
//                    String PrevTemp = (String)result[i-1];
//                    String CurrTemp = (String)result[i];
//
//                    String[] result1 = PrevTemp.split(" ");
//                    String[] result2 = CurrTemp.split(" ");
//                  
//                    System.out.println("Time :"+result2[0]);
//                    t = result2[0]+" ";
//                    
//                    int CumMsg = Integer.parseInt(result2[1])+Integer.parseInt(result1[1]);
//                    t = t.concat(String.valueOf(CumMsg))+" \n";
//                    
//                    result[i] = "";
//                    result[i] = result[i].concat(t);                    
//                    this.AppendFile("c:\\Communications.txt",result[i]);
//                    
//                }
//
                
                
            }
            catch(IOException e)
            {
                
            }
        }
        
        public void TotalMsg()
        {
            String FileName = new String("D:\\k_means\\SampleSize\\K=3_1000_2.0\\PoBoxL2Messages.txt");
            int factor = 100000; 
            try
            {
                File file = new File(FileName);
                byte buffer[] = new byte[(int)file.length()];
                BufferedInputStream input = new BufferedInputStream(new FileInputStream(file));
                input.read(buffer,0,buffer.length);
                input.close();					 
            
                String fileData = new String(buffer);

                String[] result = fileData.split("\n");

                int Msg = 0;
                
               
                for (int i=0;i<result.length;i++)
                {
                    StringTokenizer st = new StringTokenizer(result[i]," ,\n"); 
                    int Time = Integer.parseInt(st.nextToken());
                    

                    int NumMessages = Integer.parseInt(st.nextToken());
                    Msg += NumMessages;
                        
                }
              
                System.out.println("Total Messages : "+Msg);
                
            }
            catch(IOException e)
            {
                
            }
        }

        
        public  void AppendFile(String filename,String fileData)
        {
            try 
            {
                BufferedWriter out = new BufferedWriter(new FileWriter(filename,true));

                out.write(fileData);
                out.close();
            }
            catch (IOException e) 
            {
                    System.out.println("Writing exception");
            }
        }
	
	public  void WriteFile(String filename,String fileData)
        {   
            try 
            {
                BufferedWriter out = new BufferedWriter(new FileWriter(filename,false));

                out.write(fileData);
                out.close();
            }
            catch (IOException e) 
            {
                    System.out.println("Writing exception");
            }
        }
        
        public void AverageAccuracy()
        {
            
            String FileName = "C:\\CorrectNess.txt";
            File f = new File(FileName);
            int factor = 100000;
            
            try
            {
                FileReader fr = new FileReader(FileName);
                BufferedReader reader = new BufferedReader(fr);

                String lines = null;
                lines = reader.readLine();
                
                StringTokenizer st = new StringTokenizer(lines," ,\n");
                int Time = Integer.parseInt(st.nextToken());
                double PercentCorrectinnerst = 0.0;
                double PercentCorrectouterst = 0.0;
                double PercentCorrectdy = 0.0;
                int countinnerst = 0;
                int countouterst = 0;
                int countdy = 0;
                
                while (lines!=null)
                {   
                    double temp = Double.parseDouble(st.nextToken());
   
                    if (Time > 10*factor && Time< 15*factor)
                    {
                        countdy++;
                        if (Time > 5*factor && Time < 10*factor)
                        {
                            double x = 100.0 - temp;
                            PercentCorrectdy +=x;
                        }
                        else
                            PercentCorrectdy +=temp;
                    }
                    
                    if (Time > 1*factor && Time<5*factor)
                    {
                        countinnerst++;
                        PercentCorrectinnerst +=temp;
                    }
                    if (Time > 6*factor && Time<10*factor)
                    {
                        countouterst++;
                        double x = 100.0 - temp;
                        PercentCorrectouterst +=x;
                    }
                    lines = reader.readLine();
                    if (lines!=null)
                    {
                        st = new StringTokenizer(lines," ,\n");
                        Time = Integer.parseInt(st.nextToken());
                    }
                }                
                System.out.println("Average Accuracy Inside Static : "+PercentCorrectinnerst/countinnerst);
                System.out.println("Average Accuracy Outside Static : "+PercentCorrectouterst/countouterst);
                System.out.println("Average Accuracy Overall : "+PercentCorrectdy/countdy);
            }
            catch(FileNotFoundException e)
            {
                System.out.println("Reading Exception");
            }
            catch(IOException e)
            {

            }
        }
        
        public void AverageSi(int numNodes)
        {
            this.numNodes = numNodes;
            
            try
            {
                String FileName = "C:\\Norm0.txt";
                File f = new File(FileName);

                byte buffer[] = new byte[(int)f.length()];
                BufferedInputStream input = new BufferedInputStream(new FileInputStream(f));
                input.read(buffer,0,buffer.length);
                input.close();					 
            
                String fileData = new String(buffer);

                String[] result = fileData.split("\n");
                       
                double Si = 0.0;
                String lines = "";
                for (int i=0;i<result.length;i++)
                {
                    lines = result[i];
                    StringTokenizer st = new StringTokenizer(lines," ,\n");	
                    int Time = Integer.parseInt(st.nextToken());
                    double temp = Double.parseDouble(st.nextToken());
                    Si += temp;
                }
                System.out.println("Average Si : "+(Si/result.length));
            }
            catch (IOException e) 
            {
                    System.out.println("Writing exception");
            }
            
        }
	
	public static void main(String[] args) 
	{
		ExptScript ex = new ExptScript();
		
		try 
                {
                    ex.NormCalc(200,5);
		} 
                catch (IOException e) 
                {
                    e.printStackTrace();
		}
//		ex.CalculateGlobalMsg(1);
//                ex.CorrectNess(200);
                ex.PoBoxMessages();
//                ex.AverageSi(200);
//                ex.AverageAccuracy();
//                ex.TotalMsg();
	}
}
