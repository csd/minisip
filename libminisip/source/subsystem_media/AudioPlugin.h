/*
 Copyright (C) 2006  Mikael Magnusson
 
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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Copyright (C) 2006
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#ifndef AUDIO_MEDIA_PLUGIN_H
#define AUDIO_MEDIA_PLUGIN_H

#include<libminisip/libminisip_config.h>
#include<libminisip/media/Media.h>

class Media;
class SipSoftPhoneConfiguration;

class AudioPlugin : public MediaPlugin{
	public:
		AudioPlugin(MRef<Library*> lib);
		virtual ~AudioPlugin();

		virtual MRef<Media*> createMedia( MRef<SipSoftPhoneConfiguration *> config );
///////////////////
		virtual MRef<Media*> createMedia2stream( MRef<SipSoftPhoneConfiguration *> config){};

		virtual std::string getMemObjectType() const { return "AudioPlugin"; }
		virtual std::string getName() const{ return "audio"; }
		virtual uint32_t getVersion() const{ return 0x00000001; }
		virtual std::string getDescription() const{
			return "audio media plugin";
		}
};

#endif	// AUDIO_MEDIA_PLUGIN_H
