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
*/

#include<config.h>

#include<libminisip/media/codecs/Codec.h>
#include"G711CODEC.h"

using namespace std;

AudioCodecRegistry::AudioCodecRegistry(){
	registerPlugin( new G711Codec( NULL, G711U ) );
	registerPlugin( new G711Codec( NULL, G711A ) );
}

MRef<CodecState *> AudioCodecRegistry::createState( uint8_t payloadType ){
	list< MRef<MPlugin*> >::iterator iter;
	list< MRef<MPlugin*> >::iterator stop = plugins.end();

	for( iter = plugins.begin(); iter != stop; iter++ ){
		MRef<MPlugin*> plugin = *iter;

		MRef<AudioCodec*> codec = dynamic_cast<AudioCodec*>(*plugin);

		if( !codec ){
			merr << "Not an AudioCodec? " << plugin->getName() << endl;
		}

		if( codec && codec->getSdpMediaType() == payloadType ){
			return codec->newInstance();
		}
	}

	merr << "AudioCodec not found pt: " << payloadType << endl;
	return NULL;
}


MRef<AudioCodec *> AudioCodecRegistry::create( const std::string & description ){
	list< MRef<MPlugin*> >::iterator iter;
	list< MRef<MPlugin*> >::iterator stop = plugins.end();

	for( iter = plugins.begin(); iter != stop; iter++ ){
		MRef<MPlugin*> plugin = *iter;

		MRef<AudioCodec*> codec = dynamic_cast<AudioCodec*>(*plugin);

		if( !codec ){
			merr << "Not an AudioCodec? " << plugin->getName() << endl;
		} 

		if( codec && codec->getCodecName() == description ){
			return codec;
		}
	}

	mdbg << "AudioCodec not found name: " << description << endl;
	return NULL;
}

