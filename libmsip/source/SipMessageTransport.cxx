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



#include<assert.h>

#include<config.h>

#include<errno.h>
#include<stdio.h>

#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipAck.h>
#include<libmsip/SipCancel.h>
#include<libmsip/SipBye.h>
#include<libmsip/SipSubscribe.h>
#include<libmsip/SipNotify.h>
#include<libmsip/SipIMMessage.h>
#include<libmsip/SipException.h>
#include<libmsip/SipHeaderVia.h>

#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>
#include<libmutil/Timestamp.h>
#include<libmutil/MemObject.h>
#include<libmutil/mtime.h>
#include<libmutil/dbg.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>

#ifdef WIN32
#include<winsock2.h>
#endif

#define TIMEOUT 600000
#define NB_THREADS 5
#define BUFFER_UNIT 1024

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
	for (int i=0; s1[i]!=0 && s2[i]!=0 && i<n; i++){
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
					ts.save(tmp);
					MRef<SipMessage*> msg = SipMessage::createMessage( messageString );
					ts.save("createMessage end");
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
				ts.save(tmp);
				MRef<SipMessage*> msg = SipMessage::createMessage( messageString );
				ts.save("createMessage end");
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
			
			while( i + j < index && (buffer[i+j] == ' ' || buffer[i+j] == '\t') ){
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
#ifdef DEBUG_OUTPUT

uint64_t startTime = 0;

void printMessage(string header, string packet){
	if (startTime==0)
		startTime = mtime();
	uint64_t t;
	t=mtime();
	int64_t sec = t / 1000 - startTime / 1000;
	int64_t msec = t - startTime;
	
	header = (sec<100?string("0"):string("")) + (sec<10?"0":"") + itoa(msec/1000)+":"+(msec<10?"0":"")+ (msec<100?"0":"")+itoa(msec%1000)+ " " + header;
	
	if (sipdebug_print_packets){
		int strlen=packet.size();;
		mout << header<<": ";
		for (int i=0; i<strlen; i++){
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
			string preferredTransport,
                        int32_t externalContactUdpPort, 
                        int32_t local_udp_port, 
                        int32_t local_tcp_port,
                        int32_t local_tls_port,
			MRef<certificate_chain *> cchain, 
                        MRef<ca_db *> cert_db
			):
                                        udpsock(false,local_udp_port),
                                        localIP(local_ip),
                                        contactIP(contactIP),
					preferredTransport(preferredTransport),
                                        externalContactUdpPort(externalContactUdpPort),
                                        localUDPPort(local_udp_port),
                                        localTCPPort(local_tcp_port),
					localTLSPort(local_tls_port),
                                        cert_chain(cchain), 
                                        cert_db(cert_db),
                                        tls_ctx(NULL)
							
{
	int i;
        Thread::createThread(udpThread, this);
	
	for( i=0; i < NB_THREADS ; i++ ){
            Thread::createThread(streamThread, new StreamThreadData(this));
	}
}

void SipMessageTransport::setSipSMCommandReceiver(MRef<SipSMCommandReceiver*> rec){
	commandReceiver = rec;
}

void SipMessageTransport::addViaHeader( MRef<SipMessage*> pack,
		                        MRef<StreamSocket *> socket,
					string branch ){
	string transport;
	uint16_t port;

	if( socket ){
		if( socket->getType() == SOCKET_TYPE_TLS ){
			transport = "TLS";
			port = localTLSPort;
		}
		else{
			transport = "TCP";
			port = localTCPPort;
		}
	}
	else{
		transport = "UDP";
		port = localUDPPort;
	}
	
	MRef<SipHeaderValue*> hdrVal = 
		new SipHeaderValueVia(transport, localIP, port, branch);
	
	MRef<SipHeader*> hdr = new SipHeader( hdrVal );
	
	pack->addHeader( hdr );
}

void SipMessageTransport::sendMessage(MRef<SipMessage*> pack, 
                                     IPAddress &ip_addr, 
                                     int32_t port, 
				     string branch,
				     bool addVia
				     )
{
	MRef<StreamSocket *> socket;

				
	try{
		socket = findStreamSocket(ip_addr, port);

		if( socket.isNull() && preferredTransport != "UDP" ){
			/* No existing StreamSocket to that host,
			 * create one */

			if( preferredTransport == "TLS" ){
				//FIXME have a different port per transport,
				//to avoid this ...
				socket = findStreamSocket(ip_addr, 5061);
				if( socket.isNull() ){
					socket = new TLSSocket( ip_addr, 
		                        5061, tls_ctx, getMyCertificate(),
					cert_db );
				}

				addSocket( socket );
			}

			else{ /* TCP */
				socket = new TCPSocket( ip_addr, port );

				addSocket( socket );
			}
			
		}

		if (addVia){
			addViaHeader( pack, socket, branch );
		}

		string packetString = pack->getString();
		
		if( socket ){
			/* At this point if socket != we send on a 
			 * streamsocket */
#ifdef DEBUG_OUTPUT
			printMessage("OUT (STREAM)", packetString);
#endif
#ifndef _MSC_VER
			//ts.save( PACKET_OUT );
			char tmp[12];
			tmp[11]=0;
			memcpy(&tmp[0], packetString.c_str() , 11);
			ts.save( tmp );
#endif
			if( socket->write( packetString ) == -1 ){
				throw new SendFailed( errno );
			}
		}
		else{
			/* otherwise use the UDP socket */
#ifdef DEBUG_OUTPUT
			printMessage("OUT (UDP)", packetString);
#endif
#ifndef _MSC_VER
			//ts.save( PACKET_OUT );
			char tmp[12];
			tmp[11]=0;
			memcpy(&tmp[0], packetString.c_str() , 11);
			ts.save( tmp );

#endif
			if( udpsock.sendTo( ip_addr, port, 
					(const void*)packetString.c_str(),
					(int32_t)packetString.length() ) == -1 ){
			
				throw new SendFailed( errno );
			
			}; 
		}
	}
	catch( NetworkException * exc ){
		string message = exc->errorDescription();
		string callId = pack->getCallId();
#ifdef DEBUG_OUTPUT
		mdbg << "Transport error in SipMessageTransport: " << message << end;
#endif
		CommandString transportError( callId, 
					      SipCommandString::transport_error,
					      "SipMessageTransport: "+message );
		SipSMCommand transportErrorCommand(
				transportError, 
				SipSMCommand::remote, 
				SipSMCommand::transaction);

		delete exc;

		if (! commandReceiver.isNull())
			commandReceiver->handleCommand( transportErrorCommand );
		else
			mdbg<< "SipMessageTransport: ERROR: NO SIP COMMAND RECEIVER - DROPPING COMMAND"<<end;
	}
	
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

MRef<StreamSocket *> SipMessageTransport::findStreamSocket( IPAddress & address, uint16_t port ){
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

void SipMessageTransport::udpSocketRead(){
	char buffer[16384];
	int avail;
	MRef<SipMessage*> pack;
#ifdef MINISIP_MEMDEBUG
	pack.setUser("SipMessageTransport::udpSocketRead:pack");
#endif
	int32_t nread;
    	fd_set set;
	
	while( true ){
	        FD_ZERO(&set);
        	FD_SET(udpsock.getFd(), &set);

		do{
            		avail = select(udpsock.getFd()+1,&set,NULL,NULL,NULL );
		} while( avail < 0 );

		if( FD_ISSET( udpsock.getFd(), &set )){
			nread = udpsock.recv(buffer, 16384);
			
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
#ifndef _MSC_VER
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
				
///				pack->setSocket( NULL );

				
				SipSMCommand cmd(pack, SipSMCommand::remote, SipSMCommand::ANY);
				
				if (!commandReceiver.isNull())
					commandReceiver->handleCommand( cmd );
				else
					mdbg<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;
				pack=NULL;
			}
			
			catch(SipExceptionInvalidMessage * exc){
				delete exc;
				/* Probably we don't have enough data
				 * so go back to reading */
#ifdef DEBUG_OUTPUT
				mdbg << "Invalid data on UDP socket, discarded" << end;
#endif
				continue;
			}
			
			catch(SipExceptionInvalidStart * exc){
				// This does not look like a SIP
				// packet, close the connection
				
				delete exc;
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

void StreamThreadData::streamSocketRead( MRef<StreamSocket *> socket ){
	char buffer[16384];
	int avail;
	MRef<SipMessage*> pack;
#ifdef MINISIP_MEMDEBUG
	pack.setUser("SipMessageTransport::streamSocketRead:pack");
#endif
	int32_t nread;
	fd_set set;
	struct timeval tv;
	tv.tv_sec = 600;
	tv.tv_usec = 0;

	
	while( true ){
		FD_ZERO(&set);
		FD_SET(socket->getFd(), &set);

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
			nread = socket->read( buffer, 16384 );

			if (nread == -1){
				mdbg << "Some error occured while reading from StreamSocket" << end;
				continue;
			}

			if ( nread == 0){
				// Connection was closed
				break;
			}
#ifndef _MSC_VER
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

						SipSMCommand cmd(pack, SipSMCommand::remote, SipSMCommand::ANY);
						if (!transport->commandReceiver.isNull())
							transport->commandReceiver->handleCommand( cmd );
						else
							mdbg<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;
					}

				}
			}
			
			catch(SipExceptionInvalidMessage * exc){
#if 0
				// Check that we received data
				// is not too big, in which case close
				// the socket, to avoid DoS
				if(socket->received.size() > 8192){
					break;
				}
#endif
				delete exc;
				/* Probably we don't have enough data
				 * so go back to reading */
				continue;
			}
			
			catch(SipExceptionInvalidStart * exc){
				// This does not look like a SIP
				// packet, close the connection
				
				delete exc;
				break;
			}
		} // if event
	}// while true
}

static void * streamThread( void * arg ){
	StreamThreadData * data((StreamThreadData *)arg);

	data->run();
	return NULL;
}

static void * udpThread( void * arg ){
	MRef<SipMessageTransport*>  trans( (SipMessageTransport *)arg);

	trans->udpSocketRead();
	return NULL;
}

