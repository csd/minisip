/*
 Copyright (C) 2004-2006 the Minisip Team

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include<libminisip/mediahandler/RtpReceiver.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/NetworkException.h>
#include<libmutil/Thread.h>

#include<libminisip/rtp/SRtpPacket.h>
#include<libminisip/codecs/Codec.h>
#include<iostream>
#include<libminisip/mediahandler/MediaStream.h>
#include<libminisip/ipprovider/IpProvider.h>

#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h> //for rand

#ifdef WIN32
#include<winsock2.h>
#endif

#ifdef _MSC_VER

#else
#include<sys/time.h>
#include<unistd.h>
#include<errno.h>
#endif

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

//Set the range of rtp usable ports ... starting at min and spanning a range, up to max
#define RTP_LOCAL_PORT_RANGE_MIN 30000
#define RTP_LOCAL_PORT_RANGE 5000
#define RTP_LOCAL_PORT_RANGE_MAX RTP_LOCAL_PORT_RANGE_MIN + RTP_LOCAL_PORT_RANGE
#define RTP_RECEIVER_MAX_RETRIES 5


using namespace std;

RtpReceiver::RtpReceiver( MRef<IpProvider *> ipProvider){

	socket = NULL;

	string externalIp = ipProvider->getExternalIp();
	bool useIPv6;

	if( externalIp.find(':') == string::npos )
		useIPv6 = false;
	else
		useIPv6 = true;
	
	int portretry = 0;
	for (; portretry<RTP_RECEIVER_MAX_RETRIES; portretry++ ) {
		//generate a random port, even number, in the given range
		float randPartial =  (float)rand()  /  RAND_MAX;
		int port = (int) (RTP_LOCAL_PORT_RANGE * randPartial );
		port = (int) ( 2 * (int)(port/2 ) ); //turn this into an even number
		port += RTP_LOCAL_PORT_RANGE_MIN; //add the min port to set it within the range
		#ifdef DEBUG_OUTPUT
			printf( "RtpReceiver:: final trying port = %d\n", port );
		#endif
		try{
			socket = new UDPSocket( port, useIPv6 );
			if( socket ) {
				break;
			}
		}
		catch( NetworkException &  ){
			// FIXME: do something nice
// 			merr << "Minisip could not create a UDP socket!" << end;
// 			merr << "Check your network settings." << end;
// 			exit( 1 );
			#ifdef DEBUG_OUTPUT
			cerr << "RtpReceiver: Could not create UDP socket" << endl;
			#endif
		}
	}
	if( portretry == RTP_RECEIVER_MAX_RETRIES && !socket ) {
			merr << "Minisip could not create a UDP socket!" << end;
			merr << "Check your network settings." << end << "Quitting badly" << end;
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

/**
	register a mediaStreamReceiver ... if the receiver is already registered,
	update the exhisting one to point to the new receiver.
*/
void RtpReceiver::registerMediaStream( MRef<MediaStreamReceiver *> mediaStream ){
	list< MRef<MediaStreamReceiver *> >::iterator iter;
	mediaStreamsLock.lock();
	/* Don't register new streams if the receiver is being closed */
	if( !kill ){
		bool found = false;
		//cerr << "RtpReceiver::registerMediaStream: register done!" << endl;
		for( iter = mediaStreams.begin();
				iter != mediaStreams.end();
				iter++ ) {
			if( (*iter)->getId() == mediaStream->getId() ) {
				found = true;
			#ifdef DEBUG_OUTPUT
				cerr << "RtpRcvr::registerMediaStream: media stream already registered. Updating MRef." << endl;
			#endif
				(*iter) = mediaStream;
				break;
			}
		}
		if( !found ) {
			mediaStreams.push_back( mediaStream );
		}
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

void RtpReceiver::stop(){
	kill=true;
}

void RtpReceiver::join(){
	thread->join();
}

void RtpReceiver::run(){
	MRef<SRtpPacket *> packet;

	while( !kill ){
		list< MRef<MediaStreamReceiver *> >::iterator i;
		fd_set rfds;
		struct timeval tv;
		int ret = -1;

		FD_ZERO( &rfds );
		#ifdef WIN32
		FD_SET( (uint32_t) socket->getFd(), &rfds );
		#else
		FD_SET( socket->getFd(), &rfds );
		#endif

		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		while( !kill && ret < 0 ){
			ret = select( socket->getFd() + 1, &rfds, NULL, NULL, &tv );
			if( ret < 0 ){
				#ifdef DEBUG_OUTPUT
					//FIXME: do something better
					cerr << "RtpReceiver::run() - select returned -1" << endl;
				#endif

				#ifndef _WIN32_WCE
					if( errno == EINTR ) { continue; }
				#else
					if( errno == WSAEINTR ) { continue; }
				#endif
					else {
						kill = true;
						break;
					}
			}
		}

		if( kill ) {
			break;
		}

		if( ret == 0 /* timeout */ ){
			//notify the mediaStreams of the timeout
			for( i = mediaStreams.begin();
					i != mediaStreams.end(); i++ ){
				(*i)->handleRtpPacket( NULL, NULL );
			}
			continue;
		}
		MRef<IPAddress *> from = NULL;
		try{
		    packet = SRtpPacket::readPacket( **socket, from);
		}

		catch (NetworkException &  ){
			continue;
		}

		if( !packet ){
			continue;
		}

		mediaStreamsLock.lock();
		for ( i = mediaStreams.begin(); i != mediaStreams.end(); i++ ) {
                    std::list<MRef<Codec *> > codecs = (*i)->getAvailableCodecs();
                    std::list<MRef<Codec *> >::iterator iC;
                    int found = 0;
			//printf( "|" );
                    for( iC = codecs.begin(); iC != codecs.end(); iC ++ ){
                        if ( (*iC)->getSdpMediaType() == packet->getHeader().getPayloadType() ) {
                            (*i)->handleRtpPacket( packet, from );
                            found = 1;
                            //printf( "~" );
                            break;
                        }
                    }
#ifdef ZRTP_SUPPORT
                    /*
                     * If we come to this point:
                     * no codec was found for this packet.
                     */
                    MRef<ZrtpHostBridgeMinisip *>zhb = (*i)->getZrtpHostBridge();

                    /*
                     * If the packet was not processed above and it contains an
                     * extension header then check for ZRTP packet.
                    */
                    if (!found && zhb && packet->getHeader().getExtension()) {
                        (*i)->handleRtpPacketExt(packet);
                    }
#endif // ZRTP_SUPPORT
                }
                mediaStreamsLock.unlock();

                packet = NULL;
	}
	socket=NULL;
}
