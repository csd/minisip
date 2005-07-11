/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<libminisip/Codec.h>

#include<config.h>
#include<libminisip/G711CODEC.h>
//#include<libminisip/ILBCCODEC.h>
#include<libminisip/SPEEXCODEC.h>

MRef<CodecState *> AudioCodec::createState( uint8_t payloadType ){
        switch( payloadType ){
                case 0:
                        return new G711CodecState();
//                case 97:
//                      return new ILBCCodecState();
#ifdef HAS_SPEEX
                case 114:
                        return new SpeexCodecState();
#endif
                default:
                        return NULL;
        }
}

MRef<AudioCodec *> AudioCodec::create( const std::string & description ){
        if( description == "G.711" ){
                return new G711Codec();
        }
        
//        if( description == "iLBC" ){
//                return new ILBCCODEC();
//        }
        
#ifdef HAS_SPEEX
        if( description == "speex" ){
                return new SpeexCodec();
        }
#endif

        return NULL;
}
