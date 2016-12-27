import java.io.*;
import java.net.*;
import java.util.*;

public class Client {
	public static void main(String[] args) throws Exception {
		BufferedReader inFromUser = new BufferedReader(new InputStreamReader(System.in));
		DatagramSocket clientSocket = new DatagramSocket();
		
		System.out.print("Enter a File to transfer: ");
		String filename = inFromUser.readLine();
		byte[] sendData = filename.getBytes();

		byte[] serverAddress = {10, 0, 0, 1};
		InetAddress address = InetAddress.getByAddress(serverAddress);
		DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, address, 9876);
		clientSocket.send(sendPacket);
		
		boolean finished = false;
		FileOutputStream outToFile = new FileOutputStream(filename + ".out");
		byte total = 5;
		while (!finished) {
			ArrayList<Frame> window = new ArrayList<Frame>();
			for (byte num = 0; num < total; num++) {
				byte[] recData = new byte[1024];
				DatagramPacket recPacket = new DatagramPacket(recData, recData.length);
				clientSocket.receive(recPacket);
				Frame frame = new Frame(recData);
				window.add(frame);
				System.out.println("\nFrame: " + num);
				System.out.println("Length: " + frame.size);
			}
			// Before this loop, packets need to be correct and in order
			for (int i = 0; i < window.size(); i++) {
				Frame frame = window.get(i);
				outToFile.write(frame.data);
				finished = (frame.size == 0);
			}
		}
		outToFile.close();
	}
}
