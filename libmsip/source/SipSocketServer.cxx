/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2005  Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include<config.h>

#include<libmnetutil/NetworkException.h>
#include<libmnetutil/DatagramSocket.h>
#include<libmsip/SipSocketServer.h>
#include<libmsip/SipLayerTransport.h>

using namespace std;

// 
// SipSocketServer
// 


SipSocketServer::SipSocketServer(MRef<SipLayerTransport*> r, MRef<Socket*> sock): ssock(sock), receiver(r),doStop(false){
	externalPort = ssock->getPort();
}

SipSocketServer::~SipSocketServer(){
}

bool SipSocketServer::isIpv6() const{
	return ssock->getLocalAddress()->getType() == IP_ADDRESS_TYPE_V6;
}

int32_t SipSocketServer::getType() const{
	return ssock->getType();
}

MRef<Socket *> SipSocketServer::getSocket() const {
	return ssock;
}

MRef<SipLayerTransport *> SipSocketServer::getReceiver() const {
	return receiver;
}

void SipSocketServer::run(){
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
		if (avail==0){
// 			cerr<< "SipSocketServer::run(): Timeout"<< endl;
		}
		MRef<SipLayerTransport *> r = receiver;
		if (avail && !doStop && r){
			inputReady();
		}

	}

	cerr << "SipSocketServer stopped" << endl;
} // "myself" will be freed here and the object can be freed.

void SipSocketServer::start(){
	Thread t(this);
}

void SipSocketServer::stop(){
	doStop=true;
}

void SipSocketServer::inputReady(){
}


// 
// StreamSocketServer
// 
StreamSocketServer::StreamSocketServer(MRef<SipLayerTransport*> r, MRef<ServerSocket*> sock): SipSocketServer(r, *sock){
}

void StreamSocketServer::inputReady(){
	MRef<SipLayerTransport *> r = getReceiver();
	MRef<Socket *> sock = getSocket();
	if (r && sock){
		MRef<ServerSocket *> ssock = (ServerSocket*)*sock;
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


// 
// DatagramSocketServer
// 

DatagramSocketServer::DatagramSocketServer(MRef<SipLayerTransport*> r, MRef<DatagramSocket*> sock): SipSocketServer(r, *sock){
}

void DatagramSocketServer::inputReady(){
	MRef<SipLayerTransport *> transport = getReceiver();
	MRef<Socket *> sock = getSocket();
	if (transport && sock){
		MRef<DatagramSocket *> dsock = (DatagramSocket*)*sock;

		transport->datagramSocketRead(dsock);
	}
}
