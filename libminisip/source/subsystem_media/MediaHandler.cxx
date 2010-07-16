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

/* Copyright (C) 2004, 2005, 2006 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include <signal.h>

#include"MediaHandler.h"



#include<string.h>
#include<libminisip/signaling/sdp/SdpPacket.h>
#include<libmikey/KeyAgreement.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libminisip/media/codecs/Codec.h>
#include"Session.h"
#include<libminisip/media/MediaStream.h>

#include<libminisip/media/zrtp/ZrtpHostBridgeMinisip.h>
#include<libminisip/media/Media.h>
#include<libminisip/media/ReliableMedia.h>
#include<libminisip/media/RtpReceiver.h>
#include<libminisip/media/MediaCommandString.h>
#include<libmnetutil/UDPSocket.h>

#include<libminisip/media/soundcard/SoundIO.h>
#include<libminisip/media/soundcard/SoundDevice.h>
#include<libminisip/media/codecs/Codec.h>

#include<libminisip/media/CallRecorder.h>
#include"SessionRegistry.h"

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif


using namespace std;


MediaHandler::MediaHandler( MRef<SipSoftPhoneConfiguration *> conf, MRef<IpProvider *> ipp, MRef<IpProvider *> ip6p ): ip6Provider(ip6p){

	this->ipProvider = ipp;
	this->config = conf;
	init();
}

MediaHandler::~MediaHandler(){
}

void MediaHandler::init(){

	media.clear();

	MRef<MediaRegistry*> registry = MediaRegistry::getInstance();
	MediaRegistry::const_iterator i;
	MediaRegistry::const_iterator last = registry->end();

	for( i = registry->begin(); i != last; i++ ){
		MRef<MPlugin *> plugin = *i;
		MRef<MediaPlugin *> mediaPlugin = dynamic_cast<MediaPlugin*>( *plugin );
		if( mediaPlugin ){
			MRef<Media *> m = mediaPlugin->createMedia( config );
			MRef<AudioMedia *> audio = dynamic_cast<AudioMedia *>( *m );
  
			if( m ){
				registerMedia( m );
			}

			if( !audioMedia && audio ){
				audioMedia = audio;
			}

		}
	}

	Session::registry = this;

//	muteAllButOne = config->muteAllButOne;
	
        ringtoneFile = config->ringtone;
}


MRef<Session *> MediaHandler::createSession( MRef<SipIdentity*> id, string callId ){


	list< MRef<Media *> >::iterator i;
	MRef<Session *> session;
	MRef<RtpReceiver *> rtpReceiver = NULL;
	MRef<RtpReceiver *> rtp6Receiver;
	string contactIp;
	string contactIp6;
#ifdef ZRTP_SUPPORT
	MRef<ZrtpHostBridgeMinisip *> zhb = NULL;
#endif


	if( ipProvider )
		contactIp = ipProvider->getExternalIp();

	if( ip6Provider )
		contactIp6 = ip6Provider->getExternalIp();

	session = new Session( contactIp, /*securityConfig*/ id, contactIp6 );
	session->setCallId( callId );



	for( i = media.begin(); i != media.end(); i++ ){


		MRef<Media *> m = *i;
		MRef<RealtimeMedia*> rtm = dynamic_cast<RealtimeMedia*>(*m);
		MRef<ReliableMedia*> relm = dynamic_cast<ReliableMedia*>(*m);



		if (relm){
			session->addReliableMediaSession( relm->createMediaStream(callId) );
		}

		if( rtm && rtm->receive ){
			if( ipProvider )
// edw pernei to port mesw callId mallon 
				rtpReceiver = new RtpReceiver( ipProvider, callId );

			if( ip6Provider )
				rtp6Receiver = new RtpReceiver( ip6Provider, callId );

			MRef<RealtimeMediaStreamReceiver *> rStream;
			rStream = new RealtimeMediaStreamReceiver( callId, rtm, rtpReceiver, rtp6Receiver );
			session->addRealtimeMediaStreamReceiver( rStream );

/* FIXME: The call recorder makes the audio output sound bad. Most likely,
 * it causes incoming audio to be put into the jitter buffer twice which
 * makes it overflow. Not sure why. FIXME.

			// Need to dereference MRef:s, Can't compare MRef:s
			// of different types
			if( *rtm == *audioMedia ) {
				CallRecorder * cr;
				cr = new CallRecorder( audioMedia, rtpReceiver, ipProvider );
				session->callRecorder = cr;
			}
*/

#ifdef ZRTP_SUPPORT
		    if(/*securityConfig.use_zrtp*/ id->use_zrtp) {
#ifdef DEBUG_OUTPUT
		        cerr << "MediaHandler::createSession: enabling ZRTP for receiver" << callId << endl;
#endif

			zhb = new ZrtpHostBridgeMinisip(callId, *messageRouterCallback);
			zhb->setReceiver(rStream);
			rStream->setZrtpHostBridge(zhb);
		    }
#endif
		}
		
		if( rtm && rtm->send ){
		    if( !rtpReceiver && !ipProvider.isNull() ){
			rtpReceiver = new RtpReceiver( ipProvider, callId );
		    }

		    if( !rtp6Receiver && !ip6Provider.isNull() ){
		      rtp6Receiver = new RtpReceiver( ip6Provider, callId );
		    }

		    MRef<UDPSocket *> sock;
		    MRef<UDPSocket *> sock6;

		    if( rtpReceiver )
			    sock = rtpReceiver->getSocket();
		    if( rtp6Receiver )
			    sock6 = rtp6Receiver->getSocket();

//////////////////////////////


		   if( rtm->getSdpMediaType().compare("video")  == 0 ) {
		

			    session->setUDPSocket ( sock );
			    session->setUDPSocket6( sock6);
		  }	


		    MRef<RealtimeMediaStreamSender *> sStream;
		    sStream = new RealtimeMediaStreamSender( callId, rtm, sock, sock6 );
		    session->addRealtimeMediaStreamSender( sStream );
#ifdef ZRTP_SUPPORT
		    if(/*securityConfig.use_zrtp*/ id->use_zrtp) {
#ifdef DEBUG_OUTPUT
		        cerr << "MediaHandler::createSession: enabling ZRTP for sender: " << callId << endl;
#endif
			if (!zhb) {
                            zhb = new ZrtpHostBridgeMinisip(callId, *messageRouterCallback);
			}
			zhb->setSender(sStream);
			sStream->setZrtpHostBridge(zhb);
		    }
#endif
		}
	}
	
	//set the audio settings for this session ...
	session->muteSenders( true );
	session->silenceSources( false );
	
///////////////////
	addSession(session);
	return session;

}


///////////////////////////////
void MediaHandler :: addSession ( MRef<Session *> session ){
	this->sessionList.push_back(session);
}

MRef<Session *> MediaHandler :: getSession ( string callId ){

	list< MRef <Session *> > :: iterator i;
	for ( i = this->sessionList.begin() ; i != this->sessionList.end(); i++){			
		cout << " getSession comparing callId !! wanted " << callId << " current " << (*i)->getCallId() <<"\n" ;
		if ( callId.compare((*i)->getCallId() ) == 0 ){
			cout << " Found matching session ....wanted " << callId << " current " << (*i)->getCallId() <<"\n" ;
			return (*i);
		}
	}
	return NULL;
}


void MediaHandler::registerMedia( MRef<Media*> m){
	this->media.push_back( m );
}

CommandString MediaHandler::handleCommandResp(string, const CommandString& c){
	assert(1==0); //Not used
	return c; // Not reached; masks warning
}

void MediaHandler::parse_screen_command(char * buf, char **args)
{
    while (*buf != NULL) {

        /* Strip whitespace.  Use nulls, so that the previous argument is terminated automatically. */
        while ((*buf == ' ') || (*buf == '\t'))
            *buf++ = NULL;

        /* Save the argument. */
        *args++ = buf;

        /* Skip over the argument. */
        while ((*buf != NULL) && (*buf != ' ') && (*buf != '\t'))
            buf++;
    }

    *args = NULL;
}


void MediaHandler::handleCommand(string subsystem, const CommandString& command ){

	assert(subsystem=="media");

	if( command.getOp() == MediaCommandString::start_ringing ){
// 		cerr << "MediaHandler::handleCmd - start ringing" << endl;
		if( audioMedia && ringtoneFile != "" ){
			audioMedia->startRinging( ringtoneFile );
		}
		return;
	}

	if( command.getOp() == MediaCommandString::stop_ringing ){
// 		cerr << "MediaHandler::handleCmd - stop ringing" << endl;
		if( audioMedia ){
			audioMedia->stopRinging();
		}
		return;
	}
	
	if( command.getOp() == MediaCommandString::session_debug ){
	#ifdef DEBUG_OUTPUT
		cerr << getDebugString() << endl;
	#endif
		return;
	}

	if( command.getOp() == MediaCommandString::audio_forwarding_enable){
		getMedia("audio")->setMediaForwarding(true);
		return;
	}

	if( command.getOp() == MediaCommandString::audio_forwarding_disable){
		getMedia("audio")->setMediaForwarding(false);
		return;
	}

	if( command.getOp() == MediaCommandString::video_forwarding_enable){
		getMedia("video")->setMediaForwarding(true);
		return;
	}

	if( command.getOp() == MediaCommandString::video_forwarding_disable){
		getMedia("video")->setMediaForwarding(false);
		return;
	}


	if( command.getOp() == MediaCommandString::send_dtmf){
		MRef<Session *> session = Session::registry->getSession( command.getDestinationId() );
		if( session ){
			string tmp = command.getParam();
			if (tmp.length()==1){
				uint8_t c = tmp[0];
				session->sendDtmf( c );
			}else{
				merr("media/dtmf") << "Error: DTMF format error. Ignored."<<endl;
			}
		}
		return;
	}


	
	if( command.getOp() == MediaCommandString::set_session_sound_settings ){
		bool turnOn;
	#ifdef DEBUG_OUTPUT
		cerr << "MediaHandler::handleCmd: received set session sound settings" 
				<< endl << "     " << command.getString()  << endl;
	#endif
		if( command.getParam2() == "ON" ) turnOn = true;
		else turnOn = false;
		setSessionSoundSettings( command.getDestinationId(), 
					command.getParam(), 
					turnOn );
		return;
	}

	if( command.getOp() == MediaCommandString::reload ){
		init();
		return;
	}
	
	if( command.getOp() == "call_recorder_start_stop" ){
	#ifdef DEBUG_OUTPUT
		cerr << "MediaHandler::handleCmd: call_recorder_start_stop" << endl 
			<< command.getString() << endl;
	#endif		
		bool start = (command.getParam() == "START" );
		sessionCallRecorderStart( command.getDestinationId(), start );
	}
	  MRef <Session * > sessionTmp;
	if( command.getOp() == MediaCommandString::start_camera ){
		string tmp = command.getParam();
		sessionTmp = getSession(tmp);
		int DestPort =  sessionTmp->getDestinationPort();
		string DestIp = sessionTmp ->getDestinationIp ()->getString();
//		cout << "\n\n\n.........................................\n\n\nFINALLY destination Ip and destination Port .... " << DestIp << ":"<<DestPort<<endl;

		MRef<MediaRegistry*> registry = MediaRegistry::getInstance();
        	MediaRegistry::const_iterator i;
        	MediaRegistry::const_iterator last = registry->end();


		MRef<RealtimeMedia*> videoMedia ;
		// this is bad i should create a VideoPlugin only or just a videoMedia
		for( i = registry->begin(); i != last; i++ ){
			MRef<MPlugin *> plugin = *i;
        		MRef<MediaPlugin *> videoPlugin = dynamic_cast<MediaPlugin*>( *plugin );
		        if( (videoPlugin->getName()).compare("video") == 0  ){
				cout << "............ENTER FOR VIDEO VideoPlugin getName ..........."<<videoPlugin->getName()<<endl;
				//config->videoDevice =  "/dev/video0";
				MRef<Media*> ms = videoPlugin ->createMedia2stream( config );
	//			cout << "...........testing for video........." << ms -> getSdpMediaType()<<endl;
        		        videoMedia =  dynamic_cast<RealtimeMedia*> (* ms);
	       		}
		}
				   
                  sStream = new RealtimeMediaStreamSender( tmp,videoMedia, sessionTmp->getUDPSocket ( ), sessionTmp->getUDPSocket6 ( ) );
                    
		  (*sStream)->setPort( sessionTmp->getDestinationPort() );
                  (*sStream)->setRemoteAddress( sessionTmp->getDestinationIp()  );

                  sessionTmp->addRealtimeMediaStreamSender( sStream );
		  sStream->setSelectedCodecHacked(videoMedia);
		  (*sStream)->start();
		 			

        }
        if( command.getOp() == MediaCommandString::stop_camera ){
		(*sStream)->stop();
		// sessionTmp ->removeRealtimeMediaStreamSender( sStream );
        }

	int screen_pid;
	static pid_t pid;

        if( command.getOp() == MediaCommandString::start_screen ) {
		string tmp = command.getParam();
                MRef <Session * > sessionTmp = getSession(tmp);
                int DestPort =  sessionTmp->getDestinationPort();
                string DestIp = sessionTmp ->getDestinationIp ()->getString();
               // cout << "FINALLY destination Ip and destination Port .... " << DestIp << ":"<<DestPort<<endl; 
		//cout << " display frame rate "<< config->displayFrameRate <<" display frame size " <<  config->displayFrameSize <<endl;
	

		/******************************* SCREEN STREAM WITH FFMPEG LIBRARY ******************************/
		int status;
		FILE *fpipe;
   		char *command="echo $DISPLAY";
		char display_variable[500];
		char *args[256];
		std::string destPort_str;
		std::stringstream destPort_stream;		
		char *ffmpeg_command_array;
		std::string ffmpeg_command;

		if ( !(fpipe = (FILE*)popen(command,"r")) ) {  
			// If fpipe is NULL
      			perror("Problems with pipe");
      			exit(1);
   		}

	   	while ( fgets( display_variable, sizeof display_variable, fpipe)) {
     			printf("%s", display_variable);
   		}
   		pclose(fpipe);
		
		char* p = strchr(display_variable,'\n');
		if (p) {
			*p = '\0';
		}		
			
		destPort_stream << DestPort;
		destPort_str = destPort_stream.str();

		ffmpeg_command = "ffmpeg -f x11grab -s " + config->displayFrameSize + " -r " + config->displayFrameRate + " -i " + display_variable + " -vcodec libx264 -vpre default -f rtp rtp://" + DestIp + ":" + destPort_str;
		cout << ffmpeg_command << endl;


		/*if (ffmpeg_command == NULL) {
            		printf("\n");
            		exit(0);
        	} */
		ffmpeg_command_array = new char[ffmpeg_command.size()+1];
		ffmpeg_command_array[ffmpeg_command.size()]=0;	
		memcpy(ffmpeg_command_array,ffmpeg_command.c_str(),ffmpeg_command.size());

        	/* Split the string into arguments. */
        	parse_screen_command(ffmpeg_command_array, args);

	        /**************** Execute the command. ***********/

	        /* Get a child process. */
	    	if ((pid = fork()) < 0) {
        		perror("fork");
        		exit(1);

		}

		/* The child executes the code inside the if.*/
    		if (pid == 0) {
        		//screen_pid = getpid();
	                //cout << " ********************  The process id of the screen stream is: " << screen_pid << " ************************" << endl;

			execvp(*args, args);
			
			//perror(*args);
        		//exit(1);
		}else if(pid > 0) {
			cout << "************************  I am the parent process streaming! ******************** My process id is: " << getpid() << " The process id of my child is: " << pid << endl;
			
			//screen_pid = getpid();
			//cout << " ******************** 2  The process id of the screen stream is: " << screen_pid << " ************************" << endl;
		}


		//execute_screen_command(args);	
		/************************************************************************************************/

        }
        if( command.getOp() == MediaCommandString::stop_screen ){

//		cout << "************************------- Stop screen: ******************** My process id is: " << getpid() << " The process id of my child is: " << pid << endl;
		kill( pid, SIGKILL );			
        }


}



std::string MediaHandler::getExtIP(){
	return ipProvider->getExternalIp();
}

void MediaHandler::setSessionSoundSettings( std::string callid, std::string side, bool turnOn ) {
        list<MRef<Session *> >::iterator iSession;

	//what to do with received audio
	if( side == "receivers" ) {
		sessionsLock.lock();
		for( iSession = sessions.begin(); iSession != sessions.end(); iSession++ ){
			if( (*iSession)->getCallId() == callid ){
				//the meaning of turnOn is the opposite of the Session:: functions ... silence/mute
				(*iSession)->silenceSources( ! turnOn );
			} 
		}
		sessionsLock.unlock();
	} else if ( side == "senders" ) { //what to do with audio to be sent over the net
		//set the sender ON as requested ... 
		sessionsLock.lock();
		for( iSession = sessions.begin(); iSession != sessions.end(); iSession++ ){
			if( (*iSession)->getCallId() == callid ){
				//the meaning of turnOn is the opposite of the Session:: functions ... silence/mute
				(*iSession)->muteSenders( !turnOn );
				
			} 
		}
		sessionsLock.unlock();
	} else {
		cerr << "MediaHandler::setSessionSoundSettings - not understood" << endl;
		return;
	}
	
}

void MediaHandler::sessionCallRecorderStart( string callid, bool start ) {
	CallRecorder * cr;
	list<MRef<Session *> >::iterator iSession;
	
	sessionsLock.lock();
	for( iSession = sessions.begin(); iSession != sessions.end(); iSession++ ){
		if( (*iSession)->getCallId() == callid ){
			cr = dynamic_cast<CallRecorder *>( *((*iSession)->callRecorder) );
			if( cr ) {
				cr->setAllowStart( start );
			}
		}
	}
	sessionsLock.unlock();
}

MRef<Media*> MediaHandler::getMedia(std::string sdpType){
	list<MRef<Media*> >::iterator i;
	for (i=media.begin(); i!=media.end(); i++){
		if ((*i)->getSdpMediaType()==sdpType){
			return *i;
		}
	}
	return NULL;
}


#ifdef DEBUG_OUTPUT	
string MediaHandler::getDebugString() {
	string ret;
	ret = getMemObjectType() + ": Debug Info\n";
	for( std::list<MRef<Session *> >::iterator it = sessions.begin();
				it != sessions.end(); it++ ) {
		ret += "** Session : \n" + (*it)->getDebugString() + "\n";
	}
	return ret;
}
#endif
