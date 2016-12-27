import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.*;


public class proj2 {

	public static ArrayList<String> savedQueries = new ArrayList<String>();
	public static ArrayList<String> usedQueries = new ArrayList<String>();
	public static final ArrayList<String> ROOT_SERVERS =
		new ArrayList<String> (Arrays.asList("198.41.0.4",
			"192.228.79.201", "192.33.4.12", "199.7.91.13",
			"192.203.230.10", "192.5.5.241", "192.112.36.4",
			"128.63.2.53", "192.36.148.17", "192.58.128.30",
			"193.0.14.129", "199.7.83.42", "202.12.27.33"));
	public static HashMap<String, ArrayList<HashMap<String, Date>>> cache =
		new HashMap<String, ArrayList<HashMap<String, Date>>>();

	public static void main (String[] args) {
		int port = 9876;
		if (args.length == 1) {
			try {
				port = Integer.parseInt(args[0]);
			}
			catch (Exception e) {
				System.out.println("Error parsing port number");
				return;
			}
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
			System.out.println("\n\nORIGINAL QUERY:");
			printDNSPacket(queryData, qbb, queryData, qbb);

			// Sets RD flag tp 0
			short flags = qbb.getShort(2);
			flags &= ~(1<<8);
			qbb.putShort(2, flags);

			// Print the query to be sent to other servers
			System.out.println("\n\nQUERY FOR SERVERS:");
			printDNSPacket(queryData, qbb, queryData, qbb);

			// Instantiate variables to use to try to get a valid
			// response from a server
			usedQueries.clear();
			savedQueries.clear();
			savedQueries.addAll(ROOT_SERVERS);

			String query = getFirstDNSQueryString(queryData, qbb);
			boolean cacheHasAnswer =
				setUpSavedQueriesWithCache(query);

			if (cacheHasAnswer) {
				try {
					Date cachedDate = (new ArrayList<Date>(
						cache.get(query).get(0).
						values())).get(0);
					ByteArrayOutputStream b = new
						ByteArrayOutputStream();
					DataOutputStream d =
						new DataOutputStream(b);

					flags |= 0x8000;
					qbb.putShort(2, flags);
					qbb.putShort(6, (short)1);
					qbb.putShort(8, (short)0);
					qbb.putShort(10, (short)0);
					int index = skipName(queryData, qbb,
						12);
					index += 4;
					d.write(queryData, 0, index);
					d.write(192);
					d.write(12);
//					d.writeBytes(
//						getCompressedDNSResponseString(
//							qbb, queryData, 12));
					d.writeShort(1);
					d.writeShort(1);
					d.writeInt((int)(cachedDate.getTime() -
						(new Date()).getTime())/1000);

//					d.writeShort(2);
					String answer = savedQueries.get(
						savedQueries.size()-1);
					String[] answerParts = answer.split("\\.");
					d.writeShort(answerParts.length);
					for (int i = 0; i < answerParts.length;
					  i++) {
						d.writeByte(Integer.parseInt(
							answerParts[i]));
					}
					d.flush();

					byte[] sendData = b.toByteArray();
					DatagramPacket sendPacket =
						new DatagramPacket(
						sendData, sendData.length,
						queryPacket.getAddress(),
						queryPacket.getPort());
					serverSocket.send(sendPacket);
				}
				catch (Exception e) {
					System.out.println("Error sending " +
						"cached response");
				}
			}

			byte[] recvData = new byte[1024];
			ByteBuffer bb = ByteBuffer.wrap(recvData).
				order(ByteOrder.BIG_ENDIAN);
			boolean mustFindValidAnswer = true;
			while(!cacheHasAnswer && mustFindValidAnswer &&
			  savedQueries.size() > 0) {

				// Get the next IP address to query for a
				// valid answer	
				String nextIP = savedQueries.remove(
					savedQueries.size()-1);
				usedQueries.add(nextIP);
				System.out.println("Next IP: " +nextIP);
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
					System.out.println("\n\nRESPONSE:");
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

			printCache();
		}
	}

	// Print DNS packet info
	private static boolean printDNSPacket(byte[] data, ByteBuffer bb,
	  byte[] qData, ByteBuffer qbb) {
		// Flag to determine if the packet has a valid answer
		boolean hasValidAnswer = false;

		// Get the DNS header info
		int id = (int)bb.getShort(0) & 0x0FFFF;
                int flags = (int)bb.getShort(2) & 0x0FFFF;
	        int qc = (int)bb.getShort(4) & 0x0FFFF;
        	int anc = (int)bb.getShort(6) & 0x0FFFF;
                int authc = (int)bb.getShort(8) & 0x0FFFF;
	        int addc = (int)bb.getShort(10) & 0x0FFFF;

        	// Print the DNS header info
                System.out.println("ID: " + id);
		System.out.println("Flags: " +
        	      Integer.toBinaryString(0xFFFF & flags));
	        System.out.println("QCount: " + qc);
        	System.out.println("AnsCount: " + anc);
                System.out.println("AuthCount: " + authc);
	        System.out.println("AddCount: " + addc);

		int currentIndex = 12;

		// Print the queries
		System.out.println("Queries:");
		for (int i = 0; i < qc; i++) {
	                String responseString = "\t";
			responseString +=
				getCompressedDNSResponseString(bb, data,
					currentIndex);
			currentIndex = skipName(data, bb, currentIndex);

        	        int type = (int)bb.getShort(currentIndex) & 0x0FFFF;
	                int qclass = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	currentIndex += 4;
        	        responseString += " Type: " + type + " Class: " +
				qclass;
	                System.out.print(responseString);
		}

		// Print the answers from the DNS Response
		if (anc > 0) {
			System.out.print("\nAnswers:");
		}
		for (int i = 0; i < anc; i++) {
	                String responseString = "\n\t";
			String name = getCompressedDNSResponseString(bb, data,
                                        currentIndex);
			responseString += name;
			currentIndex = skipName(data, bb, currentIndex);

        	        int type = (int)bb.getShort(currentIndex) & 0x0FFFF;
	                int qclass = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	currentIndex += 4;

			if (type == 1 && qclass == 1) {
				hasValidAnswer = true;
			}

        	        responseString += " Type: " + type + " Class: " +
				qclass;
	                System.out.print(responseString);

			int time1 = (int)bb.getShort(currentIndex) & 0x0FFFF;
        	        int time2 = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	int time = time1 << 8;
	                time += time2;

        	        int dataLength = (int)bb.getShort(currentIndex + 4) &
				0x0FFFF;
                	currentIndex += 6;
	                System.out.print(" Time to Live: " + time +
				" seconds " + "Data Length: " + dataLength);

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
				addressString = getCompressedDNSResponseString(
					bb, data, currentIndex);
				int indexAfterAddress = skipName(data, bb,
					currentIndex);
				int startIndex = 12;
				int endIndex = skipName(qData, qbb, 12);
				byte[] qDataCopy = new byte[1024];
				System.arraycopy(qData, 0, qDataCopy, 0,
					qData.length);
				ByteBuffer qbbCopy = ByteBuffer.wrap(qDataCopy).
	                                order(ByteOrder.BIG_ENDIAN);
				qbb.clear();
				int oldNameLength = endIndex - startIndex;
				int newNameLength = indexAfterAddress-currentIndex;
				int nameLengthDiff = newNameLength - oldNameLength;
				System.out.println("Total Length: " + qData.length);
				System.out.println("Headers: " + startIndex);
				System.out.println("Old Name: " + oldNameLength);
				System.out.println("New Name: " + newNameLength);
				System.out.println("More Data1: " + (qData.length-endIndex));
				System.out.println("More Data2: " + (qData.length-endIndex-nameLengthDiff));
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
				System.out.println("CNAME");
				printDNSPacket(qData, qbb, qData, qbb);
				currentIndex = indexAfterAddress;
			}

			if (qclass == 1 && (type == 1 || type == 5)) {
				System.out.print("\n\t\tData:\n\t\t\t");
				System.out.print(addressString);

				addToCache(name, addressString, time);
			}
		}

		// Print the authoritatice nameservers from the DNS
		// Response
		if (authc > 0) {
			System.out.print("\nAuthoritative Nameservers:");
		}
		for (int i = 0; i < authc; i++) {
	                String responseString = "\n\t";
			String name = getCompressedDNSResponseString(bb, data,
                                        currentIndex);
			responseString += name;
			currentIndex = skipName(data, bb, currentIndex);

        	        int type = (int)bb.getShort(currentIndex) & 0x0FFFF;
	                int qclass = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	currentIndex += 4;

        	        responseString += " Type: " + type + " Class: " +
				qclass;
	                System.out.print(responseString);

			int time1 = (int)bb.getShort(currentIndex) & 0x0FFFF;
        	        int time2 = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
	                int time = time1 << 8;
        	        time += time2;

                	int dataLength = (int)bb.getShort(currentIndex + 4) &
				0x0FFFF;
                	currentIndex += 6;
	                System.out.print(" Time to Live: " + time +
				" seconds " + "Data Length: " + dataLength);

			String authServer =
				getCompressedDNSResponseString(bb,
				data, currentIndex);
			currentIndex = skipName(data, bb, currentIndex);

			if (qclass == 1 && type == 2) {
				System.out.print("\n\t\tData:\n\t\t\t" +
					authServer);
				System.out.println("");

				addToCache(name, authServer, time);
			}
		}

		// Print any additional records from the DNS Response
		if (addc > 0) {
	                System.out.print("Additional Records:");
		}
                for (int i = 0; i < addc; i++) {
	                String responseString = "\n\t";
			String name = getCompressedDNSResponseString(bb, data,
                                        currentIndex);
			responseString += name;
			currentIndex = skipName(data, bb, currentIndex);

        	        int type = (int)bb.getShort(currentIndex) & 0x0FFFF;
	                int qclass = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
                	currentIndex += 4;

        	        responseString += " Type: " + type + " Class: " +
				qclass;
	                System.out.print(responseString);

			int time1 = (int)bb.getShort(currentIndex) & 0x0FFFF;
        	        int time2 = (int)bb.getShort(currentIndex + 2) &
				0x0FFFF;
	                int time = time1 << 8;
        	        time += time2;

                	int dataLength = (int)bb.getShort(currentIndex + 4) &
				0x0FFFF;
	                currentIndex += 6;
        	        System.out.print(" Time to Live: " + time +
				" seconds " + "Data Length: " + dataLength);

			String addRecord = "";
                	for (int j = 0; j < dataLength; j++) {
                        	int address = (int)data[currentIndex] &
				0x0FF;
	                       	addRecord += address + ".";
        	                currentIndex++;
			}

			if (qclass == 1 && type == 1) {
        	                System.out.print("\n\t\tData:\n\t\t\t");
				addRecord = addRecord.substring(0,
					addRecord.length() - 1);
				System.out.print(addRecord);

				addToCache(name, addRecord, time);
				if (!savedQueries.contains(addRecord) &&
				  !usedQueries.contains(addRecord)) {
					savedQueries.add(addRecord);
				}
			}
                }
		System.out.println("");
		return hasValidAnswer;
	}

	// Add a record to the cache
	private static void addToCache(String name, String record, int time) {
		Date now = new Date();
		Date ttlDate = new Date(now.getTime() + (time * 1000));
		HashMap<String, Date> ttl = new HashMap<String, Date>();
		ttl.put(record, ttlDate);

		if (cache.containsKey(name)) {
			ArrayList<HashMap<String, Date>> cacheArray =
				cache.get(name);
			for (int i = 0; i < cacheArray.size(); i++) {
				if (cacheArray.get(i).containsKey(record)) {
					cacheArray.remove(i);
				}
			}
			cache.get(name).add(ttl);
		}
		else {
			ArrayList<HashMap<String, Date>> cacheArray=
				new ArrayList<HashMap<String, Date>>();
			cacheArray.add(ttl);
			cache.put(name, cacheArray);
		}
	}

	// Print the contents of the cache
	public static void printCache() {
		System.out.println("\n" + cache.toString());
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

	// Print DNS Response strings and handle data compression
        private static String getCompressedDNSResponseString(ByteBuffer bb,
          byte[] data, int currentIndex) {
                String responseString = "";

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
                                responseString += (char)data[currentIndex];
                                currentIndex++;
                        }
                        responseString += ".";

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
                if (responseString.length() > 0) {
			responseString = responseString.substring(0,
				responseString.length()-1);
		}
                return responseString;
        }

	// Gets the first DNS query string
	private static String getFirstDNSQueryString(byte[] data,
	  ByteBuffer bb) {
		return getCompressedDNSResponseString(bb, data, 12);
	}

	// Appends checks the cache for useful data. If found, it is added to
	// savedQueries
	private static boolean setUpSavedQueriesWithCache(String query) {
		boolean cacheHasAnswer = false;

		String[] queryParts = query.split("\\.");
		for (int i = 0; i < queryParts.length; i++) {
			System.out.println("QUERYPART " + i + ": " + queryParts[i]);
		}
		for (int i = 1; i <= queryParts.length; i++) {
			System.out.println("i: " + i);
			String queryPart = "";
			for (int j = 0; j < i; j++) {
				System.out.println("j: " + j);
				queryPart = queryParts[queryParts.length-j-1] +
					"." + queryPart;
			}

			if (queryPart.length() > 0) {
				queryPart = queryPart.substring(0,
					queryPart.length() - 1);
				System.out.println("\n\nLOOKING FOR CACHED VALUES: " + queryPart);

				if (cache.containsKey(queryPart)) {
					boolean ipAdded =
						addIPsToSavedQueries(queryPart);
					if (ipAdded && i == queryParts.length) {
						cacheHasAnswer = true;
					}
				}
			}
		}
		return cacheHasAnswer;
	}

	// Adds IPs to savedQueries based on cache
	private static boolean addIPsToSavedQueries(String query) {
		boolean ipsAdded = false;

		ArrayList<HashMap<String, Date>> cachedInfo = cache.get(query);
		for (int i = 0; cachedInfo != null && i < cachedInfo.size();
		  i++) {
			Date cachedDate = (new ArrayList<Date>(cachedInfo.
				get(i).values())).get(0);
			if (cachedDate.before(new Date())) {
				System.out.println("TTL EXPIRED");
				cache.get(query).remove(i);
			}
			else {
				String cachedValue = (new ArrayList<String>(
					cachedInfo.get(i).keySet()))
					.get(0);
				if (Character.isDigit(cachedValue.charAt(0)) &&
				  !savedQueries.contains(cachedValue) &&
				  !usedQueries.contains(cachedValue)) {
					System.out.println("ADDING " + cachedValue);
					savedQueries.add(cachedValue);
					ipsAdded = true;
				}
				else {
					System.out.println("Searching for value: " + cachedValue);
					addIPsToSavedQueries(cachedValue);
				}
			}
		}
		return ipsAdded;
	}
}
