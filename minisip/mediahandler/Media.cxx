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
#include"Media.h"
#include"../codecs/Codec.h"
#include"../soundcard/SoundIO.h"
#include"../minisip/ipprovider/IpProvider.h"
#include"MediaStream.h"
#include"RtpReceiver.h"
#include<libmutil/itoa.h>
#include"../sdp/SdpHeaderM.h"
#include"../sdp/SdpHeaderA.h"

#ifdef VIDEO_SUPPORT
#include"../video/codec/VideoCodec.h"
#include"../video/codec/VideoEncoderCallback.h"
#include"../video/grabber/Grabber.h"
#include"../video/display/VideoDisplay.h"
#include"../video/mixer/ImageMixer.h"
#endif
#include<libmutil/print_hex.h>


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
Media::Media( std::list<MRef<Codec *> > codecListing, MRef<Codec *> defaultCodec ){
	codecList = codecListing;
	//selectedCodec = defaultCodec;
}

#if 0
// pn430 Renamed and rewritten for multicodec
uint8_t Media::getCurrentRtpPayloadType(){
	// pn430 Changed for multicodec
	//if( codec ){
	if( selectedCodec ){
		// pn430 Changed for multicodec
		//return codec->getSdpMediaType();
		return selectedCodec->getSdpMediaType();
	}
	
	return 0;
}
#endif
// pn430 Added for multicodec
std::list<uint8_t> Media::getAllRtpPayloadTypes(){
	std::list<uint8_t> list;
	
    std::list< MRef<Codec *> >::iterator iCodec;

    for( iCodec = codecList.begin(); iCodec != codecList.end(); iCodec ++ ){
        list.push_back((*iCodec)->getSdpMediaType());
    }

    return list;
}

#if 0
// pn430 Renamed and rewritten for multicodec
string Media::getCurrentRtpMap(){
	// pn430 Changed for multicodec
	//if( codec ){
	if( selectedCodec ){
		// pn430 Changed for multicodec
		//return codec->getSdpMediaAttributes();
		return selectedCodec->getSdpMediaAttributes();
	}
	
	return "";
}
#endif
// pn430 Added for multicodec
std::list<std::string> Media::getAllRtpMaps(){
	std::list<std::string> list;
	
    std::list< MRef<Codec *> >::iterator iCodec;

    for( iCodec = codecList.begin(); iCodec != codecList.end(); iCodec ++ ){
        list.push_back((*iCodec)->getSdpMediaAttributes());
    }

    return list;

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
	// FIXME! This one should be flexible enough for video
	//

	sendersLock.lock();
	for( i = senders.begin(); i != senders.end(); i++ ){
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

