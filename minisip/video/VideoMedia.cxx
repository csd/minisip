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

#include"VideoMedia.h"
#include"../codecs/Codec.h"
#include"codec/VideoCodec.h"
#include"codec/AVDecoder.h"
#include"codec/AVCoder.h"
#include"grabber/Grabber.h"
#include"mixer/ImageMixer.h"
#include"display/VideoDisplay.h"
#include"display/SdlDisplay.h"
#include"../sdp/SdpHeaderM.h"
#include"../sdp/SdpHeaderA.h"
#include<libmutil/itoa.h>
#include<assert.h>

#define SOURCE_QUEUE_SIZE 7


using namespace std;

VideoMedia::VideoMedia( MRef<Codec *> codec, MRef<VideoDisplay *> display, MRef<ImageMixer *> mixer, MRef<Grabber *> grabber, uint32_t receivingWidth, uint32_t receivingHeight ):
                Media( codec ),display(display),grabber(grabber),mixer(mixer),receivingWidth(receivingWidth),receivingHeight(receivingHeight){

        this->codec = dynamic_cast<VideoCodec *>(*codec);
        assert( this->codec );
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



void VideoMedia::playData( RtpPacket * packet ){

	MRef<VideoMediaSource *> source = getSource( packet->getHeader().SSRC );

	if( source ){
		cerr << "Before playData" << endl;
		source->playData( packet );
		cerr << "After playData" << endl;
	}
	else{
		delete packet;
	}

}



void VideoMedia::sendVideoData( byte_t * data, uint32_t length, uint32_t ts, bool marker ){
        Media::sendData( data, length, ts, marker );
}

void VideoMedia::registerMediaSource( uint32_t ssrc ){
	MRef<VideoMediaSource *> source;

	source = new VideoMediaSource( ssrc, receivingWidth, receivingHeight );

        if( mixer ){
                source->getDecoder()->setHandler( *mixer );
        }

        else{
                MRef<VideoDisplay *> newdisplay = VideoDisplay::create( receivingWidth, receivingHeight );
                source->getDecoder()->setHandler( *newdisplay );
                source->display = newdisplay;
                cerr << "Starting DISPLAY" << endl;
                newdisplay->start();
        }


        cerr << "Registering VideoMediaSource, sources.size = " << sources.size() << endl;

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

void VideoMedia::unRegisterMediaSource( uint32_t ssrc ){
cerr << "Unregister source" << endl;
	MRef<VideoMediaSource *> source = getSource( ssrc );

	if( source ){
		cerr << "found source" << endl;
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
        cerr << "end unregister" << endl;
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

void VideoMedia::registerMediaSender( MRef<MediaStreamSender *> sender ){
        sendersLock.lock();
        if( senders.size() == 0 ){
                sendersLock.unlock();
                codec->startSend( sendingWidth, sendingHeight );
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

void VideoMediaSource::playData( RtpPacket * packet ){
	int seqNo = packet->getHeader().getSeqNo();

	if( savedPacket ){
		if( seqNo != expectedSeqNo - 2 ){
			packetLoss = true;
			delete savedPacket;
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
			cerr << "Packet lost in video stream, dropping one frame (seqNo == expectedSeqNo + 1)" << endl;
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

void VideoMediaSource::addPacketToFrame( RtpPacket * packet ){
cerr << "Started addPacketToFrame" << endl;
        if( !packetLoss ){
	 	packet->getContent()[0] = 0;
                memcpy( frame + index, packet->getContent() , packet->getContentLength()  );
                index += packet->getContentLength();
        }

        if( packet->getHeader().marker ){
                if( ! packetLoss ){
                        /* We have a frame */
			cerr << "Before decodeFrame" << endl;
                        decoder->decodeFrame( frame, index );
			cerr << "After decodeFrame" << endl;
                }
                index = 0;
                packetLoss = false;
        }

	delete packet;
}
