import java.net.*;
import java.util.*;
import java.nio.*;


public class project2 {

	public static ArrayList<String> savedQueries = new ArrayList<String>();
	public static ArrayList<String> usedQueries = new ArrayList<String>();
	public static final ArrayList<String> ROOT_SERVERS =
		new ArrayList<String> (Arrays.asList("198.41.0.4",
			"192.228.79.201", "192.33.4.12", "199.7.91.13",
			"192.203.230.10", "192.5.5.241", "192.112.36.4",
			"128.63.2.53", "192.36.148.17", "192.58.128.30",
			"193.0.14.129", "199.7.83.42", "202.12.27.33"));

	@SuppressWarnings("resource")
	public static void main (String[] args) {
		int port = 9876;
		if (args.length == 1) {
				port = Integer.parseInt(args[0]);
		}
		else if (args.length > 1) {
			System.out.println("Too many arguments. Only " +
				"specifying the port number is supported.");
			return;
		}


		DatagramSocket serverSocket;
		try {
			serverSocket  = new DatagramSocket(port);
		}
		catch (Exception e) {
			System.out.println("Error creating socket");
			return;
		}

		while (true) {
			byte[] queryData = new byte[1024];
			DatagramPacket queryPacket =
				new DatagramPacket(queryData, queryData.length);
			try {
				serverSocket.receive(queryPacket);
			}
			catch (Exception e) {
				System.out.println("Error receiving query");
				break;
			}
			ByteBuffer qbb = ByteBuffer.wrap(queryData).
				order(ByteOrder.BIG_ENDIAN);

			// Print the original query
			printDNSPacket(queryData, qbb, queryData, qbb);

			// Sets RD flag tp 0
			short flags = qbb.getShort(2);
			flags &= ~(1<<8);
			qbb.putShort(2, flags);

			// Print the query to be sent to other servers
			printDNSPacket(queryData, qbb, queryData, qbb);

			// Instantiate variables to use to try to get a valid
			// response from a server
			usedQueries.clear();
			savedQueries.clear();
			savedQueries.addAll(ROOT_SERVERS);


			byte[] recvData = new byte[1024];
			ByteBuffer bb = ByteBuffer.wrap(recvData).
				order(ByteOrder.BIG_ENDIAN);
			boolean mustFindValidAnswer = true;
			while(mustFindValidAnswer &&
			  savedQueries.size() > 0) {

				// Get the next IP address to query for a
				// valid answer	
				String nextIP = savedQueries.remove(
					savedQueries.size()-1);
				usedQueries.add(nextIP);
				InetAddress address;
				try {
					address = InetAddress.getByName(nextIP);
				}
				catch (Exception e) {
					System.out.println("Error getting " +
						" address");
					break;
				}

				// Send the query to the next IP address
				try {
					DatagramPacket sendPacket =
						new DatagramPacket(queryData,
							queryData.length,
							address, 53);
					serverSocket.send(sendPacket);

					// Set a socket timeout of 2 seconds
					serverSocket.setSoTimeout(2000);
				}
				catch (Exception e) {
					System.out.println("Error sending " +
						"packet");
					break;
				}

				// Make sure the buffer is clear before
				// receiving the response
				bb.clear();
				DatagramPacket recvPacket =
					new DatagramPacket(recvData,
					recvData.length);
				try {
					serverSocket.receive(recvPacket);

					// Print the response received
					mustFindValidAnswer =
						!printDNSPacket(recvData, bb,
							queryData, qbb);

				}
				catch (Exception e) {
					System.out.println("Error receiving " +
						"packet: " + e.getMessage());
					e.printStackTrace();
				}

				// Set an infinite socket timeout
				try {
					serverSocket.setSoTimeout(0);
				}
				catch (Exception e) {
					System.out.println("Error resetting " +
						"socket timeout");
				}
			}

			if (!mustFindValidAnswer) {
				// Send the final response to the client
				try {
					DatagramPacket responsePacket =
						new DatagramPacket(recvData,
						recvData.length,
						queryPacket.getAddress(),
						queryPacket.getPort());
					serverSocket.send(responsePacket);
				}
				catch (Exception e) {
					System.out.println("Error sending " +
						"final response");
				}
			}
			else {
				System.out.println("\nNo answer obtained " +
					"from servers or answer was from " +
					"cache");
			}
		}
	}

	// Print DNS packet info
	private static boolean printDNSPacket(byte[] data, ByteBuffer bb,
	  byte[] qData, ByteBuffer qbb) {
		// Flag to determine if the packet has a valid answer
		boolean hasValidAnswer = false;

		// Get the DNS header info
	        int qc = (int)bb.getShort(4) & 0x0FFFF;
        	int anc = (int)bb.getShort(6) & 0x0FFFF;
                int authc = (int)bb.getShort(8) & 0x0FFFF;
	        int addc = (int)bb.getShort(10) & 0x0FFFF;

		int currentIndex = 12;

		for (int i = 0; i < qc; i++) {
			currentIndex = skipName(data, bb, currentIndex);

                	currentIndex += 4;
		}

		// Print the answers from the DNS Response
		if (anc > 0) {
			System.out.print("\nAnswers:");
		}
		for (int i = 0; i < anc; i++) {
			currentIndex = skipName(data, bb, currentIndex);

        	        int type = (int)bb.getShort(currentIndex) & 0x0FFFF;
	                int qclass = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	currentIndex += 4;

			if (type == 1 && qclass == 1) {
				hasValidAnswer = true;
			}


        	        int dataLength = (int)bb.getShort(currentIndex + 4) &
				0x0FFFF;
                	currentIndex += 6;

			String addressString = "";
			if (type == 1) {
				for (int j = 0; j < dataLength; j++) {
					int address = (int)data[currentIndex] &
						0x0FF;
					addressString += address + ".";
					currentIndex++;
				}
				addressString = addressString.substring(0,
					addressString.length() - 1);
			}
			else if (type == 5) {
				int indexAfterAddress = skipName(data, bb,
					currentIndex);
				int startIndex = 12;
				int endIndex = skipName(qData, qbb, 12);
				byte[] qDataCopy = new byte[1024];
				System.arraycopy(qData, 0, qDataCopy, 0,
					qData.length);
				qbb.clear();
				int oldNameLength = endIndex - startIndex;
				int newNameLength = indexAfterAddress-currentIndex;
				int nameLengthDiff = newNameLength - oldNameLength;
				qbb.put(qDataCopy, 0, startIndex-0);
				qbb.put(data, currentIndex, newNameLength);
				if (nameLengthDiff < 0) {
					qbb.put(qDataCopy, endIndex,
						qDataCopy.length-endIndex);
				}
				else {
					qbb.put(qDataCopy, endIndex,
						qDataCopy.length-endIndex- 
							nameLengthDiff);
				}
				printDNSPacket(qData, qbb, qData, qbb);
				currentIndex = indexAfterAddress;
			}
			
			// IPs
			if (qclass == 1 && (type == 1 || type == 5)) {
				System.out.print("\n\t\tData:\n\t\t\t");
				System.out.print(addressString);
			}
		}

		// Print the authoritatice nameservers from the DNS
		// Response
		for (int i = 0; i < authc; i++) {
			currentIndex = skipName(data, bb, currentIndex);

                	currentIndex += 4;


                	currentIndex += 6;

			currentIndex = skipName(data, bb, currentIndex);

		}

                for (int i = 0; i < addc; i++) {
			currentIndex = skipName(data, bb, currentIndex);

        	        int type = (int)bb.getShort(currentIndex) & 0x0FFFF;
	                int qclass = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	currentIndex += 4;


                	int dataLength = (int)bb.getShort(currentIndex + 4) &
				0x0FFFF;
	                currentIndex += 6;

			String addRecord = "";
                	for (int j = 0; j < dataLength; j++) {
                        	int address = (int)data[currentIndex] &
				0x0FF;
	                       	addRecord += address + ".";
        	                currentIndex++;
			}

			if (qclass == 1 && type == 1) {
				addRecord = addRecord.substring(0,
					addRecord.length() - 1);

				if (!savedQueries.contains(addRecord) &&
				  !usedQueries.contains(addRecord)) {
					savedQueries.add(addRecord);
				}
			}
                }
		return hasValidAnswer;
	}

	private static int skipName(byte[] data, ByteBuffer bb,
	  int currentIndex){
                // Initialize numChars, isCompressing, and savedIndex
                int numChars = (int)data[currentIndex] & 0x0FF;
                boolean isCompressing = false;
                currentIndex++;
                int savedIndex = -1;

                while (numChars > 0) {
                        // If numChars >= 192, we should do data compression
                        if (numChars >= 192) {
                                // Calculate the offset
                                int offset = (int)bb.getShort(currentIndex - 1)
                                        & 0x3FFF;

                                // Save the currentIndex and change it to
                                // point to wherever the desired data is
                                if (!isCompressing) {
                                        savedIndex = currentIndex;
                                }
                                currentIndex = offset;

                                // Set isCompressing to true, get the real
                                // numChars to read next, and increment
                                // currentIndex
                                isCompressing = true;
                                numChars = (int)data[currentIndex] & 0x0FFFF;
                                currentIndex++;
                        }

                        // Loop continuously to get numChar chars added to the
                        // output string
                        for (int j = 0; j < numChars; j++) {
                                currentIndex++;
                        }

                        // Get the next numChars to read and increment
                        // currentIndex
                        numChars = data[currentIndex] & 0x0FFFF;
                        currentIndex++;
                }

                // Change currentIndex back after compression is done
                if (isCompressing) {
                        currentIndex = savedIndex + 1;
                }

                // Return the resulting string, including data compression
                return currentIndex;
	}


}
