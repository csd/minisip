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

#ifndef SOUNDDRIVER_H
#define SOUNDDRIVER_H

#include<libminisip/libminisip_config.h>
#include<libmutil/MPlugin.h>

#include<string>
#include<iterator>
#include<libmutil/MemObject.h>

#include<libminisip/media/soundcard/SoundDevice.h>

#include<vector>

class LIBMINISIP_API SoundDeviceName{
	public:
		SoundDeviceName(): maxInputChannels(0), maxOutputChannels(0) {}

		SoundDeviceName(std::string name, 
				std::string description, 
				int maxInputChannels = 2, 
				int maxOutputChannels = 2 );

		/** Sound device name including driver id prefix */
		std::string getName() const;

		/** Human readable string  */
		std::string getDescription() const;

		int getMaxInputChannels() const;

		int getMaxOutputChannels() const;

		int operator==( const SoundDeviceName &dev ) const;

	private:
		std::string name;
		std::string description;
		int maxInputChannels;
		int maxOutputChannels;
};

/**
 * Abstract base class of loadable sound driver plugins.
 */
class LIBMINISIP_API SoundDriver: public MPlugin{
	public:
		SoundDriver( std::string driverId, MRef<Library *> lib = NULL );
		virtual ~SoundDriver();
		virtual MRef<SoundDevice*> createDevice( std::string deviceName ) = 0;
		/** Identification string used as prefix in device names */
		std::string getId() const;

		/** Human readable string */
		virtual std::string getDescription() const = 0;

		std::string getPluginType() const { return "SoundDriver"; }

		/** Returns a list of device names supported by the driver */
		virtual std::vector<SoundDeviceName> getDeviceNames() const = 0;

		/** @return the name of the driver's default input device */
		virtual bool getDefaultInputDeviceName( SoundDeviceName &name ) const = 0;

		/** @returns the name of the driver's default output device */
		virtual bool getDefaultOutputDeviceName( SoundDeviceName &name ) const = 0;

		int operator==( const SoundDriver &driver ) const;
		
	private:
		std::string id;
};

#endif	// SOUNDDRIVER_H
