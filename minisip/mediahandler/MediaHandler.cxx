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
#include"../sdp/SdpPacket.h"
#include<libmikey/keyagreement.h>
#include"../sip/SipDialogSecurityConfig.h"
#include"../sip/SipSoftPhoneConfiguration.h"
#include"../minisip/ipprovider/IpProvider.h"
#include"../codecs/Codec.h"
#include"MediaStream.h"
#include"MediaHandler.h"
#include"Media.h"
#include"Session.h"
#include"RtpReceiver.h"
#include"MediaCommandString.h"
#include<libmnetutil/UDPSocket.h>

#include"../soundcard/SoundIO.h"
#include"../soundcard/SoundDevice.h"
#include"../codecs/Codec.h"
#include"../codecs/G711CODEC.h"

#ifdef VIDEO_SUPPORT
#include"../video/grabber/Grabber.h"
#include"../video/display/VideoDisplay.h"
#include"../video/codec/VideoCodec.h"
#include"../video/codec/AVCoder.h"
#include"../video/codec/AVDecoder.h"
#include"../video/mixer/ImageMixer.h"
#ifdef IPAQ
#include"../video/display/SdlDisplay.h"
#else
#include"../video/display/XvDisplay.h"
#endif
#endif

using namespace std;

MediaHandler::MediaHandler( MRef<SipSoftPhoneConfiguration *> config, MRef<IpProvider *> ipProvider ){

	this->ipProvider = ipProvider;

#ifdef VIDEO_SUPPORT
	MRef<Grabber *> grabber = Grabber::create( config->videoDevice );
	MRef<VideoCodec *> videoCodec = new VideoCodec();
	MRef<ImageMixer *> mixer = new ImageMixer();
	//FIXME
#ifdef IPAQ
	MRef<VideoDisplay *> display = new SdlDisplay( config->frameWidth, config->frameHeight);
#else
	MRef<VideoDisplay *> display = new XvDisplay( config->frameWidth, config->frameHeight);
#endif

	MRef<VideoMedia *> videoMedia = new VideoMedia( videoCodec, display, mixer, grabber, config->frameWidth, config->frameHeight );
	mixer->setMedia( videoMedia );
	registerMedia( *videoMedia );
#endif

	string soundDev = config->soundDevice;
        if( soundDev != "" ){

                MRef<SoundDevice *> sounddev = SoundDevice::create( soundDev );
                MRef<SoundIO *> soundIo = new SoundIO( sounddev, 2, 48000 );
                // FIXME: go through the codecs and add all
                MRef<AudioMedia *> media = new AudioMedia( soundIo, new G711CODEC() );
                registerMedia( *media );
		if( !audioMedia ){
			audioMedia = media;
		}
        }

	ringtoneFile = config->ringtone;
}

MRef<Session *> MediaHandler::createSession( SipDialogSecurityConfig &securityConfig ){

	list< MRef<Media *> >::iterator i;
	MRef<Session *> session;
	MRef<MediaStream *> stream;
	MRef<RtpReceiver *> rtpReceiver;
	string contactIp;

	contactIp = ipProvider->getExternalIp();

	session = new Session( contactIp, securityConfig );

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
