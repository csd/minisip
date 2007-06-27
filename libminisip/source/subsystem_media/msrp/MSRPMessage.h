
#ifndef MSRPMessage_H
#define MSRPMessage_H

#include<libmutil/minilist.h>
#include<libmutil/MemObject.h>
#include<libmnetutil/Socket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/NetworkException.h>
#include<libmutil/CommandString.h>
#include<libmutil/Thread.h>

#include<libmsip/SipStack.h>

#include<list>
#include<fstream>
#include<iostream>
#include<string>

#define SERVER_PORT 3333

using namespace std;

class MSRPMessage : public MObject{

	public:
		//MSRPMessage(TCPSocket *sock);
		//MSRPMessage(char * buf, int buflenght);
		
		//virtual std::string getstring() const = 0;
		
		//char * chunk;
		int lenght;
};

class MSRPHeader : public MObject{

	public:
		string attrib;
		string val;
		
		MSRPHeader(char* name, char* value){
			attrib = name;
			val = value;
		}

		string getString(){
			return (attrib+val);
		}
};

class MSRPMessageSend : public MSRPMessage{

	public:
		MSRPMessageSend(char * chunk, int chunklength);
		
		//virtual ~MSRPMessageSend();

		//MSRPMessageSend(TCPSocket * sock);
		char * CreateMSRPSend(int &outlength);
		//string CreateMSRPSend();
		string UpdateByteRange();
		bool PutLastLine();
/*	
	private:
		char * mChunk;
		int mChunkLength;*/
};

class MSRPMessageReport : public MSRPMessage{

	public:
		MSRPMessageReport(char * chunk, int chunklenght);
		MSRPMessageReport(TCPSocket * sock);
		string CreateMSRPReport();

		//string getString(){
		//	return "test";
		//}
};

class MSRPSender : public Runnable{
	
	public:
		MSRPSender(const std::string serverAddr, int32_t serverPort, const std::string nameOfFile, MRef<SipStack*> stack, const std::string callId);
					
		//virtual ~MSRPSender();

		void run();
		void setchunk();

		int BytesSet;
		int BytesLeft;
		int BytesIndex;
		int BytesTotal;
		
		ifstream myfile;
		string string_msg;
		string filetosend;		
		
	private:
		//char chunk[8];

		string sAddr;
		string fName;
		int32_t sPort;
		MRef<SipStack*> sipstack;
		string callid;
};

class MSRPReceiver : public Runnable{

	public:
		MSRPReceiver(const std::string filename, int file_size);
		void run();	
	private:
		char chunk[1024];
		string name;
		int sizercv;
};

#endif
