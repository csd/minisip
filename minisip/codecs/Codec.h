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

#ifndef CODECINTERFACE_H
#define CODECINTERFACE_H

#include<sys/types.h>

#include<string>

#include<libmutil/MemObject.h>

/* Fixed size blocks (always results in the same output frame size)
 *
*/

using namespace std;


class Codec: public MObject{
	public:

		
		virtual std::string getCodecName()=0;
		
		virtual std::string getCodecDescription()=0;
		
		virtual int32_t getSdpMediaType()=0;

		virtual std::string getSdpMediaAttributes()=0;

		virtual std::string getMemObjectType(){return "Codec";}

};

class AudioCodec : public Codec{
	public:
		/**
		 * @returns Number of bytes in output buffer
		 */
		virtual void encode(void *in_buf, int32_t in_buf_size, void *out_buf)=0;

		/**
		 * 
		 * @returns Number of bytes in output buffer
		 */
		virtual void decode(void *in_buf, int32_t in_buf_size, void *out_buf)=0;

		/**
		 * Decodes a frame without having any input. Typically done when
		 * packets are lost.
		 * @return number of samples in putput buffer
		 */
		virtual void decode(void *out_buf)=0;
		
		/**
		 * size of the output of the codec in bytes.
		 */
		virtual int32_t getEncodedNrBytes()=0;
		
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
