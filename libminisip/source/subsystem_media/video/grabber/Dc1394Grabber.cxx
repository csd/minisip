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

#include"Dc1394Grabber.h"
#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/video/VideoMedia.h>
#include<libminisip/media/video/VideoException.h>
#include<stdio.h>
#include<errno.h>
#include<libmutil/mtime.h>
#include<libmutil/stringutils.h>
#include<libmutil/merror.h>


#define NUM_BUFFERS 2
#define DROP_FRAMES 1


using namespace std;


static std::list<std::string> pluginList;
static bool initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *mdc1394_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mdc1394_LTX_getPlugin( MRef<Library*> lib ){
	return new Dc1394Plugin( lib );
}


static void yuv422_to_yuv420p(MData *dst, const MData *src,
                              int width, int height, int downsamplingFactor);

Dc1394Grabber::Dc1394Grabber( uint32_t portId, uint32_t cameraId ){
	this->portId = portId;
	this->cameraId = cameraId;
}

void Dc1394Grabber::open(){
	raw1394handle_t rawHandle;
	int32_t nbPorts = MAX_PORTS;
	int32_t nbCameras = 0;
	struct raw1394_portinfo ports[MAX_PORTS];
	nodeid_t * cameraNodes;
	int ret;

	rawHandle = raw1394_new_handle();
	if( rawHandle == NULL ){
		merror( "raw1394_new_handle" );
                throw VideoException( strerror( errno ) );
	}

	nbPorts = raw1394_get_port_info( rawHandle, ports, nbPorts );
	
	if( nbPorts < 0 ){
		merror( "raw1394_get_port_info" );
                throw VideoException( strerror( errno ) );
	}

	if( portId >= (uint32_t)nbPorts ){
                throw VideoException(
                          "Could not find raw1394 port id " + portId );
	}

	raw1394_set_port( rawHandle, portId );

	cameraNodes = dc1394_get_camera_nodes( rawHandle, &nbCameras, 0 );

	if( cameraNodes == NULL ){
                throw VideoException( "Could not get camera nodes"  );
	}

	raw1394_destroy_handle( rawHandle );

	if( cameraId >= (uint32_t)nbCameras ){
                throw VideoException( (std::string)"Could not find camera id " + itoa( cameraId  )+ " on IEEE1394 bus " + itoa( portId )  );
	}
	
	cameraHandle = dc1394_create_handle( portId );
			
	if( cameraHandle == NULL ){
                throw VideoException( strerror( errno ) );
	}

	camera.node = cameraNodes[cameraId];

	ret = dc1394_dma_setup_capture( cameraHandle, camera.node, cameraId+1, 
			          FORMAT_VGA_NONCOMPRESSED, MODE_640x480_YUV422,
				  SPEED_400, FRAMERATE_15,
                                  NUM_BUFFERS,/* 0,*/ DROP_FRAMES, NULL, 
                                  &camera );
	
	if( ret != DC1394_SUCCESS ){
                throw VideoException( strerror( errno ) );
	}

	dc1394_free_camera_nodes( cameraNodes );

        getCapabilities();
        getImageFormat();

}

void Dc1394Grabber::getCapabilities(){
}

void Dc1394Grabber::printCapabilities(){

}

void Dc1394Grabber::getImageFormat(){
	width = 640;
	height = 480;
}

bool Dc1394Grabber::setImageChroma( uint32_t chroma ){
	return true;
}

	

void Dc1394Grabber::printImageFormat(){
}

void Dc1394Grabber::start(){
	massert(!runthread);
	runthread = new Thread(this);
}

void Dc1394Grabber::stop(){
	stopped = true;
	massert(runthread);
	runthread->join();
	runthread=NULL;

}

void Dc1394Grabber::run(){
#ifdef DEBUG_OUTPUT
	setThreadName("Dc1394Grabber::run");
#endif

	fprintf( stderr, "Start read()\n" );
	stopped = false;
	read( handler );
}

void Dc1394Grabber::setHandler( ImageHandler * handler ){
	grabberLock.lock();
	this->handler = handler;
 	grabberLock.unlock();
}

void Dc1394Grabber::read( ImageHandler * handler ){
	fprintf( stderr, "Start read( handler )\n" );
	grabberLock.lock();
	int ret;
	MImage captured;
	MImage * image;
	bool handlerProvidesImage = ( handler && handler->providesImage() );
	uint32_t handlerInputWidth = handler->getRequiredWidth();
	uint32_t handlerInputHeight = handler->getRequiredHeight();
	uint32_t offset = 0;
	uint32_t downsamplingFactor = 1;
	
	if( handlerInputWidth*2 < width && handlerInputHeight*2 < height ){
		// We have enough picture do downsample by half
		downsamplingFactor = 2;
	}

	if( handlerInputWidth < width && handlerInputHeight < height ){
		offset = width*(height - handlerInputHeight) + (width - handlerInputWidth);
		offset /= downsamplingFactor;
	}

	

	ret = dc1394_start_iso_transmission( cameraHandle, camera.node );

	if( ret !=DC1394_SUCCESS ){
		fprintf( stderr, "Error when starting iso transmission\n");
		merror( "dc1394_start_iso_transmission" );
                throw VideoException( strerror( errno ) );
	}

	/* Reading loop */

	if( !handlerProvidesImage ){
		image = new MImage;
		image->data[0] = new uint8_t[ handlerInputWidth * handlerInputHeight ];
		image->data[1] = new uint8_t[ handlerInputWidth * handlerInputHeight / 2];
		image->data[2] = new uint8_t[ handlerInputWidth * handlerInputHeight / 2];
		image->linesize[0] = handlerInputWidth;
		image->linesize[1] = handlerInputWidth / 2;
		image->linesize[2] = handlerInputWidth / 2;
		image->width=handlerInputWidth;
		image->height=handlerInputHeight;

		image->chroma = M_CHROMA_I420;
	}
	
	while( !stopped ){
		dc1394_dma_single_capture( &camera );
		if( handlerProvidesImage ){
			image = handler->provideImage();
		}

		captured.data[0] = (uint8_t *)camera.capture_buffer + offset;
		captured.linesize[0] = width*2;

		/* The camera provides packed I422, the codec and
		 * displays want plannar I420, we need to downsample */

		yuv422_to_yuv420p( (MData*)image, (MData*)&captured,
				   handlerInputWidth, handlerInputHeight,
				   downsamplingFactor );

		/*
		if( mixer ){
			mixer->removeMixedImage( oldImage );
			mixer->addMixedImage( image );
		}
		*/

		// FIXME: get it from the camera
		image->mTime = mtime();

		if( handler ){
			handler->handle( image );
		}


                dc1394_dma_done_with_buffer( &camera );
		oldImage = image;
	}

	dc1394_stop_iso_transmission( cameraHandle, camera.node );
	if( !handlerProvidesImage ){
		delete [] image->data[0];
		delete [] image->data[1];
		delete [] image->data[2];
		delete image;
	}

	grabberLock.unlock();

}

void Dc1394Grabber::close(){
	stop();
	grabberLock.lock();

	dc1394_dma_release_camera( cameraHandle, &camera );
	dc1394_destroy_handle( cameraHandle );

	grabberLock.unlock();
	
}

void Dc1394Grabber::setLocalDisplay(MRef<VideoDisplay*>){

}


/* Borrowed from libavcodec */

static void yuv422_to_yuv420p(MData *dst, const MData *src,
                              int width, int height, int downsamplingFactor)
{
    const uint8_t *p, *p1;
    uint8_t *lum, *cr, *cb, *lum1, *cr1, *cb1;
    int w;

    p1 = src->data[0];
    lum1 = dst->data[0];
    cb1 = dst->data[1];
    cr1 = dst->data[2];

    for(;height >= 1; height -= 2) {
        p = p1;
        lum = lum1;
        cb = cb1;
        cr = cr1;
        for(w = width; w >= 2; w -= 2) {
            /*lum[0] = p[0];
            cb[0] = p[1];
            lum[1] = p[2];
            cr[0] = p[3];*/
            lum[0] = p[1];
            cb[0] = p[0];
            lum[1] = p[3];
            cr[0] = p[2];
          //  p += 4;
            p += 4*downsamplingFactor;
            lum += 2;
            cb++;
            cr++;
        }
        if (w) {
            lum[0] = p[0];
            cb[0] = p[1];
            cr[0] = p[3];
            cb++;
            cr++;
        }
        //p1 += src->linesize[0];
        p1 += src->linesize[0] * downsamplingFactor;
        lum1 += dst->linesize[0];
        if (height>1) {
            p = p1;
            lum = lum1;
            for(w = width; w >= 2; w -= 2) {
		    /*
                lum[0] = p[0];
                lum[1] = p[2];*/
		lum[0] = p[1];
		lum[1] = p[3];
            //    p += 4;
                p += 4*downsamplingFactor;
                lum += 2;
            }
            if (w) {
                lum[0] = p[0];
	    }
            //p1 += src->linesize[0];
            p1 += downsamplingFactor*src->linesize[0];
            lum1 += dst->linesize[0];
        }
        cb1 += dst->linesize[1];
        cr1 += dst->linesize[2];
    }
}


MRef<Grabber *> Dc1394Plugin::create( const std::string &device ) const{
	uint32_t portId = atoi( device.substr( 0, 1 ).c_str() );
	uint32_t cameraId = atoi( device.substr( 2, 1 ).c_str() );
		
	return new Dc1394Grabber( portId, cameraId );
}
