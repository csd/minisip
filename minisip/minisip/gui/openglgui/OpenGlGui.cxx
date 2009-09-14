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


OpenGlGui::OpenGlGui(bool fullscreen) {
	startFullscreen=fullscreen;
	thread=NULL;
//	inCall=false;
	quitSem = new Semaphore();
	IrInput* ir = new IrInput(this);
	ir->start();
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

	if (cmd.getOp()=="key"){
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

#if 0
	bool handled = false;

	if (cmd.getOp()==SipCommandString::ask_password){
		MRef<QuestionDialog*> d= new QuestionDialog;
		d->questionId = cmd.getDestinationId();
		d->questionId2 = cmd.get("realm");
		d->questions.push_back("Enter USER NAME for realm <"+d->questionId2+">");
		d->questions.push_back("Enter PASSWORD for realm <"+d->questionId2+">");
		showQuestionDialog(d);
	}

	if (cmd.getOp()=="register_ok"){
		handled=true;
		displayMessage("Register to proxy "+cmd.getParam()+" OK", green);
	}

	if (cmd.getOp()=="call_terminated"){
		handled=true;
		//we ignore this command
	}

	if (cmd.getOp()=="file_transfer_done"){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("File transfer done.",red);
		callId=""; //FIXME: should check the callId of cmd.
		inCall=false;
	}

	if (cmd.getOp()=="incoming_filetransfer_accept"){
		handled=true;
		if(state=="IDLE"){
			state="RECEIVE?";
			setPrompt(state);
			callId=cmd.getDestinationId();
			displayMessage("The file transfer from "+cmd.getParam(), blue);
		}
	}



	if (cmd.getOp()=="register_sent"){
		handled=true; // we don't need to inform the user
	}

		
	if (cmd.getOp()==SipCommandString::incoming_im){
		handled=true;
		displayMessage("IM to <"+cmd.getParam3()+"> from <"+cmd.getParam2()+">: "+cmd.getParam(), bold);
	}
	
	
	if (cmd.getOp()=="invite_ok"){
		handled=true;
		state="INCALL";
		inCall = true;
		setPrompt(state);
		displayMessage("PROGRESS: remote participant accepted the call...", blue);
		
		displayMessage("PROGRESS: Unmuting sending of sound.", blue);
		CommandString cmdstr( callId,
				MediaCommandString::set_session_sound_settings,
				"senders", "ON");
		sendCommand("media", cmdstr);
	
	}
	else if (cmd.getOp()=="security_failed"){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("Security is not handled by the receiver", red);
		inCall=false;
	}
	
	if (cmd.getOp()=="remote_ringing"){
		handled=true;
		state="REMOTE RINGING";
		setPrompt(state);
		displayMessage("PROGRESS: the remote UA is ringing...", blue);
	}
	
	if (cmd.getOp()==SipCommandString::remote_user_not_found && !p2tmode){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("User "+cmd.getParam()+" not found.",red);
		callId=""; //FIXME: should check the callId of cmd.
	}
	
	if (cmd.getOp()==SipCommandString::remote_hang_up){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("Remote user ended the call.",red);
		callId=""; //FIXME: should check the callId of cmd.
		inCall=false;
	}
	else if (cmd.getOp()==SipCommandString::remote_cancelled_invite){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("Remote user cancelled the call.",red);
		callId=""; //FIXME: should check the callId of cmd.
		inCall=false;
	}
	
	
	if (cmd.getOp()==SipCommandString::transport_error && !p2tmode){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("The call could not be completed because of a network error.", red);
		callId=""; //FIXME: should check the callId of cmd.
	}
	
	
	if (cmd.getOp()=="error_message"){
		handled=true;
		displayMessage("ERROR: "+cmd.getParam(), red);
	}
	
	if (cmd.getOp()=="remote_reject" && !p2tmode){
		handled=true;
		state="IDLE";
		setPrompt(state);
		callId="";
		displayMessage("The remote user rejected the call.", red);
	}
	
	if (cmd.getOp()==SipCommandString::incoming_available){
		handled=true;
		if(state=="IDLE"){
			state="ANSWER?";
			setPrompt(state);
			callId=cmd.getDestinationId();
			displayMessage("The incoming call from "+cmd.getParam(), blue);
		}
		else{
			displayMessage("You missed call from "+cmd.getParam(), red);
			CommandString hup(cmd.getDestinationId(), SipCommandString::reject_invite);
			sendCommand("sip",hup);
		
		}
		
	}
	if (cmd.getOp()=="conf_join_received"){
		handled=true;
		if(state=="IDLE"){
			state="ANSWER?";
			setPrompt(state);
			callId=cmd.getDestinationId();
			//addCommand("addc");
			
			
			currentcaller=cmd.getParam();
			
					//conf->setGui(this);
			string confid="";
			string users=cmd.getParam3();
			int i=0;	
			while (users[i]!=';'&&users.length()!=0 &&!(i>((int)users.length()-1))){
				confid=confid+users[i];
				i++;
			}
			string mysipuri = config->defaultIdentity->getSipUri().getUserIpString();
			users=trim(users.substr(i));
			currentconf=new ConferenceControl(mysipuri,confid,false);
			confCallback->setConferenceController(currentconf);
			displayMessage("The incoming conference call from "+cmd.getParam(), blue);
			displayMessage("The participants are "+cmd.getParam()+" "+users, blue);
			currentconfname=confid;
		}
		else{
			displayMessage("You missed call from "+cmd.getParam(), red);
			CommandString hup(cmd.getDestinationId(), SipCommandString::reject_invite);
			confCallback->guicb_handleCommand(hup);
			
		
		}
		
	}
	
	if (cmd.getOp()==SipCommandString::transfer_pending){
		handled=true;
		if(inCall){
			displayMessage( "Call transfer in progress..." );
		}
		
	}
	
	if (cmd.getOp()==SipCommandString::transfer_requested){
		handled=true;
		cerr << "TestUI got transfer_requested" << endl;
		if(inCall){
			state="TRANSFER?";
			setPrompt(state);
			displayMessage("Accept call transfer to "+cmd.getParam(), blue );
		}
		
	}
	
	if (cmd.getOp()==SipCommandString::call_transferred){
		handled=true;
		callId = cmd.getParam();
		state="INCALL";
		displayMessage("Call transferred ...");
		setPrompt(state);
	}

	if (autoanswer && state=="ANSWER?"){
		handled=true;
		CommandString command(callId, SipCommandString::accept_invite);
		sendCommand("sip",command);
		state="INCALL";
		setPrompt(state);
		displayMessage("Autoanswered call from "+ cmd.getParam());
		displayMessage("Unmuting sending of sound.", blue);
		CommandString cmdstr( callId,
				MediaCommandString::set_session_sound_settings,
				"senders", "ON");
		sendCommand("media",cmdstr);
	}

	if (!handled){
		displayMessage("WARNING: Gui did not handle command: "+ cmd.getString(), red );
	}
#endif

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

