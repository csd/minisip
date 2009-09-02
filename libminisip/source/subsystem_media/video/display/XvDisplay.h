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

#ifndef XVDISPLAY_INCLUDE_HEADER
#define XVDISPLAY_INCLUDE_HEADER

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/ImageHandler.h>

#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>

#include<libminisip/media/video/display/VideoDisplay.h>
#include"X11Display.h"

class XvDisplay: public X11Display{
	public: 
		XvDisplay( uint32_t width, uint32_t height );

		/* From ImageHandler */
		virtual void init( uint32_t height,  uint32_t width );
		virtual bool handlesChroma( uint32_t chroma );
		virtual uint32_t getRequiredWidth();
		virtual uint32_t getRequiredHeight();

	private:

		virtual void openDisplay();
//		virtual void createWindow();
		virtual void destroyWindow();
//		virtual void handleEvents();

		virtual MImage * allocateImage();
		virtual void deallocateImage( MImage * image );

		virtual void displayImage( MImage * image );

//		void toggleFullscreen();

		int xvPort;
//		Display * display;
//		int screen;
//		Window baseWindow;
//		Window videoWindow;

//		GC gc;

		bool fullscreen;

};

class XvPlugin: public VideoDisplayPlugin{
	public:
		XvPlugin( MRef<Library *> lib ): VideoDisplayPlugin( lib ){}
		
		virtual std::string getMemObjectType() const { return "XvPlugin"; }

		virtual std::string getName() const { return "xv"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "XVideo display"; }

		virtual MRef<VideoDisplay *> create( uint32_t width, uint32_t height, bool fullscreen ) const{
			return new XvDisplay( width, height );
		}
};


#endif
