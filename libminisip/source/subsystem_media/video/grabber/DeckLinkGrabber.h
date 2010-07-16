/*
 Copyright (C) 2010 Erik Eliasson
 
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

/* Copyright (C) 2010
 *
 * Authors: Erik Eliasson <ere@kth.se>
*/

#ifndef DECKLINKGRABBER_INCLUDE_HEADER
#define DECKLINKGRABBER_INCLUDE_HEADER

#include<libminisip/libminisip_config.h>

#include<stdint.h>
#include<libmutil/Mutex.h>
#include<libmutil/Semaphore.h>
#include<libminisip/media/video/grabber/Grabber.h>

#include"decklinksdk/DeckLinkAPI.h"

class ImageHandler;

//#define NB_GRAB_MAX 3

#if 0
typedef struct {
	MIL_ID        *MilImage;
	MIL_ID        MilDigitizer;
	MIL_ID        GrabEndEvent;
	int 	      stopped;
	long          NbFrames;
	long          NbGrabStart;
	MIL_ID   MilApplication;
	MIL_ID   MilSystem;
} UserDataStruct;
#endif


class DeckLinkCaptureDelegate : public IDeckLinkInputCallback {
	public:
		DeckLinkCaptureDelegate(int fps);
		
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
		virtual ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
		virtual ULONG STDMETHODCALLTYPE  Release(void) { return 1; }
		virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
		virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);
		MImage* getImage();
		int getWidth(){return width;}
		int getHeight(){return height;}
		void stop(){doStop=true;}
		bool doStop;
	private:
		void allocateImage();
		long unsigned int frameCount;

		void putImage(IDeckLinkVideoInputFrame* videoFrame);
		


		Mutex bufferLock;
		Semaphore bufferSem;
		//image *
		bool filled;
		int width;
		int height;
		MImage *frame;

		int fps;
		uint64_t nextTimeGrab;
};



class DeckLinkGrabber : public Grabber{
	public:
		DeckLinkGrabber( std::string deviceId );

		void open();
		void getCapabilities();
		bool setImageChroma( uint32_t chroma );


		void read( ImageHandler * );
		//void read();
		virtual void run();
		virtual void stop();
		virtual void start();

		virtual void close();

		uint32_t getHeight();//{ return height; };
		uint32_t getWidth();//{ return width; };

		void setHandler( ImageHandler * handler );

		virtual void setLocalDisplay(MRef<VideoDisplay*>);

	private:
		void init();
		void displayLocal(MImage* frame);
		MImage* localRgb;

		bool doStop;

		DeckLinkCaptureDelegate* capture;

		IDeckLinkInput                  *deckLinkInput;
		IDeckLinkIterator           *deckLinkIterator;
		IDeckLink                       *deckLink;
		IDeckLinkDisplayModeIterator    *displayModeIterator;

		std::string device;
//		MIL_ID   MilImage[NB_GRAB_MAX];
//		MIL_TEXT_CHAR FrameIndex[10];
//		UserDataStruct UserStruct;


//		uint32_t height;
//		uint32_t width;

		Mutex grabberLock;
		ImageHandler * handler;
		MRef<VideoDisplay*> localDisplay;
		
		bool initialized;
		MRef<Thread*> runthread;
		MRef<Semaphore*> startBlockSem;
		MRef<Semaphore*> initBlockSem;
		int fps;
		int deviceno;
};

class DeckLinkPlugin : public GrabberPlugin{
	public:
		DeckLinkPlugin( MRef<Library *> lib ) : GrabberPlugin( lib ){}

		virtual MRef<Grabber *> create( const std::string &device ) const;

		virtual std::string getMemObjectType() const { return "DeckLinkPlugin"; }

		virtual std::string getName() const { return "decklink"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "DeckLink grabber"; }

};

#endif
