/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef ALSASOUNDDEVICE_H
#define ALSASOUNDDEVICE_H

#include"SoundDevice.h"

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>
#define MIN_HW_PO_BUFFER 15000



class AlsaSoundDevice: public SoundDevice{
	public:
		AlsaSoundDevice( std::string device );
		virtual int read( byte_t * buffer, uint32_t nSamples );
		virtual int write( byte_t * buffer, uint32_t nSamples );

		virtual int openRecord( int samplingRate, int nChannels, int format );
		virtual int closeRecord();
		
		virtual int openPlayback( int samplingRate, int nChannels, int format );
		virtual int closePlayback();

		virtual void sync();

		virtual std::string getMemObjectType(){ return "AlsaSoundDevice";};


	private:
		snd_pcm_t * readHandle;
		snd_pcm_t * writeHandle;

};


		
#endif