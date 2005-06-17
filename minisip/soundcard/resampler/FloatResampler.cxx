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

#ifdef FLOAT_RESAMPLER

#include"FloatResampler.h"
#include<iostream>


FloatResampler::FloatResampler( uint32_t inputFreq, uint32_t outputFreq, 
		      uint32_t duration, uint32_t nChannels ){

	src_data = new SRC_DATA();
	src_data->input_frames  = inputFreq * duration / 1000;
        src_data->output_frames = outputFreq * duration / 1000;

        inputLength  = src_data->input_frames  * nChannels;
        outputLength = src_data->output_frames * nChannels;

        src_data->src_ratio = (float)outputFreq / inputFreq;

        src_data->data_in  = new float[inputLength];
        src_data->data_out = new float[outputLength];

        src_state=src_new( 2, nChannels, &error );

}

FloatResampler::~FloatResampler(){
	if( src_data ){
		delete [] src_data->data_in;
		delete [] src_data->data_out;
		delete src_data;
	}

	if( src_state ){
		src_delete( src_state );
	}
}

void FloatResampler::resample( short * input, short * output ){
	if( src_data && src_state ){
		src_short_to_float_array( input, src_data->data_in, inputLength );
		src_process(src_state,src_data);
		src_float_to_short_array( src_data->data_out, output, outputLength );
	}
}

        
#endif

