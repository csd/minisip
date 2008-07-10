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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#include<config.h>

#include"SimpleResampler.h"
#include<iostream>
#include<string.h>


SimpleResampler::SimpleResampler( uint32_t inputFreq, uint32_t outputFreq, 
		      uint32_t duration, uint32_t nChannels_ ){

	sampleRatio = 10000*inputFreq/outputFreq;

        inputFrames  = inputFreq * duration / 1000;
        outputFrames = outputFreq * duration / 1000;

	this->nChannels = nChannels_;

	previousFrame = new short[ nChannels ];
	memset( previousFrame, '\0', nChannels * sizeof( short ) );

}

SimpleResampler::~SimpleResampler(){
	delete [] previousFrame;
}

void SimpleResampler::resample( short * input, short * output ){
	if( sampleRatio / 10000 > 1 ){
		downSample( input, output );
	}
	else if( sampleRatio / 10000 == 0 ){
		upSample( input, output );
	}
	else{
		if( input != output && inputFrames == outputFrames ){
			memcpy( output, input, inputFrames*nChannels*sizeof(short) );
		}
	}
}

void SimpleResampler::downSample( short * input, short * output ){

	int i, j, channel;
	int epsilon = 0;
	int inputOffset = 0;
	int sum;
	int sampleGroupSize;
	int sample;

	for( channel = 0; channel < (int)nChannels; channel ++ ){
		inputOffset = channel;
		for (i = 0; i < (int)outputFrames; i++){
			epsilon += sampleRatio;
			sampleGroupSize = epsilon / 10000;
			epsilon -= sampleGroupSize * 10000;

			sum = 0;
			for (j = 0; j < sampleGroupSize; j++){
				sample = input[inputOffset + channel
					+ j * nChannels ];
				sum += sample;
			}

			output[channel + nChannels * i] = (sum / sampleGroupSize);

			inputOffset += sampleGroupSize*nChannels;
		}
	}
}

/* Linear interpolation */

void SimpleResampler::upSample( short * input, short * output ){
	int i, j, channel;
//	int epsilon = 0;
	int outputOffset = 0;
	//int sum;
	int sampleGroupSize;
	//int sample;
	int step;
	int lastInputSample;
	
	if( inputFrames == 1 ){
		for( channel = 0 ; channel < (int)nChannels ; channel ++ ){
			for( j = 0; j < (int)outputFrames; j++ ){
				output[j*nChannels + channel] = input[channel];
			}

		}
		return;
	}


	for( channel = 0; channel < (int)nChannels; channel ++ ){
		outputOffset = channel;
		sampleGroupSize = outputFrames / inputFrames;
		lastInputSample = previousFrame[ channel ];
		for (i = 0; i < (int)inputFrames - 1; i++){

			step = input[(i)*nChannels + channel] / sampleGroupSize  - lastInputSample / sampleGroupSize;  

			output[outputOffset] = lastInputSample;
			
			for (j = 1; j < sampleGroupSize; j++){
				output[outputOffset + j*nChannels] = output[outputOffset + (j - 1)*nChannels] + step;
			}

			outputOffset += sampleGroupSize*nChannels;

			lastInputSample = input[i*nChannels + channel];
		}
		
		/* For the last case, we take a slightly bigger sampleGroupSize
		 * to cover the whole output buffer */
		
		sampleGroupSize = outputFrames - outputOffset / nChannels;

		output[outputOffset] = lastInputSample;
		
		step = input[(i)*nChannels + channel] / sampleGroupSize  
		     - lastInputSample / sampleGroupSize;  
			
		for (j = 1; j < sampleGroupSize; j++){
			output[outputOffset + j*nChannels] = output[outputOffset + (j - 1)*nChannels] + step;
		}

	}
	
	/* Save the last input frame for smooth transition */
	memcpy( previousFrame, &input[ (inputFrames - 1)*nChannels ], nChannels * sizeof(short) );

}


