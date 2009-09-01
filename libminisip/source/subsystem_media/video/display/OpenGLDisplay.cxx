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

#include"OpenGLDisplay.h"
#include<config.h>
#include<sys/time.h>
#include<SDL/SDL_syswm.h>

#include<libmutil/mtime.h>

#include<iostream>

using namespace std;

#define NB_IMAGES 3


static std::list<std::string> pluginList;
static MRef<MPlugin *> plugin;
static bool initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *mopengl_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mopengl_LTX_getPlugin( MRef<Library*> lib ){
	return new OpenGLPlugin( lib );
}

/**
 * A single OpenGl widow is used to display all video data. It displays
 * video from multiple OpenGLDisplay objects.
 */


#define DRAW_Z -20.0F

class OpenGLWindow : public Runnable {
	public:
		OpenGLWindow(int width, int height, bool fullscreen);

		void init();

		void start();
		void stop();
		virtual void run();

//		void display(MImage* image, uint64_t source);

		void addDisplay(OpenGLDisplay* displ);
		void removeDisplay(OpenGLDisplay* displ);
		
		string getMemObjectType(){return "OpenGLWindow";}

		static MRef<OpenGLWindow*> getWindow();

		/*
		 * OpenGL implementations limits the size of a texture
		 * to NxN where N=2^m.
		 * This method returns N.
		 */
		int getTextureMaxSize(){return t_max_size;}

		/**
		 * Suggest good dimension of window. If there are multiple videos, then this will be ignored
		 */
		void sizeHint(int w, int h){
			cerr <<"EEEE: doing sizeHint("<<w<<","<<h<<")"<<endl;
			if (displays.size()<=1){
//				if (initialized && ! isFullscreen() ){
//					windowed_width  = w;
//					windowed_height = h;
//					windowResized(w,h);
//				}
//				if (!initialized){
					windowed_width  = w;
					windowed_height = h;
//				}
			resized=true;
			}
	
		} 
	private:
		int animation_ms;
		GLdouble windowX0;
		GLdouble windowY0;
		bool resized;
		void findScreenCoords(){
			GLdouble x=0;
			GLdouble y=0;
			GLdouble z=0;

			GLdouble model[16];
			GLdouble proj[16];
			GLint view[4];
			glGetDoublev(GL_PROJECTION_MATRIX, &proj[0]);
			glGetDoublev(GL_MODELVIEW_MATRIX, &model[0]);
			glGetIntegerv(GL_VIEWPORT, &view[0]);
	

			GLdouble winx;
			GLdouble winy;
			GLdouble winz;
			GLdouble delta=40;
			int i;
			for (i=0; i<64; i++){

				int ret = gluProject(x,y,z, model,proj,view, &winx, &winy, &winz);

				if (winx<0)
					x=x+delta;
				else
					x=x-delta;
				delta=delta/2;
			}
			windowX0=x;

			x=0;
			delta=20;

			for (i=0; i<64; i++){

				int ret = gluProject(x,y,z, model,proj,view, &winx, &winy, &winz);

				if (winy<0)
					y=y+delta;
				else
					y=y-delta;
				delta=delta/2;
			}
			windowY0=y;

			cerr << "EEEE: findScreenCoords: window 0,0 is at coord "<<windowX0 << ","<< windowY0 <<",-20"<<endl;


		}
		void updateVideoPositions(bool doAnimate);

		void rectToCoord(float &x1, float&y2, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, float aratio);
		void drawSurface();
		void sdlQuit();
		void windowResized(int w, int h);
		bool isFullscreen();
		void toggleFullscreen();

		Mutex displayListLock;
		list<OpenGLDisplay*> displays;
		bool displaysChanged;

		int t_max_size;
		

		//Counts how many times displays have called "start()"
		int runCount;
		MRef<Semaphore*> startWaitSem;

		static MRef<OpenGLWindow*> globalWindowObj;
		void initSdl();
		void initSurface();

		bool initialized;

		SDL_Surface* gDrawSurface;
		bool startFullscreen;


		int windowed_width;
		int windowed_height;
		int native_width;
		int native_height;
		int cur_width;
		int cur_height;
		float screen_aratio;

		int sdlFlags;
		int bpp;


		bool doStop;
		Mutex lock;
		MRef<Thread*> thread;

//		Mutex queueLock;
		//Semaphore queueSem;
//		std::list<MImage *> displayQueue;

//		GLuint texture;
};


MRef<OpenGLWindow*> OpenGLWindow::globalWindowObj=NULL;

OpenGLWindow::OpenGLWindow(int w, int h, bool fullscreen){
	//texture=-1;
	runCount=0;
	doStop=false;
	initialized=false;
	windowed_width=w;
	windowed_height=h;
	resized=false;
	native_width=0;
	native_height=0;
	screen_aratio=1.0;
	startFullscreen=fullscreen;
	animation_ms=250;
	displaysChanged=false;

	gDrawSurface=NULL;

	t_max_size=0;

	bpp=0;

}

#define REPORT_N 500


void OpenGLWindow::rectToCoord(float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, float aratio){
	
	float l_aratio=(lx2-lx1)/(ly2-ly1);

	if (aratio > l_aratio) { // fill full width, center vertically
		float h=(lx2-lx1)/aratio;
		float lh=ly2-ly1;
		x1=lx1;
		y1=ly1+((lh-h)/2.0);
		x2=lx2;
		y2=y1+h;


	}else{	//fill full height
		y1=ly1;
		float w=(ly2-ly1)*aratio;
		float lw=lx2-lx1;
		x1=lx1+(lw-w)/2.0;
		y2=ly2;
		x2=x1+w;
	}
	cerr <<"EEEE: mapped "<<lx1<<","<<ly1<<"->"<<lx2<<","<<ly2<<" to " << x1<<","<<y1<<"->"<<x2<<","<<y2<<endl;

}

void findVideoArea(int video_n, int ntot, float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, bool preferHorisontal){
	
	int n=1;
	int nr=1;
	int nc=1;
	while (nr*nc<ntot){	//find how many columns and rows to layout in
		if (preferHorisontal){
			if (nr<nc)
				nr++;
			else	
				nc++;
		}else{
			if (nc<nr){
				nc++;
			}else{
				nr++;
			}

		}
	}

	int nextra=nr*nc-ntot; //now all rows are full. This many are not
	int nfull = nr - nextra;

	int row=0;
	n=video_n;
//	cerr <<"nfull="<<nfull<<endl;
	while(n>=nc || (n>=(nc-1)&&row>=nfull) ){		//find what row this video should be in
		if (row>=nfull){
			n-=(nc-1);
		}else{
			n-=nc;
		}
		row++;
	}
	int col_in_row=nc;	//number of videos in the row our video is in
	if (row>=nfull){	
		col_in_row--;	
	}
//	cerr <<"EEEE: row="<<row<<"/"<<nr<<endl;
	int col=n;
//	cerr <<"EEEE: col="<<col<<"/"<<nc<<endl;
//	cerr <<"EEEE: columns in this row="<<col_in_row<<endl;

	float rowh=(ly2-ly1)/(float)nr;
//	cerr <<"EEEE: row height="<<rowh<<endl;
	y1=ly1+(float)row*rowh;
	y2=y1+rowh;

	float colw = (lx2-lx1)/(float)col_in_row;
//	cerr <<"EEEE: colw="<<colw<<endl;
	x1=lx1+col*(float)colw;
	x2=x1+colw;


#if 0
	//  1  2  3  4  5  6  7  8  9  10 
	int nrow[]={1, 1, 2, 2, 3, 2, 3, 3, 3, 4}
	int ncol[]={1, 2, 2, 2, 2, 3, 3, 3, 3, 3}
	int nr;
	int nc:
		if (ntot<=10){
		nr=nrow[ntot-1];
		nc=nrow[ntot-1];
	}else{
		int n=4;
		while (n*n<ntot)
			n++;
		nr=nc=n;
	}

	int r=video_n/nc;

	int x, int y;

	for (x=0; x<nc; x++){
		
		for (y=0; y<nr; y++){
		
		}
#endif

}

void OpenGLWindow::updateVideoPositions(bool doAnimate){

	cerr << "EEEE: doing updateVideoPositions()"<<endl;
	displayListLock.lock();
	int n_videos = displays.size();

	list<OpenGLDisplay*>::iterator i;
	float screen_aratio=(float)cur_width/(float)cur_height;



	//Make sure local displays are in the end of 
	//the list of displays. They are transparant, and
	//must be displayed on top of other video displays.
	list<OpenGLDisplay*> tmpdisplays=displays;
	displays.clear();
	int n_local=0;
	for (i=tmpdisplays.begin(); i!=tmpdisplays.end(); i++){
		if ((*i)->getIsLocalVideo())
			n_local++;
		else
			displays.push_back(*i);
	}

	for (i=tmpdisplays.begin(); i!=tmpdisplays.end(); i++){
		if ((*i)->getIsLocalVideo())
			displays.push_back(*i);
	}
	tmpdisplays.clear();
	
	int n_remote=n_videos-n_local;

	float leftx=windowX0; 
	float middlex=0.0F;
	float rightx=-windowX0;
	float topy=windowY0;
	float middley=0.0F;
	float bottomy=-windowY0;

	int remote_video_n=0;
	int global_n=0;
	for (i=displays.begin(); i!=displays.end(); i++, global_n++){ 

		mgl_gfx* gfx = (*i)->getTexture();
		float tex_x1, tex_y1, tex_x2, tex_y2, alpha;

		if ((*i)->getIsLocalVideo()){
			float glwidth = rightx-leftx;
			float glheight= bottomy-topy;
			float localwidth=glwidth/5;
//			cerr <<"EEEE: glwidth="<<glwidth<<endl;
//			cerr <<"EEEE: glheight="<<glheight<<endl;
			float localx=rightx-localwidth-glwidth/128;
//			cerr <<"EEEE: localx="<<localx<<endl;
			tex_x1= localx;
			tex_y1= /*bottomy-glheight/8-glheight/128*/ topy+glheight/128;
			tex_x2=localx+localwidth;
			tex_y2= tex_y1+localwidth/gfx->aratio /*-glheight/128*/;
			alpha=0.6;


		}else{
			alpha=1.0;
			bool horisontal = screen_aratio>1.25;

			//Find where to layout
			findVideoArea(remote_video_n, n_remote, tex_x1, tex_y1, tex_x2, tex_y2, leftx, topy, rightx, bottomy, horisontal);
			
			//Keep correct aspect ratio
			rectToCoord(tex_x1,tex_y1,tex_x2,tex_y2, tex_x1, tex_y1, tex_x2,tex_y2, gfx->aratio);
			middlex=tex_x1+(tex_x2-tex_x1)/2;
			middley=tex_y1+(tex_y2-tex_y1)/2;
			remote_video_n++;
		}

		float lastx1=tex_x1;
		float lasty1=tex_y1;
		float lastx2=tex_x2;
		float lasty2=tex_y2;


		if (gfx->x1){
			lastx1=gfx->x1->getVal();
			delete gfx->x1;
		}else{
			if (n_remote>1){
				lastx1=lastx2=middlex;	//grow from middle if it it did previously not exist
				lasty1=lasty2=middley;
			}else{
				lastx1=tex_x1;
				lasty1=tex_y1;
				lastx2=tex_x2;
				lasty2=tex_y2;
			}
		}
		if (gfx->y1){
			lasty1=gfx->y1->getVal();
			delete gfx->y1;
		}
		if (gfx->x2){
			lastx2=gfx->x2->getVal();
			delete gfx->x2;
		}
		if (gfx->y2){
			lasty2=gfx->y2->getVal();
			delete gfx->y2;
		}

		float lastAlpha=0.0;
		if (gfx->alpha){
			lastAlpha=gfx->alpha->getVal();
			delete gfx->alpha;
		}


		cerr << "EEEE: setting position of video "<< global_n <<" to " << tex_x1<<","<<tex_y1<<" "<<tex_x2<<","<<tex_y2<<" alpha="<<alpha<<endl;

		if (!doAnimate){
			gfx->x1= new Animate(tex_x1);
			gfx->y1= new Animate(tex_y1);
			gfx->x2= new Animate(tex_x2);
			gfx->y2= new Animate(tex_y2);
			gfx->alpha= new Animate(alpha);
		}else{
			gfx->x1= new Animate(animation_ms, lastx1, tex_x1, ANIMATE_STARTSTOP);
			gfx->y1= new Animate(animation_ms, lasty1, tex_y1, ANIMATE_STARTSTOP);
			gfx->x2= new Animate(animation_ms, lastx2, tex_x2, ANIMATE_STARTSTOP);
			gfx->y2= new Animate(animation_ms, lasty2, tex_y2, ANIMATE_STARTSTOP);
			gfx->alpha= new Animate(animation_ms, lastAlpha, alpha, ANIMATE_STARTSTOP);
		}

		gfx->x1->start();
		gfx->y1->start();
		gfx->x2->start();
		gfx->y2->start();
		gfx->alpha->start();

	}

	displayListLock.unlock();
}

void OpenGLWindow::addDisplay(OpenGLDisplay* displ){
	cerr << "EEEE: doing addDisplay()"<<endl;
	displayListLock.lock();
	displays.push_back(displ);
	displaysChanged=true;
	displayListLock.unlock();
}

void OpenGLWindow::removeDisplay(OpenGLDisplay* displ){
	displayListLock.lock();
	displays.remove(displ);
	displayListLock.unlock();
	updateVideoPositions(true);
}

void OpenGLWindow::drawSurface(){
	//	cerr << "EEEE: doing OpenGLWindow::drawSurface()"<<endl;
	static struct timeval lasttime;
	static int i=0;
	i++;
	if (i%REPORT_N==1){
		struct timeval now;
		gettimeofday(&now, NULL);
		int diffms = (now.tv_sec-lasttime.tv_sec)*1000+(now.tv_usec-lasttime.tv_usec)/1000;
		float sec = (float)diffms/1000.0f;
		printf("%d frames in %fs\n", REPORT_N, sec);
		printf("FPS_OPENGL: %f\n", (float)REPORT_N/(float)sec );
		lasttime=now;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //EE: not use depth?

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );

	glPushMatrix();

	glTranslatef( 0, 0.0f, DRAW_Z);

	static int N=0;
	N++;
	//	glRotatef(((float)N)/5.0, 0.0F, 1.0F,0.0F);

	GLdouble x=0;

	if (displaysChanged){
		updateVideoPositions(true);
		displaysChanged=false;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	displayListLock.lock();
	list<OpenGLDisplay*>::iterator video;
	int dummy=1;
	int nvideos=displays.size();
	for (video=displays.begin(); video!=displays.end(); video++){
//				cerr << "EEEE: getting video texture for display "<< dummy++ <<"/"<<nvideos <<endl;
		struct mgl_gfx* gfx = (*video)->getTexture();
		if (gfx->texture>0){
			float alpha=gfx->alpha->getVal();
			glColor4f(1.0,1.0,1.0, alpha );
			glBindTexture( GL_TEXTURE_2D, gfx->texture);
//			cerr<<"EEEE: drawing texture "<<gfx->texture<<" with alpha "<<alpha<<endl;
			glBegin( GL_QUADS );
			glTexCoord2f( 0, gfx->hu );
			glVertex3f(  gfx->x1->getVal(), gfx->y1->getVal(), 0.0f );

			glTexCoord2f( gfx->wu, gfx->hu );
			glVertex3f( gfx->x2->getVal(), gfx->y1->getVal(), 0.0f );

			glTexCoord2f( gfx->wu, 0 );
			glVertex3f( gfx->x2->getVal(), gfx->y2->getVal(), 0.0f );

			glTexCoord2f( 0, 0 );
			glVertex3f( gfx->x1->getVal(), gfx->y2->getVal(), 0.0f );
			glEnd();
		}
	}
	displayListLock.unlock();

	glPopMatrix();

	glFlush();
	SDL_GL_SwapBuffers();
}


void OpenGLWindow::sdlQuit(){
	cerr << "EEEE: sdlQuit called"<<endl;
	if (isFullscreen() ){
		toggleFullscreen();
	}
	SDL_Quit();
}



void OpenGLWindow::toggleFullscreen(){
	printf("Toggle fullscreen\n");
	sdlFlags ^= SDL_FULLSCREEN;
	//        SDL_WM_ToggleFullScreen(gDrawSurface);

	initSurface();

	//	startFullscreen=isFullscreen();
	//        initSdl();

}


bool OpenGLWindow::isFullscreen(){
	if (sdlFlags&SDL_FULLSCREEN)
		return true;
	else
		return false; 
}



void OpenGLWindow::windowResized(int w, int h){
	cerr<<"EEEE: doing OpenGLWindow::windowResized("<<w<<","<<h<<")"<<endl;
	windowed_width=w;
	windowed_height=h;

	screen_aratio=(float)w/(float)h;

	initSurface();

	updateVideoPositions(false);
	SDL_GL_SwapBuffers();
}


void OpenGLWindow::run(){
	cerr << "EEEE: OpenGLWindow::run start"<<endl;
#ifdef DEBUG_OUTPUT
	setThreadName("OpenGLWindow");
#endif

	if (!initialized)
		init();

	startWaitSem->inc();

	SDL_Event event;
	while(!doStop)
	{
		//cerr << "EEEE: OpenGLWindow::run loop"<<endl;
		while( SDL_PollEvent( &event ))
		{
			string url;
			switch( event.type )
			{
				case SDL_MOUSEBUTTONDOWN:
					printf("PRESS: %d,%d\n",event.button.x, event.button.y);
#if 0
					url=d_login->clickedUrl(event.button.x, event.button.y);
					if (url!=""){
						gui->doRegister(url);
						d_login->hide();
					}
#endif

					break;
				case SDL_MOUSEMOTION:
					//printf("mouse %d,%d\n",event.motion.x, event.motion.y);
					//			d_login->mouseMove(event.motion.x, event.motion.y);
					break;

				case SDL_QUIT:
					sdlQuit();
					return;
				case SDL_VIDEORESIZE:
					printf("EE: RESIZE!\n");
					windowResized(event.resize.w, event.resize.h);
					break;
				case SDL_KEYDOWN:

					//trap quit and fullscreen events. Forward everything else

					if (event.key.keysym.sym == SDLK_ESCAPE ||
							event.key.keysym.sym == SDLK_q){
						sdlQuit();
						return;
					}

					/*				if (event.key.keysym.sym=='w'){
									windowResized(rand()%800+100, rand()%800+100);
									break;
									}
									*/


					if (event.key.keysym.sym == SDLK_RETURN &&
							event.key.keysym.mod & KMOD_ALT){
						toggleFullscreen();
					}else{
						char key = event.key.keysym.sym;
						if (event.key.keysym.mod == KMOD_SHIFT){        //if shift, make upper case
							printf("Shift detected\n");
							key-='a'-'A';
						}
						//gui->keyPressed(key);

					}

					break;
			}

		} // -- while event in queue

		if (resized){
			windowResized(windowed_width,windowed_height);
			resized=false;
		}
		drawSurface();
//		Thread::msleep(500);
	}

	cerr <<"----------------EEEE: OpenGl thread quitting"<<endl;
	SDL_Quit();
} 


void OpenGLWindow::start(){
	cerr << "EEEE: doing OpenGLWindow::start()"<<endl;
	lock.lock();
	bool useSem=false;
	if (runCount<=0){
		startWaitSem = new Semaphore();
		useSem=true;
		doStop=false;
		if (runCount==0){
			massert(!thread);
			thread = new Thread(this);
		}

	}
	runCount++;

	lock.unlock();

	//cerr << "EEEE: waiting for startWaitSem->dec()"<<endl;
	if (useSem)
		startWaitSem->dec();
	//cerr << "EEEE: done waiting for startWaitSem->dec()"<<endl;

	cerr << "EEEE: after OpenGLWindow::start() runCount="<<runCount<<endl;
}

void OpenGLWindow::stop(){
	cerr << "EEEE: doing OpenGLWindow::stop()"<<endl;
	lock.lock();
	runCount--;
	if (runCount==0){
		doStop=true;
//		cerr <<"EEEE: waiting for OpenGLWindow thread..."<<endl;
		thread->join();
//		cerr <<"EEEE: done waiting for OpenGLWindow thread..."<<endl;
		thread=NULL;
	}
	initialized=false;
	lock.unlock();

	cerr << "EEEE: after OpenGLWindow::stop() runCount="<<runCount<<endl;
}

MRef<OpenGLWindow*> OpenGLWindow::getWindow(){
	if (!globalWindowObj)
		globalWindowObj = new OpenGLWindow(800,  600, false);
	return globalWindowObj;

}

void OpenGLWindow::init(){
	if (!initialized){
		initialized=true;
		initSdl();
	}

}

typedef GLvoid (*glXSwapIntervalSGIFunc) (GLint);
typedef GLvoid (*glXSwapIntervalMESAFunc) (GLint);

void OpenGLWindow::initSdl(){
	// init video system
	if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr,"Failed to initialize SDL Video!\n");
		exit(1);
	}

#if 0
	if(TTF_Init()){
		fprintf(stderr,"Failed to initialize SDL_TTF!\n");
		exit(1);
	}

	text = new Text("share/FreeSans.ttf");
#endif


	// tell system which funciton to process when exit() call is made
	atexit(SDL_Quit);

	// get optimal video settings
	const SDL_VideoInfo* vidinfo = SDL_GetVideoInfo();
	if(!vidinfo)
	{
		fprintf(stderr,"Coudn't get video information!\n%s\n", SDL_GetError());
		exit(1);
	}
	if (!native_height){ //the size of the desktop is only available the first
			 	//time this function is called. The second time
			 	//it is the size of the current window.
		native_width=vidinfo->current_w;
		native_height=vidinfo->current_h;
	}
	cerr << "EEEE: OpenGL: native screen dimension is "<< native_width<<"x"<<native_height<<endl;

	// set opengl attributes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       5);
#ifdef __APPLE__
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      32);
#else
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      16);
#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,    1);


	/* We want to enable synchronizing swapping buffers to
 	 * vsynch to avoid tearing. SDL_GL_SWAP_CONTROL does
 	 * not (always?) work under linux, so we try to
 	 * use GL extensions.
 	 */

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,    1);

	SDL_putenv((char*)"__GL_SYNC_TO_VBLANK=1");


#if 1
	cerr <<"EEEE: trying to VSYNCH using OpenGL extensiosn"<<endl;

	glXSwapIntervalSGIFunc glXSwapIntervalSGI = 0;
	glXSwapIntervalMESAFunc glXSwapIntervalMESA = 0;
	glXSwapIntervalSGI = (glXSwapIntervalSGIFunc) glXGetProcAddress((const GLubyte*)"glXSwapIntervalSGI");
	if (false && glXSwapIntervalSGI){
		cerr <<"EEEE: setting VSYNCH using SGI"<<endl;
		glXSwapIntervalSGI (2);
	}else{
		cerr <<"EEEE: no SGI"<<endl;
		glXSwapIntervalMESA = (glXSwapIntervalMESAFunc) glXGetProcAddress((const GLubyte*)"glXSwapIntervalMESA");

		if (glXSwapIntervalMESA){
			cerr <<"EEEE: setting VSYNCH using MESA"<<endl;
			glXSwapIntervalMESA (2);
		}else{
			cerr <<"EEEE: no MESA"<<endl;
		}


	}
#endif 

	sdlFlags = SDL_OPENGL | SDL_RESIZABLE;

	if (startFullscreen)
		sdlFlags = sdlFlags | SDL_FULLSCREEN;

	bpp = vidinfo->vfmt->BitsPerPixel; //attribute used by initSurface

	initSurface( );
}

void OpenGLWindow::initSurface(){

	printf("initSurface running\n");

	// get a framebuffer
	int w,h;
	cerr <<"EEEE: sdlFlags="<<sdlFlags<<endl;
	cerr <<"EEEE: SDL_FULLSCREEN="<<SDL_FULLSCREEN<<endl;
	if (sdlFlags & SDL_FULLSCREEN){
		w=native_width;
		h=native_height;
		cerr << "EEEE: initializing to fullscreen dimensions "<< w <<"x"<<h<<endl;
	}else{
		w=windowed_width;
		h=windowed_height;
		cerr << "EEEE: initializing to windowed dimensions "<< w <<"x"<<h<<endl;
	}
	cur_width=w;
	cur_height=h;

	cerr << "EEEE: setting video mode to " << w << "x" << h << endl;
	gDrawSurface = SDL_SetVideoMode(w,h, bpp, sdlFlags);

	if( !gDrawSurface )
	{
		fprintf(stderr,"Couldn't set video mode!\n%s\n", SDL_GetError());
		exit(1);
	}

	GLint texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
	printf("INFO: max texture size: %dx%d\n",texSize,texSize);
	t_max_size=texSize;




	glShadeModel(GL_SMOOTH);
	//glClearColor(0x04/255.0F,0x01/255.0F,0x16/255.0F,0);
	glClearColor(0.0F,0.0F,0.0F,0);
	// set opengl viewport and perspective view
	glViewport(0,0,cur_width,cur_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat) cur_width/(GLfloat) cur_height, 1.0, 200.0);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//        loadTexture();

	///	massert(d_login);
	///	d_login->initGl(text);

	///	if (text)
	///		text->restartGl();


	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       5);
#ifdef __APPLE__
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      32);
#else
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      16);
#endif

	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,    1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,    1);



	glDisable(GL_DEPTH_TEST);




	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glPushMatrix();
	glTranslatef( 0, 0.0f, DRAW_Z);
	findScreenCoords();
	glPopMatrix();

}



//////////////////////////////////////////////////////////////////////////////////////



OpenGLDisplay::OpenGLDisplay( uint32_t width, uint32_t height):VideoDisplay(){
	cerr << "EEEE: OpenGLDisplay::OpenGLDisplay("<< width<<","<<height<<") running"<<endl;
	this->width = width;
	this->height = height;
	fullscreen = false;
	nallocated=0;
	//isLocalVideo=false;

	memset(&gfx, 0, sizeof(gfx));
	gfx.texture=-1;
	gfx.aratio=(float)width/(float)height;
	gfx.wu=1;
	gfx.hu=1;
	gfx.tex_dim=0;

	rgb=NULL;
	needUpload=false;
	newRgbData=false;
	//rgb32=false;
	colorNBytes=3;

	window = * (OpenGLWindow::getWindow() );
	window->sizeHint(width,height);
	massert(window);
}

struct mgl_gfx*  OpenGLDisplay::getTexture(){
	dataLock.lock();
//	cerr <<"EEEE: gl extensions: " << glGetString(GL_EXTENSIONS);
	if (gfx.texture==-1){
		massert(colorNBytes==3 || colorNBytes==4);
		int hw_max_dim = window->getTextureMaxSize();
		int dim = (hw_max_dim>2048) ? 2048 : hw_max_dim;
		gfx.tex_dim=dim;
		glGenTextures( 1, (GLuint*)&gfx.texture);
		glBindTexture( GL_TEXTURE_2D, gfx.texture);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		uint8_t *dummy_black=(uint8_t*)calloc(1,dim*dim*colorNBytes);
		massert(dummy_black);
		GLenum pixFormat = colorNBytes==3?GL_RGB:GL_BGRA;
		glTexImage2D( GL_TEXTURE_2D, 0, 3, dim, dim, 0, /*pixFormat*/ GL_RGB, GL_UNSIGNED_BYTE, dummy_black );
		free(dummy_black);
	}

	if (newRgbData){
		//cerr <<"EEEE: uploading new texture"<<endl;
		massert(gfx.texture>0);
		newRgbData=false;
		glBindTexture( GL_TEXTURE_2D, gfx.texture);

		if (width>gfx.tex_dim){
			int factor=2;
			while (width/factor > gfx.tex_dim)
				factor++;
			cerr << "WARNING: insufficent OpenGL hardware. Resizing image to fit in texture (factor="<<factor<<")"<<endl;
			uint8_t* tmpbuf = new uint8_t[(width/factor)*(height/factor)*colorNBytes+16];

			int x,y;
			int newwidth = width/factor;
			int newheight = height/factor;
			for (y=0; y<newheight; y++){
				for (x=0; x<newwidth; x++){
					tmpbuf[x*colorNBytes+y*newwidth*colorNBytes+0]=rgb[(x*colorNBytes+y*width*colorNBytes+0)*factor];
					tmpbuf[x*colorNBytes+y*newwidth*colorNBytes+1]=rgb[(x*colorNBytes+y*width*colorNBytes+2)*factor];
					tmpbuf[x*colorNBytes+y*newwidth*colorNBytes+2]=rgb[(x*colorNBytes+y*width*colorNBytes+1)*factor];
				}
			}

			glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0 , newwidth, newheight, GL_RGB, GL_UNSIGNED_BYTE, tmpbuf );
			gfx.wu=(newwidth/*/(float)factor*/)/(float)gfx.tex_dim;
			gfx.hu=(newheight/*/(float)factor*/)/(float)gfx.tex_dim;
			gfx.aratio = (float)width/(float)height;
			//cerr << "EEEE: new dim="<<newwidth<<"x"<<newheight<<" and gfx.hu="<<gfx.hu<<" and tex_dim="<<gfx.tex_dim<<endl;
			delete []tmpbuf;
			
		}else{
			//cerr << "EEEE: getTexture: uploading texture of size "<<width<<"x"<<height << " for display "<<(uint64_t)this<< "colorNBytes="<<colorNBytes<< " texdim="<<gfx.tex_dim<<endl;

			GLenum pixFormat = (colorNBytes==3)?GL_RGB:GL_BGRA;

			glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0 , width, height, pixFormat, GL_UNSIGNED_BYTE, rgb );
			gfx.wu=width/(float)gfx.tex_dim;
			gfx.hu=height/(float)gfx.tex_dim;
			gfx.aratio = (float)width/(float)height;
		}
	}
	dataLock.unlock();
	return &gfx;
}

void OpenGLDisplay::handle( MImage * mimage){
	//cerr <<"EEEE: doing OpenGLDisplay::handle on display "<<(uint64_t)this<<" image size="<<mimage->width<<"x"<<mimage->height<<" local="<<isLocalVideo<<endl;
//	cerr <<"EEEE: linesize0="<< mimage->linesize[0]<<endl;
	massert(mimage->linesize[1]==0);
	
	colorNBytes=mimage->linesize[0]/mimage->width;
//	cerr <<"EEEE: colorNBytes="<<colorNBytes<<endl;

	dataLock.lock();
	if (!rgb || width!=mimage->width || height!=mimage->height){
		cerr << "EEEE: allocating RGB of size "<<mimage->width<<"x"<<mimage->height<<endl;
		if (rgb)
			delete rgb;
		width=mimage->width;
		height=mimage->height;
		gfx.aratio=(float)width/(float)height;
		rgb = new uint8_t[width*height*colorNBytes+16]; // +16 to avoid mesa bug
	
	}

	massert(rgb);
//	cerr <<"EEEE:handle: copying data to rgb buf, n="<<width<<"x"<<height<<endl;
	memcpy(rgb, &mimage->data[0][0], width*height*colorNBytes); //TODO: don't copy since it is in correct format.
	newRgbData=true;
//	cerr << "EEEE: OpenGLDisplay::handle: done copying new data"<<endl;
	if (!isLocalVideo)
		emptyImages.push_back(mimage);
	dataLock.unlock();



//	window->display(mimage, (uint64_t)this);
}



void OpenGLDisplay::start(){
	cerr <<"EEEE: doing OpenGLDisplay::start"<<endl;
	massert(window);
	window->start();
	window->addDisplay(this);

}

void OpenGLDisplay::stop(){
	cerr <<"EEEE: doing OpenGLDisplay::stop"<<endl;
	window->removeDisplay(this);
	window->stop();
	gfx.texture=-1;
}

void OpenGLDisplay::openDisplay(){
	cerr <<"EEEE: doing OpenGLDisplay::openDisplay"<<endl;

}

void OpenGLDisplay::init( uint32_t width, uint32_t height ){
	cerr <<"EEEE: doing OpenGLDisplay::init("<<width<<","<<height<<")"<<endl;
	this->width = width;
	this->height=height;
}

void OpenGLDisplay::createWindow(){
	cerr <<"EEEE: doing OpenGLDisplay::createWindow"<<endl;
}

void OpenGLDisplay::resize(int w, int h){
	cerr << "EEEE: doing OpenGLDisplay::resize("<<w<<","<<h<<") old size="<<width<<"x"<<height<<endl;
	this->width=w;
	this->height=h;
	gfx.aratio=(float)w/(float)h;
	window->sizeHint(w,h);
}

void OpenGLDisplay::destroyWindow(){
	cerr << "EEEE: doing OpenGLDisplay::destroyWindow()"<<endl;
}

MImage * OpenGLDisplay::provideImage(){
	dataLock.lock();
	if (emptyImages.size()==0){
		emptyImages.push_back( allocateImage() );
	}
	MImage* ret = *emptyImages.begin();
	emptyImages.pop_front();
	dataLock.unlock();
	if (ret->width!=width||ret->height!=height){	//If there has been a resize, and the 
							//MImage is allocated with the old size, re-allocate
		deallocateImage(ret);
		ret=allocateImage();
	}
	return ret;
}

MImage * OpenGLDisplay::allocateImage(){
	cerr << "EEEE: doing OpenGLDisplay::allocateImage of size "<< width<<"x"<<height<<endl;
	MImage * mimage = new MImage;
	nallocated++;

	mimage->data[0] = (uint8_t*)calloc(1,width*height*3+1);
	massert(mimage->data[0]);
	mimage->data[1] = NULL;
	mimage->data[2] = NULL;

	mimage->linesize[0] = width*3;
	mimage->linesize[1] = 0;
	mimage->linesize[2] = 0;
	mimage->width=width;
	mimage->height=height;

	mimage->privateData = NULL;

	return mimage;
}

void OpenGLDisplay::deallocateImage( MImage * mimage ){
	cerr << "EEEE: doing OpenGLDisplay::deallocateImage()"<<endl;
	free(mimage->data[0]);
	delete mimage;
}

bool OpenGLDisplay::handlesChroma( uint32_t chroma ){
	cerr << "EEEE: doing OpenGLDisplay::handlesChroma "<<chroma<<endl;
	return chroma == M_CHROMA_RV24;
}

void OpenGLDisplay::displayImage( MImage * mimage ){
	cerr <<"EEEE: doing OpenGLDisplay::displayImage"<<endl;

	massert(1==0); //This should not be called?!

}

void OpenGLDisplay::handleEvents(){
	cerr <<"EEEE: doing OpenGLDisplay::handleEvents"<<endl;
}

void OpenGLDisplay::setIsLocalVideo(bool isLocal){
	isLocalVideo=isLocal;
}

OpenGLPlugin::OpenGLPlugin( MRef<Library *> lib ): VideoDisplayPlugin( lib ){
}

OpenGLPlugin::~OpenGLPlugin(){
}

