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

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#ifndef MIKEYPAYLOADHDR_H
#define MIKEYPAYLOADHDR_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyCsIdMap.h>

#include<list>


#define HDR_DATA_TYPE_PSK_INIT    0
#define HDR_DATA_TYPE_PSK_RESP    1
#define HDR_DATA_TYPE_PK_INIT     2 
#define HDR_DATA_TYPE_PK_RESP     3
#define HDR_DATA_TYPE_DH_INIT     4
#define HDR_DATA_TYPE_DH_RESP     5
#define HDR_DATA_TYPE_ERROR       6
#define HDR_DATA_TYPE_DHHMAC_INIT 7
#define HDR_DATA_TYPE_DHHMAC_RESP 8
#define HDR_DATA_TYPE_RSA_R_INIT  9
#define HDR_DATA_TYPE_RSA_R_RESP 10

#define HDR_PRF_MIKEY_1   0
#define HDR_PRF_MIKEY_256 1
#define HDR_PRF_MIKEY_384 2
#define HDR_PRF_MIKEY_512 3

#define HDR_CS_ID_MAP_TYPE_SRTP_ID 0

#define MIKEYPAYLOAD_HDR_PAYLOAD_TYPE (-1)
/**
 * @author Erik Eliasson, Johan Bilien
*/


class LIBMIKEY_API MikeyPayloadHDR : public MikeyPayload{
	public:
		
		MikeyPayloadHDR( byte_t *start_of_header, int lengthLimit );
		MikeyPayloadHDR( int data_type, int V, int PRF_func, 
				 uint32_t CSB_id, int n_cs, int map_type, 
				 MRef<MikeyCsIdMap *> map );


		~MikeyPayloadHDR();
		virtual int length();

		virtual void writeData( byte_t * start, int expectedLength );
		virtual std::string debugDump();

		int dataType() const;
		int v() const;
		uint32_t csbId() const;
		int csIdMapType() const;
		MRef<MikeyCsIdMap *> csIdMap();
		uint8_t nCs() const;

	private:
		int version;
		int dataTypeValue;
		int vValue;
		int prfFunc;
		uint32_t csbIdValue;
		int nCsValue;
		int csIdMapTypeValue;
		MRef<MikeyCsIdMap *> csIdMapPtr;
		
};

		
#endif
