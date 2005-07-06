/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<string.h>
#include<libminisip/SdpPacket.h>
#include<libmikey/keyagreement.h>
#include<libminisip/SipDialogSecurityConfig.h>
#include<libminisip/SipSoftPhoneConfiguration.h>
#include<libminisip/IpProvider.h>
#include<libminisip/Codec.h>
#include<libminisip/Session.h>
#include<libminisip/MediaStream.h>
#include<libminisip/MediaHandler.h>
#include<libminisip/Media.h>
#include<libminisip/RtpReceiver.h>
#include<libminisip/MediaCommandString.h>
#include<libmnetutil/UDPSocket.h>

#include<libminisip/SoundIO.h>
#include<libminisip/SoundDevice.h>
#include<libminisip/Codec.h>


#ifdef VIDEO_SUPPORT
#include<libminisip/Grabber.h>
#include<libminisip/VideoDisplay.h>
#include<libminisip/VideoCodec.h>
#include<libminisip/AVCoder.h>
#include<libminisip/AVDecoder.h>
#include<libminisip/ImageMixer.h>
#endif


using namespace std;

MediaHandler::MediaHandler( MRef<SipSoftPhoneConfiguration *> config, MRef<IpProvider *> ipProvider ){

	this->ipProvider = ipProvider;

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
                MRef<SoundIO *> soundIo = new SoundIO( sounddev, 2, 48000 );
						
		std::list<MRef<Codec *> > codecList;
		std::list<std::string>::iterator iCodec;
		MRef<Codec *> selectedCodec;

                for( iCodec = config->audioCodecs.begin(); 
                     iCodec != config->audioCodecs.end();
                     iCodec ++ ){

                        selectedCodec = (Codec*)*AudioCodec::create( *iCodec );
                        if( selectedCodec ){
                                codecList.push_back( selectedCodec );
                        }

                }
		
		MRef<AudioMedia *> media = new AudioMedia( soundIo, codecList );
		
                registerMedia( *media );
		if( !audioMedia ){
			audioMedia = media;
		}
        }

	ringtoneFile = config->ringtone;
}

MRef<Session *> MediaHandler::createSession( SipDialogSecurityConfig &securityConfig, string callId ){

	list< MRef<Media *> >::iterator i;
	MRef<Session *> session;
	MRef<MediaStream *> stream;
	MRef<RtpReceiver *> rtpReceiver;
	string contactIp;

	contactIp = ipProvider->getExternalIp();

	session = new Session( contactIp, securityConfig );
        session->setCallId( callId );

	for( i = media.begin(); i != media.end(); i++ ){
		if( (*i)->receive ){
			rtpReceiver = new RtpReceiver( ipProvider );
			stream = new MediaStreamReceiver( *i, rtpReceiver, ipProvider );
			session->addMediaStreamReceiver( stream );
		}
		
		if( (*i)->send ){
			stream = new MediaStreamSender( *i, rtpReceiver->getSocket() );
			session->addMediaStreamSender( stream );
		}
	}

	return session;

}


void MediaHandler::registerMedia( MRef<Media*> media ){
	this->media.push_back( media );
}

void MediaHandler::handleCommand( CommandString command ){
	if( command.getOp() == MediaCommandString::start_ringing ){
		if( audioMedia && ringtoneFile != "" ){
			audioMedia->startRinging( ringtoneFile );
		}
		return;
	}

	if( command.getOp() == MediaCommandString::stop_ringing ){
		if( audioMedia ){
			audioMedia->stopRinging();
		}
		return;
	}
}

std::string MediaHandler::getExtIP(){
	return ipProvider->getExternalIp();
}
