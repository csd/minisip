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

#ifndef CODECINTERFACE_H
#define CODECINTERFACE_H

#include<libminisip/libminisip_config.h>

#include<sys/types.h>

#include<string>

#include<libmutil/MemObject.h>
#include<libmutil/MPlugin.h>
#include<libmutil/MSingleton.h>

class Codec;
class CodecState;

class Codec: public MPlugin{
	public:

		virtual MRef<CodecState *> newInstance()=0;
		
		virtual std::string getCodecName()=0;
		
		/**
		 * This should be a single line that can be presented to
		 * in the GUI describing what the codec is (name, bandwidth
		 * usage, HZ or what ever is found necessary)
		 */
		virtual std::string getCodecDescription()=0;
		
		/**
		 * Returns the number assigned to this CODEC.
		 * Example: 0 for PCMu and 8 for PCMa
		 */
		virtual uint8_t getSdpMediaType()=0;

		virtual std::string getSdpMediaAttributes()=0;

		virtual std::string getMemObjectType() const {return "Codec";}

		virtual std::string getName()const {
			return (const_cast<Codec*>(this))->getCodecName();
		}

		virtual std::string getDescription()const {
			return (const_cast<Codec*>(this))->getCodecDescription();
		}

	protected:
		Codec( MRef<Library *> lib ): MPlugin( lib ) {}
		Codec(): MPlugin() {}
};

class CodecState: public MObject{
	public:
		/**
		 * @returns Number of bytes in output buffer
		 */
		virtual uint32_t encode(void *in_buf, int32_t in_buf_size, int samplerate, void *out_buf)=0;

		/**
		 * 
		 * @returns Number of frames in output buffer
		 */
		virtual uint32_t decode(void *in_buf, int32_t in_buf_size, void *out_buf)=0;

		virtual std::string getMemObjectType() const {return "CodecState";};
	
		uint8_t getSdpMediaType(){ return codec->getSdpMediaType(); };

		void setCodec( MRef<Codec *> c ){ codec = c; };
	
		MRef<Codec*> getCodec(){return codec;}
		
	private:
		MRef<Codec *> codec;
		
};


class AudioCodec : public Codec{
	public:
		/**
		 * size of the output of the codec in bytes.
		 * Returns -1 if output size may vary.
		 */
//		virtual int32_t getEncodedNrBytes()=0;//
		
		virtual int32_t getInputNrSamples()=0;
	
		/**
		 * @return Requested sampling freq for the CODEC
		 */
		virtual int32_t getSamplingFreq()=0;

		/**
		 * Time in milliseconds to put in each frame/packet
		 */
		virtual int32_t getSamplingSizeMs()=0;
		
		//virtual std::string getMemObjectType(){return "AudioCodec";}

		virtual std::string getPluginType()const { return "AudioCodec"; }		

	protected:
		AudioCodec( MRef<Library *> lib ): Codec( lib ) {}
		AudioCodec(): Codec() {}
};

/** Registry of audio codec plugins */
class AudioCodecRegistry: public MPluginRegistry, public MSingleton<AudioCodecRegistry>{
	public:
		virtual std::string getPluginType(){ return "AudioCodec"; }

		/**
		 * @returns A CODEC state for the given payloadType
		 * (NULL if not handled)
		 */
		MRef<CodecState *> createState( uint8_t payloadType );
		
                /**
		 * @returns A CODEC instance for the given description string
		 * (NULL if not handled)
		 */
		MRef<AudioCodec *> create( const std::string& );

	protected:
		AudioCodecRegistry();

	private:
		friend class MSingleton<AudioCodecRegistry>;
};

#endif
