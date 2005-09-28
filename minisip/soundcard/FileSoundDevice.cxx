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

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>
#include<assert.h>
#include<signal.h>
#include<time.h>
#include<libmutil/itoa.h>
#include<libmutil/mtime.h>



#ifdef _MSC_VER
#include<io.h>
#else
#include<sys/time.h>
#include<unistd.h>
#endif

#include"SoundDevice.h"
#include"FileSoundDevice.h"

#ifdef WIN32
	#include<winsock2.h>
#else
	#include<time.h>
#endif

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

	sleepTime = 20; //default value 
	lastTimeRead = 0;
	lastTimeWrite = 0;

	isFirstTimeOpenWrite = true;
	loopRecord = true;
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
	printf( "FSD:record - samplerate = %d, nChannels = %d, sampleSize = %d\n", samplingRate, nChannelsRecord, sampleSize );
#endif
	in_fd=::open(inFilename.c_str(), O_RDONLY);
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
	ret = ::close(in_fd);
	if( ret == -1 ) {
		printError("openRecord");
	}
	lastTimeRead = 0;
	return ret;
}

int FileSoundDevice::closePlayback(){
	int ret;
	openedPlayback=false;
	ret = ::close(out_fd);
	if( ret == -1 ) {
		printError("openPlayback");
	}
	lastTimeWrite = 0;
	return ret;
}


void FileSoundDevice::sync(){
	cerr << "ERROR: sync unimplemented for file sound device"<< endl;
}

//n is in samples! not in bytes
int FileSoundDevice::read(byte_t *buf, uint32_t n){
	int retValue;
	
	if (lastTimeRead==0){
		lastTimeRead = mtime();
	}

	//loop if needed
	if( loopRecord ) {
		int currPos;
		//Check if we are at the end of the file ...
		currPos = lseek( in_fd, 0, SEEK_CUR );
		if( currPos == -1 ) { printError("read-loop"); return -1; }
		if( currPos == getFileSize( in_fd ) ) {
			if( currPos == -1 ) { printError("read-loop2"); return -1; }
			currPos = lseek( in_fd, 0, SEEK_SET );
			if( currPos == -1 ) { printError("read-loop3"); return -1; }
		}
	}
	
	//select the appropriate way to write to the file ...
	switch( fileType ) {
		case FILESOUND_TYPE_RAW: 
			retValue = ::read(in_fd, 
					buf, 
					n * getSampleSize() * getNChannelsRecord() );
			if( retValue == -1 ) {
				printError( "read" );
			}
			break;
		case FILESOUND_TYPE_WAV:
		case FILESOUND_TYPE_MP3:
			cerr << "FileSoundDevice::read - filetype not implemented" << endl;
			break;
	}
	
	if( sleepTime != 0 )
		readSleep();

	return retValue;
}

//n is in samples!! not in bytes
int FileSoundDevice::write(byte_t *buf, uint32_t n){
	int retValue;
	
	if (lastTimeWrite==0)
		lastTimeWrite=mtime();
	
	//select the appropriate way to write to the file ...
	switch( fileType ) {
		case FILESOUND_TYPE_RAW: 
			//write n samples to the file ... 
			retValue = ::write(out_fd, 
					buf, 
					n * getSampleSize() * getNChannelsPlay() );
			if( retValue == -1 ) {
				printError( "write" );
			}
			break;
		case FILESOUND_TYPE_WAV:
		case FILESOUND_TYPE_MP3:
			cerr << "FileSoundDevice::write - filetype not implemented" << endl;
			break;
	}

	if( sleepTime != 0 )
		writeSleep();
	
	return retValue;
}


void FileSoundDevice::readSleep( ) {
	uint64_t currentTime;
	
	currentTime = mtime();
	
	//the sleep thingy is deactivated if sleeptime < 0
	// (the time in the computer should not go backward, right?!
// 	printf("R: %d ", currentTime - lastTimeRead );
	while (currentTime - lastTimeRead < sleepTime){
		int ret;
		int sleep = sleepTime - (currentTime-lastTimeRead);
		if( sleep < 0 ) sleep = 0;
// 		printf(" [%d] ", sleep);
		ret = filesleep( sleep * 1000);
		currentTime = mtime();
// 		printf(" %d ", currentTime - lastTimeRead);
	}
// 	printf("\n");
	
	lastTimeRead+=sleepTime;
}

void FileSoundDevice::writeSleep( ) {
	uint64_t currentTime;
	
	currentTime = mtime();

	//the sleep thingy is deactivated if sleeptime < 0
	// (the time in the computer should not go backward, right?!
// 	printf("W: %d ", currentTime - lastTimeWrite );
	while (currentTime - lastTimeWrite < sleepTime ){
		int ret;
		int sleep = sleepTime - (currentTime-lastTimeWrite);
		if( sleep < 0 ) sleep = 0;
// 		printf(" [%d] ", sleep);
		ret = filesleep( sleep * 1000);
		currentTime = mtime();
// 		printf(" %d ", currentTime - lastTimeWrite);
	}
// 	printf("\n");
	
	//lastTimeWrite = currentTime;
	lastTimeWrite += sleepTime;
}

int FileSoundDevice::getFileSize( int fd ) {
	int ret;
	int filesize;
	int currentPos;
	
	currentPos = lseek( fd, 0, SEEK_CUR );
	if( ret == -1 ) {
#ifdef DEBUG_OUTPUT
		printError("getFileSize (1)");
#endif
	}
	
	filesize = lseek( fd, 0, SEEK_END );
	if( filesize == -1 ) {
#ifdef DEBUG_OUTPUT
		printError("getFileSize(2)");
#endif
	}
	
	ret = lseek( fd, currentPos, SEEK_SET);
	if( ret == -1 ) {
#ifdef DEBUG_OUTPUT
		printError("getFileSize(3)");
#endif
	}
	
	return filesize;
}

void FileSoundDevice::printError( string func ) {
	string errStr;
	errStr = "FileSoundDevice::" + func + " - errno = ";
	switch( errno ) {
		case EACCES: errStr + "eaccess"; break;
		case EEXIST: errStr + "eexist"; break;
		case EFAULT: errStr + "efault"; break;
		case EISDIR: errStr + "eisdir"; break;
		case ELOOP: errStr + "eloop"; break;
		case EMFILE: errStr + "emfile"; break;
		case ENAMETOOLONG: errStr + "toolong"; break;
		case ENFILE: errStr + "enfile"; break;
		case ENODEV: errStr + "enodev"; break;
		case ENOENT: errStr + "enoent"; break;
		case ENOMEM: errStr + "enomem"; break;
		case ENOSPC: errStr + "enospc"; break;
		case ENOTDIR: errStr + "enotdir"; break;
		case ENXIO: errStr + "enxio"; break;
		case EOVERFLOW: errStr + "eoverflow"; break;
		case EROFS: errStr + "erofs"; break;
		case ETXTBSY: errStr + "etxtbsy"; break;
		default: errStr + "unknown";
	}
	cerr << errStr << " (check man page for explanation)" << endl;
}


