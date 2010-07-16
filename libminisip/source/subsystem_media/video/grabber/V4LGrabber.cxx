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

#include"V4LGrabber.h"
#include<libminisip/media/video/VideoMedia.h>
#include<libminisip/media/video/VideoException.h>
#include<libminisip/media/video/ImageHandler.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<sys/mman.h>
#include<errno.h>

#include<iostream>
#include<fstream>
#include<string.h>

#include<libmutil/mtime.h>
#include<libmutil/merror.h>

/*
  TODO
  The V4LGrabber contains a unresolved race condition that occurs
  if it's closed before the initialization has finished.
  It is triggered if the peer answers a video call and directly terminates it,
  which results in a crash.
 */

using namespace std;

V4LGrabber::V4LGrabber( string device ):device(device){
	v4lCapacity = NULL;
	handler = NULL;
	//imageBuffer = NULL;
	imageFormat = NULL;
        imageWindow = NULL;
	stopped = false;
	fd = -1;

	mdbg << "V4LGrabber: device " << device << endl;
}

void V4LGrabber::open(){
	


	fd = ::open( device.c_str(), O_RDWR );

	mdbg << "V4LGrabber: opened " << fd << endl;

	if( fd == -1 ){
		merror( "open" );
                throw VideoException( "Error opening " + device + " " + strerror( errno ) );
	}

	mapMemory();

	getCapabilities();
	getImageFormat();
}

void V4LGrabber::getCapabilities(){
	v4lCapacity = new struct video_capability;
	
	if( ioctl( fd, VIDIOCGCAP, v4lCapacity ) != 0 ){
		merror( "getCapacities" );
                throw VideoException( strerror( errno ) );
	}
}

void V4LGrabber::printCapabilities(){
	
	if( v4lCapacity == NULL ){
		getCapabilities();
	}

//	ret += "Driver: " + string((char *)v4lCapacity->driver) + "\n";
	cout << "Card: " + string((char *)v4lCapacity->name) + "\n";

	cout << "The card has the following capabilities: \n";
	
	cout << "Video capture: \t\t";
	
	if( v4lCapacity->type & VID_TYPE_CAPTURE )
		cout << "yes\n";
	else
		cout << "no\n";

	cout <<  "Tuner: \t\t";
	if( v4lCapacity->type & VID_TYPE_TUNER )
		cout << "yes\n";
	else
		cout << "no\n";

	cout << "Picture size: " << endl;
	cout << "  Max: " << v4lCapacity->maxwidth << "x" <<  v4lCapacity->maxheight << endl;
	cout << "  Min: " << v4lCapacity->minwidth << "x" <<  v4lCapacity->minheight << endl;

}

void V4LGrabber::mapMemory(){
//	struct v4l2_requestbuffers reqbuf;
//	struct v4l2_buffer deviceBuffer;
	struct video_mbuf mBuf;
	int i;

//	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//	reqbuf.memory = V4L2_MEMORY_MMAP;
//	reqbuf.count = N_BUFFERS;

	if( ioctl( fd, VIDIOCGMBUF, &mBuf ) != 0 ){
		fprintf( stderr, "Could not allocate device memory\n" );
                throw VideoException( strerror( errno ) );
	}
	
	mdbg << "Got " << mBuf.frames << " device buffers" << endl;

	nFrames = mBuf.frames;

	buffers[0] = new struct cardBuffer;
	buffers[0]->start = (uint8_t * )mmap( NULL, mBuf.size,
			      PROT_READ | PROT_WRITE, /* required */
		              MAP_SHARED,             /* recommended */
			      fd, 0 );
	buffers[0]->length = mBuf.size;
	
	if( buffers[0]->start == MAP_FAILED ){
		merror( "mmap" );
                throw VideoException( strerror( errno ) );
	}

	for( i = 1 ; i < mBuf.frames ; i++ ){
		buffers[i] = new struct cardBuffer;

		buffers[i]->start  = buffers[0]->start + mBuf.offsets[i];
	}

}

void V4LGrabber::getImageFormat(){
        if( !imageFormat ){
	        imageFormat = new struct video_picture;
        }

	if( ioctl( fd, VIDIOCGPICT, imageFormat ) != 0 ){
		merror( "VIDIOCGPICT" );
                throw VideoException( strerror( errno ) );
	}

        if( ! imageWindow ){
	        imageWindow = new struct video_window;
        }

	if( ioctl( fd, VIDIOCGWIN,  imageWindow ) != 0 ){
		merror( "VIDIOCGWIN" );
                throw VideoException( strerror( errno ) );
	}

	height = imageWindow->height;
	width  = imageWindow->width;
}

bool V4LGrabber::setImageSize( uint32_t width, uint32_t height ){

	massert(v4lCapacity);
	if (v4lCapacity && (width>v4lCapacity->maxwidth || height>v4lCapacity->maxheight)){
		mdbg << "V4LGrabber::setImageSize: "<<width<<"x"<<height<<" is too large. Using "<<v4lCapacity->maxwidth<<"x"<<v4lCapacity->maxheight<<endl;
		this->width=width=v4lCapacity->maxwidth;
		this->height=height=v4lCapacity->maxheight;
	}

        if( !imageWindow ){
		getImageFormat();
        }

        mdbg << "Trying to set input size to " << width << "x" << height << endl;
        mdbg << "Clipcount " << imageWindow->clipcount << endl;

        imageWindow->height = height;
        imageWindow->width = width;
	
        if( ioctl( fd, VIDIOCSWIN,  imageWindow ) != 0 ){
		merror( "VIDIOCSWIN" );
		return false;
	}

        getImageFormat();

        if( imageWindow->height != height || imageWindow->width != width ){
                return false;
        }

        return true;
}

bool V4LGrabber::setImageChroma( uint32_t chroma ){

	if( imageFormat == NULL ){
		getImageFormat();
	}

	switch( chroma ){
		case M_CHROMA_I420:
			imageFormat->palette = VIDEO_PALETTE_YUV420P;
                        imageFormat->depth = 12;
			break;
		case M_CHROMA_RV24:
			imageFormat->palette = VIDEO_PALETTE_RGB24;
                        imageFormat->depth = 24;
			break;
		case M_CHROMA_RV32:
			imageFormat->palette = VIDEO_PALETTE_RGB32;
                        imageFormat->depth = 32;
			break;
		default:
                        return false;
	}

        mdbg << "Depth: " << imageFormat->depth << endl;

	if( ioctl( fd, VIDIOCSPICT,  imageFormat ) != 0 ){
		//merror( "VIDIOCSPICT" );
		return false;
	}

	getImageFormat();
	
	switch( chroma ){
		case M_CHROMA_I420:
			if( imageFormat->palette != VIDEO_PALETTE_YUV420P )
				return false;
			break;
		case M_CHROMA_RV24:
			if( imageFormat->palette != VIDEO_PALETTE_RGB24 )
				return false;
			break;
		case M_CHROMA_RV32:
			if( imageFormat->palette != VIDEO_PALETTE_RGB32 )
				return false;
			break;
		default:
			return false;
	}
	return true;
}

	

void V4LGrabber::printImageFormat(){
	if( imageFormat == NULL ){
		getImageFormat();
	}

	cout << "Image format: " << endl;

	cout << "  width: " << imageWindow->width << endl;
	cout << "  height: " << imageWindow->height << endl;

	cout << "  colorformat: ";

	switch( imageFormat->palette ){
		case VIDEO_PALETTE_GREY:
			cout << "VIDEO_PALETTE_GREY" << endl;
			break;
		case VIDEO_PALETTE_HI240:
			cout << "VIDEO_PALETTE_HI240" << endl;
			break;
		case VIDEO_PALETTE_RGB565:
			cout << "VIDEO_PALETTE_RGB565" << endl;
			break;
		case VIDEO_PALETTE_RGB24:
			cout << "VIDEO_PALETTE_RGB24" << endl;
			break;
		case VIDEO_PALETTE_RGB32:
			cout << "VIDEO_PALETTE_RGB32" << endl;
			break;
		case VIDEO_PALETTE_RGB555:
			cout << "VIDEO_PALETTE_RGB555" << endl;
			break;
		case VIDEO_PALETTE_YUV422:
			cout << "VIDEO_PALETTE_YUV422" << endl;
			break;
		case VIDEO_PALETTE_YUYV:
			cout << "VIDEO_PALETTE_YUYV" << endl;
			break;
		case VIDEO_PALETTE_UYVY:
			cout << "VIDEO_PALETTE_UYVY" << endl;
			break;
		case VIDEO_PALETTE_YUV420:
			cout << "VIDEO_PALETTE_YUV420" << endl;
			break;
		case VIDEO_PALETTE_YUV411:
			cout << "VIDEO_PALETTE_YUV411" << endl;
			break;
		case VIDEO_PALETTE_RAW:
			cout << "VIDEO_PALETTE_RAW" << endl;
			break;
		case VIDEO_PALETTE_YUV422P:
			cout << "VIDEO_PALETTE_YUV422P" << endl;
			break;
		case VIDEO_PALETTE_YUV411P:
			cout << "VIDEO_PALETTE_YUV411P" << endl;
			break;
		case VIDEO_PALETTE_YUV420P:
			cout << "VIDEO_PALETTE_YUV420P" << endl;
			break;
		case VIDEO_PALETTE_YUV410P:
			cout << "VIDEO_PALETTE_YUV410P" << endl;
			break;
		default:
			cout << "unknown" << endl;
	}
}

void V4LGrabber::start(){
	massert(!runthread); // we don't want to start a second thread
       				// for a grabber object
        runthread = new Thread(this);
}

void V4LGrabber::stop(){
	stopped = true;
 	massert(runthread);
        //cerr << nowStr() << "EEEE: Grabber::joinThread: waiting for thread"<<endl;
        runthread->join();
        //cerr << nowStr() << "EEEE: Grabber::joinThread: thread joined"<<endl;
        runthread=NULL;

}

void V4LGrabber::run(){
#ifdef DEBUG_OUTPUT
	setThreadName("V4LGrabber::run");
#endif

	mdbg << "Start read()" << endl;
	stopped = false;
	read( handler );
}

void V4LGrabber::setHandler( ImageHandler * handler ){
	grabberLock.lock();
	this->handler = handler;
	grabberLock.unlock();
}

void V4LGrabber::read( ImageHandler * handler ){

	mdbg << "Start read( handler )" << endl;
	grabberLock.lock();
	int i;
	struct video_mmap mMap;
	MImage * image;
	bool handlerProvidesImage = ( handler && handler->providesImage() );
	uint32_t handlerInputWidth = handler->getRequiredWidth();
	uint32_t handlerInputHeight = handler->getRequiredHeight();
        uint32_t mChroma = 0;
        uint8_t pixelSize;

	if( v4lCapacity == NULL ){
		getCapabilities();
	}

        if( imageFormat == NULL ){
                getImageFormat();
        }
        
        if( ! setImageSize( handlerInputWidth, handlerInputHeight ) ){
                merr << "Could not set grabber image size." << endl;
                merr << "Grabber size: " << width << "x" << height << endl;
        }
        

        mdbg << "ImageFormat->palette " << imageFormat->palette << endl;
        
        if( imageFormat->palette == VIDEO_PALETTE_RGB24 ){
                pixelSize = 3;
                mChroma = M_CHROMA_RV24;
        }
        else if( imageFormat->palette == VIDEO_PALETTE_RGB32 ){
                pixelSize = 4;
                mChroma = M_CHROMA_RV32;
        }
	else if( imageFormat->palette == VIDEO_PALETTE_YUV420P ){
		pixelSize = 0;	// handlerProvidesImage not supported
		mChroma = M_CHROMA_I420;
	}
	
	mMap.frame = 0;
	mMap.height = height;//v4lCapacity->maxheight;
	mMap.width = width;//v4lCapacity->maxwidth;
	mMap.format = imageFormat->palette;

	/* start to capture the first frame */
	if( ioctl( fd, VIDIOCMCAPTURE, &mMap ) != 0 ){
		merror( "VIDIOCMCAPTURE (1)" );
                throw VideoException( strerror( errno ) );
	}
	mdbg << "before loop" << endl;


	if( !handlerProvidesImage ){
                image = new MImage;
		memset(image, 0, sizeof(*image));
		image->width=width;
		image->height=height;

		switch( mChroma ){
			case M_CHROMA_I420:
				image->linesize[0] = width;
				image->linesize[1] = width / 2;
				image->linesize[2] = width / 2;
				break;

			default:
				image->linesize[0] = /*handlerInputWidth * 3;*/width*pixelSize;
		}

		image->chroma = mChroma;
	}


	/* Reading loop */
	while( !stopped ){
		/* go through in all the available frames */
		for( i = 0 ; i < nFrames ; i++ ){

			/* start the capture on the next frame */
			mMap.frame = ( i + 1 ) % nFrames;
			
			if( ioctl( fd, VIDIOCMCAPTURE, &mMap ) != 0 ){
				merror( "VIDIOCMCAPTURE (2)" );
                                throw VideoException( strerror( errno ) );
			}

			/* read the current one */

			mMap.frame = i;
			if( ioctl( fd, VIDIOCSYNC, &mMap ) != 0 ){
				merror( "VIDIOCSYNC" );
                                throw VideoException( strerror( errno ) );
			}
	
			if( handlerProvidesImage ){
				image = handler->provideImage();
				memcpy( image->data[0], buffers[i]->start,
					width * height * pixelSize );
					
			}
			else{
				image->data[0] = buffers[i]->start;

				if( image->chroma == M_CHROMA_I420 ){
					image->data[1] = image->data[0] + image->linesize[0] * height;
					image->data[2] = image->data[1] + image->linesize[1] * height / 2;
				}
			}

                        /* FIXME get it from the camera */

                        image->mTime = mtime();
			if( handler ){
				handler->handle( image );
			}
		}
	}

	grabberLock.unlock();

}

void V4LGrabber::close(){

	mdbg << "V4LGrabber: Close" << endl;

	stop();

	if (fd!=-1){
		grabberLock.lock();
		unmapMemory();

		::close( fd );
		fd = -1;
		grabberLock.unlock();
	}
}

void V4LGrabber::unmapMemory(){
	uint32_t i;

	for( i = 1 ; i < nFrames ; i++ ){
		delete buffers[i];
	}

	if( munmap( buffers[0]->start, buffers[0]->length ) < 0 ){
		merror( "munmap" );
		throw VideoException( strerror( errno ) );
	}

	delete buffers[0];


}


void V4LGrabber::setLocalDisplay(MRef<VideoDisplay*>){

}


#if 0
	
		



int main( int argc, char ** argv ){
	V4LGrabber * g;
	PpmWriter * w;
	
	if( argc != 2 ){
		printf( "Usage: grabber <device>.\n" );
		exit( 0 );
	}

	g = new V4LGrabber( argv[1] );
	w = new PpmWriter( "/tmp/image.ppm" );


	g->open();

	g->printCapabilities();

	g->printImageFormat();
	
	w->init( g->getHeight(), g->getWidth() );

	g->mapMemory();

	g->read( w );

	return 0;

}
#endif


