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

/* Copyright (C) 2006
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libminisip/soundcard/SoundDriver.h>
#include<libminisip/soundcard/SoundDriverRegistry.h>
#include<libmutil/MPlugin.h>
#include<portaudio.h>

#include"PortAudioDriver.h"
#include"PortAudioDevice.h"

using namespace std;

static const char PA_PREFIX[] = "pa";
static std::list<std::string> pluginList;
static int initialized;


extern "C"
std::list<std::string> *mportaudio_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("mportaudio_LTX_getPortAudioPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C"
MRef<MPlugin *> mportaudio_LTX_getPortAudioPlugin( MRef<Library*> lib ){
	return new PortAudioDriver( lib );
}

PortAudioDriver::PortAudioDriver( MRef<Library*> lib ) : SoundDriver( PA_PREFIX, lib ), initialized( false ){
	PaError res = Pa_Initialize();

	if( res == paNoError ){
		initialized = true;
	}
}

PortAudioDriver::~PortAudioDriver( ){
	if( initialized ){
		Pa_Terminate();
		initialized = false;
	}
}

MRef<SoundDevice*> PortAudioDriver::createDevice( string deviceId ){
	if( !initialized ){
		merr << "PortAudioDriver not initialized" << end;
		return NULL;
	}

	PaDeviceIndex device = atoi( deviceId.c_str() );

	if( device < 0 || device >= Pa_GetDeviceCount() ){
		merr << "PortAudio: invalid device: " << deviceId << end;
		return NULL;
	}

	return new PortAudioDevice( device );
}

std::vector<SoundDeviceName> PortAudioDriver::getDeviceNames() const {
	std::vector<SoundDeviceName> names;

	for( int i = 0; i < Pa_GetDeviceCount(); i++ ){
		char device[10] = "";

		const PaDeviceInfo *info = Pa_GetDeviceInfo( i );

		if( !info ){
			continue;
		}

		snprintf( device, sizeof( device ), "%s:%d", PA_PREFIX, i );
		names.push_back( SoundDeviceName( device, info->name, info->maxInputChannels, info->maxOutputChannels ) );
	}

	return names;
}

uint32_t PortAudioDriver::getVersion() const{
	return 0x00000001;
}
