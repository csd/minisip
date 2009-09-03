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

#include"Animate.h"
#include"Text.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <GL/glxext.h>
//#define glXGetProcAddress(x) (*glXGetProcAddressARB)((const GLubyte*)x)
#include<unistd.h>
#include<sys/time.h>
#include<fstream>



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
	bool isSelected;
	char*name;
};

class Menu{
public:
	Menu(struct mgl_gfx* _icon, bool _visible=true){
		visible=_visible;
		icon=_icon;
		selected=false;
	}
	int nVisible(){
		int ret=0;
		list<Menu*>::iterator i;
		for (i=mitems.begin();i!=mitems.end();i++){
			if ((*i)->visible)
				ret++;
		}
		return ret;
	}
	int getSelectedIndex(){
		int n=0;
		list<Menu*>::iterator i;
		for (i=mitems.begin();i!=mitems.end();i++, n++){
			if ((*i)->selected)
				return n;
		}
		return 0;
	}

	///Returns currently selected menu
	Menu* selectRight(){
		Menu* ret=NULL;
		cerr <<"EEEE: doing selectLeft"<<endl;
		int s=getSelectedIndex();
		list<Menu*>::iterator item=mitems.begin();
		for (int i=0; i<s; i++)
			item++;
		(*item)->selected=false;	// clear old selection
		item++;

		while (item!=mitems.end() && (*item)->visible==false)
			item++;

		if (item==mitems.end()){       //if wrap
			(*mitems.begin())->selected=true;
			ret=*mitems.begin();
		}else{
			(*item)->selected=true;
			ret=*item;
		}
		massert(ret!=NULL);
		return ret;
	}
	
	///Returns currently selected menu
	Menu* selectLeft(){
		cerr <<"EEEE: doing selectRight"<<endl;
		int s=getSelectedIndex();

		Menu*last=*mitems.begin();
		list<Menu*>::iterator item=mitems.begin();
		do{
			if ((*item)->visible)
				last=*item;
			item++;
		}while(item!=mitems.end());

		
		Menu*prev=last;


		item=mitems.begin();
		for (int i=0; i<s; i++){
			prev=*item;
			item++;
		}
		(*item)->selected=false;
		massert(prev!=NULL);
		prev->selected=true;
		return prev;
	}


	bool visible;
	bool selected;
	list<Menu*> mitems;
	Menu* selectedItem;
	struct mgl_gfx* icon;
	CommandString command;
};

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

		static MRef<OpenGLWindow*> getWindow(bool fullscreen);

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
				if (!initialized && !startFullscreen){
					windowed_width  = w;
					windowed_height = h;
				}
			resized=true;
			}
	
		} 

		void setTexturePath(string path);
		void enableMenu();
		void keyPressed(string key, bool isRepeat);

		void setCallback(OpenGLDisplay* cb){
			if (!callback)
				callback=cb;
		}

		///send command to callback
		void send(CommandString cmd);
		void incomingCall(string callid, string uri, bool unprotected);
	private:
		bool loadTexture(string name);
		void hideAcceptCallMenu(bool hideAllMenus);
		struct mgl_gfx* getIcon(string name);

		void findVideoArea(int video_n, int ntot, float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, bool preferHorisontal, bool* selectedptr);
		void setupMenu();
		bool texturesLoaded;
		bool showMenu;
		Menu* menuRoot;
		Menu* menuCur;
		Menu* menuItemCur;

		Menu* menuActions;
		Menu* menuAcceptCall;
		Mutex menuLock;
		string texturePath;

		bool inCall;
		int animation_ms;
		GLdouble windowX0;
		GLdouble windowY0;
		bool resized;
		void findScreenCoords();

		void updateVideoPositions(bool doAnimate);

		void rectToCoord(float &x1, float&y2, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, float aratio);
		void drawSurface();
		void sdlQuit();
		void windowResized(int w, int h);
		bool isFullscreen();
		void toggleFullscreen();

		Mutex displayListLock;
		list<OpenGLDisplay*> displays;
		list<struct mgl_gfx*> icons;
		bool displaysChanged;

		int t_max_size;

		OpenGLDisplay* callback;
		

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

		int video_select_row;
		int video_select_col;
		int video_nrows;
		int video_ncols;

		int sdlFlags;
		int bpp;


		bool doStop;
		Mutex lock;
		MRef<Thread*> thread;

//		Mutex queueLock;
		//Semaphore queueSem;
//		std::list<MImage *> displayQueue;

//		GLuint texture;
//
		Text *text;
};


MRef<OpenGLWindow*> OpenGLWindow::globalWindowObj=NULL;

OpenGLWindow::OpenGLWindow(int w, int h, bool fullscreen){
	cerr<<"EEEE: ------------------------ CREATING OPENGL WINDOW---------------"<<endl;
	cerr <<"EEEE: fullscreen="<<fullscreen<<endl;
	//texture=-1;
	runCount=0;
	inCall=false;
	showMenu=false;
	menuRoot=NULL;
	menuCur=NULL;
	menuItemCur=NULL;
	menuActions=NULL;
	menuAcceptCall=NULL;
	texturesLoaded=false;
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
	video_select_row=0;
	video_select_col=0;
	video_nrows=0;
	video_ncols=0;

	gDrawSurface=NULL;

	callback=NULL;

	t_max_size=0;

	bpp=0;

#if 0
	if (TTF_Init()){
		cerr <<"Failed to initialize SDL_TTF!\n"<<endl;
		exit(1);
	}

	text=new Text("/home/erik/FreeSans.ttf");
#else
	text=NULL;
#endif
}

void OpenGLWindow::send(CommandString cmd){
	if (callback)
		callback->sendCmd(cmd);

}

void OpenGLWindow::hideAcceptCallMenu(bool hideAll){
	menuLock.lock();
	menuCur->selected=false;
	menuRoot=menuActions;
	menuCur=menuActions;
	menuItemCur=*menuActions->mitems.begin();
	showMenu=!hideAll;
	menuLock.unlock();
}

void OpenGLWindow::incomingCall(string callid, string uri, bool unprotected){
	cerr <<"OpenGLWindow::incomingCall: trying to display accept call menu"<<endl;
	menuLock.lock();
	list<Menu*>::iterator i=menuAcceptCall->mitems.begin();
	(*i)->command=CommandString(callid, "accept_invite");	//command for yes button
	i++;
	(*i)->command=CommandString(callid, "reject_invite");	//command for no button
	menuRoot=menuAcceptCall;
	menuCur=menuAcceptCall;
	menuItemCur=*menuAcceptCall->mitems.begin();
	menuLock.unlock();

}

void OpenGLWindow::keyPressed(string key, bool isRepeat){
	cerr <<"EEEE: OpenGLWindow::keyPressed: "<<key<<endl;
	if (key=="KEY_MENU" && !isRepeat){
		showMenu=!showMenu;
		
		cerr <<"EEEE: MENU DETECTED"<<endl;	
		return;
	}

	if (!showMenu){
		if (key=="KEY_CHANNELUP"){
			cerr <<"EEEE: selecting row up"<<endl;
			cerr <<"EEEE: video_nrows="<<video_nrows<<endl;
			cerr <<"EEEE: video_ncols="<<video_ncols<<endl;
			menuLock.lock();
			video_select_row++;
			if (video_select_row>=video_nrows)
				video_select_row--;
			menuLock.unlock();
			updateVideoPositions(false);
		}
		if (key=="KEY_CHANNELDOWN"){
			cerr <<"EEEE: selecting row down"<<endl;
			menuLock.lock();
			video_select_row--;
			if (video_select_row<0)
				video_select_row=0;
			menuLock.unlock();
			updateVideoPositions(false);
		}
		if (key=="BTN_RIGHT"){
			cerr <<"EEEE: selecting row up"<<endl;
			cerr <<"EEEE: video_nrows="<<video_nrows<<endl;
			cerr <<"EEEE: video_ncols="<<video_ncols<<endl;
			menuLock.lock();
			video_select_col++;
			if (video_select_col>=video_ncols)
				video_select_col--;
			menuLock.unlock();
			updateVideoPositions(false);
		}
		if (key=="BTN_LEFT"){
			cerr <<"EEEE: selecting row down"<<endl;
			menuLock.lock();
			video_select_col--;
			if (video_select_col<0)
				video_select_col=0;
			menuLock.unlock();
			updateVideoPositions(false);
		}

	}

	if (showMenu && !isRepeat){


		if (key=="BTN_LEFT"){
			menuLock.lock();
			menuItemCur = menuCur->selectLeft();
			menuLock.unlock();
		}
		if (key=="BTN_RIGHT"){
			menuLock.lock();
			menuItemCur = menuCur->selectRight();
			menuLock.unlock();
			
		}
		if (key=="KEY_PLAY"){
			menuLock.lock();
			string cmd = menuItemCur->command.getOp();
			cerr <<"EEEE: MENU SELECTED: "<< cmd<<endl;
			
			if (cmd!=""){
				CommandString cmd = menuItemCur->command;
				menuLock.unlock();
				send( menuItemCur->command );
			}else{
				menuLock.unlock();
			}

			if (menuRoot==menuAcceptCall)
				hideAcceptCallMenu(cmd=="accept_invite");
			
		}



	}

}

void OpenGLWindow::setTexturePath(string p){
	texturePath=p;
}

struct mgl_gfx* OpenGLWindow::getIcon(string name){
	list<struct mgl_gfx*>::iterator i;
	for (i=icons.begin(); i!=icons.end(); i++)
		if (name==(*i)->name)
			return *i;
	return NULL;

} 

#define ICON_DIM 256
bool OpenGLWindow::loadTexture(string fname){
	texturesLoaded=true;
	cerr <<"EEEE: OpenGLWindow::loadTexture: trying to load "<<fname<<endl;

	string path = "/home/erik/share/minisip/"+fname+".raw";
	int len=ICON_DIM*ICON_DIM*4;
	byte_t *tmp = new byte_t[len+16];
	cerr <<"EEEE: opening file <"<<path<<">"<<endl;
	ifstream inf;
	inf.open(path.c_str(), ios::binary);
	cerr <<"EEEE: reading file..."<<endl;
	inf.read((char*)tmp,len);

	struct mgl_gfx* t = new struct mgl_gfx;
	memset(t,0,sizeof(struct mgl_gfx));
	t->isSelected=true;
	t->hu=1.0;
	t->wu=1.0;
	t->tex_dim=256;

	glGenTextures( 1, (GLuint*)&(t->texture));
	cerr <<"EEEE: generated texture id "<< t->texture<<endl;
	massert(t->texture > 0);
	glBindTexture( GL_TEXTURE_2D, t->texture);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	cerr <<"EEEE: uploading to GL..."<<endl;
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ICON_DIM, ICON_DIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);

	t->name=strdup(fname.c_str());

	icons.push_back(t);

	delete[] tmp;
	cerr <<"EEEE: done loading texture"<<endl;
}

void OpenGLWindow::setupMenu(){

	menuActions=new Menu(NULL);
	menuCur=menuRoot=menuActions;
	
	Menu* m;

	const char* names[]={"invite","hangup","settings","exit" ,0};

	int i=0;
	while (names[i++]){
		struct mgl_gfx* icon = getIcon(names[i-1]);
		massert(icon);
		m = new Menu(icon);
		m->command=CommandString("",names[i-1]);
		menuActions->mitems.push_back(m);
	}
	(*(menuActions->mitems.begin()))->selected=true;
	menuItemCur = *(menuActions->mitems.begin());
	
	
	menuAcceptCall=new Menu(NULL);
	
	struct mgl_gfx* icon = getIcon("yes");
	massert(icon);
	m = new Menu(icon);
	menuAcceptCall->mitems.push_back(m);

	icon = getIcon("no");
	massert(icon);
	m = new Menu(icon);
	menuAcceptCall->mitems.push_back(m);
	(*(menuAcceptCall->mitems.begin()))->selected=true;



}

void OpenGLWindow::enableMenu(){
	cerr<<"EEEE: called enableMenu()"<<endl;
	massert(texturePath.size()>0);
	showMenu=true;
}

#define REPORT_N 500




void OpenGLWindow::findScreenCoords() {
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
//	cerr <<"EEEE: mapped "<<lx1<<","<<ly1<<"->"<<lx2<<","<<ly2<<" to " << x1<<","<<y1<<"->"<<x2<<","<<y2<<endl;

}

void OpenGLWindow::findVideoArea(int video_n, int ntot, float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, bool preferHorisontal, bool* selectedptr){

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

	video_nrows=nr;
	video_ncols=nc;

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

	if (selectedptr){
		if (row==video_select_row && (col==video_select_col)){
			cerr <<"EEEE: Video SELECTEd: row="<<row<<" col="<<col<<" video_select_row="<<video_select_row<<" video_select_col="<<video_select_col<<endl;
			*selectedptr=true;
		}else{
			cerr <<"EEEE: Video not selected: row="<<row<<" col="<<col<<" video_select_row="<<video_select_row<<" video_select_col="<<video_select_col<<endl;
			*selectedptr=false;	
		}
	}



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
			findVideoArea(remote_video_n, n_remote, tex_x1, tex_y1, tex_x2, tex_y2, leftx, topy, rightx, bottomy, horisontal, &gfx->isSelected);
			
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
	inCall=true;
	cerr << "EEEE: doing addDisplay()"<<endl;
	displayListLock.lock();
	displays.push_back(displ);
	displaysChanged=true;
	cerr<<"EEEE: --------------------------------- after add ndisplays="<< displays.size()<<endl;
	displayListLock.unlock();
}

void OpenGLWindow::removeDisplay(OpenGLDisplay* displ){
	displayListLock.lock();
	displays.remove(displ);
	inCall=displays.size()>0;
	displaysChanged=true;
	cerr<<"EEEE: --------------------------------- after remove ndisplays="<< displays.size()<<endl;
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


	if (showMenu&&!texturesLoaded){
		cerr <<"EEEE: drawSurface: SHOW MENU: loading icons"<<endl;
		loadTexture("invite");
		loadTexture("hangup");
		loadTexture("settings");
		loadTexture("exit");
		loadTexture("yes");
		loadTexture("no");
		setupMenu();
		cerr <<"EEEE: drawSurface: done loading textures"<<endl;
	}


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
			float x1= gfx->x1->getVal();
			float y1= gfx->y1->getVal();
			float x2= gfx->x2->getVal();
			float y2= gfx->y2->getVal();

			glBegin( GL_QUADS );
			glTexCoord2f( 0, gfx->hu );
			glVertex3f(  x1, y1, 0.0f );

			glTexCoord2f( gfx->wu, gfx->hu );
			glVertex3f( x2, y1, 0.0f );

			glTexCoord2f( gfx->wu, 0 );
			glVertex3f( x2, y2, 0.0f );

			glTexCoord2f( 0, 0 );
			glVertex3f( x1, y2, 0.0f );
			glEnd();

			if (gfx->isSelected){
				float bwidth=(y2-y1)/20.0;
				glColor4f(0.0, 0.0, 0.7, 0.5); 
				glRectf( x1, y1, x2, y1+bwidth );
				glRectf( x1, y2, x2, y2-bwidth );
				glRectf( x1, y1+bwidth, x1+bwidth, y2-bwidth);
				glRectf( x2, y1+bwidth, x2-bwidth, y2-bwidth);
			}

		}
	}
	displayListLock.unlock();

	if (text){
		SDL_Color white={255,255,255};
		SDL_Color black={0,0,0};
		cerr <<"EEEE: drawing text"<<endl;

		int width = text->getTextWidth("Hello world", 28, black, white);
		text->draw3D(0,0,0, 1.0/30.0,"Hello world",48, white, black);
		text->draw2D(10,10,"Hello world",28, white, black);
		text->draw3D(0,0,0,10,10,10,"Hello world",28, white, black);
	}

	if (showMenu){
		menuLock.lock();
		massert(menuRoot);
		
		int nicons=menuRoot->nVisible();
		float x1= windowX0;
		float y1= windowY0;
		float x2= -windowX0;
		float y2= -windowY0;
		
		cerr <<"EEEE: nvideos="<<nvideos<<endl;
		if (nvideos==0){ 	//limit to middle 40% of height
			cerr <<"EEEE: layout middle"<<endl;
			y1= y1/5;
			y2= y2/5;
			rectToCoord(x1,y1, x2,y2,  x1,y1, x2,y2,  1.0); //draw menu on bottom 1/8 of screen
		}else{
			cerr <<"EEEE: layout bottom"<<endl;
			y2= y1+(y2-y1)/8   ;
			rectToCoord(x1,y1,x2,y2,x1,y1,x2,y2,1.0);
		}

		float iconWidth=x2-x1;
		float totWidth=iconWidth*nicons;
		float startx=-totWidth/2;

		int iconi=0;
		list<Menu*>::iterator i;

		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (i=menuRoot->mitems.begin(); i!=menuRoot->mitems.end(); i++, iconi++){
			if ((*i)->visible){
				x1=startx+iconi*iconWidth;
				x2=x1+iconWidth;

				//Bug workaround - draw off-screen texture. If not,
				//the glRectf in the next block will not result in
				//anything on screen.
				if (iconi==0){
					glColor4f(1.0,1.0,1.0, 1.0);
					glBindTexture( GL_TEXTURE_2D, (*i)->icon->texture);

					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glBegin( GL_QUADS );
					glTexCoord2f( 0, 1.0 );
					glVertex3f(  100, 100, 0.0f );

					glTexCoord2f( 1.0, 1.0 );
					glVertex3f( 100, 100, 0.0f );

					glTexCoord2f( 1.0, 0 );
					glVertex3f( 100, 100, 0.0f );

					glTexCoord2f( 0, 0 );
					glVertex3f( 100, 100, 0.0f );
					glEnd();
				}

				if ((*i)->selected){
					glBlendFunc(GL_ONE, GL_ONE);
					//glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR);
			//		cerr <<"EEEE: drawing rectangle"<<endl;
					float bwidth=(y2-y1)/10.0;
			//		cerr <<"EEEE: bwidth="<<bwidth<<endl;
					//glColor4f(0.5, 0.5, 0.6, 0.6); 
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glColor4f(1.0, 1.0, 1.0, 0.6); 

					glRectf( x1, y1, x2, y1+bwidth );
					glRectf( x1, y2, x2, y2-bwidth );
					glRectf( x1, y1+bwidth, x1+bwidth, y2-bwidth);
					glRectf( x2, y1+bwidth, x2-bwidth, y2-bwidth);
				}


				glColor4f(1.0,1.0,1.0, 1.0);
				glBindTexture( GL_TEXTURE_2D, (*i)->icon->texture);

				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBegin( GL_QUADS );
				glTexCoord2f( 0, 1.0 );
				glVertex3f(  x1, y1, 0.0f );

				glTexCoord2f( 1.0, 1.0 );
				glVertex3f( x2, y1, 0.0f );

				glTexCoord2f( 1.0, 0 );
				glVertex3f( x2, y2, 0.0f );

				glTexCoord2f( 0, 0 );
				glVertex3f( x1, y2, 0.0f );
				glEnd();


			}


		}
		menuLock.unlock();
	}

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
	updateVideoPositions(false);

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
		Thread::msleep(500);
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

MRef<OpenGLWindow*> OpenGLWindow::getWindow(bool fullscreen){
	if (!globalWindowObj)
		globalWindowObj = new OpenGLWindow(800,  600, fullscreen);
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

	text = new Text("/home/erik/FreeSans.ttf");
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

	if (text)
		text->restartGl();


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



OpenGLDisplay::OpenGLDisplay( uint32_t width, uint32_t height, bool _fullscreen):VideoDisplay(){
	cerr << "EEEE: OpenGLDisplay::OpenGLDisplay("<< width<<","<<height<<","<<fullscreen<<") running"<<endl;
	this->width = width;
	this->height = height;
	fullscreen = _fullscreen;
	nallocated=0;
	//isLocalVideo=false;
	
	gfx= new struct mgl_gfx;

	memset(gfx, 0, sizeof(struct mgl_gfx));
	gfx->texture=-1;
	gfx->aratio=(float)width/(float)height;
	gfx->wu=1;
	gfx->hu=1;
	gfx->tex_dim=0;
	gfx->isSelected=false;

	rgb=NULL;
	needUpload=false;
	newRgbData=false;
	//rgb32=false;
	colorNBytes=3;

	window = * (OpenGLWindow::getWindow(fullscreen) );
	window->sizeHint(width,height);
	massert(window);
	window->setCallback(this);
}


//TODO: remove this method - it does the same thing as in the base class
void OpenGLDisplay::setCallback(MRef<CommandReceiver*> cb){
	callback=cb;
}

void OpenGLDisplay::sendCmd(CommandString cmd){
	if (callback)
		callback->handleCommand("gui",cmd);
}

struct mgl_gfx*  OpenGLDisplay::getTexture(){
	dataLock.lock();
	if (gfx->texture==-1){
		massert(colorNBytes==3 || colorNBytes==4);
		int hw_max_dim = window->getTextureMaxSize();
		int dim = (hw_max_dim>2048) ? 2048 : hw_max_dim;
		gfx->tex_dim=dim;
		glGenTextures( 1, (GLuint*)&(gfx->texture));
		glBindTexture( GL_TEXTURE_2D, gfx->texture);
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
		massert(gfx->texture>0);
		newRgbData=false;
		glBindTexture( GL_TEXTURE_2D, gfx->texture);

		if (width>gfx->tex_dim){
			int factor=2;
			while (width/factor > gfx->tex_dim)
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
			gfx->wu=(newwidth/*/(float)factor*/)/(float)gfx->tex_dim;
			gfx->hu=(newheight/*/(float)factor*/)/(float)gfx->tex_dim;
			gfx->aratio = (float)width/(float)height;
			//cerr << "EEEE: new dim="<<newwidth<<"x"<<newheight<<" and gfx->hu="<<gfx->hu<<" and tex_dim="<<gfx->tex_dim<<endl;
			delete []tmpbuf;
			
		}else{
			//cerr << "EEEE: getTexture: uploading texture of size "<<width<<"x"<<height << " for display "<<(uint64_t)this<< "colorNBytes="<<colorNBytes<< " texdim="<<gfx->tex_dim<<endl;

			GLenum pixFormat = (colorNBytes==3)?GL_RGB:GL_BGRA;

			glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0 , width, height, pixFormat, GL_UNSIGNED_BYTE, rgb );
			gfx->wu=width/(float)gfx->tex_dim;
			gfx->hu=height/(float)gfx->tex_dim;
			gfx->aratio = (float)width/(float)height;
		}
	}
	dataLock.unlock();
	return gfx;
}

void OpenGLDisplay::handle( MImage * mimage){
	//cerr <<"EEEE: doing OpenGLDisplay::handle on display "<<(uint64_t)this<<" image size="<<mimage->width<<"x"<<mimage->height<<" local="<<isLocalVideo<<endl;
	massert(mimage->linesize[1]==0);
	colorNBytes=mimage->linesize[0]/mimage->width;
	dataLock.lock();
	if (!rgb || width!=mimage->width || height!=mimage->height){
		cerr << "EEEE: allocating RGB of size "<<mimage->width<<"x"<<mimage->height<<endl;
		if (rgb)
			delete rgb;
		width=mimage->width;
		height=mimage->height;
		gfx->aratio=(float)width/(float)height;
		rgb = new uint8_t[width*height*colorNBytes+16]; // +16 to avoid mesa bug
	
	}

	massert(rgb);
	memcpy(rgb, &mimage->data[0][0], width*height*colorNBytes); //TODO: don't copy since it is in correct format.
	newRgbData=true;
	if (!isLocalVideo)
		emptyImages.push_back(mimage);
	dataLock.unlock();
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
	gfx->texture=-1;
}

bool OpenGLDisplay::getIsSelected(){
	return gfx->isSelected;
}

void OpenGLDisplay::setIsSelected(bool is){
	gfx->isSelected=is;
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
	gfx->aratio=(float)w/(float)h;
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
	mimage->chroma= M_CHROMA_RV24;

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

bool OpenGLDisplay::handleCommand(CommandString cmd){
	bool handled=false;
	if (cmd.getOp()=="make_proxy"){	//if we will not display any video
		window->removeDisplay(this);
		

	}

	if (cmd.getOp()=="set_texture_path"){
		window->setTexturePath(cmd.getParam());
		handled=true;
	}

	if (cmd.getOp()=="key"){
			window->keyPressed(cmd.getParam(), cmd.getParam2()=="REPEAT");
			handled=true;
	}

	if (cmd.getOp()=="incoming_available"){
		string fromuri = cmd.getParam();
		string callid  = cmd.getDestinationId();
		bool unprotected = cmd.getParam2()=="unprotected";
		window->incomingCall(callid, fromuri, unprotected);
	}

	if (cmd.getOp()=="enable_menu"){
		window->enableMenu();
		handled=true;
	}
	if (!handled){
		cerr <<"EEEE: warning: OpenGLDisplay did not handle command "<< cmd.getOp()<<endl;

	}
	return handled;
	
}

OpenGLPlugin::OpenGLPlugin( MRef<Library *> lib ): VideoDisplayPlugin( lib ){
}

OpenGLPlugin::~OpenGLPlugin(){
}

