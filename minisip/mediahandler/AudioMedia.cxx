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

#include"AudioMedia.h"
#include"../rtp/RtpHeader.h"
#include"MediaStream.h"
#include"../soundcard/FileSoundSource.h"

#define RINGTONE_SOURCE_ID 0x42124212

class G711CODEC;

// pn430 Parameter list changed for multicodec
//AudioMedia::AudioMedia( MRef<SoundIO *> soundIo, MRef<Codec *> codec ):
//                Media(codec),
//                soundIo(soundIo){
AudioMedia::AudioMedia( MRef<SoundIO *> soundIo, std::list<MRef<Codec *> > codecList, MRef<Codec *> defaultCodec ):
                Media(codecList, defaultCodec),
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
	resampler = Resampler::create( SOUND_CARD_FREQ, 8000, 20, 1 /*Nb channels */);
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

	soundIo->registerSource( *source );
	sources.push_back( source );
}

void AudioMedia::unRegisterMediaSource( uint32_t ssrc ){
	std::list< MRef<AudioMediaSource *> >::iterator iSource;

	soundIo->unRegisterSource( ssrc );

	for( iSource = sources.begin(); iSource != sources.end(); iSource ++ ){
		if( (*iSource)->getSsrc() == ssrc ){
			sources.erase( iSource );
			return;
		}
	}
}

void AudioMedia::playData( RtpPacket * packet ){
	MRef<AudioMediaSource *> source = getSource( packet->getHeader().SSRC );

	if( source ){
		source->playData( packet );
	}
        delete packet;

}

void AudioMedia::srcb_handleSound( void * data ){

        resampler->resample( (short *)data, resampledData );
        sendData( (byte_t*) &resampledData, 0, 0, false );
        seqNo ++;
}

void AudioMedia::sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker ){

    list< MRef<MediaStreamSender *> >::iterator i;
    sendersLock.lock();
    
    
    for( i = senders.begin(); i != senders.end(); i++ ){
        MRef<AudioCodec *> selectedCodec = (AudioCodec*)(*(*i)->getSelectedCodec());
        
        uint32_t encodedLength = 
            selectedCodec->encode( data, selectedCodec->getInputNrSamples()*sizeof(short), encoded );

        (*i)->send( encoded, encodedLength, &ts, marker );
    }
    
    sendersLock.unlock();
}

void AudioMedia::startRinging( string ringtoneFile ){
	soundIo->registerSource( new FileSoundSource( ringtoneFile,RINGTONE_SOURCE_ID, 44100, 2, SOUND_CARD_FREQ, 20, 2, true ) );
}

void AudioMedia::stopRinging(){
	soundIo->unRegisterSource( RINGTONE_SOURCE_ID );
}

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
	BasicSoundSource( ssrc, NULL, 0/*position*/, SOUND_CARD_FREQ, 20, 2 ),
	media(media),ssrc(ssrc)
{
}

void AudioMediaSource::playData( RtpPacket * rtpPacket ){
        RtpHeader hdr = rtpPacket->getHeader();
	MRef<AudioCodec *> codec = findCodec( hdr.getPayloadType() );

	if( codec ){
                uint32_t outputSize = codec->decode( rtpPacket->getContent(), rtpPacket->getContentLength(), codecOutput );

        	pushSound( codecOutput,
                 	outputSize, hdr.getSeqNo() );
		
        }
}

MRef<AudioCodec *> AudioMediaSource::findCodec( uint8_t payloadType ){
	std::list< MRef<AudioCodec *> >::iterator iCodec;
	MRef<AudioCodec *> newCodec;

	for( iCodec = codecs.begin(); iCodec != codecs.end(); iCodec ++ ){
		if( (*iCodec)->getSdpMediaType() == payloadType ){
			return (*iCodec);
		}
	}
	
	newCodec = AudioCodec::create( payloadType );
	if( newCodec ){
		codecs.push_back( newCodec );
	}

	return newCodec;
}

uint32_t AudioMediaSource::getSsrc(){
	return ssrc;
}
