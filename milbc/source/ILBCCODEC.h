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
*/

#ifndef ILBCCodec_H
#define ILBCCodec_H

#include<libminisip/libminisip_config.h>

#include<string>

#include<libminisip/media/codecs/Codec.h>

#include"ilbc/iLBC_define.h"
#include"ilbc/iLBC_encode.h"
#include"ilbc/iLBC_decode.h"

class ILBCCodecState : public CodecState{
	public:
		ILBCCodecState();

		/**
		 * @returns Number of bytes in output buffer
		 */
		virtual uint32_t encode(void *in_buf, int32_t in_buf_size, void *out_buf);

		/**
		 * 
		 * @returns Number of frames in output buffer
		 */
		virtual uint32_t decode(void *in_buf, int32_t in_buf_size, void *out_buf);
	

	private:
		iLBC_Enc_Inst_t enc_inst; 
		iLBC_Dec_Inst_t dec_inst; 

};

class ILBCCodec : public AudioCodec{
	public:
		ILBCCodec( MRef<Library *> lib );

		virtual MRef<CodecState *> newInstance();
	
		/**
		 * @return Requested sampling freq for the CODEC
		 */
		virtual int32_t getSamplingFreq();

		/**
		 * Time in milliseconds to put in each frame/packet. This is 30ms for the ILBC codec.
		 */
		virtual int32_t getSamplingSizeMs();

		/**
		 * size of the output of the codec in bytes. This is 50.
		 */
//		virtual int32_t getEncodedNrBytes();
		
		virtual int32_t getInputNrSamples();

		virtual std::string getCodecName();
		
		virtual std::string getCodecDescription();
		
		virtual uint8_t getSdpMediaType();

		virtual std::string getSdpMediaAttributes();

		virtual uint32_t getVersion()const;
};

#endif
