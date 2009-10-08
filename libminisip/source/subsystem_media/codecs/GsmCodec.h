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

/* Copyright (C) 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef MINISIP_GSM_CODEC_H
#define MINISIP_GSM_CODEC_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/codecs/Codec.h>

typedef struct gsm_state *      gsm;

class GsmCodec: public AudioCodec{
	public:
		GsmCodec( MRef<Library *> lib );

		MRef<CodecState *> newInstance();
		std::string getCodecName();
		std::string getCodecDescription();
		uint8_t getSdpMediaType();
		std::string getSdpMediaAttributes();
		std::string getMemObjectType() const {return "GsmCodec";};

//		int32_t getEncodedNrBytes();
		int32_t getInputNrSamples();
		int32_t getSamplingFreq();
		int32_t getSamplingSizeMs();
		virtual uint32_t getVersion()const;
};

class GsmCodecState: public CodecState{
	public:

		GsmCodecState();
		~GsmCodecState();

		uint32_t encode( void *inBuf, int32_t inSize, int samplerate, void *outBuf );
		uint32_t decode( void *inBuf, int32_t inSize, void *outBuf );

		std::string getMemObjectType() const {return "GsmCodecState";};

	private:
		gsm gsmState;
};

#endif
