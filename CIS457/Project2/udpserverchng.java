import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.*;

public class udpserverchng{
	public static ArrayList<String> queries = new ArrayList<String>();
	public static ArrayList<String> usedQueries = new ArrayList<String>();
	public static final ArrayList<String> ROOT_SERVERS = new ArrayList<String> (Arrays.asList("198.41.0.4",
			"192.228.79.201", "192.33.4.12", "199.7.91.13",
			"192.203.230.10", "192.5.5.241", "192.112.36.4",
			"128.63.2.53", "192.36.148.17", "192.58.128.30",
			"193.0.14.129", "199.7.83.42", "202.12.27.33"));

    public static void main(String args[]) throws Exception{
		int portnum;
		DatagramSocket serverSocket;

		BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));
		System.out.println("Enter a port number: ");
   	    String tempport = inFromUser.readLine();

		//Try to parse the uer port number
		try{
			portnum = Integer.parseInt(tempport);
		}
		catch (Exception e){
			System.out.println("Error parsing port number");
			return;
		}

		//Try to create the socket
		try{
			serverSocket = new DatagramSocket(portnum);
		}
		catch (Exception e){
			System.out.println("Error creating socket");
			return;
		}	

		while(true){
		    byte[] queryData = new byte[1024];	

		    DatagramPacket receivePacket = new DatagramPacket(queryData,queryData.length);
			try{
				serverSocket.receive(receivePacket);
			}
			catch (Exception e){
				System.out.println("Error receiving data");
			}
	
			//Wraps the data into a buffer. Bytes of a multibyte value are ordered from most
			//significant to least significant
			ByteBuffer receiveDataByteBuff = ByteBuffer.wrap(queryData).order(ByteOrder.BIG_ENDIAN);

			//Prints the original query
			System.out.println("\n\nORIGINAL QUERY:");
			//printDNSPacket(receiveData, receiveDataByteBuff, receiveData, receiveDataByteBuff);

			//Sets RD flag to 0
			short flags = receiveDataByteBuff.getShort(2);
			flags &= ~(1<<8);
			receiveDataByteBuff.putShort(2,flags);

			//Instantiate variables to get a valid response from a server. 
			queries.clear();
			queries.addAll(ROOT_SERVERS);

			String query = getFirstDNSQueryString(queryData, receiveDataByteBuff);
			//boolean cacheHasAnswer = setUpSavedQueriesWithCache(query);
			byte[] recvData = new byte[1024];
			ByteBuffer bb = ByteBuffer.wrap(recvData).order(ByteOrder.BIG_ENDIAN);
			boolean mustFindValidAnswer = true;
			while(mustFindValidAnswer && queries.size() > 0){
				//Get next IP address for valid answer
				String nextIP = queries.remove(queries.size() - 1);
				usedQueries.add(nextIP);
				System.out.println("Next IP" + nextIP);
				InetAddress address;
				address = InetAddress.getByName(nextIP);
				DatagramPacket sendPacket = new DatagramPacket(queryData, queryData.length, address, 53);
                                serverSocket.send(sendPacket);
				serverSocket.setSoTimeout(2000);
				// Make sure the buffer is clear before
				//receiving the response
				bb.clear();
				DatagramPacket recvPacket = new DatagramPacket(recvData, recvData.length);
				serverSocket.receive(recvPacket);
				// Print the response received
                                System.out.println("\n\nRESPONSE:");
                                mustFindValidAnswer = !printDNSPacket(recvData, bb, queryData, receiveDataByteBuff);
			}
			if(!mustFindValidAnswer){

			}else{

			}

















			//Shitty code we wrote
		    //String message = new String(receiveData);
		    //InetAddress address = InetAddress.getByName(message);
		    //System.out.println(address.getHostAddress());
		    //String adrs = address.getHostAddress();
		    //InetAddress IPAddress = receivePacket.getAddress();
		    //int port = receivePacket.getPort();
		    //System.out.println("Got message: "+message);
		    //byte[] sendData = adrs.getBytes();
		    //DatagramPacket sendPacket = new DatagramPacket(sendData,sendData.length,IPAddress,port);
	    //serverSocket.send(sendPacket);



		}
    }

	private static String getCompressedDNSResponseString(ByteBuffer byteBuf, byte[] data, int index){
		String responseString;
		int numChars, savedIndex, offset;
		boolean isCompressing; 

		//Initialize variables
		responseString = "";
		numChars = (int)data[index] & 0x0FF;
		isCompressing = false;
		savedIndex = -1;

		while (numChars > 0){
		
			//Checks if the we should compress the data
			//This is a pointer.
			if (numChars >= 192){
				
				//Calculates the offset
				offset = (int)byteBuf.getShort(index - 1) & 0x3FFF;
				
				if(!isCompressing){
					savedIndex = index;
				}

				index = offset;
				isCompressing = true;
				numChars = (int)data[index] & 0x0FFFF;
				index++;
			}

			//Reads the data into our output string
			for (int j = 0; j < numChars; j++){
				responseString += (char)data[index];
				index++;
			}
			responseString +=".";

			//Gets the next numChars
			numChars = data[index] & 0x0FFF;
			index++;
		}

		if(isCompressing){
			index = savedIndex +1;
		}

		if(responseString.length() > 0){
			responseString = responseString.substring(0,responseString.length()-1);
		}		
		return responseString;
	}

	private static String getFirstDNSQueryString(byte[] data, ByteBuffer byteBuf){
		return getCompressedDNSResponseString(byteBuf, data, 12);
	}

}
