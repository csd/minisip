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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#include<config.h>
#include"SimpleResampler.h"
#include<iostream>
#include<libmutil/print_hex.h>


SimpleResampler::SimpleResampler( uint32_t inputFreq, uint32_t outputFreq, 
		      uint32_t duration, uint32_t nChannels ){

	sampleRatio = 10000*inputFreq/outputFreq;

        inputFrames  = inputFreq * duration / 1000;
        outputFrames = outputFreq * duration / 1000;

	this->nChannels = nChannels;

}

SimpleResampler::~SimpleResampler(){
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

	for( channel = 0; channel < nChannels; channel ++ ){
		inputOffset = channel;
		for (i = 0; i < outputFrames; i++){
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
	int sum;
	int sampleGroupSize;
	int sample;
	int step;
	
	if( inputFrames == 1 ){
		for( channel = 0 ; channel < nChannels ; channel ++ ){
			for( j = 0; j < outputFrames; j++ ){
				output[j*nChannels + channel] = input[channel];
			}

		}
		return;
	}


	for( channel = 0; channel < nChannels; channel ++ ){
		outputOffset = channel;
		sampleGroupSize = outputFrames / (inputFrames - 1);
		for (i = 0; i < inputFrames - 2; i++){

			step = input[(i+1)*nChannels + channel] / sampleGroupSize  - input[i*nChannels + channel] / sampleGroupSize;  

			output[outputOffset] = input[i*nChannels + channel];
			
			for (j = 1; j < sampleGroupSize; j++){
				output[outputOffset + j*nChannels] = output[outputOffset + (j - 1)*nChannels] + step;
			}

			outputOffset += sampleGroupSize*nChannels;
		}
		
		/* For the last case, we take a slightly bigger sampleGroupSize
		 * to cover the whole output buffer */
		
		sampleGroupSize = outputFrames - outputOffset / nChannels;


		output[outputOffset] = input[i*nChannels + channel];
		
		step = input[(i+1)*nChannels + channel] / sampleGroupSize  
		     - input[i*nChannels + channel] / sampleGroupSize;  
			
		for (j = 1; j < sampleGroupSize; j++){
			output[outputOffset + j*nChannels] = output[outputOffset + (j - 1)*nChannels] + step;
		}

		/*

		for( i = outputOffset ; i < outputFrames*nChannels ; i += nChannels ){
			output[i] = input[(inputFrames - 1)*nChannels + channel];
		}
		*/
	}

//	cerr << "output: " << print_hex( (unsigned char *)output, 2*nChannels*outputFrames ) << endl;


}


