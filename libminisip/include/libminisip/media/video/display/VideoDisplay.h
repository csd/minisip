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

#ifndef VIDEO_DISPLAY_H
#define VIDEO_DISPLAY_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmutil/Mutex.h>
#include<libmutil/CondVar.h>
#include<libmutil/Semaphore.h>
#include<libmutil/MPlugin.h>
#include<libmutil/MSingleton.h>
#include<libmutil/CommandString.h>
#include<libmutil/MessageRouter.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

#include<libminisip/media/video/ImageHandler.h>

class LIBMINISIP_API VideoDisplay : public ImageHandler, public Runnable{
	public:
		virtual std::string getMemObjectType() const { return "VideoDisplay"; };
		~VideoDisplay();
		virtual void start();
		virtual void stop();

		virtual void run();

		/* From ImageHandler */
		virtual void handle( MImage * );
		virtual void setIsLocalVideo( bool isLocal );
		virtual bool getIsLocalVideo();
		virtual MImage * provideImage();
		virtual void releaseImage( MImage * image );
		virtual bool providesImage();

		virtual bool handleCommand(CommandString cmd);

		virtual void setCallback(MRef<CommandReceiver*> cb);

//		virtual void setTitle(std::string t);
		virtual void setCallId(std::string id);
//		virtual void setRemoteUri(std::string u);
			
		virtual void setPhoneConfig(MRef<SipSoftPhoneConfiguration*> conf);
		
	protected:
		VideoDisplay();
		
		virtual void showWindow();
		virtual void hideWindow();
		
		// Need to be implemented by the subclass
		virtual void createWindow()=0;
		virtual void destroyWindow()=0;
		virtual void handleEvents()=0;
		virtual void displayImage( MImage * image )=0;
		virtual MImage * allocateImage()=0;
		virtual void deallocateImage( MImage * image )=0;

		bool isLocalVideo; //Local video might get other treatment. For example, handle() will be called
					// with a MImage that was not allocated using allocateImage()

		MRef<CommandReceiver*> callback;

	private:
		std::list<MImage *> filledImages;
		Mutex filledImagesLock;

		std::list<MImage *> emptyImages;
		Mutex emptyImagesLock;
		
		std::list<MImage *> allocatedImages;

		Semaphore emptyImagesSem;
		Semaphore filledImagesSem;

		CondVar showCond;
		Mutex showCondLock;
		bool show;

		Thread * thread;
		MRef<Semaphore *> sem;

};

class LIBMINISIP_API VideoDisplayPlugin : public MPlugin{
	public:
		virtual std::string getPluginType() const;

		VideoDisplayPlugin( MRef<Library *> lib );

		virtual MRef<VideoDisplay *> create( uint32_t width, uint32_t height, bool fullscreen) const = 0;
};

/**
 * Registry of video display plugins.
 */
class LIBMINISIP_API VideoDisplayRegistry: public MPluginRegistry, public MSingleton<VideoDisplayRegistry>{
	public:
		virtual std::string getPluginType(){ return "VideoDisplay"; }

		MRef<VideoDisplay*> createDisplay( uint32_t width, uint32_t height, bool doStart, bool fullscreen );
		void signalDisplayDeleted();

	protected:
		VideoDisplayRegistry();

	private:
		uint32_t displayCounter;
		Mutex displayCounterLock;

		friend class MSingleton<VideoDisplayRegistry>;
};

#endif
