/*
 Copyright (C) 2010 Erik Eliasson
 
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

/* Copyright (C) 2010
 *
 * Authors: Erik Eliasson <ere@kth.se>
*/

#include<config.h>

#include"DeckLinkGrabber.h"
#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/video/VideoMedia.h>
#include<libminisip/media/video/VideoException.h>
#include<stdio.h>
#include<errno.h>
#include<libmutil/mtime.h>
#include<libmutil/stringutils.h>
#include<libmutil/merror.h>
#include<time.h>

using namespace std;

static std::list<std::string> pluginList;
static bool initialized;

extern "C" LIBMINISIP_API
std::list<std::string> *decklink_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * decklink_LTX_getPlugin( MRef<Library*> lib ){
	return new DeckLinkPlugin( lib );
}


#if 0
MIL_INT MFTYPE GrabEnd(MIL_INT HookType, MIL_ID EventId, void MPTYPE *UserStructPtr)
{
	UserDataStruct *UserPtr=(UserDataStruct*)UserStructPtr;

	/* Signal the end of grab event to the main thread waiting. */
	MthrControl(UserPtr->GrabEndEvent, M_EVENT_SET, M_SIGNALED);

	return 0;
}
#endif



void yuv2rgb(const unsigned char* y,
                int linesizey,
                const unsigned char* u,
                int linesizeu,
                const unsigned char* v,
                int linesizev,
                int width,
                int height,
                unsigned char *rgb){
//        cerr <<"EEEE: doing yuv2rgb for data of size "<<width<<"x"<<height<<endl;
        massert(y);
        massert(u);
        massert(v);
        massert(rgb);


        static const int precision=32768;

        static const int coefficientY=(int)(1.164*precision+0.5);
        static const int coefficientRV = (int)(1.596*precision+0.5);
        static const int coefficientGU = (int)(0.391*precision+0.5);
        static const int coefficientGV = (int)(0.813*precision+0.5);
        static const int coefficientBU = (int)(2.018*precision+0.5);
       // cerr <<"EEEE: doing loop"<<endl;

        for (int h=0; h<height; h++){
                for (int w=0; w<width; w++){

                        //int k=h*width+w;
                        int k=h*linesizey+w;
                        //int i=(h/2)*(width/2)+(w/2);
                        int i=(h/2)*(linesizeu)+(w/2);
                        int Y=y[k];
                        int U=u[i];
                        int V=v[i];

                        if (!(i&0x0F)){
                                __builtin_prefetch (&y[k+32], 0); // prefetch for read
                                __builtin_prefetch (&u[i+32], 0); // prefetch for read
                                __builtin_prefetch (&v[i+32], 0); // prefetch for read
                                //__builtin_prefetch (&rgb[(h*width+w)+32], 1); // prefetch for write
                        }

                        int R = coefficientY*(Y-16)+coefficientRV*(V-128);
                        int G = coefficientY*(Y-16)-coefficientGU*(U-128)-coefficientGV*(V-128);
                        int B = coefficientY*(Y-16)+coefficientBU*(U-128);

                        R = (R+precision/2)/precision;
                        G = (G+precision/2)/precision;
                        B = (B+precision/2)/precision;
                        if (R<0) R=0;
                        if (G<0) G=0;
                        if (B<0) B=0;
                        if (R>255) R=255;
                        if (G>255) G=255;
                        if (B>255) B=255;
                        rgb[(h*width+w)*3+0]=R;
                        rgb[(h*width+w)*3+1]=G;
                        rgb[(h*width+w)*3+2]=B;
                        //rgb[k*4+0]=R;
                        //rgb[k*4+1]=G;
                        //rgb[k*4+2]=B;
                        //cerr <<"EEEE: w=" << w <<" h="<<h<<" k3="<<k*3<<" i="<<i<<endl;
                }
        }
//        cerr<<"EEEE: yuv2rgb done!"<<endl;
}



void uyvy_to_yuv420p(int w, int h, byte_t *in, byte_t* outy, byte_t* outu, byte_t* outv){
	int x;
	int y;
	int inc;
	int W=w/2;
	for (y=0; y<h; y++){
		inc = y & 1;
		for (x=0; x<W; x++){
			//Take u
			*outu=*in;
			in++;
			outu+=inc;

			//Take y1
			*outy=*in;
			in++;
			outy++;

			//Take v
			*outv=*in;
			in++;
			outv+=inc;

			//Take y2
			*outy=*in;
			in++;
			outy++;
		}
	}
}

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate(){
	width=0;
	height=0;
	frame=NULL;
	filled=false;
	doStop=false;
	allocateImage();
}

void DeckLinkCaptureDelegate::allocateImage(){
	if (!frame){
		frame = new MImage;
		//TODO: delete frame->data if it is not NULL, and set to NULL
	}

	if (width>0 && height>0){
		//cerr <<"EEEE: allocating image of size "<<width<<"x"<<height<<endl;
		frame->data[0] = new uint8_t[ width * height];
		frame->data[1] = new uint8_t[ width * height / 2];
		frame->data[2] = new uint8_t[ width * height / 2];
		frame->linesize[0] = width;
		frame->linesize[1] = width/2;
		frame->linesize[2] = width/2;
		frame->width=width;
		frame->height=height;
		frame->chroma = M_CHROMA_I420;
	}

}

void DeckLinkCaptureDelegate::putImage(IDeckLinkVideoInputFrame* videoFrame){
//	cerr <<"EEEE: doing DeckLinkCaptureDelegate::putImage()"<<endl;
	int pw = videoFrame->GetWidth();
	int ph = videoFrame->GetHeight();
//	if (doStop)
//		return;

//	cerr <<"EEEE: size: "<<pw<<"x"<<ph<<endl;
	if (pw!=width || ph!=height){
		width=pw;
		height=ph;
		allocateImage();
	}
	byte_t* vbytes;
	videoFrame->GetBytes((void**)&vbytes);

	bufferLock.lock();
	if (filled){
		bufferSem.dec();
		cerr <<"EEEE: Warning: dropping grabbed frame. Low CPU?"<<endl;
	}
//	cerr <<"EEEE: start convert to 420p"<<endl;
	uyvy_to_yuv420p(pw,ph, vbytes, frame->data[0], frame->data[1], frame->data[2]  );
//	cerr <<"EEEE: end convert to 420p"<<endl;
	filled=true;
	bufferSem.inc();
	bufferLock.unlock();
}

MImage* DeckLinkCaptureDelegate::getImage(){
	bufferSem.dec();
	filled=false;	
	return frame;
}


HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioFrame)
{
//	cerr <<"EEEE: VideoInputFrameArrived called..."<<endl;
	void *frameBytes;
	void *audioFrameBytes;
//	if (doStop)
//		return S_OK;

	// Handle Video Frame
	if(videoFrame)
	{
		if (videoFrame->GetFlags() & bmdFrameHasNoInputSource)
		{
			fprintf(stderr, "Frame received (#%lu) - No input signal detected\n", frameCount);
		}
		else
		{
//			fprintf(stderr, "Frame received (#%lu) - Valid Frame (Size: %li bytes)\n", frameCount, videoFrame->GetRowBytes() * videoFrame->GetHeight());
			putImage(videoFrame);

#if 0
			if (videoOutputFile != -1)
			{
				videoFrame->GetBytes(&frameBytes);
				w
rite(videoOutputFile, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());
			}
#endif
		}
		frameCount++;
	}

	// Handle Audio Frame
	if (audioFrame)
	{
#if 0
		if (audioOutputFile != -1)
		{
			audioFrame->GetBytes(&audioFrameBytes);
			write(audioOutputFile, audioFrameBytes, audioFrame->GetSampleFrameCount() * g_audioChannels * (g_audioSampleDepth / 8));
		}
#endif
	}
	return S_OK;
}


HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events, IDeckLinkDisplayMode *mode, BMDDetectedVideoInputFormatFlags)
{
	cerr <<"WARNING: DeckLinkCaptureDelegate::VideoInputFormatChanged called. "<<endl;
	return S_OK;
}


////

DeckLinkGrabber::DeckLinkGrabber( string dev ){
	device=dev;
	capture=NULL;
	initialized=false;
	localRgb=NULL;
	//	memset(&UserStruct,0,sizeof(UserStruct));
}

void DeckLinkGrabber::setLocalDisplay(MRef<VideoDisplay*> d){
	localDisplay=d;
}

uint32_t DeckLinkGrabber::getHeight(){
	if (capture){
		return capture->getHeight();
	}else
		return 0;
}
uint32_t DeckLinkGrabber::getWidth(){
	if (capture){
		return capture->getWidth();
	}else	
		return 0;

}

void DeckLinkGrabber::init(){

	doStop=false;
	initialized=true;
	//width=height=-1;
	capture = new DeckLinkCaptureDelegate;

	deckLinkIterator = CreateDeckLinkIteratorInstance();
	HRESULT                     result;

	BMDDisplayMode              selectedDisplayMode = bmdModeHD1080i50;
	IDeckLinkDisplayMode        *displayMode;

	if (!deckLinkIterator)
	{
		fprintf(stderr, "This application requires the DeckLink drivers installed.\n");
		initialized=false;
		return;
	}

	/* Connect to the first DeckLink instance */
	result = deckLinkIterator->Next(&deckLink);
	if (result != S_OK)
	{
		fprintf(stderr, "No DeckLink PCI cards found.\n");
		initialized=false;
		return;
	}

	if (deckLink->QueryInterface(IID_IDeckLinkInput, (void**)&deckLinkInput) != S_OK){
		fprintf(stderr, "DeckLink error.\n");
		initialized=false;
		return;
	}

	deckLinkInput->SetCallback(capture);



	result = deckLinkInput->GetDisplayModeIterator(&displayModeIterator);
	if (result != S_OK)
	{
		fprintf(stderr, "Could not obtain the video output display mode iterator - result = %08x\n", result);
		initialized=false;
		return;
	}
#if 0
	selectedDisplayMode = -1;
	while (displayModeIterator->Next(&displayMode) == S_OK)
	{
		if (g_videoModeIndex == displayModeCount)
		{
			selectedDisplayMode = displayMode->GetDisplayMode();
			break;
		}
		displayModeCount++;
		displayMode->Release();
	}

	if (selectedDisplayMode < 0)
	{
		fprintf(stderr, "Invalid mode %d specified\n", g_videoModeIndex);
		initialized=false;
		return;
	}
#endif


	result = deckLinkInput->EnableVideoInput(selectedDisplayMode, bmdFormat8BitYUV, 0);
	if(result != S_OK)
	{
		fprintf(stderr, "Failed to enable video input. Is another application using the card?\n");
		initialized=false;
		return;
	}




#if 0
	if (M_NULL==MappAlloc(M_DEFAULT, &UserStruct.MilApplication)){
		cerr << "ERROR: failed to allocate MIL application!"<<endl;
		return;
	}
	if (M_NULL==MsysAlloc(M_SYSTEM_VIO, M_DEV0, M_DEFAULT, &UserStruct.MilSystem)){
		cerr << "ERROR: failed to allocate MIL system!"<<endl;
		return;
	}

	if (M_NULL==MdigAlloc(UserStruct.MilSystem, M_DEV0, "M_DEFAULT", M_DEFAULT, &UserStruct.MilDigitizer)){
		//if (M_NULL==MdigAlloc(UserStruct.MilSystem, M_DEV0, MIL_TEXT("/opt/matrox_imaging/drivers/vio/dcf/standard/hd_sdi_1280x720P_60Hz.dcf"), M_DEFAULT, &UserStruct.MilDigitizer)){
		//if (M_NULL==MdigAlloc(UserStruct.MilSystem, M_DEV0, MIL_TEXT("/opt/matrox_imaging/drivers/vio/dcf/standard/hd_sdi_1920x1080i_30Hz.dcf"), M_DEFAULT, &UserStruct.MilDigitizer)){
		//if (M_NULL==MdigAlloc(UserStruct.MilSystem, M_DEV0, MIL_TEXT("/opt/matrox_imaging/drivers/vio/dcf/standard/hd_rgb_1280x720p_50Hz.dcf"), M_DEFAULT, &UserStruct.MilDigitizer)){
		//if (M_NULL==MdigAlloc(UserStruct.MilSystem, M_DEV0, MIL_TEXT("/opt/matrox_imaging/drivers/vio/dcf/standard/ntsc.dcf"), M_DEFAULT, &UserStruct.MilDigitizer)){
		cerr << "ERROR: could not allocate digitizer"<<endl;
		return;
	}

	width = MdigInquire(UserStruct.MilDigitizer, M_SIZE_X, M_NULL);
	height= MdigInquire(UserStruct.MilDigitizer, M_SIZE_Y, M_NULL);
	cerr << "DeckLinkGrabber: Info: grabber dimensions are "<< width<<"x"<<height<<endl;
#endif

}

void DeckLinkGrabber::open(){
	massert(!startBlockSem);
	massert(!runthread); // we don't want to start a second thread
	// for a grabber object
	startBlockSem = new Semaphore();
	initBlockSem = new Semaphore();
	runthread = new Thread(this);

	initBlockSem->dec(); // wait for init to complete
	initBlockSem=NULL;

}

bool DeckLinkGrabber::setImageChroma( uint32_t chroma ){
	cerr << "Warning: ignoring setImageChroma request for chroma "<< chroma <<endl;
	return true;
}

void DeckLinkGrabber::start(){
	//	UserStruct.stopped = false;
	doStop=false;
	if (capture)
		capture->doStop=false;
	massert(startBlockSem);
	startBlockSem->inc();
}

void DeckLinkGrabber::stop(){
	//Note: this can be called even if open has not been done.
	//	UserStruct.stopped = true;
	doStop=true;
	if (capture)
		capture->stop();
	if (runthread){
		runthread->join();
		runthread=NULL;

	}
}

/*loop that reads from the card (and calls handler->handle()) until stop() is called*/
void DeckLinkGrabber::run(){
#ifdef DEBUG_OUTPUT
	setThreadName("DeckLinkGrabber::run");
#endif
	//	UserStruct.stopped = false;
	doStop=false;

	if (!initialized)
		init();
#if 0

	/* Initialize hook structure. */
	UserStruct.MilImage     = MilImage;
	UserStruct.NbGrabStart  = 0;

	MdigHookFunction(UserStruct.MilDigitizer, M_GRAB_END,   GrabEnd,   (void *)(&UserStruct));
#endif

	initBlockSem->inc(); // tell thread blocking in "open()" to continue.


	startBlockSem->dec(); // wait until start() has been called
	startBlockSem=NULL; // free

	read( handler );
}

void DeckLinkGrabber::setHandler( ImageHandler * handler ){
	grabberLock.lock();
	this->handler = handler;
	grabberLock.unlock();
}

void DeckLinkGrabber::displayLocal(MImage* frame){
#define SCALE 1
//	cerr <<"EEEE: displayLocal: running "<<endl;
	int targetWidth = frame->width/SCALE;
	int targetHeight = frame->height/SCALE;

	if (!localRgb || localRgb->width!=targetWidth || localRgb->height!=targetHeight){
//		cerr << "EEEE: displayLocal: allocating size "<< targetWidth<<"x"<<targetHeight<<endl;
		//FIXME: delete if not NULL
		localRgb = new MImage;

		localRgb->data[0] = new uint8_t[ targetWidth * targetHeight * 4 ];
		localRgb->data[1] = NULL;
		localRgb->data[2] = NULL;
		localRgb->linesize[0] = targetWidth*3;
		localRgb->linesize[1] = 0;
		localRgb->linesize[2] = 0;
		localRgb->width=targetWidth;
		localRgb->height=targetHeight;
		localRgb->chroma = M_CHROMA_RV24;
	}
	

//	cerr << "EEEE: starting conversion.."<<endl;
	yuv2rgb(  
			&frame->data[0][0], frame->linesize[0],
			&frame->data[1][0],frame->linesize[1],
			&frame->data[2][0],frame->linesize[2],
			frame->width,
			frame->height,
			&localRgb->data[0][0]
	       );
//	cerr << "EEEE: conversion done"<<endl;



	localDisplay->handle(localRgb);

}

void DeckLinkGrabber::read( ImageHandler * handler ){
	if (localDisplay){
		localDisplay->setIsLocalVideo(true);
		localDisplay->start();
	}

	HRESULT result = deckLinkInput->StartStreams();
	if(result != S_OK)
	{
		cerr<<"ERROR: could not start DeckLink capturing"<<endl;
		return;
	}

	while (!doStop){

		static int i=0;
		i++;
		//			if (i%50==1){ //grabber does 50fps. We send 25fps.

		MImage*frame = capture->getImage();
//		if (i%2==0){
			handler->handle( frame );

			if (localDisplay){
				displayLocal(frame);
				//localDisplay->handle(frame);
			}
//		}

		if (i%200==0){
			static struct timespec last_wallt;
			static struct timespec last_cput;

			struct timespec now_cpu;
			struct timespec now_wall;
			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now_cpu);
			clock_gettime(CLOCK_REALTIME, &now_wall);
			cerr <<"cpusec="<<now_cpu.tv_sec<<" lastsec="<<last_cput.tv_sec<<endl;
			uint64_t delta_cpu = (now_cpu.tv_sec-last_cput.tv_sec)*1000000000LL+(now_cpu.tv_nsec-last_cput.tv_nsec);
			uint64_t delta_wall = (now_wall.tv_sec-last_wallt.tv_sec)*1000000000LL+(now_wall.tv_nsec-last_wallt.tv_nsec);

			cerr <<"DeckLinkGrabber:: CPU USAGE: "<< now_cpu.tv_sec<<"."<<now_cpu.tv_nsec<<endl;
			cerr <<"delta_cpu="<<delta_cpu/1000<<" delta_wall="<<delta_wall/1000<<endl;
			cerr <<"========> DeckLinkGrabber: CPU usage: "<< ((float)delta_cpu/(float)delta_wall)*100.0<<"%"<<endl;
			last_cput=now_cpu;
			last_wallt=now_wall;

		}


	}

	result = deckLinkInput->StopStreams();
	deckLinkInput->FlushStreams();
	Thread::msleep(100);

	if (localDisplay){
		localDisplay->stop();
	}


	if (displayModeIterator != NULL)
	{
		displayModeIterator->Release();
		displayModeIterator = NULL;
	}

	if (deckLinkInput != NULL)
	{
		deckLinkInput->Release();
		deckLinkInput = NULL;
	}

	if (deckLink != NULL)
	{
		deckLink->Release();
		deckLink = NULL;
	}

	if (deckLinkIterator != NULL)
		deckLinkIterator->Release();

	initialized=false;

#if 0
	/* Allocate and clear sequence and display images. */
	MappControl(M_ERROR, M_PRINT_DISABLE);
	for (UserStruct.NbFrames=0; UserStruct.NbFrames<NB_GRAB_MAX+2; UserStruct.NbFrames++)
	{
		MbufAllocColor(UserStruct.MilSystem, 
				MdigInquire(UserStruct.MilDigitizer, M_SIZE_BAND, M_NULL), 
				MdigInquire(UserStruct.MilDigitizer, M_SIZE_X, M_NULL), 
				MdigInquire(UserStruct.MilDigitizer, M_SIZE_Y, M_NULL), 
				8L+M_UNSIGNED, 
				M_IMAGE + M_GRAB + M_NON_PAGED + M_BGR32 + M_HOST_MEMORY, 
				&MilImage[UserStruct.NbFrames]); //Allocate grab buffer with three bands for RGB24 capture

		if (MilImage[UserStruct.NbFrames])
		{
			MbufClear(MilImage[UserStruct.NbFrames], 0xFF);
		}
		else{
			cerr << "EEEE: ERROR: could not allocate image buffer"<<endl;
			break;
		}	
	}


	MappControl(M_ERROR, M_PRINT_ENABLE);

	/* Free buffers to leave space for possible temporary buffers. */
	int n;
	for (n=0; n<2 && UserStruct.NbFrames; n++)
	{
		UserStruct.NbFrames--;
		MbufFree(MilImage[UserStruct.NbFrames]);
	}

	/* MIL event allocation for grab end hook. */
	MthrAlloc(UserStruct.MilSystem, M_EVENT, M_DEFAULT, M_NULL, M_NULL, &UserStruct.GrabEndEvent);

	/* Put digitizer in asynchronous mode. */
	MdigControl(UserStruct.MilDigitizer, M_GRAB_MODE, M_ASYNCHRONOUS);
	/* Start sequence with a grab in the first buffer. */
	MdigGrab(UserStruct.MilDigitizer, UserStruct.MilImage[0]);

	MImage *frame = new MImage;

	frame->data[0] = new uint8_t[ width * height * 4 ];
	frame->data[1] = NULL;
	frame->data[2] = NULL;
	frame->linesize[0] = width*4;
	frame->linesize[1] = 0;
	frame->linesize[2] = 0;
	frame->width=width;
	frame->height=height;
	frame->chroma = M_CHROMA_RV32;
	frame->width=width;
	frame->height=height;


	while (!UserStruct.stopped)
	{
		/* Wait end of grab event */
		MthrWait(UserStruct.GrabEndEvent, M_EVENT_WAIT, M_NULL);

		int frameindex = UserStruct.NbGrabStart;

		UserStruct.NbGrabStart = (UserStruct.NbGrabStart+1) % NB_GRAB_MAX;

		MdigGrab(UserStruct.MilDigitizer, UserStruct.MilImage[ UserStruct.NbGrabStart] );

		MIL_INT line_delta=-33;
		uint8_t *rgb32ptr;

		MbufInquire( UserStruct.MilImage[ frameindex ], M_PITCH_BYTE, &line_delta ); //FIXME: copy from correct index
		MbufInquire(UserStruct.MilImage[ frameindex ], M_HOST_ADDRESS, &rgb32ptr);
		memcpy(frame->data[0], rgb32ptr, width * height * 4 );
		frame->mTime = mtime();
		if (!UserStruct.stopped){
			static int i=0;
			i++;
			//			if (i%50==1){ //grabber does 50fps. We send 25fps.

			handler->handle( frame );
			if (localDisplay){
				localDisplay->handle(frame);
			}
			//			}

			if (i%200==0){
				static struct timespec last_wallt;
				static struct timespec last_cput;

				struct timespec now_cpu;
				struct timespec now_wall;
				clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now_cpu);
				clock_gettime(CLOCK_REALTIME, &now_wall);
				cerr <<"cpusec="<<now_cpu.tv_sec<<" lastsec="<<last_cput.tv_sec<<endl;
				uint64_t delta_cpu = (now_cpu.tv_sec-last_cput.tv_sec)*1000000000LL+(now_cpu.tv_nsec-last_cput.tv_nsec);
				uint64_t delta_wall = (now_wall.tv_sec-last_wallt.tv_sec)*1000000000LL+(now_wall.tv_nsec-last_wallt.tv_nsec);

				cerr <<"DeckLinkGrabber:: CPU USAGE: "<< now_cpu.tv_sec<<"."<<now_cpu.tv_nsec<<endl;
				cerr <<"delta_cpu="<<delta_cpu/1000<<" delta_wall="<<delta_wall/1000<<endl;
				cerr <<"========> DeckLinkGrabber: CPU usage: "<< ((float)delta_cpu/(float)delta_wall)*100.0<<"%"<<endl;
				last_cput=now_cpu;
				last_wallt=now_wall;

			}

		}
	}
	if (localDisplay){
		localDisplay->stop();
	}

	/* Wait for end of last grab. */
	MdigGrabWait(UserStruct.MilDigitizer, M_GRAB_END);


	/* Unhook functions. */
	//	MdigHookFunction(UserStruct.MilDigitizer, M_GRAB_START+M_UNHOOK, GrabStart, (void *)(&UserStruct));
	MdigHookFunction(UserStruct.MilDigitizer, M_GRAB_END+M_UNHOOK, GrabEnd, (void *)(&UserStruct));

	/* Free event. */
	MthrFree(UserStruct.GrabEndEvent);

	/* Free allocations. */
	for (n=0; n<UserStruct.NbFrames; n++)
	{
		MbufFree(UserStruct.MilImage[n]);
	}
	//XXX: TODO: free resources
	MappFreeDefault(UserStruct.MilApplication, UserStruct.MilSystem, M_NULL, UserStruct.MilDigitizer, M_NULL);

	initialized=false;
#endif
}

void DeckLinkGrabber::close(){
	stop();
}


MRef<Grabber *> DeckLinkPlugin::create( const std::string &device ) const{
	return new DeckLinkGrabber( device );
}

