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

#include"Dc1394Grabber.h"
#include"../ImageHandler.h"
#include"../VideoMedia.h"
#include<stdio.h>


#define NUM_BUFFERS 2
#define DROP_FRAMES 1


using namespace std;

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
		perror( "raw1394_new_handle" );
		exit( 1 );
	}

	nbPorts = raw1394_get_port_info( rawHandle, ports, nbPorts );
	
	if( nbPorts < 0 ){
		perror( "raw1394_get_port_info" );
		exit( 1 );
	}

	if( portId >= nbPorts ){
		fprintf( stderr, "Could not find raw1394 port id %i\n", portId );
		exit( 1 );
	}

	raw1394_set_port( rawHandle, portId );

	cameraNodes = dc1394_get_camera_nodes( rawHandle, &nbCameras, 0 );

	if( cameraNodes == NULL ){
		fprintf( stderr, "Could not get camera nodes\n" );
		exit( 1 );
	}

	raw1394_destroy_handle( rawHandle );

	if( cameraId >= nbCameras ){
		fprintf( stderr, "Could not find camera id %i\n", cameraId );
		exit( 1 );
	}
	
	cameraHandle = dc1394_create_handle( portId );
			
	if( cameraHandle == NULL ){
		perror( "dc1394_create_handle" );			
		exit( 1 );
	}

	camera.node = cameraNodes[cameraId];

	ret = dc1394_dma_setup_capture( cameraHandle, camera.node, cameraId+1, 
			          FORMAT_VGA_NONCOMPRESSED, MODE_640x480_YUV422,
				  SPEED_400, FRAMERATE_15, NUM_BUFFERS, 0,
				  DROP_FRAMES, NULL, &camera );
	
	if( ret != DC1394_SUCCESS ){
		perror( "dc1394_dma_setup_capture" );
		exit( 1 );
	}

	dc1394_free_camera_nodes( cameraNodes );
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
}

	

void Dc1394Grabber::printImageFormat(){
}

void Dc1394Grabber::stop(){
	stopped = true;
}

void Dc1394Grabber::run(){
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
		perror( "dc1394_start_iso_transmission" );
		exit( 1 );
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






