/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Joachim Orrblad <joachim@orrblad.com>
*/


#ifndef MIKEYCSIDMAP_H
#define MIKEYCSIDMAP_H

#include<libmikey/libmikey_config.h>

#define HDR_CS_ID_MAP_TYPE_SRTP_ID 0
#define HDR_CS_ID_MAP_TYPE_IPSEC4_ID 7
#include<vector>
#include<libmutil/MemObject.h>

// CS# info for srtp
class LIBMIKEY_API MikeySrtpCs{
        public:
                MikeySrtpCs( uint8_t policyNo, uint32_t ssrc, uint32_t roc=0 );
                uint8_t policyNo;
                uint32_t ssrc;
                uint32_t roc;
};

// CS# info for ipv4 IPSEC
// each CS# is related to an unique combination of spi and spiaddresses. 
class LIBMIKEY_API MikeyIPSEC4Cs{
        public:
                MikeyIPSEC4Cs( uint8_t policyNo, uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr );
                uint8_t policyNo;
                uint32_t spi;
		uint32_t spiSrcaddr;
                uint32_t spiDstaddr;
};

class LIBMIKEY_API MikeyCsIdMap : public MObject{
        public:
                virtual int length()=0;
                virtual void writeData( byte_t * start,
                                         int expectedLength )=0;
		virtual std::string debugDump()=0;
		virtual std::string getMemObjectType() const { return "MikeyCsIdMap";};
};

// Srtp map
class LIBMIKEY_API MikeyCsIdMapSrtp : public MikeyCsIdMap{
        public:
                MikeyCsIdMapSrtp();
                MikeyCsIdMapSrtp( byte_t * data, int length );
                ~MikeyCsIdMapSrtp();

                virtual int length();
                virtual void writeData( byte_t * start,
                                         int expectedLength );

		virtual std::string debugDump();

		byte_t findCsId( uint32_t ssrc );
		uint32_t findRoc( uint32_t ssrc );
		byte_t findpolicyNo( uint32_t ssrc );
		void addStream( uint32_t ssrc, uint32_t roc=0,
				byte_t policyNo=0, byte_t csId=0 );

		void setRoc( uint32_t roc, uint8_t csId );
		void setSsrc( uint32_t ssrc, uint8_t csId );

        private:
		std::vector<MikeySrtpCs *> cs;
};

// ipv4 IPSEC map
class LIBMIKEY_API MikeyCsIdMapIPSEC4 : public MikeyCsIdMap{
        public:
                MikeyCsIdMapIPSEC4();
                MikeyCsIdMapIPSEC4( byte_t * data, int length );
                ~MikeyCsIdMapIPSEC4();

                virtual int length();
                virtual void writeData( byte_t * start,
                                         int expectedLength );
		virtual std::string debugDump();
		
		MikeyIPSEC4Cs * getCsIdnumber(int number);
		byte_t findCsId( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr );
		byte_t findpolicyNo( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr);
		void addSA( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr,
				byte_t policyNo=0, byte_t csId = 0 );

        private:
		std::list<MikeyIPSEC4Cs *> cs;
};


#endif

