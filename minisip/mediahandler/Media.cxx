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

Media::Media( MRef<Codec *> codec ):codec(codec){
}

uint8_t Media::getRtpPayloadType(){
	if( codec ){
		return codec->getSdpMediaType();
	}
	
	return 0;
}

string Media::getRtpMap(){
	if( codec ){
		return codec->getSdpMediaAttributes();
	}
	return "";
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
		(*i)->send( data, length, ts, marker );
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

