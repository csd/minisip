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

#include<config.h>
#include<libminisip/media/video/codec/AVDecoder.h>
#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/video/VideoException.h>

#include<iostream>
#include<string.h>


#include <sys/time.h>
#include <time.h>


extern "C"{
#include<avcodec.h>
#include<swscale.h>
}



using namespace std;

/* used by ffmpeg to get a frame from the ImageHandler */

int AVDecoder::ffmpegGetBuffer( struct AVCodecContext * context, AVFrame * frame ){
	AVDecoder * decoder = (AVDecoder*)context->opaque;
	
	/* get an image from the handler */
	MImage * image = decoder->handler->provideImage( decoder->ssrc );
	if( image ){
		decoder->lastImage = image;
	}
	
	frame->pts = 0;
	frame->type = FF_BUFFER_TYPE_USER;

	/* the mimage data[] and linesize[] can be copied */
	memcpy( frame, decoder->lastImage, sizeof( MData ) );

	frame->age = 256*256*256*64; // FIXME

	return 0;
}

void AVDecoder::ffmpegReleaseBuffer( struct AVCodecContext * context, AVFrame * frame ){
	//AVDecoder * decoder = (AVDecoder*)context->opaque;
	memset( frame, '\0', sizeof( MImage ) );
	// XXX: is this still valid code?
	//decoder->lastImage = NULL;
}

AVDecoder::AVDecoder():handler(NULL),codec( NULL ),context( NULL ){

	swsctx=NULL;
	/* Initialize AVcodec */
	avcodec_init();
	avcodec_register_all();

	codec = avcodec_find_decoder( CODEC_ID_H264 );

	if( codec == NULL ){
		cerr << "EEEE: Error: libavcodec does not support H264"<<endl;
		throw VideoException( "libavcodec does not support H264" );
	}

	context = avcodec_alloc_context();

#ifdef HAVE_MMX
	context->dsp_mask = ( FF_MM_MMX | FF_MM_MMXEXT | FF_MM_SSE );
#endif

	if( avcodec_open( context, codec ) != 0 ){
		throw VideoException( "Could not open libavcodec codec" );
	}
	
	context->opaque = this;
	lastImage = NULL;

}

void AVDecoder::close(){
	//if( lastImage ){
	//	handler->handle( lastImage );
	//}
//	avcodec_close( context );
}

void AVDecoder::setHandler( ImageHandler * handler ){
	this->handler = handler;
        
        needsConvert = handler && ! handler->handlesChroma( M_CHROMA_I420 );

	/* If the handler provides its own buffers, use them */
	if( handler && !needsConvert && handler->providesImage() ){
		context->get_buffer = &ffmpegGetBuffer;
		context->release_buffer = &ffmpegReleaseBuffer;
	}

}

#define REPORT_N 50

void AVDecoder::decodeFrame( uint8_t * data, uint32_t length ){

//	cerr <<"EEEE: doing AVDecoder::decodeFrame len="<<length<<endl;
        static struct timeval lasttime;
        static int i=0;
        i++;
        if (i%REPORT_N==1){
                struct timeval now;
                gettimeofday(&now, NULL);
                int diffms = (now.tv_sec-lasttime.tv_sec)*1000+(now.tv_usec-lasttime.tv_usec)/1000;
                float sec = (float)diffms/1000.0f;
                printf("%d frames in %fs\n", REPORT_N, sec);
                printf("FPS_DECODE: %f\n", (float)REPORT_N/(float)sec );
                lasttime=now;
        }

	int ret;
	AVFrame * decodedFrame=NULL;
	int gotFrame = 0;

	decodedFrame = avcodec_alloc_frame();

	ret = avcodec_decode_video( context, decodedFrame,
			&gotFrame, data, length );


	if( gotFrame ){
		/* send to the handler */
		if( handler ){
			if (context->width!=handler->getRequiredWidth() || context->height!=handler->getRequiredHeight()){
				mdbg<<"AVDecoder trying to resize window"<<endl;
				handler->resize(context->width, context->height);
				swsctx=NULL;
			}
                        if( needsConvert ){
                               PixelFormat ffmpegFormat;

                               MImage * converted = NULL;
                               if( handler->providesImage() ){
                                       converted = handler->provideImage();
                               }

                               //else ...
                               //

                               switch( converted->chroma ){
                                       case M_CHROMA_RV16:
                                               ffmpegFormat = PIX_FMT_RGB565;
                                               break;
                                       case M_CHROMA_RV32:
                                               ffmpegFormat = PIX_FMT_RGBA32;
					       break;
                                       case M_CHROMA_RV24:
                                       default:
                                               ffmpegFormat = PIX_FMT_RGB24;
                                               break;
                               }
#if 0
                               img_convert( (AVPicture *)converted,
                                            ffmpegFormat,
                                            (AVPicture *)decodedFrame, 
                                            PIX_FMT_YUV420P,
                                            handler->getRequiredWidth(),
                                            handler->getRequiredHeight() );
#else
			       if (!swsctx)
				       swsctx =  sws_getContext(context->width, context->height, PIX_FMT_YUV420P, context->width, context->height, ffmpegFormat,SWS_FAST_BILINEAR, NULL,NULL,NULL);
			       struct SwsContext* ctx = (struct SwsContext*)swsctx;
			       sws_scale( ctx, decodedFrame->data, decodedFrame->linesize, 0, context->height, converted->data, converted->linesize);
#endif
//			       cerr <<"EEEE: AVDecoder: decoded frame "<<endl;
			       handler->handle( converted );
                        }
                        
                        else{
//			       cerr <<"EEEE: AVDecoder: decoded frame "<<endl;
                                handler->handle( lastImage );
                        }
			lastImage = NULL;
		}

	}

	if (i%200==0){
		static struct timespec last_wallt;
		static struct timespec last_cput;

		struct timespec now_cpu;
		struct timespec now_wall;
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now_cpu);
		clock_gettime(CLOCK_REALTIME, &now_wall);
		cerr <<"=======> AVDecoder:: CPU USAGE: "<< now_cpu.tv_sec<<"."<<now_cpu.tv_nsec<<endl;
		uint64_t delta_cpu = (now_cpu.tv_sec-last_cput.tv_sec)*1000000000LL+(now_cpu.tv_nsec-last_cput.tv_nsec);
		cerr << "Last 200 frames took "<< delta_cpu/1000 <<"us"<<endl;
		uint64_t delta_wall = (now_wall.tv_sec-last_wallt.tv_sec)*1000000000LL+(now_wall.tv_nsec-last_wallt.tv_nsec);

		last_cput  = now_cpu;
		last_wallt = now_wall;
		cerr <<"========> AVDecoder CPU usage: "<< ((float)delta_cpu/(float)delta_wall)*100.0<<"%"<<endl;
	}

}	

void AVDecoder::setSsrc( uint32_t ssrc ){
	this->ssrc = ssrc;
}
