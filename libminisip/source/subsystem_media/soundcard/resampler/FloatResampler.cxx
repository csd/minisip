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


#include"FloatResampler.h"
#include<iostream>


static std::list<std::string> pluginList;
static bool initialized;

extern "C" LIBMINISIP_API
std::list<std::string> *mfloat_resampler_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mfloat_resampler_LTX_getPlugin( MRef<Library*> lib ){
	return new FloatResamplerPlugin( lib );
}


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

        

