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

#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include"OpenGlGui.h"
#include<libminisip/media/MediaCommandString.h>
#include<libmutil/MemObject.h>
#include<libmutil/stringutils.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include<libminisip/media/video/display/VideoDisplay.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>


using namespace std;

#if 0

class IrInput : public Runnable{
	public:
		IrInput(Gui* handler);
		void start();
		void stop();
		void run();
		

	private:
		bool doStop;
		MRef<Semaphore*> quitSignal;
		Thread *thread;
		Gui* handler;



};

IrInput::IrInput(Gui* h){
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
	cerr <<"EEEE: doing IrInput::run"<<endl;

	int fd=-1;

	do{
		if (fd==-1)	
			fd=iropen();
		if (fd>0){
//			cerr <<"EEEE: opened IR connection"<<endl;
		}



		
		if (fd!=-1){
			int n;
			char buf[129];
//			cerr <<"EEEE: waiting for IR data..."<<endl;
			n=read(fd,buf,128);
//			cerr <<"EEEE: got IR data"<<endl;
			if(n==-1 || n==0)  {
				fd=-1;
			};
			buf[n]=0;
			string s(buf);
//			cerr <<"------------> DATA <"<<s<<">"<<endl;
			bool repeat=false;
			string key=parseBuf(s, repeat);
			if (key.size()>0)
				handler->handleCommand("gui",CommandString("","key",key, repeat?"REPEAT":"FIRST"));

		}

		if (!doStop&&fd==-1){	//Sleep 10s and then re-try to open
			for (int i=0; i<20 && !doStop; i++)
				Thread::msleep(500);
		}


	}while(!doStop);

	quitSignal->inc();
}

#endif

OpenGlGui::OpenGlGui(bool fullscreen) {
	startFullscreen=fullscreen;
	thread=NULL;
//	inCall=false;
	quitSem = new Semaphore();
//	IrInput* ir = new IrInput(this);
//	ir->start();
}

void OpenGlGui::start(){
	thread = new Thread(this);
}

void OpenGlGui::join(){
	display->handleCommand( CommandString("", "wait_quit")  );
}

void OpenGlGui::run(){
	cerr <<"EEEE: OpenGlGui::run started"<<endl;
	VideoDisplayRegistry::getInstance();

	display = VideoDisplayRegistry::getInstance()->createDisplay( 960, 640, true, startFullscreen);
	massert(display);
	display->setCallback(this);
	display->handleCommand(CommandString("","set_texture_path","/tmp/") );
	display->handleCommand(CommandString("","make_proxy") );
	display->handleCommand(CommandString("","enable_menu") );
//	display->start();
//	Thread::msleep(50000);




	//Un-block any thread waiting for us to quit
	quitSem->inc();

	cerr <<"EEEE: ......................... run quitting"<<endl;

}

void OpenGlGui::waitQuit(){
	quitSem->dec();
}

void OpenGlGui::displayErrorMessage(string msg){
//	displayMessage(msg, red);
}

CommandString OpenGlGui::handleCommandResp(string subsystem, const CommandString&cmd){
	if (cmd.getOp()=="invite"){
		return sendCommandResp("sip", cmd);
	}
	massert(false);
}

void OpenGlGui::handleCommand(const CommandString &cmd){
	cerr << "OpenGlGui::handleCommand: Got "<<cmd.getString() << endl;

	if (cmd.getOp()=="remote_user_not_found"){
		display->handleCommand(cmd);
	}

	if (cmd.getOp()=="key"){
		display->handleCommand(cmd);
	}

	if (cmd.getOp()=="remote_ringing"){	
		display->handleCommand(cmd);
	}

	if (cmd.getOp()=="remote_presence_update"){
		display->handleCommand(cmd);
	}

	if (cmd.getOp()=="incoming_available"){
		sendCommand( "media",CommandString("","start_ringing") );
		display->handleCommand(cmd);
	}
	
	if (cmd.getOp()=="invite"){
		CommandString resp=sendCommandResp("sip", cmd);
	}

	if (cmd.getOp()=="invite_ok"){
		CommandString cmdstr( cmd.getDestinationId(),
				MediaCommandString::set_session_sound_settings,
				"senders", "ON");
		sendCommand("media", cmdstr);
		display->handleCommand(cmd);
	}


	if (cmd.getOp()=="accept_invite" || cmd.getOp()=="reject_invite"){
		sendCommand( "media",CommandString("","stop_ringing") );
		sendCommand("sip", cmd);
		
               CommandString cmdstr( cmd.getDestinationId(),
                                MediaCommandString::set_session_sound_settings,
                                "senders", "ON");
                sendCommand("media",cmdstr);


	}

	if (cmd.getOp()=="hang_up"){
		cerr <<"EEEE: got hangup from opengl for callid "<< cmd.getDestinationId()<<endl;
		sendCommand("sip", cmd);
	}


}

bool OpenGlGui::configDialog( MRef<SipSoftPhoneConfiguration *> /*conf*/ ){
	cout << "ERROR: OpenGlGui::configDialog is not implemented"<< endl;
	return false;
}


void OpenGlGui::setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *>sipphoneconfig){
	config = sipphoneconfig;       
	display->setPhoneConfig(config);
}

void OpenGlGui::setCallback(MRef<CommandReceiver*> callback){
	Gui::setCallback(callback);
	MRef<Semaphore *> localSem = semSipReady;
	if( localSem ){
		localSem->inc();
	}
}

