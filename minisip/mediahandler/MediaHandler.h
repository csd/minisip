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

#ifndef MEDIA_HANDLER_H
#define MEDIA_HANDLER_H

#include<libmutil/MemObject.h>

#include"Media.h"
#include"AudioMedia.h"
#include"MediaStream.h"
#include"Session.h"
#include"RtpReceiver.h"
#include"../minisip/ipprovider/IpProvider.h"
#include<libmutil/CommandString.h>


class SipSoftPhoneConfiguration;
class IpProvider;


class MediaHandler : public MObject, public SessionRegistry{

	public:
		MediaHandler( MRef<SipSoftPhoneConfiguration *> config, MRef<IpProvider *> ipProvider );
		MRef<Session *>createSession( SipDialogSecurityConfig &config, string callId = "" );
		
		void registerMedia( MRef<Media *> media );

		void handleCommand( CommandString command );
		std::string getExtIP();
		virtual std::string getMemObjectType(){return "MediaHandler";}

		MRef<Codec *> createCodec( uint8_t payloadType );

	private:
		std::list< MRef<Media *> > media;

		string ringtoneFile;

		MRef<AudioMedia *> audioMedia;
		MRef<IpProvider *> ipProvider;

};

#endif
