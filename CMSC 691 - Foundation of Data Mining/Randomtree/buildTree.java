package DDMT.Nodes;

import java.io.*;
import java.util.*;
import java.util.regex.*;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.StringTokenizer;
import java.lang.String;
import java.util.Vector;

/* TreeNode and DataPoint classes to hold data and define tree nodes
 * DataPoint: contains an attribute array to hold the values for a particular node and is 
 * contained within each TreeNode instance. 
 */
public class buildTree {
	
    //The class to represent a node in the decomposition tree.
	public class TreeNode{
					
		/* If this is not a leaf node, the attribute that is used to divide the set of 
		 * data points*/
		public int decompositionAttribute;	
		// the attribute-value that is used to divide the parent node
		public int decompositionValue;		
		public int treeLevel=0;
		public TreeNode []children;	// If this is not a leaf node, references to the children nodes
		public TreeNode parent;	// The parent to this node.  The root has parent == null
		public TreeNode(){}	
	};
	// The class for leaf node structure
	public class leafPoint{
		public String leafSchema;
		//public int[][] structuredSchema;
		public String data;		// The set of data points if this is a leaf node
		public int[] classCount; //number of data tuples per class
		public int totalCount; // total number of data tuples that fall into this leaf node
		public double[] distributionStatistics;
		public leafPoint(int numClasses){
			data="";
			classCount= new int[numClasses];
			totalCount=0;
			distributionStatistics=new double[numClasses];
		}
	}
	
//  The class to represent a data point consisting of numAttributes values of attributes 
	public class DataPoint{
		public int [] attributes;
		/* The values of all attributes stored in this array.  i-th element in this array
		   is the index to the element in the vector domains representing the symbolic value of
		   the attribute.  For example, if attributes[2] is 1, then the actual value of the
		   2-nd attribute is obtained by domains[2].elementAt(1).  This representation makes
		   comparing values of attributes easier - it involves only integer comparison and
		   no string comparison.
		   The last attribute is the output attribute
		*/

		public String label;
		public DataPoint(final int numattributes){
			attributes = new int[numattributes];
		}
	};
	
	
	
/*
 * This is the base class for decision tree implementation and has a containment relation 
 * with the TreeNode class. It encapsulates all the implementation details for decision tree
 * creation and entropy calculation. It uses the max-gain criteria for splitting over nodes. 
 */
	
	public int numAttributes=11;  //The number of attributes including the output attributes
	public String [] attributeNames;
	int numClasses=2; //number of classes
	/*Possible values for each attribute is stored in a vector. domains is an array of dimension
	 * numAttributes. Each element of this array is a vector that contains values for the
	 * corresponding attribute. domains[0] is a vector containing the values of the 0th 
	 * attribute, etc.. The last attribute is the output attribute.
	*/
	
	public Double[] randomSeed = new Double[100];
	public Vector [] domains;
	public String [] classNames;
	public TreeNode trainingRoot;
	public TreeNode testingRoot;
	public String namesFile;
	public String trainingDataFile;
	public String testingDataFile;
	public int correctCount;
	public int inCorrectCount;
	public boolean debug;
	public int maxDepth;
	public int treeDepth=3;
	int numOfLeafNode = treeDepth*3;
	public leafPoint[] leafnodelist = new leafPoint[numOfLeafNode];
		
	public int leafIndex=0;
	/*  The root of the decomposition tree  */
	TreeNode root = new TreeNode();
    public String statistics="";
	
/*
 * The decision-tree class invokes the InputProcessor (explained below) , which populates the 
 * training and testing Roots with input data. After this, it invokes createDecisionTree and 
 * induce modules to the create the actual tree.
 */
	public TreeNode induceTree (TreeNode node,int treeDepth, int DepthCounter,int k) {
        //System.out.println("################Enter induceTree()######################");
		boolean selected = false;
		int selectedAttribute;
		//int numdata = node.data.size ();
	    int i=0;
		while (DepthCounter<treeDepth){
			//randomly select an attribute
			selectedAttribute = attrIndex(0,numAttributes-2,k,node);
			System.out.println("selectedAttribute.elementAt("+i+") is:"+selectedAttribute);
			i+=1;
			//check to see whether this attribute already selected in the path
			if (alreadyUsedToDecompose (node, selectedAttribute)) {
				k+=1;
				selected= false;
				continue;
			}
			else {
				selected = true;
				int numvalues = domains[selectedAttribute].size ();
				node.decompositionAttribute = selectedAttribute;
				node.children = new TreeNode [numvalues];
				++DepthCounter;
				System.out.println("current RDT_depth is:"+DepthCounter);
				System.out.println("number of values for the attribute is:"+numvalues);
				for (int j = 0; j < numvalues; j++) {
					//System.out.println("ok");
				    node.children[j] = new TreeNode ();
				    node.children[j].parent = node;
				    node.children[j].treeLevel=DepthCounter;
				    //node.children[j].data = getSubset(node.data,selectedAttribute, j);
				    node.children[j].decompositionValue = j;
				    if(DepthCounter==treeDepth-1){
				    	node.children[j].children = new TreeNode [numvalues];
				    	for(int m=0; m<numvalues; m++){
				    	    node.children[j].children[m] = new TreeNode ();
					        node.children[j].children[m].parent = node.children[j];
					        node.children[j].children[m].treeLevel=DepthCounter+1;
					        //node.children[j].data = getSubset(node.data,selectedAttribute, j);
					        node.children[j].children[m].decompositionValue = m;
				    	}
				    }
				}
               
				if(DepthCounter<treeDepth-1){
				    for (int j = 0; j < numvalues; j++) {
					     //System.out.println("haha");
					     k+=1;
					     induceTree (node.children[j],treeDepth,node.children[j].treeLevel,k);
				    }
				}
				break;
			}
		}
		//System.out.println("################End induceTree()######################");
		//node.data = null;
		return node;
	}

/*  This function checks if the specified attribute is used to decompose the data set
	in any of the parents of the specfied node in the decomposition tree.
	Recursively checks the specified node as well as all parents
*/
	public boolean alreadyUsedToDecompose(TreeNode node, int attribute) {
		if (node.children != null) {
			if (node.decompositionAttribute == attribute ){
				System.out.println("This attribute has already been selected in the path");
				return true;
			}
		}
		if (node.parent == null) return false;
		return alreadyUsedToDecompose(node.parent, attribute);
	}
	
	
	
/*  This function returns an integer corresponding to the symbolic value of the attribute.
	If the symbol does not exist in the domain, the symbol is added to the domain of the 
	attribute
*/
/*	public int getSymbolValue(int attribute, String symbol) {
		int index = domains[attribute].indexOf(symbol);
		if (index < 0) {
			//System.out.println("add element to domain");
			domains[attribute].addElement(symbol);
			return domains[attribute].size() -1;
		}
		return index;
	}
	*/
	
//For numerical data, I set: [0,3),[3,6),[6,infinity)
	public int getContinousValue(int attribute, String value){
		String symbol="";
		if (Integer.parseInt(value) >=0 && Integer.parseInt(value)<3 )
			symbol="[0,3)";
		else if(Integer.parseInt(value)>=3 && Integer.parseInt(value)<6)
			symbol="[3,6)";
		else if(Integer.parseInt(value)>=6)
		    symbol="[6,infinity)";
		
		int index=domains[attribute].indexOf(symbol);
		return index;
	}
	
/* Function to initialize the name of the attributes and the splitting points.
	The first line of the data file should contain the names of all attributes.
	The number of attributes is inferred from the number of words in this line.
	The last word is taken as the name of the output attribute.
*/

	public void initData() {
         
		 //define the splitting point of the numerical data
		 domains = new Vector[numAttributes-1];
		 String symbol0="[0,3)";
		 String symbol1="[3,6)";
		 String symbol2="[6,infinity)";
		 for (int i=0; i < numAttributes-1; i++){ 
			 domains[i] = new Vector();
			 domains[i].addElement(symbol0);
			 domains[i].addElement(symbol1);
			 domains[i].addElement(symbol2);
		 }
		 
		 //name of the attributes. Here the name is the index of the attributes
		 attributeNames = new String[numAttributes];
       	 for (int i=0; i < numAttributes-1; i++) {
	         attributeNames[i]  = Integer.toString(i);
	         System.out.println("name of attributes:"+attributeNames[i]);
	     }
       	 attributeNames[numAttributes-1]="label";
       	 
       	 //name of the classes, this needs to be read from a file
       	 classNames = new String[numClasses];
       	 classNames[0]="Hot Stars";
       	 classNames[1]="Galaxies";
       	 
       	       	 
       	 
	}

/*  This function prints the decision tree in the form of rules.
	The action part of the rule is of the form
		outputAttribute = "symbolicValue"
	or
		outputAttribute = { "Value1", "Value2", ..  }
	The second form is printed if the node cannot be decomposed any further into an homogenous set
*/
    public void printTree(TreeNode node,int treeDepth, String tab) {
    	int outputattr = numAttributes-2;
    	
    	
    	//System.out.println("outputattr:"+outputattr);
    	if (node.treeLevel==treeDepth-1) {
    		//System.out.println("node has no children.");
    		
    		TreeNode node1=node.parent;
    		String temp="";
    		temp=attributeNames[node1.decompositionAttribute]+":"+domains[node.decompositionValue].elementAt(node.decompositionValue)+";";
    		//leafnodelist[leafIndex].structuredSchema[leafIndex][1]=node1.children[node1.decompositionValue].decompositionValue;
    		while(node1!=null&&node1.parent!=null){
    			TreeNode node2=node1;
    			node1=node1.parent;
    			String temp1=temp;
    			temp = attributeNames[node1.decompositionAttribute]+":"+domains[node2.decompositionValue].elementAt(node2.decompositionValue)+";";
    			//leafnodelist[leafIndex].structuredSchema[leafIndex][1]=node1.children[node1.decompositionValue].decompositionValue;
    			temp=temp.concat(temp1); 			
    		}
    		leafnodelist[leafIndex]=new leafPoint(numClasses);
    		leafnodelist[leafIndex].leafSchema=temp;
    		System.out.println("leafnodelist["+leafIndex+"].leafSchema is:"+leafnodelist[leafIndex].leafSchema);
    		try {
				//updateStatistics(fileName, node,treeDepth);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    		leafIndex+=1;
   		    return;
	    }
    	else if (node.children!=null && node.treeLevel<treeDepth-1){
    		int numvalues = node.children.length;
    		//System.out.println("node is not leaf node.");
            for (int i=0; i < numvalues; i++) {
            	//System.out.println("node.decompositionAttribute"+node.decompositionAttribute);
            	System.out.println(tab + "if( the value of attribute " + attributeNames[node.decompositionAttribute] + " is in " +domains[node.decompositionAttribute].elementAt(i) + ") {" );
            	if (node.children[i].children==null){
            		printTree(node.children[i],treeDepth, tab + "\t");
            		break;
            	}else
            		printTree(node.children[i],treeDepth, tab + "\t");
            	
                if (i != numvalues-1) System.out.print(tab +  "} else ");
                else System.out.println(tab +  "}");
            }
    	}
    	
    }

   /*  This function creates the decision tree and prints it in the form of rules on the console
	*/
	public TreeNode createDecisionTree(int treeDepth, int DepthCounter) {
		System.out.println("############Enter createDecisionTree()#################");
		String fileName="D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\matrixGal1.txt";
		readRandomSeed();
		induceTree(root,treeDepth, DepthCounter,0);
		printTree(root,treeDepth, "");
		try {
			updateStatistics(fileName, root, treeDepth);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("############End createDecisionTree()#################");
		return root;
	}

	//randomly select an attribute
	/*public int attrIndex(int aStart, int aEnd){
		Random random = new Random();
		long range = (long)aEnd-(long)aStart+1;
		long fraction = (long)(range*random.nextDouble());
		int randomNumber =(int)(fraction+aStart);
		System.out.println("random selected attribute index is:"+randomNumber);
		return randomNumber;
	}*/
	public Double[] readRandomSeed(){
		String fileName1 = "D:\\Ph.D\\Spring 2010\\research\\experiment\\mycode\\randomNumber.txt";
		FileInputStream in = null;
				
		try {
		     File inputFile = new File (fileName1);
		     in = new FileInputStream (inputFile);
		} 
		catch (Exception e) {
		     System.err.println ("Unable to open file:" + fileName1 + "n" + e);
		     System.exit(0);
		}
		BufferedReader bin = new BufferedReader (new InputStreamReader (in));
		//Double[] randomSeed= new Double[100];
		String input="";
		int i=0;
		
		while(true) {
			try {
				input = bin.readLine();
				if (input == null) break;
				else{
				    //System.out.println("input is:"+input);
				    randomSeed[i]=Double.parseDouble(input);
				    //System.out.println("node.randomSeed["+i+"] is:"+node.randomSeed[i]);
				}
				i+=1;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}     			
		}
		return randomSeed;
		
	}
	public int attrIndex(int aStart, int aEnd, int k ,TreeNode node){
		long range = (long)aEnd-(long)aStart+1;
		//System.out.println("First The node.randomSeed["+k+"] is:"+node.randomSeed[k]);
		long fraction = (long)(range*randomSeed[k]);
		//System.out.println("The node.randomSeed["+k+"] is:"+node.randomSeed[k]);
		int randomNumber =(int)(fraction+aStart);
		//System.out.println("New random selected attribute index is:"+randomNumber);
		return randomNumber;
	}
	
	/*After constructing the decision tree, using the training data to update the leaf node class 
	probability distributions*/
	public int updateStatistics(String fileName, TreeNode node, int treeDepth)  throws Exception {
         
         
		 FileInputStream in = null;
		 
		 try {
		     File inputFile = new File (fileName);
		     in = new FileInputStream (inputFile);
		 } 
		 catch (Exception e) {
		     System.err.println ("Unable to open file:" + fileName + "n" + e);
		     return -1;
		 }

		 BufferedReader bin = new BufferedReader (new InputStreamReader (in));
		 String input="";
		 StringTokenizer tokenizer = new StringTokenizer(input);
		 
		 while(true) {
			 TreeNode nodeTemp=node;
      		 input = bin.readLine();
      		 if (input == null) break;
      		 if (input.startsWith("//")) continue;
      		 if (input.equals("")) continue;

			 tokenizer = new StringTokenizer(input,",");
			 int numtokens = tokenizer.countTokens();
			 
			 //whenever read a datapoint
			 DataPoint point = new DataPoint(numAttributes);
	     	 for (int i=0; i < numAttributes; i++) {
	     		 if (i==(numAttributes-1)){
	     			 point.label=tokenizer.nextToken();
	     			 System.out.println("the label is:"+point.label);
	     		 }
	     		 else{
	     			//point.attributes[i]  = getSymbolValue(i, tokenizer.nextToken() );
		     		 point.attributes[i]  = getContinousValue(i, tokenizer.nextToken() );
		     		 System.out.println("point attributes:"+point.attributes[i]);
		     		 System.out.println("actual attributes value:"+domains[i].elementAt(point.attributes[i]));
	     		 }
	             
	     	 }
	     	 
	     	//recursively find the matched path
	     	 String temp="";
	     	 nodeTemp = nodeTemp.children[point.attributes[node.decompositionAttribute]];
	     	 temp=node.decompositionAttribute+":"+domains[node.decompositionAttribute].elementAt(point.attributes[node.decompositionAttribute])+";";
			 for (int j=1;j<treeDepth-1;j++){
				 if(nodeTemp!=null&&nodeTemp.children!=null){
					 TreeNode nodeTemp1=nodeTemp;
				     nodeTemp = nodeTemp.children[point.attributes[nodeTemp.decompositionAttribute]];
				     temp+=nodeTemp1.decompositionAttribute+":"+domains[nodeTemp1.decompositionAttribute].elementAt(point.attributes[nodeTemp1.decompositionAttribute])+";";
				 }
			 }
			 int matchIndex=0;
			 for (int k=0;k<treeDepth*3;k++){
				 System.out.println("leafnodelist["+k+"].leafSchema is:"+leafnodelist[k].leafSchema);
				 System.out.println("temp is:"+temp);
				 if(leafnodelist[k].leafSchema.equals(temp)){
					 matchIndex=k;
					 leafnodelist[k].totalCount++;
					 System.out.println("matchIndex is:"+matchIndex);
				 }
			 }
				 
	     	 //determine the label of the data and update count
			 String[] newPoint = new String[numClasses];
			 for(int i=0;i<numClasses;i++){
				 //System.out.println("classNames["+i+"] is:"+classNames[i]);
				 //System.out.println("point.label is:"+point.label);
				 if (point.label.equals(classNames[i])){
					 System.out.println("haha classNames[i] is:"+classNames[i]);
					 newPoint[i]="1";
					 leafnodelist[matchIndex].classCount[i]++;
				 }
				 else{
					 newPoint[i]="0";
				 }
			 }
			 String newPoint1="";
			 for (int i=0;i<numClasses;i++){
				 if (i==numClasses-1)
					 newPoint1+=newPoint[i];
				 else
					 newPoint1+=newPoint[i]+",";
			 }
			 System.out.println("The new added point is:"+newPoint1);
			 leafnodelist[matchIndex].data+=newPoint1+"\n";
			 System.out.println("The current leafnodelist["+matchIndex+"].data is:"+leafnodelist[matchIndex].data);
			 /*//print the class probability distribution for the leaf node
			 for(int i=0;i<numClasses;i++){
				 System.out.println("the data count is:"+node.data[i]);
			 }*/
			 
		 }
		 String temp="";
		 String temp1="";
		 for(int i=0;i<treeDepth*3;i++){
			 System.out.println("haha leafnodelist["+i+"].totalCount is:"+leafnodelist[i].totalCount);
			 for(int j=0;j<numClasses;j++){
				 System.out.println("leafnodelist["+i+"].classCount["+j+"] is:"+leafnodelist[i].classCount[j]);
				 if (i==treeDepth*3-1 && j==numClasses-1){
					 temp+=leafnodelist[i].classCount[j];
				     temp1+=leafnodelist[i].totalCount;
				 }
				 else{
					 temp+=leafnodelist[i].classCount[j]+",";
					 temp1+=leafnodelist[i].totalCount+",";
				 }
			 }
		 }
		 statistics=temp+"\n"+temp1;
		 System.out.println("statistics is:"+statistics);
		 bin.close();
         return 1;
	}
	
}