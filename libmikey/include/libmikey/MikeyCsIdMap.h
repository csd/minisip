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

#ifndef MIKEYCSIDMAP_H
#define MIKEYCSIDMAP_H

#define HDR_CS_ID_MAP_TYPE_SRTP_ID 0
#include<list>
#include<libmutil/MemObject.h>

class MikeySrtpCs{
        public:
                MikeySrtpCs( uint8_t policyNo, uint32_t ssrc, uint32_t roc=0 );

                uint8_t policyNo;
                uint32_t ssrc;
                uint32_t roc;
};

class MikeyCsIdMap : public MObject{
        public:
                virtual int length()=0;
                virtual void writeData( byte_t * start,
                                         int expectedLength )=0;
		virtual std::string getMemObjectType(){ return "MikeyCsIdMap";};
};

class MikeyCsIdMapSrtp : public MikeyCsIdMap{
        public:
                MikeyCsIdMapSrtp();
                MikeyCsIdMapSrtp( byte_t * data, int length );
                ~MikeyCsIdMapSrtp();

                virtual int length();
                virtual void writeData( byte_t * start,
                                         int expectedLength );

		byte_t findCsId( uint32_t ssrc );
		uint32_t findRoc( uint32_t ssrc );
		void addStream( uint32_t ssrc, uint32_t roc=0,
				byte_t policyNo=0, byte_t csId=0 );

        private:
		std::list<MikeySrtpCs *> cs;
};


#endif

