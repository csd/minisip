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

#ifndef MEDIA_H
#define MEDIA_H

#include<libminisip/libminisip_config.h>

#include<libmutil/Mutex.h> include<libmutil/MPlugin.h> include<libmutil/MSingleton.h> include <Codec.h>
#include<libminisip/media/codecs/Codec.h>


class SdpHeaderM;
class SipSoftPhoneConfiguration;
class SubsystemMedia;
class MediaHandler;


/**
 * The Media class is a representation of a medium type, for
 * example video or audio.  
 */
class LIBMINISIP_API Media : public MObject{
public:

	~Media();

	/**
	 * Returns the media type as used in the SDP (audio or video).
	 * @returns the media type as a string
	 */
	virtual std::string getSdpMediaType()=0;


	void setMediaForwarding(bool);

	/**
	 * Used to query media specific attributes to add in the
	 * session description (e.g. framesize).
	 * @returns a list of attributes (as string) to add in
	 * a: headers in the session description
	 */
	std::list<std::string> getSdpAttributes();

	/**
	 * Add a media attribute to use in session descriptions (SDP).
	 * @attribute the attribute as a string, as it will appear
	 * in a: headers in the SDP
	 */
	void addSdpAttribute( std::string attribute );

	/**
	 * Used to adapt the medium to a received session description
	 * (e.g. change the framesize). This is likely to 
	 * be a problem with multiple calls and should
	 * be called carefully.
	 * @param m a reference to the received m: SDP header
	 * on which the media should be tweaked
	 */
	virtual void handleMHeader( MRef<SdpHeaderM *> m );

	////
	virtual uint8_t  getCodecgetSdpMediaType()=0;
	virtual MRef<CodecState *> getCodecInstance ()=0;

protected:
	Media();

	std::list<std::string> sdpAttributes;
	bool mediaForwarding;
};


class LIBMINISIP_API MediaPlugin : public MPlugin{
	public:
		MediaPlugin(MRef<Library*> lib);
		virtual ~MediaPlugin();

		virtual MRef<Media*> createMedia( MRef<SipSoftPhoneConfiguration *> config ) = 0;
		virtual MRef<Media*> createMedia2stream (  MRef<SipSoftPhoneConfiguration *> config) = 0;	
	
		virtual std::string getPluginType() const{ return "Media"; }

};


class LIBMINISIP_API MediaRegistry : public MPluginRegistry, public MSingleton<MediaRegistry>{
	public:
		virtual std::string getPluginType(){ return "Media"; }

	protected:
		MediaRegistry();

	private:
		friend class MSingleton<MediaRegistry>;
};

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/media/SubsystemMedia.h>

#endif
