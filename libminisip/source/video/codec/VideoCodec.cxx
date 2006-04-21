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

#include<libminisip/video/codec/VideoCodec.h>
#include<libminisip/video/codec/AVCoder.h>
#include<libminisip/video/codec/AVDecoder.h>
#include<libminisip/mediahandler/MediaHandler.h>
#include<libminisip/codecs/Codec.h>
#include<libminisip/video/grabber/Grabber.h>
#include<libminisip/video/display/VideoDisplay.h>
#include<libminisip/video/VideoException.h>

#include<libmutil/dbg.h>


VideoCodec::VideoCodec(){
	decoder = new AVDecoder();
	coder = new AVEncoder();

}

void VideoCodec::encode( MRef<MImage *> image ){
	coder->handle( *image );
}

void VideoCodec::decode( byte_t * inputData, uint32_t inputSize ){
	decoder->decodeFrame( inputData, inputSize );
}

std::string VideoCodec::getCodecName(){
	return std::string( "H.263" );
}

std::string VideoCodec::getCodecDescription(){
	return std::string( "ITU-T H.263 Video Encoder/Decoder" );
}

uint8_t VideoCodec::getSdpMediaType(){
	return 105;
}

std::string VideoCodec::getSdpMediaAttributes(){
	return std::string("h263-1998/90000");
}

void VideoCodec::setGrabber( MRef<Grabber *> grabber ){
	this->grabber = grabber;
	if( grabber ){
		grabber->setHandler( *coder );
	}
}

void VideoCodec::setDisplay( MRef<VideoDisplay *> display ){
	this->display = display;
	if( display ){
		decoder->setHandler( *display );
	}
}

void VideoCodec::setEncoderCallback( VideoEncoderCallback * cb ){
	coder->setCallback( cb );
}

void VideoCodec::startSend( uint32_t width, uint32_t height){
	coder->init( width, height );

	if( grabber ){
                try{
		grabber->open();
	
		grabber->getCapabilities();
		grabber->getImageFormat();
                if( !grabber->setImageChroma( M_CHROMA_RV32 ) &&
		    !grabber->setImageChroma( M_CHROMA_I420 ) ){
                        merr << "Could not select video capture chroma: "
                             << end;
                        grabber = NULL;
			return;
		}

		Thread t(*grabber);
                }
                catch( VideoException & exc ){
                        merr << "Could not open the video capture device: "
                             << exc.error() << end;
                        grabber = NULL;
                }
	}
	
}

void VideoCodec::stopSend(){
	if( grabber ){
                try{
		grabber->close();
                }
                catch( VideoException & exc ){
                        merr << "Could not close the video capture device: "
                             << exc.error() << end;
                }
	}
	coder->close();

}

uint32_t VideoCodec::getVersion() const{
	return 0x00000001;
}
