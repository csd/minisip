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

#include<config.h>
#include<libmutil/dbg.h>
#include<libminisip/media/video/VideoMedia.h>
#include<libminisip/media/video/VideoException.h>
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/video/display/VideoDisplay.h>
#include<libminisip/media/video/codec/AVDecoder.h>
#include<libminisip/media/video/grabber/Grabber.h>
#ifdef HAVE_LINUX_VIDEODEV_H
#include"V4LGrabber.h"
#endif

using namespace std;


GrabberPlugin::GrabberPlugin( MRef<Library *> lib ){
}

GrabberPlugin::~GrabberPlugin(){
}


GrabberRegistry::GrabberRegistry(){
#ifdef HAVE_LINUX_VIDEODEV_H
	registerPlugin( new V4LPlugin( NULL ) );
#endif
}

MRef<Grabber *> GrabberRegistry::createGrabber( string device ){
	MRef<Grabber *> result;

        try{
		size_t pos = device.find(':');
		string name;
		string dev;

		if( pos == string::npos ){
			name = "v4l";
			dev = device;
		}
		else{
			name = device.substr( 0, pos );
			dev = device.substr( pos + 1 );
		}

		MRef<MPlugin *> plugin = findPlugin( name );

		if( !plugin ) {
			merr << "GrabberRegistry: " << name << " grabber not found " << endl;
			return NULL;
		}

		MRef<GrabberPlugin *> gPlugin = dynamic_cast<GrabberPlugin *>(*plugin);
		result = gPlugin->create( dev );
        }
        catch( VideoException & exc ){
                merr << exc.error() << endl;
                return NULL;
        }

	return result;
}

