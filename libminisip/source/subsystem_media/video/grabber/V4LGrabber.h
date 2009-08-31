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

#ifndef V4L_GRABBER_H
#define V4L_GRABBER_H

#include<libminisip/libminisip_config.h>

#include<string>
#include<stdint.h>
#include<libmutil/Mutex.h>

#include<linux/types.h>
#include<linux/videodev.h>
#include<libminisip/media/video/grabber/Grabber.h>
#define N_BUFFERS 20

struct v4l2_capability;

struct cardBuffer{
	uint8_t * start;
	size_t length;
};

class ImageHandler;

class V4LGrabber : public Grabber{
	public:
		V4LGrabber( std::string device );

		virtual void open();
		virtual void getCapabilities();
		virtual void printCapabilities();
		virtual void printImageFormat();
		virtual void getImageFormat();
		bool setImageChroma( uint32_t chroma );
		bool setImageSize( uint32_t width, uint32_t height );

		virtual void read( ImageHandler * );
		virtual void run();

		virtual void start();
		virtual void stop();
		virtual void close();

		uint32_t getHeight(){ return height; };
		uint32_t getWidth(){ return width; };

		virtual void setHandler( ImageHandler * handler );

		virtual void setLocalDisplay(MRef<VideoDisplay*>);

	private:
		/* V4L stuff */
		struct video_capability * v4lCapacity;
		struct video_picture * imageFormat;
		struct video_window * imageWindow;
		
		void mapMemory();
		void unmapMemory();
		
		int fd;
		std::string device;

		struct cardBuffer * buffers[ N_BUFFERS ];

		uint8_t nFrames;

		uint32_t height;
		uint32_t width;

		ImageHandler * handler;
		Mutex grabberLock;

		bool stopped;
		MRef<Thread*> runthread;

};


class V4LPlugin : public GrabberPlugin{
	public:
		V4LPlugin( MRef<Library *> lib ) : GrabberPlugin( lib ){}

		virtual MRef<Grabber *> create( const std::string &device ) const{
			return new V4LGrabber( device );
		}

		virtual std::string getName() const { return "v4l"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "Video4linux grabber"; }
		virtual std::string getMemObjectType() const { return "V4LPlugin"; }
};

#endif
