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

#ifndef _ZRTPPACKETBASE_H_
#define _ZRTPPACKETBASE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <libminisip/zrtp/zrtpPacket.h>
#include <libminisip/zrtp/ZrtpTextData.h>

enum PKType {
	DH3072,
	DH4096
};

const uint16_t zrtpId = ZRTP_EXT_PACKET;

class ZrtpPacketBase {

  protected:
    zrtpPacketHeader_t *zrtpHeader;

  public:
    bool isZrtpPacket()  { return (ntoh16(zrtpHeader->zrtpId) == zrtpId); };
    uint16_t getLength() { return ntoh16(zrtpHeader->length); };
    char *getMessage()   { return zrtpHeader->message; };

    void setLength(uint16_t len) { zrtpHeader->length = hton16(len); };
    void setMessage(char *msg)   { strncpy(zrtpHeader->message, msg, ZRTP_MSG_SIZE); };
    void setZrtpId()             { zrtpHeader->zrtpId = hton16(zrtpId); }

    void hexdump(FILE *f, const char *title, int l);
  private:
};

#endif // ZRTPPACKETBASE
