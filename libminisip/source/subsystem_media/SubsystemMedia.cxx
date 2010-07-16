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

#include<libminisip/media/SubsystemMedia.h>
#include"MediaHandler.h"

#include<string.h>
#if 0
#include<libminisip/signaling/sdp/SdpPacket.h>
#include<libmikey/KeyAgreement.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/Session.h>
#include<libminisip/media/MediaStream.h>

#include<libminisip/media/Media.h>
#include<libminisip/media/RtpReceiver.h>
#include<libminisip/media/MediaCommandString.h>
#include<libmnetutil/UDPSocket.h>

#include<libminisip/media/soundcard/SoundIO.h>
#include<libminisip/media/soundcard/SoundDevice.h>
#include<libminisip/media/codecs/Codec.h>

#include<libminisip/media/CallRecorder.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#endif

using namespace std;

#define MH (*(MRef<MediaHandler*> *) mediaHandler)

SubsystemMedia::SubsystemMedia( MRef<SipSoftPhoneConfiguration *> conf, 
				MRef<IpProvider *> ipp, 
				MRef<IpProvider *> ip6p )
{

	MediaHandler *mh = new MediaHandler(conf, ipp, ip6p);
	mediaHandler = new MRef<MediaHandler*>(mh);

}

SubsystemMedia::~SubsystemMedia(){
	if (mediaHandler){
		MRef<MediaHandler*> * mrefPtr = (MRef<MediaHandler*> *) mediaHandler;
		//(*mrefPtr)->free();
		delete mrefPtr;
		mediaHandler=NULL;
	}
}

void SubsystemMedia::setMessageRouterCallback(MRef<CommandReceiver*> callback){
	MH->setMessageRouterCallback(callback);
}

CommandString SubsystemMedia::handleCommandResp(string, const CommandString& c){
	assert(1==0); //Not used
	return c; // Not reached; masks warning
}


void SubsystemMedia::handleCommand(string subsystem, const CommandString& command ){
	assert(subsystem=="media");


	return MH->handleCommand(subsystem, command);

}


MRef<Session *> SubsystemMedia::createSession( MRef<SipIdentity*> ident, std::string callId ){
	return MH->createSession(ident, callId);

}

