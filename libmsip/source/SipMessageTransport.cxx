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

#if 0
#ifdef LINUX
#include<netinet/in.h>
#include<sys/socket.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#endif

#endif

//#include<sys/poll.h>
//#include<sys/types.h>
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

//#include<libmsip/dialogs/DefaultCallHandler.h>

#define TIMEOUT 600000
#define NB_THREADS 5


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
static MRef<SipMessage*> checkType(string data);

SipMessageTransport::SipMessageTransport(
                        string local_ip, 
                        string contactIP, 
			string preferredTransport,
//                        MRef<SipDialogContainer*> dContainer, 
                        int32_t externalContactUdpPort, 
                        int32_t local_udp_port, 
                        int32_t local_tcp_port 
#ifndef NO_SECURITY
                        ,int32_t local_tls_port,
			MRef<certificate *> cert, 
                        MRef<ca_db *> cert_db
#endif
			):/*MObject("SipMessageTransport"),*/
//                                        commandReceiver(NULL), 
                                        udpsock(false,local_udp_port),
                                        localIP(local_ip),
                                        contactIP(contactIP),
					preferredTransport(preferredTransport),
                                        externalContactUdpPort(externalContactUdpPort),
                                        localUDPPort(local_udp_port),
                                        localTCPPort(local_tcp_port)
#ifndef NO_SECURITY
					, localTLSPort(local_tls_port),
//                                        dialogContainer(dContainer), //FIXME: Bug in older versions when not using security
                                        cert(cert), 
                                        cert_db(cert_db),
                                        tls_ctx(NULL)
#endif
							
{
	int i;
        Thread::createThread(udpThread, this);
	
	for( i=0; i < NB_THREADS ; i++ ){
            Thread::createThread(streamThread, this);
	}
}

void SipMessageTransport::setSipSMCommandReceiver(MRef<SipSMCommandReceiver*> rec){
	commandReceiver = rec;
}

void SipMessageTransport::addViaHeader( MRef<SipMessage*> pack,
		                        StreamSocket * socket,
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
	StreamSocket * socket;

	try{
		socket = findStreamSocket(ip_addr, port);

		if( socket == NULL && preferredTransport != "UDP" ){
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
			
			if( pack->getType() != SipResponse::type ){
				MRef<SipHeaderValue*> hval = 
					new SipHeaderValueVia(preferredTransport, localIP, localTCPPort, branch);
				
				MRef<SipHeader*> hdr = new SipHeader(hval);
				
				pack->addHeader(hdr);
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

void SipMessageTransport::addSocket(StreamSocket * sock){
	socks_lock.lock();
	this->socks.push_back(sock);
	socks_lock.unlock();
        semaphore.inc();
}

StreamSocket * SipMessageTransport::findStreamSocket( IPAddress & address, uint16_t port ){
	list<StreamSocket *>::iterator i;

	socks_lock.lock();
	for( i=socks.begin(); i != socks.end(); i++ ){
		if( (*i)->matchesPeer(address, port) ){
			socks_lock.unlock();
			return *i;
		}
	}
	socks_lock.unlock();
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
				pack = checkType( data );
				
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

void SipMessageTransport::threadPool(){
	while(true){

		StreamSocket * socket;
                semaphore.dec();
                
                socks_lock.lock();
                
		socket = socks.front();
		socks.pop_front();
		
		socks_lock.unlock();
                
		streamSocketRead( socket );

		delete socket;
	}
}

void SipMessageTransport::streamSocketRead( StreamSocket * socket ){
	char buffer[16384];
	int avail;
	//struct pollfd p[1];
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
        FD_SET(udpsock.getFd(), &set);
		//p[0].fd = socket->getFd();
		//p[0].events = POLLIN;

		do{
			//avail = poll( p, 1, TIMEOUT );
            avail = select(1,&set,NULL,NULL,&tv );
		} while( avail <= 0 );

		if( avail == 0 ){
#ifdef DEBUG_OUTPUT
			merr << "Closing Stream socket due to inactivity" << end;
#endif
			break;
		}

		//if( p[0].revents != 0 ){
		if( FD_ISSET( udpsock.getFd(), &set )){
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
				while( true ){
					
					if( socket->received.length() > 0 ){
						pack = checkType( socket->received );
					}
					else{
						break;
					}
///					pack->setSocket( socket );

#ifdef DEBUG_OUTPUT
					printMessage("IN (STREAM)", buffer);
#endif
//					dialogContainer->enqueueMessage( pack );

					SipSMCommand cmd(pack, SipSMCommand::remote, SipSMCommand::ANY);
					if (!commandReceiver.isNull())
						commandReceiver->handleCommand( cmd );
					else
						merr<< "SipMessageTransport: ERROR: NO SIP MESSAGE RECEIVER - DROPPING MESSAGE"<<end;

				}
			}
			
			catch(SipExceptionInvalidMessage * exc){
				// Check that we received data
				// is not too big, in which case close
				// the socket, to avoid DoS
				if(socket->received.size() > 8192){
					break;
				}
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




	


static MRef<SipMessage*> checkType(string data){

	int n = data.size();

	if (n>3   &&    (data[0]=='S'||data[0]=='s') && 
			(data[1]=='I'||data[1]=='i') && 
			(data[2]=='P'||data[2]=='p' )){
		return MRef<SipMessage*>(new SipResponse(data));
	}else{
		if (n> 7 && data.substr(0, 7) == "MESSAGE"){
			return MRef<SipMessage*>(new SipIMMessage(data));
		}
		if (n> 6 && data.substr(0, 6) == "CANCEL"){
			return MRef<SipMessage*>(new SipCancel(data));
		}
		if (n> 3 && data.substr(0, 3)=="BYE"){
			return MRef<SipMessage*>(new SipBye(data));
		}
		if (n> 6 && data.substr(0, 6)=="INVITE"){
			return MRef<SipMessage*>(new SipInvite(data));
		}
		if (n> 3 && data.substr(0, 3)=="ACK"){
			return MRef<SipMessage*>(new SipAck(data));
		}
		if (n> 9 && data.substr(0, 9)=="SUBSCRIBE"){
			return MRef<SipMessage*>(new SipSubscribe(data));
		}
		if (n> 6 && data.substr(0, 6)=="NOTIFY"){
			return MRef<SipMessage*>(new SipNotify(data));
		}

		throw new SipExceptionInvalidStart;
	}
	return NULL;
}

static void * streamThread( void * arg ){
	MRef<SipMessageTransport*>  trans((SipMessageTransport *)arg);

	trans->threadPool();
	return NULL;
}

static void * udpThread( void * arg ){
//	mout << "ALIVE: udpThread started... casting arg"<<end;
	MRef<SipMessageTransport*>  trans( (SipMessageTransport *)arg);
//	mout << "udpThread: done casting arg"<<end;

	trans->udpSocketRead();
	return NULL;
}

