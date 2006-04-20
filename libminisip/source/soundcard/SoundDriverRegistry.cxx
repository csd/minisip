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

#include<libminisip/soundcard/SoundDriverRegistry.h>
#include<libmutil/dbg.h>

// #ifdef PORTAUDIO_SUPPORT
// #include<libminisip/soundcard/PortAudioDriver.h>
// #endif

using namespace std;

MRef<SoundDriverRegistry *> SoundDriverRegistry::instance;

SoundDriverRegistry::SoundDriverRegistry(){
}

SoundDriverRegistry::~SoundDriverRegistry(){
}

#ifdef DEBUG_OUTPUT
static void dumpAllNames( MRef<SoundDriverRegistry*> instance ){
	std::vector<SoundDeviceName> names = instance->getAllDeviceNames();

	vector<SoundDeviceName>::iterator iter;
	vector<SoundDeviceName>::iterator stop = names.end();

	mdbg << "Dumping sound device names:" << end;
	for( iter = names.begin(); iter != stop; iter++ ){
		mdbg << iter->getName() << " " << iter->getDescription() << " in:" << iter->getMaxInputChannels() << ", out:" << iter->getMaxOutputChannels() << end;
	}
}
#endif

MRef<SoundDriverRegistry*> SoundDriverRegistry::getInstance(){
	if( !instance ){
		instance = new SoundDriverRegistry();

#ifdef PORTAUDIO_SUPPORT
// 		instance->registerDriver( new PortAudioDriver() );
#endif
#ifdef DEBUG_OUTPUT
		dumpAllNames( instance );
#endif
	}

	return instance;
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


MRef<SoundDevice*> SoundDriverRegistry::createDevice( std::string deviceName ){
	string driverId;
	string deviceId;

	size_t pos = deviceName.find( ':', 0 );
	if( pos == string::npos ){
		driverId = "oss";
		deviceId = deviceName;
	}
	else{
		driverId = deviceName.substr( 0, pos );
		deviceId = deviceName.substr( pos + 1 );
	}

	vector< MRef<SoundDriver*> >::iterator iter;
	vector< MRef<SoundDriver*> >::iterator stop = drivers.end();

	for( iter = drivers.begin(); iter != stop; iter++ ){
		MRef<SoundDriver*> driver = *iter;

		if( driver->getId() == driverId ){
			return driver->createDevice( deviceId );
		}
	}

	merr << "device not found " << deviceName << end;
	return NULL;
}

void SoundDriverRegistry::registerPlugin( MRef<MPlugin*> plugin ){
	MPluginRegistry::registerPlugin( plugin );

	MRef<SoundDriver *> driver = dynamic_cast<SoundDriver*>(*plugin);

	if( driver ){
		registerDriver( driver );
	}
	else {
		merr << "Not SoundDriver!" << end;
	}
}


bool SoundDriverRegistry::registerDriver( MRef<SoundDriver*> driver ){
	vector< MRef<SoundDriver*> >::iterator iter;

	iter = find( drivers.begin(), drivers.end(), driver );

	if ( iter != drivers.end() ){
		merr << "registerDriver: Driver already registered: " << driver->getId() << end;
		return false;
	}

	mdbg << "SoundDriverRegistry: registering " << driver->getDescription() << " as " << driver->getId() << end;
	drivers.push_back( driver );
	return true;
}

bool SoundDriverRegistry::unregisterDriver( MRef<SoundDriver*> driver ){
	vector< MRef<SoundDriver*> >::iterator iter;

	iter = find( drivers.begin(), drivers.end(), driver );

	if ( iter == drivers.end() ){
		merr << "unregisterDriver: Driver not registered: " << driver->getId() << end;
		return false;
	}

	drivers.erase( iter, iter + 1 );
	return true;
}
