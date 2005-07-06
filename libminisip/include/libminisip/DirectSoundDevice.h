/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef DIRECT_SOUND_DEVICE_H
#define DIRECT_SOUND_DEVICE_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif



#include<libminisip/SoundDevice.h>



//typedef uint8_t byte_t;

class LIBMINISIP_API DirectSoundDevice : public SoundDevice{
	public:
		DirectSoundDevice( std::string fileName );

		virtual ~DirectSoundDevice();

		virtual int openRecord( int samplingRate, int nChannels, int format );
		virtual int openPlayback( int samplingRate, int nChannels, int format );
		virtual int closeRecord();
		virtual int closePlayback();

		virtual int read( byte_t * buffer, uint32_t nSamples );
		virtual int write( byte_t * buffer, uint32_t nSamples );

		virtual void sync();

	private:
		
};

#endif
