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

#ifndef VIDEO_DISPLAY_H
#define VIDEO_DISPLAY_H

#include"../ImageHandler.h"
#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmutil/Mutex.h>
#include<libmutil/CondVar.h>

class VideoDisplay : public ImageHandler, public MObject, public Runnable{
	public:
		virtual std::string getMemObjectType(){ return "VideoDisplay"; };
		virtual void start();
		virtual void stop();

		virtual void run();

		/* From ImageHandler */
		virtual void handle( MImage * );
		virtual MImage * provideImage();
		virtual void releaseImage( MImage * image );
		virtual bool providesImage();
		
	protected:
		VideoDisplay();
		
		virtual void showWindow();
		virtual void hideWindow();
		
		// Need to be implemented by the subclass
		virtual void createWindow()=0;
		virtual void destroyWindow()=0;
		virtual void displayImage( MImage * image )=0;
		virtual MImage * allocateImage()=0;
		virtual void deallocateImage( MImage * image )=0;
	private:
                std::list<MImage *> filledImages;
                CondVar filledImagesCond;
                Mutex filledImagesLock;

                std::list<MImage *> emptyImages;
                CondVar emptyImagesCond;
                Mutex emptyImagesLock;

		CondVar showCond;
		bool show;

};

#endif
