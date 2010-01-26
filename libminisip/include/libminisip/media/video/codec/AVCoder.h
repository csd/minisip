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

#ifndef AV_CODER_H
#define AV_CODER_H
#define HAVE_MMX

#include<libminisip/libminisip_config.h>

#include<string>

#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/video/display/VideoDisplay.h>

#include<libmutil/MemObject.h>



extern "C"{
#include<video_codec.h>
//#include<avcodec.h>
}
#define AVCODEC_MAX_VIDEO_FRAME_SIZE (3*1024*1024)

typedef uint8_t byte_t;

class VideoEncoderCallback;
//struct Video;

class LIBMINISIP_API AVEncoder: public ImageHandler, public MObject{
	public:
		AVEncoder();
		virtual void handle( MImage * image );
		virtual void init( uint32_t height, uint32_t width );
		virtual MImage * provideImage();
		virtual void releaseImage( MImage * );
		virtual bool providesImage();
		virtual bool handlesChroma( uint32_t chroma );

		virtual std::string getMemObjectType() const {return "AVEncoder";};

		virtual uint32_t getRequiredWidth();
		virtual uint32_t getRequiredHeight();

		virtual void resize(int width, int height);
		
		uint8_t rtpPayload[10000];
                uint32_t mbCounter;

		VideoEncoderCallback * getCallback(){return callback;};
		void setCallback( VideoEncoderCallback * cb ){this->callback = cb;};

		void setLocalDisplay(MRef<VideoDisplay*>);

		void close();

	private:
		/*VideoCodec*/ void *videoCodec;
		/*Video*/ void *video;

		void make_h264_header(unsigned char *buf, int packetization_mode, unsigned char nal_unit_octet, int frag_start, int frag_end);
		void hdviper_h264_packetize_nal_unit(Video *v, unsigned char *h264_data, int size, int timecode, int last_nal_unit_of_frame);
		void hdviper_h264_packetize(Video *v, int timecode);


		VideoEncoderCallback * callback;
	 	byte_t outBuffer[AVCODEC_MAX_VIDEO_FRAME_SIZE];

		MRef<VideoDisplay*> localDisplay;

		void *swsctx;
		int N;
};


#endif
