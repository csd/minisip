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
#include"../codecs/G711CODEC.h"
#include"../codecs/ILBCCODEC.h"

#ifdef HAS_SPEEX
#include"../codecs/SPEEXCODEC.h"
#endif

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
		// pn430 Following two lines (incl. comment) changed for multicodec
                // FIXME: go through the codecs and add all
                //MRef<AudioMedia *> media = new AudioMedia( soundIo, new G711CODEC() );
		// pn New version
						
		MRef<Codec *> codecG711 = new G711CODEC();
		MRef<Codec *> codeciLBC = new ILBCCODEC();
#ifdef HAS_SPEEX
		MRef<Codec *> codecSPEEX = new SPEEXCODEC();
#endif

		std::list<MRef<Codec *> > codecList;

				
		MRef<Codec *> selectedCodec;
		
		int n = 0; 
		
		if ( config->selectedCodec == "G.711" )
			n = 0;
		if ( config->selectedCodec == "iLBC" )
			n = 97;
		if ( config->selectedCodec == "speex" )
			n = 114;
			
		switch ( n ) {		
#ifdef HAS_SPEEX		 	
			case 114:
				selectedCodec=codecSPEEX;
				codecList.push_back(selectedCodec);
				codecList.push_back(codecG711);
				codecList.push_back(codeciLBC);
				break;
#endif
			case 97:
				selectedCodec=codeciLBC;
				codecList.push_back(selectedCodec);
				codecList.push_back(codecG711);
#ifdef HAS_SPEEX
				codecList.push_back(codecSPEEX);
#endif
				break;	
			default:
				selectedCodec=codecG711;
				codecList.push_back(selectedCodec);
				codecList.push_back(codeciLBC);
#ifdef HAS_SPEEX
				codecList.push_back(codecSPEEX);
#endif
		}	
	
		MRef<AudioMedia *> media = new AudioMedia( soundIo, codecList, selectedCodec);
		
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
