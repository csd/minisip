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

void Media::registerMediaReceiver( MRef<MediaStreamReceiver *> receiver ){
	receiversLock.lock();
	receivers.push_back( receiver );
	receiversLock.unlock();
}

void Media::unRegisterMediaReceiver( MRef<MediaStreamReceiver *> receiver ){
	receiversLock.lock();
	receivers.remove( receiver );
	receiversLock.unlock();
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

AudioMedia::AudioMedia( MRef<SoundIO *> soundIo, MRef<Codec *> codec ):
		Media(codec),
		soundIo(soundIo){
	// for audio media, we assume that we can both send and receive
	receive = true;
	send = true;

	soundIo->register_recorder_receiver( this, ((AudioCodec *)*codec)->getInputNrSamples(), false );

	seqNo = 0;
#ifdef IPAQ
	iIPAQ = 0;
#endif
}

string AudioMedia::getSdpMediaType(){
	return "audio";
}

void AudioMedia::registerMediaSender( MRef<MediaStreamSender *> sender ){
	sendersLock.lock();
	if( senders.empty() ){
		sendersLock.unlock();
		soundIo->startRecord();
		sendersLock.lock();
	}

	senders.push_back( sender );
	sendersLock.unlock();
}

void AudioMedia::unRegisterMediaSender( MRef<MediaStreamSender *> sender ){
	bool emptyList;
	sendersLock.lock();
	senders.remove( sender );
	emptyList = senders.empty();
	sendersLock.unlock();


	if(  emptyList ){
		soundIo->stopRecord();
	}
}

void AudioMedia::registerMediaReceiver( MRef<MediaStreamReceiver *> receiver ){
	Media::registerMediaReceiver( receiver );
	soundIo->registerSource( receiver->getId() );
}

void AudioMedia::unRegisterMediaReceiver( MRef<MediaStreamReceiver *> receiver ){
	Media::unRegisterMediaReceiver( receiver );

	soundIo->unRegisterSource( receiver->getId() );
}

void AudioMedia::playData( uint32_t receiverId, byte_t * data, uint32_t length, uint32_t ssrc, uint16_t seqNo, bool marker, uint32_t ts ){
	short output[1600];

	if( length == (uint32_t) ((AudioCodec *)*codec)->getEncodedNrBytes() ){
		((AudioCodec *)*codec)->decode( data, length, output );
	}

#ifdef IPAQ
	// The iPAQ doen't support 8kHz, we need to resample to 16kHz
	short buffer1[1600];
	short buffer2[1600];
	int i;
	for( int i =0; i < 80; i++ ){
		buffer1[2*i] = output[i]/32; // /32 to reduce the output volume ...
		buffer1[2*i+1] = output[i]/32; // /32 to reduce the output volume ...
	}
	for( int i =0; i < 80; i++ ){
		buffer2[2*i] = output[80+i]/32; // /32 to reduce the output volume ...
		buffer2[2*i+1] = output[80+i]/32; // /32 to reduce the output volume ...
	}
	
	soundIo->pushSound( receiverId, buffer1, 
		((AudioCodec*)*codec)->getInputNrSamples(), seqNo );
	
	soundIo->pushSound( receiverId, buffer2, 
		((AudioCodec*)*codec)->getInputNrSamples(), seqNo );
#else

	soundIo->pushSound( receiverId, output, 
		((AudioCodec*)*codec)->getInputNrSamples(), seqNo );
		
#endif
}

void AudioMedia::srcb_handleSound( void * data ){
	byte_t encoded[1600];

#ifdef IPAQ
	uint32_t i;
	short sent[160];
	// The iPAQ does not support 8kHz, so we use 16kHz and resample
	if( iIPAQ % 2 ){
		// save the sample
		memcpy( saved, data, 160*sizeof(short) );
		iIPAQ ++;
		return;
	}
	else{
		// mix with the old sample
		for( i = 0; i < 80; i++ ){
			sent[i] = saved[i*2];
		}
		for( i = 0; i < 80; i++ ){
			sent[80+i] = ((short *)data)[i*2];
		}
		data = (void *)sent;
		iIPAQ ++;
	}
			
#endif

	((AudioCodec *)*codec)->encode( data, ((AudioCodec*)*codec)->getInputNrSamples()*sizeof(short), encoded );
	uint32_t encodedLength = ((AudioCodec *)*codec)->getEncodedNrBytes();


	
	sendData( encoded, encodedLength, seqNo * ((AudioCodec*)*codec)->getInputNrSamples() );
	seqNo ++;
}

#ifdef VIDEO_SUPPORT

VideoMedia::VideoMedia( MRef<VideoCodec *> codec, MRef<VideoDisplay *> display, MRef<Grabber *> grabber, uint32_t receivingWidth, uint32_t receivingHeight ):
		Media( *codec ),display(display),grabber(grabber),receivingWidth(receivingWidth),receivingHeight(receivingHeight){
	
	receive = true;
	send = (!grabber.isNull());
	index = 0;
        packetLoss = false;
        expectedSeqNo = 0;
	codec->setDisplay( display );
	codec->setGrabber( grabber );
	codec->setEncoderCallback( this );
	sendingWidth = 176;
	sendingHeight = 144;

	addSdpAttribute( "framesize:34 " + itoa( receivingWidth ) + "-" + itoa( receivingHeight ) );
}


string VideoMedia::getSdpMediaType(){

	return "video";
}

void VideoMedia::handleMHeader( MRef< SdpHeaderM * > m ){
	string framesizeString = m->getAttribute( "framesize", 0 );
	cerr << "FRAMESIZE" << framesizeString << endl;
	if( framesizeString != "" ){
		size_t space = framesizeString.find( " " );
		size_t coma = framesizeString.find( "-" );

		if( space != string::npos && coma != string::npos ){
			string widthString = framesizeString.substr( space+1,coma );
			string heightString = framesizeString.substr( coma+1, framesizeString.size() );

			sendingWidth = atoi( widthString.c_str() );
			sendingHeight = atoi( heightString.c_str() );
		}
	}
}

void VideoMedia::playData( uint32_t receiverId, byte_t * data, uint32_t length, uint32_t ssrc, uint16_t seqNo, bool marker, uint32_t ts ){
	
        if( seqNo != expectedSeqNo + 1 ){
                packetLoss = true;
#ifdef DEBUG_OUTPUT
                merr << "Packet lost in video stream, dropping one frame" << end;
#endif
        }

        expectedSeqNo = seqNo;

        if( !packetLoss ){
	        memcpy( frame + index, data + 4, length - 4 );
	        index += length - 4 ;
        
        }
        
        if( marker ){
                if( ! packetLoss ){
                        /* We have a frame */
                        ((VideoCodec *)*codec)->decode( frame, index );
                }
                index = 0;
                packetLoss = false;
        }
}

void VideoMedia::sendVideoData( byte_t * data, uint32_t length, uint32_t ts, bool marker ){
	Media::sendData( data, length, ts, marker );
}

void VideoMedia::registerMediaReceiver( MRef<MediaStreamReceiver *> receiver ){
	receiversLock.lock();
	if( receivers.size() == 0 ){
		receiversLock.unlock();
		((VideoCodec *)*codec)->startReceive( receivingWidth, receivingHeight );
		receiversLock.lock();
	}

	receivers.push_back( receiver );
	receiversLock.unlock();
}

void VideoMedia::unRegisterMediaReceiver( MRef<MediaStreamReceiver *> receiver ){
	receiversLock.lock();
	receivers.remove( receiver );
	receiversLock.unlock();

	if( receivers.size() == 0 ){
		((VideoCodec *)*codec)->stopReceive();
	}
}

void VideoMedia::registerMediaSender( MRef<MediaStreamSender *> sender ){
	sendersLock.lock();
	if( senders.size() == 0 ){
		sendersLock.unlock();
		((VideoCodec *)*codec)->startSend( sendingWidth, sendingHeight );
		sendersLock.lock();
	}

	senders.push_back( sender );
	sendersLock.unlock();
}

void VideoMedia::unRegisterMediaSender( MRef<MediaStreamSender *> sender ){
	sendersLock.lock();
	senders.remove( sender );
	sendersLock.unlock();

	if( senders.size() == 0 ){
		((VideoCodec *)*codec)->stopSend();
	}
}




#endif
