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

#ifndef _ZRTPPACKETGOCLEAR_H_
#define _ZRTPPACKETGOCLEAR_H_

#include <libminisip/zrtp/ZrtpPacketBase.h>

/**
 * Implement the GoClear packet.
 *
 * The ZRTP message GoClear. The implementation sends this
 * to order the peer to switch to clear mode (non-SRTP mode).
 *
 * @author Werner Dittmann <Werner.Dittmann@t-online.de>
 */

class ZrtpPacketGoClear : public ZrtpPacketBase {

 protected:
    GoClear_t* clearHeader;

 public:
     ZrtpPacketGoClear();		/* Creates a Error packet with default data */
     ZrtpPacketGoClear(uint8_t* data);	/* Creates a Error packet from received data */
     virtual ~ZrtpPacketGoClear();

     uint8_t* getClearHmac() { return clearHeader->clearHmac; };

     void setClearHmac(uint8_t *text) { memcpy(clearHeader->clearHmac, text, 32); };

 private:
};

#endif // ZRTPPACKETGOCLEAR

