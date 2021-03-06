import java.io.*;
import java.net.*;
import java.util.Scanner;
import java.util.ArrayList;

/********************************************************************************
* CIS 457
* Project 2
* Recursive DNS caching resolver
*
*
* @author Joel Truman
* @version Fall 2015
********************************************************************************/

class Project2Resolver {

  /* Port value used if user input is invalid */
  static int DEFAULT_PORT = 8025;

  static ArrayList<String> results;
  static ArrayList<String> answers;
  static Cache cache;

  /**************************************************************************
  * Main method that prompts user input and opens a DatagramSocket
  **************************************************************************/
  public static void main(String[] args) throws IOException {
    cache = new Cache();
    //Read command line argument
    String userInput;
    int socketNum;
    DatagramSocket serverSocket = null;
    if(args.length > 0)
      userInput = args[0];
    else
      userInput = "8025";

    //Check for valid argument
    if(isInteger(userInput))
      socketNum = Integer.parseInt(userInput);
    else {
      System.out.printf("The port you specified was not an integer. A default value of %d has been substituted.\n", DEFAULT_PORT);
      socketNum = DEFAULT_PORT;
    }

    // Create a socket that listens on port designated by user.
    try {
      serverSocket = new DatagramSocket(socketNum);
    }
    catch (IOException e) {
      System.out.printf("The port you specified cannot be used. Please launch the application again.\n");
      System.exit(1);
    }
    System.out.printf("DNS resolver started on port %d\n", socketNum);
    // Set server to listen for a DatagramPacket from client
    byte[] receiveData = new byte[1024];
    DatagramPacket packet = new DatagramPacket(receiveData, receiveData.length);
    serverSocket.receive(packet);
	
    // Print packet info from client using helper class PacketInfo()
    PacketInfo information = new PacketInfo(packet);
    information.getValues();
    information.getQuestions();
    information.getAnswers();
    information.getAuthority();
    //System.out.printf("%s\n", information.getValues());
    //System.out.printf("%s\n", information.getQuestions());
    //System.out.printf("%s\n", information.getAnswers());
    //System.out.printf("%s\n", information.getAuthority());
    //System.out.printf("%s\n----------------------\n", information.getAdditional());
    System.out.printf("\nReceived query from client for %s\n", information.getNameRequested());

    // Check for question type A and class type IN before proceeding
    if(!information.getValidQuestion()) {
      information.setErrorCode();
      System.out.println("\nSending answer to client\n");

      //send error code back to client
      InetAddress retAddress = InetAddress.getByName("127.0.0.1");
      int port = packet.getPort();
      DatagramPacket toClient = new DatagramPacket(information.getByteArray(),
	information.getByteArray().length, retAddress, port);
      serverSocket.send(toClient);
    }
    boolean inCache = false;
    boolean doneSearching = false;
    if(cache.searchCache(information.getNameRequested()) != null){
	inCache = true;
	doneSearching = true;	
    }
    //Send DatagramPacket down server tree with recursion desired bit unset
    information.unsetRecursion();
    String nextServer = "198.41.0.4"; //Initial server to check is 198.41.0.4
    boolean error = false;
    InetAddress address;
    DatagramPacket response;
    PacketInfo responseInfo = null;

    //Loop until an answer is found or there is an error
    while(!doneSearching && !error) {
      address = InetAddress.getByName(nextServer);
      DatagramPacket sendPacket = new DatagramPacket(information.getByteArray(),
	information.getByteArray().length, address, 53);
      serverSocket.send(sendPacket);

      byte[] fromServer = new byte[1024];
      response = new DatagramPacket(fromServer, fromServer.length);
      serverSocket.receive(response);

      responseInfo = new PacketInfo(response);
    
      System.out.printf("\nQuerying server %s\n", nextServer);
      //read information in received packet
      responseInfo.getValues();
      responseInfo.getQuestions();
      responseInfo.getAnswers();
      responseInfo.getAuthority();
      responseInfo.getAdditional();

      //This code is useful for viewing more complete packing info in terminal
      //System.out.printf("%s\n", responseInfo.getValues());
      //System.out.printf("%s\n", responseInfo.getQuestions());
      //System.out.printf("%s\n", responseInfo.getAnswers());
      //System.out.printf("%s\n", responseInfo.getAuthority());
      //System.out.printf("%s\n----------------------\n", responseInfo.getAdditional());
      //System.out.println(responseInfo.isAnswer());
      //System.out.println(responseInfo.nextIP());
      if(!inCache){
      	System.out.printf("Received answer: %b\n", responseInfo.isAnswer());
      	error = responseInfo.isError();
      	doneSearching = responseInfo.isAnswer();
      	nextServer = responseInfo.nextIP();
      	results = responseInfo.getResults();
      }
      if(!doneSearching) {
	System.out.println("Authority records found:");
	  for(int i = 0; i < results.size(); i++) {
	    System.out.println(results.get(i));
	  }
      }
      if(nextServer.equals("")) {
	error = true;
	responseInfo.setErrorCode();
      }
   } 
	

    if(!inCache){	    
    //Send answer back to client
    answers = responseInfo.getAnswerRecords();
    System.out.println("Answers found:");
    for(int i = 0; i < answers.size(); i++) {
      System.out.println(answers.get(i));
    }
    System.out.println("\nSending answer to client\n");
   
    address = InetAddress.getByName("127.0.0.1");
    int port = packet.getPort();
  
    DatagramPacket toClient = new DatagramPacket(responseInfo.getByteArray(),
      responseInfo.getByteArray().length, address, port);
    if(answers.size() > 0){
    	cache.addToCache(information.getNameRequested(), answers.get(0));
    	cache.storeCache();
    }	
    serverSocket.send(toClient);
   }

   if(inCache){
	String a = cache.searchCache(information.getNameRequested());	
   	System.out.println("Answer found in cache: " + a);
   	System.out.println("\nSending answer to client\n");
   	address = InetAddress.getByName("127.0.0.1");
   	int portx = packet.getPort();
  
   	DatagramPacket toClientx = new DatagramPacket(information.getByteArray(),
     		information.getByteArray().length, address, portx);
    }
  }

  /*********************************************************
  * Method to check if user generated input is an integer
  **********************************************************/
  public static boolean isInteger(String str) {  
    
    try {  
      int i = Integer.parseInt(str);  
    }  
    catch(NumberFormatException e) {  
      return false;  
    }  
    return true;  
  }

}
