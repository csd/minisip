/*
 Copyright (C) 2007  Mikael Magnusson
 
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

/* Copyright (C) 2007
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libminisip/media/soundcard/SoundDriver.h>
#include<libminisip/media/soundcard/SoundDriverRegistry.h>
#include<libmutil/MPlugin.h>

#include"OssSoundDriver.h"
#include"OssSoundDevice.h"

using namespace std;

static const char DRIVER_PREFIX[] = "oss";
static std::list<std::string> pluginList;
static int initialized;


extern "C" LIBMINISIP_API
std::list<std::string> *moss_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * moss_LTX_getPlugin( MRef<Library*> lib ){
	return new OssSoundDriver( lib );
}

OssSoundDriver::OssSoundDriver( MRef<Library*> lib ) : SoundDriver( DRIVER_PREFIX, lib ){
}

OssSoundDriver::~OssSoundDriver( ){
}

MRef<SoundDevice*> OssSoundDriver::createDevice( string deviceId ){
	return new OssSoundDevice( deviceId );
}

std::vector<SoundDeviceName> OssSoundDriver::getDeviceNames() const {
	std::vector<SoundDeviceName> names;

	mdbg << "OssSoundDriver::getDeviceNames not implemented" << endl;

	return names;
}

bool OssSoundDriver::getDefaultInputDeviceName( SoundDeviceName &name ) const{
	name = SoundDeviceName( "/dev/dsp", "/dev/dsp" );
	return true;
}

bool OssSoundDriver::getDefaultOutputDeviceName( SoundDeviceName &name ) const {
	name = SoundDeviceName( "/dev/dsp", "/dev/dsp" );
	return true;
}

uint32_t OssSoundDriver::getVersion() const{
	return 0x00000001;
}
