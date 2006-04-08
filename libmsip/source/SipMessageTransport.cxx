/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include<config.h>

#include<libmsip/SipMessageTransport.h>

#include<errno.h>
#include<stdio.h>

#ifdef WIN32
#include<winsock2.h>
//#include<io.h>
#endif

#include<libmsip/SipResponse.h>
#include<libmsip/SipRequest.h>
#include<libmsip/SipException.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderRoute.h>

#include<libmnetutil/IP4Address.h>
#include<libmnetutil/IP4ServerSocket.h>
#include<libmnetutil/TLSServerSocket.h>
#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/NetworkException.h>
#include<libmutil/Timestamp.h>
#include<libmutil/MemObject.h>
#include<libmutil/mtime.h>
#include<libmutil/dbg.h>
//#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>

#include<cctype>
#include<string>
#include<algorithm>

using namespace std;

#define TIMEOUT 600000
#define NB_THREADS 5
#define BUFFER_UNIT 1024

#if !defined(_MSC_VER) && !defined(__MINGW32__)
# define ENABLE_TS
#endif

#ifdef _MSC_VER
static int nocaseequal(char c1, char c2){
	if ( ((c1>='A') && (c1<='Z')) ){
		return (c1==c2) || (c1 == (c2 - ('a'-'A')));
	}
	if ( (c1>='a') && (c1<='z') ){
		return (c1==c2) || (c1 == (c2 + ('a'-'A')));
	}
	return c1==c2;
}
static int strncasecmp(const char *s1, const char *s2, int n){
	int i;
	for (i=0; s1[i]!=0 && s2[i]!=0 && i<n; i++){
		if ( nocaseequal(s1[i],s2[i]) ){
			if (s1[i]<s2[i])
				return -1;
			else
				return 1;
		}
			
	}
	if (s2[i]!=0)
		return -1;
	return 0;
}
#endif


SocketServer::SocketServer(MRef<ServerSocket*> sock, MRef<SipMessageTransport*> r): ssock(sock), receiver(r),doStop(false){

}




void SocketServer::run(){
	struct timeval timeout;
	fd_set set;
	int fd = ssock->getFd();
	while (!doStop){

		int avail;
		do{
			FD_ZERO(&set);
			#ifdef WIN32
			FD_SET( (uint32_t) fd, &set);
			#else
			FD_SET(fd, &set);
			#endif
			
			timeout.tv_sec = 5;
			timeout.tv_usec= 0;
			avail = select(fd+1,&set,NULL,NULL,&timeout );
			if (avail<0){
				Thread::msleep(500);
			}
		} while( avail < 0 );
		//if (avail==0){
		//	cerr<< "SocketServer::run(): Timeout"<< endl;
		//}
		MRef<SipMessageTransport *> r = receiver;
		if (avail && !doStop && r){
			MRef<StreamSocket *> ss;

			try{
				ss = ssock->accept();
			} catch( NetworkException &){
			}

			if (ss){
				r->addSocket(ss);
			}else{
				cerr << "Warning: Failed to accept client"<< endl;
			}
		}

	}

} // "myself" will be freed here and the object can be freed.

void SocketServer::start(){
	Thread t(this);
}

void SocketServer::stop(){
	doStop=true;
}


class SipMessageParser{
	public:
		SipMessageParser();
		~SipMessageParser();

		MRef<SipMessage *> feed( uint8_t data );
		void init();
	private:
		void expandBuffer();
		uint32_t findContentLength();
		uint8_t * buffer;
		uint32_t length;
		uint32_t index;
		uint8_t state;
		uint32_t contentIndex;
		uint32_t contentLength;
};

SipMessageParser::SipMessageParser(){
	buffer = (uint8_t *)malloc( BUFFER_UNIT * sizeof( uint8_t ) );
	for (unsigned int i=0; i< BUFFER_UNIT * sizeof( uint8_t ); i++)
		buffer[i]=0;

	length = BUFFER_UNIT;
	index = 0;
	contentIndex = 0;
	state = 0;

}

void SipMessageParser::init(){
	realloc( buffer, BUFFER_UNIT );
	length = BUFFER_UNIT;
	state = 0;
	index = 0;
	contentIndex = 0;
}

SipMessageParser::~SipMessageParser(){
	free( buffer );
}

MRef<SipMessage*> SipMessageParser::feed( uint8_t udata ){
	char data = (char)udata;
	if( index >= length ){
		expandBuffer();
	}

	buffer[index++] = udata;

	switch( state ){
		case 0:
			if( data == '\n' )
				state = 1;
			break;
		case 1:
			if( data == '\n' ){
				/* Reached the end of the Header */
				state = 2;
				contentLength = findContentLength();
				if( contentLength == 0 ){
					char tmp[12];
					tmp[11]=0;
					memcpy(&tmp[0], buffer , 11);
					string messageString( (char *)buffer, index );
					init();
#ifdef ENABLE_TS
					ts.save(tmp);
#endif
					MRef<SipMessage*> msg = SipMessage::createMessage( messageString );
#ifdef ENABLE_TS
					ts.save("createMessage end");
#endif
					return msg;
					
					//return SipMessage::createMessage( messageString );
				}
				contentIndex = 0;
			}
			else if( data != '\r' )
				state = 0;
			break;
		case 2:
			if( ++contentIndex == contentLength ){
				char tmp[12];
				tmp[11]=0;
				memcpy(&tmp[0], buffer , 11);
				string messageString( (char*)buffer, index );
				init();
#ifdef ENABLE_TS
				ts.save(tmp);
#endif
				MRef<SipMessage*> msg = SipMessage::createMessage( messageString );
#ifdef ENABLE_TS
				ts.save("createMessage end");
#endif
				return msg;
				//return SipMessage::createMessage( messageString );
			}
	}
	return NULL;
}


void SipMessageParser::expandBuffer(){
	buffer = (uint8_t *)realloc( buffer, BUFFER_UNIT * ( length / BUFFER_UNIT + 1 ) );
	length += BUFFER_UNIT;
}

uint32_t SipMessageParser::findContentLength(){
	uint32_t i = 0;
	const char * contentLengthString = "\nContent-Length: ";

	for( i = 0; i + 17 < index; i++ ){
		if( strncasecmp( contentLengthString, (char *)(buffer + i) , 17  ) == 0 ){
			uint32_t j = 0;
			string num;
			
			while( i + j + 17 < index && (buffer[i+j+17] == ' ' || buffer[i+j+17] == '\t') ){
				j++;
			}
			
			for( ; i + 17 + j < index ; j++ ){
				if( buffer[i+j+17] >= '0' && buffer[i+j+18] <= '9' ){
					num += buffer[i+j+17];
				}
				else break;
			}
			return atoi( num.c_str() );
		}
	}
	return 0;
}

class StreamThreadData{
	public:
		StreamThreadData( MRef<SipMessageTransport *> );
		SipMessageParser parser;
		MRef<SipMessageTransport  *> transport;
		void run();
		void streamSocketRead( MRef<StreamSocket *> socket );
};

StreamThreadData::StreamThreadData( MRef<SipMessageTransport *> transport){
	this->transport = transport;
}


bool sipdebug_print_packets=false;

void set_debug_print_packets(bool f){
	sipdebug_print_packets=f;
}

bool get_debug_print_packets(){
	return sipdebug_print_packets;
}

#ifdef DEBUG_OUTPUT

uint64_t startTime = 0;

void printMessage(string header, string packet){
	if (startTime==0)
		startTime = mtime();
	uint64_t t;
	t=mtime();
	int64_t sec = t / 1000 - startTime / 1000;
	int64_t msec = t - startTime;
	
	header = (sec<100?string("0"):string("")) + 
		 (sec<10?"0":"") + 
		 itoa((int)(msec/1000))+
		 ":"+
		 (msec<10?"0":"")+
		 (msec<100?"0":"")+
		 itoa((int)(msec%1000))+ 
		 " " + 
		 header;
	
	if (sipdebug_print_packets){
		size_t strlen=packet.size();;
		mout << header<<": ";
		for (size_t i=0; i<strlen; i++){
			mout << packet[i];
			if (packet[i]=='\n')
				mout << header<<": ";
		}
		mout << end;
	}
}
#endif

static void * udpThread( void * arg );
static void * streamThread( void * arg );

SipMessageTransport::SipMessageTransport(
						string local_ip, 
						string contactIP, 
						int32_t externalContactUdpPort, 
						int32_t local_udp_port, 
						int32_t local_tcp_port,
						int32_t local_tls_port,
						MRef<certificate_chain *> cchain, 
						MRef<ca_db *> cert_db
			):
			//udpsock(false,local_udp_port),
			localIP(local_ip),
			contactIP(contactIP),
			externalContactUdpPort(externalContactUdpPort),
			localUDPPort(local_udp_port),
			localTCPPort(local_tcp_port),
			localTLSPort(local_tls_port),
			cert_chain(cchain), 
			cert_db(cert_db),
			tls_ctx(NULL)
{
	udpsock = new UDPSocket(local_udp_port, false );
	
	Thread::createThread(udpThread, this);
	
	int i;
	for( i=0; i < NB_THREADS ; i++ ){
            Thread::createThread(streamThread, new StreamThreadData(this));
	}
}

//void SipMessageTransport::startUdpServer(){ }
//void SipMessageTransport::stopUdpServer(){ }

bool SipMessageTransport::handleCommand(const SipSMCommand& ){
	cerr << "SipMessageTransport::handleCommand: NOT IMPLEMENTED - BUG"<<endl;
	
}

void SipMessageTransport::startTcpServer(){
	try{
		tcpSocketServer = new SocketServer(new IP4ServerSocket(localTCPPort),this);
		tcpSocketServer->start();
	}catch( NetworkException & exc ){
		cerr << exc.what() << endl;
		return;
	}
}

void SipMessageTransport::stopTcpServer(){
	tcpSocketServer->stop();
	tcpSocketServer=NULL;
}

void SipMessageTransport::startTlsServer(){
	if( getMyCertificate().isNull() ){
		merr << "You need a personal certificate to run "
			"a TLS server. Please specify one in "
			"the certificate settings. minisip will "
			"now disable the TLS server." << end;
		return;
	}

	try{
		tlsSocketServer = new SocketServer(new TLSServerSocket(localTLSPort, getMyCertificate(), getCA_db() ),this);
		tlsSocketServer->start();
	}catch( NetworkException & exc ){
		cerr << "Exception caught when creating TCP server." << endl;
		cerr << exc.what() << endl;
		return;
	}


}

void SipMessageTransport::stopTlsServer(){
	tlsSocketServer->stop();
	tlsSocketServer=NULL;
}

/*
void SipMessageTransport::setSipSMCommandReceiver(MRef<SipSMCommandReceiver*> rec){
	commandReceiver = rec;
}
*/

void SipMessageTransport::addViaHeader( MRef<SipMessage*> pack,
									MRef<Socket *> socket,
									string branch ){
	string transport;
	uint16_t port;

	if( !socket )
		return;
	
	switch( socket->getType() ){
		case SOCKET_TYPE_TLS:
			transport = "TLS";
			port = (uint16_t)localTLSPort;
			break;
			
		case SOCKET_TYPE_TCP:
			transport = "TCP";
			port = (uint16_t)localTCPPort;
			break;
			
		case SOCKET_TYPE_UDP:
			transport = "UDP";
			port = (uint16_t)externalContactUdpPort;
			break;

		default:
			mdbg<< "SipMessageTransport: Unknown transport protocol " + socket->getType() <<end;
			return;
	}
	
	MRef<SipHeaderValue*> hdrVal = 
		new SipHeaderValueVia(transport, localIP, port);
	
	hdrVal->setParameter("branch",branch);
	
	MRef<SipHeader*> hdr = new SipHeader( hdrVal );
	
	pack->addHeader( hdr );
}


static bool getDestination(MRef<SipMessage*> pack, /*MRef<IPAddress*>*/ string &destAddr,
			   int32_t &destPort, string &destTransport)
{
	if( pack->getType() == SipResponse::type ){
		// Send responses to sent by address in top via.

		MRef<SipHeaderValueVia*> via = pack->getFirstVia();

		if ( via ){
			string peer = via->getParameter( "received" );
			if( peer == "" ){
				peer = via->getIp();
			}

			//destAddr = IPAddress::create( via->getIp() );
			destAddr = via->getIp();
			if( destAddr.size()>0 ){
				string rport = via->getParameter( "rport" );
				if( rport != "" ){
					destPort = atoi( rport.c_str() );
				}
				else{
					destPort = via->getPort();
				}
				if( !destPort ){
					destPort = 5060;
				}
				destTransport = via->getProtocol();
				return true;
			}
		}
	}
	else{
		// Send requests to address in first route if the route set
		// is non-empty, or directly to the reqeuest uri if the 
		// route set is empty.

		MRef<SipHeaderValue*> routeHeader =
			pack->getHeaderValueNo(SIP_HEADER_TYPE_ROUTE, 0);

		SipUri uri;

		if( routeHeader ){
			MRef<SipHeaderValueRoute*> route =
				(SipHeaderValueRoute*)*routeHeader;
			string str = route->getString();
			uri.setUri( str );
		}
		else {
			MRef<SipRequest*> req = (SipRequest*)*pack;
			uri.setUri(req->getUri());
		}

		if( uri.isValid() ){
			//destAddr = IPAddress::create( uri.getIp() );
			destAddr = uri.getIp();
			
			if( /*destAddr*/ destAddr.size()>0 ){
				destPort = uri.getPort();
				if( !destPort ){
					if( uri.getProtocolId() == "sips" ){
						destPort = 5061;
					}
					else{
						destPort = 5060;
					}
				}
				destTransport = uri.getTransport();
				if( destTransport.length() == 0 ){
					destTransport = "UDP";
				}
				return true;
			}
		}
	}

	return false;
}

void SipMessageTransport::sendMessage(MRef<SipMessage*> pack, 
				      const string &branch,
				      bool addVia)
{
	//MRef<IPAddress*> destAddr;
	string destAddr;
	int32_t destPort = 0;
	string destTransport;

	if( !getDestination( pack, destAddr, destPort, destTransport) ){
#ifdef DEBUG_OUTPUT
		cerr << "SipMessageTransport: WARNING: Could not find destination. Packet dropped."<<endl;
#endif
		return;
	}

	transform( destTransport.begin(), destTransport.end(),
		   destTransport.begin(), (int(*)(int))toupper );

	sendMessage( pack, /* **destAddr */ destAddr, destPort,
		     branch, destTransport, addVia );
}


MRef<Socket*> SipMessageTransport::findSocket(const string &transport,
					      /*IPAddress &*/ string destAddr,
					      uint16_t port)
{
	MRef<Socket*> socket;

	if( transport == "UDP" ){
		socket = (Socket*)*udpsock;
	}
	else{
		MRef<StreamSocket*> ssocket = findStreamSocket(destAddr, port);
		if( ssocket.isNull() ) {
			/* No existing StreamSocket to that host,
			 * create one */
			cerr << "SipMessageTransport: sendMessage: creating new socket" << endl;
			if( transport == "TLS" ){
				ssocket = new TLSSocket( destAddr, 
							port, tls_ctx, getMyCertificate(),
							cert_db );
			}
			else{ /* TCP */
				ssocket = new TCPSocket( destAddr, port );
			}

			addSocket( ssocket );
		} else cerr << "SipMessageTransport: sendMessage: reusing old socket" << endl;
		socket = *ssocket;
	}

	return socket;
}


void SipMessageTransport::sendMessage(MRef<SipMessage*> pack, 
				      /*IPAddress &*/ string ip_addr, 
				      int32_t port, 
				      string branch,
				      string preferredTransport,
				      bool addVia)
{
	MRef<Socket *> socket;
	MRef<IPAddress *> tempAddr;
	//IPAddress *destAddr = &ip_addr;

				
	try{
		socket = pack->getSocket();

		if( !socket ){
			socket = findSocket(preferredTransport, ip_addr, (uint16_t)port);
			pack->setSocket( socket );
		}

		if (addVia){
			addViaHeader( pack, socket, branch );
		}

		string packetString = pack->getString();
		if (!socket){
			cerr << "EE: NO SOCKET"<<endl<<endl;;
		}

		UDPSocket *dsocket = dynamic_cast<UDPSocket*>(*socket);
		StreamSocket *ssocket = dynamic_cast<StreamSocket*>(*socket);
		
		if( ssocket ){
			/* At this point if socket != we send on a 
			 * streamsocket */
#ifdef DEBUG_OUTPUT
			printMessage("OUT (STREAM)", packetString);
#endif
#ifdef ENABLE_TS
			//ts.save( PACKET_OUT );
			char tmp[12];
			tmp[11]=0;
			memcpy(&tmp[0], packetString.c_str() , 11);
#endif
			if( ssocket->write( packetString ) == -1 ){
				throw SendFailed( errno );
			}
		}
		else if( dsocket ){
			/* otherwise use the UDP socket */
#ifdef DEBUG_OUTPUT
			printMessage("OUT (UDP)", packetString);
#endif
#ifdef ENABLE_TS
			//ts.save( PACKET_OUT );
			char tmp[12];
			tmp[11]=0;
			memcpy(&tmp[0], packetString.c_str() , 11);
			ts.save( tmp );

#endif
			IPAddress *destAddr = IPAddress::create(ip_addr);

			
			if (destAddr){
				if( dsocket->sendTo( *destAddr, port, 
							(const void*)packetString.c_str(),
							(int32_t)packetString.length() ) == -1 ){

					throw SendFailed( errno );

				}
			}else{
				CommandString transportError( pack->getCallId(), 
						SipCommandString::transport_error,
						"SipMessageTransport: host could not be resolved: "+ip_addr);
				SipSMCommand transportErrorCommand(
						transportError, 
						SipSMCommand::transport_layer, 
						SipSMCommand::transaction_layer);

				if (dispatcher)
					dispatcher->enqueueCommand( transportErrorCommand, LOW_PRIO_QUEUE );
				else
					mdbg<< "SipMessageTransport: ERROR: NO SIP COMMAND RECEIVER - DROPPING COMMAND"<<end;

			}
		}
		else{
			cerr << "No valid socket!" << endl;
		}
	}
	catch( NetworkException & exc ){
		string message = exc.what();
		string callId = pack->getCallId();
#ifdef DEBUG_OUTPUT
		mdbg << "Transport error in SipMessageTransport: " << message << end;
		cerr << "SipMessageTransport: sendMessage: exception thrown!" << endl;
#endif
		CommandString transportError( callId, 
					      SipCommandString::transport_error,
					      "SipMessageTransport: "+message );
		SipSMCommand transportErrorCommand(
				transportError, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer);

		if (dispatcher)
			dispatcher->enqueueCommand( transportErrorCommand, LOW_PRIO_QUEUE );
		else
			mdbg<< "SipMessageTransport: ERROR: NO SIP COMMAND RECEIVER - DROPPING COMMAND"<<end;
	}
	
}

void SipMessageTransport::setDispatcher(MRef<SipMessageDispatcher*> d){
	dispatcher=d;
}

void SipMessageTransport::addSocket(MRef<StreamSocket *> sock){
	socksLock.lock();
	this->socks.push_back(sock);
	socksLock.unlock();
	socksPendingLock.lock();
	this->socksPending.push_back(sock);
	socksPendingLock.unlock();
        semaphore.inc();
}

MRef<StreamSocket *> SipMessageTransport::findStreamSocket( /*IPAddress &*/ string address, uint16_t port ){
	list<MRef<StreamSocket *> >::iterator i;

	socksLock.lock();
	for( i=socks.begin(); i != socks.end(); i++ ){
		if( (*i)->matchesPeer(address, port) ){
			socksLock.unlock();
			return *i;
		}
	}
	socksLock.unlock();
	return NULL;
}

static void updateVia(MRef<SipMessage*> pack, IPAddress *from,
		      uint16_t port)
{
	MRef<SipHeaderValueVia*> via = pack->getFirstVia();
	string peerAddr = from->getString();

	if( !via ){
		merr << "No Via header in incoming message!" << end;
		return;
	}

	if( via->hasParameter( "rport" ) ){
		char buf[20] = "";
		sprintf(buf, "%d", port);
		via->setParameter( "rport", buf);
	}
	
	string addr = via->getIp();
	if( addr != peerAddr ){
		via->setParameter( "received", peerAddr );
	}
}


#define UDP_MAX_SIZE 65536

void SipMessageTransport::udpSocketRead(){
	char buffer[UDP_MAX_SIZE];

	for (int i=0; i<UDP_MAX_SIZE; i++){
		buffer[i]=0;
	}
	
	int avail;
	MRef<SipMessage*> pack;
	int32_t nread;
    fd_set set;
	
	while( true ){
		FD_ZERO(&set);
		#ifdef WIN32
		FD_SET( (uint32_t) udpsock->getFd(), &set);
		#else
		FD_SET(udpsock->getFd(), &set);
		#endif

		do{
			avail = select(udpsock->getFd()+1,&set,NULL,NULL,NULL );
		} while( avail < 0 );

		if( FD_ISSET( udpsock->getFd(), &set )){
			IPAddress *from = NULL;
			int32_t port = 0;

			nread = udpsock->recvFrom(buffer, UDP_MAX_SIZE, from, port);
			
			if (nread == -1){
				mdbg << "Some error occured while reading from UdpSocket"<<end;
				continue;
			}

			if ( nread == 0){
				// Connection was closed
				break;
			}

			if (nread < (int)strlen("SIP/2.0")){
				continue;
			}

			try{
#ifdef ENABLE_TS
				//ts.save( PACKET_IN );
				char tmp[12];
				tmp[11]=0;
				memcpy(&tmp[0], buffer, 11); 
				ts.save( tmp );

#endif
				string data = string(buffer, nread);
#ifdef DEBUG_OUTPUT
				printMessage("IN (UDP)", data);
#endif
				pack = SipMessage::createMessage( data );
				
				pack->setSocket( *udpsock );
				updateVia(pack, from, (uint16_t)port);
				
				SipSMCommand cmd(pack, 
						SipSMCommand::transport_layer, 
						SipSMCommand::transaction_layer);
				
				if (dispatcher)
					dispatcher->enqueueCommand( cmd, LOW_PRIO_QUEUE );
				else
					mdbg<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;
				pack=NULL;
			}
			
			catch(SipExceptionInvalidMessage & ){
				/* Probably we don't have enough data
				 * so go back to reading */
#ifdef DEBUG_OUTPUT
				mdbg << "Invalid data on UDP socket, discarded" << end;
#endif
				continue;
			}
			
			catch(SipExceptionInvalidStart & ){
				// This does not look like a SIP
				// packet, close the connection
				
#ifdef DEBUG_OUTPUT
				mdbg << "Invalid data on UDP socket, discarded" << end;
#endif
				continue;
			}
		} // if event
	}// while true
}

void StreamThreadData::run(){
	while(true){

		MRef<StreamSocket *> socket;
                transport->semaphore.dec();
                
		/* Take the last socket pending to be read */
		transport->socksPendingLock.lock();
		socket = transport->socksPending.front();
		transport->socksPending.pop_front();
		transport->socksPendingLock.unlock();
                
		/* Read from it until it gets closed */
		streamSocketRead( socket );

		/* The socket was closed */
		transport->socksLock.lock();
		transport->socks.remove( socket );
		transport->socksLock.unlock();
#ifdef DEBUG_OUTPUT
		mdbg << "StreamSocket closed" << end;
#endif

		parser.init();
	}
}

#define STREAM_MAX_PKT_SIZE 65536

void StreamThreadData::streamSocketRead( MRef<StreamSocket *> socket ){
	char buffer[STREAM_MAX_PKT_SIZE+1];
	for (int i=0; i< STREAM_MAX_PKT_SIZE+1; i++){
		buffer[i]=0;
	}
	int avail;
	MRef<SipMessage*> pack;
	int32_t nread;
	fd_set set;
	struct timeval tv;
	tv.tv_sec = 600;
	tv.tv_usec = 0;

	
	while( true ){
		FD_ZERO(&set);
		#ifdef WIN32
		FD_SET( (uint32_t) socket->getFd(), &set);
		#else
		FD_SET(socket->getFd(), &set);
		#endif

		do{
			avail = select(socket->getFd()+1,&set,NULL,NULL,&tv );
		} while( avail <= 0 );

		if( avail == 0 ){
#ifdef DEBUG_OUTPUT
			mdbg << "Closing Stream socket due to inactivity" << end;
#endif
			break;
		}

		if( FD_ISSET( socket->getFd(), &set )){
			nread = socket->read( buffer, STREAM_MAX_PKT_SIZE);

			if (nread == -1){
				mdbg << "Some error occured while reading from StreamSocket" << end;
				continue;
			}

			if ( nread == 0){
				// Connection was closed
				break;
			}
#ifdef ENABLE_TS
			//ts.save( PACKET_IN );


#endif

			try{
				uint32_t i;
				for( i = 0; i < (uint32_t)nread; i++ ){
					pack = parser.feed( buffer[i] );
					
					if( pack ){
#ifdef DEBUG_OUTPUT
						printMessage("IN (STREAM)", buffer);
#endif
						//					dialogContainer->enqueueMessage( pack );

						IPAddress *peer = socket->getPeerAddress();
						pack->setSocket( *socket );
						updateVia( pack, peer, (int16_t)socket->getPeerPort() );

						SipSMCommand cmd(pack, SipSMCommand::transport_layer, /*SipSMCommand::ANY*/ SipSMCommand::transaction_layer);
						if (transport->dispatcher){
							transport->dispatcher->enqueueCommand( cmd, LOW_PRIO_QUEUE );
						}else
							mdbg<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;
						pack=NULL;
					}

				}
			}
			
			catch(SipExceptionInvalidMessage & ){
#if 0
				// Check that we received data
				// is not too big, in which case close
				// the socket, to avoid DoS
				if(socket->received.size() > 8192){
					break;
				}
#endif
				/* Probably we don't have enough data
				 * so go back to reading */
				continue;
			}
			
			catch(SipExceptionInvalidStart & ){
				// This does not look like a SIP
				// packet, close the connection
				
				break;
			}
		} // if event
	}// while true
}

static void * streamThread( void * arg ){
	StreamThreadData * data;
	data = (StreamThreadData *)arg;

	data->run();
	return NULL;
}

static void * udpThread( void * arg ){
	MRef<SipMessageTransport*>  trans( (SipMessageTransport *)arg);

	trans->udpSocketRead();
	return NULL;
}

