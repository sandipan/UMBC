package DDMT.Nodes;


import java.util.*;
import java.io.*;

import jade.lang.acl.*;
import java.util.regex.*;
import DDMT.Agents.NodeAgent.AgentInterface;//not initial
import java.lang.Object;
import java.awt.geom.Point2D;
import java.lang.Math;

public class randomTreeNode extends NodeCode{
	
	//algorithm data structure
	int dimension = 2; //dimension of the feature vectors
	
	Vector S_i_instance = new Vector(); 
	Vector S_i = new Vector();    //original set of local points
    AvgVector[] X_ij;   // i sends to j
    AvgVector[] X_ji;   // i receives from j
    AvgVector K_i;      // knowledge
    AvgVector K_inew;   // chages in knowledge
    AvgVector[] A_ij;   // agreement
    AvgVector[] W_ij;   // withheld
    AvgVector S_i_instance_avg; // avg of all local points per instance

    double epsilon = 0.1;
    double NoiseProb = 0.1;

    int d = 4*dimension;
    Vector Perps = new Vector();
    ArrayList nbors = new ArrayList();
    double[] pattern_mean = new double[dimension];
    
    boolean Change = false;
    int iteration = 0;
    
    int timestamp = 0;
    
    int NUM_NODES = 6;
    
    double ChangeInXiX = 0.0;
    double ChangeInXiY = 0.0;
    int[] flag;        //-1 implies null, 0 implies send and 1 means receive
    //int TIME = 1;
    int IntervalofL2Msg     = 10;
    int LastL2MsgSent       = -10;

    int CorrectStatus = 0;
    Random r = new Random();
    int IntervalofDataAlert = 5;
    int LastDataMsgAlert = 0;
    boolean AlertFlag = false;

    double PercentPointstoSend = 10.0;

    double[] Received_Data_Sum = new double[dimension];
    int Received_Data_Size;
    
    int DataCollectionRound = -1;
    int PatternRound       = -1;
    int RecvdRound          = -1;
    int NewRoot             = -1;
    int OldRoot             = -1;
    int CentroidRound       = -1;
    
    
    Vector[] DataCollectionBuffers;
    int numOfTrees = 2; //number of random trees need to be built
  //The depth of the random tree
	//int RDT_depth = attrNumber/2;
	int RDT_depth = 3;
	int roundCount=0;
	buildTree newTree [] = new buildTree[numOfTrees];
	
    public randomTreeNode(){
    	
    }
    
    public void start() 
    {
    	System.out.println("#####################Start build the random tree################");
    	//class names which ideally will get from the plugin (server)
		//Here the class name is hard coded, will be changed later on
		String className="Astronomy, NSF, Science";
		roundCount+=1;
		//Total number of attributes
		//Should read from the feature vector file, now is hard coded
		int attrNumber=11;
		
		//set the attribute range that can be randomly selected
		int tStart=1;
		int tEnd=attrNumber-1;
			
		//------------------------------------------------------------------
		//create a hash table to store all the possible schemas
		//this hash table is not in use currently
		/*Map map = new HashMap();  //hash table
		map = new TreeMap();   //sorted map
		//Add key/value pairs to the map
		int[] schema= new int[attrNumber-1];
		
		String[] temp={"111","222","333","122","133","211","233","311","322","123"};
		for(int i=0;i<temp.length;i++){
			String[] elements = new String[attrNumber-1];
			for (int j=0;j<attrNumber-1;j++){
				char[] a = temp[i].toCharArray();
				if(j<temp[i].length())
					elements[j]=String.valueOf(a[j]);
				else
					elements[j]="0";					
			}
			//for(int j=0;j<attrNumber-1;j++)
				//System.out.println("elements is:"+elements[j]);
			
			int[] indices;
			CombinationGenerator x = new CombinationGenerator (elements.length, attrNumber-1);
			StringBuffer combination;
			while (x.hasMore ()) {
			  combination = new StringBuffer ();
			  indices = x.getNext ();
			  for (int k = 0; k < indices.length; k++) {
			    combination.append (elements[indices[k]]);
			  }
			  System.out.println ("the combination is:"+combination.toString ());
			}
		}*/
		//--------------------------------------------------------------------
		buildTreeSet();
		toStart(roundCount);
    }
    public void run(String m, int num){
    	System.out.println("node:"+Integer.toString(this.getNodeNumber())+"***************enter function: Run******************");
    	System.out.println("String m is:"+m);
    	System.out.println("the node that send the message:"+Integer.toString(num));
    	System.out.println("The time that receives the message is:"+this.getAgentInterface().getTime());
    	int factor = 150;
    	
    	if (this.getAgentInterface().getTime()>roundCount*factor){
    		//before start another round,save the pattern mean, which is the class probability for the leaf node
    		classdistribution(roundCount);
    		
    		System.out.println("start another round!");
    		roundCount++;
    		toStart(roundCount);
    	}
    	
    	//stop simulation
        if (this.getAgentInterface().getTime()>20*factor)
        {
        	String tempString="";
        	//write the class distribution into a file
        	for (int i=0;i<numOfTrees;i++){
        		BufferedWriter out1 = null;
    			String filename="D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\files\\tree"+i+"_classDistribution.txt";
    			try {
					out1 = new BufferedWriter(new FileWriter(filename));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
        		for (int j=0;j<RDT_depth*3;j++){
        			for(int k=0; k<this.dimension;k++){
        				if(k==this.dimension-1)
        					tempString+=newTree[i].leafnodelist[j].distributionStatistics[k];
        				else
        					tempString+=newTree[i].leafnodelist[j].distributionStatistics[k]+" ";
        			    
        			}
        			try {
						out1.write(tempString);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
        		}
        		try {
					out1.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
        	}
        			    
            ExptScript ex = new ExptScript();
            ex.PoBoxMessages();
            System.exit(0);
        }
        
//      System.out.println(m);
        if(m.equals("Empty Msg")){
    		this.getAgentInterface().sendMessageWithDelay(m,this.getNodeNumber(),IntervalofL2Msg);
    	}
        //when L2Msg!="" and time is not up, send the "Wait Msg"
        else if (m.startsWith("Wait Msg")){
        	//FetchMessageComponents(m,num);
            System.out.println("Waiting L2");
            String[] result = m.split("\n");
            for(int i=0;i<result.length;i++)
            	System.out.println("result["+i+"] is:"+result[i]);
            int waittime = Integer.parseInt(result[result.length-1]);
            this.getAgentInterface().pauseNode(waittime,"wake for L2");
            System.out.println("The wait time is:"+ waittime);
            
        }
        else if (m.startsWith("wake for L2")){
        	this.WakeUp();
        }
       /* else if (m.startsWith("WriteCentroid")) {
            String Msg = "Empty Msg";
            agentInterface.sendMessage(Msg, 100);
        }*/
        else if (m.startsWith("Wait Data Msg")){
        	System.out.println("Wait Data Msg:"+this.getNodeNumber());
        	//FetchMessageComponents(m,num);
            String[] result = m.split("\n");
        //    System.out.println("Waiting Data "+this.getNumber());
            for(int i=0;i<result.length;i++)
            	System.out.println("result["+i+"] is:"+result[i]);
            int waittime = Integer.parseInt(result[result.length-1]);
            this.getAgentInterface().pauseNode(waittime,"wake for data");
        //    this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Recvd by "+this.getNumber()+"\n");
        }
        else if (m.startsWith("wake for data")){
        	System.out.println("enter wake for data");
        	this.WakeUpForData();
        }
        /*else if (m.startsWith("DataGen")){
            //    System.out.println("Msg recvd from : "+num +" by "+getNumber()+" for new data generation");
            //    System.out.println(m);
                //extract the time of the message

    	    StringTokenizer st = new StringTokenizer(m,"\n");
            String Temp = st.nextToken();
            String[] Temp2 = Temp.split(" ");
            String numPointstoAdd = Temp2[Temp2.length-1];
            Temp = st.nextToken();
            String Time = st.nextToken();

            this.getAgentInterface().getTime() = Integer.parseInt(Time);
            //this.AppendFile(post_box_message,"Calling the Add point method********* at "+this.TIME);
            //System.out.println ("Calling the Add point method********* at "+this.TIME);
            //this.AppendFile(post_box_message,"The bin we are adding is "+bin+"time stamp of the bin: "+timestamp+"for the peer : "+this.getNumber());
            //System.out.println ("The bin we are adding is "+bin+"time stamp of the bin: "+timestamp);
               
            //this.AddPoint(Integer.parseInt(numPointstoAdd));
        }*/
       // else if (m.startsWith("Pattern")){
        else if (m.startsWith("PatternStates")){
            //when a peer gets a new model
        	System.out.println("m.startsWith(PatternStates)");
            boolean ForwardPattern = false;
            String[] result = m.split("\n");
            System.out.println("Pattern recvd from "+Integer.toString(num)+" by "+this.getNodeNumber());
            Neighbor n = null;

            this.RecvdRound = Integer.parseInt(result[result.length-3]);
            this.NewRoot = Integer.parseInt(result[result.length-2]);
            int Sender = num;
            //this.getAgentInterface().getTime() = Integer.parseInt(result[result.length-1]);
            this.DataCollectionRound = this.RecvdRound+1;


            String Msg = "";

            if (this.PatternRound==this.RecvdRound){
                if (this.NewRoot>=this.OldRoot)
                    ForwardPattern = true;
            }
            else
                ForwardPattern = true;
            System.out.println("ForwardPattern is:"+ForwardPattern);
            if (ForwardPattern){
                this.pattern_mean = new double[dimension];
                this.FetchPatterns(m);

                if (this.nbors.size()>1){
                    String PatternString = this.MakePatternString();
                    for (int i=0;i<this.nbors.size();i++){
                        //forward the centroids as soon as you get them...check if two centroids are for the same
                        //round and peer i<peer j
                        Neighbor nb = (Neighbor)nbors.get(i);
                        if (nb.getNodeNumber() != Sender){
                            n = (Neighbor)nbors.get(i);
                            Msg = Msg.concat(PatternString +"#"+n.getNodeNumber()+"#"+(int)(n.getDelay()*10.0)+"%");
                        }
                        this.flag[i] = -1;
                    }
                }
                this.flag[0] = -1;
                this.PatternRound = this.RecvdRound;
                this.OldRoot = this.NewRoot;
            }
            RunLocalL2(Msg);
        }
        else if (m.startsWith("DataSet")) {
            /////////////////////////added power method/////////////////////////////
            System.out.println("m.startsWith(DataSet)");
        	int index = -1;
            Neighbor n = null;
            String[] result = m.split("\n");
            for (int i=0;i<nbors.size();i++) {
                n = (Neighbor)nbors.get(i);
                if (n.getNodeNumber()==num) {
                    index = i;
                    break;
                }
            }
            
            System.out.println("DataSet recvd from "+Integer.toString(num) +" by "+this.getNodeNumber());

           // GetTime(m);

            //this.AppendFile("c:\\WakeUpData.txt", "Time : "+this.TIME+ " DataSet recvd from "+Integer.parseInt(result[result.length-2]) +" by "+this.getNumber()+"\n");
            if (index!=-1){
                // if not already received
                if (flag[index]!=1)
                    AccumulateRemoteData(m);
            }

            //set the flag to received
            this.flag[index] = 1;

            int NumRecvd=0;
            for (int i=0;i<this.nbors.size();i++){
                if (this.flag[i] == 1){
                    NumRecvd++;
                }
            }
            
            String DataMsg = "";
            index = -1;
            //the data will be propageted if the peer has received data from all the peers except one
            if (NumRecvd == nbors.size()-1){
                //the message may need to be propageted
                System.out.println("The NumRecvd is:"+NumRecvd+",which is equal to nbors.size()-1");
            	String DataSet = "";
                
                boolean ToSent = false;
                if (this.norm(this.K_i)>epsilon){
               	 ToSent = true;
               	 DataSet = this.CreateDataSet();
               	 
                }
                
                if (ToSent) {
                    //send the collectedData now
                    for (int i=0;i<this.nbors.size();i++) {
                        if (flag[i]==-1){
                            n = (Neighbor)nbors.get(i);
                            DataMsg = DataMsg.concat(DataSet+"#"+n.getNodeNumber()+"#"+(int)(n.getDelay()*10)+"%");
                            index = i;
                            if (!this.AlertFlag) {
                                this.AlertFlag = true;
                                this.LastDataMsgAlert = this.getAgentInterface().getTime();
                            }
                        }
                    }
                }

                if (DataMsg.equals(""))
                	this.AlertFlag = false;
                
	             String Msg = "";
                if (this.AlertFlag) {
                	if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert)	{
                        Msg = Msg.concat(DataMsg);
                        //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data sent by "+this.getNumber()+"\n");
                        this.flag[index] = 0;
                        this.AlertFlag = false;     //reset alert flag
                	}
        	    	else{
        	    		int waitTime = this.LastDataMsgAlert + this.IntervalofDataAlert - this.getAgentInterface().getTime();
                        Msg = Msg.concat("Wait Data Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                        //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Sent by "+this.getNumber()+"\n");
                    }
                }
                
                if (Msg.equals("")){
                    Msg = "Empty Msg";
                    System.out.print("print out the pattern mean!");
                    for (int i=0;i<this.dimension;i++)                        
                        System.out.print(this.pattern_mean[i]+" ");
                }
                
                //agentInterface.sendMessage(Msg, this.getNodeNumber());
                sendExtractedMessageforRun(Msg);
             }
             else if (NumRecvd == this.nbors.size()) {
                //compute new means and new covariance
                //first create root mean dataset by adding its own data
                double[] totalsum = this.CreateRootSum();
                int totalcount = this.CreateRootCount();
                
                System.out.println("*********NEW MEAN********************");
               
                //compute the mean
                for (int i=0;i<this.dimension;i++)
                {
                    this.pattern_mean[i] = totalsum[i]/totalcount;
                    System.out.print(this.pattern_mean[i]+" ");
                }
                System.out.println();
                
                this.NewRoot = this.getNodeNumber();
                this.OldRoot = this.getNodeNumber();
                        
                
                String PatternString = this.MakePatternString();

                for (int i=0;i<this.nbors.size();i++){
                    n = (Neighbor)nbors.get(i);
                    DataMsg = DataMsg.concat(PatternString+"#"+n.getNodeNumber()+"#"+(int)(n.getDelay()*10)+"%");
                    this.flag[i] = -1;
                    //System.out.println("Send patterns to "+n.getNumber());
                     //this.AppendFile(this.ouputDataFile+this.getNumber()+".txt","Send patterns to "+n.getNumber());
                }
                System.out.println("The DataMsg is:"+DataMsg);
                for (int i=0;i<this.Received_Data_Sum.length;i++)
                    this.Received_Data_Sum[i] = 0.0;
                this.Received_Data_Size = 0;
                
                this.RunLocalL2(DataMsg);
            }
            else {
                if (DataMsg.equals("")){
                    DataMsg = "Empty Msg";
                    System.out.print("print out the pattern mean!");
                    for (int i=0;i<this.dimension;i++)                        
                        System.out.print(this.pattern_mean[i]+" ");
                    //agentInterface.sendMessage(DataMsg, this.getNodeNumber());
                    sendExtractedMessageforRun(DataMsg);
                }
            }
        }
        else {
        	System.out.println("Enter else condition message passing!");
            FetchMessageComponents(m,num);
                       
          //  System.out.println("Msg recvd from : "+result1[result1.length-2] +" by "+getNumber() +" for centroid no "+i);
            boolean SendMessage = false;
            int SendMessageTo = -1;
            double EdgeDelay = 0.0;
        
            //recompute the sufficient statistics
            //computing K_i
            boolean ToChange = false;
            K_inew.size = S_i_instance.size();
            System.out.println("K_inew.size is:"+K_inew.size);
            for( int k= 0; k<nbors.size(); k++){
                K_inew.size += X_ji[k].size;
                System.out.println("X_ji["+k+"].size is:"+X_ji[k].size);
            }
            System.out.println("K_inew.size(after change) is:"+K_inew.size);
            
	        for (int j=0;j<this.dimension;j++){
                K_inew.Avg.x[j] = S_i_instance_avg.Avg.x[j]*S_i_instance_avg.size;
                for( int k= 0; k<nbors.size(); k++)
                    K_inew.Avg.x[j] += X_ji[k].Avg.x[j]*X_ji[k].size;
                
                if (K_inew.size!=0)
                    K_inew.Avg.x[j] /= K_inew.size;
	            else
                    K_inew.Avg.x[j] = 0.0;
                System.out.println("K_inew.Avg.x["+j+"] is:"+K_inew.Avg.x[j]);
            }
	       
	        for (int j=0;j<this.dimension;j++){
	        	if(K_inew.Avg.x[j] != K_i.Avg.x[j]){
	        		ToChange = true;
	        		break;
	        	}
	        }

	        if (K_inew.size != K_i.size)
	        	ToChange = true;
	        
	        for (int j=0;j<this.dimension;j++)
	        	K_i.Avg.x[j] = K_inew.Avg.x[j];
	        K_i.size =  K_inew.size;

	        //    this.AppendFile("c:\\MsgDebug.txt", ""+this.MakeString(this.K_i[0],0)+"\n\n");

	        //computing A_ij
            for( int k= 0; k<nbors.size(); k++){
            	A_ij[k].size = X_ij[k].size + X_ji[k].size;
                System.out.println("A_ij["+k+"].size is:"+A_ij[k].size);
            }
            for (int j=0;j<this.dimension;j++){
            	for( int k= 0; k<nbors.size(); k++){
            		Neighbor n = (Neighbor)nbors.get(k);
            		if ( A_ij[k].size!=0){
            			A_ij[k].Avg.x[j] = (X_ij[k].Avg.x[j] * X_ij[k].size +
	                             X_ji[k].Avg.x[j] * X_ji[k].size)/
	                             (A_ij[k].size);
	 			    }
	                else
	                    A_ij[k].Avg.x[j] = 0.0;
	                         //   AppendFile(DataPath+"A_ij\\"+"A_ij-"+n.getNumber()+i+".txt",MakeString(A_ij[k][i],0));
            		System.out.println("A_ij["+k+"].Avg.x["+j+"] is:"+A_ij[k].Avg.x[j]);
	            }
	         }

	         //Xinotj
	         for (int j=0;j<this.dimension;j++){
	        	 for( int k= 0; k<nbors.size(); k++){
	        		 W_ij[k].size = K_i.size - A_ij[k].size;
	        	 }
                 for( int k= 0; k<nbors.size(); k++){
                	 Neighbor n = (Neighbor)nbors.get(k);
                	 if (W_ij[k].size!=0){
                		 W_ij[k].Avg.x[j] = (K_i.Avg.x[j] * K_i.size -
	                             A_ij[k].Avg.x[j] * A_ij[k].size)/
	                             (W_ij[k].size);
	                 }
	                 else
	                	 W_ij[k].Avg.x[j] = 0.0;
                	 System.out.println("W_ij["+k+"].Avg.x["+j+"] is:"+W_ij[k].Avg.x[j]);
	             }
	         }

//	                 int savedStatus = this.CorrectStatus;
	 //
//	                 if (this.norm(this.K_i[0])>this.epsilon)
//	                     this.CorrectStatus = 1;
//	                 else
//	                     this.CorrectStatus = 0;
	 //
//	                 if (this.CorrectStatus!=savedStatus)
//	                     this.AppendFile(this.CorrectNessFile, ""+this.TIME+" "+this.CorrectStatus+"\n");

	        
	        String L2Msg = "";
	        boolean SendL2 = false;
	        //sending the message based on the two conditions
	        //conditions for sending a message
            SendMessage = false;
            SendMessageTo = -1;
            int option = 0;
            EdgeDelay = 0.0;
            boolean SendToOne = false;

            int Kicase = this.CheckKiShape(this.K_i);
            for( int k= 0; k<nbors.size(); k++)
            {
                SendMessage = false;
                SendMessageTo = -1;
                option = -1;
                System.out.println("The neighbor size is:"+nbors.size());
                //tie area
                //option = 0 -- outside any shape
                //option = 1 -- inside circle
                //option = 2 -- inside half space
                System.out.println("Kicase is:"+Kicase);
                System.out.println("The this.W_ij[k].size is:"+this.W_ij[k].size);
                if (Kicase==0)
                {
                     if (!this.CheckEqualVector(this.K_i,this.A_ij[k])
                         ||!this.CheckEqualSize(this.K_i,this.A_ij[k]))
                     {
                        //flood the data
                        SendMessage = true;
                        Neighbor n = (Neighbor)nbors.get(k);
                        SendMessageTo = n.getNodeNumber();
                        EdgeDelay = n.getDelay();
                        SendL2 = true;
                        option = 1;
                        System.out.println("option is:"+option);
                     }
                }

                if (option ==-1 && this.W_ij[k].size==0 &&
                    (Kicase==1 || Kicase==2) &&
                    (!this.CheckEqualVector(this.K_i,this.A_ij[k])))
                {
                    SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 2;
                    System.out.println("option is:"+option);
                }

                if (option ==-1 && (Kicase==1 || Kicase==2)
                    && (!this.CheckInsideRegion(this.A_ij[k],Kicase)
                    ||!this.CheckInsideRegion(this.W_ij[k],Kicase)))
                {
                    //send something less
                    SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 3;
                    System.out.println("option is:"+option);
                }
                System.out.println("SendMessage is:"+SendMessage);
                if (SendMessage) {
//                  this.PrintMsgStatsBefore(k, SendMessageTo,option,p);
                    this.UpdateXij(k,this.PercentPointstoSend,option,Kicase);
                    //Message String
                    L2Msg = L2Msg.concat(CreateMessage(X_ij[k], X_ij[k].size, k));

                    //Add the delay and the destination
                    int CurrentTime=this.getAgentInterface().getTime()+(int)(EdgeDelay*10.0);
                    L2Msg = L2Msg.concat("#"+SendMessageTo+"#"+(int)(EdgeDelay*10.0)+"%");

                    SendToOne = true;
                    this.UpdateAfterMsgSent(k);
                }
            }
            
            int NumRecvd=0;
            for (int j=0;j<this.nbors.size();j++){
            	if (this.flag[j] == 1){
            		NumRecvd++;
            	}
            }
               // this.AppendFile(this.XiFile, ""+this.TIME+" "+this.norm(this.K_i[0])+"\n");

            String DataSet = "";
            boolean ToSend = false;
            System.out.println("the norm of K_i is:"+this.norm(this.K_i));
            System.out.println("The number received is:"+NumRecvd);
            System.out.println("The neighbor size is:"+this.nbors.size());
            if (NumRecvd == this.nbors.size()-1){
                if (this.norm(this.K_i)>=this.epsilon){
                	ToSend = true;
                	DataSet = CreateDataSet();
                }
            }
            int index = -1;
            String DataMsg = "";
            boolean DataSetToSend = false;
            if (ToSend){
            	for (int j=0;j<this.nbors.size();j++){
            		if (this.flag[j]==-1){
            			Neighbor n = (Neighbor)nbors.get(j);
            			index = j;
            			int CurrentTime=this.getAgentInterface().getTime()+(int)(EdgeDelay*10.0);
            			DataMsg = DataMsg.concat(DataSet+"#"+n.getNodeNumber()+"#"+(int)(EdgeDelay*10.0)+"%");
            			//                            AppendFile(this.MsgFile, this.TIME+" "+String.valueOf(Num_DataMessages)+"\n");
                        DataSetToSend = true;
                        if (!this.AlertFlag){
                        	this.AlertFlag = true;
                        	this.LastDataMsgAlert = this.getAgentInterface().getTime();
                        }
                            //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data needs to be sent by "+this.getNumber()+"\n");
                    }
                }
            }

            if (DataMsg.equals(""))
            	this.AlertFlag = false;

            //now sent the msg depending on the conditions
            String Msg = "";

            //no meesage needs to be sent
            if (L2Msg.equals("")&&DataMsg.equals("")){
                Msg = "Empty Msg";
                System.out.print("print out the pattern mean!");
                for (int i=0;i<this.dimension;i++)                        
                    System.out.print(this.pattern_mean[i]+" ");
            }

            //check for L2 msg
            if (!L2Msg.equals("")){
            	//check the time
            	if (this.getAgentInterface().getTime()>=this.LastL2MsgSent+this.IntervalofL2Msg){
            		Msg = Msg.concat(L2Msg);
            		this.LastL2MsgSent = this.getAgentInterface().getTime();
            	}
                else {
                	int waitTime = this.IntervalofL2Msg + this.LastL2MsgSent - this.getAgentInterface().getTime();
                	Msg = Msg.concat("Wait Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                }
            }

            //Data Msg
            if (this.AlertFlag){
            	if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert){
            		Msg = Msg.concat(DataMsg);
            		//this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data sent by "+this.getNumber()+"\n");
            		this.flag[index]=0;         // change to send
            		this.AlertFlag = false;     //reset alert flag
            	}
                else {
                	int waitTime = this.LastDataMsgAlert + this.IntervalofDataAlert - this.getAgentInterface().getTime();
                	Msg = Msg.concat("Wait Data Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                        //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Sent by "+this.getNumber()+"\n");
                }
            }
            System.out.println("Ready to send message in run():"+Msg);
            //agentInterface.sendMessage(Msg,this.getNodeNumber());
            sendExtractedMessageforRun(Msg);
            System.out.println("###############End run() "+this.getNodeNumber()+"#########################3");
        }
        System.out.println("###############End run()########################");
    }
    
    public void classdistribution(int roundcount){
    	System.out.print("print out the pattern mean!");
    	int treeindex=(roundcount-1) / (RDT_depth*3);
        int leafindex=(roundcount-1) % (RDT_depth*3);
        for (int i=0;i<this.dimension;i++){                        
            System.out.print(this.pattern_mean[i]+" ");
            newTree[treeindex].leafnodelist[leafindex].distributionStatistics[i]=this.pattern_mean[i];
        }
    }
    
    public void toStart(int roundcount){
/////////////////////////////////////////////////////////////////////////
    	nbors=this.getNeighbors();
    	//System.out.println("node:"+Integer.toString(this.getNodeNumber())+"***************enter function: start******************");
    	
        //if(this.getNodeNumber()==4){
    	
    	for (int j=0;j<this.dimension;j++){
            this.Received_Data_Sum[j]=0.0;
        }
        System.out.println("node:"+Integer.toString(this.getNodeNumber())+"***************enter function: start******************");
        this.X_ij = new AvgVector[this.getNeighbors().size()];
        this.X_ji = new AvgVector[this.getNeighbors().size()];
        this.K_i  = new AvgVector(this.dimension);
        this.K_inew = new AvgVector(this.dimension);
        this.A_ij = new AvgVector[this.getNeighbors().size()];
        this.W_ij = new AvgVector[this.getNeighbors().size()];                                  
        this.S_i_instance_avg = new AvgVector(this.dimension);
        
        this.flag = new int[this.getNeighbors().size()];
        
        //String fileName = "D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\node"+this.getNodeNumber()+".txt";
        int treeindex=(roundcount-1) /(RDT_depth*3);
        int leafindex=(roundcount-1) % (RDT_depth*3);
        System.out.println("treeindex is:"+treeindex);
        System.out.println("leafindex is:"+leafindex);
        String fileName = "D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\files\\node"+this.getNodeNumber()+"_tree"+treeindex+"_leaf"+leafindex+".txt";
        
        FileInputStream in = null;
		 
		try {
		     File inputFile = new File (fileName);
		     in = new FileInputStream (inputFile);
		} 
		catch (Exception e) {
		     System.err.println ("Unable to open file:" + fileName + "n" + e);
		     System.exit(0);
		}

		BufferedReader bin = new BufferedReader (new InputStreamReader (in));
		String input="";
		StringTokenizer tokenizer = new StringTokenizer(input);
		 
		double []tuple = new double [this.dimension];
		// replace the old data
		// read the feature vectors along with class labels from file, and store in S_i 
        S_i = new Vector();
        
		while(true) {
			try {
				input = bin.readLine();
				System.out.println("input is:"+input);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
     		if (input == null) break;
     		
			tokenizer = new StringTokenizer(input,",");
			int numtokens = tokenizer.countTokens();
           	
			//put in the new data k
			for (int x = 0; x<numtokens; x++){
				tuple[x] = Double.parseDouble(tokenizer.nextToken());
				System.out.println("double numtokens:"+Double.toString(tuple[x]));
        	}
            Point new_point = new Point(tuple,-1);
            System.out.println("See the new added point");
            new_point.print_point(this.dimension);
            this.S_i.add(new_point);	
		}
		timestamp --;
		
		//set up the local ui's
		//not understand
        DistributeSpherePoints ds = new DistributeSpherePoints(this.d,this.dimension,200);
        Perps.addAll(ds.GetPoints());
        this.Perps = this.OrderPerps(this.Perps);
        
      //*****************randomly generate a point in R^{dimension} and put it in pattern_mean vector 
        Random r = new Random();
        r.setSeed(1000);
        for (int i=0;i<this.pattern_mean.length;i++){
            this.pattern_mean[i] = r.nextDouble();
            System.out.println("random seed:"+this.pattern_mean[i]);
        }
        
        this.S_i_instance = new Vector();
       //DummyVector is a vector that each element of this vector is a Point 
        //so DummyVector is equal to S_i
        Vector DummyVector = new Vector();
        for (int i=0;i<this.S_i.size();i++)
        {
            Point a = (Point)S_i.elementAt(i);
            Point newpt = new Point(a.d);
            for (int j=0;j<a.d;j++)
                newpt.x[j] = a.x[j];
            DummyVector.add(newpt);
        }

        System.out.println ("the points being added to Si_instance[0] are");
        //for each Point in the DummyVector, do a mean shift on this Point
        //Store the mean shifted Point to a
        //Store all the such mean shifted Point to S_i_instance
        for (int i=0;i<DummyVector.size();i++)
        {
            Point a = (Point)DummyVector.elementAt(i);
            for (int j=0;j<a.d;j++)
                a.x[j] = a.x[j] - this.pattern_mean[j];
            a.membership = -1;
            this.S_i_instance.addElement(a);
            a.print_point(this.dimension);
        }
        
        //compute the avg vector S_i_instance_avg[0]
        this.S_i_instance_avg = new AvgVector(this.S_i_instance,this.dimension);
        
        System.out.println("si instance is *****"+this.S_i_instance.size()); 
        for (int i=0;i<this.dimension;i++)
            System.out.println(this.S_i_instance_avg.Avg.x[i]);
        
        //initialization
        //set the flag to null for all the peers
        for (int i=0;i<this.getNeighbors().size();i++)
            this.flag[i] = -1;

        for (int i=0;i<this.getNeighbors().size();i++){
        	this.K_i = new AvgVector(this.dimension);
        	this.K_inew = new AvgVector(this.dimension);
        	this.X_ij[i] = new AvgVector(this.dimension);
        	this.X_ji[i] = new AvgVector(this.dimension);
        	this.A_ij[i] = new AvgVector(this.dimension);
        	this.W_ij[i] = new AvgVector(this.dimension);
        }
        
        // actual algorithm starts
        //computing K_i
        //K_i.size is = its own size + the size that its neighbors send to i
        //K_i is the average of all these data points
        K_i.size = this.S_i_instance_avg.size;
        System.out.println("K_i size is:"+K_i.size);
        for( int k= 0; k<this.getNeighbors().size(); k++)
                K_i.size += X_ji[k].size;
        System.out.println("K_i size is:(after)"+K_i.size);
  

        for (int j=0;j<this.dimension;j++){
        	K_i.Avg.x[j] = this.S_i_instance_avg.Avg.x[j]*this.S_i_instance_avg.size;
        	for( int k = 0; k<this.getNeighbors().size(); k++)
        		K_i.Avg.x[j] += X_ji[k].Avg.x[j]*X_ji[k].size;

                if (K_i.size!=0)
                    K_i.Avg.x[j] /= K_i.size;
                else
                    K_i.Avg.x[j] = 0.0;
                System.out.println("K_i.Avg.x["+j+"] is:"+K_i.Avg.x[j]);//average of knowledge
        }
        
            
     //computing A_ij: initially this A_ij should be zero
     //this A_ij is computed with respect to neighbors, so for each neighbor it has a A_ij
     //thus, A_ij[k]
        for( int k= 0; k<this.getNeighbors().size(); k++){
            A_ij[k].size = X_ij[k].size + X_ji[k].size;
            System.out.println("A_ij["+k+"] size is:"+A_ij[k].size);
        }
        
        for (int j=0;j<this.dimension;j++){
            for( int k= 0; k<this.getNeighbors().size(); k++){
                Neighbor n = (Neighbor)nbors.get(k);
                if ( A_ij[k].size!=0){
                    A_ij[k].Avg.x[j] = (X_ij[k].Avg.x[j] * X_ij[k].size +
                    X_ji[k].Avg.x[j] * X_ji[k].size)/
                    (A_ij[k].size);
                }
                else
                    A_ij[k].Avg.x[j] = 0.0;
                System.out.println("A_ij["+k+"].Avg.x["+j+"] is:"+A_ij[k].Avg.x[j]);
            }
        }
        
      //computing W_ij: initially is ...
        for( int k= 0; k<this.getNeighbors().size(); k++){
            W_ij[k].size = K_i.size - A_ij[k].size;
            System.out.println("W_ij["+k+"] size is:"+W_ij[k].size);
        }
        
        for (int j=0;j<this.dimension;j++){
            for( int k= 0; k<this.getNeighbors().size(); k++){
                Neighbor n = (Neighbor)nbors.get(k);
                if ( W_ij[k].size!=0){
                    W_ij[k].Avg.x[j] = (K_i.Avg.x[j] * K_i.size -
                    A_ij[k].Avg.x[j] * A_ij[k].size)/
                    (A_ij[k].size);
                }
                else
                    W_ij[k].Avg.x[j] = 0.0;
                System.out.println("W_ij["+k+"].Avg.x["+j+"] is:"+W_ij[k].Avg.x[j]);
            }
        }
        
        String L2Msg = "";
        boolean SendL2 = false;
        //sending the message based on the two conditions

        //conditions for sending a message
        boolean SendMessage = false;
        int SendMessageTo = -1;
        int option = 0;
        double EdgeDelay = 0.0;
        boolean SendToOne = false;

        int Kicase = this.CheckKiShape(this.K_i);
        System.out.println("Whether Kicase(vector K_i) is outside or inside the region:"+Kicase);
        for( int k= 0; k<nbors.size(); k++){
        	SendMessage = false;
        	SendMessageTo = -1;
        	option = -1;

        	//tie area
        	//option = 0 -- outside any shape
            //option = 1 -- inside circle
            //option = 2 -- inside half space
            if (Kicase==0){
            	System.out.println("Enter Kicase==0(outside any shape):");
            	if (!this.CheckEqualVector(this.K_i,this.A_ij[k])||
            			!this.CheckEqualSize(this.K_i,this.A_ij[k])){
            		//flood the data
            		System.out.println("flood the data!");
            		SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 1;
                 }
            }
            //inside circle or inside half space
            if (option ==-1 && this.W_ij[k].size==0 && (Kicase==1 || Kicase==2) && (!this.CheckEqualVector(this.K_i,this.A_ij[k]))){
            	 System.out.println("Enter Kicase==1 or 2(inside circle or half space):");
            	 System.out.println("W_ij["+k+"] size is"+this.W_ij[k].size);
            	 SendMessage = true;
                 Neighbor n = (Neighbor)nbors.get(k);
                 SendMessageTo = n.getNodeNumber();
                 EdgeDelay = n.getDelay();
                 SendL2 = true;
                 option = 2;
            }

            if (option ==-1 && (Kicase==1 || Kicase==2) && (!this.CheckInsideRegion(this.A_ij[k],Kicase)||!this.CheckInsideRegion(this.W_ij[k],Kicase))){
            	 //send something less
            	 System.out.println("Enter Kicase==1 or 2(inside circle or half space):");
            	 if (!this.CheckInsideRegion(this.A_ij[k],Kicase))
            		 System.out.println("A_ij["+k+"] is outside region");
            	 else if(!this.CheckInsideRegion(this.W_ij[k],Kicase))
            		 System.out.println("W_ij["+k+"] is outside region");
            	 
                 SendMessage = true;
                 Neighbor n = (Neighbor)nbors.get(k);
                 SendMessageTo = n.getNodeNumber();
                 EdgeDelay = n.getDelay();
                 SendL2 = true;
                 option = 3;
            }
            //if sendMessage is true,update Xij, createMessage and update Aij and Wij after send the message
            if (SendMessage){
            	 System.out.println("SendMessage is true. Update Xij and create Message!");
            	 //System.out.println ("*********sending a message ********");
            	 //this.PrintMsgStatsBefore(k, SendMessageTo,option,p);
            	 this.UpdateXij(k,this.PercentPointstoSend,option,Kicase);
            	 //Message String
            	 System.out.println("The current L2Msg is:"+L2Msg);
            	 L2Msg = L2Msg.concat("\n"+CreateMessage(X_ij[k], X_ij[k].size,k));
            	 System.out.println("The X_ij["+k+"].size is:"+X_ij[k].size);
            	 System.out.println("The X_ij.size is:"+X_ij.length);
            	 //Add the delay and the destination
            	 //int CurrentTime=this.TIME+(int)(EdgeDelay*10.0);
            	 //int CurrentTime = this.getAgentInterface().getTime();
     		     //System.out.println("The L2 message sending time is:"+CurrentTime);
            	 L2Msg = L2Msg.concat("#"+SendMessageTo+"#"+(int)(EdgeDelay*10.0)+"%");
            	 System.out.println("The EdgeDelay is:"+EdgeDelay);
            	 System.out.println("Msg after adding the delay and the destination:"+L2Msg);
            	 SendToOne = true;
                 this.UpdateAfterMsgSent(k);
             }
        }
        

        boolean SendDataSet = false;
        String DataSet = "";
        String DataMsg = "";

        //now sent the data if necessary
        boolean SendWaitDataMsg = false;
        System.out.println("the norm of K_i is:"+this.norm(this.K_i));
        if (this.norm(this.K_i)>=this.epsilon) {
        	//bad luck....send the data
        	System.out.println("bad luck....send the data");
            //if this is a leaf node
        	System.out.println("the neighbor size is:"+this.nbors.size());
        	if (this.nbors.size()==1) {
        		System.out.println("Here calls the CreateDataSet function");
        		SendDataSet = true;
        		DataSet = CreateDataSet();
        	}
        }
        

        int index = -1;
        if(SendDataSet){
            for (int i=0;i<this.nbors.size();i++){
                Neighbor n = (Neighbor)nbors.get(i);
                if (this.flag[i]==-1){
                    index = i;
                    //int CurrentTime=this.TIME+(int)(EdgeDelay*100.0);
                    //int CurrentTime = this.getAgentInterface().getTime();
                    
               	    DataMsg = DataMsg.concat(DataSet+"#"+n.getNodeNumber()+"#"+(int)(EdgeDelay*10.0)+"%");
                    if (!this.AlertFlag){
                        this.AlertFlag = true;
                        this.LastDataMsgAlert = this.getAgentInterface().getTime();
                        System.out.println("The data message sending time is:"+this.LastDataMsgAlert);
                    }
                }
                else
                    SendDataSet = false;
            }
        }

        if (DataMsg.equals(""))
            this.AlertFlag = false;

         
        //now sent the msg depending on the conditions
        String Msg = "Initialization Msg!";

        //no meesage needs to be sent
        if (L2Msg.equals("")&&DataMsg.equals(""))
            Msg = "Empty Initialization Msg!";

        //check for L2 msg
        if (!L2Msg.equals("")){
        	System.out.println("L2Msg is not equal to null");
            //check the time
           // if (this.TIME>=this.LastL2MsgSent+this.IntervalofL2Msg){
        	if (this.getAgentInterface().getTime()>=this.LastL2MsgSent+this.IntervalofL2Msg){
            	System.out.println("ready to send");
                Msg = Msg.concat(L2Msg);
                this.LastL2MsgSent = this.getAgentInterface().getTime();
            }
            else {
            	System.out.println("still need to wait");
                int waitTime = this.IntervalofL2Msg + this.LastL2MsgSent - this.getAgentInterface().getTime();
                Msg = Msg.concat("\nWait Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
            }
        }


        if (this.AlertFlag){
            if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert){
                Msg = Msg.concat(DataMsg);
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data sent by "+this.getNumber()+"\n");
                this.flag[index]=0;         // change to send
                this.AlertFlag = false;     //reset alert flag
            }
            else {
		        int waitTime = this.IntervalofDataAlert + this.LastDataMsgAlert - this.getAgentInterface().getTime();
                Msg = Msg.concat("Wait Data Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Sent by "+this.getNumber()+"\n");
            }
        }
        //System.out.println ("***********message from "+this.getNumber()+"\n message "+Msg);
        //send message to its neighbors
        /*Current the string stored 'Msg' is in the following format:
         * message label
         * X_ij
         * X_ij.size
         * #node number#wait time%
         * X_ij
         * X_ij.size
         * #node number#wait time%
         * */ 
        //so we need to create message for each node that needs to send to 
        //this.getAgentInterface().sendMessage(Msg,3);
        System.out.println("The final message sent is:"+Msg);
        sendExtractedMessage(Msg);
        System.out.println("node:"+Integer.toString(this.getNodeNumber())+"***************end function: Start******************");
   // }
    }
    public void buildTreeSet(){
    	
		int DepthCounter=0;
		System.out.println("RDT_depth is:"+RDT_depth);
    	//start to build random trees
		
		
		for (int i=0;i<numOfTrees;i++){
			newTree[i]= new buildTree();
			newTree[i].initData();
			
			//newTree[i].createDecisionTree(RDT_depth,DepthCounter);
			try {
				newTree[i].createDecisionTree(RDT_depth,DepthCounter);
				//write the leafnodelist.data into files
				BufferedWriter out0;
				String filename="D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\files\\node"+this.getNodeNumber()+"_tree"+i+".txt";
				out0 = new BufferedWriter(new FileWriter(filename));
		        out0.write(newTree[i].statistics);
   		        out0.close();
				for (int j=0; j<RDT_depth*3;j++){
				     BufferedWriter out;
				     try {
				    	 filename="D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\files\\node"+this.getNodeNumber()+"_tree"+i+"_leaf"+j+".txt";
				    	 out = new BufferedWriter(new FileWriter(filename));
				         out.write(newTree[i].leafnodelist[j].data);
		    		     out.close();
				     }
				     catch(IOException e){
				         System.out.println("There was a problem:" + e);
				     }
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		
		//newTree.readData("D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\matrixGal1.txt");
		System.out.println("#####################End build the random tree################");
    }
    
    public void RunLocalL2(String PassedMsg)
    {
    	System.out.println("#################Enter RunLocalL2()###############################");
        boolean DataSetToSend = false;
        String L2Msg = "";

        String PassedPattern = "";
        if (PassedMsg!="")
        {
            DataSetToSend = true;
            PassedPattern = PassedMsg;
        }

        //remove the old points first
        this.S_i_instance.removeAllElements();
        
        Vector DummyVector = new Vector();
        for (int i=0;i<this.S_i.size();i++)
        {
            Point a = (Point)S_i.elementAt(i);
            Point newpt = new Point(a.d);
            for (int j=0;j<a.d;j++)
                newpt.x[j] = a.x[j];
            DummyVector.add(newpt);
        }
        
        for (int i=0;i<DummyVector.size();i++)
        {
            Point a = (Point)DummyVector.elementAt(i);
            for (int j=0;j<a.d;j++)
                a.x[j] = a.x[j] - this.pattern_mean[j];
            a.membership = -1;
            this.S_i_instance.addElement(a);
        }
        this.S_i_instance_avg = new AvgVector(this.S_i_instance,this.dimension);
        double [][]A = new double [this.dimension][this.dimension];
        for (int i=0;i<this.dimension;i++)
            for (int j=0;j<this.dimension;j++)
                A[i][j] = 0;
        
        Point p = new Point (this.pattern_mean,-1);
        double[] oldSi = new double[this.dimension];
        double[] newSi = new double[this.dimension];
        int oldSisize = 0;
        int newSisize = 0;
        oldSisize = this.S_i_instance_avg.size;
        for (int j=0;j<this.dimension;j++){
            oldSi[j] = this.S_i_instance_avg.Avg.x[j];
            System.out.println("old this.S_i_instance_avg.Avg.x["+j+"] is:"+this.S_i_instance_avg.Avg.x[j]);
        }

        S_i_instance_avg = new AvgVector(S_i_instance,this.dimension);
        newSisize = this.S_i_instance_avg.size;
        for (int j=0;j<this.dimension;j++){
            newSi[j] = this.S_i_instance_avg.Avg.x[j];
            System.out.println("new this.S_i_instance_avg.Avg.x["+j+"] is:"+this.S_i_instance_avg.Avg.x[j]);
        }

        //computing K_i
        K_i.size = S_i_instance.size();
        for( int k= 0; k<nbors.size(); k++)
            K_i.size += X_ji[k].size;
        for (int j=0;j<this.dimension;j++){
            K_i.Avg.x[j] = S_i_instance_avg.Avg.x[j]*S_i_instance_avg.size;
            for( int k= 0; k<nbors.size(); k++)
                K_i.Avg.x[j] += X_ji[k].Avg.x[j]*X_ji[k].size;

            if (K_i.size!=0)
                K_i.Avg.x[j] /= K_i.size;
            else
                K_i.Avg.x[j] = 0.0;
        }
                //AppendFile(DataPath+"K_i\\"+"K_i-"+i+".txt",MakeString(K_i[i]));
                //DEBUG END

                //computing A_ij

        for( int k= 0; k<nbors.size(); k++)
            A_ij[k].size = X_ij[k].size + X_ji[k].size;
        for (int j=0;j<this.dimension;j++){
            for( int k= 0; k<nbors.size(); k++){
                Neighbor n = (Neighbor)nbors.get(k);
                if ( A_ij[k].size!=0){
                    A_ij[k].Avg.x[j] = (X_ij[k].Avg.x[j] * X_ij[k].size +
                    X_ji[k].Avg.x[j] * X_ji[k].size)/
                       (A_ij[k].size);
                }
                else
                    A_ij[k].Avg.x[j] = 0.0;
            }
        }

        //computing W_ij
        for( int k= 0; k<nbors.size(); k++)
            W_ij[k].size = K_i.size - A_ij[k].size;
        for (int j=0;j<this.dimension;j++){
            for( int k= 0; k<nbors.size(); k++){
        	    Neighbor n = (Neighbor)nbors.get(k);
                if (W_ij[k].size!=0){
                   W_ij[k].Avg.x[j] = (K_i.Avg.x[j] * K_i.size -
                       A_ij[k].Avg.x[j] * A_ij[k].size)/
                            (W_ij[k].size);
                }
                else
                   W_ij[k].Avg.x[j] = 0.0;
            }
        }

//            this.AppendFile("c:\\Quality"+i+".txt",""+this.TIME+" ");
//            for (int j=0;j<this.dimension;j++)
//                this.AppendFile("c:\\Quality"+i+".txt",String.valueOf(newSi[j]-oldSi[j])+" ");
//            this.AppendFile("c:\\Quality"+i+".txt",String.valueOf(newSisize-oldSisize)+"\n");
        

        
        boolean SendL2 = false;
        //sending the message based on the two conditions

        //conditions for sending a message
       
        boolean SendMessage = false;
        int SendMessageTo = -1;
        int option = 0;
        double EdgeDelay = 0.0;
        boolean SendToOne = false;

        int Kicase = this.CheckKiShape(this.K_i);
        for( int k= 0; k<nbors.size(); k++){
            SendMessage = false;
            SendMessageTo = -1;
            option = -1;

                //tie area
            if (Kicase==0){
                if (!this.CheckEqualVector(this.K_i,this.A_ij[k])
                         ||!this.CheckEqualSize(this.K_i,this.A_ij[k])){
                     //flood the data
                     SendMessage = true;
                     Neighbor n = (Neighbor)nbors.get(k);
                     SendMessageTo = n.getNodeNumber();
                     EdgeDelay = n.getDelay();
                     SendL2 = true;
                     option = 1;
                }
            }

            if (option ==-1 && this.W_ij[k].size==0 &&
                    (Kicase==1 || Kicase==2) &&
                    (!this.CheckEqualVector(this.K_i,this.A_ij[k]))){
                    SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 2;
            }

            if (option ==-1 && (Kicase==1 || Kicase==2)
                    && (!this.CheckInsideRegion(this.A_ij[k],Kicase)
                    ||!this.CheckInsideRegion(this.W_ij[k],Kicase))){
                    //send something less
                    SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 3;
             }



             if (SendMessage){
//                    this.PrintMsgStatsBefore(k, SendMessageTo,option,p);
                    this.UpdateXij(k,this.PercentPointstoSend,option,Kicase);
                    //Message String
                    L2Msg = L2Msg.concat(CreateMessage(X_ij[k], X_ij[k].size,k));

                    //Add the delay and the destination
                    L2Msg = L2Msg.concat("#"+SendMessageTo+"#"+(int)(EdgeDelay*10.0)+"%");

                    SendToOne = true;
                    this.UpdateAfterMsgSent(k);
              }
     
        }
        //now sent the data if necessary (a heavy communication indeed)
        int NumRecvd=0;
        for (int j=0;j<this.nbors.size();j++){
            if (this.flag[j] == 1){
                NumRecvd++;
            }
        }

        String DataSet = "";
        String DataMsg = "";
        boolean ToSend = false;
        int index = -1;
        if (NumRecvd == this.nbors.size()-1){
            if (this.norm(this.K_i)>=this.epsilon){
                    ToSend = true;
                    DataSet = CreateDataSet();
            }
        }


        if (ToSend){
            for (int j=0;j<this.nbors.size();j++){
                if (this.flag[j]==-1){
                    Neighbor n = (Neighbor)nbors.get(j);
                    index = j;
                    DataMsg = DataMsg.concat(DataSet+"#"+this.getNodeNumber()+"#"+(int)(n.getDelay()*10.0)+"%");
//                    AppendFile(this.MsgFile,this.TIME+" "+ String.valueOf(Num_DataMessages)+"\n");
                    DataSetToSend = true;
                    if (!this.AlertFlag){
                        this.AlertFlag = true;
                        this.LastDataMsgAlert = this.getAgentInterface().getTime();;
                    }
                }
            }
        }

        if (DataMsg.equals(""))
            this.AlertFlag = false;

        //now sent the msg depending on the conditions
        String Msg = "";

        //no meesage needs to be sent
        if (L2Msg.equals("")&&DataMsg.equals("")&&PassedPattern.equals("")){
            Msg = "Empty Msg";
            System.out.print("print out the pattern mean!");
            for (int i=0;i<this.dimension;i++)                        
                System.out.print(this.pattern_mean[i]+" ");
        }

        //check for L2 msg
        if (!L2Msg.equals("")){
            //check the time
            if (this.getAgentInterface().getTime()>=this.LastL2MsgSent+this.IntervalofL2Msg){
                Msg = Msg.concat(L2Msg);
                this.LastL2MsgSent = this.getAgentInterface().getTime();;
            }
            else{
                int waitTime = this.IntervalofL2Msg + this.LastL2MsgSent - this.getAgentInterface().getTime();
                Msg = Msg.concat("Wait Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
            }
        }

        //check for Data msg
        if (this.AlertFlag){
            if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert){
                Msg = Msg.concat(DataMsg);
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data sent by "+this.getNumber()+"\n");
                this.flag[index]=0;         // change to send
                this.AlertFlag = false;     //reset alert flag
            }
            else{
                int waitTime = this.LastDataMsgAlert + this.IntervalofDataAlert - this.getAgentInterface().getTime();;
                Msg = Msg.concat("Wait Data Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Sent by "+this.getNumber()+"\n");
            }
        }

        if (!PassedPattern.equals(""))
            Msg = Msg.concat(PassedPattern);
        //iface.sendMessage(Msg,this.getNodeNumber());
        System.out.println("The message sent is:"+Msg);
        sendExtractedMessageforRun(Msg);
        System.out.println("#################End RunLocalL2()###############################");
    }

	public int CheckKiShape(AvgVector K) {
    	//System.out.println("############Enter CheckKiShape function#########################");
        //option = 0 -- outside any shape
        //option = 1 -- inside circle
        //option = 2 -- inside half space

    	//System.out.println("The norm of K is:"+this.norm(K));
    	//System.out.println("The epsilon is:"+this.epsilon);
        int option = 0;
        if (this.norm(K)<this.epsilon)
            option = 1;
        else {
            //check if K is in the half space
            for (int i=0;i<this.Perps.size();i++){
                Point u = (Point)this.Perps.elementAt(i);
                if (this.dot(u,K)>this.epsilon){
                	int next = i+1;
                    if (i==this.Perps.size()-1)
                    	next = 0;
                    Point un = (Point)this.Perps.elementAt(next);
                    if (this.dot(un,K) < this.epsilon){
                        option = 2;
                        break;
                    }
                }
            }
        }
        //System.out.println("The option is:"+option);
        //System.out.println("############End CheckKiShape function#########################");
        return option;
    }
    
    public  double norm(AvgVector a) {
        double dist = 0.0;
        for (int i=0;i<a.Avg.d;i++)
            dist += Math.pow(a.Avg.x[i],2);

        dist = Math.sqrt(dist);
        return dist;
    }
    

    public double dot(Point a, AvgVector b) {
        double dot = 0.0;
        for (int i=0;i<a.d;i++)
            dot+=a.x[i]*b.Avg.x[i];
        return dot;
    }
    
    public boolean CheckEqualSize(AvgVector a, AvgVector b)
    {
        if (a.size!=b.size)
            return false;
        else
            return true;
    }
    

    public Vector OrderPerps(Vector OldPerp)
    {
        Vector CurrentUnitvects = new Vector();
        Vector ModifiedUnitvects = new Vector();

  
        //get the first point
        Point p = (Point)OldPerp.elementAt(0);
        //add it to the list
        ModifiedUnitvects.add(p);
        //find the vector nearest to this
        int[] orderedlist = this.SortUnitVectors(p, OldPerp);
        p = (Point)OldPerp.elementAt(orderedlist[1]);
        //add it to the list
        ModifiedUnitvects.add(p);
       
        //System.out.println("this is the modified vector and the point p is");
       //p.print_point(this.dimension);
        //System.out.println (" the order perps method Modified unit vectors addaed point is "+p.print_point(this.dimension));

        //now order the other points
        for (int i=1;i<ModifiedUnitvects.size();i++)
        {
            Point b = (Point)ModifiedUnitvects.elementAt(i);
            orderedlist = this.SortUnitVectors(b, OldPerp);

            //find the nearest point which is not inserted
            for (int j=1;j<orderedlist.length;j++)
            {
                Point a = (Point)OldPerp.elementAt(orderedlist[j]);
                if (!this.SearchPoints(a,ModifiedUnitvects))
                {
                    ModifiedUnitvects.add(a);
                    break;
                }
            }
        }
        return ModifiedUnitvects;
    }
    
    public int[] SortUnitVectors(Point x, Vector Perps)
    {
        //sort all the points in increasing distance from x
        //first find the distance
        double[] dist = new double[this.Perps.size()];
        int[] index = new int[this.Perps.size()];
        
        int min = 0;
        for (int i=0;i<Perps.size();i++)
        {
            Point p = (Point)Perps.elementAt(i);
            double distance = this.Distance(x,p);
            dist[i] = distance;            
            index[i] = i;
        }
        
        double[] temp = new double[this.Perps.size()];
        for(int i=0;i<Perps.size();i++)
            temp[i] = dist[i];
            
        Arrays.sort(temp);
        
        for(int i=0;i<Perps.size();i++)
        {
            for(int j=0;j<Perps.size();j++)
            {
                if(temp[i]==dist[j])
                {
                    index[i] = j;
                    break;
                }
                else
                    continue;
            }
        }
//        for (int i=0;i<this.Perps.size();i++)
//        {
//            System.out.println("Orig Dist : "+dist[i]+" Sorted Dist : "+temp[i]+"
//Index : "+index[i]);
////                this.AppendFile("c:\\Perps.txt", ""+p.x[0]+" "+p.x[1]+"\n");
//        }
        return index;
    }
    
    public double Distance(Point a, Point b)
    {
        double dist = 0.0;
        for (int i=0;i<a.d;i++)
            dist += Math.pow(a.x[i]-b.x[i],2);

        dist = Math.sqrt(dist);
        return dist;
    }
    
    public boolean SearchPoints(Point p, Vector X)
    {
        boolean found = false;
        for (int i=0;i<X.size();i++)
        {
            Point t = (Point)X.elementAt(i);
            if (this.CheckEqualPoint(t,p))
            {
                found = true;
                break;
            }
            else
                continue;
        }
        return found;
    }
    
    public boolean CheckEqualPoint(Point p1, Point p2){
        
        for (int i =0; i<this.dimension; i++)
        {
            if (p1.x[i]!= p2.x[i])
            {
                return false;
            }
        }
         return true;
     }
    
    public String CreateDataSet()
    {
    	System.out.println("***************enter function: CreateDataSet******************");
        String Msg = "";
        boolean ToSent = false;
        for (int i=0;i<this.nbors.size();i++)
        {
            if (this.flag[i]==-1)
            {
                ToSent = true;
                break;
            }
        }
        System.out.println("ToSent is:"+ToSent);
        System.out.println("this.AlertFlag is:"+this.AlertFlag);
        if (this.AlertFlag && ToSent)
        {
        	System.out.println("haha");
            if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert)
            {
            	System.out.println("In yeah");
                Msg = "DataSet\n";
                //this is the data for mean monitoring
                //create a local average vector from S_i
                AvgVector localAvg = new AvgVector(this.S_i,this.dimension);
                Point p = localAvg.Avg;
                
                double[] temp = new double[this.dimension];
                for (int j=0;j<this.dimension;j++)
                {
                    temp[j] = p.x[j]*this.S_i.size() + this.Received_Data_Sum[j];
                    System.out.println("p.x["+j+"] is:"+p.x[j]);
                    System.out.println("this.S_i.size() is:"+this.S_i.size());
                    System.out.println("this.Received_Data_Sum["+j+"] is:"+this.Received_Data_Sum[j]);
                    Msg = Msg.concat(String.valueOf(temp[j])+" ");
                }
                int t = this.Received_Data_Size + this.S_i.size();
                Msg = Msg.concat("\n"+String.valueOf(t)+"\n");
            }
        }
        System.out.println("The Msg is:"+Msg);
        System.out.println("***************end function: CreateDataSet******************");
        return Msg;
        
    }
    
    public boolean CheckEqualVector(AvgVector a, AvgVector b)
    {
    	//System.out.println("#################Enter CheckEqualVector#####################################");
        boolean equal = true;
        for (int i=0;i<a.Avg.d;i++){
            if (a.Avg.x[i] != b.Avg.x[i]){
                equal = false;
                break;
            }
            else
                continue;
        }
        //System.out.println("The equal is:"+equal);
        //System.out.println("#################End CheckEqualVector#####################################");
        return equal;
    }
    
    public boolean CheckInsideRegion(AvgVector a, int KiOption){
    	//System.out.println("#################Enter CheckInsideRegion#####################################");
        boolean insideshape = false;
        if (KiOption == 0)      //Ki is not in any shape
            insideshape = false;

        if (KiOption == 1)      //Ki is inside circle
        {
            if (this.norm(a)<this.epsilon)
            {
                //a is inside the circle
                insideshape = true;
            }
        }

        if (KiOption == 2)      //Ki is inside halfspace
        {
            //check if a is in the half space as Ki
            //first find the half space of Ki
            Point u = new Point(this.dimension);
            for (int i=0;i<this.Perps.size();i++)
            {
                u = (Point)this.Perps.elementAt(i);
                if (this.dot(u,this.K_i)>this.epsilon)
                    break;
            }
            //now check for a
            if (this.dot(u,a)>this.epsilon)
                insideshape = true;
        }
        //System.out.println("The insideshape is:"+insideshape);
        //System.out.println("#################End CheckInsideRegion#####################################");
        return insideshape;
    }
    
    public void UpdateXij(int neighbor, double Percent, int option, int Kicase)
    {
    	System.out.println("***************enter function: UpdateXij******************");
        int k = neighbor;
        AvgVector X = new AvgVector (this.dimension);
        //new stuff
        if (option==1){//K_i outside any shape, K_i != A_ij[k] or |K_i| != |A_ij[k]|
        	System.out.println("K_i is outside any shape.");
            for (int j=0;j<this.dimension;j++){
                if (K_i.size - this.X_ji[k].size!=0){//size of knowledge is not equal to the size of x_ij[k] 
                    //compute the average
                	//Average X = (knowledge*size of knowledge-X_ji*X_ji size)/(knowledge size-X_ji size)
                	X.Avg.x[j] = (this.K_i.Avg.x[j]*this.K_i.size -
                                    this.X_ji[k].size*this.X_ji[k].Avg.x[j])
                                    /(K_i.size - this.X_ji[k].size);
                }
                else //if knowledge size = X_ji size
                    X.Avg.x[j] = 0.0;
                System.out.println("X.Avg.x["+j+"] is:"+X.Avg.x[j]);
            }
            //set the size of X
            X.size = this.K_i.size - this.X_ji[k].size;
        }
        else {//K_i is either inside circle or inside half space
        	System.out.println("K_i is either inside circle or inside half space.");
            int w = 0;
            //set the vector X
            for (int j=0;j<this.dimension;j++) {
                if (K_i.size - this.X_ji[k].size!=0) {
                    X.Avg.x[j] = (this.K_i.Avg.x[j]*this.K_i.size -
                                    this.X_ji[k].size*this.X_ji[k].Avg.x[j])
                                    /(K_i.size - this.X_ji[k].size);
                }
                else
                    X.Avg.x[j] = 0.0;
                System.out.println("this.K_i.Avg.x["+j+"] is:"+this.K_i.Avg.x[j]);
                System.out.println("K_i.size is:"+K_i.size);
                System.out.println("this.X_ji["+k+"].Avg.x["+j+"] is:"+this.X_ji[k].Avg.x[j]);
                System.out.println("this.X_ji["+k+"].size is:"+this.X_ji[k].size);
                System.out.println("X.Avg.x["+j+"] is:"+X.Avg.x[j]);
            }
            //set the size of X
            System.out.println("Originally,X.size is:"+X.size);
            X.size = this.K_i.size - this.X_ji[k].size;
            System.out.println("Set this size to be:"+X.size);
            w = X.size;
            AvgVector newA = new AvgVector(this.dimension);
            AvgVector newW = new AvgVector(this.dimension);

//            this.AppendFile("c:\\MsgDebug.txt", "Updating Xij\n");

            do
            {
            	System.out.println("enter do while loop");
            	System.out.println("w:"+w);
                w = w/2;
                X.size = this.K_i.size - this.X_ji[k].size - w;
                //actually newA.size = this.K_i.size-w
                newA.size = X.size + this.X_ji[k].size;
                //actually newW.size=w
                newW.size = this.K_i.size - X.size - this.X_ji[k].size;
                for (int j=0;j<this.dimension;j++)
                {
                    newA.Avg.x[j] = (X.Avg.x[j] * X.size + X_ji[k].Avg.x[j] * X_ji[k].size)/
                                    (newA.size);

                    newW.Avg.x[j] = (this.K_i.size * this.K_i.Avg.x[j]
                                     - X.Avg.x[j] * X.size
                                     - X_ji[k].Avg.x[j] * X_ji[k].size)/
                                    (newW.size);
                }
//                System.out.println(this.CheckInsideRegion(newA,nearestpt));
//                System.out.println(this.CheckInsideRegion(newW,nearestpt));
//                System.out.println(w);
//                this.AppendFile("c:\\MsgDebug.txt", "X : "+this.MakeString(X)+"\n");
//                this.AppendFile("c:\\MsgDebug.txt", "First term : "+this.MakeString(newA)+"\n");
//                this.AppendFile("c:\\MsgDebug.txt", "Second term : "+this.MakeString(newW)+"\n");
            }
            while((!this.CheckInsideRegion(newA,Kicase))
                   ||(!this.CheckInsideRegion(newW,Kicase) && newW.size>0));
            System.out.println("W is:"+w);
            
        }

        //set Sij
        this.X_ij[k].size = X.size;
        
        System.out.println("X_ij["+k+"] is:"+this.X_ij[k].size);
        for (int j=0;j<this.dimension;j++){
            this.X_ij[k].Avg.x[j] = X.Avg.x[j];
            System.out.println("X.Avg.x["+j+"] is:"+X.Avg.x[j]);
        }
        System.out.println("***************end function: UpdateXij******************");
    }
    
    public  String CreateMessage(AvgVector a, int size, int neighbor)
    {
    	System.out.println("***************enter function: CreateMessage******************");
        String Msg = "";
        for (int i=0;i<this.dimension;i++)
            Msg = Msg.concat(String.valueOf(a.Avg.x[i])+" ");
        Msg = Msg.concat("\n");
        Msg = Msg.concat(String.valueOf(size)+"\n");
        System.out.println("The Msg is:"+Msg);
        System.out.println("***************end function: CreateMessage******************");
        return Msg;
    }
    
    public void WakeUp(){
    	
    	System.out.println("##################Enter WakeUp() function#################################");
    	
        String L2Msg = "";
        boolean SendL2 = false;
        //sending the message based on the two conditions

        //conditions for sending a message
        boolean SendMessage = false;
        int SendMessageTo = -1;
        int option = 0;
        double EdgeDelay = 0.0;
        boolean SendToOne = false;

        int Kicase = this.CheckKiShape(this.K_i);
        for( int k= 0; k<nbors.size(); k++){
        	SendMessage = false;
        	SendMessageTo = -1;
        	option = -1;
        	
        	//tie area
            if (Kicase==0){
            	if (!this.CheckEqualVector(this.K_i,this.A_ij[k])
                         ||!this.CheckEqualSize(this.K_i,this.A_ij[k])){
                        //flood the data
                        SendMessage = true;
                        Neighbor n = (Neighbor)nbors.get(k);
                        SendMessageTo = n.getNodeNumber();
                        EdgeDelay = n.getDelay();
                        SendL2 = true;
                        option = 1;
                }
            }

            if (option ==-1 && this.W_ij[k].size==0 &&
                    (Kicase==1 || Kicase==2) &&
                    (!this.CheckEqualVector(this.K_i,this.A_ij[k]))){
                    SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 2;
            }

            if (option ==-1 && (Kicase==1 || Kicase==2)
                    && (!this.CheckInsideRegion(this.A_ij[k],Kicase)
                    ||!this.CheckInsideRegion(this.W_ij[k],Kicase))){
                    //send something less
                    SendMessage = true;
                    Neighbor n = (Neighbor)nbors.get(k);
                    SendMessageTo = n.getNodeNumber();
                    EdgeDelay = n.getDelay();
                    SendL2 = true;
                    option = 3;
            }

            if (SendMessage){
//                    this.PrintMsgStatsBefore(k, SendMessageTo,option,p);
            	this.UpdateXij(k,this.PercentPointstoSend,option,Kicase);
            	//Message String
            	L2Msg = L2Msg.concat(CreateMessage(X_ij[k], X_ij[k].size, k));

                //Add the delay and the destination
            	int CurrentTime=this.getAgentInterface().getTime()+(int)(EdgeDelay*10.0);
            	System.out.println("The CurrentTime is:"+CurrentTime);
           	    System.out.println("The EdgeDelay is:"+EdgeDelay);
                L2Msg = L2Msg.concat("#"+SendMessageTo+"#"+CurrentTime+"%");

                SendToOne = true;
                this.UpdateAfterMsgSent(k);
            }
        }
        

        //now sent the data if necessary (a heavy communication indeed)
        //first check if sending the data is allowed
        int NumRecvd=0;
        for (int j=0;j<this.nbors.size();j++){
            if (this.flag[j] == 1){
                NumRecvd++;
            }
        }


      //  this.AppendFile(XiFile, ""+this.TIME+" "+this.norm(this.K_i[0])+"\n");
        String DataSet = "";
        String DataMsg = "";
        int index = -1;
        boolean ToSend = false;
        if (NumRecvd == this.nbors.size()-1){
           if (this.norm(this.K_i)>=this.epsilon){
        	   ToSend = true;
        	   DataSet = CreateDataSet();
           }
        }

        if (ToSend){
            for (int j=0;j<this.nbors.size();j++){
                if (this.flag[j]==-1){
                    Neighbor n = (Neighbor)nbors.get(j);
                    index = j;
                    int CurrentTime=this.getAgentInterface().getTime()+(int)(EdgeDelay*10.0);
                    System.out.println("The CurrentTime is:"+CurrentTime);
               	 System.out.println("The EdgeDelay is:"+EdgeDelay);
                    DataMsg = DataMsg.concat(DataSet+"#"+n.getNodeNumber()+"#"+CurrentTime+"%");
                    //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data needs to be sent by "+this.getNumber()+"\n");
                    if (!this.AlertFlag){
                        this.AlertFlag = true;
                        this.LastDataMsgAlert = this.getAgentInterface().getTime();
                    }
                }
            }
        }

        if (DataMsg.equals(""))
            this.AlertFlag = false;

        //now sent the msg depending on the conditions
        String Msg = "";

        //no meesage needs to be sent
        
        if (L2Msg.equals("")&&DataMsg.equals("")){
        	System.out.print("print out the pattern mean!");
            Msg = "Empty Msg";
            for (int i=0;i<this.dimension;i++)                        
                System.out.print(this.pattern_mean[i]+" ");
        }

        //check for L2 msg
        if (!L2Msg.equals("")) {
            //check the time
            if (this.getAgentInterface().getTime()>=this.LastL2MsgSent+this.IntervalofL2Msg){
                Msg = Msg.concat(L2Msg);
                this.LastL2MsgSent = this.getAgentInterface().getTime();
            }
            else {
                int waitTime = this.IntervalofL2Msg + this.LastL2MsgSent - this.getAgentInterface().getTime();
                Msg = Msg.concat("Wait Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
            }

        }

        //check for Data msg
        if (this.AlertFlag) {
            if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert){
                Msg = Msg.concat(DataMsg);
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data sent by "+this.getNumber()+"\n");
                this.flag[index]=0;         // change to send
                this.AlertFlag = false;     //reset alert flag
            }
            else {
                int waitTime = this.LastDataMsgAlert + this.IntervalofDataAlert - this.getAgentInterface().getTime();
                Msg = Msg.concat("Wait Data Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Sent by "+this.getNumber()+"\n");
            }
        }

       // agentInterface.sendMessage(Msg,this.getNodeNumber());
        sendExtractedMessageforRun(Msg);
        System.out.println("##################End WakeUp() function#################################");
    }
    
    public void UpdateAfterMsgSent(int neighbor)
	{
		System.out.println("***************enter function: UpdateAfterMsgSent******************");
	    int k = neighbor;
	    
	    //A_ij
	    A_ij[k].size = X_ij[k].size + X_ji[k].size;
	
	    W_ij[k].size = K_i.size - A_ij[k].size;
	    if (W_ij[k].size < 0)
	        W_ij[k].size = 0;
	    System.out.println("A_ij["+k+"].size is:"+A_ij[k].size);
	    System.out.println("W_ij["+k+"].size is:"+W_ij[k].size);
	    
	    Neighbor n = (Neighbor)nbors.get(k);
	    for (int j=0;j<this.dimension;j++)
	    {
	        if (A_ij[k].size!=0)
	        {
	            A_ij[k].Avg.x[j] = (X_ij[k].Avg.x[j] * X_ij[k].size +
	            X_ji[k].Avg.x[j] * X_ji[k].size)/
	            (A_ij[k].size);
	
	        }
	        else
	            A_ij[k].Avg.x[j] = 0.0;
	        System.out.println("A_ij["+k+"].Avg.x["+j+"] is:"+A_ij[k].Avg.x[j]);
	        //W_ij
	        if (W_ij[k].size!=0)
	        {
	            W_ij[k].Avg.x[j] = (K_i.Avg.x[j] * K_i.size -
	            A_ij[k].Avg.x[j] * A_ij[k].size)/
	            (W_ij[k].size);
	        }
	        else
	            W_ij[k].Avg.x[j] = 0.0;
	        System.out.println("W_ij["+k+"].Avg.x["+j+"] is:"+W_ij[k].Avg.x[j]);
	    }
	    //DEBUG BEGIN
	    //DEBUG END
	    System.out.println("***************end function: UpdateAfterMsgSent******************");
	}

	public void WakeUpForData() {
		System.out.println("################enter WakeUpForData()###############");
        String Msg = "";
        String DataMsg = "";
        if (this.AlertFlag) {
        	System.out.println("this.AlertFlag is:"+this.AlertFlag);
            String DataSet = CreateDataSet();
            int index = -1;
            for (int j=0;j<this.nbors.size();j++) {
                if (this.flag[j]==-1){
                    Neighbor n = (Neighbor)nbors.get(j);
                    index = j;
                    DataMsg = DataMsg.concat(DataSet+"#"+n.getNodeNumber()+"#"+(int)(n.getDelay()*10)+"%");
                    //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data needs to be sent by "+this.getNumber()+"\n");
                }
            }

            if (this.getAgentInterface().getTime()>=this.LastDataMsgAlert+this.IntervalofDataAlert){
                Msg = Msg.concat(DataMsg);
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Data sent by "+this.getNumber()+"\n");
                this.flag[index]=0;         // change to send
                this.AlertFlag = false;     //reset alert flag
                sendExtractedMessageforRun(Msg);
            }
            else {
                int waitTime = this.LastDataMsgAlert + this.IntervalofDataAlert - this.getAgentInterface().getTime();
                Msg = Msg.concat("Wait Data Msg#"+this.getNodeNumber()+"#"+waitTime+"%");
                //this.AppendFile("c:\\WakeUpData.txt","Time: "+this.TIME+" Wake Up Data Sent by "+this.getNumber()+"\n");
                sendExtractedMessageforRun(Msg);
            }
            

        }
        else {
            Msg = "Empty Msg";
            System.out.print("print out the pattern mean!");
            for (int i=0;i<this.dimension;i++)                        
                System.out.print(this.pattern_mean[i]+" ");
            //this.getAgentInterface().sendMessage(Msg, this.getNodeNumber());
        }
        System.out.println("################end WakeUpForData()###############");
    }
  
    public void FetchPatterns(String Message)
    {
    	System.out.println("#################enter FetchPatterns()################################");
        //get the mean vector, eigenvector and eigenvalue
        String[] result = Message.split("\n");
        //first extract the mean vector
        String[] comp = result[1].split(" ");
        double[] a = new double[this.dimension];
        for (int i=0;i<this.dimension;i++)
            a[i] = Double.parseDouble(comp[i].trim());
        Point p = new Point(a,-1);
        Point temp = new Point(this.dimension);
        for (int i=0;i<this.dimension;i++)
        {
            temp.x[i] = p.x[i];
            pattern_mean[i]=temp.x[i];
            System.out.println("pattern_mean["+i+"] is:"+pattern_mean[i]);
        }        
        this.Received_Data_Size = 0;
        System.out.println("#################end FetchPatterns()################################");
    }

    public String MakePatternString()
    {
    	System.out.println("#################enter MakePatternString()################################");
        String Msg = "PatternStates\n";
        
        for (int i=0;i<this.pattern_mean.length;i++)
            Msg = Msg.concat(String.valueOf(this.pattern_mean[i])+" ");
        Msg = Msg.concat("\n");
       
        //if this is the root centroid round is 1+DataCollection round else propagate the centroid round
        if (this.getNodeNumber()==this.NewRoot){
        	System.out.println("haha");
            this.CentroidRound = DataCollectionRound+1;
            this.DataCollectionRound +=2; 
            Msg = Msg.concat(""+this.CentroidRound+"\n");
        }
        else
            Msg = Msg.concat(""+this.RecvdRound+"\n");
        
        Msg = Msg.concat(""+this.NewRoot+"\n");
        
//        this.AppendFile("c:\\Test.txt", ""+this.TIME+"\n");
//        this.AppendFile("c:\\Test.txt", "Node Num : "+this.getNumber()+"\n");
//        this.AppendFile("c:\\Test.txt", "DataCollection : "+this.DataCollectionRound+" "+"Centroid: "+this.RecvdRound+"\n\n");
        System.out.println("The Msg is:"+Msg);
        System.out.println("#################end MakePatternString()################################");
        return Msg;
        
    }
    
   /* public void GetTime(String Message)
    {
        String[] result = Message.split("\n");
        this.getAgentInterface().getTime() = Integer.parseInt(result[result.length-1]);
    }*/

    public void AccumulateRemoteData(String Message)
    {
    	System.out.println("################enter AccumulateRemoteData###################################");
        //System.out.println(Message);
        String[] result = Message.split("\n");
        
        //get the sum first
        String[] comp = result[1].split(" ");
        double[] a = new double[this.dimension];
        for (int i=0;i<this.dimension;i++)
            a[i] = Double.parseDouble(comp[i].trim());
        
        Point p = new Point(a,-1);
        p.print_point(this.dimension);
        Point temp = new Point(this.dimension);
        for (int i=0;i<this.dimension;i++)
            temp.x[i] = p.x[i];
        for (int i=0;i<this.dimension;i++){
            this.Received_Data_Sum[i] += temp.x[i];
            System.out.println("The received data sum is:"+this.Received_Data_Sum[i]);
        }
        this.Received_Data_Size += Integer.parseInt(result[2].trim());
        System.out.println("The received data size is:"+Integer.parseInt(result[2].trim()));
        //now get the covariance
        /*for (int i=0;i<this.dimension;i++)
        {
            comp = result[i+3].split(" ");
            double[] a_cov = new double[this.dimension];
            for (int j=0;j<this.dimension;j++)
                a_cov[j] = Double.parseDouble(comp[j].trim());
            p = new Point(a_cov,-1);
            temp = new Point(this.dimension);
            for (int j=0;j<this.dimension;j++)
                temp.x[j] = p.x[j];
            
        }*/
        //this.DataCollectionRound = Integer.parseInt(result[result.length-3]);

        this.NewRoot = -1;
        this.OldRoot = -1;
        this.CentroidRound = -1;
        this.RecvdRound = -1;
        System.out.println("################end AccumulateRemoteData###################################");
    }
    
    public int CreateRootCount()
    {
        return (this.Received_Data_Size + this.S_i.size());
    }
    
    public double[] CreateRootSum()
    {
        AvgVector localAvg = new AvgVector(this.S_i,this.dimension);
        Point p = localAvg.Avg;

        double[] temp = new double[this.dimension];
        for (int j=0;j<this.dimension;j++)
            temp[j] = p.x[j]*this.S_i.size() + this.Received_Data_Sum[j];
        return temp;
    }
    
    //this method is called in the end of start() function, and just for start() function
    //to create message for every destination node
    //and call sendMessage method separately
    public void sendExtractedMessage(String Msg){
    	System.out.println("#########Begin sendExtractedMessage################");
    	//split message by destination nodes
    	if(Msg.equals("Empty Msg")){
    		this.getAgentInterface().sendMessageWithDelay(Msg,this.getNodeNumber(),IntervalofL2Msg);
    	}	
    	else if(Msg.equals("Empty Initialization Msg!")){
    		System.out.println("Empty Initialization Msg!");
    	}
    	else {
    	    String[] result = Msg.split("%");
    	    String[] temp = null;
    	    String MsgTemp="";
    	    for(int i=0;i<result.length;i++){
        	    System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
            }
    	    String[] result1 = result[0].split("\n");
    	    /*for(int i=0;i<result1.length;i++){
        	    System.out.println("Msg compoinents: result1["+i+"] is:"+result1[i]);
            }*/
		    String MessageLabel = result1[0];
		    System.out.println("MessageLabel is:"+MessageLabel);
		    //if(MessageLabel.equals("Initialization Msg!")){
			System.out.println("The result.length is:"+result.length);
			for(int i=0;i<result.length;i++){
				System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
				System.out.println("The result i is:"+i);
				if(result[i].startsWith("Initialization Msg!")){
					String[] m = result[i].split("\n");
					if(m[1].startsWith("Wait Msg")){
						MsgTemp="";
			    	    temp=result[i].split("#");
			    	    for(int j=0;j<temp.length;j++){
	            	        System.out.println("temp["+j+"] is:"+temp[j]);
	                    }
			    	    //i+=1;
			    	    //Message label; X_ij; X_ij.size; edge delay; wait time
	    		        MsgTemp="Wait Msg"+"\n"+temp[2];
	    		        System.out.println("The Msg send is:"+MsgTemp);
	    		        this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(temp[1]));
					}
				}
				else if (result[i].startsWith("Wait Msg")){
					MsgTemp="";
		    	    temp=result[i].split("#");
		    	    for(int j=0;j<temp.length;j++){
            	        System.out.println("temp["+j+"] is:"+temp[j]);
                    }
		    	    //i+=1;
		    	    //Message label; X_ij; X_ij.size; edge delay; wait time
    		        MsgTemp="Wait Msg"+"\n"+temp[2];
    		        System.out.println("The Msg send is:"+MsgTemp);
    		        this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(temp[1]));
		        }
				else if (result[i].startsWith("Wait Data Msg")){
					MsgTemp="";
		    	    temp=result[i].split("#");
		    	    for(int j=0;j<temp.length;j++){
            	        System.out.println("temp["+j+"] is:"+temp[j]);
                    }
		    	    //i+=1;
		    	    //Message label; X_ij; X_ij.size; edge delay; wait time
    		        MsgTemp="Wait Data Msg"+"\n"+temp[2];
    		        System.out.println("The Msg send is:"+MsgTemp);
    		        this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(temp[1]));
		        }
		    	else if(result[i].startsWith("DataSet")){
		    		MsgTemp="";
		        	System.out.println("Start with dataset");
		        	String[] result3 = result[i].split("\n");
		        	for(int j=0;j<result3.length;j++){
            	        System.out.println("result3["+j+"] is:"+result3[j]);
            	        if(j<result3.length-1){
            	            MsgTemp+=result3[j]+"\n";
            	        }
                    }
		        	temp=result3[3].split("#");
		        	for(int j=0;j<temp.length;j++){
            	        System.out.println("temp["+j+"] is:"+temp[j]);
                    }
		        	//i+=1;
		        	MsgTemp+=temp[2];
		        	System.out.println("The Msg send is:"+MsgTemp);
    		        this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(temp[1]),IntervalofL2Msg);
		        }
		    	else if(i==0){
		    		MsgTemp="";
					System.out.println("enter i==0");
					String [] result2 = result1[3].split("#");
        		    for(int j=0;j<result2.length;j++){
                	    System.out.println("Msg compoinents: result2["+j+"] is:"+result2[j]);
                    }
        		    //Message label; X_ij; X_ij.size; edge delay
        		    MsgTemp=MessageLabel+"\n"+result1[1]+"\n"+result1[2]+"\n"+result2[2];
            		
        		    System.out.println("Send message:"+MsgTemp);
        		    //message, destination and delay time
        		    this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(result2[1]),IntervalofL2Msg);
        		   // this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(result2[1]));
				}
				else {
					MsgTemp="";
					System.out.println("enter i!=0");
        		    result1 = result[i].split("\n");
        		    for(int j=0;j<result1.length;j++){
                	    System.out.println("Msg compoinents: result1["+j+"] is:"+result1[j]);
                    }
        		    String [] result2 = result1[3].split("#");
        		    for(int j=0;j<result2.length;j++){
                	    System.out.println("Msg compoinents: result2["+j+"] is:"+result2[j]);
                    }
        		    //Message label; X_ij; X_ij.size; edge delay
                	MsgTemp=MessageLabel+"\n"+result1[1]+"\n"+result1[2]+"\n"+result2[2];
            		        		  
        		    System.out.println("Send message:"+MsgTemp);
        		    //message, destination and delay time
        		    this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(result2[1]),IntervalofL2Msg);
        		   // this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(result2[1]));
				}
				
			}
		}
			
    	System.out.println("#########End sendExtractedMessage################");
    }
    
    //this method is called in the run() function
    public void sendExtractedMessageforRun(String Msg){
    	
    	System.out.println("##################Enter sendExtractedMessageforRun()##################################");
    	String[] result = null;
    	String[] result0 = null;
    	String[] result1 = null;
    	String[] result2 = null;
    	String[] result3 = null;
    	String MsgTemp="";
    	
    	//if message starts with some data and not with "Empty Msg"
    	if(!Msg.startsWith("Empty Msg")){
    		//split message by destination nodes
        	result = Msg.split("%");
        	for(int i=0;i<result.length;i++){
            	System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
            }
        	
        	for (int k=0;k<result.length;k++){
        		//if message starts with "Wait Msg"
            	if(result[k].startsWith("Wait Msg")){
            		System.out.println("start with Wait Msg");
            		MsgTemp="";
            		//result = Msg.split("%");
            		/*for(int i=0;i<result.length;i++){
                    	System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
                    }*/
            		result1 = result[k].split("#");
                	for(int i=0;i<result1.length;i++){
                       	System.out.println("Msg compoinents: result1["+i+"] is:"+result1[i]);
                    }
                	MsgTemp=result1[0]+"\n"+result1[2];
            		System.out.println("Send message:"+MsgTemp);
            		this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(result1[1]));
            	}
                
            	else if(result[k].startsWith("Wait Data Msg")){
            		System.out.println("start with Wait Data Msg");
            		//result = Msg.split("%");
            		/*for(int i=0;i<result.length;i++){
                    	System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
                    }*/
            		MsgTemp="";
            		result1 = result[k].split("#");
                	for(int i=0;i<result1.length;i++){
                       	System.out.println("Msg compoinents: result1["+i+"] is:"+result1[i]);
                    }
                	MsgTemp=result1[0]+"\n"+result1[2];
            		System.out.println("Send message:"+MsgTemp);
            		this.getAgentInterface().sendMessage(MsgTemp,Integer.parseInt(result1[1]));
            	}
            	
            	else if(result[k].startsWith("DataSet")){
            		System.out.println("start with DataSet");
            		//result = Msg.split("%");
            		/*for(int i=0;i<result.length;i++){
                    	System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
                    }*/
            		MsgTemp="";
            		result0 = result[k].split("\n");
            		result1 = result0[3].split("#");
                	for(int i=0;i<result1.length;i++){
                       	System.out.println("Msg compoinents: result1["+i+"] is:"+result1[i]);
                	}  
                	MsgTemp=result0[0]+"\n"+result0[1]+"\n"+result0[2]+"\n"+result1[2];
            		System.out.println("Send message:"+MsgTemp);
            		this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(result1[1]),IntervalofL2Msg);
            	}
            	
            	else if(result[k].startsWith("PatternStates")){
            		System.out.println("start with PatternStates");
            		//result = Msg.split("%");
            		/*for(int i=0;i<result.length;i++){
                    	System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
                    }*/
            		MsgTemp="";
            		result0 = result[k].split("\n");
            		result1 = result0[4].split("#");
                	for(int i=0;i<result1.length;i++){
                       	System.out.println("Msg compoinents: result1["+i+"] is:"+result1[i]);
                	}  
                	MsgTemp=result0[0]+"\n"+result0[1]+"\n"+result0[2]+"\n"+result0[3]+"\n"+result1[2];
            		System.out.println("Send message:"+MsgTemp);
            		this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(result1[1]),IntervalofL2Msg);
            	}
            	else if(result.length==1){
            		MsgTemp="";
            		System.out.println("result.length==1");
        			result1 = result[k].split("\n");
        			result2 = result1[2].split("#");
        			for(int j=0;j<result2.length;j++){
            			System.out.println("Msg compoinents: result2["+j+"] is:"+result2[j]);
            		}
        			MsgTemp=result1[0]+"\n"+result1[1]+"\n"+result2[2];
        		    System.out.println("Send message:"+MsgTemp);
        		    //X_ij; X_ij.size; edge delay
        		    this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(result2[1]),IntervalofL2Msg);
        		}
        		else{
        			MsgTemp="";
        			result1 = result[k].split("\n");
        			System.out.println("k!=0 and NOT start with DataSet");
        			result2 = result1[2].split("#");
        			for(int j=0;j<result1.length;j++){
                	    System.out.println("Msg compoinents: result1["+j+"] is:"+result1[j]);
                	}
            		for(int j=0;j<result2.length;j++){
                	    System.out.println("Msg compoinents: result2["+j+"] is:"+result2[j]);
                	}
            		MsgTemp+=result1[0]+"\n"+result1[1]+"\n"+result2[2];
            		System.out.println("Send message:"+MsgTemp);
            		this.getAgentInterface().sendMessageWithDelay(MsgTemp,Integer.parseInt(result2[1]),IntervalofL2Msg);
        		}	
        	}
        	
    	}
    	else if(Msg.equals("Empty Msg")){
    		this.getAgentInterface().sendMessageWithDelay(Msg,this.getNodeNumber(),IntervalofL2Msg);
    	}
    	
    	System.out.println("##################End sendExtractedMessageforRun()##################################");
    	
    }
    
    public void FetchMessageComponents(String Msg, int fromNodeNumber)
    {
    	System.out.println("########begin fetch message########");
        System.out.println("Msg as recvd : "+Msg);
        int index = -1;
        StringTokenizer st;
        String[] result0 = Msg.split("%");
        for (int kk=0; kk<result0.length;kk++){
        	if(result0[kk].startsWith("Wait Data Msg")){
        		System.out.println("Wait Data Msg");
        	}
        	else if(result0[kk].startsWith("Wait Msg")){
        		System.out.println("Wait Msg");
        	}
        	else if(result0[kk].startsWith("DataSet")){
        		String[] result = Msg.split("\n");
        	}
        	else{
        		String[] result = Msg.split("\n");
        		for(int i=0;i<result.length;i++){
        			System.out.println("Msg compoinents: result["+i+"] is:"+result[i]);
        		}
        
                if (result[0].equals("Initialization Msg!")){
                	//this.getAgentInterface().getTime() = Integer.parseInt(result[result.length-1]);
                	st = new StringTokenizer(result[1]," ");
                	System.out.println("Initialization the token count is:"+st.countTokens());
                } else{
                	st = new StringTokenizer(result[0]," ");
                	System.out.println("the token count is:"+st.countTokens());
                }
        
        
                int j=this.getNodeNumber();
                System.out.println("receive node number is:"+j);
                System.out.println("from node number is:"+fromNodeNumber);
                for(int k=0;k<this.nbors.size();k++){
                	Neighbor n = (Neighbor)nbors.get(k);
                	//if the neighbor number is equal to the fromNodeNumber
                	if (fromNodeNumber==n.getNodeNumber()){
                		index = k;
                		System.out.println("The from node number index is:"+index);
                		break;
                	}
                }
                if (index != -1){
                	System.out.println("index != -1");
                	for (int i=0;i<this.dimension;i++){
                		String temp = st.nextToken();
                		System.out.println("the token is:"+temp);
                		X_ji[index].Avg.x[i] = Double.parseDouble(temp);
                		System.out.println("X_ji["+index+"].Avg.x["+i+"] is:"+X_ji[index].Avg.x[i]);
                	}
                	if(result[0].equals("Initialization Msg!")){
                        X_ji[index].size = Integer.parseInt(result[2]);
                        System.out.println("X_ji["+index+"].size is:"+X_ji[index].size);
                	}else {
                		X_ji[index].size = Integer.parseInt(result[1]);
                        System.out.println("X_ji["+index+"].size is:"+X_ji[index].size);
                	}
                }        	
            }
        }
        System.out.println("########End fetch message########");
        
    }
    
    public void UpdateAfterMsgSent(int instance, int neighbor)
    {
        int k = neighbor;
       
        //A_ij
        A_ij[k].size = X_ij[k].size + X_ji[k].size;

        W_ij[k].size = K_i.size - A_ij[k].size;
        if (W_ij[k].size < 0)
            W_ij[k].size = 0;

        Neighbor n = (Neighbor)nbors.get(k);
        for (int j=0;j<this.dimension;j++)
        {
            if (A_ij[k].size!=0)
            {
                A_ij[k].Avg.x[j] = (X_ij[k].Avg.x[j] * X_ij[k].size +
                X_ji[k].Avg.x[j] * X_ji[k].size)/
                (A_ij[k].size);

            }
            else
                A_ij[k].Avg.x[j] = 0.0;

            //W_ij
            if (W_ij[k].size!=0)
            {
                W_ij[k].Avg.x[j] = (K_i.Avg.x[j] * K_i.size -
                A_ij[k].Avg.x[j] * A_ij[k].size)/
                (W_ij[k].size);
            }
            else
                W_ij[k].Avg.x[j] = 0.0;
        }
        //DEBUG BEGIN
        //DEBUG END
    }

}
  

