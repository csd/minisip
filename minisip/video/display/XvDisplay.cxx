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

#include "XvDisplay.h"
#include<sys/time.h>

using namespace std;

#define NB_IMAGES 3

XvDisplay::XvDisplay( uint32_t width, uint32_t height):VideoDisplay(){
	this->width = width;
	this->height = height;
	xvPort = -1;
	fullscreen = false;

	//openDisplay();
}

void XvDisplay::openDisplay(){
	char * displayName = NULL;
	unsigned int i,j,k;
	unsigned int nAdaptors;
	XvAdaptorInfo * adaptors;

	xvPort = -1;
	

	displayName = getenv( "DISPLAY" );

	if( displayName == NULL ){
		fprintf( stderr, "Could not open X11 display\n" );
		exit( 1 );
	}
	
	/* Open the display */

	display = XOpenDisplay( displayName );

	if( display == NULL ){
		fprintf( stderr, "Cound not open display: %s\n", display );
		exit( 1 );
	}

	screen = DefaultScreen( display );

	/* Query the Xv extension */

	if( XvQueryExtension( display, &i, &i, &i, &i, &i ) != Success ){
		fprintf( stderr, "Could not find Xv extension\n" );
		exit( 1 );
	}

	/* Query the Xv adaptors */

	if( XvQueryAdaptors( display, DefaultRootWindow( display ),
			     &nAdaptors, &adaptors ) != Success ){
		fprintf( stderr, "Could not find Xv adaptors\n" );
		exit( 1 );
	}

	for( i = 0; i < nAdaptors; i++ ){

		
		XvImageFormatValues * formats;
		int nFormats;
		int j;
			
		if( !( adaptors[i].type & XvInputMask ) | 
		    !(adaptors[i].type & XvImageMask ) ){
			continue;
		}

		formats = XvListImageFormats( display, 
				              adaptors[ i ].base_id,
					      &nFormats );
	

		for( j = 0; j < nFormats; j++ ){
			if( formats[ j ].id != M_CHROMA_I420 ){
				continue;
			}

			for( k = adaptors[i].base_id; 
			     k < adaptors[i].base_id + adaptors[i].num_ports;
			     k ++ ){
				if( XvGrabPort( display, k, CurrentTime )
					== Success ){
					xvPort = k;
					break;
				}
			}

			if( xvPort != -1 ){
				break;
			}
		}

		if( xvPort != -1 ){
			break;
		}

	}

	if( xvPort == -1 ){
		fprintf( stderr, "Could not find a suitable Xv Port\n" );
		exit( 1 );
	}
}

void XvDisplay::init( uint32_t width, uint32_t height ){
	this->width = width;
	this->height = height;
}

void XvDisplay::createWindow(){
	XSizeHints sizeHints;
	XSetWindowAttributes windowAttributes;
	XGCValues gcValues;
	char * imageData = NULL;
	bool exposeSent = false;
	bool mapNotifySent = false;
	bool configureNotifySent = false;
	XEvent event;

	this->height = this->baseWindowHeight = height;
	this->width = this->baseWindowWidth = width;

	openDisplay();

	/* Create the window */

	sizeHints.min_width = 2;
	sizeHints.min_height = 1;

	windowAttributes.backing_store = Always;
	windowAttributes.background_pixel = BlackPixel( display, screen );

	windowAttributes.event_mask = ExposureMask | StructureNotifyMask|
		                      KeyPressMask;

	sizeHints.base_width = width;
	sizeHints.base_height = height;

	baseWindow = XCreateWindow( display,
                           DefaultRootWindow( display ),
                           0, 0,
                           width, height,
                           0,
                           0, InputOutput, 0,
                           CWBackingStore | CWBackPixel | CWEventMask,
                           &windowAttributes );

	//XSetWMNormalHints( display, baseWindow, &sizeHints );

	XStoreName( display, baseWindow, "Minisip XVideo" );

	gcValues.graphics_exposures = False;
	gc = XCreateGC( display, baseWindow, GCGraphicsExposures, &gcValues );
	
	XMapWindow( display, baseWindow );
	do{
		XNextEvent( display, &event );
		if( ( event.type == Expose )
			&& ( event.xexpose.window == baseWindow ) ){
			exposeSent = true;
		}
		else if( ( event.type == MapNotify)
			&& ( event.xmap.window == baseWindow ) ){
			mapNotifySent = true;
		}
		else if( ( event.type == ConfigureNotify )
			&& ( event.xconfigure.window == baseWindow ) ){
			configureNotifySent = true;
			baseWindowWidth = event.xconfigure.width;
			baseWindowHeight = event.xconfigure.height;
		}
	} while( !( exposeSent && configureNotifySent && mapNotifySent ) );

    	XSelectInput( display, baseWindow, StructureNotifyMask | KeyPressMask );
//                  StructureNotifyMask | KeyPressMask |
//                  ButtonPressMask | ButtonReleaseMask |
//                  PointerMotionMask );



 	videoWindow =  XCreateSimpleWindow(
                                      display,
                                      baseWindow, 0, 0,
                                      width, height,
                                      0,
                                      BlackPixel( display, screen ),
                                      WhitePixel( display, screen ) );


	XSetWindowBackground( display, videoWindow, 
				BlackPixel( display, screen ) );


	XSelectInput( display, videoWindow, ExposureMask );
	
	XMapWindow( display, videoWindow );

	XSync( display, False );
}

void XvDisplay::destroyWindow(){
	XvUngrabPort( display, xvPort, CurrentTime );
	XSync( display, False );
	fprintf( stderr, "Destroying video window\n");
	XDestroyWindow( display, videoWindow );
	XFreeGC( display, gc );

	fprintf( stderr, "Destroying base window\n");
	XUnmapWindow( display, baseWindow );
	XDestroyWindow( display, baseWindow );
	XCloseDisplay( display );
}

MImage * XvDisplay::allocateImage(){
	

	char * imageData = ( char * )malloc( width * height * 3 );
	XvImage * image= XvCreateImage( display, xvPort, M_CHROMA_I420,
			       imageData, width, height );

	
	MImage * mimage;

	mimage = new MImage;

	for( unsigned int i = 0; i < 3; i++ ){
		mimage->data[i] = (uint8_t *)(image->data + image->offsets[i]);
		mimage->linesize[i] = image->pitches[i];
	}

	mimage->privateData = image;

	return mimage;
	
}

void XvDisplay::deallocateImage( MImage * mimage ){
	char * imageData = ( char * )malloc( width * height * 3 );
	XvImage * image = (XvImage *)mimage->privateData;

	XFree( image );
	
	delete mimage;
	
}

bool XvDisplay::handlesChroma( uint32_t chroma ){
	return chroma == M_CHROMA_I420;
}


void XvDisplay::displayImage( MImage * mimage ){

	XvPutImage( display, xvPort, videoWindow, gc,
                    (XvImage*)(mimage->privateData),
                    0 /*src_x*/, 0 /*src_y*/,
                    width, height,
                    0 /*dest_x*/, 0 /*dest_y*/, baseWindowWidth, baseWindowHeight );
}

uint32_t XvDisplay::getRequiredWidth(){
	return width;
}

uint32_t XvDisplay::getRequiredHeight(){
	return height;
}

void XvDisplay::handleEvents(){
	XEvent xEvent;
	
	while( XCheckWindowEvent( display, baseWindow, 
			StructureNotifyMask | KeyPressMask, &xEvent ) == True ){

		if( xEvent.type == ConfigureNotify ){
			fprintf( stderr, "Got ConfigureNotify event\n");
			if( (uint32_t)xEvent.xconfigure.width 
					!= baseWindowWidth ||
			    (uint32_t)xEvent.xconfigure.height
			                != baseWindowHeight ){
				baseWindowWidth  = xEvent.xconfigure.width;
				baseWindowHeight = xEvent.xconfigure.height;
				XMoveResizeWindow( display, videoWindow, 0, 0, 
					baseWindowWidth, baseWindowHeight );
			}
		}

		else if( xEvent.type == KeyPress ){
			fprintf( stderr, "KeyPressed event\n");
			KeySym xKeySymbol;

			xKeySymbol = XKeycodeToKeysym( display, xEvent.xkey.keycode, 0 );
			char keyVal;// = ConvertKey( (int)xKeySymbol );

			XLookupString( &xEvent.xkey, &keyVal, 1, NULL, NULL );

			if( keyVal == 'f' ){
				fprintf( stderr, "f pressed\n" );
				toggleFullscreen();
			}
		}
	}

}

void XvDisplay::toggleFullscreen(){
	XClientMessageEvent event;

	memset( &event, '\0', sizeof( XClientMessageEvent ) );

	event.type = ClientMessage;
	event.message_type = XInternAtom( display, "_NET_WM_STATE", False ); 
	event.display = display;
	event.window = baseWindow;
	event.format = 32;
	event.data.l[ 0 ] = (fullscreen)?0:1;
	event.data.l[ 1 ] = XInternAtom( display, "_NET_WM_STATE_FULLSCREEN",
			False );

	XSendEvent( display, DefaultRootWindow( display ), False, 
			SubstructureRedirectMask, (XEvent*)&event );

	fullscreen = !fullscreen;
	 
}
