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

#include<stdint.h>

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
/*    
	socks_lock = new pthread_mutex_t;
	if( pthread_mutex_init (socks_lock, NULL) < 0 ){
		merr<< "An error occured when creating sockets mutex"<<end;
		exit(1);
	}
*/

/*
	semaphore = semget( rand(), 1, 0600 | IPC_CREAT );

	if (semaphore<0){
		perror("Could not create semaphore");
		exit(1);
	}
	union semun {
		int val;
		struct semid_ds *buf;
		ushort * array;
	} argument;

	argument.val = 0;

	if( semctl(semaphore, 0, SETVAL, argument) < 0) {
		fprintf( stderr, "Cannot set semaphore value.\n");
		exit(1);
	}
*/
    
//	pthread_t udp, stream;
	int i;

        
        
	//if( pthread_create( &udp, NULL, udpThread, this ) ){
	//	merr << "Could not create transport thread" << end;
	//	exit(1);
	//}
//	mout << "Creating udpThread..."<<end;
        Thread::createThread(udpThread, this);
//	mout<< "Done creating thread"<< end;
	
	for( i=0; i < NB_THREADS ; i++ ){
		//if( pthread_create( &stream, NULL, streamThread, this ) ){
		//	merr << "Could not create transport thread" << end;
		//	exit(1);
		//}
            Thread::createThread(streamThread, this);
	}
}


void SipMessageTransport::setSipSMCommandReceiver(MRef<SipSMCommandReceiver*> rec){
//	cerr << "SipMessageTransport::setSipSMCommandReceiver: setting commandReceiver"<<endl;
	commandReceiver = rec;
//	cerr << "SipMessageTransport::setSipSMCommandReceiver: done setting commandReceiver"<<endl;
}

/*
void SipMessageTransport::setSipMessageReceiver(MRef<SipMessageReceiver *> rcvr){
//	cerr << "SipMessageTransport::setSipMessageReceiver: setting messageReceiver"<<endl;
	messageReceiver = rcvr;
//	cerr << "SipMessageTransport::setSipMessageReceiver: done setting messageReceiver"<<endl;
}
*/


//Create a socket and send on this one.
void SipMessageTransport::sendMessage(MRef<SipMessage*> pack, 
                                     IPAddress &ip_addr, 
                                     int32_t port, 
                                     Socket * &socket, 
				     string branch,
                                     string transport
				     )
{
	try{
		if (transport=="ANY" || transport=="UDP"){
			string packet_string;
			if( pack->getType() != SipResponse::type ){
//#ifdef DEBUG_OUTPUT
//                                merr << "Transport layer adding via with port "<< externalContactUdpPort << end;
//#endif
	
//				if (pack->getType()==SipAck::type)
//					pack->addHeader(MRef<SipHeader*>(new SipHeaderVia("UDP", localIP, localUDPPort, branch+"ACK")));
//				else

				MRef<SipHeaderValue*> hdrVal = new SipHeaderValueVia("UDP", localIP, localUDPPort, branch);
				MRef<SipHeader*> hdr = new SipHeader(hdrVal);
				pack->addHeader(hdr);
//pack->addHeader(MRef<SipHeader*>(new SipHeader(SIP_HEADER_TYPE_VIA, "Via:", MRef<SipHeaderValue*>(new SipHeaderVia("UDP", localIP, localUDPPort, branch))));
				
//				
			}
			
			packet_string = pack->getString();
			ts.save( PACKET_OUT );
#ifdef DEBUG_OUTPUT
//			mout << "OUT (UDP), From localhost:"<<udpsock.get_port()
//					<< " to "<<ip_addr.get_string()
//					<<":"<<port<<end;

			printMessage("OUT (UDP)", packet_string);
#endif
#if 0
			if( sendto(udpsock.getFd(), 
					packet_string.c_str(), 
					packet_string.length(), 
					0, 
					ip_addr.getSockaddrptr(port), 
					ip_addr.getSockaddrLength()
				  ) == -1){
				perror("Could not send a SIP packet through UDP:");
			}
#endif
			if( udpsock.sendTo( ip_addr, port, 
					(void*)packet_string.c_str(),
					packet_string.length() ) == -1 ){
			
				throw new SendFailed( errno );
			
			}; 
	
		} else if (transport=="TCP"){
			TCPSocket * tcpsock;
			addSocket(tcpsock = new TCPSocket(ip_addr, port));
			if( pack->getType() != SipResponse::type ){
				MRef<SipHeaderValue*> hval = new SipHeaderValueVia(transport, localIP, localTCPPort, branch);
				MRef<SipHeader*> hdr = new SipHeader(hval);
				pack->addHeader(hdr);
				//pack->addHeader(MRef<SipHeader*>(new SipHeaderVia(transport, localIP, localTCPPort, branch))); //FIXME: WARNING: if we are behind NAT TCP will not get any incoming connections--EE
			}
			ts.save( PACKET_OUT );
#ifdef DEBUG_OUTPUT
			printMessage("OUT (TCP)", pack->getString());
#endif
			if( tcpsock->write(pack->getString()) == -1 ){
				throw new SendFailed( errno );
			}

			merr << "Created new TCP Socket" << end;
			socket = tcpsock;
#ifndef NO_SECURITY
		} else if (transport=="TLS"){
			TLSSocket * tlssock;
			ts.save( TLS_START );
			tlssock = new TLSSocket(ip_addr, 5061/*FIXME*/,tls_ctx, cert, cert_db);
			ts.save( TLS_END );
			socket = tlssock; //socket is a reference
			
			addSocket( tlssock );
			if( pack->getType() != SipResponse::type ){     //WARNING: If behind NAT, TLS will not get any incoming connections.
				pack->addHeader(new SipHeader(new SipHeaderValueVia("TLS"/*SER fix "TCP"*/, localIP, localTLSPort, branch)));
			}
			merr << "Created new TLS Socket" << end;
			ts.save( PACKET_OUT );
#ifdef DEBUG_OUTPUT
			printMessage("OUT (TLS)", pack->getString());
#endif
			if( tlssock->write(pack->getString()) == -1 ){
				throw new SendFailed( errno );
			}
#endif //NO_SECURITY
		}else{
			merr << "SipMessageTransport: ERROR: unimplemented transport protocol: "<< transport << end;
			socket = NULL;
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

		
//		commandReceiver->enqueueCommand( transportErrorCommand );
		if (! commandReceiver.isNull())
			commandReceiver->handleCommand( transportErrorCommand );
		else
			merr<< "SipMessageTransport: ERROR: NO SIP COMMAND RECEIVER - DROPPING COMMAND"<<end;
	}
	
}

//Use a previously created socket
void SipMessageTransport::sendMessage(MRef<SipMessage*> pack, StreamSocket * socket, string branch){
	assert(socket != NULL);
	
	if( pack->getType() != SipResponse::type ){
		if( socket->getType() == SOCKET_TYPE_TCP ){
			pack->addHeader(MRef<SipHeader*>(new SipHeader(new SipHeaderValueVia("TCP", contactIP, localTCPPort, branch))));
		}else{
			pack->addHeader(MRef<SipHeader*>(new SipHeader(new SipHeaderValueVia("TCP"/*SER fix "TCP"*/, contactIP, localTLSPort, branch))));
		}
	}

	ts.save( PACKET_OUT );
	socket->write(pack->getString());
}

void SipMessageTransport::addSocket(StreamSocket * sock){
	this->socks.push_back(sock);
/*
	struct sembuf operations[1];
	operations[0].sem_num = 0;
	operations[0].sem_op = 1;
	operations[0].sem_flg = 0;

	semop(semaphore, operations, 1 );
*/
        semaphore.inc();
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
//	struct sockaddr_in from;                        //FIXME: This should be handled by netutil --EE
//	int32_t fromlen = sizeof(from);
    	fd_set set;
	
	while( true ){
		//p[0].fd = udpsock.getFd();
		//p[0].events = POLLIN;
	        FD_ZERO(&set);
        	FD_SET(udpsock.getFd(), &set);

		do{
            		avail = select(udpsock.getFd()+1,&set,NULL,NULL,NULL );
			//avail = poll( p, 1, -1);
		} while( avail < 0 );

		//if( p[0].revents != 0 ){
		if( FD_ISSET( udpsock.getFd(), &set )){
//#ifndef WIN32
			//nread = recvfrom(udpsock.getFd(), buffer, 16384, 0, (struct sockaddr *)&from, (socklen_t *)&fromlen);
			nread = udpsock.recv(buffer, 16384);
//#else
//			nread = recvfrom(udpsock.getFd(), buffer, 16384, 0, (struct sockaddr *)&from, (int *)&fromlen);
//#endif
			
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

/*
                struct sembuf operations[1];
		operations[0].sem_num = 0;
		operations[0].sem_op = -1;
		operations[0].sem_flg = 0;
		
		semop(semaphore, operations, 1);
*/
                semaphore.dec();
                
		//pthread_mutex_lock( socks_lock );
                socks_lock.lock();
                
		socket = socks.front();
		socks.pop_front();
		
                //pthread_mutex_unlock( socks_lock );
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

