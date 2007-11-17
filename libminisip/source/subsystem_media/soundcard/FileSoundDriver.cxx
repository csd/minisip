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

#include"FileSoundDriver.h"
#include<libminisip/media/soundcard/FileSoundDevice.h>

using namespace std;

static const char DRIVER_PREFIX[] = "file";

FileSoundDriver::FileSoundDriver( MRef<Library*> lib ) : SoundDriver( DRIVER_PREFIX, lib ){
}

FileSoundDriver::~FileSoundDriver( ){
}

MRef<SoundDevice*> FileSoundDriver::createDevice( string deviceId ){
	return new FileSoundDevice( deviceId, deviceId, FILESOUND_TYPE_RAW );
}

std::vector<SoundDeviceName> FileSoundDriver::getDeviceNames() const {
	std::vector<SoundDeviceName> names;

	return names;
}

uint32_t FileSoundDriver::getVersion() const{
	return 0x00000001;
}
