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
#include<sys/soundcard.h>
#include<errno.h>
#include<iostream>
#include<sys/ioctl.h>
#include<stdio.h>
#include<unistd.h>


int main(int argc, char **argv){
	int handle = open("/dev/dsp",/*O_WRONLY*/ O_RDWR);
	if (handle == -1){
		perror("open /dev/dsp");
		return -1;
	}
	
	
	int setting =0x000A000A; //10 fragments, 2^10=1024 as buffer size
	if (ioctl(handle, SNDCTL_DSP_SETFRAGMENT, &setting)==-1){
		perror("ioctl, SNDCTL_DSP_SETFRAGMENT (set buffer size)");
	}

	
	int channels=0; //mono
	if (ioctl(handle, SNDCTL_DSP_STEREO, &channels)==-1){
		perror("ioctl, SNDCTL_DSP_STEREO (tried to set to mono)");
	}
	if (channels!=0){
		cerr << "ERROR: could not set to mono - running stereo instead"<< endl;
	}

	int format = AFMT_S16_LE;
	if (ioctl(handle, SNDCTL_DSP_SETFMT, &format)==-1){
		perror("ioctl, SNDCTL_DSP_SETFMT (failed to set format to AFMT_S16_LE)");
	}
	
	int speed = 8000;
	if (ioctl(handle, SNDCTL_DSP_SPEED, &speed)==-1){
		perror("ioctl, SNDCTL_DSP_SPEED (tried to set sample rate to 8000)");
	}
	cerr << "Sample speed set to "<< speed << endl;

	short data[10000];
	for (int i=0; i< 10000; i++)
		data[i]= ((i%2)==0) ? 1000:-1000;
	
	write(handle, data, 10000);
	
	if (ioctl(handle, SNDCTL_DSP_SYNC)==-1){
		perror("ioctl sync error");
	}
	
	for (int i=0; i< 200000; i++){
		short sample;
		int n = read(handle, &sample, sizeof (sample));
		if (n!=sizeof(sample))
			perror("read did not read a short");
		printf("%.4X ",sample);
		if (i%10==0)
			printf("\n");
	}
	
	close(handle);

}
