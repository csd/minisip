// MSRPReceiver.cxx

#include<libmnetutil/ServerSocket.h> 
#include<libmutil/stringutils.h>  //removes whitespace from start and end of strings

#include"../../subsystem_media/msrp/MSRPReceiver.h"

#define SERVER_PORT 3333   //This defines the port number that the server will open

using namespace std;

void MSRPReceiver::run(){
#ifdef DEBUG_OUTPUT
	setThreadName("MSRPReceiver::run");
#endif
	//Creating a server socket
	
	//MRef<IP4ServerSocket*> ssock = new IP4ServerSocket(SERVER_PORT);
	MRef<ServerSocket*> ssock = ServerSocket::create(SERVER_PORT);;

	//string from_client;
	//char transc_id_recv[8];
	int i=0;
	int j;
	//char first_line_recv[];
	//char last_line_recv[];

	//list<MSRPHeader> header_recvs;
	
	//MSRPMessage M_get;
	
	//MSRPMessageReport M_Report;

	MRef<StreamSocket *> client_sock = ssock->accept();
		
	do{
		//Receive a chunk of data from client
	
		client_sock.read(first_line_recv, 18);
		
		if(first_line_recv[14] == "S" && first_line_recv[15] == "E" && first_line_recv[16] == "N" && first_line_recv[17] == "D"){
		
			for( i=5; i<13, i++ ){
				transc_id_recv[j]==first_line_recv[i];
			}

		}
		
		for( i=0; i<8; i++)
		client_sock.read(header_recvs[i], sizeof(header_recvs));

		M_get.getchunk(client_sock);
		
		M_Report.CreateMSRPReport(M_get.received_chunk, sizeof(M_get.received_chunk));

		client_sock.read(last_line_recv, 16);

	}while(last_line_recv[15]=="+");

}
