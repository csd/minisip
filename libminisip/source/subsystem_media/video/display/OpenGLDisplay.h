/*
 Copyright (C) 2009 Erik Eliasson
 
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

/* Copyright (C) 2009
 *
 * Authors: Erik Eliasson <ere@kth.se>
*/

#ifndef OPENGL_DISPLAY_H
#define OPENGL_DISPLAY_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/display/VideoDisplay.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
//#define glXGetProcAddress(x) (*glXGetProcAddressARB)((const GLubyte*)x)
#include<unistd.h>
#include<sys/time.h>


#include<libmutil/Thread.h>
#include<libmutil/Mutex.h>
#include<libmutil/CondVar.h>
#include<list>
#include"Animate.h"

class OpenGLWindow;

struct mgl_gfx{
	GLuint texture;
	float wu;	//width usage of texture (0..1). How much of texture is rendered
	float hu;	//height usage
	float aratio;   // width/height
	int tex_dim;
	Animate* x1;
	Animate* y1;
	Animate* x2;
	Animate* y2;
	Animate* alpha;
};

/**
 * A OpenGLDisplay represents one stream of images being displayed in
 * OpenGL. For this display it is not the same thing as a window
 * since a single window is used to display all video.
 */
class OpenGLDisplay: public VideoDisplay{
	public: 
		OpenGLDisplay( uint32_t width, uint32_t height );
		virtual void init( uint32_t height,  uint32_t width );
		virtual bool handlesChroma( uint32_t chroma );

		virtual MImage * provideImage();

		virtual void handle( MImage * mimage );
//		virtual void handleLocal( MImage * mimage );

		virtual void resize(int w, int h);

		virtual uint32_t getRequiredWidth(){ return width; };
		virtual uint32_t getRequiredHeight(){ return height; };

		virtual void start();
		virtual void stop();

		/*
		 * Must only be called by the thread doing OpenGL calls.
		 */
		struct mgl_gfx* getTexture();

		virtual void setIsLocalVideo(bool);	
		

	private:
		int colorNBytes;
		mgl_gfx gfx;
		

		uint8_t *rgb;
		bool newRgbData;
		bool needUpload;
		Mutex dataLock;

		uint32_t height;
		uint32_t width;
		bool fullscreen;

		OpenGLWindow* window;

		int nallocated;
		std::list<MImage*> emptyImages;

		void openDisplay();
		virtual void createWindow();
		virtual void destroyWindow();

		virtual void displayImage( MImage * image );

		virtual MImage * allocateImage();
		virtual void deallocateImage( MImage * image );
		virtual void handleEvents();
};


class OpenGLPlugin: public VideoDisplayPlugin{
	public:
		OpenGLPlugin( MRef<Library *> lib );
		~OpenGLPlugin();
		
		virtual std::string getMemObjectType() const { return "OpenGLPlugin"; }

		virtual std::string getName() const { return "opengl"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "OpenGL display"; }

		virtual MRef<VideoDisplay *> create( uint32_t width, uint32_t height ) const{
			return new OpenGLDisplay( width, height );
		}
};

#endif
