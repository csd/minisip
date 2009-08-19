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

#include"SdlDisplay.h"
#include<config.h>
#include<sys/time.h>
#include<SDL/SDL_syswm.h>

#include<iostream>

using namespace std;

#define NB_IMAGES 3


static std::list<std::string> pluginList;
static MRef<MPlugin *> plugin;
static bool initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *msdl_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * msdl_LTX_getPlugin( MRef<Library*> lib ){
	return new SdlPlugin( lib );
}


SdlDisplay::SdlDisplay( uint32_t width, uint32_t height):VideoDisplay(){
	this->width = width;
	this->height = height;
	baseWindowWidth = this->width;
	baseWindowHeight = this->height;
	fullscreen = false;
}

void SdlDisplay::openDisplay(){
	
}
		
void SdlDisplay::init( uint32_t width, uint32_t height ){
	baseWindowWidth = this->width = width;
	baseWindowHeight = this->height = height;
}

void SdlDisplay::createWindow(){
	
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD |SDL_INIT_NOPARACHUTE) < 0 ){
		fprintf( stderr, "Could not initialize SDL: %s\n",
				SDL_GetError() );
		exit( 1 );
	}

//        uint32_t flags;
//        int bpp;

        flags = SDL_ANYFORMAT | SDL_HWPALETTE | SDL_HWSURFACE |
                SDL_DOUBLEBUF | SDL_RESIZABLE;

        bpp = SDL_VideoModeOK( baseWindowWidth, baseWindowHeight, 16, flags );

        if( bpp == 0 ){
                fprintf( stderr, "Could not find an SDL video mode\n" );
                exit( 1 );
        }

        surface = SDL_SetVideoMode( baseWindowWidth, baseWindowHeight, bpp, flags );

        if( surface == NULL ){
                fprintf( stderr, "Could not set SDL video mode\n" );
                exit( 1 );
        }

	initWm();


        SDL_WM_SetCaption( "minisip video", "minisip video" );


        SDL_LockSurface( surface );
}

void SdlDisplay::initWm(){
#ifdef IPAQ
	if( !fullscreen ){
		int ret;
		SDL_SysWMinfo wmInfo;

		SDL_VERSION( &wmInfo.version );
		ret = SDL_GetWMInfo( &wmInfo );

		if( ret > 0 ){
			XClientMessageEvent event;
			display = wmInfo.info.x11.display;
			window = wmInfo.info.x11.wmwindow;
			Atom type = XInternAtom( display, "_NET_WM_WINDOW_TYPE", False );
			Atom typeDialog = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False );

			XChangeProperty( display, window, type,
					XA_ATOM, 32, PropModeReplace,
					(unsigned char *) &typeDialog, 1);



			XUnmapWindow( display, window );
			XResizeWindow( display, window, width, height );
			XMapWindow( display, window );
		}
		else{
			fprintf( stderr, "SDL Error when getting WM info: %s\n",
					SDL_GetError() );
		}
	}
#endif
}

void SdlDisplay::destroyWindow(){
	SDL_UnlockSurface( surface );

	SDL_FreeSurface( surface );

	SDL_QuitSubSystem( SDL_INIT_VIDEO );

}

MImage * SdlDisplay::allocateImage(){
 	
	SDL_Overlay  * overlay = SDL_CreateYUVOverlay( width, height, 
			SDL_YV12_OVERLAY, surface );

        if( overlay == NULL ){
                fprintf( stderr, "Could not create SDL I420 overlay\n" );
                exit( 1 );
        }

        SDL_LockYUVOverlay( overlay );

        MImage * mimage = new MImage;

        mimage->data[0] = overlay->pixels[0];
        mimage->data[1] = overlay->pixels[2];
        mimage->data[2] = overlay->pixels[1];

        mimage->linesize[0] = overlay->pitches[0];
        mimage->linesize[1] = overlay->pitches[2];
        mimage->linesize[2] = overlay->pitches[1];
	mimage->width=width;
	mimage->height=height;

        mimage->privateData = overlay;

	return mimage;
	
	
}

void SdlDisplay::deallocateImage( MImage * mimage ){
	
	SDL_Overlay * overlay = (SDL_Overlay *)mimage->privateData;

        SDL_UnlockYUVOverlay( overlay );
	SDL_FreeYUVOverlay( overlay );

	delete mimage;
}

bool SdlDisplay::handlesChroma( uint32_t chroma ){
	return chroma == M_CHROMA_I420;
}

void SdlDisplay::displayImage( MImage * mimage ){

	SDL_Overlay * overlay = (SDL_Overlay *)mimage->privateData;
	massert( overlay );
	SDL_Rect rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = baseWindowWidth;
	rect.h = baseWindowHeight;

	SDL_UnlockYUVOverlay( overlay );
	SDL_DisplayYUVOverlay( overlay , &rect );
	SDL_LockYUVOverlay( overlay);
}

void SdlDisplay::handleEvents(){
	SDL_Event event;

	while( SDL_PollEvent( &event ) ){
		if( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f || event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT ){
#ifdef IPAQ
			if( fullscreen ){
				cerr << "not fullscreen" << endl;
				baseWindowWidth = width;
				baseWindowHeight = height;
				fullscreen = false;
			}
			else{
				cerr << "fullscreen" << endl;
				baseWindowWidth = 240;
				baseWindowHeight = 180;
				fullscreen = true;
			}

                	//XResizeWindow( display, window, baseWindowWidth, baseWindowHeight );
			//XSync( display, false );
			SDL_UnlockSurface( surface );

			SDL_FreeSurface( surface );
			surface = SDL_SetVideoMode( baseWindowWidth, 
				baseWindowHeight, bpp, flags);
			initWm();
			SDL_LockSurface( surface );	
//			destroyWindow();
//			createWindow();

			
#endif
			if( fullscreen ){
				SDL_WM_ToggleFullScreen( surface );
			}
			break;
		}
		else if( event.type == SDL_VIDEORESIZE ){
			//XSync( display, false );
			baseWindowWidth = event.resize.w;
			baseWindowHeight = event.resize.h;

			SDL_UnlockSurface( surface );
			SDL_FreeSurface( surface );
			surface = SDL_SetVideoMode( baseWindowWidth, 
				baseWindowHeight, bpp, flags);
//			initWm();
			SDL_LockSurface( surface );	
		}

	}
}


SdlPlugin::SdlPlugin( MRef<Library *> lib ): VideoDisplayPlugin( lib ){
}

SdlPlugin::~SdlPlugin(){
}
