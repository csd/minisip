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

#include<libminisip/mediahandler/MediaHandler.h>

#include<string.h>
#include<libminisip/sdp/SdpPacket.h>
#include<libmikey/keyagreement.h>
#include<libminisip/sip/SipDialogSecurityConfig.h>
#include<libminisip/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libminisip/codecs/Codec.h>
#include<libminisip/mediahandler/Session.h>
#include<libminisip/mediahandler/MediaStream.h>

#include<libminisip/mediahandler/Media.h>
#include<libminisip/mediahandler/RtpReceiver.h>
#include<libminisip/mediahandler/MediaCommandString.h>
#include<libmnetutil/UDPSocket.h>

#include<libminisip/soundcard/SoundIO.h>
#include<libminisip/soundcard/SoundDevice.h>
#include<libminisip/codecs/Codec.h>

#include<libminisip/mediahandler/CallRecorder.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif



using namespace std;

MediaHandler::MediaHandler( MRef<SipSoftPhoneConfiguration *> config, MRef<IpProvider *> ipProvider ){

	this->ipProvider = ipProvider;
	this->config = config;
	init();
}

MediaHandler::~MediaHandler(){
// 	cerr << "~MediaHandler" << end;
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
			MRef<Media *> media = mediaPlugin->createMedia( config );
			MRef<AudioMedia *> audio = dynamic_cast<AudioMedia *>( *media );

			if( media ){
				registerMedia( media );
			}

			if( !audioMedia && audio ){
				audioMedia = audio;
			}
		}
	}

//	muteAllButOne = config->muteAllButOne;
	
	ringtoneFile = config->ringtone;
}

// MediaHandler::~MediaHandler() {
// 	cerr << "~MediaHandler" << endl;
// 	if( ! Session::registry ){
// 		cerr << "deleting session::registry" << endl;
// 	}
// }


MRef<Session *> MediaHandler::createSession( SipDialogSecurityConfig &securityConfig, string callId ){

	list< MRef<Media *> >::iterator i;
	MRef<Session *> session;
	MRef<MediaStreamReceiver *> rStream;
	MRef<MediaStreamSender *> sStream;
	MRef<RtpReceiver *> rtpReceiver = NULL;
	string contactIp;
#ifdef ZRTP_SUPPORT
	MRef<ZrtpHostBridgeMinisip *> zhb = NULL;
#endif

	contactIp = ipProvider->getExternalIp();

	session = new Session( contactIp, securityConfig );
	session->setCallId( callId );

	for( i = media.begin(); i != media.end(); i++ ){
		MRef<Media *> media = *i;

		if( media->receive ){
			rtpReceiver = new RtpReceiver( ipProvider );
			rStream = new MediaStreamReceiver( media, rtpReceiver, ipProvider );
			session->addMediaStreamReceiver( rStream );
			if( (*i) == this->audioMedia ) {
				CallRecorder * cr;
				cr = new CallRecorder( audioMedia, rtpReceiver, ipProvider );
				session->callRecorder = cr;
			}
#ifdef ZRTP_SUPPORT
		    if(securityConfig.use_zrtp) {
			zhb = new ZrtpHostBridgeMinisip();
			zhb->setReceiver(rStream);
			rStream->setZrtpHostBridge(zhb);
		    }
#endif
		}
		
		if( media->send ){
		    if( !rtpReceiver ){
			rtpReceiver = new RtpReceiver( ipProvider );
		    }
		    sStream = new MediaStreamSender( media, rtpReceiver->getSocket() );
		    session->addMediaStreamSender( sStream );
#ifdef ZRTP_SUPPORT
		    if(securityConfig.use_zrtp) {
			if (!zhb) {
			    zhb = new ZrtpHostBridgeMinisip();
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
	
	return session;

}


void MediaHandler::registerMedia( MRef<Media*> media ){
	this->media.push_back( media );
}

CommandString MediaHandler::handleCommandResp(string, const CommandString& c){
	assert(1==0); //Not used
	return c; // Not reached; masks warning
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
