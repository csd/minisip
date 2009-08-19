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

#include<config.h>
#include<libmutil/dbg.h>
#include<libminisip/media/video/display/VideoDisplay.h>
#include<libminisip/media/video/VideoException.h>

#include<iostream>
#define NB_IMAGES 3

using namespace std;

VideoDisplayRegistry::VideoDisplayRegistry(): displayCounter( 0 ){
}

MRef<VideoDisplay *> VideoDisplayRegistry::createDisplay( uint32_t width, uint32_t height ){
        MRef<VideoDisplay *> display = NULL;
	const char *names[] = { "sdl", "xv", "x11", NULL };
        
        displayCounterLock.lock();

	for( int i = 0;; i++ ){
		if( !names[i] ){
			break;
		}

		string name = names[i];

		if( displayCounter != 0 &&
		    ( name == "sdl" || name == "xv" ) ){
			continue;
		}

		try{
			MRef<MPlugin *> plugin;
			plugin = findPlugin( name );
			if( !plugin ){
				mdbg << "VideoDisplayRegistry: Can't find " << name << endl;
				continue;
			}
			
			MRef<VideoDisplayPlugin *> videoPlugin;
			
			videoPlugin = dynamic_cast<VideoDisplayPlugin*>( *plugin );
			if( !videoPlugin ){
				merr << "VideoDisplayPlugin: Not display plugin " << name << endl;
				continue;
			}
			
			display = videoPlugin->create( width, height );
			
			if( !display ){
				merr << "VideoDisplayPlugin: Couldn't create display " << name << endl;
				continue;
			}
			
			display->start();
			displayCounter ++;
			break;
		}
		catch( VideoException & exc ){
			mdbg << "Error opening the video display: "
			     << exc.error() << endl;
		}
	}

        displayCounterLock.unlock();
        return display;
}

void VideoDisplayRegistry::signalDisplayDeleted(){
	displayCounterLock.lock();
        displayCounter --;
        displayCounterLock.unlock();

}


VideoDisplay::VideoDisplay(){
	show = false;
//	emptyImagesLock.lock();
        show = true;
}

VideoDisplay::~VideoDisplay(){
//        thread->join();
	VideoDisplayRegistry::getInstance()->signalDisplayDeleted();
}

void VideoDisplay::start(){
	show=true;
        showWindow();
	thread = new Thread( this );        
}

void VideoDisplay::stop(){
	show = false;
	//FIXME
        filledImagesSem.inc();
	thread->join();
}

void VideoDisplay::showWindow(){
        uint32_t i;
        MImage * mimage;

        createWindow();

        if( providesImage() ){
                for( i = 0; i < NB_IMAGES; i++ ){
                        mimage = allocateImage();
                        allocatedImages.push_back( mimage );
                        emptyImagesLock.lock();
                        emptyImages.push_back( mimage );
                        emptyImagesLock.unlock();
                        emptyImagesSem.inc();
                }
        }
}


/* The lock on emptyImages should always have been taken */
void VideoDisplay::hideWindow(){
        while( ! emptyImages.empty() ){
                emptyImages.pop_front();
                emptyImagesSem.dec();
        }

        while( ! allocatedImages.empty() ){
                deallocateImage( *allocatedImages.begin() );
                allocatedImages.pop_front();
        }

        destroyWindow();
}


MImage * VideoDisplay::provideImage(){
        MImage * ret;


        // The decoder is running, wake the display if
        // it was sleeping
        showCondLock.lock();
        showCond.broadcast();
        showCondLock.unlock();

        emptyImagesSem.dec();
        emptyImagesLock.lock();
        /*
        emptyImagesLock.lock();
        if( emptyImages.empty() ){

                emptyImagesLock.unlock();

                emptyImagesCondLock.lock();
                emptyImagesCond.wait( &emptyImagesCondLock );
                emptyImagesCondLock.unlock();

                emptyImagesLock.lock();
        }

        */

        ret = *emptyImages.begin();
        emptyImages.pop_front();

        emptyImagesLock.unlock();

        return ret;
}


void VideoDisplay::run(){
#ifdef DEBUG_OUTPUT
	setThreadName("VideoDisplay::run");
#endif
        MImage * imageToDisplay;


        while( show ){

                handleEvents();

                if( !show ){
                        break;

                }
                filledImagesSem.dec();
                if( !show ){
                        break;

                }

                filledImagesLock.lock();

                imageToDisplay = *filledImages.begin();

                filledImages.pop_front();

                filledImagesLock.unlock();

                displayImage( imageToDisplay );


                if( providesImage() ){
                        emptyImagesLock.lock();

                        emptyImagesSem.inc();
                        emptyImages.push_back( imageToDisplay );

                        emptyImagesLock.unlock();
                }
        }

        emptyImagesLock.lock();

        filledImagesLock.lock();

        while( ! filledImages.empty() ){
                imageToDisplay = *filledImages.begin();

                filledImages.pop_front();

                displayImage( imageToDisplay );

        }

        filledImagesLock.unlock();

        emptyImagesLock.unlock();

        hideWindow();

}

void VideoDisplay::handle( MImage * mimage ){

        filledImagesLock.lock();
        filledImages.push_back( mimage );
        filledImagesSem.inc();
        filledImagesLock.unlock();
}

bool VideoDisplay::providesImage(){
	return true;
}

void VideoDisplay::releaseImage( MImage * mimage ){
}

VideoDisplayPlugin::VideoDisplayPlugin( MRef<Library *> lib ): MPlugin( lib ){
}

std::string VideoDisplayPlugin::getPluginType() const{
	return "VideoDisplay";
}
