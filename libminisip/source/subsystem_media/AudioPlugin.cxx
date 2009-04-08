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

#include<config.h>
#include"AudioPlugin.h"

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/media/Media.h>
#include<libminisip/media/AudioMedia.h>
#include<libminisip/media/soundcard/SoundDevice.h>


using namespace std;

static std::list<std::string> pluginList;
static int initialized;
static MRef<MPlugin *> plugin;


extern "C"
std::list<std::string> *maudio_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		
		plugin = new AudioPlugin( lib );
	}

	return &pluginList;
}

extern "C"
MPlugin * maudio_LTX_getPlugin( MRef<Library*> lib ){
	return /*&plugin*/ *plugin;
}

AudioPlugin::AudioPlugin( MRef<Library*> lib ): MediaPlugin( lib ){
}

AudioPlugin::~AudioPlugin(){
}

MRef<Media*> AudioPlugin::createMedia( MRef<SipSoftPhoneConfiguration *> config ){
	string soundDevIn = config->soundDeviceIn;
	string soundDevOut = config->soundDeviceOut;
	if( soundDevIn == "" || soundDevOut == ""){
		return NULL;
	}

	MRef<SoundDevice *> sounddevin = SoundDevice::create( soundDevIn );
	MRef<SoundDevice *> sounddevout;

	if( soundDevIn == soundDevOut )
		sounddevout = sounddevin;
	else
		sounddevout = SoundDevice::create( soundDevOut );

	MRef<SoundIO *> soundIo = new SoundIO( sounddevin, sounddevout,
					       config->soundIOmixerType, 
					       2,  //number of channels
					       SOUND_CARD_FREQ ); //sampling rate

	std::list<MRef<Codec *> > codecList;
	std::list<std::string>::iterator iCodec;
	
	for( iCodec = config->audioCodecs.begin(); 
	     iCodec != config->audioCodecs.end();
	     iCodec ++ ){
		MRef<Codec *> selectedCodec;
		MRef<AudioCodec *> codec = AudioCodecRegistry::getInstance()->create( *iCodec );
		
		
		if( codec ){
			selectedCodec = *codec;
		}
		
		if( selectedCodec ){
			mdbg << "Adding audio codec: " << selectedCodec->getCodecName() << endl;
			codecList.push_back( selectedCodec );
		}
		
	}
	
	return new AudioMedia( soundIo, codecList );
}
