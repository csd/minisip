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

#ifndef SPEEXCODEC_H
#define SPEEXCODEC_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif

#include<libminisip/Codec.h>

//#ifdef HAS_SPEEX
#include<speex/speex.h> 
#define     MAX_NB_BYTES  1024

class LIBMINISIP_API SpeexCodecState : public CodecState{
        public:
                SpeexCodecState();
                virtual ~SpeexCodecState();

                /**
                 * @returns Number of bytes in output buffer
                 */
                virtual uint32_t encode(void *in_buf, int32_t in_buf_size, void *out_buf);

                /**
                 *
                 * @returns Number of samples in output buffer
                 */
                virtual uint32_t decode(void *in_buf, int32_t in_buf_size, void *out_buf);

        private:
                void         *enc_state;
                void         *dec_state;
                SpeexBits    bits;
                float        input_frame[160];
                int          nbBytes;
                char         bytes_ptr[MAX_NB_BYTES];
                char        *input_bytes;
                float       *output_frame;
                int          frame_size;
};

class LIBMINISIP_API SpeexCodec : public AudioCodec{
	public:
		virtual MRef<CodecState *> newInstance();

		/**
		 * @return Requested sampling freq for the CODEC
		 */
		virtual int32_t getSamplingFreq();

		/**
		 * Time in milliseconds to put in each frame/packet. This is 20ms for the SPEEX codec.
		 */
		virtual int32_t getSamplingSizeMs();

		/**
		 * size of the output of the codec in bytes. This is 160.
		 */
		virtual int32_t getEncodedNrBytes();
		
		virtual int32_t getInputNrSamples();
		
		virtual string getCodecName();
		
		virtual string getCodecDescription();

		virtual uint8_t getSdpMediaType();

		virtual string getSdpMediaAttributes();
		
};

//#endif //HAS_SPEEX

#endif
