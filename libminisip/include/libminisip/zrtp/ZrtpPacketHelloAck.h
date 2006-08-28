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

#ifndef _ZRTPPACKETHELLOACK_H_
#define _ZRTPPACKETHELLOACK_H_

#include <libminisip/zrtp/ZrtpPacketBase.h>

/**
 * Implement the HelloAck packet.
 *
 * The ZRTP simple message HelloAck. The implementation sends this
 * after receiving a Hello packet. Sending a HelloAck is optional, a
 * Commit can be sent instead.
 *
 * @author Werner Dittmann <Werner.Dittmann@t-online.de>
 */

class ZrtpPacketHelloAck : public ZrtpPacketBase {

 public:
    ZrtpPacketHelloAck();		/* Creates a HelloAck packet with default data */
    ZrtpPacketHelloAck(char* data);	/* Creates a HelloAck packet from received data */
    virtual ~ZrtpPacketHelloAck();

 private:
};

#endif // ZRTPPACKETHELLOACK

