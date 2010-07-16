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

/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include<libminisip/media/Media.h>

//#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/soundcard/SoundIO.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libminisip/media/MediaStream.h>
#include<libmutil/stringutils.h>
#include<libminisip/signaling/sdp/SdpHeaderM.h>
#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<libmikey/KeyAgreement.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#include"AudioPlugin.h"

#ifdef VNC_SUPPORT
#include"vnc/MediaSharedWorkspacePlugin.h"
#endif


using namespace std;

Media::Media(){
	mediaForwarding=false;
}

Media::~Media(){

}

void Media::setMediaForwarding(bool forw){
	mediaForwarding=forw;
}

list<string> Media::getSdpAttributes(){
	return sdpAttributes;
}

void Media::addSdpAttribute( string attribute ){
	sdpAttributes.push_back( attribute );
}

void Media::handleMHeader( MRef< SdpHeaderM * > m ){
}

MediaPlugin::MediaPlugin( MRef<Library*> lib ): MPlugin( lib ){
}

MediaPlugin::~MediaPlugin(){
}


MediaRegistry::MediaRegistry(){
	registerPlugin( new AudioPlugin( NULL ) );
#ifdef VNC_SUPPORT
	registerPlugin( new SharedWorkspacePlugin( NULL ) );
#endif
}

