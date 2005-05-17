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

#include<config.h>
#include<libmutil/dbg.h>
#include"VideoDisplay.h"
#include"../VideoException.h"
#include"XvDisplay.h"
#ifdef SDL_SUPPORT
#include"SdlDisplay.h"
#endif

#include<iostream>
#define NB_IMAGES 3

uint32_t VideoDisplay::displayCounter;
Mutex VideoDisplay::displayCounterLock;

MRef<VideoDisplay *> VideoDisplay::create( uint32_t width, uint32_t height ){
        MRef<VideoDisplay *> display = NULL;
        
        VideoDisplay::displayCounterLock.lock();

        
        if( VideoDisplay::displayCounter == 0 ){
                try{
#ifdef SDL_SUPPORT
                display = new SdlDisplay( width, height );
#else
                display =  new XvDisplay( width, height );
#endif
                }
                catch( VideoException exc ){
                        mdbg << "Error opening the video display: "
                             << exc.error() << end;
                        goto X11;

                }
        }


        else{
X11:
                try{
                display = new X11Display( width, height );
                }
                catch( VideoException exc ){
                        merr << "Error opening the video display: "
                             << exc.error() << end;
                }
        }

        displayCounter ++;
        displayCounterLock.unlock();
        return display;
}

VideoDisplay::VideoDisplay(){
	show = false;
//	emptyImagesLock.lock();
	thread = new Thread( this );        
        show = true;
}

VideoDisplay::~VideoDisplay(){
        thread->join();
        VideoDisplay::displayCounterLock.lock();
        VideoDisplay::displayCounter --;
        VideoDisplay::displayCounterLock.unlock();

}

void VideoDisplay::start(){
	show = true;
}

void VideoDisplay::stop(){
	show = false;
	//FIXME
        filledImagesSem.inc();
        filledImagesCondLock.lock();
	filledImagesCond.broadcast();
        filledImagesCondLock.unlock();
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
        list<MImage *>::iterator i;

        fprintf( stderr, "Started hideWindow()\n" );
        
        while( ! emptyImages.empty() ){
                emptyImages.pop_front();
                emptyImagesSem.dec();
        }
        fprintf( stderr, "After emptyImages\n" );

        while( ! allocatedImages.empty() ){
                deallocateImage( *allocatedImages.begin() );
                allocatedImages.pop_front();
        }
        fprintf( stderr, "After allocatedImages\n" );

        destroyWindow();

}


MImage * VideoDisplay::provideImage(){
        MImage * ret;


        // The decoder is running, wake the display if
        // it was sleeping
	cerr << "Before taking showCondLock" << endl;
        showCondLock.lock();
        showCond.broadcast();
        showCondLock.unlock();
	cerr << "After releasing showCondLock" << endl;

        emptyImagesSem.dec();
	cerr << "Before taking emptyImagesLock" << endl;
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
	cerr << "After releasing emptyImagesLock" << endl;

        return ret;
}


void VideoDisplay::run(){
        MImage * imageToDisplay;

//        while( true ){

		fprintf( stderr, "starting display main loop\n");
                
        //        showCondLock.lock();
          //      showCond.wait( &showCondLock );
            //    showCondLock.unlock();

                showWindow();


//                emptyImagesLock.unlock();

                while( show ){

                    handleEvents();

                        /*

                        if( filledImages.empty() ){
                                filledImagesLock.unlock();

                                filledImagesCondLock.lock();
                                filledImagesCond.wait( &filledImagesCondLock );
                                filledImagesCondLock.unlock();

                                if( !show ){
                                        break;

                                }

                                filledImagesLock.lock();
                        }
                        */
                    if( !show ){
                        cerr << "Exiting display loop" << endl;
                        break;

                    }
                    fprintf( stderr, "Before filledImagesSem\n" );
                    filledImagesSem.dec();
                    fprintf( stderr, "After filledImagesSem\n" );
                    if( !show ){
                        cerr << "Exiting display loop" << endl;
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
                            /*
                               emptyImagesCondLock.lock();
                               emptyImagesCond.broadcast();
                               emptyImagesCondLock.unlock();
                               */
                    }
                }

                emptyImagesLock.lock();

                filledImagesLock.lock();

                while( ! filledImages.empty() ){
                    //filledImagesSem.dec();
                    imageToDisplay = *filledImages.begin();

                    filledImages.pop_front();

                    displayImage( imageToDisplay );

                    
                   //emptyImages.push_back( imageToDisplay );
                   // emptyImagesSem.inc();

                }

                filledImagesLock.unlock();

                hideWindow();

//        }
}

void VideoDisplay::handle( MImage * mimage ){

cerr << "Called VideoDisplay::handle" << endl;
        filledImagesLock.lock();
        filledImages.push_back( mimage );
cerr << "before filledImagesSem.inc()" << endl;
        filledImagesSem.inc();
        filledImagesLock.unlock();
/*
        filledImagesCondLock.lock();
        filledImagesCond.broadcast();
        filledImagesCondLock.unlock();
*/
cerr << "ended VideoDisplay::handle" << endl;
}

bool VideoDisplay::providesImage(){
	return true;
}

void VideoDisplay::releaseImage( MImage * mimage ){
}

