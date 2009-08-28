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

#include<config.h>

#include<libminisip/media/video/VideoMedia.h>
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/video/codec/VideoCodec.h>
#include<libminisip/media/video/codec/AVDecoder.h>
#include<libminisip/media/video/codec/AVCoder.h>
#include<libminisip/media/video/grabber/Grabber.h>
#include<libminisip/media/video/mixer/ImageMixer.h>
#include<libminisip/media/video/display/VideoDisplay.h>
#include<libminisip/signaling/sdp/SdpHeaderM.h>
#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<libminisip/media/MediaStream.h>
#include<libmutil/stringutils.h>
#include<libmutil/massert.h>
#include<string.h>

#define SOURCE_QUEUE_SIZE 7


using namespace std;

VideoMedia::VideoMedia( MRef<Codec *> codec, MRef<VideoDisplay *> display, MRef<ImageMixer *> mixer, MRef<Grabber *> grabber, uint32_t receivingWidth, uint32_t receivingHeight ):
                RealtimeMedia( codec ),grabber(grabber),display(display),mixer(mixer),receivingWidth(receivingWidth),receivingHeight(receivingHeight){

        this->codec = dynamic_cast<MVideoCodec *>(*codec);
        massert( this->codec );
        receive = true;         
	send = (!grabber.isNull());
        this->codec->setDisplay( display );
        this->codec->setGrabber( grabber );
        this->codec->setEncoderCallback( this );
        if( mixer ){
                mixer->setOutput( *display );
                mixer->init( receivingWidth, receivingHeight );
        }
        sendingWidth = 176;
        sendingHeight = 144;


        addSdpAttribute( "framesize:34 " + itoa( receivingWidth ) + "-" + itoa( receivingHeight ) );
}


string VideoMedia::getSdpMediaType(){

        return "video";
}

void VideoMedia::handleMHeader( MRef< SdpHeaderM * > m ){
        string framesizeString = m->getAttribute( "framesize", 0 );
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

void VideoMedia::playData( MRef<RtpPacket *> packet ){

	MRef<VideoMediaSource *> source = getSource( packet->getHeader().SSRC );

	if( source ){ //FIXME: Deliberately break playing video, introduced Dec 18 2007, must be removed same day
		source->playData( packet );
	}

#ifdef VIDEO_FORWARD_CODE_REMOVETHIS
	if (mediaForwarding){
		list< MRef<RealtimeMediaStreamSender *> >::iterator i;
		sendersLock.lock();
		for( i = senders.begin(); i != senders.end(); i++ ){
			(*i)->sendRtpPacket( packet );
		}
		sendersLock.unlock();
	}
#endif //VIDEO_FORWARD_CODE_END
	
}



void VideoMedia::sendVideoData( byte_t * data, uint32_t length, uint32_t ts, bool marker ){
        sendData( data, length, ts, marker );
}

void VideoMedia::registerMediaSource( uint32_t ssrc, string callId ){
	MRef<VideoMediaSource *> source;

	source = new VideoMediaSource( ssrc, receivingWidth, receivingHeight );

        if( mixer ){
                source->getDecoder()->setHandler( *mixer );
        }

        else{
                MRef<VideoDisplay *> newdisplay = VideoDisplayRegistry::getInstance()->createDisplay( receivingWidth, receivingHeight );
                source->getDecoder()->setHandler( *newdisplay );
                source->display = newdisplay;
                if( newdisplay ){
                        //newdisplay->start();
                }
        }



	sourcesLock.lock();
	if( sources.size() == 0 ){
		sourcesLock.unlock();
		/* start everything */
                if( mixer ){
                        mixer->selectMainSource( ssrc );
                        this->display->start();
                }
		sourcesLock.lock();
	}
	
	sources.push_back( source );
	sourcesLock.unlock();
	
}

void VideoMedia::unregisterMediaSource( uint32_t ssrc ){
	MRef<VideoMediaSource *> source = getSource( ssrc );

	if( source ){
		sourcesLock.lock();
		sources.remove( source );
		if( sources.size() == 0 ){
                        if( mixer ){
                                display->stop();
                        }
		}
		sourcesLock.unlock();
                source->getDecoder()->close();
                if( source->display ){
                        source->display->stop();
                }
	}
}

MRef<VideoMediaSource *> VideoMedia::getSource( uint32_t ssrc ){
	list<MRef<VideoMediaSource *> >::iterator i;
	
	sourcesLock.lock();

	for( i = sources.begin(); i != sources.end(); i++ ){
		if( (*i)->ssrc == ssrc ){
			sourcesLock.unlock();
			return (*i);
		}
	}
	sourcesLock.unlock();
	return NULL;
}

void VideoMedia::registerRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender ){
        sendersLock.lock();
        if( senders.size() == 0 ){
                sendersLock.unlock();
                codec->startSend( sendingWidth, sendingHeight );
                sendersLock.lock();
        }

        senders.push_back( sender );
        sendersLock.unlock();
}


void VideoMedia::unregisterRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender ){
        sendersLock.lock();
        senders.remove( sender );
        sendersLock.unlock();

        if( senders.size() == 0 ){
                ((MVideoCodec *)*codec)->stopSend();
        }
}

void VideoMedia::getImagesFromSources( MImage ** images, uint32_t & nImagesToMix,                                        uint32_t mainSource ){
	list< MRef<VideoMediaSource *> >::iterator i;
	uint32_t j = 0;

	sourcesLock.lock();
	for( i = sources.begin(); i!= sources.end() && j < MAX_SOURCES; i++ ){
		if( (*i)->ssrc != mainSource ){
			images[j] = (*i)->provideFilledImage();
			j++;
		}
	}
	nImagesToMix = j;
	sourcesLock.unlock();
}

void VideoMedia::releaseImagesToSources( MImage ** images, uint32_t nImages ){
	uint32_t i;
	MRef<VideoMediaSource *> source;

	for( i = 0; i < nImages; i++ ){
		if( images[i] ){
			source = getSource( images[i]->ssrc );
			if( source ){
				source->addEmptyImage( images[i] );
			}
		}
	}
}


VideoMediaSource::VideoMediaSource( uint32_t ssrc, uint32_t width, uint32_t height ):ssrc(ssrc),width(width),height(height){
        uint8_t i;
        MImage * image;
	index = 0;
	packetLoss = false;
	firstSeqNo = true;
	expectedSeqNo = 0;
	savedPacket = NULL;
        display = NULL;

        for( i = 0; i < SOURCE_QUEUE_SIZE ; i++ ){
                image = new MImage();
                image->data[0] = new uint8_t[height*width];
                image->data[1] = new uint8_t[height*width/2];
                image->data[2] = new uint8_t[height*width/2];
                image->linesize[0] = width;
                image->linesize[1] = width/2;
                image->linesize[2] = width/2;
		image->ssrc = ssrc;
		image->width=width;
		image->height=height;
                emptyImages.push_back( image );
        }

	decoder = new AVDecoder;
	decoder->setSsrc( ssrc );
	//decoder->init( width, height );

}

MImage * VideoMediaSource::provideEmptyImage(){
        MImage * image;

        emptyImagesLock.lock();
	if( emptyImages.empty() ){
		fprintf( stderr, "emptyImages.empty()!\n" );
		emptyImagesLock.unlock();
		return NULL;
	}
        image = *emptyImages.begin();
        emptyImages.pop_front();
        emptyImagesLock.unlock();

        return image;
}

MImage * VideoMediaSource::provideFilledImage(){
        MImage * image;

        filledImagesLock.lock();
	if( filledImages.empty() ){
		filledImagesLock.unlock();
		return NULL;
	}
        image = *filledImages.begin();
        filledImages.pop_front();
        filledImagesLock.unlock();

        return image;
}

void VideoMediaSource::addEmptyImage( MImage * image ){
        emptyImagesLock.lock();
        emptyImages.push_back( image );
        emptyImagesLock.unlock();
}

void VideoMediaSource::addFilledImage( MImage * image ){
        filledImagesLock.lock();
        filledImages.push_back( image );
        filledImagesLock.unlock();
}

MRef<AVDecoder *> VideoMediaSource::getDecoder(){
	return decoder;
}

void VideoMediaSource::playData( MRef<RtpPacket *> packet ){
	int seqNo = packet->getHeader().getSeqNo();

	if( firstSeqNo ){
		expectedSeqNo = seqNo;
		firstSeqNo = false;
	}

	if( savedPacket ){
		if( seqNo != expectedSeqNo - 2 ){
			packetLoss = true;
			savedPacket = NULL;
			//merr << "Packet lost in video stream, dropping one frame(seqNo == expectedSeqNo - 2)" << end;
#ifdef DEBUG_OUTPUT
			cerr << "Packet lost in video stream, dropping one frame (seqNo == expectedSeqNo - 2)" << endl;
#endif
		}
	}

	else if( seqNo != expectedSeqNo ){
		if( seqNo == expectedSeqNo + 1 ){
			savedPacket = packet;
        		expectedSeqNo = seqNo + 1;
			return;
		}
		else{
			packetLoss = true;
			//rr << "Packet lost in video stream, dropping one frame seqNo == expectedSeqNo + 1)" << end;
#ifdef DEBUG_OUTPUT
			cerr << "Packet lost in video stream, dropping one frame (seqNo " + itoa( seqNo  ) + " == expectedSeqNo ("+itoa(expectedSeqNo)+") + 1)" << endl;
#endif
		}
        }


	addPacketToFrame( packet );
	if( savedPacket ){
        	expectedSeqNo = seqNo + 2;
		addPacketToFrame( savedPacket );
		savedPacket = NULL;
	}
	else{
		expectedSeqNo = seqNo + 1;
	}	


}

void VideoMediaSource::addPacketToFrame( MRef<RtpPacket *> packet){
	unsigned char *content = packet->getContent();
	uint32_t clen = packet->getContentLength();
	bool marker= packet->getHeader().marker;

//	cerr << "EEEE: VideoMediaSource::addPacketToFrame: ssrc=" << ssrc << " len="<<clen<< " seq="<<packet->getHeader().getSeqNo() << " timestamp="<<packet->getHeader().getTimestamp()<<" marker="<<marker<<endl;

	if (!content || !clen)
		return;

	uint8_t nal = content[0];
	uint8_t type= nal & 0x1f;

	//cerr << "EEEE: VideoMediaSource::addPacketToFrame: type="<<(int)type<<endl;

	if (!((type>=1 && type<=23) || type==28)){
		cerr << "VideoMediaSource::addPacketToFrame: WARNING: unexpected packet type: "<< (int)type<<endl;
		cerr << "VideoMediaSource::addPacketToFrame: WARNING: shutting down for debugging"<<endl;
	}
//	massert( (type>=1 && type<=23) || type==28 );
	
	if (type>=1 && type<=23){
		frame[index+0]=0;
		frame[index+1]=0;
		frame[index+2]=1;
		index+=3;
                memcpy( &frame[index] , content , clen  );
		index+=clen;
		if (marker){
			decoder->decodeFrame( frame, index);
			index=0;
		}
	}else if (type==28){
		content++; // skip FU indicator
		clen--;
		
		uint8_t fu_indicator = nal;
		uint8_t fu_header = *content;
		uint8_t start_bit = (fu_header&0x80)>>7;
		uint8_t nal_type = (fu_header&0x1f);
		uint8_t reconstructed_nal = fu_indicator&0xe0;
		uint8_t end_bit = (fu_header&0x40)>>6;
		
		reconstructed_nal |= (nal_type & 0x1f);
		//cerr <<"EEEE: VideoMediaSource::addPacketToFrame: reconstructed nal_type="<<(int)nal_type <<" start_bit="<<(int)start_bit<<" end_bit="<<(int)end_bit<<endl;

		content++; //skip fu_header
		clen--;

		if (start_bit){
			frame[index+0]=0;
			frame[index+1]=0;
			frame[index+2]=1;
			frame[index+3]=reconstructed_nal;
			index+=4;
		}

		memcpy(&frame[index], content, clen);
		index+=clen;
		if (/*end_bit*/ packet->getHeader().marker){
			decoder->decodeFrame( frame, index);
			index=0;
		}

	}
	

}

