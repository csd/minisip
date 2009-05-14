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

/* Copyright (C) 2004, 2005, 2006
 *
 * Author: Cesc Santasusana (cesc DOT santa A{T gmail D.OT com )
*/

#include<config.h>

#include<libminisip/media/CallRecorder.h>

#include<libminisip/media/AudioMedia.h>
#include<libminisip/media/soundcard/FileSoundDevice.h>

#include<libmutil/stringutils.h>
#include<libmutil/Mutex.h>
#include<libmutil/mtime.h>
#include<libmutil/CircularBuffer.h>

#include<string.h>

#define AUDIO_FRAME_DURATION_MS 20

using namespace std;


CallRecorder::CallRecorder(  MRef<AudioMedia *> aMedia, 
				MRef<RtpReceiver *> rtpReceiver_,
				MRef<IpProvider *> /*ipProvider*/ ):
		RealtimeMediaStreamReceiver( "callrecorder", (RealtimeMedia *)*aMedia, rtpReceiver_ ),
		enabledMic(false),
		enabledNtwk(false),
		fileDev( NULL ),
		audioMedia( aMedia)
{
	static int count = 0;
	count ++;
	
	flushLastTime = 0;	
	
	setAllowStart( false );
	setEnabledMic( false );
	setEnabledNetwork( false );
	
	//Init the circular buffers ... max delay of 5 "groups" of samples = 100ms
	micData = ntwkData = NULL;
	micData = new CircularBuffer( 160 * 5 );
	ntwkData = new CircularBuffer( 160 * 5 );
	
	setFilename( itoa(count), 0 );
	
	fileDev = new FileSoundDevice( "", getFilename(), FILESOUND_TYPE_RAW );
	fileDev->setSleepTime( 0 );
	fileDev->setLoopRecord(false );
	
	start(); //register to rtp receiver and start getting packets
	
	audioMedia->getSoundIO()->register_recorder_receiver( this, 
					SOUND_CARD_FREQ * AUDIO_FRAME_DURATION_MS / 1000, 
					false );
	
#ifdef DEBUG_OUTPUT
	cerr << "CallRecorder::created: " << getDebugString() << endl;
#endif
// 	setEnabledMic( true );
// 	setEnabledNetwork( true );
}

CallRecorder::~CallRecorder( ) {
	#ifdef DEBUG_OUTPUT
	cerr << "CallRecorder Destroyed - " << getFilename() << endl;
	#endif
	if( fileDev->isOpenedPlayback() ) {
		if( fileDev->closePlayback() == -1 ) {
			cerr << "CallRecorder::destroy - ERROR closing file opened to record = " << filename << endl;
		} else {
			cerr << "CallRecorder::destroy - closing file opened to record = " << filename << endl;
		}
	}
	if (audioMedia)
		audioMedia->getSoundIO()->unregisterRecorderReceiver( this );
	if( micData!=NULL ) delete micData;
	if( ntwkData!=NULL ) delete ntwkData;
// 
}

void CallRecorder::free(){
	if (audioMedia)
		audioMedia->getSoundIO()->unregisterRecorderReceiver( this );
	audioMedia=NULL;

	//free inherited references
	rtpReceiver=NULL;
	rtp6Receiver=NULL;
}

void CallRecorder::setFilename( string name, int ssrc ) {
	filename = "minisip.callrecord." + name + "." + itoa( ssrc ) + ".8khz.16bit.signed.raw.sw";
#ifdef DEBUG_OUTPUT
	cerr << "CallRecorder::setFilename - " << filename << endl;;
#endif
}

//we receive the audio from the mic via this function ...(from SoundIO)
//length is in samples
void CallRecorder::srcb_handleSound(void *samplearr, int /*nSamples*/) {
	//printf( "YY" );
	if( !allowStart ) { return; }
	if( !enabledMic ) { return; }
	
	if( !audioMedia || !audioMedia->getResampler() ) { return; }
        audioMedia->getResampler()->resample( (short *)samplearr, resampledData );
	
	addMicData( (void *)resampledData, 160 );
	flush();
}

#ifdef AEC_SUPPORT
	void CallRecorder::srcb_handleSound(void *samplearr, void *samplearrR){
		printf( "CallRecorder:: Function not implemented!!!\n" );
		if( ! enabledMic ) { return; }
	}
#endif

//This function receives the audio from the mediastreamreceiver, that is,
//from the network
void CallRecorder::handleRtpPacket( MRef<SRtpPacket *> packet, MRef<IPAddress *> /*from*/ ) {
	if( !allowStart ) { return; }
	if( ! enabledNtwk ) { return; }

	//if we have a timeout notification from the rtpreceiver ...
	if( !packet ) {
		return;
	}
	
	uint32_t packetSsrc = packet->getHeader().getSSRC();
	uint16_t seq_no = packet->getHeader().getSeqNo();
	
	if( packet->unprotect( getCryptoContext( packetSsrc, seq_no ) )){
		// Authentication or replay protection failed
		return;
	}

//	media->playData( *packet );
	if( !audioMedia ) {
		audioMedia = dynamic_cast<AudioMedia *>(*media);
		if( !audioMedia ) {
			cerr << "ERROR!!! MediaStreamReceiverOverride being used for something not Audio!!" << endl;
		}
		#ifdef DEBUG_OUTPUT
		cerr << "CallRecorder: not audioMeida" << endl;
		#endif		
		return;
	}
	
	//we implement the ::playData functionality from AudioMedia and from AudioMediaSource ..
	MRef<AudioMediaSource *> source = audioMedia->getSource( packet->getHeader().SSRC );
	if( !source ) { 
		#ifdef DEBUG_OUTPUT
		cerr << "CALLREC: no source" << endl; 
		#endif
		return; 
	}
	
	//we are sure we got a source ... let's get the codec to decode the data
	RtpHeader hdr = packet->getHeader();
	MRef<CodecState *> codec = source->findCodec( hdr.getPayloadType() );

	if( !codec ) { 
		#ifdef DEBUG_OUTPUT
		cerr << "CALLREC: no codec" << endl; 
		#endif
		return;
	}
	
	//Now we got the codec. We need to decode the audio 
	uint32_t outputSize = codec->decode( packet->getContent(), 
					packet->getContentLength(), 
					codecOutput );

	addNtwkData( (void *)codecOutput, outputSize );
	flush();
}

//length in samples
void CallRecorder::addMicData( void * data, int nSamples ) {
	bool ret;
	
	//printf("M1\n");
	micMutex.lock();
	//printf("M2\n");
	
	if( micData->getFree() < nSamples ) {
// 		printf("CRMIC_OF\n");
		micMutex.unlock();
		return;
	}
	ret = micData->write( (short *)data, nSamples );
	if( !ret ) {
#ifdef DEBUG_OUTPUT
		printf("CR: Error in addMicData\n");
#endif
	}
	
	//printf("M3 - byteLength=%d - storedSamples=%d\n", length, micWrite-micData);
	micMutex.unlock();
	//printf("M4\n");
}

//length in samples
void CallRecorder::addNtwkData( void * data, int nSamples ) {
	bool ret;
	
	//printf("N1\n");
	ntwkMutex.lock();
	//printf("N2\n");
	
	if( ntwkData->getFree() < nSamples ) {
// 		printf("CRNTWK_OF\n");
		ntwkMutex.unlock();
		return;
	}
	ret = ntwkData->write( (short *)data, nSamples );
	if( !ret ) {
#ifdef DEBUG_OUTPUT
		printf("CR: Error in addMicData\n");
#endif
	}
	
	//printf("N3 - byteLength=%d - storedSamples=%d\n", length, ntwkWrite-ntwkData);
	ntwkMutex.unlock();
	//printf("N4\n");
}

//flush (write to the file device) the audio samples in the mic and ntwk buffers
//Only let this function take samples every 20ms
void CallRecorder::flush( ) {

	bool ret;
	
	//printf("FA\n");
	fileDevMutex.lock();
	micMutex.lock();	
	ntwkMutex.lock();
	
	if (flushLastTime==0)
		flushLastTime=mtime();
	uint64_t currentTime = mtime();

	if(currentTime - flushLastTime < AUDIO_FRAME_DURATION_MS ) {
// 		printf( "F0 - not time to write\n" );
		ntwkMutex.unlock();
		micMutex.unlock();
		fileDevMutex.unlock();
		return;
	}
	
	//make sure there is no left overs from previous executions
	memset( mixedData, 0, 160 * 2 * sizeof(int) );
	//now we only use half this buffer ... we reading a mono stream
	memset( tempFlush, 0, 160 * 2 * sizeof(short) );
	
	ret = micData->read( tempFlush, 160 );
	if( ret ) {
		for( int i=0; i < 160; i++ ) {
			mixedData[i * 2] = tempFlush[i];
		}
	} else {
// 		printf( "FLUSHMIC_UF\n" );
	}
	
	//now we only use half this buffer ... we reading a mono stream
	memset( tempFlush, 0, 160 * 2 * sizeof(short) );
	ret = ntwkData->read( tempFlush, 160 );
	if( ret ) {
		for( int i=0; i < 160; i++ ) {
			mixedData[i * 2 + 1] = tempFlush[i];
		}
	} else {
// 		printf( "FLUSHNTWK_UF\n" );
	}
	
	//normalize ... not really needed ... we ain't mixing
	//Here we are using all the tempFlush ... it is a stereo stream 
	//	after mixing
	memset( tempFlush, 0, 160 * 2 * sizeof(short) );
	for( int j = 0; j < 160 * fileDev->getNChannelsPlay(); j++ ) {
		if( mixedData[j] > 32737 ) {
			tempFlush[j] = 32737;
		} else if( mixedData[j] < -32737 ) {
			tempFlush[j] = -32737;
		} else {
			tempFlush[j] = (short)mixedData[j];
		}
	}
	
	write( (void *)tempFlush, 160 );
	
	flushLastTime = mtime();
	
	ntwkMutex.unlock();
	micMutex.unlock();
	fileDevMutex.unlock();
	//printf("F12\n");
}

/*
Write audio samples to the file.
@param data Audio samples to be written to the file
@param length length of the data, in samples
*/
int CallRecorder::write( void * data, int nSamples ) {
	if( !fileDev ) {
		return 0;
	}
	//if enabled, check if open ... if not, do it
	if( !fileDev->isOpenedPlayback() ) {
		 //don't care about the params ... the function does not use them
		#ifdef DEBUG_OUTPUT
		cerr << "CallRecorder: Start recording to file <" 
			<< getFilename() << ">" << endl;
		#endif
		fileDev->openPlayback( 8000,  //sampling rate ...
					2 /* number of channels */
					/*, SOUND_S16LE*/);
	}
	
	int writtenBytes;
	//the filedevice returns the number of samples written
	writtenBytes = fileDev->write( (byte_t *)data, nSamples );
	if( writtenBytes < nSamples  ) {
#ifdef DEBUG_OUTPUT
		cerr << "CallRecorder::write - not all written (" 
				<< itoa(writtenBytes) << "/" 
				<< itoa(nSamples) << ")" << endl;
#endif
	}
// 	printf("write:: %d/%d\n", writtenBytes, nSamples);
	return 0;
}

#ifdef DEBUG_OUTPUT
string CallRecorder::getDebugString() {
	string ret;
	ret=  "CallRecorder: filename=" + getFilename();
	ret+= (enabledMic?"; MIC enabled":"; MIC disabled");
	ret+= (enabledNtwk?"; NTWK enabled":"; NTWK disabled");
	ret+= (allowStart?"; START allowed":"; START not allowed");
	return ret;
}
#endif	


