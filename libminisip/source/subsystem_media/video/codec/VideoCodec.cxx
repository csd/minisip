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

#include<libminisip/media/video/codec/VideoCodec.h>
#include<libminisip/media/video/codec/AVCoder.h>
#include<libminisip/media/video/codec/AVDecoder.h>
#include"../../MediaHandler.h"
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/video/grabber/Grabber.h>
#include<libminisip/media/video/display/VideoDisplay.h>
#include<libminisip/media/video/VideoException.h>

#include<libmutil/dbg.h>

using namespace std;

MVideoCodec::MVideoCodec(){
	decoder = new AVDecoder();
	coder = new AVEncoder();

}

void MVideoCodec::encode( MRef<MImage *> image ){
	coder->handle( *image );
}

MRef<AVEncoder*> MVideoCodec::getEncoder(){
	return coder;
}

void MVideoCodec::decode( byte_t * inputData, uint32_t inputSize ){
	decoder->decodeFrame( inputData, inputSize );
}

std::string MVideoCodec::getCodecName(){
	return std::string( "H.264" );
}

std::string MVideoCodec::getCodecDescription(){
	return std::string( "X264 based H.264 Video Encoder/Decoder" );
}

uint8_t MVideoCodec::getSdpMediaType(){
// printf("---------------------------------------- codec   test 1  \n");
	return 99;
}

std::string MVideoCodec::getSdpMediaAttributes(){
	return std::string("H264/90000");
}

void MVideoCodec::setGrabber( MRef<Grabber *> grabber ){
	this->grabber = grabber;
	if( grabber ){
		grabber->setHandler( *coder );
	}
}

void MVideoCodec::setDisplay( MRef<VideoDisplay *> display ){
	this->display = display;
	if( display ){
		decoder->setHandler( *display );
	}
}

void MVideoCodec::setEncoderCallback( VideoEncoderCallback * cb ){
	coder->setCallback( cb );
}

void MVideoCodec::startSend( uint32_t width, uint32_t height){
	coder->init( width, height );

	if( grabber ){
                try{
		grabber->open();
	
//		grabber->getCapabilities();
//		grabber->getImageFormat();
                if( !grabber->setImageChroma( M_CHROMA_RV32 ) &&
		    !grabber->setImageChroma( M_CHROMA_I420 ) ){
                        merr << "Could not select video capture chroma: "
                             << endl;
                        grabber = NULL;
			return;
		}

		//Thread t(*grabber);
		grabber->start();
                }
                catch( VideoException & exc ){
                        merr << "Could not open the video capture device: "
                             << exc.error() << endl;
                        grabber = NULL;
                }
	}
	
}

void MVideoCodec::stopSend(){
 cout <<"--------------------------------------------------------------------------------- stopSend \n";
	if( grabber ){
                try{
cout <<"--------------------------------------------------------------------------------- stopSend   2  \n";
		grabber->close();
                }
                catch( VideoException & exc ){
                        merr << "Could not close the video capture device: "
                             << exc.error() << endl;
                }
	}
	coder->close();


}

uint32_t MVideoCodec::getVersion() const{
	return 0x00000001;
}
