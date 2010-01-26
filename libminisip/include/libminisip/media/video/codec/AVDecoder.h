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

#ifndef AVDECODER_H
#define AVDECODER_H

//extern "C"{
//#include<avcodec.h>
//}

#include<libmutil/MemObject.h>
#include<string>

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/ImageHandler.h>

class ImageHandler;
class MImage;

struct AVCodec;
struct AVCodecContext;
struct AVFrame;

class LIBMINISIP_API AVDecoder : public MObject{
	public:
		AVDecoder();

		//void init( uint32_t width, uint32_t height );

		void setHandler( ImageHandler * handler );

		void decodeFrame( uint8_t * data, uint32_t length );

		virtual std::string getMemObjectType() const { return "AVDecoder";};

		void close();

		void setSsrc( uint32_t ssrc );

	private:
		static void ffmpegReleaseBuffer( struct AVCodecContext * context, AVFrame * frame );
		static int ffmpegGetBuffer( struct AVCodecContext * context, AVFrame * frame );
		ImageHandler * handler;
		MImage * lastImage;

		AVCodec * codec;
		AVCodecContext * context;

		uint32_t ssrc;

		bool needsConvert;

		void* swsctx;
		int N;

};

#endif
