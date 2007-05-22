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

#include<libminisip/media/soundcard/SoundDriver.h>


SoundDeviceName::SoundDeviceName(std::string name_,
		std::string descr,
		int maxInputChannels_,
		int maxOutputChannels_) :
			name( name_ ),
			description( descr ),
			maxInputChannels( maxInputChannels_ ),
			maxOutputChannels( maxOutputChannels_ )
{
}


std::string SoundDeviceName::getDescription() const { 
	return description;
}

int SoundDeviceName::getMaxInputChannels() const { 
	return maxInputChannels;
}

int SoundDeviceName::getMaxOutputChannels() const {
	return maxOutputChannels;
}

int SoundDeviceName::operator==( const SoundDeviceName &dev ) const {
	return name == dev.name;
}

std::string SoundDeviceName::getName() const {
	return name;
}

SoundDriver::SoundDriver( std::string driverId,  MRef<Library*> lib ) : MPlugin( lib ), id( driverId ){
}

SoundDriver::~SoundDriver( ){
}

int SoundDriver::operator==( const SoundDriver &driver ) const{
	return id == driver.id;
}

std::string SoundDriver::getId() const {
	return id;
}


