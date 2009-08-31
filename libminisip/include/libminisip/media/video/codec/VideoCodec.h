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

#ifndef AV_CODEC_H
#define AV_CODEC_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/codecs/Codec.h>

class RealtimeMediaStreamSender;
class AVEncoder;
class AVDecoder;
class Grabber;
class VideoDisplay;
class VideoEncoderCallback;

typedef uint8_t byte_t;

class LIBMINISIP_API MVideoCodec : public Codec{
	public:

		MRef<CodecState *> newInstance(){/*TODO*/ return NULL;};
		MVideoCodec();

		/* called by the Grabber directly */
		void encode( MRef<MImage *> image );

		void decode( byte_t * inputData, uint32_t inputSize );

		virtual std::string getCodecName();
		virtual std::string getCodecDescription();
		virtual uint8_t getSdpMediaType();
		virtual std::string getSdpMediaAttributes();

		void setGrabber( MRef<Grabber *> grabber );
		void setDisplay( MRef<VideoDisplay *> display );
		void setEncoderCallback( VideoEncoderCallback * cb );

		void startSend( uint32_t width, uint32_t height );
		void stopSend();

		virtual std::string getMemObjectType() const {return "MVideoCodec";}

		virtual std::string getPluginType()const{return "MVideoCodec";}

		virtual uint32_t getVersion() const;

		virtual MRef<AVEncoder*> getEncoder();

	private:

		MRef<RealtimeMediaStreamSender *> senders;
		MRef<AVEncoder *> coder;
		MRef<AVDecoder *> decoder;

		MRef<Grabber *> grabber;
		MRef<VideoDisplay *> display;
};

#endif
