MSRPSender.cxx */
#include<libmnetutil/TCPSocket.h> 

#include<libmnetutil/NetworkException.h> //ConnectFailed && HostNotFound
#include<libmutil/trim.h>  //removes whitespace from start and end of strings

#include <iostream>
#include <fstream>

#include"../../subsystem_media/msrp/MSRPSender.h"
#include"../../subsystem_media/msrp/MSRPMessage.h"

using namespace std;

void MSRPSender::run(){
	cerr <<"MAFE: MSRPSender::run() started"<<endl;
	
	MRef<MSRPSender*> msrpSender;
	//= new MSRPSender*(const string serverAdd, int32_t serverPort, const string filename);
	//char new_chunk[] = "";	
	//ifstream myfile;
	MRef<MSRPMessageSend*> M_Send;

	/*if (argc!=4){
		return 1;
	}
	else{*/

//-------if (no TCPSocket open between offerer and answerer) { --> verify with the URI received by SIP messages.

	TCPSocket *sock;
	try{
		//Create a TCP connection to the server (address and port given
		//as arguments to the application).
		sock = new TCPSocket(msrpSender->serverAdd, msrpSender->serverPort);
	}catch(ConnectFailed &){
		cerr << "Sorry, I could not connect to port "<< msrpSender.serverPort << " on the server." << endl;
		return -3;
	
	}catch(HostNotFound &){
		cerr << "Sorry, the server <"<< msrpSender.serverAdd<<"> could not be found"<< endl;
		return -3;
	}
//-----	} else;

	myfile.open(filetosend,ios::in|ios::binary|ios::ate);
	if (myfile.is_open()) 
	{
		while( !myfile.eof() )
		{
			while(M_Send.BytesLeft != 0)
			{
				 M_Send.CreateMSRPSend(setchunk.set_chunk, sizeof(setchunk.set_chunk));

					// ToDo Receive_Report;
				 }
				else;

				// error detect. with Report -->cerr << "Sorry, chunk not received correctly, trying again" << endl;

			}
		}
	}
	else
		cerr << "Unable to open file" << endl;
		
	sock->close();
}
