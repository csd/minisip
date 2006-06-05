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

#include<libminisip/soundcard/SoundDriver.h>
#include<libminisip/soundcard/SoundDriverRegistry.h>
#include<libmutil/MPlugin.h>

#include"AlsaSoundDriver.h"
#include"AlsaSoundDevice.h"

using namespace std;

static const char DRIVER_PREFIX[] = "alsa";
static std::list<std::string> pluginList;
static int initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *malsa_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MRef<MPlugin *> malsa_LTX_getPlugin( MRef<Library*> lib ){
	return new AlsaSoundDriver( lib );
}

AlsaSoundDriver::AlsaSoundDriver( MRef<Library*> lib ) : SoundDriver( DRIVER_PREFIX, lib ){
}

AlsaSoundDriver::~AlsaSoundDriver( ){
}

MRef<SoundDevice*> AlsaSoundDriver::createDevice( string deviceId ){
	return new AlsaSoundDevice( deviceId );
}

std::vector<SoundDeviceName> AlsaSoundDriver::getDeviceNames() const {
	std::vector<SoundDeviceName> names;

	mdbg << "AlsaSoundDriver::getDeviceNames not implemented" << end;

	return names;
}

uint32_t AlsaSoundDriver::getVersion() const{
	return 0x00000001;
}
