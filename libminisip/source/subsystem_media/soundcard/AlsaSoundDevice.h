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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Cesc Santasusana <c e s c DOT s a n t a [AT} g m a i l DOT c o m>
*/

#ifndef ALSASOUNDDEVICE_H
#define ALSASOUNDDEVICE_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/soundcard/SoundDevice.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>
#define MIN_HW_PO_BUFFER (20 * 1000)   /* This buffer size is in micro secs ...*/

class LIBMINISIP_API AlsaSoundDevice: public SoundDevice{
	public:
		AlsaSoundDevice( std::string device );
		virtual int readFromDevice( byte_t * buffer, uint32_t nSamples );
		virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples );
		
		virtual int writeToDevice( byte_t * buffer, uint32_t nSamples );
		virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples );
		
		virtual int openRecord( int samplingRate, int nChannels, int format );
		virtual int closeRecord();
		
		virtual int openPlayback( int samplingRate, int nChannels, int format );
		virtual int closePlayback();

		virtual void sync();

		virtual std::string getMemObjectType() const { return "AlsaSoundDevice";};


	private:
	
		/**
		Calculate, given the values and the MIN_HW_PO_BUFFER, the 
		correct values to set to the alsa device.
		If periodSize and numPeriods (class members) are already set (different than zero),
		we will use these values.
		@return 0 if ok, -1 if error. The calculated values will be kept in the class vars below
		*/
		int calculateAlsaParams( unsigned long &periodSizeMin, 
					unsigned long &periodSizeMax, 
					uint32_t &periodsMin, 
					uint32_t &periodsMax, 
					unsigned long &maxBufferSize);
	
		/**
		Some old soundcards do not accept different values for period size and number of periods
		configured for the soundcard buffer ... (same applies to OSS, at least for the period size).
		So, we store the values here and the function calculateAlsaParams will use them if set.
		The buffer is numPeriods*periodSize [periodsInBuffer]*[alsaFramesPerPeriod].
		An alsa frame is numChannels*numBytesPerSample [byte]
		*/
		unsigned long periodSize;
		uint32_t numPeriods;
		
		snd_pcm_t * readHandle;
		snd_pcm_t * writeHandle;

		/**
		Use to lock the oening, be playback or record ... don't want the two at the same time ...
		*/
		Mutex lockOpen;
};


		
#endif
