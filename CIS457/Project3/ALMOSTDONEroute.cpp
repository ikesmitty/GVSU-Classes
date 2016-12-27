#include <iostream>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <cstring>
#include <cstdio>
using namespace std;

#define ARP_P 2054
#define IP_P 2048
#define ICMP_P 1
#define ECHO_REQ 8

void readMAC (char buf[1500], char mac[6], int idx);
void writeMAC(char buf[1500], char mac[6], int idx);
void readIP  (char buf[1500], char ip[4],  int idx);
void writeIP (char buf[1500], char ip[4],  int idx);

// R1 MAC c6:51:4a:93:ae:87
char rmac[6] = {(char) 198, 81, 74,
		(char) 147, (char) 174, (char) 135};
// R1 IP 10.1.0.1
char rip[4] = {10, 1, 0, 1};

char smac[6];
char sip[4];

int main() {
    int packet_socket;
    // get list of interfaces
    struct ifaddrs *ifaddr, *tmp;
    if (getifaddrs(&ifaddr) == -1) {
        cerr << "getifaddrs";
        return 1;
    }
    
    // loop over the list
    for (tmp = ifaddr; tmp != NULL; tmp=tmp->ifa_next) {
        if(tmp->ifa_addr->sa_family == AF_PACKET) {
            cout << "Interface: " << tmp->ifa_name << endl;
            // create a packet socket on interface r?=eth1
            if (!strncmp(&(tmp->ifa_name[3]), "eth1", 4)) {
                cout << "Creating Socket on interface " << tmp->ifa_name << endl;
                // Create a packet socket
                // AF_PACKET makesit a packet socket
                // SOCK_RAW makes it so we get the entire packet
                // ETH_P_ALL indicates we want all protocols
                packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
                if (packet_socket < 0) {
                    cerr << "socket";
                    return 2;
                }
                // Bind the socket to the address
                if (bind(packet_socket, tmp->ifa_addr, 
                    sizeof(struct sockaddr_ll)) == -1) {
                    cerr << "bind";
                }
            }
        }
    }
    // free the interface list when we don't need it anymore
    freeifaddrs(ifaddr);
    
    // loop and receive packets
    cout << "Ready to receive now " << endl;
    while (1) {
        char buf[1500];
        struct sockaddr_ll recvaddr;
        int recvaddrlen = sizeof(struct sockaddr_ll);
        int n = recvfrom(packet_socket, buf, 1500, 0,(struct sockaddr*) &recvaddr,(socklen_t*) &recvaddrlen);
        if (recvaddr.sll_pkttype == PACKET_OUTGOING)
            continue;
        cout << "Got a " << n << " byte packet" << endl;
	
	int protocol = 0;
	protocol += ((unsigned int) buf[12]) * 256;
	protocol += (unsigned int) buf[13];
	
	switch (protocol) {
	  case ARP_P:
	    cout << "ARP" << endl;
	    // Read source MAC address
	    readMAC(buf, smac, 22);
	    // Read source IP address
	    readMAC(buf, sip, 28);
	    // Update Ethernet header destination
	    writeMAC(buf, smac, 0);
	    // Update Ethernet header source
	    writeMAC(buf, rmac, 6);
	    // Set operation for reply
	    buf[21] = 2;
	    // Write router MAC address
	    writeMAC(buf, rmac, 22);
	    // Write router IP address
	    writeIP(buf, rip, 28);
	    // Write sender MAC address
	    writeMAC(buf, smac, 32);
	    // Write sender IP address
	    writeIP(buf, sip, 38);
	    send(packet_socket, buf, n, 0);
	    break;
	  case IP_P:
	    // Read source MAC address
	    readMAC(buf, smac, 6);
	    // Read source IP address
	    readMAC(buf, sip, 26);
	    // Determine IP Protocol
	    switch ((unsigned int) buf[23]) {
	    case ICMP_P:
	      // Determine ICMP Type
	      cout << "ICMP Type " << (unsigned int) buf[34] << endl;
	      switch ((unsigned int) buf[34]) {
	      case ECHO_REQ:
		cout << "Echo request" << endl;
		// Set Type for echo reply
		buf[34] = 0;
		// Update Ethernet header destination
		writeMAC(buf, smac, 0);
		// Update Ethernet header source
		writeMAC(buf, rmac, 6);
		// Update IP header source
		writeIP(buf, rip, 26);
		// Update IP header destination
		writeIP(buf, sip, 30);
		send(packet_socket, buf, n, 0);
		break;
	      }
	      break;
	    }
	    break;
	}
    }
}

void readMAC(char buf[1500], char mac[6], int idx) {
  for (int i = idx; i < idx+6; i++)
    mac[i-idx] = buf[i];
}

void writeMAC(char buf[1500], char mac[6], int idx) {
  for (int i = idx; i < idx+6; i++)
    buf[i] = mac[i-idx];
}

void readIP(char buf[1500], char ip[4], int idx) {
  for (int i = idx; i < idx+4; i++)
    ip[i-idx] = buf[i];
}

void writeIP(char buf[1500], char ip[4], int idx) {
  for (int i = idx; i < idx+4; i++)
    buf[i] = ip[i-idx];
}
