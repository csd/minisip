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

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<string.h>
#include"../sdp/SdpPacket.h"
#include<libmikey/keyagreement.h>
#include"../sip/SipDialogSecurityConfig.h"
#include"../sip/SipSoftPhoneConfiguration.h"
#include"../minisip/ipprovider/IpProvider.h"
#include"../codecs/Codec.h"
#include"Session.h"
#include"MediaStream.h"
#include"MediaHandler.h"
#include"Media.h"
#include"RtpReceiver.h"
#include"MediaCommandString.h"
#include<libmnetutil/UDPSocket.h>

#include"../soundcard/SoundIO.h"
#include"../soundcard/SoundDevice.h"
#include"../codecs/Codec.h"


#ifdef VIDEO_SUPPORT
#include"../video/grabber/Grabber.h"
#include"../video/display/VideoDisplay.h"
#include"../video/codec/VideoCodec.h"
#include"../video/codec/AVCoder.h"
#include"../video/codec/AVDecoder.h"
#include"../video/mixer/ImageMixer.h"
#endif


using namespace std;

MediaHandler::MediaHandler( MRef<SipSoftPhoneConfiguration *> config, MRef<IpProvider *> ipProvider ){

	this->ipProvider = ipProvider;
	this->config = config;
	init();
}

void MediaHandler::init(){

	media.clear();

#ifdef VIDEO_SUPPORT
	MRef<Grabber *> grabber = Grabber::create( config->videoDevice );
	MRef<VideoCodec *> videoCodec = new VideoCodec();
	MRef<ImageMixer *> mixer = NULL;//new ImageMixer();
	MRef<VideoMedia *> videoMedia = new VideoMedia( *videoCodec, NULL/*display*/, mixer, grabber, config->frameWidth, config->frameHeight );
	if( mixer ){
		mixer->setMedia( videoMedia );
	}
	registerMedia( *videoMedia );
#endif

	string soundDev = config->soundDevice;
	if( soundDev != "" ){

		MRef<SoundDevice *> sounddev = SoundDevice::create( soundDev );
		MRef<SoundIO *> soundIo = new SoundIO( sounddev, config->soundIOmixerType, 2, 48000 );

		std::list<MRef<Codec *> > codecList;
		std::list<std::string>::iterator iCodec;
		MRef<Codec *> selectedCodec;

		for( iCodec = config->audioCodecs.begin(); 
					iCodec != config->audioCodecs.end();
					iCodec ++ ){

			selectedCodec = (Codec*)*AudioCodec::create( *iCodec );
			if( selectedCodec ){
#ifdef DEBUG_OUTPUT
				cerr << "Adding audio codec: " << selectedCodec->getCodecName() << endl;
#endif
				codecList.push_back( selectedCodec );
			}
		}
		
		MRef<AudioMedia *> media = new AudioMedia( soundIo, codecList );
		
		registerMedia( *media );
		audioMedia = media;
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
	MRef<RtpReceiver *> rtpReceiver;
	string contactIp;

	contactIp = ipProvider->getExternalIp();

	session = new Session( contactIp, securityConfig );
	session->setCallId( callId );

	for( i = media.begin(); i != media.end(); i++ ){
		if( (*i)->receive ){
			rtpReceiver = new RtpReceiver( ipProvider );
			rStream = new MediaStreamReceiver( *i, rtpReceiver, ipProvider );
			session->addMediaStreamReceiver( rStream );
		}
		
		if( (*i)->send ){
			sStream = new MediaStreamSender( *i, rtpReceiver->getSocket() );
			//sStream->setMuted( muteAllButOne ); //if muteAll, then mute the stream by default
			session->addMediaStreamSender( sStream );
		}
	}
	
	//set the audio settings for this session ...
	session->muteSenders( true ); //muteAllButOne );
	session->silenceSources( false );
	
	return session;

}


void MediaHandler::registerMedia( MRef<Media*> media ){
	this->media.push_back( media );
}

void MediaHandler::handleCommand( CommandString command ){
	if( command.getOp() == MediaCommandString::start_ringing ){
		cerr << "MediaHandler::handleCmd - start ringing" << endl;
		if( audioMedia && ringtoneFile != "" ){
			audioMedia->startRinging( ringtoneFile );
		}
		return;
	}

	if( command.getOp() == MediaCommandString::stop_ringing ){
		cerr << "MediaHandler::handleCmd - stop ringing" << endl;
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
		//set the sender ON as requested ... only mute all the rest if muteAllButOne is true
		sessionsLock.lock();
		for( iSession = sessions.begin(); iSession != sessions.end(); iSession++ ){
			if( (*iSession)->getCallId() == callid ){
				//the meaning of turnOn is the opposite of the Session:: functions ... silence/mute
				(*iSession)->muteSenders( !turnOn );
				
			} else {
				//if the feature is deactivated, ignore the set_active_source command
/*				if( muteAllButOne && !turnOn) {
					(*iSession)->muteSenders( true );
				}*/
			}
		}
		sessionsLock.unlock();
	
	} else {
		cerr << "MediaHandler::setSessionSoundSettings - not understood" << endl;
		return;
	}
	
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
