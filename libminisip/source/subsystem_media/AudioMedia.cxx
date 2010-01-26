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

#include<libminisip/media/AudioMedia.h>

#include<libminisip/media/rtp/RtpHeader.h>
#include<libminisip/media/MediaStream.h>
#include<libminisip/media/soundcard/FileSoundSource.h>

#include<libminisip/media/soundcard/Resampler.h>
#include<libminisip/media/soundcard/SoundSource.h>

#include<libminisip/media/rtp/RtpPacket.h>

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
#include <libmutil/stringutils.h>
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
AudioMedia::AudioMedia( MRef<SoundIO *> soundIo_, 
			const std::list<MRef<Codec *> > & codecList_):
							RealtimeMedia(codecList_)//,
							/*soundIo(soundIo_)*/{
						
	soundIo = soundIo_;
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

void AudioMedia::registerRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender ){
	sendersLock.lock();
	if( senders.empty() ){
		sendersLock.unlock();
		soundIo->startRecord();
		sendersLock.lock();
	}

	//Don't add if it's already in the list. This is so that we support
	//multiple calls to RealtimeMediaStream::start().
	bool found=false;
	list<MRef<RealtimeMediaStreamSender *> >::iterator i;
	for (i=senders.begin(); i!=senders.end(); i++)
		if ( *i == sender)
			found = true;
	if (!found)
		senders.push_back( sender );
	sendersLock.unlock();
}

void AudioMedia::unregisterRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender ){
	bool emptyList;
	sendersLock.lock();
	senders.remove( sender );
	emptyList = senders.empty();
	sendersLock.unlock();

	if( emptyList ){
		soundIo->stopRecord();
	}
}

void AudioMedia::registerMediaSource( uint32_t ssrc, string callId ){
	MRef<AudioMediaSource *> source;

	source = new AudioMediaSource( ssrc, callId, this );
	soundIo->registerSource( *source );
	sources.push_back( source );
}

void AudioMedia::unregisterMediaSource( uint32_t ssrc ){
	std::list< MRef<AudioMediaSource *> >::iterator iSource;

	soundIo->unregisterSource( ssrc );

	for( iSource = sources.begin(); iSource != sources.end(); iSource ++ ){
		if( (*iSource)->getSsrc() == ssrc ){
			sources.erase( iSource );
			return;
		}
	}
}

void AudioMedia::playData(const MRef<RtpPacket *> & packet ){
	MRef<AudioMediaSource *> source = getSource( packet->getHeader().SSRC );

	if( source ){
		source->playData( packet );
	}
        //delete packet;

}

//this function is called from SoundIO::recorderLoop
//the void *data is originally a short *
void AudioMedia::srcb_handleSound( void * data, int nsamples, int samplerate){
//	resampler->resample( (short *)data, resampledData );
//	sendData( (byte_t*) &resampledData, 160*sizeof(short), 0, false );

	sendData( (byte_t*) data, nsamples, samplerate, 0, false );
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

void AudioMedia::sendData( byte_t * data, uint32_t nsamples, int samplerate, uint32_t ts, bool marker ){

	//all these zerodata vars are used to send silence to muted senders
	//we need a silence data vector, as we cannot simply erase the data vector, 
	//as it is shared by all media stream senders ... 
	static byte_t zeroData[160*sizeof(short)*10]; //this needs to be of _length_ length
	static bool zeroDataInit = false;
	bool encodeZeroData;
	
	uint32_t encodedLength;
	
	if( ! zeroDataInit ) {
		zeroDataInit = true;
		for( uint32_t i = 0; i<nsamples*sizeof(short); i++ ) zeroData[i] = 0; 
	}
	
	list< MRef<RealtimeMediaStreamSender *> >::iterator i;
	list< MRef<RealtimeMediaStreamReceiver *> >::iterator ir;
	list< MRef<Session*> >::iterator is;
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

			int sfreq = ((AudioCodec*)(*selectedCodec->getCodec()))->getSamplingFreq();
			
			encodedLength = selectedCodec->encode( &zeroData, sfreq==8000?160*sizeof(short):320*sizeof(short), sfreq, encoded);
		} else {


			// If audio forwarding is enabled, then we need to
			// connect the input of all other calls with the
			// output that we are about to send. Here we do
			// this by taking the last sample of audio received
			// and add it to out output. 
			// TODO: Copying the last received audio is not the
			// optimal thing to do. We should have a jitter
			// buffer that handled re-ordered packets and such.
			if (mediaForwarding){
				std::list< MRef<AudioMediaSource *> >::iterator iSource;
				for( iSource = sources.begin(); iSource != sources.end(); iSource ++ ){
					if ( (*iSource)->getCallId()!= (*i)->getCallId() ){
						short *stream = (*iSource)->getCodecOutputBuffer();
						for (uint32_t ii=0; ii< nsamples; ii++)
							((short*)data)[ii] += stream[ii];
						

					}
				}
			}
			int sfreq = ((AudioCodec*)(*selectedCodec->getCodec()))->getSamplingFreq();
			if (sfreq==8000 && SOUND_CARD_FREQ!=8000){
				resampler->resample( (short *)data, resampledData );
				encodedLength = selectedCodec->encode( resampledData, nsamples/2*sizeof(short), 8000, encoded);
			}else{ 
				if (sfreq==SOUND_CARD_FREQ){
					//cerr <<"EEEE: NOT resampling sending native"<<endl;


					encodedLength = selectedCodec->encode(data, nsamples*sizeof(short), SOUND_CARD_FREQ, encoded);
					//cerr <<"EEEE: ENCODE DONE, length="<<encodedLength<<endl;
				}else{
					massert(1==0);
				}
			}
		}

		(*i)->send( encoded, encodedLength, &givenTs, marker );
	}
	
	sendersLock.unlock();
}

void AudioMedia::startRinging( string ringtoneFile ){
	soundIo->registerSource( new FileSoundSource( "", ringtoneFile,RINGTONE_SOURCE_ID, 44100, 2, SOUND_CARD_FREQ, 20, 2, true ) );
}

void AudioMedia::stopRinging(){
	soundIo->unregisterSource( RINGTONE_SOURCE_ID );
}

#ifdef DEBUG_OUTPUT
string AudioMedia::getDebugString() {
	string ret;
	ret = getMemObjectType() + ": this=" + itoa(reinterpret_cast<int64_t>(this));
	for( std::list< MRef<RealtimeMediaStreamSender *> >::iterator it = senders.begin();
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


AudioMediaSource::AudioMediaSource( uint32_t ssrc_, string callId, MRef<Media *> m):
	BasicSoundSource( ssrc_, 
			callId,
			NULL, //plc
			0/*position*/, 
			SOUND_CARD_FREQ, 
			20, //duration in ms
			2 //number of channels (numChannels)
			//buffer size defaults to 16000 * numChannels
			),
	media(m),
	ssrc(ssrc_)
{
	//The codec output might be mixed into outgoing streams
	//any data has been decoded. We therefore initialize it
	//to silence.
	for (int i=0; i<AUDIOMEDIA_CODEC_MAXLEN; i++)
		codecOutput[i]=0;
}

void AudioMediaSource::playData( const MRef<RtpPacket *> & rtpPacket ){
        RtpHeader hdr = rtpPacket->getHeader();
	MRef<CodecState *> codec = findCodec( hdr.getPayloadType() );

	if( codec ){
		uint32_t outputSize = codec->decode( rtpPacket->getContent(), rtpPacket->getContentLength(), codecOutput );
		int sfreq = ((AudioCodec*)(*codec->getCodec()))->getSamplingFreq();
		//cerr <<"EEEE: -------------------------> decode len="<<outputSize<<" sfreq="<<sfreq<<endl;
		//cerr <<"EEEE: decoded data="<<binToHex((unsigned char*)codecOutput,outputSize*2)<<endl;

		pushSound( codecOutput, outputSize, hdr.getSeqNo(), sfreq, false);
		
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
