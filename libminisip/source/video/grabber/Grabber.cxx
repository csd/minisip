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
#include<libminisip/video/VideoMedia.h>
#include<libminisip/video/VideoException.h>
#include<libminisip/codecs/Codec.h>
#include<libminisip/video/display/VideoDisplay.h>
#include<libminisip/video/codec/AVDecoder.h>
#include<libminisip/video/grabber/Grabber.h>
#include<libminisip/video/grabber/Grabber.h>

#ifdef DC1394_SUPPORT
#include<libminisip/video/grabber/Grabber.h>
#endif

using namespace std;



MRef<Grabber *> Grabber::create( string device ){
	MRef<Grabber *> result;
        try{

#ifdef DC1394_SUPPORT
	if( device.substr( 0, 3 ) == "fw:" ){
		uint32_t portId = atoi( device.substr( 3, 1 ).c_str() );
		uint32_t cameraId = atoi( device.substr( 5, 1 ).c_str() );
		
		Dc1394Grabber * dc1394Grabber = new Dc1394Grabber( portId, cameraId );
		result = (Grabber *)dc1394Grabber;
		return result;
	}
#endif

	if( device != "" ){
		V4LGrabber * v4lGrabber = new V4LGrabber( device );
		result = (Grabber*)v4lGrabber;
	}
        }
        catch( VideoException & exc ){
                merr << exc.error() << end;
                return NULL;
        }

	return result;
}
