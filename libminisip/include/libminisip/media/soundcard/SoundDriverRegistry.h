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

#ifndef SOUNDDRIVERREGISTRY_H
#define SOUNDDRIVERREGISTRY_H

#include<libminisip/libminisip_config.h>

#include<vector>
#include<libmutil/MemObject.h>
#include<libmutil/MPlugin.h>
#include<libmutil/MSingleton.h>
#include<libminisip/media/soundcard/SoundDriver.h>

/**
 * Registry of sound driver plugins.
 */
class LIBMINISIP_API SoundDriverRegistry: public MPluginRegistry, public MSingleton<SoundDriverRegistry>{
	public:
		virtual std::string getPluginType(){ return "SoundDriver"; }
		const std::vector<MRef<SoundDriver*> > &getDrivers() const;
		std::vector<SoundDeviceName> getAllDeviceNames() const;

		/** @return the default sound driver */
		virtual MRef<SoundDriver*> getDefaultDriver() const;

		/**
		 * @param device specifies a sound device and
		 * should be written in the format "driver:id", where driver
		 * is the registered name of the driver, and id depends on
		 * the driver.
		 */
		MRef<SoundDevice*> createDevice( std::string deviceName );
		virtual void registerPlugin( MRef<MPlugin*> plugin );
		bool registerDriver( MRef<SoundDriver*> driver );
		bool unregisterDriver( MRef<SoundDriver*> driver );

		/** Work around for Win32, which doesn't support, weak
		    symbols in DLLs */
		static MRef<SoundDriverRegistry*> getInstance();

	protected:
		SoundDriverRegistry();

	private:
		std::vector< MRef<SoundDriver*> > drivers;

		friend class MSingleton<SoundDriverRegistry>;		
};

#endif	// SOUNDDRIVERREGISTRY_H
