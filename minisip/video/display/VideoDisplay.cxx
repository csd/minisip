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
#include"VideoDisplay.h"
#define NB_IMAGES 3

VideoDisplay::VideoDisplay(){
	show = false;
	emptyImagesLock.lock();
	Thread( this );
}

void VideoDisplay::start(){
	show = true;
}

void VideoDisplay::stop(){
	show = false;
	//FIXME
	filledImagesCond.broadcast();
}

void VideoDisplay::showWindow(){
        uint32_t i;
        MImage * mimage;

        createWindow();

        for( i = 0; i < NB_IMAGES; i++ ){
                mimage = allocateImage();
                emptyImages.push_back( mimage );
        }
}


/* The lock on emptyImages should always have been taken */
void VideoDisplay::hideWindow(){
        list<MImage *>::iterator i;

        while( ! emptyImages.empty() ){
                deallocateImage( *emptyImages.begin() );
                emptyImages.pop_front();
        }

        destroyWindow();

}


MImage * VideoDisplay::provideImage(){
        MImage * ret;


        // The decoder is running, wake the display if
        // it was sleeping
        showCond.broadcast();

        emptyImagesLock.lock();
        if( emptyImages.empty() ){

                emptyImagesLock.unlock();
                emptyImagesCond.wait();

                emptyImagesLock.lock();
        }

        ret = *emptyImages.begin();
        emptyImages.pop_front();

        emptyImagesLock.unlock();

        return ret;
}


void VideoDisplay::run(){
        MImage * imageToDisplay;

        while( true ){

		fprintf( stderr, "starting display main loop\n");
                showCond.wait();
                showWindow();


                emptyImagesLock.unlock();

                while( show ){

			handleEvents();

                        filledImagesLock.lock();

                        if( filledImages.empty() ){
                                filledImagesLock.unlock();

                                filledImagesCond.wait();
                                if( !show ){
                                        break;

                                }

                                filledImagesLock.lock();
                        }

                        imageToDisplay = *filledImages.begin();

                        filledImages.pop_front();

                        filledImagesLock.unlock();

                        displayImage( imageToDisplay );


                        emptyImagesLock.lock();
                        emptyImages.push_back( imageToDisplay );
                        emptyImagesLock.unlock();

                        emptyImagesCond.broadcast();
                }

                emptyImagesLock.lock();

                filledImagesLock.lock();

                while( ! filledImages.empty() ){
                        imageToDisplay = *filledImages.begin();

                        filledImages.pop_front();

                        displayImage( imageToDisplay );
                        
			emptyImages.push_back( imageToDisplay );

                }

                filledImagesLock.unlock();

                hideWindow();

	}
}

void VideoDisplay::handle( MImage * mimage ){

        filledImagesLock.lock();
        filledImages.push_back( mimage );
        filledImagesLock.unlock();

        filledImagesCond.broadcast();

}

bool VideoDisplay::providesImage(){
	return true;
}

void VideoDisplay::releaseImage( MImage * mimage ){
}

