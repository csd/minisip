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

#ifndef DC1394GRABBER_INCLUDE_HEADER
#define DC1394GRABBER_INCLUDE_HEADER

#include<libminisip/libminisip_config.h>

#include<stdint.h>
#include<libraw1394/raw1394.h>
#include<libdc1394/dc1394_control.h>
#include<libmutil/Mutex.h>
#include<libminisip/media/video/grabber/Grabber.h>

#define MAX_PORTS   4
#define MAX_CAMERAS 8

class ImageHandler;

class Dc1394Grabber : public Grabber{
	public:
		Dc1394Grabber( uint32_t portId, uint32_t cameraId );

		void open();
		void getCapabilities();
		void printCapabilities();
		void printImageFormat();
		void getImageFormat();
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
		dc1394_cameracapture camera;
		raw1394handle_t cameraHandle;
		uint32_t portId;
		uint32_t cameraId;

		uint32_t height;
		uint32_t width;

		bool stopped;

		Mutex grabberLock;
		ImageHandler * handler;
		bool handlerProvidesImage;

		MImage * oldImage;
		MRef<Thread*> runthread;

};

class Dc1394Plugin : public GrabberPlugin{
	public:
		Dc1394Plugin( MRef<Library *> lib ) : GrabberPlugin( lib ){}

		virtual MRef<Grabber *> create( const std::string &device ) const;

		virtual std::string getMemObjectType() const { return "Dc1394Plugin"; }

		virtual std::string getName() const { return "fw"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "Video4linux grabber"; }

};

#endif
