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

/* Copyright (C) 2004, 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net> 
*/

#include"X11Display.h"
#include<sys/time.h>
#include<libminisip/media/video/VideoException.h>
#include<X11/Xatom.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

using namespace std;

#define NB_IMAGES 3

static std::list<std::string> pluginList;
static bool initialized;

extern "C" LIBMINISIP_API
std::list<std::string> *mx11_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mx11_LTX_getPlugin( MRef<Library*> lib ){
	return new X11Plugin( lib );
}

X11Display::X11Display( uint32_t width, uint32_t height):VideoDisplay(){
	this->width = width;
	this->height = height;
	fullscreen = false;

	//openDisplay();
}

void X11Display::openDisplay(){
        XVisualInfo xVisualTemplate;
        XPixmapFormatValues * format;
	char * displayName = NULL;
	int nFormat;

	displayName = getenv( "DISPLAY" );

	if( displayName == NULL ){
		throw VideoException( "Could not open X11 display" );
	}
	
	/* Open the display */

	display = XOpenDisplay( displayName );

	if( display == NULL ){
		throw VideoException( "Could not open X11 display" );
	}

	screen = DefaultScreen( display );
        screenDepth = XDefaultDepth( display, screen );

        if( screenDepth < 16 ){
		throw VideoException( "Screen depth should be at least 16 bpp" );
        }

        xVisualTemplate.screen = screen;
        xVisualTemplate.c_class = TrueColor;

        visualInfo = XGetVisualInfo( display, VisualClassMask,
                        &xVisualTemplate, &nFormat );

        if( visualInfo == NULL ){
		throw VideoException( "Could not find a TrueColor visual" );
        }

        format = XListPixmapFormats( display, &nFormat );

        bytesPerPixel = 0;

        for( ; nFormat --; format ++ ){
                if( format->depth == screenDepth ){
                        uint32_t bpp = format->bits_per_pixel / 8;
                        if( bpp > bytesPerPixel ){
                                bytesPerPixel = bpp;
                        }
                }
        }
        
}

void X11Display::init( uint32_t width, uint32_t height ){
	this->width = width;
	this->height = height;
}

void X11Display::createWindow(){
	XSizeHints sizeHints;
	XSetWindowAttributes windowAttributes;
	XGCValues gcValues;
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

	XStoreName( display, baseWindow, "Minisip video" );

	gcValues.graphics_exposures = False;
	gc = XCreateGC( display, baseWindow, GCGraphicsExposures, &gcValues );
	
//#ifdef IPAQ
        Atom type = XInternAtom( display, "_NET_WM_WINDOW_TYPE", False );
        Atom typeDialog = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False );

        XChangeProperty( display, baseWindow, type,
                        XA_ATOM, 32, PropModeReplace,
                        (unsigned char *) &typeDialog, 1);
//#endif
        
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

void X11Display::destroyWindow(){
	XSync( display, False );
	mdbg << "Destroying video window" << endl;
	XDestroyWindow( display, videoWindow );
	XFreeGC( display, gc );

	mdbg << "Destroying base window" << endl;
	XUnmapWindow( display, baseWindow );
	XDestroyWindow( display, baseWindow );
	XCloseDisplay( display );
}

MImage * X11Display::allocateImage(){
	
	char * imageData = ( char * )malloc( width * height * bytesPerPixel );
	XImage * image= XCreateImage( display, visualInfo->visual, 
                                screenDepth, ZPixmap, 0/*Offset*/,
			        imageData, width, height, 32, 0 );

	
	MImage * mimage;

	mimage = new MImage;
	mimage->width=width;
	mimage->height=height;

        fprintf( stderr, "bytesPerPixel: %i\n",  bytesPerPixel );
	//for( unsigned int i = 0; i < 3; i++ ){
		mimage->data[0] = (uint8_t *)(image->data);
		mimage->linesize[0] = width*bytesPerPixel;
	//}

	mimage->privateData = image;
        switch( screenDepth ){
                case 16:
                        mimage->chroma = M_CHROMA_RV16;
                        break;
                case 24:
                case 32:
                default:
                        mimage->chroma = M_CHROMA_RV32;
                        break;
        }

	return mimage;
}

void X11Display::deallocateImage( MImage * mimage ){
	XImage * image = (XImage *)mimage->privateData;

	XFree( image );
	
	delete mimage;
	
}

bool X11Display::handlesChroma( uint32_t chroma ){
        switch( screenDepth ){
                case 16:
                        return chroma == M_CHROMA_RV16;
                case 24:
                case 32:
                        return chroma == M_CHROMA_RV32;
                default:
                        return false;
        }
}


void X11Display::displayImage( MImage * mimage ){

        mdbg << "Called X11Display::displayImage" << endl;

	XPutImage( display, videoWindow, gc,
                    (XImage*)(mimage->privateData),
                    0 /*src_x*/, 0 /*src_y*/,
                    0 /*dest_x*/, 0 /*dest_y*/,
                    baseWindowWidth, baseWindowHeight );
}

uint32_t X11Display::getRequiredWidth(){
	return width;
}

uint32_t X11Display::getRequiredHeight(){
	return height;
}

void X11Display::resize(int w, int h){
	stop();
	init(w,h);
	start();
}

void X11Display::handleEvents(){
	XEvent xEvent;
	
	while( XCheckWindowEvent( display, baseWindow, 
			StructureNotifyMask | KeyPressMask, &xEvent ) == True ){

		if( xEvent.type == ConfigureNotify ){
			mdbg << "Got ConfigureNotify event" << endl;
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
			mdbg << "KeyPressed event" << endl;
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

void X11Display::toggleFullscreen(){
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
