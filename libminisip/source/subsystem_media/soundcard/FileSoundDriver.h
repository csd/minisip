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

#ifndef FILESOUNDDRIVER_H
#define FILESOUNDDRIVER_H

#include<libminisip/libminisip_config.h>

#include<string>
#include<libmutil/MemObject.h>

#include<libminisip/media/soundcard/SoundDriver.h>


class FileSoundDriver: public SoundDriver{
	public:
		FileSoundDriver( MRef<Library*> lib );
		virtual ~FileSoundDriver();
		virtual MRef<SoundDevice*> createDevice( std::string deviceId );
		virtual std::string getDescription() const { return "FileSound sound driver"; };

		virtual std::vector<SoundDeviceName> getDeviceNames() const;

		virtual bool getDefaultInputDeviceName( SoundDeviceName &name ) const { return false; }

		virtual bool getDefaultOutputDeviceName( SoundDeviceName &name ) const { return false; }

		virtual std::string getName() const {
			return "FileSound";
		}

		virtual std::string getMemObjectType() const { return getName(); }

		virtual uint32_t getVersion() const;
};

#endif	// FILESOUNDDRIVER_H
