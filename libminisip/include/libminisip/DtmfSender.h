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

#ifndef DTMFSENDER_H
#define DTMFSENDER_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif

#ifdef _MSC_VER
typedef uint8_t unsigned char;
#else
#include<inttypes.h>
#endif

#include<libmutil/MemObject.h>

class Session;

class LIBMINISIP_API DtmfEvent{
        public:
                DtmfEvent( uint8_t symbol, uint8_t volume, uint16_t duration, bool endOfEvent, bool startOfEvent, uint32_t * ts, bool lastBlock = false ):
                        symbol(symbol),volume(volume),duration(duration),endOfEvent(endOfEvent),startOfEvent(startOfEvent),ts(ts),lastBlock(lastBlock){};

        private:
               
                uint8_t symbol;
                uint8_t volume;
                uint16_t duration;
                bool endOfEvent;
                bool startOfEvent;
                uint32_t * ts;
                bool lastBlock;
        
                friend class DtmfSender;
};
        
class LIBMINISIP_API DtmfSender {
        public:
                DtmfSender( MRef<Session *> session );
                void timeout( DtmfEvent * event );

        private:
                MRef<Session *> session;
                void sendPayload( uint8_t payload[], bool mark, uint32_t * ts );
};

#endif
