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

#include"VideoCodec.h"
#include"AVCoder.h"
#include"AVDecoder.h"
#include"../../mediahandler/MediaHandler.h"
#include"../../codecs/Codec.h"
#include"../grabber/Grabber.h"
#include"../display/VideoDisplay.h"


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
	return std::string( "H263" );
}

std::string VideoCodec::getCodecDescription(){
	return std::string( "ITU-T H263 Video Encoder/Decoder" );
}

int32_t VideoCodec::getSdpMediaType(){
	return 34;
}

std::string VideoCodec::getSdpMediaAttributes(){
	return std::string("");
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
		grabber->open();
	
		grabber->getCapabilities();
		grabber->getImageFormat();
                grabber->setImageChroma( M_CHROMA_RV32 );

		Thread t(*grabber);
	}
	
}

void VideoCodec::stopSend(){
	fprintf( stderr, "stopSend called\n");

	fprintf( stderr, "Closing grabber\n" );
	if( grabber ){
		grabber->close();
	}
	fprintf( stderr, "Closing coder\n" );
	coder->close();

}
