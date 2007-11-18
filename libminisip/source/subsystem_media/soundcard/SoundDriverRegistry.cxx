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
#include<iostream>

#include<libminisip/media/soundcard/SoundDriverRegistry.h>
#include<libmutil/dbg.h>
#include"FileSoundDriver.h"

#ifdef _MSC_VER
#include"DirectSoundDriver.h"
#endif

#ifdef ENABLE_OSS
#include"OssSoundDriver.h"
#endif

#include<algorithm>

using namespace std;

#ifdef DEBUG_OUTPUT
static void dumpAllNames( MRef<SoundDriverRegistry*> instance ){
	std::vector<SoundDeviceName> names = instance->getAllDeviceNames();

	vector<SoundDeviceName>::iterator iter;
	vector<SoundDeviceName>::iterator stop = names.end();

	mdbg << "Dumping sound device names:" << endl;
	for( iter = names.begin(); iter != stop; iter++ ){
		mdbg << iter->getName() << " " << iter->getDescription() << " in:" << iter->getMaxInputChannels() << ", out:" << iter->getMaxOutputChannels() << endl;
	}
}
#endif

SoundDriverRegistry::SoundDriverRegistry(){
	registerPlugin( new FileSoundDriver( NULL ) );
#ifdef _MSC_VER
	registerPlugin( new DirectSoundDriver(NULL) );
#endif
#ifdef ENABLE_OSS
	registerPlugin( new OssSoundDriver( NULL ) );
#endif
}

const std::vector< MRef<SoundDriver*> > &SoundDriverRegistry::getDrivers() const{
	return drivers;
}


std::vector<SoundDeviceName> SoundDriverRegistry::getAllDeviceNames() const{
	std::vector<SoundDeviceName> allNames;

	vector< MRef<SoundDriver*> >::const_iterator iter;
	vector< MRef<SoundDriver*> >::const_iterator end = drivers.end();

	for( iter = drivers.begin(); iter != end; iter++ ){
		MRef<SoundDriver*> driver = *iter;
		std::vector<SoundDeviceName> names = driver->getDeviceNames();

		allNames.insert( allNames.end(), names.begin(), names.end() );
	}

	return allNames;
}

MRef<SoundDriver*> SoundDriverRegistry::getDefaultDriver() const{
	const char *driverPriority[] =
		{ "PortAudio", "DirectSound", "AlsaSound", "OssSound", NULL };
	int i;

	for( i = 0;; i++ ){
		const char *name = driverPriority[i];

		if( !name )
			return NULL;

		MRef<MPlugin*> plugin = findPlugin( name );
		if( plugin ){
			MRef<SoundDriver*> driver;
			driver = dynamic_cast<SoundDriver*>(*plugin);
			if( driver )
				return driver;
		}
	}

	return NULL;
}

MRef<SoundDevice*> SoundDriverRegistry::createDevice( std::string deviceName ){
	string driverId;
	string deviceId;

#ifdef DEBUG_OUTPUT
	mdbg << "SoundDriverRegistry: deviceName =  " << deviceName << endl;
#endif
	size_t pos = deviceName.find( ':', 0 );
	if( pos == string::npos ){
		driverId = "oss";
		deviceId = deviceName;
	}
	else{
		driverId = deviceName.substr( 0, pos );
		deviceId = deviceName.substr( pos + 1 );
	}
#ifdef DEBUG_OUTPUT
	mdbg << "SoundDriverRegistry: deviceId =  " << deviceId << endl;
	mdbg << "SoundDriverRegistry: driverId =  " << driverId << endl;
#endif

	vector< MRef<SoundDriver*> >::iterator iter;
	vector< MRef<SoundDriver*> >::iterator stop = drivers.end();

	for( iter = drivers.begin(); iter != stop; iter++ ){
		MRef<SoundDriver*> driver = *iter;

		if( driver->getId() == driverId ){
			mdbg << "SoundDriverRegistry: device id found!!! =  " << deviceId << endl;
			return driver->createDevice( deviceId );
		}
	}

	mdbg << "SoundDriverRegistry: device not found " << deviceName << endl;
	return NULL;
}

void SoundDriverRegistry::registerPlugin( MRef<MPlugin*> plugin ){
	MPluginRegistry::registerPlugin( plugin );

	MRef<SoundDriver *> driver = dynamic_cast<SoundDriver*>(*plugin);

	if( driver ){
		registerDriver( driver );
	}
	else {
		merr << "Not SoundDriver!" << endl;
	}
}


bool SoundDriverRegistry::registerDriver( MRef<SoundDriver*> driver ){
	vector< MRef<SoundDriver*> >::iterator iter;

	iter = find( drivers.begin(), drivers.end(), driver );

	if ( iter != drivers.end() ){
		merr << "registerDriver: Driver already registered: " << driver->getId() << endl;
		return false;
	}

	mdbg << "SoundDriverRegistry: registering " << driver->getDescription() << " as " << driver->getId() << endl;
	drivers.push_back( driver );
	return true;
}

bool SoundDriverRegistry::unregisterDriver( MRef<SoundDriver*> driver ){
	vector< MRef<SoundDriver*> >::iterator iter;

	iter = find( drivers.begin(), drivers.end(), driver );

	if ( iter == drivers.end() ){
		merr << "unregisterDriver: Driver not registered: " << driver->getId() << endl;
		return false;
	}

	drivers.erase( iter, iter + 1 );
	return true;
}

/** Work around for Win32, which doesn't support, weak
    symbols in DLLs */
MRef<SoundDriverRegistry*> SoundDriverRegistry::getInstance(){
	return MSingleton<SoundDriverRegistry>::getInstance();
}
