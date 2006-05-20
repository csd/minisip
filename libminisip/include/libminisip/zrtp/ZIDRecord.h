/*
  Copyright (C) 2006 Werner Dittmann 
 
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

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#ifndef _ZIDRECORD_H_
#define _ZIDRECORD_H_

#include <string.h>
#include <stdint.h>

#define IDENTIFIER_LEN  12
#define RS_LENGTH       32

typedef struct zidrecord {
    int8_t recValid,		// if 1 record is valid, if 0: invalid
	recVersion,			// record version, currently unused
	rs1Valid,			// if 1 RS1 contains valid data
	rs2Valid;			// if 1 RS2 contains valid data
    uint8_t identifier[IDENTIFIER_LEN]; // the peer's ZID
    uint8_t rs1Data[RS_LENGTH], rs2Data[RS_LENGTH]; // the peer's RS data
} zidrecord_t;

/**
 * This class implements the ZID record.
 * The ZID record holds data about a peer. According to ZRTP specification
 * we use a ZID to identify a peer. ZRTP uses the RS (Retained Secret) data
 * to construct shared secrets.
 * <p/>
 * NOTE: ZIDRecord has ZIDFile as friend. ZIDFile knows about the private
 *	 data of ZIDRecord - please keep both classes synchronized.
 */

class ZIDRecord {
    friend class ZIDFile;

 private:
    zidrecord_t record;
    unsigned long position;

 public:
    ZIDRecord(char *idData) {
	memset(&record, 0, sizeof(zidrecord_t));
	memcpy(record.identifier, idData, IDENTIFIER_LEN); };

    int32_t isRs1Valid() { return record.rs1Valid; };
    int32_t isRs2Valid() { return record.rs2Valid; };

    const uint8_t *getRs1() { return record.rs1Data; };
    const uint8_t *getRs2() { return record.rs2Data; };

    void setNewRs1(const char *data);
};

#endif // ZIDRECORD

