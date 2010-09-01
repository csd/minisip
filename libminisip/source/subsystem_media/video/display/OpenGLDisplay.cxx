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
#include<map>

#include<libmutil/Timestamp.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>


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



#define CAM_IDLE      0
#define CAM_ZOOMIN    1
#define CAM_ZOOMOUT   2
#define CAM_PANLEFT  3
#define CAM_PANRIGHT 4
#define CAM_TILTUP   5
#define CAM_TILTDOWN 6

class CameraClientMgr : public Runnable{
	public:
		CameraClientMgr(string remotehost, int remoteport){
			sock = new UDPSocket();
			tohost = remotehost;
			toip = IPAddress::create(tohost);
			toport = remoteport;
			state=CAM_IDLE;
			doStop=false;
			cmdTime=0;
			//speed=1;
		}

		void setIp(string toIp){
			dataLock.lock();
			toip=IPAddress::create(toIp);
			dataLock.unlock();
		}

		void sendCommand(string cmd){
			cmd=cmd+"\n";
			dataLock.lock();
			sock->sendTo(**toip, 3333, cmd.c_str(),cmd.length() );
			dataLock.unlock();
		}

		void setState(int s){
			state=s;
			cmdTime=mtime();
		}

		bool cmdExpired(){
			uint64_t now = mtime();
			if (cmdTime!=0 && now>cmdTime && (now-cmdTime) > 250){ //0.25s
				return true;
			}else{
				return false;
			}
		}

		void zoomStop( ){
			//speed=1;
			switch (state){
				case CAM_ZOOMIN:
				case CAM_ZOOMOUT:
					sendCommand("set_zoom_stop");
					setState(CAM_IDLE);
					break;
			}

		}

		void zoomIn( bool isRepeat ){
			/*			if (!isRepeat)
						speed=1;
						else{
						if (speed<10)
						speed++;
						}
						*/
			switch (state){
				case CAM_IDLE:
					sendCommand("set_zoom_tele_speed 4" );
				case CAM_ZOOMIN:
					setState(CAM_ZOOMIN);	//Update cmdTime for repetitive calls
					break;

				default:
					break;
			}
		}

		void zoomOut( bool isRepeat ){
			/*
			   if (!isRepeat)
			   speed=1;
			   else{
			   if (speed<10)
			   speed++;
			   }
			   */
			switch (state){
				case CAM_IDLE:
					sendCommand("set_zoom_wide_speed 4");
				case CAM_ZOOMOUT:
					setState(CAM_ZOOMOUT);	//Update cmdTime for repetitive calls
					break;

				default:
					break;
			}

		}

		void panStop( ){
			//			speed=1;
			switch (state){
				case CAM_TILTUP:
				case CAM_TILTDOWN:
				case CAM_PANLEFT:
				case CAM_PANRIGHT:
					sendCommand("set_pantilt_stop 1 1");
					setState(CAM_IDLE);
					break;
			}



		}

		void panLeft( bool isRepeat ){
			/*			if (!isRepeat)
						speed=1;
						else{
						if (speed<10)
						speed++;
						}
						*/
			switch (state){
				case CAM_IDLE:
					sendCommand("set_pantilt_left 4 4");
				case CAM_PANLEFT:
					setState(CAM_PANLEFT);	//Update cmdTime for repetitive calls
					break;
				default:
					break;
			}


		}


		void panRight( bool isRepeat ){
			/*			if (!isRepeat)
						speed=1;
						else{
						if (speed<10)
						speed++;
						}
						*/
			switch (state){
				case CAM_IDLE:
					sendCommand("set_pantilt_right 4 4");
				case CAM_PANRIGHT:
					setState(CAM_PANRIGHT);	//Update cmdTime for repetitive calls
					break;
				default:
					break;
			}



		}

		void tiltStop(){
			//			speed=1;
			switch (state){
				case CAM_TILTUP:
				case CAM_TILTDOWN:
					sendCommand("set_pantilt_stop 1 1");
					setState(CAM_IDLE);
					break;
			}

			panStop();

		}

		void tiltUp( bool isRepeat ){
			/*			if (!isRepeat)
						speed=1;
						else{
						if (speed<10)
						speed++;
						}
						*/
			switch (state){
				case CAM_IDLE:
					sendCommand("set_pantilt_up 4 4");
				case CAM_TILTUP:
					setState(CAM_TILTUP);	//Update cmdTime for repetitive calls
					break;
				default:
					break;
			}



		}

		void tiltDown( bool isRepeat ){
			/*			if (!isRepeat)
						speed=1;
						else{
						if (speed<10)
						speed++;
						}
						*/
			switch (state){
				case CAM_IDLE:
					sendCommand("set_pantilt_down 4 4");
				case CAM_TILTDOWN:
					setState(CAM_TILTDOWN);	//Update cmdTime for repetitive calls
					break;
				default:
					break;
			}



		}

		void start(){
			Thread t(this);
		}

		void run(){

			while (!doStop){
				msleep(20);
				if (cmdExpired()){
					switch (state){
						case CAM_ZOOMIN:
						case CAM_ZOOMOUT:
							zoomStop();
							cmdTime=0;
							break;
						case CAM_PANLEFT:
						case CAM_PANRIGHT:
							panStop();
							cmdTime=0;
							break;
						case CAM_TILTUP:
						case CAM_TILTDOWN:
							tiltStop();
							cmdTime=0;
							break;


						default:
							cerr <<"EEEE: WARNING: COMMAND TIMER EXPIRED FOR UNKNOWN STATE "<<state<<endl;
							//massert(1==0);
							break;
					};

				}
			}

		}


	private:
		int speed;
		bool doStop;
		int state;
		uint64_t cmdTime;
		Mutex dataLock;
		string tohost;
		MRef<IPAddress*> toip;
		string toport;
		MRef<UDPSocket*> sock;
};



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
	Animate* rotate;
	bool isSelected;
	char* name;
	char* callId;
};

class Notification{
	public:
		Notification(string message, Text* _text, int _textureGray, int timeout){
			gfx = new struct mgl_gfx;
			memset(gfx,0, sizeof(struct mgl_gfx) );
			gfx->name = strdup(message.c_str());
			text=_text;
			textureGray=_textureGray;
			if (timeout)
				gfx->alpha = new Animate(5000, 0.7, 0, ANIMATE_EXPONENTIAL);
			else
				gfx->alpha = new Animate(0.5);

			gfx->alpha->start();
		}
		~Notification(){
			delete gfx;
			gfx=NULL;
		}

		void draw(float lx1, float ly1,  float lx2, float ly2);

		struct mgl_gfx* gfx;
		Text* text;
		int textureGray;
};


void Notification::draw(float x1, float y1, float x2, float y2){ //NOTE: must be called by internal thread

	static const SDL_Color white={255,255,255};
	static const SDL_Color black={0,0,0};

	glColor4f(1.0,1.0,1.0, gfx->alpha->getVal() );
	glBindTexture( GL_TEXTURE_2D, textureGray);
	massert(glGetError()==GL_NO_ERROR);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	massert(glGetError()==GL_NO_ERROR);
	glBegin( GL_QUADS );
	glTexCoord2f( 0, 1.0 );
	glVertex3f( x1 , y1, 0.0f );

	glTexCoord2f( 1.0, 1.0 );
	glVertex3f( x2, y1, 0.0f );

	glTexCoord2f( 1.0, 0 );
	glVertex3f( x2, y2, 0.0f );

	glTexCoord2f( 0, 0 );
	glVertex3f( x1, y2, 0.0f );
	glEnd();
	massert(glGetError()==GL_NO_ERROR);

	//	cerr <<"EEEE: doing text->draw3d x1="<<x1<< " x2="<<x2<<" y1="<<y1<<" y2="<<y2<<endl;
	text->draw3D( x1,y1,0,  x2,y2,0, gfx->name, 16, white, black, TEXT_ALIGN_CENTER);
}



class Menu{
	public:
		Menu(string _name, struct mgl_gfx* _icon, string _text, bool _visible=true){
			name=_name;
			visible=_visible;
			icon=_icon;
			selected=false;
			text = _text;
			toppicture=NULL;
		}
		Menu* select(int si){ //NOTE: this method is indirectly used from external thread
			list<Menu*>::iterator i;
			int j=0;
			Menu* ret=NULL;
			for (i=mitems.begin();i!=mitems.end();i++,j++){
				(*i)->selected = si==j;
				if((*i)->selected)
					ret=*i;
			}
			return ret;
		}
		int nVisible(){ //NOTE: this method is indirectly(2) used from external thread
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
			//cerr <<"EEEE: doing selectLeft"<<endl;
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
			//cerr <<"EEEE: doing selectRight"<<endl;
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
				if ( (*item)->visible)
					prev=*item;
				item++;
			}
			(*item)->selected=false;
			massert(prev!=NULL);
			prev->selected=true;
			return prev;
		}

		Menu* getMenuItem(string name){ //NOTE: this method is indirectly(2) used from external thread
			int n=0;
			list<Menu*>::iterator i;
			for (i=mitems.begin();i!=mitems.end();i++, n++){
				//			cerr <<"EEEE: comparing with <"<<(*i)->name<<">"<<endl;
				if ((*i)->name==name)
					return *i;
			}
			cerr <<"EEEE: could not find menu item <"<<name <<">"<<endl;
			return NULL;
		}

		string name;
		bool visible;
		bool selected;
		list<Menu*> mitems;
		Menu* selectedItem;
		struct mgl_gfx* icon;
		CommandString command;
		string text;
		struct mgl_gfx* toppicture;
		string toptext;
};

class AddressItem : public MObject {
	public:
		AddressItem(string _name, string _sipuri){
			name=strdup(_name.c_str());
			sipuri=strdup(_sipuri.c_str());
			texture=0;
			wu=wu=0;
			aratio=0;
			tex_dim=0;
			x1=x2=y1=y2=NULL;
			isSelected=false;
		}
		virtual ~AddressItem(){
			if (name)
				free(name);
			if (sipuri)
				free(sipuri);
		}

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
		char* name;
		char* sipuri;

};



class AddressItemList : public MObject{
	public:
		AddressItemList(string name){
			listName=name;
			lastSelect=0;
		}
		string listName;
		list<MRef<AddressItem*> > addresses;
		float headerx1,headery1,headerx2,headery2;
		int lastSelect;

		bool contains(string sipuri){
			list<MRef<AddressItem*> >::iterator i;
			for (i=addresses.begin(); i!=addresses.end(); i++){
				if (sipuri==(*i)->sipuri)
					return true;
			}
			return false;
		}

		void remove(string sipuri){
			list<MRef<AddressItem*> >::iterator i;
			for (i=addresses.begin(); i!=addresses.end(); i++){
				if (sipuri==(*i)->sipuri){
					addresses.erase(i);
					return;
				}
			}
		}

		string getUri(int i){
			list<MRef<AddressItem*> >::iterator li;
			int j=0;
			for (li=addresses.begin(); j<i; j++)
				li++;
			return (*li)->sipuri;
		}



		void selectnone(){
			list<MRef<AddressItem*> >::iterator li;
			int j=0;
			for (li=addresses.begin(); li!=addresses.end(); li++)
				(*li)->isSelected=false;
		}


		int select(int i){
			selectnone();
			if (i<0)
				i=addresses.size()-1;
			if (i>=addresses.size())
				i=0;
			list<MRef<AddressItem*> >::iterator li;
			int j=0;
			for (li=addresses.begin(); j<i; j++)
				li++;
			(*li)->isSelected=true;
			return i;
		}

		int keyDown(int i){
			if (i>=addresses.size())
				i=0;
			select(i);
			return i;
		}
		int keyUp(int i){
			if (i<0)
				i=addresses.size()-1;
			select(i);
			return i;
		}

};


class AddressBookMenu {
	public:
		AddressBookMenu(){
			selectList=0;
			selectItem=0;
		}
		MRef<AddressItemList*> getList(string name){
			list< MRef<AddressItemList*> >::iterator i;
			for (i=menuLists.begin(); i!=menuLists.end(); i++){
				if ( (*i)->listName == name)
					return *i;
			}
			return NULL;
		}

		string getSelectedUri(){
			MRef<AddressItemList*> list = getAt(selectList);
			return list->getUri(selectItem);
		}

		MRef<AddressItemList*> getAt(int i){
			list<MRef<AddressItemList*> >::iterator li;
			int j=0;
			for (li=menuLists.begin(); j<i; j++)
				li++;
			return *li;
		}

		void keyDown(){
			MRef<AddressItemList*> list = getAt(selectList);
			selectItem++;
			selectItem=list->keyDown(selectItem);
			//cerr <<"EEEE: AddressBookMenu: selectList="<<selectList<<" selectItem="<<selectItem<<endl;

		}
		void keyUp(){
			MRef<AddressItemList*> list = getAt(selectList);
			selectItem--;
			selectItem=list->keyUp(selectItem);
			//cerr <<"EEEE: AddressBookMenu: selectList="<<selectList<<" selectItem="<<selectItem<<endl;

		}
		void keyLeft(){
			MRef<AddressItemList*> oldlist = getAt(selectList);
			oldlist->selectnone();
			selectList--;
			if (selectList<0)
				selectList=menuLists.size()-1;
			MRef<AddressItemList*> list = getAt(selectList);
			if (list->addresses.size()==0)	//don't select empty list
				list=oldlist;
			selectItem=list->select(selectItem);
			//cerr <<"EEEE: AddressBookMenu: selectList="<<selectList<<" selectItem="<<selectItem<<endl;
		}
		void keyRight(){

			MRef<AddressItemList*> oldlist = getAt(selectList);
			oldlist->selectnone();
			selectList++;
			if (selectList>=menuLists.size())
				selectList=0;
			MRef<AddressItemList*> list = getAt(selectList);
			if (list->addresses.size()==0) //prevent going to empty list
				list=oldlist;
			selectItem=list->select(selectItem);
			//cerr <<"EEEE: AddressBookMenu: selectList="<<selectList<<" selectItem="<<selectItem<<endl;
		}
		void updateSelected(){
			//cerr <<"EEEE: getting list "<< selectList<<endl;
			MRef<AddressItemList*> l = getAt(selectList);
			massert(l);
			if (l->addresses.size()==0){ //If list is empty, select next. This should only
				//happen when noone is online
				selectList++;
				selectItem=0;
				//cerr <<"EEEE: getting _next_ list "<< selectList<<endl;
				l=getAt(selectList);
			}
			//cerr <<"EEEE: doing select"<<endl;
			selectItem=l->select(selectItem);
			//cerr <<"EEEE: done doing select"<<endl;
		}






		int selectList;
		int selectItem;
		list< MRef<AddressItemList*> > menuLists;
};


#define ZOOM_MODE_OFF 0
#define ZOOM_MODE_ON 1




#define MENU_HIDDEN      1
#define MENU_MAIN        2
#define MENU_ASKACCEPT   3
#define MENU_SETTINGS    4
#define MENU_ADDRESSBOOK 5

#define VIDEO_EQUAL      1
#define VIDEO_PREFERENCE 2
#define VIDEO_TALKER     3

#define LOCAL_VIDEO_OFF 1
#define LOCAL_VIDEO_TRANSPARENT 2

class IrInput;

class OpenGLWindow : public Runnable {
	public:
		OpenGLWindow(int width, int height, bool fullscreen);

		void init();

		void start();
		void stop();
		virtual void run();

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
		void sizeHint(int w, int h){ //NOTE: this method is used from external thread
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

		void setPhoneConfig(MRef<SipSoftPhoneConfiguration*> conf);
		void setCallback(OpenGLDisplay* cb){ //NOTE: this method is used from external thread
			if (!callback)
				callback=cb;
		}

		///send command to callback
		void send(CommandString cmd);
		CommandString sendResp(CommandString cmd);
		void incomingCall(string callid, string uri, bool unprotected);

		void presenceUpdated(){addressBookUpdated=true;}
		void join(){
			thread->join();

		}

		void registerUri(string callid, string remoteuri){ //NOTE: this method is used from external thread
			id_uri[callid]=remoteuri;

		}

		void updateVideoLayout(){ //NOTE: this method is used from external thread
			displaysChanged=true;
		}

		void showNotification(string message, int timeout);
		void clearNotifications();

		void notificationsDraw();

		int nVisibleDisplays();

		void zoomKey(string key, bool isRepeat);

		void setRemoteCamera(string toip);


	private:
		std::map<string, string> id_uri;

		bool loadTexture(string name);


		struct mgl_gfx* getIcon(string name);

		void videoSelect(string key);
		void findVideoArea(int video_n, int ntot, float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, bool preferHorisontal, bool* selectedptr);
		void setupMenu();
		void updateMenu(bool animate);
		void hideAcceptCallMenu(bool hideAllMenus);
		void menuDrawAskAccept();
		void menuDraw();
		void menuLayout(bool animate);
		void menuDrawSettings();
		void menuDrawAddressBook();

		void deadVideoProcessing();

		bool texturesLoaded;
		//		bool showMenu;
		Menu* menuRoot;
		Menu* menuCur;
		Menu* menuItemCur;

		Menu* menuActions;
		Menu* menuAcceptCall;
		Mutex menuLock;
		string texturePath;

		MRef<SipSoftPhoneConfiguration*> pconf;

		bool inCall;
		int animation_ms;
		GLdouble windowX0;
		GLdouble windowY0;
		bool resized;
		void findScreenCoords();

		void updateVideoPositions(bool doAnimate);

		void rectToCoord(float &x1, float& y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, float aratio);
		void drawSurface();
		void sdlQuit();
		void windowResized(int w, int h);
		bool isFullscreen();
		void toggleFullscreen();

		Mutex displayListLock;
		list<OpenGLDisplay*> displays;
		list<struct mgl_gfx*> icons;
		bool displaysChanged;

		OpenGLDisplay* getSelectedDisplay();

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

		GLuint textureGray;

		Text *text;

		bool useGui;
		int menuMode;
		int videoMode;
		int localVideoMode;
		bool localVideoSelected;


		//list< string > addressBookNames;
		//list< list<  MRef<ContactEntry *>  > > addressBook;

		//list<MRef<ContactEntry*> > getAddressBook(){

		//}

		AddressBookMenu addressBookMenu;


		void layoutAddressBook(bool doAnimate);
		bool addressBookUpdated;
		bool addressBookAnimate;

		Mutex notificationLock;
		list<Notification*> notifications;

		MRef<CameraClientMgr*> camClient;
		int zoomMode;
		bool zoomLocal;
		int zoomEscapeCount; 

		IrInput *ir;
};



class IrInput : public Runnable{
	public:
		IrInput(OpenGLWindow* handler);
		void start();
		void stop();
		void run();


	private:
		bool doStop;
		MRef<Semaphore*> quitSignal;
		Thread *thread;
		OpenGLWindow* handler;



};

IrInput::IrInput(OpenGLWindow* h){
	quitSignal=new Semaphore();
	thread=NULL;
	doStop=false;
	handler=h;
}


void IrInput::start(){
	doStop=false;
	thread=new Thread(this);
}

void IrInput::stop(){

	quitSignal->dec();
}


static int iropen(){
	struct sockaddr_un addr;
	addr.sun_family=AF_UNIX;
	strcpy(addr.sun_path,"/dev/lircd");
	int fd=socket(AF_UNIX,SOCK_STREAM,0);
	if(fd==-1)  {
		cerr <<"EEEE: COULD NOT CREATE SOCKET"<<endl;
		return -1;
	};
	if(connect(fd,(struct sockaddr *)&addr,sizeof(addr))==-1)  {
		close(fd);
		return -1;
	};
	return fd;

}

static string parseBuf(string s, bool &isRepeat){
	int len=s.size();

	int i=0;
	while (i<len && s[i]!=' ')	//pass first digits
		i++;

	i++;				//pass space

	if (len>i+2 && s[i]=='0' && s[i+1]=='0')
		isRepeat=false;
	else
		isRepeat=true;

	while (i<len && s[i]!=' ')	//pass repeat counter
		i++;

	i++;				//pass space

	string ret;

	while (i<len && s[i]!=' ')
		ret=ret+s[i++];

	return ret;

}

void IrInput::run(){

	int fd=-1;

	do{
		if (fd==-1)	
			fd=iropen();
		if (fd>0){
		}




		if (fd!=-1){
			int n;
			char buf[129];
			n=read(fd,buf,128);
			if(n==-1 || n==0)  {
				fd=-1;
			};
			buf[n]=0;
			string s(buf);
			//cerr <<"------------> DATA <"<<s<<">"<<endl;
			bool repeat=false;
			string key=parseBuf(s, repeat);
			if (key.size()>0){
				handler->keyPressed(key, repeat);
			}
		}

		if (!doStop&&fd==-1){	//Sleep 10s and then re-try to open
			for (int i=0; i<20 && !doStop; i++)
				Thread::msleep(500);
		}


	}while(!doStop);

	quitSignal->inc();
}

void OpenGLWindow::setRemoteCamera(string toip)
{
	if (camClient)
		camClient->setIp(toip);
}


void OpenGLWindow::clearNotifications(){
	notificationLock.lock();
	notifications.clear( );
	notificationLock.unlock();
}

void OpenGLWindow::showNotification(string message, int timeout){

	Notification* n = new Notification(message, text, textureGray, timeout);

	notificationLock.lock();
	notifications.push_back( n );
	notificationLock.unlock();


}



MRef<OpenGLWindow*> OpenGLWindow::globalWindowObj=NULL;

OpenGLWindow::OpenGLWindow(int w, int h, bool fullscreen){
	cerr<<"EEEE: ------------------------ CREATING OPENGL WINDOW---------------"<<endl;

	ir = new IrInput(this);


	menuMode=MENU_HIDDEN;
	videoMode=VIDEO_EQUAL;
	localVideoMode=LOCAL_VIDEO_TRANSPARENT;

	useGui=false;
	runCount=0;
	inCall=false;
	//	showMenu=false;
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
	animation_ms=400;
	displaysChanged=false;
	video_select_row=0;
	video_select_col=0;
	localVideoSelected=false;
	video_nrows=0;
	video_ncols=0;
	textureGray=0;

	gDrawSurface=NULL;

	callback=NULL;

	t_max_size=0;
	addressBookUpdated=true;
	addressBookAnimate=false;

	bpp=0;

	zoomMode=ZOOM_MODE_OFF;
	zoomEscapeCount=0;

	camClient = new CameraClientMgr("192.16.126.158", 3333);
	camClient->start();


}

void OpenGLWindow::setPhoneConfig(MRef<SipSoftPhoneConfiguration*> conf){ //NOTE: this method is used from external thread
	pconf=conf;
}

CommandString OpenGLWindow::sendResp(CommandString cmd){
	CommandString ret;
	if (callback)
		ret=callback->sendCmdResp(cmd);
	return ret;
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
	//	showMenu=!hideAll;
	if (hideAll)
		menuMode=MENU_HIDDEN;
	else{
		menuItemCur=menuRoot->select(0);

		menuMode=MENU_MAIN;
	}
	menuLock.unlock();
}

void OpenGLWindow::incomingCall(string callid, string uri, bool unprotected){ //NOTE: this method is used from external thread
	menuLock.lock();
	list<Menu*>::iterator i=menuAcceptCall->mitems.begin();
	(*i)->command=CommandString(callid, "accept_invite");	//command for yes button
	i++;
	(*i)->command=CommandString(callid, "reject_invite");	//command for no button

	menuAcceptCall->toptext="Call from <"+uri+">"; //remove - this should be set at each incoming call

	menuRoot=menuAcceptCall;
	menuCur=menuAcceptCall;
	Menu* hangup = menuActions->getMenuItem("hangup");
	massert(hangup);
	hangup->command=CommandString(callid, "hang_up");
	//	showMenu=true;
	menuMode=MENU_ASKACCEPT;
	menuItemCur=*menuAcceptCall->mitems.begin();
	menuLock.unlock();

}

OpenGLDisplay* OpenGLWindow::getSelectedDisplay(){
	displayListLock.lock();
	list<OpenGLDisplay*>::iterator video;
	for (video=displays.begin(); video!=displays.end(); video++){
		if ((*video)->getTexture()->isSelected || (*video)->getIsLocalVideo() &&localVideoSelected){
			OpenGLDisplay* ret = (*video);
			displayListLock.unlock();
			return ret;
		}
	}
	displayListLock.unlock();
	return NULL;
}

void OpenGLWindow::videoSelect(string key){ //NOTE: this method is indirectly used from external thread
	//cerr <<"EEEE: videoSelect("<<key<<")"<<endl;

	if (key=="KEY_PLAY"){
		OpenGLDisplay* selected = getSelectedDisplay();
		if (selected){

			zoomMode=ZOOM_MODE_ON;
			zoomLocal = selected->getIsLocalVideo();
			selected->setShowZoomIcons(true);

#if 0
			float lastrot=0;
			if (selected->getTexture()->rotate){
				lastrot=selected->getTexture()->rotate->getVal();
				delete selected->getTexture()->rotate;
			}
			selected->getTexture()->rotate = new Animate(1000,lastrot, lastrot>1?0.0F:180.0F ,ANIMATE_STARTSTOP);
			selected->getTexture()->rotate->start();
#endif
		}
	}

	if (key=="KEY_CHANNELUP"){
		//cerr <<"EEEE: selecting row up"<<endl;
		//cerr <<"EEEE: video_nrows="<<video_nrows<<endl;
		//cerr <<"EEEE: video_ncols="<<video_ncols<<endl;
		menuLock.lock();
		localVideoSelected=false;

		if (video_select_col>=video_ncols) //if local video selected, decrease column
			video_select_col--;

		video_select_row++;
		if (video_select_row>=video_nrows)
			video_select_row--;
		menuLock.unlock();
		updateVideoPositions(false);
	}
	if (key=="KEY_CHANNELDOWN"){
		//cerr <<"EEEE: selecting row down"<<endl;
		menuLock.lock();
		localVideoSelected=false;

		if (video_select_col>=video_ncols) //if local video selected, decrease column
			video_select_col--;

		video_select_row--;
		if (video_select_row<0)
			video_select_row=0;
		menuLock.unlock();
		updateVideoPositions(false);
	}
	if (key=="BTN_RIGHT"){
		//cerr <<"EEEE: selecting row right"<<endl;
		//cerr <<"EEEE: video_nrows="<<video_nrows<<endl;
		//cerr <<"EEEE: video_ncols="<<video_ncols<<endl;
		menuLock.lock();
		if (video_select_col==video_ncols-1 && video_select_row==0){
			localVideoSelected=true;
		}else{
			localVideoSelected=false;
			video_select_col++;
			if (video_select_col>=video_ncols){
				//if not selecting local video
				//				if ( !(video_select_row==0 && video_select_col==video_ncols) 
				//						&& localVideoMode==LOCAL_VIDEO_TRANSPARENT)
				video_select_col--;
			}
		}

		menuLock.unlock();
		updateVideoPositions(false);
	}
	if (key=="BTN_LEFT"){
		//cerr <<"EEEE: selecting row down"<<endl;
		menuLock.lock();
		localVideoSelected=false;
		video_select_col--;
		if (video_select_col<0)
			video_select_col=0;
		menuLock.unlock();
		updateVideoPositions(false);
	}




}


void OpenGLWindow::zoomKey(string key, bool isRepeat){
	//cerr <<"EEEE: zoomKey: "<< key<<" repeat="<<isRepeat<<endl;
	if (isRepeat || key!="KEY_MENU")
		zoomEscapeCount=0;
	else
		zoomEscapeCount++;

	if (zoomEscapeCount>=2){
		zoomEscapeCount=0;
		zoomMode=ZOOM_MODE_OFF;
	}

	if (zoomMode==ZOOM_MODE_ON){

		if (key=="KEY_PLAY")
			camClient->zoomIn(isRepeat);
		if (key=="KEY_MENU")
			camClient->zoomOut(isRepeat);
		if (key=="KEY_CHANNELUP")
			camClient->tiltUp(isRepeat);
		if (key=="KEY_CHANNELDOWN")
			camClient->tiltDown(isRepeat);
		if (key=="BTN_LEFT")
			camClient->panLeft(isRepeat);
		if (key=="BTN_RIGHT")
			camClient->panRight(isRepeat);
	}

}


void OpenGLWindow::keyPressed(string key, bool isRepeat){ //NOTE: this method is used from external thread
	cerr <<"EEEE: OpenGLWindow::keyPressed: <"<< key <<"> repeat="<< isRepeat<<endl;

	if (isRepeat){
		if (zoomMode==ZOOM_MODE_ON)
			zoomKey(key, isRepeat);
		return;

	}



	if (key=="KEY_MENU"  && !zoomMode==ZOOM_MODE_ON){
		if (menuMode==MENU_ADDRESSBOOK){
			menuItemCur=menuActions->select(0);
			menuMode=MENU_MAIN;
		}else{
			if (menuMode==MENU_HIDDEN){
				menuItemCur=menuActions->select(0);
				menuMode=MENU_MAIN;
			}else{
				menuMode=MENU_HIDDEN;
			}
		}
		return;
	}

	if (menuMode==MENU_HIDDEN){
		if (zoomMode==ZOOM_MODE_OFF)
			videoSelect(key);
		else
			zoomKey(key, isRepeat);
	}

	if (menuMode==MENU_ADDRESSBOOK){

		if (key=="KEY_PLAY"){
			string uri = addressBookMenu.getSelectedUri();


			CommandString resp=sendResp( CommandString("","invite",uri));
			registerUri(resp.getDestinationId(), uri);
			Menu* hangup = menuActions->getMenuItem("hangup");
			massert(hangup);
			hangup->command=CommandString( resp.getDestinationId(), "hang_up" );

			menuMode=MENU_HIDDEN;
		}


		if (key=="BTN_LEFT"){
			addressBookMenu.keyLeft();
			addressBookAnimate=false;
			addressBookUpdated=true;
		}
		if (key=="BTN_RIGHT"){
			addressBookMenu.keyRight();
			addressBookAnimate=false;
			addressBookUpdated=true;
		}
		if (key=="KEY_CHANNELUP"){
			addressBookMenu.keyUp();
			addressBookAnimate=false;
			addressBookUpdated=true;
		}
		if (key=="KEY_CHANNELDOWN"){
			addressBookMenu.keyDown();
			addressBookAnimate=false;
			addressBookUpdated=true;
		}




	}

	if (menuMode!=MENU_HIDDEN){


		if (key=="BTN_LEFT"){
			menuLock.lock();
			menuItemCur = menuCur->selectLeft();
			updateMenu(true);
			menuLock.unlock();
		}
		if (key=="BTN_RIGHT"){
			menuLock.lock();
			menuItemCur = menuCur->selectRight();
			updateMenu(true);
			menuLock.unlock();

		}
		if (key=="KEY_PLAY"){

			clearNotifications();

			menuLock.lock();
			string cmd = menuItemCur->command.getOp();
			//cerr <<"EEEE: MENU SELECTED: "<< cmd<<endl;

			if (cmd!=""){
				if (cmd=="exit"){
					doStop=true;
				}
				if (cmd=="invite"){
					addressBookUpdated=true;
					menuMode=MENU_ADDRESSBOOK;
					//addressBookMenu.selectList=0;
					//addressBookMenu.selectItem=0;
					menuLock.unlock();
					//addressBookMenu.keyUp();
					//addressBookMenu.keyDown();
				}else{
					notifications.clear();
					CommandString cmd = menuItemCur->command;
					menuLock.unlock();
					send( menuItemCur->command );
				}
			}else{
				menuLock.unlock();
			}

			if (menuRoot==menuAcceptCall)
				hideAcceptCallMenu(cmd=="accept_invite");

		}



	}

}

void OpenGLWindow::setTexturePath(string p){ //NOTE: this method is used from external thread
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
bool OpenGLWindow::loadTexture(string fname){ //NOTE: must be called by internal thread
	texturesLoaded=true;
	//cerr <<"EEEE: OpenGLWindow::loadTexture: trying to load "<<fname<<endl;

	string path = "/home/hdviaosn/share/minisip/"+fname+".raw";
	int len=ICON_DIM*ICON_DIM*4;
	byte_t *tmp = new byte_t[len+16];
	//cerr <<"EEEE: opening file <"<<path<<">"<<endl;
	ifstream inf;
	inf.open(path.c_str(), ios::binary);
	//cerr <<"EEEE: reading file..."<<endl;
	inf.read((char*)tmp,len);

	struct mgl_gfx* t = new struct mgl_gfx;
	memset(t,0,sizeof(struct mgl_gfx));
	t->isSelected=true;
	t->hu=1.0;
	t->wu=1.0;
	t->tex_dim=256;

	glGenTextures( 1, (GLuint*)&(t->texture));
	massert(glGetError()==GL_NO_ERROR);
	//cerr <<"EEEE: generated texture id "<< t->texture<<endl;
	massert(t->texture > 0);
	glBindTexture( GL_TEXTURE_2D, t->texture);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//cerr <<"EEEE: uploading to GL..."<<endl;
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, ICON_DIM, ICON_DIM, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
	massert(glGetError()==GL_NO_ERROR);

	t->name=strdup(fname.c_str());

	icons.push_back(t);

	delete[] tmp;
	//cerr <<"EEEE: done loading texture"<<endl;

	if (textureGray==0){
		byte_t* gray=(byte_t*)malloc(8*3*8*3+16);
		memset(gray,0xFF,8*3*8*3);

		glGenTextures( 1, &textureGray);
		massert(textureGray > 0);
		glBindTexture( GL_TEXTURE_2D, textureGray);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, gray);

	}
	massert(glGetError()==GL_NO_ERROR);
}

void OpenGLWindow::updateMenu(bool animate){ //NOTE: this method is indirectly used from external thread
	if (menuActions){
		massert(menuActions);
		massert(menuActions->getMenuItem("hangup"));
		massert(menuActions->getMenuItem("exit"));
		menuActions->getMenuItem("hangup")->visible=inCall;
		menuActions->getMenuItem("exit")->visible=!inCall;
	}
	if (menuRoot)
		menuLayout(animate);
}

void OpenGLWindow::setupMenu(){

	menuLock.lock();
	menuActions=new Menu("",NULL,"");
	menuCur=menuRoot=menuActions;

	Menu* m;

	const char* names[]={"invite","settings","hangup","exit" ,0};
	const char* names_text[]={"Make call","Settings","Hang up","Exit application" , 0};

	int i=0;
	while (names[i++]){
		struct mgl_gfx* icon = getIcon(names[i-1]);
		massert(icon);
		m = new Menu( names[i-1], icon, names_text[i-1]);
		if (strcmp(names[i-1],"exit")==0 )
			m->visible=false;
		m->command=CommandString("",names[i-1]);
		menuActions->mitems.push_back(m);
	}
	(*(menuActions->mitems.begin()))->selected=true;
	menuItemCur = *(menuActions->mitems.begin());


	menuAcceptCall=new Menu( "", NULL, "");

	menuAcceptCall->toppicture =getIcon("person");
	menuAcceptCall->toptext="Incoming call"; //remove - this should be set at each incoming call

	struct mgl_gfx* icon = getIcon("yes");
	massert(icon);
	m = new Menu("yes",icon, "Accept");
	menuAcceptCall->mitems.push_back(m);

	icon = getIcon("no");
	massert(icon);
	m = new Menu("no",icon, "Reject");
	menuAcceptCall->mitems.push_back(m);
	(*(menuAcceptCall->mitems.begin()))->selected=true;


	updateMenu(false);

	menuLock.unlock();

}

void OpenGLWindow::enableMenu(){ //NOTE: this method is used from external thread
	massert(texturePath.size()>0);
	/*showMenu=true;*/

	if (menuActions)
		menuItemCur=menuActions->select(0);
	menuMode=MENU_MAIN;
	useGui=true;
}

#define REPORT_N 500


void OpenGLWindow::findScreenCoords() { //NOTE: must be called by internal thread
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
	GLdouble delta=-DRAW_Z;
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
	delta=-DRAW_Z;

	for (i=0; i<64; i++){

		int ret = gluProject(x,y,z, model,proj,view, &winx, &winy, &winz);

		if (winy<0)
			y=y+delta;
		else
			y=y-delta;
		delta=delta/2;
	}
	windowY0=y;
	massert(glGetError()==GL_NO_ERROR);

	//cerr << "EEEE: findScreenCoords: window 0,0 is at coord "<<windowX0 << ","<< windowY0 <<",-20"<<endl;


}


void OpenGLWindow::rectToCoord(float &x1, float&y1, float &x2, float &y2, float lx1, float ly1, float lx2, float ly2, float aratio){ //NOTE: this method is indirectly (2) used from external thread

	float l_aratio=(lx2-lx1)/(ly2-ly1);
	//	cerr <<"l_aratio="<<l_aratio<<endl;
	//	cerr <<"aratio="<<aratio<<endl;

	if (aratio > l_aratio) { // fill full width, center vertically
		//		cerr <<"fill width"<<endl;
		float h=(lx2-lx1)/aratio;
		float lh=ly2-ly1;
		x1=lx1;
		y1=ly1+((lh-h)/2.0);
		x2=lx2;
		y2=y1+h;


	}else{	//fill full height
		//		cerr <<"fill height"<<endl;
		y1=ly1;
		float w=(ly2-ly1)*aratio;

		//		cerr <<"w="<<w<<endl;

		float lw=lx2-lx1;
		//		cerr <<"lw="<<lw<<endl;
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
		if (row==video_select_row && (col==video_select_col) && !localVideoSelected){
			//cerr <<"EEEE: Video SELECTED: row="<<row<<" col="<<col<<" video_select_row="<<video_select_row<<" video_select_col="<<video_select_col<<endl;
			*selectedptr=true;
		}else{
			//cerr <<"EEEE: Video not selected: row="<<row<<" col="<<col<<" video_select_row="<<video_select_row<<" video_select_col="<<video_select_col<<endl;
			*selectedptr=false;	
		}
	}



}

void OpenGLWindow::deadVideoProcessing(){

	//	cerr <<"EEEE: deadVideoProcessing() start"<<endl;
	bool doUpdate=false;;
	displayListLock.lock();
	list<OpenGLDisplay*>::iterator i;
	uint64_t now10k=mtime()+10000;
	int I=0;
	for (i=displays.begin(); i!=displays.end(); i++){ 
		//cerr <<"EEEE: video I="<<I++<<endl;
		if ((*i)->isHidden())
			continue;
		mgl_gfx* gfx = (*i)->getTexture();
		//(*i)->dataLock.lock();
		uint64_t trecv= (*i)->timeLastReceive;
		//(*i)->dataLock.unlock();
		uint64_t udiff=now10k;
		udiff-=trecv;
		int diff=(int) udiff;
		diff-=10000;
		if (diff<0 )
			diff=1;
		//cerr <<"EEEE: trecv="<<trecv<<endl;
		//cerr <<"EEEE: now="<<now<<endl;
		//cerr <<"EEEE: time since last frame="<< diff<< endl;
		if ( (*i)->timeLastReceive!=0 && diff>1000){
			float alpha=1.0;
			if (  gfx->alpha ){
				alpha = gfx->alpha->getVal();
			}
			if ( alpha>=0.999999){

				//cerr <<"EEEE: dead video: creating alpha animation"<<endl;
				if (gfx->alpha)
					delete gfx->alpha;
				gfx->alpha=new Animate(500,1.0,0.0,ANIMATE_LINEAR);
				gfx->alpha->start();
			}
			if (alpha<0.01){
				//cerr <<"EEEE: alpha is zero - disabling video"<<endl;
				//displayListLock.unlock();
				//removeDisplay(*i);
				//displayListLock.lock();
				(*i)->setHidden(true);
				doUpdate=true;
				break;
			}
		}

	}

	displayListLock.unlock();
	if (doUpdate)
		updateVideoPositions(true);
	//	cerr <<"EEEE: deadVideoProcessing() end"<<endl;
}

int OpenGLWindow::nVisibleDisplays(){
	int ret=0;
	list<OpenGLDisplay*>::iterator i;
	for (i=displays.begin(); i!=displays.end(); i++){
		if (!(*i)->isHidden())
			ret++;
	}
	return ret;
}

void OpenGLWindow::updateVideoPositions(bool doAnimate){ //NOTE: this method is indirectly used from external thread

	//cerr << "EEEE: doing updateVideoPositions()"<<endl;
	displayListLock.lock();
	int n_videos = /*displays.size()*/ nVisibleDisplays();

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
		if ((*i)->isHidden()){
			global_n--;
			continue;
		}

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
			massert(gfx->alpha);
			lastAlpha=gfx->alpha->getVal();
			delete gfx->alpha;
		}


		//cerr << "EEEE: setting position of video "<< global_n <<" to " << tex_x1<<","<<tex_y1<<" "<<tex_x2<<","<<tex_y2<<" alpha="<<alpha<<endl;
		//cerr<<"EEEE: !!!!!!! laying out video with aratio="<<gfx->aratio<<endl;

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

void OpenGLWindow::addDisplay(OpenGLDisplay* displ){ //NOTE: this method is used from external thread
	inCall=true;
	//cerr << "EEEE: doing addDisplay()"<<endl;
	displayListLock.lock();
	displays.push_back(displ);
	displaysChanged=true;
	//cerr<<"EEEE: --------------------------------- after add ndisplays="<< displays.size()<<endl;
	displayListLock.unlock();

	menuLock.lock();
	updateMenu(false);
	menuLock.unlock();

	updateVideoPositions(true);
}

void OpenGLWindow::removeDisplay(OpenGLDisplay* displ){ //NOTE: this method is used from external thread
	//cerr <<"EEEE: removeDisplay() running"<<endl;
	displayListLock.lock();
	displays.remove(displ);
	inCall=displays.size()>0;
	displaysChanged=true;
	if ( !inCall && useGui ){
		menuLock.lock();
		if (menuActions)
			menuItemCur=menuActions->select(0);
		menuMode=MENU_MAIN;
		menuLock.unlock();
	}

	cerr<<"EEEE: --------------------------------- after remove ndisplays="<< displays.size()<<endl;
	displayListLock.unlock();
	if (useGui){
		menuLock.lock();
		updateMenu(false);
		menuLock.unlock();
	}
	updateVideoPositions(true);

}

void OpenGLWindow::menuDrawAskAccept(){

}

void OpenGLWindow::menuDrawSettings(){

}

bool lessthan_addritem(MRef<AddressItem*> a, MRef<AddressItem*> b){
	massert(a);
	massert(b);
	massert(a->sipuri);
	massert(b->sipuri);
	if (strcmp(a->sipuri, b->sipuri)<0)
		return true;
	else
		return false;
}

/*
 * Phonebook= an AddressItemList
 * PhoneBookPerson does not have a representation
 * ContactEntry=AddressItem
 */

void OpenGLWindow::layoutAddressBook(bool doAnimate){

	//cerr <<"EEEE: started layoutAddressBook"<<endl;
	int nonline=0;
	std::list<MRef<PhoneBook *> >::iterator i;

	//cerr <<"EEEE: nphonebooks="<<pconf->phonebooks.size()<<endl;

	MRef<AddressItemList*> onlinelist= addressBookMenu.getList( "Online" );

	if (!onlinelist){
		//cerr <<"EEEE: adding online list"<<endl;
		onlinelist=new AddressItemList("Online");
		addressBookMenu.menuLists.push_front(onlinelist);
	}else{
		//onlinelist->addresses.clear();
	}

	for (i=pconf->phonebooks.begin(); i!=pconf->phonebooks.end() ; i++){
		list< MRef<PhoneBookPerson *> > persons = (*i)->getPersons();
		list< MRef<PhoneBookPerson *> >::iterator j;
		MRef<AddressItemList*> alist = addressBookMenu.getList( (*i)->getName() );
		if (!alist){
			//cerr <<"EEEE: adding list"<< (*i)->getName() <<endl;
			alist=new AddressItemList((*i)->getName() );
			addressBookMenu.menuLists.push_back(alist);
		}
		//cerr <<"EEEE: persons in book="<<persons.size()<<endl;

		for (j=persons.begin(); j!=persons.end(); j++){
			list< MRef<ContactEntry *> > entries = (*j)->getEntries();
			list< MRef<ContactEntry *> >::iterator ent;
			//cerr <<"EEEE: contacts per person: "<<entries.size()<<endl;;
			for (ent=entries.begin(); ent!=entries.end(); ent++){
				if (!alist->contains( (*ent)->getUri() ) ){ // do uri comparison and not string
					//cerr <<"EEEE: adding user "<< (*ent)->getUri() <<" to list"<<endl;
					alist->addresses.push_back(new AddressItem( (*ent)->getName(), (*ent)->getUri() ));

				}
				if ((*ent)->isOnline()){
					if (!onlinelist->contains((*ent)->getUri() )){
						//TODO: push sorted, and not at end
						//cerr <<"EEEE: online user "<< (*ent)->getUri() <<" to list"<<endl;
						onlinelist->addresses.push_front(new AddressItem( (*ent)->getName(), (*ent)->getUri() ));
						onlinelist->addresses.sort(lessthan_addritem);
					}

				}
				if (!(*ent)->isOnline() && onlinelist->contains((*ent)->getUri())){
					onlinelist->remove( (*ent)->getUri() );
				}
			}
		}
	}

	//cerr << "EEEE: address lists:"<<endl;
	list<MRef<AddressItemList*> >::iterator k;

	int nlists = addressBookMenu.menuLists.size();
	//for all AddressItemList in menuList
	int n=0;
	for (k=addressBookMenu.menuLists.begin(); k!=addressBookMenu.menuLists.end(); k++,n++){
		//		cerr <<"EEEE: Address book <"<<(*k)->listName<<">"<<endl;
		float listlx1=windowX0;
		float listlx2=-windowX0;
		float listly1=windowY0;
		float listly2=-windowY0;
		float listwidth=(listlx2-listlx1)/nlists;
		float headerheight=(listly2-listly1)/5;


		float listx1=listlx1+n*listwidth;
		float listx2=listlx1+(n+1)*listwidth;

		//layout list name
		//
		(*k)->headerx1=listx1;
		(*k)->headerx2=listx2;
		(*k)->headery1=listly2-headerheight;
		(*k)->headery2=listly2-headerheight/2;

		//layout menu items

		MRef<AddressItemList*> list = (*k); 
		float listheight=(listly2-listly1-headerheight)/20;
		std::list< MRef<AddressItem*> >::iterator j;
		int m=0;
		//for all AddressItems in addresses
		//
		for (j= (*k)->addresses.begin(); j!=(*k)->addresses.end(); j++,m++){
			bool hasLastPosition=false;

			float listy1=listly2-(m+1)*listheight - headerheight;	
			float listy2=listly2-m*listheight-headerheight;	

			float lastx1;
			float lastx2;
			float lasty1;
			float lasty2;
			float lastalpha;

			if ( (*j)->x1 ){
				lastx1=(*j)->x1->getVal();
				lastx2=(*j)->x2->getVal();
				lasty1=(*j)->y1->getVal();
				lasty2=(*j)->y2->getVal();
				lastalpha=(*j)->alpha->getVal();
				delete (*j)->x1;
				delete (*j)->x2;
				delete (*j)->y1;
				delete (*j)->y2;
				delete (*j)->alpha;
			}else{
				lastx1=listx1;
				lastx2=listx2;
				lasty1=listy1;
				lasty2=listy2;
				lastalpha=0;
			}
			if (!doAnimate){
				lastx1=listx1;
				lastx2=listx2;
				lasty1=listy1;
				lasty2=listy2;
				lastalpha=1;
			}
			(*j)->x2=new Animate(animation_ms,lastx2, listx2, ANIMATE_STARTSTOP );
			(*j)->y1=new Animate(animation_ms,lasty1, listy1, ANIMATE_STARTSTOP );
			(*j)->y2=new Animate(animation_ms,lasty2, listy2, ANIMATE_STARTSTOP );
			(*j)->alpha=new Animate(animation_ms,lastalpha, 1.0, ANIMATE_STARTSTOP );
			(*j)->x2->start();
			(*j)->y1->start();
			(*j)->y2->start();
			(*j)->alpha->start();
			(*j)->x1=new Animate(animation_ms,lastx1, listx1, ANIMATE_STARTSTOP );
			(*j)->x1->start();

		} 
	}


}

void OpenGLWindow::menuDrawAddressBook(){ //NOTE: must be called by internal thread

	static const SDL_Color white={255,255,255};
	static const SDL_Color black={0,0,0};
	massert(pconf);
	if (addressBookUpdated){
		layoutAddressBook(addressBookAnimate);
		addressBookMenu.updateSelected();
		addressBookAnimate=true;
		addressBookUpdated=false;
	}

	massert(glGetError()==GL_NO_ERROR);
	glColor4f(1.0,1.0,1.0, 0.3);
	glBindTexture( GL_TEXTURE_2D, textureGray);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin( GL_QUADS );
	glTexCoord2f( 0, 1.0 );
	float borderOffset=-windowX0/25;
	glVertex3f(  windowX0+borderOffset, windowY0+borderOffset, 0.0f );

	glTexCoord2f( 1.0, 1.0 );
	glVertex3f( -windowX0-borderOffset, windowY0+borderOffset, 0.0f );

	glTexCoord2f( 1.0, 0 );
	glVertex3f( -windowX0-borderOffset, -windowY0-borderOffset, 0.0f );

	glTexCoord2f( 0, 0 );
	glVertex3f( windowX0+borderOffset, -windowY0-borderOffset, 0.0f );
	glEnd();
	massert(glGetError()==GL_NO_ERROR);


	list<MRef<AddressItemList*> >::iterator k;

	int nlists = addressBookMenu.menuLists.size();
	//for all AddressItemList in menuList
	int n=0;
	for (k=addressBookMenu.menuLists.begin(); k!=addressBookMenu.menuLists.end(); k++,n++){
		//		cerr <<"EEEE: Address book <"<<(*k)->listName<<"> of size "<< (*k)->addresses.size() <<endl;

		glColor4f( 1.0, 1.0, 1.0, 1.0 ); 
		text->draw3D((*k)->headerx1,(*k)->headery1,0, (*k)->headerx2,(*k)->headery2,0, (*k)->listName, 36, white, black, TEXT_ALIGN_CENTER);


		MRef<AddressItemList*> list = (*k); 
		std::list< MRef<AddressItem*> >::iterator j;
		int m=0;
		//for all AddressItems in addresses
		//
		for (j= (*k)->addresses.begin(); j!=(*k)->addresses.end(); j++,m++){


			float x1= (*j)->x1->getVal();
			float y1= (*j)->y1->getVal();
			float x2= (*j)->x2->getVal();
			float y2= (*j)->y2->getVal();
			float alpha= (*j)->alpha->getVal();
			//			cerr <<"EEEE: using alpha="<<alpha<<endl;


			if ((*j)->isSelected){

				glColor4f(1.0,1.0,1.0, 0.3);
				glBindTexture( GL_TEXTURE_2D, textureGray);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBegin( GL_QUADS );
				glTexCoord2f( 0, 1.0 );
				glVertex3f( x1 , y1, 0.0f );

				glTexCoord2f( 1.0, 1.0 );
				glVertex3f( x2, y1, 0.0f );

				glTexCoord2f( 1.0, 0 );
				glVertex3f( x2, y2, 0.0f );

				glTexCoord2f( 0, 0 );
				glVertex3f( x1, y2, 0.0f );
				glEnd();
			}


			glColor4f(1.0, 1.0, 1.0, alpha); 
			massert(glGetError()==GL_NO_ERROR);
			text->draw3D( x1,y1,0, x2,y2,0, /*(*j)->sipuri*/ (*j)->name, 24, white, black, TEXT_ALIGN_CENTER);
			massert(glGetError()==GL_NO_ERROR);
		}


	} 
}


void OpenGLWindow::menuLayout(bool animate){ //NOTE: this method is indirectly used from external thread
	//cerr <<"EEEE: menuLayout called"<<endl;
	int nicons=menuRoot->nVisible();
	float x1= windowX0;
	float y1= windowY0;
	float x2= -windowX0;
	float y2= -windowY0;


	int nvideos=displays.size();
	//	cerr <<"EEEE: nvideos="<<nvideos<<endl;
	if (nvideos==0){ 	//limit to middle 40% of height
		//			cerr <<"EEEE: layout middle"<<endl;
		y1= y1/5;  //fifth of screen, below center
		y2= y2/5;
		rectToCoord(x1,y1, x2,y2,  x1,y1, x2,y2,  1.0); //draw menu on bottom 1/8 of screen
		if (menuRoot->toppicture){
			float height=y2-y1;
			y1=y1-height;
			y2=y2-height;
		}
	}else{
		//		cerr <<"EEEE: layout bottom"<<endl;
		y2= y1+(y2-y1)/8   ;
		rectToCoord(x1,y1,x2,y2,x1,y1,x2,y2,1.0);
	}

	float iconWidth=x2-x1;
	float totWidth=iconWidth*nicons;
	float startx=-totWidth/2;

	float bottomOffset = (y2-y1)/4;	//Note: The icons are drawn outside of their bounds by this amount


	int iconi=0;
	list<Menu*>::iterator i;
	for (i=menuRoot->mitems.begin(); i!=menuRoot->mitems.end(); i++, iconi++){
		if ((*i)->visible){
			x1=startx+iconi*iconWidth;
			x2=x1+iconWidth;
			float stretch = /*x1/10*/ 0.0F;

			float lastx1=x1;
			float lastx2=x2;
			float lasty1=y1+bottomOffset;
			float lasty2=y2+bottomOffset;

			if ((*i)->icon->x1!=NULL){
				if (animate){
					lastx1= (*i)->icon->x1->getVal();
					lastx2= (*i)->icon->x2->getVal();
					lasty1= (*i)->icon->y1->getVal();
					lasty2= (*i)->icon->y2->getVal();
				}

				delete (*i)->icon->x1;
				delete (*i)->icon->x2;
				delete (*i)->icon->y1;
				delete (*i)->icon->y2;

				(*i)->icon->x1=NULL;
				(*i)->icon->x2=NULL;
				(*i)->icon->y1=NULL;
				(*i)->icon->y2=NULL;
			}

			if ( (*i)->selected){
				stretch = (y2-y1)/15.0;

				if (stretch<0)
					stretch=-stretch;

				(*i)->icon->x1 = new Animate(2000, x1, x1-stretch, ANIMATE_SINUS);
				(*i)->icon->x1->start();
				long long stime = (*i)->icon->x1->getStartTime();

				(*i)->icon->x2 = new Animate(2000, x2, x2+stretch, ANIMATE_SINUS);
				(*i)->icon->x2->start( stime );

				(*i)->icon->y1 = new Animate(2000, y1+bottomOffset, y1+bottomOffset-stretch, ANIMATE_SINUS );
				(*i)->icon->y1->start(stime);

				(*i)->icon->y2 = new Animate(2000, y2+bottomOffset, y2+bottomOffset+stretch, ANIMATE_SINUS);
				(*i)->icon->y2->start(stime);
			}else{
				(*i)->icon->x1 = new Animate(300, lastx1, x1, ANIMATE_LINEAR);
				(*i)->icon->x2 = new Animate(300, lastx2, x2, ANIMATE_LINEAR);
				(*i)->icon->y1 = new Animate(300, lasty1, y1+bottomOffset, ANIMATE_LINEAR);
				(*i)->icon->y2 = new Animate(300, lasty2, y2+bottomOffset, ANIMATE_LINEAR);
				(*i)->icon->x1->start();
				long long stime=(*i)->icon->x1->getStartTime();
				(*i)->icon->x2->start(stime);
				(*i)->icon->y1->start(stime);
				(*i)->icon->y2->start(stime);
			}

		}else{
			iconi--; //don't count invisible icons
		}
	}

}

void OpenGLWindow::menuDraw(){ //NOTE: must be called by internal thread
	menuLock.lock();
	massert(menuRoot);
	bool drawBackground=true;


	if (menuMode==MENU_ADDRESSBOOK){
		menuDrawAddressBook();
		menuLock.unlock();
		return;
	}

	int nicons=menuRoot->nVisible();
	float x1= windowX0;
	float y1= windowY0;
	float x2= -windowX0;
	float y2= -windowY0;

	int nvideos=displays.size();
	//	cerr <<"EEEE: nvideos="<<nvideos<<endl;
	if (nvideos==0){ 	//limit to middle 40% of height
		//			cerr <<"EEEE: layout middle"<<endl;
		y1= y1/5;  //fifth of screen, below center
		y2= y2/5;
		rectToCoord(x1,y1, x2,y2,  x1,y1, x2,y2,  1.0); //draw menu on bottom 1/8 of screen
		if (menuRoot->toppicture){
			float height=y2-y1;
			y1=y1-height;
			y2=y2-height;
		}
		drawBackground=false;
	}else{
		//		cerr <<"EEEE: layout bottom"<<endl;
		y2= y1+(y2-y1)/8   ;
		rectToCoord(x1,y1,x2,y2,x1,y1,x2,y2,1.0);
	}
	float bottomOffset = (y2-y1)/4;	//Note: The icons are drawn outside of their bounds by this amount

	float iconWidth=x2-x1;
	float totWidth=iconWidth*nicons;
	float startx=-totWidth/2;

	int iconi=0;
	list<Menu*>::iterator i;

	static const SDL_Color white={255,255,255};
	static const SDL_Color black={0,0,0};

	massert(glGetError()==GL_NO_ERROR);
	if (menuRoot->toppicture){
		float px1=-100;	//don't limit on width
		float px2=100;
		float py1=y2;
		float py2=y2+iconWidth;
		rectToCoord(px1,py1, px2,py2, px1,py1, px2,py2, 1.0);

		//		cerr <<"EEEE: toppicture: getting texture"<<endl;
		glColor4f(1.0,1.0,1.0, 1.0);
		glBindTexture( GL_TEXTURE_2D, menuRoot->toppicture->texture);
		//		cerr <<"EEEE: toppicture: drawing"<<endl;

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin( GL_QUADS );
		glTexCoord2f( 0, 1.0 );
		glVertex3f(  px1, bottomOffset*2 + py1, 0.0f );

		glTexCoord2f( 1.0, 1.0 );
		glVertex3f( px2, bottomOffset*2 + py1, 0.0f );

		glTexCoord2f( 1.0, 0 );
		glVertex3f( px2, bottomOffset*2 + py2, 0.0f );

		glTexCoord2f( 0, 0 );
		glVertex3f( px1, bottomOffset*2 + py2, 0.0f );
		glEnd();
		massert(glGetError()==GL_NO_ERROR);
		text->draw3D(  px1+iconWidth, bottomOffset*2+py1, 0,   px1+iconWidth*4, bottomOffset*2+py2,0, menuRoot->toptext, 32, white, black, TEXT_ALIGN_CENTER);
		massert(glGetError()==GL_NO_ERROR);

	}
	if (drawBackground){

		glColor4f(1.0,1.0,1.0, 0.3);
		glBindTexture( GL_TEXTURE_2D, textureGray);
		//		cerr <<"EEEE: toppicture: drawing background"<<endl;

		float bgx1=-totWidth/2-iconWidth/4;
		float bgx2=totWidth/2+iconWidth/4;
		float bgy1=y1;
		float bgy2=y2+iconWidth/2;

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin( GL_QUADS );
		glTexCoord2f( 0, 1.0 );
		glVertex3f( bgx1 , bgy1, 0.0f );

		glTexCoord2f( 1.0, 1.0 );
		glVertex3f( bgx2, bgy1, 0.0f );

		glTexCoord2f( 1.0, 0 );
		glVertex3f( bgx2, bgy2, 0.0f );

		glTexCoord2f( 0, 0 );
		glVertex3f( bgx1, bgy2, 0.0f );
		glEnd();
		massert(glGetError()==GL_NO_ERROR);
	}


	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (i=menuRoot->mitems.begin(); i!=menuRoot->mitems.end(); i++, iconi++){
		if ((*i)->visible){
			x1=startx+iconi*iconWidth;
			x2=x1+iconWidth;

			//Bug workaround - draw off-screen texture. If not,
			//the glRectf in the next block will not result in
			//anything on screen.
			if (iconi==0){
				massert(glGetError()==GL_NO_ERROR);
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
				massert(glGetError()==GL_NO_ERROR);
			}


			if ((*i)->selected){
				massert(glGetError()==GL_NO_ERROR);
				glBlendFunc(GL_ONE, GL_ONE);
				float bwidth=(y2-y1)/10.0;
				//glColor4f(0.5, 0.5, 0.6, 0.6); 
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glColor4f(1.0, 1.0, 1.0, 0.6); 
				float bottomOffset = (y2-y1)/4;
				//				cerr <<"EEEE: bottomOffset="<<bottomOffset<<endl;

				glRectf( x1, bottomOffset + y1,        x2,        bottomOffset + y1+bwidth);
				glRectf( x1, bottomOffset + y2-bwidth,        x2,        bottomOffset + y2);
				glRectf( x1, bottomOffset + y1+bwidth, x1+bwidth, bottomOffset + y2-bwidth);
				glRectf( x2-bwidth, bottomOffset + y1+bwidth, x2, bottomOffset + y2-bwidth);
				massert(glGetError()==GL_NO_ERROR);
				if ((*i)->selected){

					SDL_Color white={255,255,255};
					SDL_Color black={0,0,0};

					text->draw3D(x1-200,y1,0, x2+200,y1+bottomOffset,0, (*i)->text ,36, white, black, TEXT_ALIGN_CENTER);
					massert(glGetError()==GL_NO_ERROR);

				}
			}
			if ((*i)->icon->x1==NULL){
				updateMenu(false);
			}
			massert((*i)->icon->x1);
			massert((*i)->icon->x2);
			massert((*i)->icon->y1);
			massert((*i)->icon->y2);

			glColor4f(1.0,1.0,1.0, 1.0);
			glBindTexture( GL_TEXTURE_2D, (*i)->icon->texture);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBegin( GL_QUADS );
			glTexCoord2f( 0, 1.0 );
			//glVertex3f(  x1, bottomOffset + y1, 0.0f );
			glVertex3f(  (*i)->icon->x1->getVal(), (*i)->icon->y1->getVal(), 0.0f );

			glTexCoord2f( 1.0, 1.0 );
			glVertex3f( (*i)->icon->x2->getVal(), (*i)->icon->y1->getVal(), 0.0f );

			glTexCoord2f( 1.0, 0 );
			//glVertex3f( x2, bottomOffset + y2, 0.0f );
			glVertex3f( (*i)->icon->x2->getVal(), (*i)->icon->y2->getVal(), 0.0f );

			glTexCoord2f( 0, 0 );
			//glVertex3f( x1, bottomOffset + y2, 0.0f );
			glVertex3f( (*i)->icon->x1->getVal(), (*i)->icon->y2->getVal(), 0.0f );
			glEnd();
			massert(glGetError()==GL_NO_ERROR);


		}else{
			iconi--; //don't count invisible icons
		}


	}
	menuLock.unlock();

}

void OpenGLWindow::notificationsDraw(){ //NOTE: must be called by internal(2) thread
	list<Notification*>::iterator i;

	static const SDL_Color white={255,255,255};
	static const SDL_Color black={0,0,0};
	notificationLock.lock();

	int j=0;
	for (i=notifications.begin(); i!=notifications.end(); i++){
		//		cerr <<"EEEE: Notification draw: " << (*i)->gfx->name << endl;
		float x1= windowX0/3;
		float y1= windowY0;
		float x2= -windowX0/3;
		float y2= -windowY0;
		float voffset=(y2-y1)/4;
		float delta=voffset/6;


		(*i)->draw( x1,0+voffset+j*delta, x2,y2/15+voffset+j*delta );
		j++;

		if ( (*i)->gfx->alpha->ended()){
			notifications.erase(i);
			break;
		}



	}

	notificationLock.unlock();

}

void OpenGLWindow::drawSurface(){ //NOTE: must be called by internal thread

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


	massert(glGetError()==GL_NO_ERROR);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //EE: not use depth?

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
	massert(glGetError()==GL_NO_ERROR);


	if (/*showMenu*/ /*menuMode!=MENU_HIDDEN &&*/ !texturesLoaded){
		//cerr <<"EEEE: drawSurface: SHOW MENU: loading icons"<<endl;
		loadTexture("person");
		loadTexture("invite");
		loadTexture("hangup");
		loadTexture("settings");
		loadTexture("exit");
		loadTexture("yes");
		loadTexture("no");
		loadTexture("rightarrow");
		setupMenu();
		//cerr <<"EEEE: drawSurface: done loading textures"<<endl;
		massert(glGetError()==GL_NO_ERROR);
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
	//	glCullFace(GL_BACK);
	//	glEnable(GL_CULL_FACE);
	int dummy=1;
	int nvideos=displays.size();
	for (video=displays.begin(); video!=displays.end(); video++){
		//				cerr << "EEEE: getting video texture for display "<< dummy++ <<"/"<<nvideos <<endl;
		struct mgl_gfx* gfx = (*video)->getTexture(true);
		if (!(*video)->isHidden() && gfx->texture>0){
			massert(gfx->alpha);
			float alpha=gfx->alpha->getVal();
			float rot=0;
			if (gfx->rotate)
				rot=gfx->rotate->getVal();

			float x1= gfx->x1->getVal();
			float y1= gfx->y1->getVal();
			float x2= gfx->x2->getVal();
			float y2= gfx->y2->getVal();

			massert(glGetError()==GL_NO_ERROR);
			glPushMatrix();
			float xtransl = x1+(x2-x1)/2;
			//			cerr <<"EEEE: X translation="<<xtransl<<endl;
			glTranslatef(xtransl,0,0);
			glRotatef(rot,0,1,0);
			x1=x1-xtransl;
			x2=x2-xtransl;
			glColor4f(1.0,1.0,1.0, alpha );
			glBindTexture( GL_TEXTURE_2D, gfx->texture);
			//			cerr<<"EEEE: drawing texture "<<gfx->texture<<" with alpha "<<alpha<<endl;

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

			if (gfx->isSelected || (*video)->getIsLocalVideo() &&localVideoSelected){
				if (zoomMode==ZOOM_MODE_ON){
					float iwidth=(y2-y1)/12.0;
					mgl_gfx* arrow = getIcon("rightarrow");
					massert(arrow);

					glColor4f(1.0,1.0,1.0, 0.5 );
					glBindTexture( GL_TEXTURE_2D, arrow->texture);

					//cerr << "y1="<<y1<<" y2="<<y2<<endl;

					float ix1=x1;
					float iy1=y1+(y2-y1)/2.0-iwidth /* /2.0 */;
					float ix2=x1+iwidth;
					float iy2=iy1+iwidth*2;

					//LEFT
					glBegin( GL_QUADS );
					glTexCoord2f( 1.0, 0 );
					glVertex3f(  ix1, iy1, 0.0f );

					glTexCoord2f( 0.0, 0.0 );
					glVertex3f( ix2, iy1, 0.0f );

					glTexCoord2f( 0.0, 1.0 );
					glVertex3f( ix2, iy2, 0.0f );

					glTexCoord2f( 1.0, 1.0 );
					glVertex3f( ix1, iy2, 0.0f );
					glEnd();

					//RIGHT
					ix2=x2;
					ix1=x2-iwidth;
					iy1=y1+(y2-y1)/2.0-iwidth /* /2.0 */;
					iy2=iy1+iwidth*2;

					glBegin( GL_QUADS );
					glTexCoord2f( 0, 1.0 );
					glVertex3f(  ix1, iy1, 0.0f );

					glTexCoord2f( 1.0, 1.0 );
					glVertex3f( ix2, iy1, 0.0f );

					glTexCoord2f( 1.0, 0 );
					glVertex3f( ix2, iy2, 0.0f );

					glTexCoord2f( 0, 0 );
					glVertex3f( ix1, iy2, 0.0f );
					glEnd();

					//TOP

					iy2=y2;
					iy1=y2-iwidth;

					ix1=x1+(x2-x1)/2-iwidth /* /2.0 */;
					ix2=ix1+iwidth*2;

					glBegin( GL_QUADS );
					glTexCoord2f( 0.0, 0.0 );
					glVertex3f(  ix1, iy1, 0.0f );

					glTexCoord2f( 0.0, 1.0 );
					glVertex3f( ix2, iy1, 0.0f );

					glTexCoord2f( 1.0, 1.0 );
					glVertex3f( ix2, iy2, 0.0f );

					glTexCoord2f( 1.0, 0.0 );
					glVertex3f( ix1, iy2, 0.0f );
					glEnd();

					//BOTTOM
					iy1=y1;
					iy2=y1+iwidth;
					ix1=x1+(x2-x1)/2-iwidth /* /2.0 */;
					ix2=ix1+iwidth*2;

					glBegin( GL_QUADS );
					glTexCoord2f( 1.0, 1.0 );
					glVertex3f(  ix1, iy1, 0.0f );

					glTexCoord2f( 1.0, 0.0 );
					glVertex3f( ix2, iy1, 0.0f );

					glTexCoord2f( 0.0, 0.0 );
					glVertex3f( ix2, iy2, 0.0f );

					glTexCoord2f( 0.0, 1.0 );
					glVertex3f( ix1, iy2, 0.0f );
					glEnd();


				}else{
					float bwidth=(y2-y1)/20.0;
					glColor4f(0.0, 0.0, 0.7, 0.5); 
					glRectf( x1, y1, x2, y1+bwidth );             // _
					glRectf( x1, y2-bwidth, x2, y2 );             // - X
					glRectf( x1, y1+bwidth, x1+bwidth, y2-bwidth);//|
					glRectf( x2-bwidth, y1+bwidth, x2, y2-bwidth);//  |X
				}
			}
			if (/*gfx->callId*/ false){
				//cerr <<"EEEE: drawing callid <"<<gfx->callId<<">"<<endl;

				if (id_uri.count(gfx->callId) > 0){
					string uri=id_uri[gfx->callId];
					//cerr<<"EEEE: uri=<"<<uri<<">"<<endl;
					static const SDL_Color white={255,255,255};
					static const SDL_Color black={0,0,0};
					text->draw3D( x1,y2-(y2-y1)/20,0,  x2,y2,0, uri, 16, white, black, TEXT_ALIGN_LEFT);
				}
			}else{
				//cerr <<"EEEE: NOT drawing callid"<<endl;
			}

			glPopMatrix();
			massert(glGetError()==GL_NO_ERROR);

		}
	}
	displayListLock.unlock();

	switch(menuMode){
		case MENU_HIDDEN:
			break;
		default:
			menuDraw();

	};

	notificationsDraw();


	glPopMatrix();

	massert(glGetError()==GL_NO_ERROR);
	glFlush();
	massert(glGetError()==GL_NO_ERROR);
	SDL_GL_SwapBuffers();
	massert(glGetError()==GL_NO_ERROR);

	if (i%50==0)	//looking for dead videos is a bit heavy to do every round
		deadVideoProcessing();
}


void OpenGLWindow::sdlQuit(){ //NOTE: must be called by internal thread
	if (isFullscreen() ){
		toggleFullscreen();
	}

	SDL_Quit();
}



void OpenGLWindow::toggleFullscreen(){ //NOTE: must be called by internal thread
	printf("Toggle fullscreen\n");
	sdlFlags ^= SDL_FULLSCREEN;
	//        SDL_WM_ToggleFullScreen(gDrawSurface);

	initSurface();

	//	startFullscreen=isFullscreen();
	//        initSdl();
	updateVideoPositions(false);
	menuLock.lock();
	updateMenu(false);
	menuLock.unlock();

}


	bool OpenGLWindow::isFullscreen(){
		if (sdlFlags&SDL_FULLSCREEN)
			return true;
		else
			return false; 
	}



void OpenGLWindow::windowResized(int w, int h){ //NOTE: must be called by internal thread
	//cerr<<"EEEE: doing OpenGLWindow::windowResized("<<w<<","<<h<<")"<<endl;
	windowed_width=w;
	windowed_height=h;

	screen_aratio=(float)w/(float)h;

	initSurface();

	updateVideoPositions(false);
	menuLock.lock();
	updateMenu(false);
	menuLock.unlock();
	SDL_GL_SwapBuffers();
}

bool isRepeat(char key){
	bool ret=false;
#if 0
	static uint64_t lastKeyTime =0;
	static char lastKey=0;

	uint64_t now=mtime();
	uint64_t tdiff =  now-lastKeyTime;
	cerr << "tdiff" << tdiff<< endl;
	if ( tdiff < 160 && lastKey == key )
		ret=true;


	lastKeyTime=now;
	lastKey=key;
#endif
	return ret;
}

void OpenGLWindow::run(){ //NOTE: must be called by internal thread
	//cerr << "EEEE: OpenGLWindow::run start"<<endl;
#ifdef DEBUG_OUTPUT
	setThreadName("OpenGLWindow");
#endif

	if (!initialized)
		init();

	startWaitSem->inc();

	SDL_EnableKeyRepeat(200, 150);

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

					if (event.key.keysym.sym == SDLK_n){
						//cerr <<"EEEE: showing notification"<<endl;
						showNotification("hello",1000);
						//cerr <<"EEEE: done show notification"<<endl;
					}

					//trap quit and fullscreen events. Forward everything else

					if (event.key.keysym.sym == SDLK_ESCAPE ||
							event.key.keysym.sym == SDLK_q){
						sdlQuit();
						return;
					}

					if (event.key.keysym.sym == SDLK_RETURN &&
							event.key.keysym.mod & KMOD_ALT){
						toggleFullscreen();
					}else{
						char key = event.key.keysym.sym;
						bool repeat = isRepeat(key);
						if (event.key.keysym.mod == KMOD_SHIFT){        //if shift, make upper case
							printf("Shift detected\n");
							key-='a'-'A';
						}
						if (event.key.keysym.sym== SDLK_LEFT)
							keyPressed("BTN_LEFT",repeat);

						if (event.key.keysym.sym== SDLK_RIGHT)
							keyPressed("BTN_RIGHT",repeat);

						if (event.key.keysym.sym== SDLK_UP)
							keyPressed("KEY_CHANNELUP",repeat);

						if (event.key.keysym.sym== SDLK_DOWN)
							keyPressed("KEY_CHANNELDOWN",repeat);

						if (event.key.keysym.sym== SDLK_RETURN)
							keyPressed("KEY_PLAY",repeat);

						if (event.key.keysym.sym== SDLK_BACKSPACE)
							keyPressed("KEY_MENU",repeat);

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


	TTF_Quit();
	if (menuMode==MENU_HIDDEN){
		SDL_Quit();
	}
} 


void OpenGLWindow::start(){ //NOTE: this method is used from external thread
	cerr << "EEEE: doing OpenGLWindow::start()"<<endl;
	ir->start();
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

	//cerr << "EEEE: after OpenGLWindow::start() runCount="<<runCount<<endl;
}

void OpenGLWindow::stop(){ //NOTE: this method is used from external thread
	//cerr << "EEEE: doing OpenGLWindow::stop()"<<endl;
//	ir->stop();
	lock.lock();
	runCount--;
	if (runCount==0){
		doStop=true;
		cerr <<"EEEE: waiting for OpenGLWindow thread..."<<endl;
		thread->join();
		cerr <<"EEEE: done waiting for OpenGLWindow thread..."<<endl;
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

void OpenGLWindow::init(){ //NOTE: must be called by internal thread

	if (!initialized){
		initialized=true;
		initSdl();
	}


}

typedef GLvoid (*glXSwapIntervalSGIFunc) (GLint);
typedef GLvoid (*glXSwapIntervalMESAFunc) (GLint);

void OpenGLWindow::initSdl(){ //NOTE: must be called by internal thread
	//cerr <<"---------------------> EEEE: initSdl running by thread "<<hex<<Thread::getCurrent().asLongInt()<<dec<<endl;
	// init video system
	if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr,"Failed to initialize SDL Video!\n");
		exit(1);
	}

#if 1
	if(TTF_Init()){
		fprintf(stderr,"Failed to initialize SDL_TTF!\n");
		exit(1);
	}

	text = new Text("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
#endif


	// tell system which funciton to process when exit() call is made
	//	atexit(SDL_Quit);

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
	//cerr << "EEEE: OpenGL: native screen dimension is "<< native_width<<"x"<<native_height<<endl;

	//cerr  <<"EEEE: doing glGetError"<<endl;

	//	massert(glGetError()==GL_NO_ERROR);
	// set opengl attributes
	//cerr<<"EEEE: setting attributes..."<<endl;
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

	//cerr<<"EEEE: get swap attributes..."<<endl;
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,    1);

	//cerr<<"EEEE: setting environment..."<<endl;
	SDL_putenv((char*)"__GL_SYNC_TO_VBLANK=1");


#if 0
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

	//cerr<<"EEEE: will run initsurface"<<endl;
	initSurface( );
}

void OpenGLWindow::initSurface(){ //NOTE: must be called by internal thread
	printf("initSurface running\n");

	//cerr <<"EEEE: initSurface running by thread "<<hex<<Thread::getCurrent().asLongInt()<<dec<<endl;
	//	massert(glGetError()==GL_NO_ERROR);

	// get a framebuffer
	int w,h;
	//cerr <<"EEEE: sdlFlags="<<sdlFlags<<endl;
	//cerr <<"EEEE: SDL_FULLSCREEN="<<SDL_FULLSCREEN<<endl;
	if (sdlFlags & SDL_FULLSCREEN){
		w=native_width;
		h=native_height;
		//cerr << "EEEE: initializing to fullscreen dimensions "<< w <<"x"<<h<<endl;
	}else{
		w=windowed_width;
		h=windowed_height;
		//cerr << "EEEE: initializing to windowed dimensions "<< w <<"x"<<h<<endl;
	}
	cur_width=w;
	cur_height=h;

	//cerr << "EEEE: setting video mode to " << w << "x" << h << endl;
	gDrawSurface = SDL_SetVideoMode(w,h, bpp, sdlFlags);

	if( !gDrawSurface )
	{
		fprintf(stderr,"Couldn't set video mode!\n%s\n", SDL_GetError());
		exit(1);
	}

	massert(glGetError()==GL_NO_ERROR);
	GLint texSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
	printf("INFO: max texture size: %dx%d\n",texSize,texSize);
	t_max_size=texSize;




	massert(glGetError()==GL_NO_ERROR);
	glShadeModel(GL_SMOOTH);
	//glClearColor(0x04/255.0F,0x01/255.0F,0x16/255.0F,0);
	glClearColor(0.0F,0.0F,0.0F,0);
	// set opengl viewport and perspective view
	glViewport(0,0,cur_width,cur_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat) cur_width/(GLfloat) cur_height, 1.0, 500.0);
	massert(glGetError()==GL_NO_ERROR);

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


	massert(glGetError()==GL_NO_ERROR);

	glDisable(GL_DEPTH_TEST);




	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glPushMatrix();
	glTranslatef( 0, 0.0f, DRAW_Z);
	findScreenCoords();
	glPopMatrix();

	massert(glGetError()==GL_NO_ERROR);
}



//////////////////////////////////////////////////////////////////////////////////////



OpenGLDisplay::OpenGLDisplay( uint32_t width, uint32_t height, bool _fullscreen):VideoDisplay(){
	//cerr << "EEEE: OpenGLDisplay::OpenGLDisplay("<< width<<","<<height<<","<<_fullscreen<<") running"<<endl;
	hidden=false;
	hidden=false;
	this->width = width;
	this->height = height;
	fullscreen = _fullscreen;
	nallocated=0;
	//isLocalVideo=false;
	timeLastReceive=0;

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


void OpenGLDisplay::setPhoneConfig(MRef<SipSoftPhoneConfiguration*> conf){
	window->setPhoneConfig(conf);
}


//TODO: remove this method - it does the same thing as in the base class
void OpenGLDisplay::setCallback(MRef<CommandReceiver*> cb){
	callback=cb;
}

CommandString OpenGLDisplay::sendCmdResp(CommandString cmd){
	CommandString ret;
	if (callback)
		ret=callback->handleCommandResp("gui",cmd);
	return ret;
}

	void OpenGLDisplay::sendCmd(CommandString cmd){
		if (callback)
			callback->handleCommand("gui",cmd);
	}


struct mgl_gfx*  OpenGLDisplay::getTexture(bool textureUpdate){ //NOTE: if textureUpdate is true this method can only be called by internal thread

	dataLock.lock();
	//cerr <<"EEEE: getTexture running by thread "<<hex<<Thread::getCurrent().asLongInt()<<dec<< " textureUpdate=" << textureUpdate << endl;
	if (textureUpdate && gfx->texture==-1){
		massert(glGetError()==GL_NO_ERROR);
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
		massert(glGetError()==GL_NO_ERROR);
	}

	if (textureUpdate && newRgbData){
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

			massert(glGetError()==GL_NO_ERROR);
			GLenum pixFormat = (colorNBytes==3)?GL_RGB:GL_BGRA;

			glTexSubImage2D( GL_TEXTURE_2D, 0, 0,0 , width, height, pixFormat, GL_UNSIGNED_BYTE, rgb );
			massert(glGetError()==GL_NO_ERROR);
			gfx->wu=width/(float)gfx->tex_dim;
			gfx->hu=height/(float)gfx->tex_dim;
			gfx->aratio = (float)width/(float)height;
		}
	}
	dataLock.unlock();
	return gfx;
}

void OpenGLDisplay::handle( MImage * mimage){
	//	cerr <<"EEEE: doing OpenGLDisplay::handle on display "<<(uint64_t)this<<" image size="<<mimage->width<<"x"<<mimage->height<<" local="<<isLocalVideo<<endl;
	massert(mimage->linesize[1]==0);
	colorNBytes=mimage->linesize[0]/mimage->width;
	//	cerr <<"EEEE: colorNBytes="<<colorNBytes<<endl;
	dataLock.lock();
	if (!rgb || width!=mimage->width || height!=mimage->height){
		//cerr << "EEEE: allocating RGB of size "<<mimage->width<<"x"<<mimage->height<<endl;
		if (rgb)
			delete [] rgb;
		width=mimage->width;
		height=mimage->height;
		gfx->aratio=(float)width/(float)height;
		//cerr <<"EEEE: aratio for new size: "<< gfx->aratio<<endl;
		rgb = new uint8_t[width*height*colorNBytes+16]; // +16 to avoid mesa bug
		window->updateVideoLayout();

	}

	massert(rgb);

	//	saveTime(&times_rcv.displaycopystart);

	memcpy(rgb, &mimage->data[0][0], width*height*colorNBytes); //TODO: don't copy since it is in correct format.

	//	saveTime(&times_rcv.displaycopyend);

	newRgbData=true;
	if (!isLocalVideo)
		emptyImages.push_back(mimage);
	timeLastReceive=mtime();

	if (gfx->alpha && gfx->alpha->getType()!=ANIMATE_CONSTANT && isHidden()){
		gfx->alpha=new Animate(1.0);
		gfx->alpha->start();
		setHidden(false);
		window->updateVideoLayout();
	}
	dataLock.unlock();

	//printRcv();

}



void OpenGLDisplay::start(){
	//cerr <<"EEEE: doing OpenGLDisplay::start"<<endl;
	massert(window);
	window->start();
	window->addDisplay(this);

}

void OpenGLDisplay::stop(){
	//cerr <<"EEEE: doing OpenGLDisplay::stop"<<endl;
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
	//cerr <<"EEEE: doing OpenGLDisplay::openDisplay"<<endl;

}

void OpenGLDisplay::init( uint32_t width, uint32_t height ){
	//cerr <<"EEEE: doing OpenGLDisplay::init("<<width<<","<<height<<")"<<endl;
	this->width = width;
	this->height=height;
}

void OpenGLDisplay::createWindow(){
	//cerr <<"EEEE: doing OpenGLDisplay::createWindow"<<endl;
}

void OpenGLDisplay::resize(int w, int h){
	//cerr << "EEEE: doing OpenGLDisplay::resize("<<w<<","<<h<<") old size="<<width<<"x"<<height<<endl;
	this->width=w;
	this->height=h;
	newRgbData=false;
	delete [] rgb;
	rgb=NULL;
	gfx->aratio=(float)w/(float)h;
	//cerr <<"EEEE: new aratio="<< gfx->aratio<<endl;
	window->sizeHint(w,h);
	delete [] rgb;
	rgb=NULL;

	window->updateVideoLayout();
}

void OpenGLDisplay::destroyWindow(){
	//cerr << "EEEE: doing OpenGLDisplay::destroyWindow()"<<endl;
}

MImage * OpenGLDisplay::provideImage(){
	dataLock.lock();
	if (emptyImages.size()==0){
		cerr <<"EEEE: WARNING: allocating image"<<endl;
		emptyImages.push_back( allocateImage() );
	}
	MImage* ret = *emptyImages.begin();
	emptyImages.pop_front();
	dataLock.unlock();
	if (ret->width!=width||ret->height!=height){	//If there has been a resize, and the 
		//MImage is allocated with the old size, re-allocate
		cerr <<"EEEE: provideImage: resize from "<<ret->width<<"x"<<ret->height<<" to "<<width<<"x"<<height<<endl;
		deallocateImage(ret);
		ret=allocateImage();
	}
	return ret;
}

MImage * OpenGLDisplay::allocateImage(){
	//cerr << "EEEE: doing OpenGLDisplay::allocateImage of size "<< width<<"x"<<height<<endl;
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

	//cerr << "EEEE: done doing OpenGLDisplay::allocateImage of size "<< width<<"x"<<height<<endl;
	return mimage;
}

void OpenGLDisplay::deallocateImage( MImage * mimage ){
	//cerr << "EEEE: doing OpenGLDisplay::deallocateImage()"<<endl;
	free(mimage->data[0]);
	delete mimage;
}

bool OpenGLDisplay::handlesChroma( uint32_t chroma ){
	//cerr << "EEEE: doing OpenGLDisplay::handlesChroma "<<chroma<<endl;
	return chroma == M_CHROMA_RV24;
}

void OpenGLDisplay::displayImage( MImage * mimage ){
	//cerr <<"EEEE: doing OpenGLDisplay::displayImage"<<endl;

	massert(1==0); //This should not be called?!

}

void OpenGLDisplay::handleEvents(){
	//cerr <<"EEEE: doing OpenGLDisplay::handleEvents"<<endl;
}

void OpenGLDisplay::setIsLocalVideo(bool isLocal){
	isLocalVideo=isLocal;
	if (isLocal){
		//		gfx->callId=strdup("local");

	}
}

bool OpenGLDisplay::handleCommand(CommandString cmd){
	bool handled=false;
	//cerr <<"EEEE: OpenGLDisplay::handleCommand: got: "<< cmd.getString() << endl;

	if (cmd.getOp()=="make_proxy"){	//if we will not display any video
		window->removeDisplay(this);
	}

	if (cmd.getOp()=="wait_quit"){
		window->join();
	}

	if (cmd.getOp()=="remote_presence_update"){
		window->presenceUpdated();
	}

	if (cmd.getOp()=="set_texture_path"){
		window->setTexturePath(cmd.getParam());
		handled=true;
	}

	if (cmd.getOp()=="key"){
		window->keyPressed(cmd.getParam(), cmd.getParam2()=="REPEAT");
		handled=true;
	}

	if (cmd.getOp()=="remote_user_not_found" ){
		window->showNotification("User not found", 5000);
	}

	if (cmd.getOp()=="remote_ringing" ){
		window->showNotification("Ringing", 0);
	}

	if (cmd.getOp()=="invite_ok" ){
		window->clearNotifications();
	}



	if (cmd.getOp()=="incoming_available"){
		string fromuri = cmd.getParam();
		string callid  = cmd.getDestinationId();
		bool unprotected = cmd.getParam2()=="unprotected";
		window->incomingCall(callid, fromuri, unprotected);
		window->registerUri(callid, fromuri);
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

void OpenGLDisplay::setCallId(string id){
	massert(gfx);
	cerr << "*********************call id "<< id << endl;
	gfx->callId=strdup(id.c_str());
	cerr << "**************************** remote ip <"<< id.substr(id.find("@")+1) <<">"<<endl;
	window->setRemoteCamera(id.substr(id.find("@")+1));

}

OpenGLPlugin::OpenGLPlugin( MRef<Library *> lib ): VideoDisplayPlugin( lib ){
}

OpenGLPlugin::~OpenGLPlugin(){
}

