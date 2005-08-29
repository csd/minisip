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

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include"RtpReceiver.h"
#include"MediaStream.h"
#include"../minisip/ipprovider/IpProvider.h"
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/UDPSocket.h>
#include<libmutil/Thread.h>
#include"../rtp/SRtpPacket.h"
#include"../codecs/Codec.h"
#include<iostream>

#include<stdio.h>
#include<sys/types.h>

#ifdef WIN32
#include<winsock2.h>
#endif

#ifdef _MSC_VER

#else
#include<sys/time.h>
#include<unistd.h>
#endif

using namespace std;

RtpReceiver::RtpReceiver( MRef<IpProvider *> ipProvider){
	try{
		socket = new UDPSocket();
	}
	catch( NetworkException * exc ){
		// FIXME: do something nice
		merr << "Minisip could not create a UDP socket!" << end;
		merr << "Check your network settings." << end;
		exit( 1 );
	}
                
        externalPort = ipProvider->getExternalPort( socket );

	kill = false;

	thread = new Thread(this);
}

RtpReceiver::~RtpReceiver(){
        thread->join();
        delete thread;
	socket->close();
}

void RtpReceiver::registerMediaStream( MRef<MediaStreamReceiver *> mediaStream ){
	mediaStreamsLock.lock();
	/* Don't register new streams if the receiver is being closed */
	if( !kill ){
		mediaStreams.push_back( mediaStream );
	}
	mediaStreamsLock.unlock();
}

void RtpReceiver::unregisterMediaStream( MRef<MediaStreamReceiver *> mediaStream ){
#ifdef DEBUG_OUTPUT
	// cerr << "RtpReceiver::unregisterMediaStream: Before taking lock" << endl;
#endif	
	mediaStreamsLock.lock();
	mediaStreams.remove( mediaStream );
	if( mediaStreams.size() == 0 ){
		/* End the thread */
		kill = true;
	}
	mediaStreamsLock.unlock();
#ifdef DEBUG_OUTPUT
	// cerr << "RtpReceiver::unregisterMediaStream: After taking lock" << endl;
#endif
}

uint16_t RtpReceiver::getPort(){
	return externalPort;
}

MRef<UDPSocket *> RtpReceiver::getSocket(){
	return socket;
}
			
void RtpReceiver::run(){
	MRef<SRtpPacket *> packet;
	
	while( !kill ){
		list< MRef<MediaStreamReceiver *> >::iterator i;
		fd_set rfds;
		struct timeval tv;
		int ret = -1;

		FD_ZERO( &rfds );
		FD_SET( socket->getFd(), &rfds );

		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		
		while( ret < 0 ){
			ret = select( socket->getFd() + 1, &rfds, NULL, NULL, &tv );
			/*if( ret < 0 ){
				FIXME: do something
			}*/

		}

		if( ret == 0 /* timeout */ ){
			continue;
		}

		try{
			packet = SRtpPacket::readPacket( **socket );
		}

		catch (NetworkException * exc ){
			delete exc;
			continue;
		}

		if( !packet ){
			continue;
		}

		mediaStreamsLock.lock();
		
		for( i = mediaStreams.begin(); i != mediaStreams.end(); i++ ){
			std::list<MRef<Codec *> > codecs = (*i)->getAvailableCodecs();
			std::list<MRef<Codec *> >::iterator iC;
			
			for( iC = codecs.begin(); iC != codecs.end(); iC ++ ){
				if ( (*iC)->getSdpMediaType() == packet->getHeader().getPayloadType() ) {
					(*i)->handleRtpPacket( packet );
					break;
				}
			}
		}
		
		mediaStreamsLock.unlock();

//		delete packet;
	}
}
