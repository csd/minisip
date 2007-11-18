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

#ifndef OSSSOUNDDRIVER_H
#define OSSSOUNDDRIVER_H

#include<libminisip/libminisip_config.h>

#include<string>
#include<libmutil/MemObject.h>

#include<libminisip/media/soundcard/SoundDriver.h>


class OssSoundDriver: public SoundDriver{
	public:
		OssSoundDriver( MRef<Library*> lib );
		virtual ~OssSoundDriver();
		virtual MRef<SoundDevice*> createDevice( std::string deviceId );
		virtual std::string getDescription() const { return "OssSound sound driver"; };

		virtual std::vector<SoundDeviceName> getDeviceNames() const;

		virtual bool getDefaultInputDeviceName( SoundDeviceName &name ) const;
		
		virtual bool getDefaultOutputDeviceName( SoundDeviceName &name ) const;

		virtual std::string getName() const {
			return "OssSound";
		}

		virtual std::string getMemObjectType() const { return getName(); }

		virtual uint32_t getVersion() const;
};

#endif	// OSSSOUNDDRIVER_H
