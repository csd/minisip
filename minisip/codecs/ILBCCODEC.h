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

#ifndef ILBCCODEC_H
#define ILBCCODEC_H

#include<config.h>

#include"Codec.h"

#include"ilbc/iLBC_define.h"
#include"ilbc/iLBC_encode.h"
#include"ilbc/iLBC_decode.h"

class ILBCCODEC : public AudioCodec{
	public:
		ILBCCODEC();

		/**
		 * @returns Number of bytes in output buffer
		 */
		virtual void encode(void *in_buf, int32_t in_buf_size, void *out_buf);

		/**
		 * 
		 * @returns Number of bytes in output buffer
		 */
		virtual void decode(void *in_buf, int32_t in_buf_size, void *out_buf);

		/**
		 * Decodes a frame without having any input. Typically done when
		 * packets are lost.
		 * @return number of samples in putput buffer
		 */
		virtual void decode(void *out_buf);
	
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
		virtual int32_t getEncodedNrBytes();
		
		virtual int32_t getInputNrSamples();

		virtual string getCodecName();
		
		virtual string getCodecDescription();
		
		virtual int32_t getSdpMediaType();

		virtual string getSdpMediaAttributes();
		
	private:
		iLBC_Enc_Inst_t enc_inst; 
		iLBC_Dec_Inst_t dec_inst; 
};

#endif
