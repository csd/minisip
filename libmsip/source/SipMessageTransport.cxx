/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
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
#include<libmutil/dbg.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>


#define TIMEOUT 600000
#define NB_THREADS 5
#define BUFFER_UNIT 1024

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
					string messageString( (char *)buffer, index );
					init();
					return SipMessage::createMessage( messageString );
				}
				contentIndex = 0;
			}
			else if( data != '\r' )
				state = 0;
			break;
		case 2:
			if( ++contentIndex == contentLength ){
				string messageString( (char*)buffer, index );
				init();
				return SipMessage::createMessage( messageString );
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
	const char * contentLengthString = "\nContentLength: ";

	for( i = 0; i + 16 < index; i++ ){
		if( strncasecmp( contentLengthString, (char *)(buffer + i) , 16  ) == 0 ){
			uint32_t j = 0;
			string num;
			
			while( i + j < index && (buffer[i+j] == ' ' || buffer[i+j] == '\t') ){
				j++;
			}
			
			for( ; i + 16 + j < index ; j++ ){
				if( buffer[i+j+16] >= '0' && buffer[i+j+16] <= '9' ){
					num += buffer[i+j+16];
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

struct timeval startTime={0};

void printMessage(string header, string packet){
	if (startTime.tv_sec==0)
		gettimeofday(&startTime,NULL);
	struct timeval t;
	gettimeofday(&t, NULL);
	long sec = t.tv_sec - startTime.tv_sec;
	int64_t msec = sec*1000 + (t.tv_usec - startTime.tv_usec)/1000;
	
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
//                        MRef<SipDialogContainer*> dContainer, 
                        int32_t externalContactUdpPort, 
                        int32_t local_udp_port, 
                        int32_t local_tcp_port,
                        int32_t local_tls_port,
			MRef<certificate *> cert, 
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
                                        cert(cert), 
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
				     string branch
				     )
{
	MRef<StreamSocket *> socket;

	try{
		socket = findStreamSocket(ip_addr, port);

		if( socket.isNull() && preferredTransport != "UDP" ){
			/* No existing StreamSocket to that host,
			 * create one */

			if( preferredTransport == "TLS" ){
				socket = new TLSSocket( ip_addr, 
						port, tls_ctx, cert, cert_db );

				addSocket( socket );
			}

			else{ /* TCP */
				socket = new TCPSocket( ip_addr, port );

				addSocket( socket );
			}
			
		}

		addViaHeader( pack, socket, branch );

		string packetString = pack->getString();
		
		if( socket ){
			/* At this point if socket != we send on a 
			 * streamsocket */
			if( socket->write( packetString ) == -1 ){
				throw new SendFailed( errno );
			}
		}
		else{
			/* otherwise use the UDP socket */
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
		merr << "Transport error in SipMessageTransport"<< end;
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
			merr<< "SipMessageTransport: ERROR: NO SIP COMMAND RECEIVER - DROPPING COMMAND"<<end;
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
	//struct pollfd p[1];
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
				merr << "Some error occured while reading from UdpSocket"<<end;
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
				ts.save( PACKET_IN );
				string data = string(buffer, nread);
#ifdef DEBUG_OUTPUT
				printMessage("IN (UDP)", data);
#endif
				pack = SipMessage::createMessage( data );
				
///				pack->setSocket( NULL );

				
//				dialogContainer->enqueuePacket( pack );
				SipSMCommand cmd(pack, SipSMCommand::remote, SipSMCommand::ANY);
				
				if (!commandReceiver.isNull())
					commandReceiver->handleCommand( cmd );
				else
					merr<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;
				pack=NULL;
			}
			
			catch(SipExceptionInvalidMessage * exc){
				delete exc;
				/* Probably we don't have enough data
				 * so go back to reading */
#ifdef DEBUG_OUTPUT
				merr << "Invalid data on UDP socket, discarded" << end;
#endif
				continue;
			}
			
			catch(SipExceptionInvalidStart * exc){
				// This does not look like a SIP
				// packet, close the connection
				
				delete exc;
#ifdef DEBUG_OUTPUT
				merr << "Invalid data on UDP socket, discarded" << end;
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
				merr << "Some error occured while reading from StreamSocket" << end;
				continue;
			}

			if ( nread == 0){
				// Connection was closed
				break;
			}

			ts.save( PACKET_IN );
			socket->received += string( buffer, nread );

			try{
				uint32_t i;
				for( i = 0; i < nread; i++ ){
					pack = parser.feed( buffer[i] );
					if( pack ){
						printMessage("IN (STREAM)", buffer);
						//					dialogContainer->enqueueMessage( pack );

						SipSMCommand cmd(pack, SipSMCommand::remote, SipSMCommand::ANY);
						if (!transport->commandReceiver.isNull())
							transport->commandReceiver->handleCommand( cmd );
						else
							merr<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;
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

