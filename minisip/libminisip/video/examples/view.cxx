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


#include"../display/XvDisplay.h"
//#include"../grabber/Dc1394Grabber.h"
#include"../grabber/V4LGrabber.h"
#include"../mixer/ImageMixer.h"


int main(){
	//Dc1394Grabber g( 0, 0 );
        V4LGrabber g( "/dev/video0" );
	XvDisplay display( 352, 288 );
	ImageMixer mixer;


	g.setMixer( &mixer );

//	g.setHandler( &mixer );
	g.setHandler( &display );

	mixer.setOutput( &display );

	g.open();
	g.printCapabilities();
	g.printImageFormat();

        

        g.setImageChroma( M_CHROMA_RV32 );

	mixer.init( 352, 288 );

//	display.init( g.getHeight(), g.getWidth() );

	display.start();
	g.run();
}
