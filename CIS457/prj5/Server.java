import java.io.*;
import java.net.*;
import java.util.*;

public class Server {
	public static void main(String[] args) throws Exception {
		DatagramSocket serverSocket = null;
		try {
			serverSocket = new DatagramSocket(9876);
		} catch (Exception e) {
			System.out.println("Cannot connect");
			System.exit(1);
		}

		byte[] recData = new byte[1024];
		while (true) {
			System.out.println("Connected to client. Waiting file to transfer.");
			DatagramPacket recPacket = new DatagramPacket(recData, recData.length);
			serverSocket.receive(recPacket);
			InetAddress clientIP = recPacket.getAddress();
			int clientPort = recPacket.getPort();
			String filename = (new String(recPacket.getData())).trim();
			System.out.println("Filename: " + filename);
			
			boolean finished = false;	
			FileInputStream inFromFile = new FileInputStream(filename);
			byte total = 5;
			while (!finished) {
				ArrayList<Frame> window = new ArrayList<Frame>();
				for (byte num = 0; num < total; num++) {
					byte[] fileData = new byte[1024 - Frame.hlen];
					int size = inFromFile.read(fileData);
					Frame frame = new Frame(num, fileData, size);
					window.add(frame);
					finished = (frame.size == 0);
				}
				for (int i = 0; i < window.size(); i++) {
					Frame frame = window.get(i);
					DatagramPacket sendPacket = new DatagramPacket(frame.buf, frame.buf.length, clientIP, clientPort);
					serverSocket.send(sendPacket);
					System.out.println("\nFrame: " + i);
					System.out.println("Frame Size: " + frame.size);
				}
			}
			inFromFile.close();
		}		
	}
}
