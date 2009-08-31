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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef MatroxGRABBER_INCLUDE_HEADER
#define MatroxGRABBER_INCLUDE_HEADER

#include<libminisip/libminisip_config.h>

#include<stdint.h>
#include<libmutil/Mutex.h>
#include<libmutil/Semaphore.h>
#include<libminisip/media/video/grabber/Grabber.h>

#include<mil.h>

class ImageHandler;

#define NB_GRAB_MAX 3

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



class MatroxGrabber : public Grabber{
	public:
		MatroxGrabber( std::string deviceId );

		void open();
		void getCapabilities();
		bool setImageChroma( uint32_t chroma );


		void read( ImageHandler * );
		//void read();
		virtual void run();
		virtual void stop();
		virtual void start();

		virtual void close();

		uint32_t getHeight(){ return height; };
		uint32_t getWidth(){ return width; };

		void setHandler( ImageHandler * handler );

		virtual void setLocalDisplay(MRef<VideoDisplay*>);

	private:
		void init();

		std::string device;
		MIL_ID   MilImage[NB_GRAB_MAX];
		MIL_TEXT_CHAR FrameIndex[10];
		UserDataStruct UserStruct;


		uint32_t height;
		uint32_t width;

		Mutex grabberLock;
		ImageHandler * handler;
		MRef<VideoDisplay*> localDisplay;
		
		bool initialized;
		MRef<Thread*> runthread;
		MRef<Semaphore*> startBlockSem;
		MRef<Semaphore*> initBlockSem;
};

class MatroxPlugin : public GrabberPlugin{
	public:
		MatroxPlugin( MRef<Library *> lib ) : GrabberPlugin( lib ){}

		virtual MRef<Grabber *> create( const std::string &device ) const;

		virtual std::string getMemObjectType() const { return "MatroxPlugin"; }

		virtual std::string getName() const { return "matrox"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "Matrox grabber"; }

};

#endif
