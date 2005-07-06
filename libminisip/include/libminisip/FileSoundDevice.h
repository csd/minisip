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

#ifndef FILESOUNDDEVICE_H
#define FILESOUNDDEVICE_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include"SoundDevice.h"



#include<stdio.h>
#include <fcntl.h>
#include<iostream>

using namespace std;

class LIBMINISIP_API FileSoundDevice: public SoundDevice{
	public:
		FileSoundDevice( string in_file, string out_file, int  nChannels, int32_t speed );
		
		virtual int read( byte_t * buffer, uint32_t nSamples );
		virtual int write( byte_t * buffer, uint32_t nSamples );

		virtual int openPlayback( int32_t samplingRate, int nChannels, int format );
		virtual int openRecord( int32_t samplingRate, int nChannels, int format );
		virtual int closePlayback();
		virtual int closeRecord();

		virtual void sync();

		virtual std::string getMemObjectType(){ return "FileSoundDevice";};

	private:
                string in_file;
                string out_file;
		int in_fd;
                int out_fd;
                

};


#endif	
