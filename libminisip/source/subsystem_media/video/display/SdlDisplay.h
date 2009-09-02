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

#ifndef SDL_DISPLAY_H
#define SDL_DISPLAY_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/display/VideoDisplay.h>

#include<SDL/SDL.h>
#include<X11/Xlib.h>

#include<libmutil/Thread.h>
#include<libmutil/Mutex.h>
#include<libmutil/CondVar.h>

class SdlDisplay: public VideoDisplay{
	public: 
		SdlDisplay( uint32_t width, uint32_t height );
		virtual void init( uint32_t height,  uint32_t width );
		virtual bool handlesChroma( uint32_t chroma );

	private:

		uint32_t height;
		uint32_t width;

		uint32_t baseWindowHeight;
		uint32_t baseWindowWidth;

		void openDisplay();
		virtual void createWindow();
		virtual void destroyWindow();

		virtual void displayImage( MImage * image );

		virtual MImage * allocateImage();
		virtual void deallocateImage( MImage * image );

		virtual void handleEvents();

		void initWm();

		Window window;
		Display * display;

		bool fullscreen;

		SDL_Surface * surface;

		uint32_t flags;
		int bpp;        

};

class SdlPlugin: public VideoDisplayPlugin{
	public:
		SdlPlugin( MRef<Library *> lib );
		~SdlPlugin();
		
		virtual std::string getMemObjectType() const { return "SdlPlugin"; }

		virtual std::string getName() const { return "sdl"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "SDL display"; }

		virtual MRef<VideoDisplay *> create( uint32_t width, uint32_t height, bool fullscreen ) const{
			return new SdlDisplay( width, height );
		}
};

#endif
