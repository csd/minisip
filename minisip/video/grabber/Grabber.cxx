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
#include"../mixer/ImageMixer.h"
#include"Grabber.h"
#include"V4LGrabber.h"

#ifdef DC1394_SUPPORT
#include"Dc1394Grabber.h"
#endif

using namespace std;



MRef<Grabber *> Grabber::create( string device ){
	MRef<Grabber *> result;

#ifdef DC1394_SUPPORT
	if( device.substr( 0, 3 ) == "fw:" ){
		uint32_t portId = atoi( device.substr( 3, 1 ).c_str() );
		uint32_t cameraId = atoi( device.substr( 5, 1 ).c_str() );

		fprintf( stderr, "portId: %i\n", portId );
		fprintf( stderr, "cameraId: %i\n", cameraId );
		
		Dc1394Grabber * dc1394Grabber = new Dc1394Grabber( portId, cameraId );
		fprintf( stderr, "After new\n" );
		fprintf( stderr, "dc1394Grabber: %x\n", dc1394Grabber );
		result = (Grabber *)dc1394Grabber;
		return result;
	}
#endif

	if( device != "" ){
		V4LGrabber * v4lGrabber = new V4LGrabber( device );
		result = (Grabber*)v4lGrabber;
	}

	return result;
}

void Grabber::setMixer( MRef<ImageMixer *> mixer ){
	this->mixer = mixer;
}
