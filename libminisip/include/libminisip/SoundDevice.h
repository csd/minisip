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

#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#ifdef _MSC_VER
#ifndef byte_t
typedef unsigned char  byte_t;
#endif
#ifndef int32_t
typedef __int32  int32_t;
#endif
#ifndef uint32_t
typedef unsigned int  uint32_t;
#endif
#else // !_MSC_VER
#include<stdint.h>
#define byte_t uint8_t
#endif

#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>

#include<iostream>

#define SOUND_S16LE 	0xF0
#define SOUND_S16BE 	0xF1
#define SOUND_U16LE	0xF2
#define SOUND_U16BE	0xF3

class LIBMINISIP_API SoundDevice: public MObject{
	public:
		static MRef<SoundDevice *> create( std::string deviceId );
		

		virtual ~SoundDevice();

		virtual int openRecord( int32_t samplingRate, int nChannels, int format )=0;
		virtual int openPlayback( int32_t samplingRate, int nChannels, int format )=0;
		virtual int closeRecord()=0;
		virtual int closePlayback()=0;

		virtual int read( byte_t * buffer, uint32_t nSamples )=0;
		virtual int write( byte_t * buffer, uint32_t nSamples )=0;

		virtual void sync()=0;


		void lockRead();
		void unlockRead();
		void lockWrite();
		void unlockWrite();
		std::string dev;

		int getSamplingRate(){ return samplingRate; };
		int getNChannelsPlay(){ return nChannelsPlay; };
		int getNChannelsRecord(){ return nChannelsRecord; };
		int getSampleSizePlay();
		int getSampleSizeRecord();
		bool isOpenedPlayback(){return openedPlayback;};
		bool isOpenedRecord(){return openedRecord;};

	protected:
		SoundDevice( std::string fileName );

		int samplingRate;
		int nChannelsPlay;
		int nChannelsRecord;
		int format;
		int sampleSize;
		bool openedRecord;
		bool openedPlayback;
	private:
                Mutex mLockRead;
                Mutex mLockWrite;

		
};

#endif