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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/* Copyright (C) 2006
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libminisip/media/soundcard/SoundDriver.h>
#include<libminisip/media/soundcard/SoundDriverRegistry.h>
#include<libmutil/MPlugin.h>

#include"DirectSoundDriver.h"
#include"DirectSoundDevice.h"

using namespace std;

static const char DRIVER_PREFIX[] = "dsound";
static std::list<std::string> pluginList;
static int initialized;

extern "C" LIBMINISIP_API
std::list<std::string> *mdsound_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getDirectSoundPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mdsound_LTX_getDirectSoundPlugin( MRef<Library*> lib ){
	return new DirectSoundDriver( lib );
}

DirectSoundDriver::DirectSoundDriver( MRef<Library*> lib ) : SoundDriver( DRIVER_PREFIX, lib ){
}

DirectSoundDriver::~DirectSoundDriver( ){
}

MRef<SoundDevice*> DirectSoundDriver::createDevice( string deviceId ){
	return new DirectSoundDevice( deviceId );
}

static BOOL CALLBACK dsEnumCallback(LPGUID guid, LPCSTR description,
				    LPCSTR module, LPVOID context,
				    bool capture){
	std::vector<SoundDeviceName> *names = (std::vector<SoundDeviceName>*)context;
	string uuid;

	if( guid ){
		unsigned char *stringUuid = NULL;
		if( UuidToStringA(guid, &stringUuid) != RPC_S_OK ){
			return true;
		}
		uuid = string( (const char*) stringUuid );
		RpcStringFreeA( &stringUuid );
	}
	else{
		uuid = "0";
	}

	string name = string( DRIVER_PREFIX ) + ':' + uuid;
	// FIXME detect channel count.
	int maxInputChannels = capture ? 2 : 0;
	int maxOutputChannels = capture ? 0 : 2;
	SoundDeviceName deviceName( name, description, maxInputChannels, maxOutputChannels );

	names->push_back( deviceName );
	
	return true;
}

static BOOL CALLBACK dsEnumPlaybackCallback(LPGUID guid, LPCSTR description,
					   LPCSTR module, LPVOID context){
	return dsEnumCallback(guid, description, module, context, false);
}

static BOOL CALLBACK dsEnumCaptureCallback(LPGUID guid, LPCSTR description,
					   LPCSTR module, LPVOID context){
	return dsEnumCallback(guid, description, module, context, true);
}

std::vector<SoundDeviceName> DirectSoundDriver::getDeviceNames() const {
	std::vector<SoundDeviceName> names;

	if( DirectSoundEnumerateA( dsEnumPlaybackCallback, &names ) != DS_OK ){
		cerr << "DirectSoundDriver::getDeviceNames DirectSoundEnumerate failed" << endl;
	}

	if( DirectSoundCaptureEnumerateA( dsEnumCaptureCallback, &names ) != DS_OK ){
		cerr << "DirectSoundDriver::getDeviceNames DirectSoundEnumerate failed" << endl;
	}

	return names;
}

bool DirectSoundDriver::getDefaultInputDeviceName( SoundDeviceName &name ) const{
	mdbg << "DirectSoundDriver::getDefaultInputDeviceName not implemented" << endl;

	return false;
}

bool DirectSoundDriver::getDefaultOutputDeviceName( SoundDeviceName &name ) const {
	mdbg << "DirectSoundDriver::getDefaultOutputDeviceName not implemented" << endl;

	return false;
}

uint32_t DirectSoundDriver::getVersion() const{
	return 0x00000001;
}
