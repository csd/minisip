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

/* Copyright (C) 2004-5
 *
 * Authors: Cesc Santasusana <cesc dot santa at gmail dot com>
*/

/* Name
 * 	AudioMixer.h
 * Author
 * 	Cesc Santasusana <cesc dot santa at gmail dot com>
 * Purpose
 * 	Base class for audio mixing implementations.
*/


#include<config.h>

#include<libminisip/media/soundcard/AudioMixer.h>
#include<string.h>
#include<stdio.h>

using namespace std;

AudioMixer::AudioMixer() {
	outputBuffer = NULL;
	
	inputBuffer = NULL;
	
	mixBuffer = NULL;
	
	numChannels = 0;
	
	frameSize = 0;
}

AudioMixer::~AudioMixer() {
	if( outputBuffer )
		delete [] outputBuffer;
	if( inputBuffer )
		delete [] inputBuffer;
	if( mixBuffer )
		delete [] mixBuffer;
}

bool AudioMixer::init( uint32_t numChannels_ ) {
	
	bool hasChanged = (this->numChannels != numChannels_ );
	
	this->numChannels = numChannels_;
	this->frameSize = (SOUND_CARD_FREQ * 20) / 1000;	

	//we may need to re-new the buffers if 
	//either of these change ... 
	if( !outputBuffer || hasChanged ) {
		massert(this->numChannels * this->frameSize);
		outputBuffer = new short[this->numChannels * this->frameSize];
		memset(outputBuffer,0,this->numChannels * this->frameSize*sizeof(short));
	}
	if( !inputBuffer || hasChanged ) {
		inputBuffer = new short[this->numChannels * this->frameSize];
		memset(inputBuffer, 0, this->numChannels * this->frameSize*sizeof(short));
	}
	if( !mixBuffer || hasChanged ) {
		mixBuffer = new int32_t[this->numChannels * this->frameSize];
		memset(mixBuffer,0,this->numChannels * this->frameSize*sizeof(int32_t));
	}
#ifdef DEBUG_OUTPUT	
	cerr << "AudioMixer::init() ... initializing audio mixer" << endl;
	printf( "AudioMixer::init - frameSize = %d, numChannels = %d\n", frameSize, numChannels );
#endif
	return true;
}
