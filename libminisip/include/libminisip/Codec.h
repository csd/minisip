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

#ifndef CODECINTERFACE_H
#define CODECINTERFACE_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<sys/types.h>

#include<string>

#include<libmutil/MemObject.h>

/* Fixed size blocks (always results in the same output frame size)
 *
*/

#ifdef _MSC_VER
typedef int32_t int;
#else
#include<inttypes.h>
#endif

using namespace std;


class LIBMINISIP_API Codec: public MObject{
	public:

		
		virtual std::string getCodecName()=0;
		
		virtual std::string getCodecDescription()=0;
		
		virtual int32_t getSdpMediaType()=0;

		virtual std::string getSdpMediaAttributes()=0;

		virtual std::string getMemObjectType(){return "Codec";}

};

class LIBMINISIP_API AudioCodec : public Codec{
	public:
		/**
		 * @returns A CODEC instance for the given payloadType
		 * (NULL if not handled)
		 */
		static MRef<AudioCodec *> create( unsigned char payloadType );
		
                /**
		 * @returns A CODEC instance for the given description string
		 * (NULL if not handled)
		 */
		static MRef<AudioCodec *> create( const std::string& );

		/**
		 * @returns Number of bytes in output buffer
		 */
		virtual uint32_t encode(void *in_buf, int32_t in_buf_size, void *out_buf)=0;

		/**
		 * 
		 * @returns Number of frames in output buffer
		 */
		virtual uint32_t decode(void *in_buf, int32_t in_buf_size, void *out_buf)=0;

		/**
		 * Decodes a frame without having any input. Typically done when
		 * packets are lost.
		 * @return number of samples in putput buffer
		 */
		virtual void decode(void *out_buf)=0;
		
		/**
		 * size of the output of the codec in bytes.
		 */
		virtual int32_t getEncodedNrBytes()=0;//
		
		virtual int32_t getInputNrSamples()=0;
	
		/**
		 * @return Requested sampling freq for the CODEC
		 */
		virtual int32_t getSamplingFreq()=0;

		/**
		 * Time in milliseconds to put in each frame/packet
		 */
		virtual int32_t getSamplingSizeMs()=0;
		
		virtual std::string getMemObjectType(){return "AudioCodec";}
		
};


#endif
