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

#ifndef AV_CODEC_H
#define AV_CODEC_H

#include"../ImageHandler.h"
#include"../../codecs/Codec.h"

class MediaStreamSender;
class AVEncoder;
class AVDecoder;
class Grabber;
class VideoDisplay;
class VideoEncoderCallback;

typedef uint8_t byte_t;

class VideoCodec : public Codec{
	public:

		VideoCodec();

		/* called by the Grabber directly */
		void encode( MRef<MImage *> image );

		void decode( byte_t * inputData, uint32_t inputSize );

		virtual std::string getCodecName();
		virtual std::string getCodecDescription();
		virtual int32_t getSdpMediaType();
		virtual std::string getSdpMediaAttributes();

		void setGrabber( MRef<Grabber *> grabber );
		void setDisplay( MRef<VideoDisplay *> display );
		void setEncoderCallback( VideoEncoderCallback * cb );

		void startSend( uint32_t width, uint32_t height );
		void startReceive( uint32_t width, uint32_t height );
		void stopReceive();
		void stopSend();





		virtual std::string getMemObjectType(){return "AVCodec";}

	private:

		MRef<MediaStreamSender *> senders;
		MRef<AVEncoder *> coder;
		MRef<AVDecoder *> decoder;

		MRef<Grabber *> grabber;
		MRef<VideoDisplay *> display;
};


#endif
