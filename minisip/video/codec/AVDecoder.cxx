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

#include<config.h>
#include"AVDecoder.h"
#include"../ImageHandler.h"



/* used by ffmpeg to get a frame from the ImageHandler */

int AVDecoder::ffmpegGetBuffer( struct AVCodecContext * context, AVFrame * frame ){
	AVDecoder * decoder = (AVDecoder*)context->opaque;
	
	/* get an image from the handler */
	decoder->lastImage = decoder->handler->provideImage();
	
	frame->pts = 0;
	frame->type = FF_BUFFER_TYPE_USER;

	/* the mimage data[] and linesize[] can be copied */
	memcpy( frame, decoder->lastImage, sizeof( MData ) );

	frame->age = 256*256*256*64; // FIXME

	return 0;
}

void AVDecoder::ffmpegReleaseBuffer( struct AVCodecContext * context, AVFrame * frame ){
	memset( frame, '\0', sizeof( MImage ) );
}

AVDecoder::AVDecoder():codec( NULL ), context( NULL ),handler(NULL){

	/* Initialize AVcodec */
	avcodec_init();
	avcodec_register_all();

	codec = avcodec_find_decoder( CODEC_ID_H263 );

	if( codec == NULL ){
		fprintf( stderr, "libavcodec does not support H263" );
		exit( 1 );
	}

	context = avcodec_alloc_context();

#ifdef HAVE_MMX
	context->dsp_mask = ( FF_MM_MMX | FF_MM_MMXEXT | FF_MM_SSE );
#endif

	if( avcodec_open( context, codec ) != 0 ){
		fprintf( stderr, "Could not open libavcodec codec\n" );
		exit( 1 );
	}
	
	context->opaque = this;

}

void AVDecoder::close(){
//	avcodec_close( context );
}

void AVDecoder::setHandler( ImageHandler * handler ){
	this->handler = handler;

	/* If the handler provides its own buffers, use them */
	if( handler->providesImage() ){
		context->get_buffer = &ffmpegGetBuffer;
		context->release_buffer = &ffmpegReleaseBuffer;
	}
}


void AVDecoder::decodeFrame( uint8_t * data, uint32_t length ){

	int ret;
	AVFrame * decodedFrame;
	int gotFrame = 0;

	decodedFrame = avcodec_alloc_frame();
	
	ret = avcodec_decode_video( context, decodedFrame,
			&gotFrame, data, length );

	if( gotFrame ){
		/* send to the handler */
		if( handler ){
	//		handler->handle( rgbPicture.data[0] );
			handler->handle( lastImage );
		}

	}
}	
