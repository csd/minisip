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

#include<libminisip/media/video/codec/AVCoder.h>
#include<sys/time.h>
#include<libmutil/mtime.h>
#include<libmutil/Timestamp.h>
#include<libminisip/media/video/codec/VideoEncoderCallback.h>
#include<libminisip/media/video/VideoException.h>
//#include<libminisip/media/rtp/RtpPacket.h>

#include<config.h>
#include<stdio.h>
#include<fcntl.h>
#include<iostream>
#include<string.h>


extern "C"{
	#include<video_codec.h>
	#include<avcodec.h>
	#include<swscale.h>
}

#include<math.h>
#include<sys/time.h>

#include<swscale.h>



#define H264_SINGLE_NAL_UNIT_PACKET 0
#define H264_AGGREGATION_PACKET 1
#define H264_FRAGMENTATION_UNIT 2

//RTP header length set to zero since it should not be added here
//(SrtpPacket handles it).
#define RTP_HEADER_LEN 0 
#define HDVIPER_RTP_VER 2

#define HDVIPER_RTP_PT_PCMU 0
#define HDVIPER_RTP_PT_PCMA 8
#define HDVIPER_RTP_PT_SPEEX 107
#define HDVIPER_RTP_PT_H264 119
#define HDVIPER_RTP_PT_H263 120
#define HDVIPER_RTP_PT_JPEG 26

#define RTP_PAYLOAD_LEN 1400


using namespace std;


AVEncoder::AVEncoder() {
	video=NULL;
	videoCodec=NULL;
	swsctx=NULL;
	N=0;


}

#ifdef GLOBAL_BANDWIDTH_HACK
extern volatile int globalBitRate;
#endif

void AVEncoder::init( uint32_t width, uint32_t height ){
	cerr << "=========================== doing AVEncoder::init============================"<<endl;
	cerr << "AVEncoder::init("<<width<<","<<height<<endl;
	if (width>4000||height>4000){
		width=1280;
		height=720;
	}

	Video *video = (Video*)this->video;
	VideoCodec *videoCodec = (VideoCodec*)this->videoCodec;

	if (video){
		delete video;
		video=NULL;
	}
	if (videoCodec){
		hdviper_destroy_video_encoder(videoCodec);
		delete videoCodec;
		videoCodec=NULL;
	}

	this->video = video = new Video;
	this->videoCodec = videoCodec = new VideoCodec;

	avcodec_init(); //perhaps needed for img_convert?

	video->width=width;
	video->height=height;
	video->fps=30;

#ifdef GLOBAL_BANDWIDTH_HACK
	if (globalBitRate<128)
		globalBitRate=128;
	if (globalBitRate>15000)
		globalBitRate=25000;
	videoCodec->bitrate=globalBitRate;
#else
	videoCodec->bitrate=6000;
#endif


	hdviper_setup_video_encoder(videoCodec, VIDEO_CODEC_H264, video);

	video->yuv=(unsigned char *)malloc(sizeof(unsigned char)*video->width*video->height*3/2);
	video->compressed=(char *)malloc(sizeof(unsigned char)*videoCodec->bitrate*1000/8);

//	if (swsctx) //make handle() allocate a new context with the new dimensions and format
//		sws_freeContext( (struct SwsContext*)swsctx );
}

void AVEncoder::close(){
	cerr << "=========================== doing AVEncoder::close============================"<<endl;
	VideoCodec *videoCodec = (VideoCodec*)this->videoCodec;
	if (videoCodec)
		hdviper_destroy_video_encoder(videoCodec);
}

void AVEncoder::setLocalDisplay(MRef<VideoDisplay*> d){
	localDisplay=d;
}

#define REPORT_N 50
#if 0

#endif


void AVEncoder::handle( MImage * image ){

	massert(image);
	//cerr << "EEEE: AVEncoder::handle image of size "<< image->width<<"x"<<image->height<<endl;

#if 1 //FPS measurement
	static struct timeval lasttime;

	static int i=0;
	//	static int N=0;
	i++;

#ifdef GLOBAL_BANDWIDTH_HACK
	static int lastBitRate=-1;
	if (lastBitRate==-1)
		lastBitRate=globalBitRate;

	if (globalBitRate!=lastBitRate){
		cerr <<"EEEE: bitrate changed. ============================="<<endl;
		init(image->width, image->height);
		lastBitRate=globalBitRate;

	}
#endif

	Video *video = (Video*)this->video;

//	if (i%7!=0)
//		return;
	N++;

        if (N%REPORT_N==0){
                struct timeval now;
                gettimeofday(&now, NULL);
                int diffms = (now.tv_sec-lasttime.tv_sec)*1000+(now.tv_usec-lasttime.tv_usec)/1000;
                float sec = (float)diffms/1000.0f;
                printf("%d frames in %fs\n", REPORT_N, sec);
                printf("FPS_ENCODE: %f for video of size %dx%d\n", (float)REPORT_N/(float)sec, video->width, video->height );
                lasttime=now;
        }
#endif


	VideoCodec *videoCodec = (VideoCodec*)this->videoCodec;

	if (!video || image->width!=video->width || image->height!=video->height){
		//cerr << "EEEE: AVCoder: doing init("<<image->width<<","<<image->height<<")"<<endl;
		init(image->width, image->height);
		video = (Video*)this->video;
		videoCodec = (VideoCodec*)this->videoCodec;
	}

	int ret;
	AVFrame frame;
	bool mustFreeFrame = false;

	if( image->chroma != M_CHROMA_I420 ){
		PixelFormat srcFormat;

		switch( image->chroma ){
		case M_CHROMA_RV32:
			srcFormat = PIX_FMT_RGB32;
			break;
		case M_CHROMA_RV24:
			srcFormat = PIX_FMT_BGR24;
			break;
		default:
			/* FIXME: handle other formats */
			srcFormat = PIX_FMT_RGB32;
			break;
		}

		/* We will need a convertion */
		avpicture_alloc( (AVPicture*)&frame, 
				PIX_FMT_YUV420P, image->width,
				image->height );
		//cerr << "EEEE: allocated picture of dim "<<video->width<<"x"<<video->height<<endl;

		/* We must free frame ourselves */
		mustFreeFrame = true;

		if (!swsctx)
			swsctx = sws_getContext(image->width, image->height, srcFormat, image->width, image->height, PIX_FMT_YUV420P,SWS_FAST_BILINEAR, NULL,NULL,NULL);
		struct SwsContext* ctx = (struct SwsContext*)swsctx;

		sws_scale( ctx, image->data, image->linesize, 0, image->height, frame.data, frame.linesize);
//		yuv2rgb(image->data[0], image->data[1], image->data[2], image->width, image->height, frame.data[0]);
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

	//Y
	massert(frame.linesize[0] == (video->width));
	memcpy(&(video->yuv[0]),frame.data[0], video->width*video->height);

	//U
	massert(frame.linesize[1] == video->width/2);
	memcpy(&(video->yuv[ video->width*video->height ]), frame.data[1], video->width/2*video->height/2);

	//V
	massert(frame.linesize[2] == video->width/2);
	memcpy(&video->yuv[ video->width*video->height + video->width/2*video->height/2 ], frame.data[2], video->width/2*video->height/2 );

	video->size_out=0;
	hdviper_video_encode(videoCodec, video);

	static int rtp_ts=0;
	rtp_ts+=3600;

	hdviper_h264_packetize(video, rtp_ts);

	if (mustFreeFrame){
		avpicture_free( (AVPicture*)&frame );
	}

	//printSnd();
}

int h264_start_code(unsigned char* p){
	if (p[0]==0 && p[1]==0 && p[2]==1){
		return p[3]&0x1F;
	}
	return 0;
}


void AVEncoder::make_h264_header(unsigned char *buf, int packetization_mode, unsigned char nal_unit_octet, int frag_start, int frag_end) {
	unsigned char fu_ind, fu_head;

	switch(packetization_mode) {
	case H264_SINGLE_NAL_UNIT_PACKET:
		*buf = nal_unit_octet;
		break;
	case H264_AGGREGATION_PACKET:
		*buf = nal_unit_octet;
		massert(1==0);
		// to be implemented...
		break;
	case H264_FRAGMENTATION_UNIT:
		fu_ind = (nal_unit_octet & 224) | 28; /* F and NRI bits from NAL octet + FU-A identifier */
		*buf = fu_ind;
		fu_head = (nal_unit_octet & 31) | (frag_start << 7) | (frag_end << 6); /* Type bits from NAL octet */
		*(buf+1) = fu_head;
		break;
	}

}

void AVEncoder::hdviper_h264_packetize_nal_unit(Video *v, unsigned char *h264_data, int size, int timecode, int last_nal_unit_of_frame) {
	unsigned char nal_unit_octet;
	int video_header_size, dgm_payload_size, first_dgm_payload_size, rest, dgms, lastsize, cnt, npad, i;
	unsigned char *video_dgm_ptr;

	/* Does video data + headers fit in one packet? */
	if(RTP_PAYLOAD_LEN >= size + RTP_HEADER_LEN) {
		/* Single unit NAL case */
		video_header_size = RTP_HEADER_LEN+1; /* note that header byte is also first byte of packet */
		lastsize = dgm_payload_size = first_dgm_payload_size = size;
		dgms = 1;
	} else {
		/* Fragmentation unit case */			
		video_header_size = RTP_HEADER_LEN+2; /* 2 bytes for FU indicator and FU header */
		dgm_payload_size = VIDEO_DGM_LEN - video_header_size;
		first_dgm_payload_size = dgm_payload_size+1; /* because NAL unit octet is in FU header */
		rest = size - first_dgm_payload_size;
		if(rest<0) rest = 0;
		dgms = 1;
		lastsize = rest%dgm_payload_size;
		dgms += (rest/dgm_payload_size) + 
			( (lastsize>0) ? 1 : 0 );
	}

	nal_unit_octet = *h264_data;

	for(i=0; i<dgms; i++) {
		/* 
		 * Set pointer to this dgm's data, minus header space needed
		 */ 
		if(i==0)
			/* The +1 below is to overwrite the NAL unit octet with the header, because the
			 * NAL unit octet is transmitted as part of the header */
			video_dgm_ptr = h264_data - video_header_size +1; /* Yes, there is room before h264_data */
		else
			video_dgm_ptr = h264_data + (i-1)*dgm_payload_size + first_dgm_payload_size - video_header_size;

		/*
		 * Make RTP header and set cnt to number of bytes in dgm (excluding header)
		 */
		if(i==dgms-1) {
			/* Last packet (which could also be the first...) */
			/* Last packet, no padding */
			cnt = lastsize;
			if(cnt==0) cnt = dgm_payload_size;
#if RTP_HEADER_LEN>0
			make_rtp_header(video_dgm_ptr, HDVIPER_RTP_VER, 0, 0, 0, last_nal_unit_of_frame, HDVIPER_RTP_PT_H264, tick_sequence_nr(&v->rtp_seq), timecode, v->ssrc, 0);
#endif
		} else {
			/* Not last packet (could be the first) */
			cnt = dgm_payload_size;
#if RTP_HEADER_LEN>0
			make_rtp_header(video_dgm_ptr, HDVIPER_RTP_VER, 0, 0, 0, 0, HDVIPER_RTP_PT_H264,
					tick_sequence_nr(&v->rtp_seq), timecode, v->ssrc, 0);
#endif
		}

		/*
		 * Make profile specific header
		 */

		if(dgms==1) {

			/* Single NAL unit packet */
			make_h264_header(video_dgm_ptr+RTP_HEADER_LEN, H264_SINGLE_NAL_UNIT_PACKET, nal_unit_octet, 0, 0);

			///send_rtp_packet((void *)video_dgm_ptr, cnt+video_header_size-1);
			if( getCallback() ){
				getCallback()->sendVideoData( video_dgm_ptr, cnt+video_header_size-1, timecode, /*last_nal_unit_of_frame*/ true);   
			}

		} else {
			/* Fragmentation units */
			if(i==dgms-1 && i!=0) {
				/* Last packet of fragmentation unit */
				make_h264_header(video_dgm_ptr+RTP_HEADER_LEN, H264_FRAGMENTATION_UNIT, nal_unit_octet, 0, 1);
			} else {

				if(i==0) {
					/* First packet of fragmentation unit */
					make_h264_header(video_dgm_ptr+RTP_HEADER_LEN, H264_FRAGMENTATION_UNIT, nal_unit_octet, 1, 0);
				} else {
					/* Neither first nor last packet of a fragmentation unit */
					make_h264_header(video_dgm_ptr+RTP_HEADER_LEN, H264_FRAGMENTATION_UNIT, nal_unit_octet, 0, 0);
				}
			}
			///send_rtp_packet((void *)video_dgm_ptr, cnt+video_header_size);
			if( getCallback() ){
				getCallback()->sendVideoData( video_dgm_ptr, cnt+video_header_size, timecode, /*(i==dgms-1) && last_nal_unit_of_frame*/ i==dgms-1 );
			}

		}
	}
}

void AVEncoder::hdviper_h264_packetize(Video *v, int timecode) {
	unsigned char *p, *nal_unit_ptrs[500];
	int s, n=0, i, size, leftsize;

	if(v->size_out==0)
		return;

	p = (unsigned char*)v->compressed;
	while( p < (unsigned char *)(v->compressed + v->size_out) && n<500) {
		s = h264_start_code(p);
		if(s!=0) { 
			/* NAL start code found */
//			printf("h264_packetize: found H.264 start code = %d at index=%d\n", s,(int)((char*)p - (char*)v->compressed));
			p+=3;
			nal_unit_ptrs[n++] = p;
		}
		p++;
	}
	if(n==0) return;
	leftsize = v->size_out;
	for(i=0; i<n-1; i++) {
		size = nal_unit_ptrs[i+1]-nal_unit_ptrs[i]-3; // -3 because of start code
		hdviper_h264_packetize_nal_unit(v, nal_unit_ptrs[i], size, timecode, 0);
		leftsize -= size;
	}
	leftsize=v->size_out - ((char*)nal_unit_ptrs[n-1] - v->compressed);
	hdviper_h264_packetize_nal_unit(v, nal_unit_ptrs[n-1], leftsize, timecode, 1);

}



uint32_t AVEncoder::getRequiredWidth(){
	Video *video = (Video*)this->video;
	return video->width;
}

uint32_t AVEncoder::getRequiredHeight(){
	Video *video = (Video*)this->video;
	return video->height;
}

void AVEncoder::resize(int w, int h){
	init(w,h); //FIXME: init does not free any previously allocated resources
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
