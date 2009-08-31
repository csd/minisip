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

#ifndef GRABBER_H
#define GRABBER_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/video/display/VideoDisplay.h>

#include<libmutil/MemObject.h>
#include<libmutil/MSingleton.h>
#include<libmutil/MPlugin.h>
#include<libmutil/Thread.h>

class ImageMixer;

class LIBMINISIP_API Grabber : public Runnable{
	public:
		virtual void open()=0;

		virtual bool setImageChroma( uint32_t chroma )=0;

		virtual void read( ImageHandler * )=0;
		virtual void run()=0;
		
		virtual void start()=0;
		virtual void stop()=0;

		virtual void close()=0;

		virtual void setHandler( ImageHandler * handler )=0;

		virtual void setLocalDisplay(MRef<VideoDisplay*>)=0;

		virtual std::string getMemObjectType() const {return "Grabber";}
		
};

class LIBMINISIP_API GrabberPlugin : public MPlugin{
	public:
		GrabberPlugin( MRef<Library *> lib );
		virtual ~GrabberPlugin();

		virtual std::string getPluginType() const{
			return "Grabber";
		}

		virtual MRef<Grabber *> create( const std::string &device ) const = 0;
};

class LIBMINISIP_API GrabberRegistry: public MPluginRegistry, public MSingleton<GrabberRegistry>{
	public:
		virtual std::string getPluginType(){ return "Grabber"; }

		MRef<Grabber*> createGrabber( std::string deviceName );

	protected:
		GrabberRegistry();

		friend class MSingleton<GrabberRegistry>;
};

#endif
