import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.*;
import java.nio.file.*;

/**********************************************************************************
* CIS 457
* Project 2
* 
* This class is a helper class that extracts info from Datagram Packets
*
* @author Joel Truman
* @version Fall 2015
**********************************************************************************/

class Cache {
	
	static HashMap<String, String> cache;
	static ArrayList<String> keys;
	static PrintWriter out;
	public Cache(){
		cache = new HashMap<String, String>();
		keys = new ArrayList<String>();
		try{
			out = new PrintWriter(new BufferedWriter
				(new FileWriter("cache.txt", true)));
		}catch(Exception e){
		}
		recoverCache();
	}
	
	public void addToCache(String name, String answer){
		keys.add(name);
		cache.put(name,answer);
	}

	public static String searchCache(String name){
		if(cache.get(name) != null){
			return cache.get(name);
		}
		
		return null;
	}
	
	public static void recoverCache(){
		try{
			for(String s : Files.readAllLines(Paths.get("cache.txt"))){	
				for(String line : s.trim().split("\\r?\\n")){
					String[] parts = line.trim().split("\\s");
					String name = parts[0];
					String answer = parts[1];
					
					keys.add(name);
					cache.put(name, answer);
				}
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	
	}
	
	public static boolean inCache(String name){
		for(int i = 0; i < keys.size(); i++){
			if(name.equals(keys.get(i)))
				return true;
		}
		return false;
	}
			
	public static void storeCache(){
		BufferedWriter writer = null;
		try{
			Scanner sc = new Scanner("cache.txt");
			while(sc.hasNextLine()){
				String s = sc.nextLine();
				if (!s.equals("cache.txt"))
					out.println(s);
			}
			for(int i = 0; i < keys.size(); ++i){
				String name = keys.get(i);
				System.out.println(name);
				String ip = cache.get(name);
				out.println(name + "\t" + ip);
			}
			out.close();
		}
		catch (Exception e){
			
		}
	}
	
}
