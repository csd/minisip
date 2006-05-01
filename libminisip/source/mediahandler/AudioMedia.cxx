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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include<libminisip/mediahandler/AudioMedia.h>

#include<libminisip/rtp/RtpHeader.h>
#include<libminisip/mediahandler/MediaStream.h>
#include<libminisip/soundcard/FileSoundSource.h>

#include<libminisip/soundcard/Resampler.h>
#include<libminisip/soundcard/SoundSource.h>

#include<libminisip/rtp/RtpPacket.h>

#define RINGTONE_SOURCE_ID 0x42124212

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>

#ifdef _MSC_VER

#else
#include<sys/time.h>
#include<unistd.h>
#endif

#include<string.h> //for memset

#ifdef DEBUG_OUTPUT
#include <libmutil/itoa.h>
#endif

class G711CODEC;
#ifdef AEC_SUPPORT
AEC AudioMedia::aec;		//hanning
#endif

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

// pn430 Parameter list changed for multicodec
//AudioMedia::AudioMedia( MRef<SoundIO *> soundIo, MRef<Codec *> codec ):
//                Media(codec),
//                soundIo(soundIo){
AudioMedia::AudioMedia( MRef<SoundIO *> soundIo, 
			std::list<MRef<Codec *> > codecList ):
							Media(codecList),
							soundIo(soundIo){
						
	// for audio media, we assume that we can both send and receive
	receive = true;
	send = true;
	// pn430 Changed for multicodec
	//MRef<AudioCodec *> acodec = ((AudioCodec *)*codec);
	
	// NOTE Frame size FIXED to 20 ms
	soundIo->register_recorder_receiver( this, SOUND_CARD_FREQ * 20 / 1000, false );

	seqNo = 0;
	
	// NOTE Sampling frequency FIXED to 8000 Hz
	resampler = ResamplerRegistry::getInstance()->create( SOUND_CARD_FREQ, 8000, 20, 1 /*Nb channels */);
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

	if( emptyList ){
		soundIo->stopRecord();
	}
}

void AudioMedia::registerMediaSource( uint32_t ssrc ){
	MRef<AudioMediaSource *> source;

	source = new AudioMediaSource( ssrc, this );
	//cerr << "AudioMedia::registerMediaSource" << endl;
	soundIo->registerSource( *source );
	sources.push_back( source );
}

void AudioMedia::unRegisterMediaSource( uint32_t ssrc ){
	std::list< MRef<AudioMediaSource *> >::iterator iSource;
	//cerr << "AudioMedia::unRegisterMediaSource" << endl;

	soundIo->unRegisterSource( ssrc );

	for( iSource = sources.begin(); iSource != sources.end(); iSource ++ ){
		if( (*iSource)->getSsrc() == ssrc ){
			sources.erase( iSource );
			return;
		}
	}
}

void AudioMedia::playData( MRef<RtpPacket *> packet ){
	MRef<AudioMediaSource *> source = getSource( packet->getHeader().SSRC );

	if( source ){
		source->playData( packet );
	}
        //delete packet;

}

//this function is called from SoundIO::recorderLoop
//the void *data is originally a short *
void AudioMedia::srcb_handleSound( void * data, int length){
	resampler->resample( (short *)data, resampledData );
	sendData( (byte_t*) &resampledData, 160*sizeof(short), 0, false );
	seqNo ++;
}

#ifdef AEC_SUPPORT
void AudioMedia::srcb_handleSound( void * data, int length, void * dataR){				//hanning
	resampler->resample( (short *)data, resampledData );
	resampler->resample( (short *)dataR, resampledDataR );

	for(int j=0; j<160; j++){
		resampledData[j] = (short)aec.doAEC((int)resampledData[j], (int)resampledDataR[j]);
	}
	sendData( (byte_t*) &resampledData, 160*sizeof(short), 0, false );
	seqNo ++;
}
#endif

void AudioMedia::sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker ){

	//all these zerodata vars are used to send silence to muted senders
	//we need a silence data vector, as we cannot simply erase the data vector, 
	//as it is shared by all media stream senders ... 
	static byte_t zeroData[160*sizeof(short)]; //this needs to be of _length_ length
	static bool zeroDataInit = false;
	bool encodeZeroData;
	
	uint32_t encodedLength;
	
	if( ! zeroDataInit ) {
		zeroDataInit = true;
		for( uint32_t i = 0; i<length; i++ ) zeroData[i] = 0; 
	}
	
	list< MRef<MediaStreamSender *> >::iterator i;
	sendersLock.lock();

	for( i = senders.begin(); i != senders.end(); i++ ){
		uint32_t givenTs;
		givenTs = ts;
		encodeZeroData = false;
		//only send if active sender, or if muted only if keep-alive
		if( (*i)->isMuted () ) {
			if( (*i)->muteKeepAlive( 50 ) ) {
				encodeZeroData = true;
				//cerr << endl << "AudioMedia::sendData - sending silence sample!" << endl;
			} else {
				(*i)->increaseLastTs(); //update the lastTimeStamp ... even we don't send, we must ...
				continue;
			}
		}
		
		MRef<CodecState *> selectedCodec = (*(*i)->getSelectedCodec());
			
		//if we must send silence (zeroData), encode it ... 
		if( encodeZeroData ) {
			encodedLength = selectedCodec->encode( &zeroData, length, encoded );
		} else {
			encodedLength = selectedCodec->encode( data, length, encoded );
		}
	
		(*i)->send( encoded, encodedLength, &givenTs, marker );
	}
	
	sendersLock.unlock();
}

void AudioMedia::startRinging( string ringtoneFile ){
	soundIo->registerSource( new FileSoundSource( ringtoneFile,RINGTONE_SOURCE_ID, 44100, 2, SOUND_CARD_FREQ, 20, 2, true ) );
}

void AudioMedia::stopRinging(){
	soundIo->unRegisterSource( RINGTONE_SOURCE_ID );
}

#ifdef DEBUG_OUTPUT
string AudioMedia::getDebugString() {
	string ret;
	ret = getMemObjectType() + ": this=" + itoa((int64_t)this);
	for( std::list< MRef<MediaStreamSender *> >::iterator it = senders.begin();
				it != senders.end(); it++ ) {
		ret += (*it)->getDebugString() + ";";
	}
	return ret;
}
#endif

MRef<AudioMediaSource *> AudioMedia::getSource( uint32_t ssrc ){
	std::list<MRef<AudioMediaSource *> >::iterator i;

	for( i = sources.begin(); i != sources.end(); i++ ){
		if( (*i)->getSsrc() == ssrc ){
			return (*i);
		}
	}
	return NULL;
}


AudioMediaSource::AudioMediaSource( uint32_t ssrc, MRef<Media *> media ):
	BasicSoundSource( ssrc, 
			NULL, //plc
			0/*position*/, 
			SOUND_CARD_FREQ, 
			20, //duration in ms
			2 //number of channels (numChannels)
			//buffer size defaults to 16000 * numChannels
			),
	media(media),
	ssrc(ssrc)
{
}

void AudioMediaSource::playData( MRef<RtpPacket *> rtpPacket ){
        RtpHeader hdr = rtpPacket->getHeader();
	MRef<CodecState *> codec = findCodec( hdr.getPayloadType() );

	if( codec ){
		uint32_t outputSize = codec->decode( rtpPacket->getContent(), rtpPacket->getContentLength(), codecOutput );

		pushSound( codecOutput,
			outputSize, hdr.getSeqNo() );
		
        }
}

MRef<CodecState *> AudioMediaSource::findCodec( uint8_t payloadType ){
	std::list< MRef<CodecState *> >::iterator iCodec;
	MRef<CodecState *> newCodecInstance;

	for( iCodec = codecs.begin(); iCodec != codecs.end(); iCodec ++ ){
		if( (*iCodec)->getSdpMediaType() == payloadType ){
			return (*iCodec);
		}
	}
	
	newCodecInstance = ((AudioMedia *)(*media))->createCodecInstance( payloadType );
	if( newCodecInstance ){
		codecs.push_back( newCodecInstance );
	}

	return newCodecInstance;
}

uint32_t AudioMediaSource::getSsrc(){
	return ssrc;
}
