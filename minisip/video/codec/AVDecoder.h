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

#ifndef AVDECODER_H
#define AVDECODER_H
#include<libavcodec/avcodec.h>
#include<libmutil/MemObject.h>
#include<string>

#include"../ImageHandler.h"

class ImageHandler;
class MImage;




class AVDecoder : public MObject{
	public:
		AVDecoder();

		void init( uint32_t width, uint32_t height );

		void setHandler( ImageHandler * handler );

		void decodeFrame( uint8_t * data, uint32_t length );

		virtual std::string getMemObjectType(){ return "AVDecoder";};

		void close();

	private:
		static void ffmpegReleaseBuffer( struct AVCodecContext * context, AVFrame * frame );
		static int ffmpegGetBuffer( struct AVCodecContext * context, AVFrame * frame );
		ImageHandler * handler;
		MImage * lastImage;

		AVCodec * codec;
		AVCodecContext * context;

};

#endif
