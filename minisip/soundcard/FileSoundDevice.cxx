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

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<signal.h>
#include<sys/time.h>
#include<time.h>
#include<libmutil/itoa.h>
#include<libmutil/mtime.h>
#include<unistd.h>
#include<stdint.h>


#include"SoundDevice.h"
#include"FileSoundDevice.h"

#ifdef WIN32
#include<winsock2.h>
int usleep( unsigned long usec ){
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = (long)usec;

	return select (0, NULL, NULL, NULL, &tv);
}
#endif




FileSoundDevice::FileSoundDevice(string in_file, string out_file, int nChannels, int32_t speed): 
SoundDevice("!notused_filesounddevice!"),
	in_file(in_file), 
	out_file(out_file)
{
	assert(nChannels==1 || nChannels==2);

	this->in_fd=-1;	
	this->out_fd=-1;	


	cerr << "Setting nChannels "<< nChannels<< endl;
	samplingRate=8000;
	nChannels=1;
	format=0;
	sampleSize=160;
	openedPlayback=false;
	openedRecord=false;
									
}

int FileSoundDevice::openRecord(int32_t samplerate, int nChannels, int format){
	this->nChannelsRecord = nChannels;

	in_fd=::open(in_file.c_str(), O_RDONLY);
	if (in_fd==-1){
		perror(("open "+in_file).c_str());
		exit(-1); //FIX: handle nicer - exception
	}
	openedRecord=true;

	return 0;
}

int FileSoundDevice::openPlayback(int32_t samplerate, int nChannels, int format){
	this->nChannelsPlay = nChannels;

	out_fd=::open(out_file.c_str(), O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);
	if (out_fd==-1){
		perror(("open "+out_file).c_str());
		exit(-1); //FIX: handle nicer - exception
	}

	openedPlayback=true;

	return 0;
}

int FileSoundDevice::closePlayback(){
	openedPlayback=false;
	return ::close(out_fd);
}

int FileSoundDevice::closeRecord(){
	openedRecord=false;
	return ::close(in_fd);
}


void FileSoundDevice::sync(){
	cerr << "ERROR: sync unimplemented for file sound device"<< endl;
}

int FileSoundDevice::read(byte_t *buf, uint32_t n){
	static uint64_t last_time;
	if (last_time==0){
		last_time = mtime();
	}

	::read(in_fd, buf, n*2);
	
	uint64_t t = mtime();
	
	while (t - last_time < 20){
		usleep((t-last_time)*1000);
		t = mtime();
	}
	
	last_time+=20;

	return n;
}

int FileSoundDevice::write(byte_t *buf, uint32_t n){
	static uint64_t last_time;

	if (last_time==0)
		last_time=mtime();
	::write(out_fd, buf, n*2);

	uint64_t t = mtime();

	while (t - last_time < 20){
		usleep((t-last_time)*1000);
		t = mtime();
	}
	
	last_time=t;
		
	return n;
}


