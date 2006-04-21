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

#include<libminisip/video/codec/AVCoder.h>
#include<sys/time.h>
#include<libmnetutil/IP4Address.h>
#include<libmutil/mtime.h>
#include<libminisip/video/codec/VideoEncoderCallback.h>
//#include<libminisip/rtp/RtpPacket.h>

#include<config.h>
#include<stdio.h>
#include<fcntl.h>
#include<iostream>
#include<libmutil/print_hex.h>

using namespace std;
#define AVCODEC_MAX_VIDEO_FRAME_SIZE (3*1024*1024)


void rtpCallback( struct AVCodecContext * context, void *data,
	          int size, int packetNumber ){

	MRef<AVEncoder *> encoder = (AVEncoder *)context->opaque;

//        fprintf( stderr, "RTP payload size: %i\n", size );
	
        /*
	encoder->rtpPayload[1] |= 
		( ( context->coded_frame->pict_type == FF_P_TYPE )?1:0 << 4 );
*/
	uint32_t ts = (uint32_t) ( 90.0 * context->coded_frame->pts ) / 1000;
	
//	memcpy( encoder->rtpPayload + 4, data, size );

        encoder->mbCounter += packetNumber;
        bool endOfFrame = ( encoder->mbCounter == (uint32_t)
                        ((context->width+15)/16)*((context->height+15)/16) );
#if 0
        cerr << "NB_MACROBLOCK: " << packetNumber << endl;
        cerr << "MACROBLOCK_COUNTER: " << encoder->mbCounter << endl;
        cerr << "PACKET_SIZE: " << size << endl;
        cerr << "WIDTH: " << context->width << endl;
        cerr << "HEIGHT: " << context->height << endl;
#endif


        if( endOfFrame ){
        //        fprintf( stderr, "End of frame, length = %i\n", size );
                encoder->mbCounter = 0;
        }
        
        /* RFC2429 payload header
         * RR = 0
         * P = 1
         * V = 0
         * PLEN = 0
         * PEBIT = 0
         */
        ((unsigned char *)data)[0] = 4;
        //data[1] = 0;

	if( encoder->getCallback() ){
		//encoder->getCallback()->sendVideoData( encoder->rtpPayload, size + 4, ts, endOfFrame );	
		encoder->getCallback()->sendVideoData( (unsigned char *)data, size, ts, endOfFrame );	
	}
}


AVEncoder::AVEncoder():context( NULL ),codec( NULL ){
	/* Initialize AVcodec */
	avcodec_init();
	avcodec_register_all();

	codec = avcodec_find_encoder( CODEC_ID_H263P );

	if( codec == NULL ){
		fprintf( stderr, "libavcodec does not support H263" );
		exit( 1 );
	}

	context = avcodec_alloc_context();

	context->dsp_mask = ( FF_MM_MMX | FF_MM_MMXEXT | FF_MM_SSE );

	context->bit_rate = 1000000;
	context->bit_rate_tolerance = 2*1024*1024;

#ifndef AVCODEC_FIXES
	context->frame_rate = 15; 
	context->frame_rate_base = 1;
#else
	AVRational timeBase = { 1, 15 };
	context->time_base = timeBase;
	context->pix_fmt = PIX_FMT_YUV420P;
#endif
        context->flags |= CODEC_FLAG_QP_RD;
        context->mb_decision = FF_MB_DECISION_RD;
        context->rc_max_rate = 1000000;
        context->rc_min_rate = 1000000;
        context->rc_buffer_size = 10000;


	context->rtp_mode = 1;
	context->rtp_payload_size = 1000;
	context->rtp_callback = &rtpCallback;


	/* from ffmpeg */
	context->qblur = 0.5;
        context->qcompress = 0.5;
        context->b_quant_offset = 1.25;
        context->b_quant_factor = 1.25;
        context->i_quant_offset = 0.0;
        context->i_quant_factor = -0.8;

        //context->qmin = 0;
        //context->mb_qmin = 0;
//        context->qmax = 5;
        //context->mb_qmax = 10;
//        context->flags |= CODEC_FLAG_QP_RD;
        context->flags |= CODEC_FLAG_H263P_SLICE_STRUCT;
	
	context->gop_size = 1;

	context->thread_count = 1;

	context->opaque = this;

	/* H263 RTP header mode A (see RFC2190) */
	rtpPayload[1] = 0x60;

        mbCounter = 0;
}

void AVEncoder::init( uint32_t width, uint32_t height ){
	fprintf(stderr, "Opening coder with width: %i\n", width );
	fprintf(stderr, "Opening coder with height: %i\n", height);
	context->width =  width;//width;
	context->height = height; //height;

	if( avcodec_open( context, codec ) != 0 ){
		fprintf( stderr, "Could not open libavcodec codec\n" );
		exit( 1 );
	}

}

void AVEncoder::close(){
	avcodec_close( context );
}

void AVEncoder::handle( MImage * image ){
	int ret;
	AVFrame frame;
	bool mustFreeFrame = false;
	
	if( image->chroma != M_CHROMA_I420 ){
		PixelFormat srcFormat;
		
		switch( image->chroma ){
			case M_CHROMA_RV32:
				srcFormat = PIX_FMT_RGBA32;
				break;
			case M_CHROMA_RV24:
				srcFormat = PIX_FMT_BGR24;
				break;
			default:
				/* FIXME: handle other formats */
				srcFormat = PIX_FMT_RGBA32;
				break;
		}

		/* Truncate the source image to fit in the CIF format */
		//image.linesize[0] = 356 * 3;
		
		/* We will need a convertion */
		avpicture_alloc( (AVPicture*)&frame, 
			context->pix_fmt, context->width,
			context->height );

		/* We must free frame ourselves */
		mustFreeFrame = true;
		
		/* Convert to the desired type (plannar YUV 420 ) */
		if( img_convert( (AVPicture*)&frame, context->pix_fmt, 
					(AVPicture*)image, srcFormat,
					context->width, context->height ) < 0 ){
			fprintf( stderr, "Could not convert image to"
					 "encoding format\n");
			exit( 1 );
		}
	}
	else{
		/* We can use the picture as is */
		memcpy( &frame, image, sizeof( MData ) );
	}

	frame.pict_type = 0;
        if( !image->mTime ){
                frame.pts = mtime() * 1000;
        }
        else{
                frame.pts = image->mTime * 1000;
        }
        //frame.pts = AV_NOPTS_VALUE;
//        fprintf( stderr, "PTS: %i\n", frame.pts);

	ret = avcodec_encode_video( context, outBuffer+4,
			            AVCODEC_MAX_VIDEO_FRAME_SIZE, &frame );

	if( mustFreeFrame ){
		avpicture_free( (AVPicture*)&frame );
	}
}

uint32_t AVEncoder::getRequiredWidth(){
	return context->width;
}

uint32_t AVEncoder::getRequiredHeight(){
	return context->height;
}
	
	
MImage * AVEncoder::provideImage(){
	return NULL;
}

bool AVEncoder::providesImage(){
	return false;
}

void AVEncoder::releaseImage( MImage * image ){
}

bool AVEncoder::handlesChroma( uint32_t chroma ){
	return (chroma == M_CHROMA_RV24) || (chroma == M_CHROMA_I420);
}
