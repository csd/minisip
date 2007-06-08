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
#include<libmutil/merror.h>
#include<libminisip/media/soundcard/Resampler.h>
#include"SimpleResampler.h"

#include<iostream>

using namespace std;

ResamplerPlugin::ResamplerPlugin( MRef<Library*> lib ): MPlugin( lib ){
}

ResamplerPlugin::~ResamplerPlugin(){
}


ResamplerRegistry::ResamplerRegistry(){
	registerPlugin( new SimpleResamplerPlugin( NULL ) );
}

MRef<Resampler *> ResamplerRegistry::create( uint32_t inputFreq, uint32_t outputFreq, uint32_t duration, uint32_t nChannels ){

	MRef<MPlugin *> plugin;

	plugin = findPlugin("float_resampler");

	if( !plugin )
		plugin = findPlugin("simple_resampler");

	if( !plugin ){
		merror("Can't create resampler");
		return NULL;
	}

	MRef<ResamplerPlugin *> resampler = dynamic_cast<ResamplerPlugin *>(*plugin);

	if( !resampler ){
		merror("dynamic_cast<ResamplerPlugin *> failed");
		return NULL;
	}

	mdbg << "Creating resampler " << resampler->getName() << endl;

	return resampler->createResampler( inputFreq, outputFreq,
					   duration, nChannels );
}
