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
  *	    Cesc Santasusana <c e s c DOT s a n t a [AT} g m a i l DOT c o m>
*/

#include<config.h>

#include<libminisip/media/soundcard/FileSoundDevice.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>
#include<signal.h>
#include<time.h>
#include<stdlib.h>
#include<libmutil/stringutils.h>
#include<libmutil/mtime.h>

#ifdef WIN32
	#include<winsock2.h>
#else
	#include<time.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#	define USE_WIN32_API
#endif

using namespace std;

int filesleep( unsigned long usec ){
#ifdef WIN32
#include<winsock2.h>
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = (long)usec;

	return select (0, NULL, NULL, NULL, &tv);
#else
	struct timespec request;
	request.tv_sec = 0;
	request.tv_nsec = (long) usec * 1000;

	return nanosleep( &request, NULL );
#endif
}

FileSoundDevice::FileSoundDevice(string in_file, 
				string out_file, 
				int32_t filetype_ ):
		SoundDevice("!notused_filesounddevice!"),
	fileType(filetype_),
	inFilename(in_file), 
	outFilename(out_file)
{
	this->in_fd=-1;	
	this->out_fd=-1;	

	format=-1;
	openedPlayback=false;
	openedRecord=false;
	
	nChannelsPlay = 0;
	nChannelsRecord = 0;
	samplingRate = 0;
	sampleSize = 0;

	fileSoundBlockSleep = 20; //default value 
	lastTimeRead = 0;
	lastTimeWrite = 0;

	isFirstTimeOpenWrite = true;
	this->loopRecord = true;
}

void FileSoundDevice::setAudioParams( int samplingRate_, int nChannels_ ) {
	if( fileType == FILESOUND_TYPE_RAW ) {
		if( samplingRate_ > 0 ) samplingRate = samplingRate_;
		if( nChannels_ > 0 ) {
			this->nChannelsRecord = nChannels_;
			this->nChannelsPlay = nChannels_;
		}
	} else {
		cerr << "FileSoundDevice: filetype not understood" << endl;
	}
}

int FileSoundDevice::openRecord(int32_t samplerate_, int nChannels_, int format_){

	if( format!= -1 && format_!=format ) {
#ifdef DEBUG_OUTPUT
		cerr << "FileSoundDevice::openRecord - trying to modify the format!" << endl;
#endif
		exit(-1);
	} else {
		setFormat( format_ );
		setAudioParams( samplerate_, nChannels_ );
	}
	
#ifdef DEBUG_OUTPUT
	printf( "FileSoundDev:record - samplerate = %d, nChannels = %d, sampleSize = %d\n", samplingRate, nChannelsRecord, sampleSize );
#endif

#ifndef _WIN32_WCE
	in_fd=::open(inFilename.c_str(), O_RDONLY);
#else
	//win32 wcecompat only takes 3 params ... open the file with mode = permission for everybody ... 
	in_fd=::_open(inFilename.c_str(), O_RDONLY, S_IREAD | S_IWRITE | S_IEXEC);
#endif
	if (in_fd==-1){
		printError("openRecord");
		exit(-1); //FIX: handle nicer - exception
	}
	openedRecord=true;

	return 0;
}

int FileSoundDevice::openPlayback(int32_t samplerate_, int nChannels_, int format_){
	
	if( format!= -1 && format_!=format ) {
#ifdef DEBUG_OUTPUT
		cerr << "FileSoundDevice::openRecord - trying to modify the format!" << endl;
#endif
		exit(-1);
	} else {
		setFormat( format_ );
		setAudioParams( samplerate_, nChannels_ );
	}
	
#ifdef DEBUG_OUTPUT
	printf( "FSD:playback - samplerate = %d, nChannels = %d, sampleSize = %d\n", samplingRate, nChannelsPlay, sampleSize );
#endif

	int openFlags;
	//if it is the first time we open for writing, create and truncate to zero size
	//otherwise, append to the file
	if( isFirstTimeOpenWrite ) {
		#ifdef _MSC_VER
			openFlags = _O_WRONLY |  _O_CREAT | _O_TRUNC;
		#else
			openFlags = O_WRONLY |  O_CREAT | O_TRUNC;
		#endif
		isFirstTimeOpenWrite = false;
	} else {
		#ifdef _MSC_VER
			openFlags = _O_WRONLY |  _O_APPEND;
		#else
			openFlags = O_WRONLY |  O_APPEND;
		#endif
	}
	
	
#ifdef _MSC_VER
	out_fd =::_open(outFilename.c_str(), openFlags, _S_IREAD | _S_IWRITE );
#else
	out_fd =::open( outFilename.c_str(), openFlags, S_IWUSR |  S_IRUSR);
#endif
	
	if (out_fd==-1){
		printError("openPlayback");
		exit(-1); //FIX: handle nicer - exception
	}

	openedPlayback=true;

	return 0;
}

int FileSoundDevice::closeRecord(){
	int ret;
	openedRecord=false;
#ifdef _WIN32_WCE
	ret = ::_close(in_fd);
#else
	ret = ::close(in_fd);
#endif
	if( ret == -1 ) {
		printError("openRecord");
	}
	lastTimeRead = 0;
	return ret;
}

int FileSoundDevice::closePlayback(){
	int ret;
	openedPlayback=false;
#ifdef _WIN32_WCE
	ret = ::_close(out_fd);
#else
	ret = ::close(out_fd);
#endif
	if( ret == -1 ) {
		printError("openPlayback");
	}
// 	lastTimeWrite = 0;
	return ret;
}

//n is in samples! not in bytes
int FileSoundDevice::read(byte_t *buffer, uint32_t nSamples){
	if (lastTimeRead==0){
		lastTimeRead = mtime();
	}

	//loop if needed
	if( loopRecord ) {
		int currPos;
		int fileSize;
		//Check if we are at the end of the file ...
		currPos = lseek( in_fd, 0, SEEK_CUR );
		fileSize = getFileSize( in_fd );
		
		if( currPos == -1 ) { 
			printError("read-loop"); 
			return -1; }
		//Check not for the exact end of file, but for when there are not enough 
		//samples to read ... skip the few left and loop ...
		if( (fileSize - currPos ) < ((int)nSamples * getSampleSize() * getNChannelsRecord() ) ) {
			if( currPos == -1 ) { 
				printError("read-loop2"); 
				return -1; }
			currPos = lseek( in_fd, 0, SEEK_SET );
			if( currPos == -1 ) { 
				printError("read-loop3"); 
				return -1; }
		}
	}
	
	SoundDevice::read( buffer, nSamples );
	
	if( fileSoundBlockSleep != 0 )
		readSleep();

	return nSamples;
}

int FileSoundDevice::readFromDevice(byte_t *buf, uint32_t nSamples){
	int retValue=-1;

	//select the appropriate way to write to the file ...
	switch( fileType ) {
		case FILESOUND_TYPE_RAW: 
#ifdef _WIN32_WCE
			retValue = ::_read(in_fd, buf, nSamples * getSampleSize() * getNChannelsRecord() );
#else
			retValue = ::read(in_fd, buf, nSamples * getSampleSize() * getNChannelsRecord() );
#endif
				
			if( retValue == -1 ) {
				retValue = -errno;
				printError( "readFromDevice" );
			} else {
				retValue = retValue / ( getSampleSize() * getNChannelsRecord() );
			}
			break;
		case FILESOUND_TYPE_WAV:
		case FILESOUND_TYPE_MP3:
			cerr << "FileSoundDevice::readFromDevice - filetype not implemented" << endl;
			break;
		default:
			cerr << "FileSoundDevice::readFromDevice - filetype unknown" << endl;
	}
	return retValue;
}

int FileSoundDevice::readError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	bool mustReturn = true;
	switch( errcode ) {
		case -EAGAIN:

	#ifndef _WIN32_WCE
		case -EINTR:
	#else
		case WSAEINTR:
	#endif
			mustReturn = false;
			break;
		default:
			mustReturn = true;
			break;
	}
	if( mustReturn ) { return -1; }
	else { return 0; } 
}

void FileSoundDevice::readSleep( ) {
	uint64_t currentTime;
	
	currentTime = mtime();
	
	//the sleep thingy is deactivated if sleeptime < 0
	// (the time in the computer should not go backward, right?!
// 	printf("R: %d ", currentTime - lastTimeRead );
	while (currentTime - lastTimeRead < fileSoundBlockSleep){
		int32_t ret;
		int32_t sleep = (int32_t ) (fileSoundBlockSleep - (currentTime-lastTimeRead));
		if( sleep < 0 ) sleep = 0;
// 		printf(" [%d] ", sleep);
		ret = filesleep( sleep * 1000);
		currentTime = mtime();
// 		printf(" %d ", currentTime - lastTimeRead);
	}
// 	printf("\n");
	
	lastTimeRead+=fileSoundBlockSleep;
}

int FileSoundDevice::write( byte_t * buffer, uint32_t nSamples ) {
	if (lastTimeWrite==0){
		lastTimeWrite = mtime();
	}

	SoundDevice::write( buffer, nSamples );
	
	//if SoundDevice::sleepTime is >0, then the device has been opened for 
	//playback in non-blocking mode, thus SoundDevice::write will do the sleeping between
	//calls (no need for FileSoundDevice::write to block for fileSoundBlockSleep miliseconds).
	if( fileSoundBlockSleep > 0  && sleepTime == 0)
		writeSleep();

	return nSamples;
}


//n is in samples!! not in bytes
int FileSoundDevice::writeToDevice( byte_t *buf, uint32_t nSamples ){
	int retValue = -1;
	
	//select the appropriate way to write to the file ...
	switch( fileType ) {
		case FILESOUND_TYPE_RAW: 
			//write n samples to the file ... 
#ifdef _WIN32_WCE
			retValue = ::_write(out_fd, buf, nSamples * getSampleSize() * getNChannelsPlay() );
#else
			retValue = ::write(out_fd, buf, nSamples * getSampleSize() * getNChannelsPlay() );
#endif
			if( retValue == -1 ) {
				retValue = -errno;
				printError( "write" );
			} else {
				retValue = retValue / ( getSampleSize() * getNChannelsPlay() );
			}
			break;
		case FILESOUND_TYPE_WAV:
		case FILESOUND_TYPE_MP3:
			cerr << "FileSoundDevice::write - filetype not implemented" << endl;
			break;
		default:
			cerr << "FileSoundDevice::write - filetype unknown" << endl;
	}

	return retValue;
}

int FileSoundDevice::writeError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	bool mustReturn = true;
	switch( errcode ) {
		case -EAGAIN:
	#ifndef _WIN32_WCE
		case -EINTR:
	#else
		case WSAEINTR:
	#endif
			mustReturn = false;
			break;
		default:
			mustReturn = true;
			break;
	}
	if( mustReturn ) { return -1; }
	else { return 0; } 
}


void FileSoundDevice::writeSleep( ) {
	uint64_t currentTime;
	
	currentTime = mtime();

	//the sleep thingy is deactivated if sleeptime < 0
	// (the time in the computer should not go backward, right?!
// 	printf("W: %d ", currentTime - lastTimeWrite );
	while (currentTime - lastTimeWrite < fileSoundBlockSleep ){
		int32_t ret;
		int32_t sleep = (int32_t) (fileSoundBlockSleep - (currentTime-lastTimeWrite));
		if( sleep < 0 ) sleep = 0;
// 		printf(" [%d] ", sleep);
		ret = filesleep( sleep * 1000);
		currentTime = mtime();
// 		printf(" %d ", currentTime - lastTimeWrite);
	}
// 	printf("\n");
	
	//lastTimeWrite = currentTime;
	lastTimeWrite += fileSoundBlockSleep;
}

void FileSoundDevice::sync(){
	cerr << "ERROR: sync unimplemented for file sound device"<< endl;
}

int FileSoundDevice::getFileSize( int fd ) {
	int ret;
	int filesize;
	int currentPos;
	
	currentPos = lseek( fd, 0, SEEK_CUR );
	if( currentPos == -1 ) {
#ifdef DEBUG_OUTPUT
//		printError("getFileSize (1)");
#endif
		return -1;
	}
	
	filesize = lseek( fd, 0, SEEK_END );
	if( filesize == -1 ) {
#ifdef DEBUG_OUTPUT
//		printError("getFileSize(2)");
#endif
		return -1;
	}
	
	ret = lseek( fd, currentPos, SEEK_SET);
	if( ret == (currentPos-1) ) {
#ifdef DEBUG_OUTPUT
//		printError("getFileSize(3)");
#endif
		return -1;
	}
	return filesize;
}

void FileSoundDevice::printError( string func ) {
	string errStr;	
	errStr = "FileSoundDevice::" + func + " - errno = ";
//cut the function in wince ... most of these are not defined ... not worth the trouble
#ifndef _WIN32_WCE
	switch( errno ) {
		case EACCES: errStr + "eaccess"; break;
		case EEXIST: errStr + "eexist"; break;
		case EFAULT: errStr + "efault"; break;
		case EISDIR: errStr + "eisdir"; break;
	#ifndef USE_WIN32_API
		case ELOOP: errStr + "eloop"; break;
	#endif
		case EMFILE: errStr + "emfile"; break;
		case ENAMETOOLONG: errStr + "toolong"; break;
		case ENFILE: errStr + "enfile"; break;
		case ENODEV: errStr + "enodev"; break;
		case ENOENT: errStr + "enoent"; break;
		case ENOMEM: errStr + "enomem"; break;
		case ENOSPC: errStr + "enospc"; break;
		case ENOTDIR: errStr + "enotdir"; break;
		case ENXIO: errStr + "enxio"; break;
	#ifndef USE_WIN32_API
		case EOVERFLOW: errStr + "eoverflow"; break;
	#endif
		case EROFS: errStr + "erofs"; break;
	#ifndef USE_WIN32_API
		case ETXTBSY: errStr + "etxtbsy"; break;
	#endif
		default: errStr + "unknown";
	}
#endif
	cerr << errStr << " (check man page for explanation)" << endl;
}


