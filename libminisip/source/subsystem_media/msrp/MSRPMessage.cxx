
//	#ifndef MSRPMessage_h
//	#define MSRPMessage_h

#include<libmutil/minilist.h>
#include<libmutil/MemObject.h>
#include<libmnetutil/Socket.h>
#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/NetworkException.h>
#include<libmutil/CommandString.h>
#include<libmutil/Thread.h>

#include<list>
#include<fstream>
#include<iostream>
#include<string>

#include<sys/types.h>
#include<netinet/in.h>

#include"../../subsystem_media/msrp/MSRPMessage.h"

#define SERVER_PORT 3333

using namespace std;

char chunk[1024];
int chunkSize;
char * byterange = "";

string first;
string second;
string last;

/*class MSRPMessage : public MObject{

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

		MSRPHeader(char *name, char *value){
			attrib = name;
			val = value;
		}

		string getString(){
			return (attrib+val);
		}
};

class MSRPMessageSend : public MSRPMessage{

	public:
		MSRPMessageSend(char * chunk, int chunklenght);
		//MSRPMessageSend(TCPSocket * sock);
		char * CreateMSRPSend(int &outlength);
		string UpdateByteRange();
		bool PutLastLine();
};

class MSRPMessageReport : public MSRPMessage{

	public:
		MSRPMessageReport(char * chunk, int chunklenght);
		MSRPMessageReport(TCPSocket * sock);
		string CreateMSRPReport();
};

class MSRPSender : public MObject{
	
	public:
		MSRPSender(int argc, char **argv);
		void setchunk();
	
		int BytesSet;
		//int BytesLeft;
		//int chunkSize;
		int BytesIndex;
		int BytesTotal;
		
		ifstream myfile;
		string string_msg;

		char * getStrMsg ( int outlenght);
};
*/

MSRPSender::MSRPSender(const string Addr, int32_t Port, const string name, MRef<SipStack*>sipStack, const string callId){
	sAddr = Addr;
	sPort = Port;
	fName = name;
	sipstack = sipStack;
	callid = callId;
}

void MSRPSender::run(){
	cerr <<"MAFE: MSRPSender::run() started"<<endl;

	string from_server;

	int num_chunks;
	TCPSocket *sock=NULL;
	char report_rec[1000];
	char * msg;
	int pktLen=0;
	//ifstream myfile;
		
	cerr <<"MAFE: creating socket, sAddr=<"<<sAddr<<"> and sPort=" << sPort << endl;
	 try{	
		sock = new TCPSocket(sAddr, sPort);
		cerr << "MY IP ADDRESS IS "<< sock->getLocalAddress()->getString()<<endl;
		
	}catch(ConnectFailed &){
	       	cerr << "Sorry, I could not connect to port "<< sPort << " on the server." << endl;
		return;
	
	}catch(HostNotFound &){
		cerr << "Sorry, the server <"<< sAddr<<"> could not be found"<< endl;
		return;
	}
		
	//cerr <<"MAFE: opening file..."<<endl;
	myfile.open(fName.c_str(), ios::in|ios::binary|ios::ate);
	if (myfile.is_open()){

		cerr << "file open correctly" << endl;

		//get size of file
		myfile.seekg(0, ifstream::end);
		int filesize = myfile.tellg();
		myfile.seekg(0);

		num_chunks = filesize/1024;

		cerr <<"Filesize is "<<filesize<<endl;
		//cerr <<num_chunks<<endl;

		string brange;

		if(num_chunks == 0){
			brange = "1-"+itoa(filesize)+"/"+itoa(filesize);
			byterange=(char*)brange.c_str();
		}
		else{
			brange = "1-1024/"+itoa(filesize);
			byterange=(char*) brange.c_str();
		}
		//cerr<<"Byte range is "<<byterange<<endl;

		if(filesize%1024!=0)
			num_chunks=num_chunks+1;

		while(num_chunks !=0){

			num_chunks--;
			setchunk();

			//cerr<<"number of chunks "<<num_chunks<<endl;

			MSRPMessageSend new_msg(chunk, chunkSize);
			//cerr<<"MAFE:  created new object... "<<endl;
			msg = new_msg.CreateMSRPSend(pktLen);

			//int aux = htonl(chunkSize);
			if (sock==NULL)
				cerr<<"MAFE: socket is NULL"<<endl;
			//int sendtam=sock->write((char*)&aux,sizeof(chunkSize));

			assert(sock);

			int nsent=sock->write(msg, pktLen);

			//int reprec = sock->read(report_rec, 1024);

			//report_rec[reprec]=0;
			//from_server=trim(string(report_rec));

			//cerr<<"receiving "<<reprec<< " bytes"<<endl;
			//cerr<<report_rec<<endl;

			//string byterange = new_msg.UpdateByteRange();
		}
		
	}
	CommandString cmd(callid, "MSRP_DONE");
	sipstack->handleCommand(cmd);
}
			
void MSRPSender::setchunk(){
	
//	ifstream myfile;

	chunkSize = myfile.readsome(chunk, 1024);
	//BytesSet += chunkSize;
	//BytesIndex += BytesSet;
	cerr << "Read " << chunkSize <<" bytes" << endl; 
}

MSRPReceiver::MSRPReceiver(const string fileName, int fileSize){
	name = fileName;
	sizercv = fileSize;
}

void MSRPReceiver::run(){

	cerr <<"MAFE: MSRPReceiver::run() started on port "<< SERVER_PORT << endl;
	//cerr<<"receiver before ssock"<<endl;

	MRef<ServerSocket*> ssock = ServerSocket::create(SERVER_PORT);
	//cerr << "MY IP ADDRESS IS "<< ssock->getLocalAddress()->getString()<<endl;

	//cerr<<"receiver after ssock"<<endl;
	
	string from_client;
	string new_chunk;
	
	string string_rep;
	
	char * loc;
	char * loc2;
	int j;
	int lng;
	int lngchunk;

	ofstream SaveFile(name.c_str(), ios::trunc);  //get filename from SDP msg

	cerr <<"waiting for client"<<endl;
	MRef<StreamSocket*> client_sock = ssock->accept();
	cerr <<"client connected"<<endl;
	
	cerr<<"size of file"<<sizercv<<endl;

	int aux;
	char receive_buf [10000];
	char chunk_buffer_prov[1024];
	int i;
	//int rectam = client_sock->read((char*)&aux,sizeof(int));
	//lng = ntohl(aux);

	int chunkRcv = sizercv/1024;
	if( (aux=sizercv%1024) !=0 )
		chunkRcv=chunkRcv+1;

	do{
		//if(chunkRcv>0){

			int nrecv = client_sock->read(receive_buf,10000);
		
			loc = strstr(receive_buf,"Content-Type: ");
			
			//cerr<<"localizador del content type: "<<loc<<endl;

			while(*loc != '\n'){
				loc++;
			}
			if(chunkRcv>1){
			for(j=0; j!=1024; j++){
				chunk_buffer_prov[j]=*loc;
				loc++;
			}	
			//cerr<<"buffer provisional "<<chunk_buffer_prov<<endl;
			SaveFile.write(chunk_buffer_prov,1024);
			}
			else{
				for(j=0; j!=aux; j++){
				chunk_buffer_prov[j]=*loc;
				loc++;
			}	
			//cerr<<"buffer provisional "<<chunk_buffer_prov<<endl;
			SaveFile.write(chunk_buffer_prov,aux);
			}
	
			chunkRcv--;
		//}else;
	}while(chunkRcv>0);
}

MSRPMessageSend::MSRPMessageSend(char * data, int len){

	//mChunk = data;
	//mChunkLength = len;
}

char* MSRPMessageSend::CreateMSRPSend(int& bufferSize){

	list<MSRPHeader> hsend;
	
	char *nametemp;
	char *valuetemp;
	MSRPHeader temp(nametemp,valuetemp);
	
	string transc_id ="num_transc";
	string first_line = "MSRP "+transc_id+" SEND\n";
	string last_line= "";
	string hdr = "";
	string newbyterange;

	MSRPMessageSend m(chunk,chunkSize);
	
	bool several_chunks=m.PutLastLine();
	
	if(several_chunks == true){
		
		last_line = "-------"+transc_id+"+";
		newbyterange =m.UpdateByteRange();
	}
	else{	
		last_line = "-------"+transc_id+"$";
		newbyterange = byterange;
	}
	
	hsend.push_back(MSRPHeader("To-Path: ","ToUri"));
	hsend.push_back(MSRPHeader("From-Path: ","FromUri"));
	hsend.push_back(MSRPHeader("Message-ID: ","messageID"));
	hsend.push_back(MSRPHeader("Byte-Range: ",(char*)newbyterange.c_str()));
	hsend.push_back(MSRPHeader("Failure-Report: ","failrep"));
	hsend.push_back(MSRPHeader("Success-Report: ","succrep"));
	hsend.push_back(MSRPHeader("Content-Type: ","contenttype"));

	list<MSRPHeader>::iterator h = hsend.begin();

	while(h != hsend.end()){
		temp = *h;
		hdr = hdr + temp.getString();
		hdr = hdr + "\n";
		h++;
	}
	hdr += "\n";

	//cerr<<newbyterange<<endl;

	//string all = first_line + hdr + chunk + last_line;
	
	string first= first_line+hdr;
	
	cerr<<"MAFE: allocating "<<first.size()+chunkSize+last_line.size()<<" bytes"<<endl;
	char *pkt = (char*)malloc(first.size()+chunkSize +last_line.size());
	
	cerr<<"MAFE: memcpy1"<<endl;
	memcpy(pkt, first.c_str(), first.size());
	pkt = pkt+first.size();
	cerr<<"MAFE: memcpy2"<<endl;
	memcpy(pkt, chunk, chunkSize);
	pkt = pkt+chunkSize;
	cerr<<"MAFE: memcpy3"<<endl;
	memcpy(pkt, last_line.c_str(), last_line.size());
	cerr<<"MAFE: memcpy3 done"<<endl;
	pkt = pkt - (chunkSize + first.size());

	bufferSize=first.size()+chunkSize+last_line.size();
	//cerr<<"MsrpMessage is: "<<pkt<<endl;

	return pkt;
}

bool MSRPMessageSend::PutLastLine(){

	bool sev_chunk_bool;
	int i=0;
	int sec;
	first = "";
	second = "";
	last = "";
	
	//cerr<<byterange<<endl;
	
	string rangehere;
	rangehere = byterange;

	do{
		first = first + byterange[i];
		i++;
	}while(byterange[i] != '-');
	i++;

	for(byterange[i] != '-'; byterange[i] != '/'; i++){
		second = second + byterange[i];
	}
	i++;

	for(byterange[i] != '/'; i<rangehere.length(); i++){
		last = last + byterange[i];
	}

	if(second == last)
		sev_chunk_bool = false;
	else
		sev_chunk_bool = true;
	
	//cerr<<sev_chunk_bool<<endl;
	return(sev_chunk_bool);
}

string MSRPMessageSend::UpdateByteRange(){

	string updbyterange;
	int one;
	int two;
	
	MSRPMessageSend m(chunk,chunkSize);
		
	one = atoi(first.c_str());
	two = atoi(second.c_str());
	
	one = two + 1;
	two = one + 1024;
	
	string strone = itoa(one);
	string strtwo = itoa(two);
	
	updbyterange = strone+"-"+strtwo+"/"+last;
		
	//cerr<<updbyterange<<endl;
	return(updbyterange);
}

MSRPMessageReport::MSRPMessageReport(char * chunk, int chunklenght){}

string MSRPMessageReport::CreateMSRPReport(){

	list<MSRPHeader> hreport;
	
	string transc_id = "numID";

	string first_line = "MSRP "+transc_id+" REPORT\n";
	string last_line = "-------"+transc_id+"$";

	string hdrep;

	hreport.push_back(MSRPHeader("To-Path: ","ToUri"));
	hreport.push_back(MSRPHeader("From-Path: ","FromUri"));
	hreport.push_back(MSRPHeader("Message-ID: ","messageID"));
	hreport.push_back(MSRPHeader("Byte-Range: ","byterange"));
	hreport.push_back(MSRPHeader("Status: ","status"));

	list<MSRPHeader>::iterator hr = hreport.begin();
		hdrep = hdrep + (*hr).getString();
		hdrep = hdrep + "\n";
		hdrep = hdrep + (*hr).getString();
		hdrep = hdrep + "\n";
		hr++;

	string all = first_line + hdrep + last_line;
	return(all);
}

