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

#include<libminisip/mediahandler/Media.h>

#include<libminisip/codecs/Codec.h>
#include<libminisip/soundcard/SoundIO.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libminisip/mediahandler/MediaStream.h>
#include<libminisip/mediahandler/RtpReceiver.h>
#include<libmutil/itoa.h>
#include<libminisip/sdp/SdpHeaderM.h>
#include<libminisip/sdp/SdpHeaderA.h>

#include<libmutil/print_hex.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#include"AudioPlugin.h"

using namespace std;

Media::Media(){
}
// pn430 Function rewritten for multicodec
//Media::Media( MRef<Codec *> codec ):codec(codec){
//}
Media::Media( MRef<Codec *> codec ){
	codecList.push_back(codec);
	//selectedCodec = codec;
}

// pn430 Function added for multicodec
Media::Media( std::list<MRef<Codec *> > codecListing ){
	codecList = codecListing;
	//selectedCodec = defaultCodec;
}


// pn507 Added for being able to change the current codec
// pn507 NOTE Using this during a conference call will most likely cause complete havoc.
MRef<Codec *> Media::getCodec( uint8_t payloadType ){
	std::list< MRef<Codec *> >::iterator iCodec;
	
	for( iCodec = codecList.begin(); iCodec != codecList.end(); iCodec ++ ){
		if ( (*iCodec)->getSdpMediaType() == payloadType ) {
//			selectedCodec = (*iCodec);
			return *iCodec;
		}
	}
	
	return NULL;
}

std::list< MRef<Codec *> >  Media::getAvailableCodecs(){
	list<MRef<Codec *> > copy;
	codecListLock.lock();
	copy = codecList;
	codecListLock.unlock();
	return copy;
}

void Media::registerMediaSender( MRef<MediaStreamSender *> sender ){
	sendersLock.lock();
	senders.push_back( sender );
	sendersLock.unlock();
}

void Media::unRegisterMediaSender( MRef<MediaStreamSender *> sender ){
	sendersLock.lock();
	senders.remove( sender );
	sendersLock.unlock();
}

void Media::registerMediaSource( uint32_t ssrc ){
}

void Media::unRegisterMediaSource( uint32_t ssrc ){
}

void Media::sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker ){
	list< MRef<MediaStreamSender *> >::iterator i;
	sendersLock.lock();
	for( i = senders.begin(); i != senders.end(); i++ ){
		//only send if active sender, or if muted only if keep-alive
		if( (*i)->isMuted () ) {
			if( (*i)->muteKeepAlive( 50 ) ) { //send one packet of every 50
				memset( data, 0, length );
			} else {
				continue;
			}
		}
		(*i)->send( data, length, &ts, marker );
	}
	sendersLock.unlock();
}

list<string> Media::getSdpAttributes(){
	return sdpAttributes;
}

void Media::addSdpAttribute( string attribute ){
	sdpAttributes.push_back( attribute );
}

void Media::handleMHeader( MRef< SdpHeaderM * > m ){
}

MRef<CodecState *> Media::createCodecInstance( uint8_t payloadType ){
	std::list< MRef<Codec *> >::iterator iC;

	for( iC = codecList.begin(); iC != codecList.end(); iC ++ ){
		if( (*iC)->getSdpMediaType() == payloadType ){
			return (*iC)->newInstance();
		}
	}
	return NULL;
}


MediaPlugin::MediaPlugin( MRef<Library*> lib ): MPlugin( lib ){
}

MediaPlugin::~MediaPlugin(){
}


MediaRegistry::MediaRegistry(){
	registerPlugin( new AudioPlugin( NULL ) );
}

std::list< MRef<MPlugin*> >::const_iterator MediaRegistry::begin() const{
	return plugins.begin();
}

std::list< MRef<MPlugin*> >::const_iterator MediaRegistry::end() const{
	return plugins.end();
}
