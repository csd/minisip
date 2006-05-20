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

#ifndef _ZRTPCALLBACK_H_
#define _ZRTPCALLBACK_H_

#include <stdint.h>
#include <libminisip/zrtp/ZrtpPacketBase.h>

/**
 * This class defines the callback functions required by ZRTP.
 * This class is a pure abstract class, aka Interface in Java, that
 * specifies the callback interface for the ZRTP implementation.
 * The ZRTP implementation needs these functions to send data
 * via the RTP/SRTP stack and to set timers.
 */

class ZrtpCallback {

 public:
    /**
     * Send a ZRTP packet via RTP.
     *
     * @param data
     *    Points to ZRTP packet to send as RTP extension header.
     * @param length
     *    The length in bytes of the data
     * @return
     *    zero if sending failed, one if packet was send
     */
    virtual int32_t sendDataRTP(char *data, int32_t length) =0;

    /**
     * Send a ZRTP packet via SRTP.
     *
     * @param dataHeader
     *    Points to ZRTP packet to send as RTP extension header
     * @param lengthHeader
     *    The length in bytes of the header data
     * @param dataContent
     *    Points to the data to send as SRTP packet content.
     * @param lengthConten
     *    The length in bytes of the content data
     * @return
     *    zero if sending failed, one if packet was send
     */
    virtual int32_t sendDataSRTP(char *dataHeader, int32_t lengthHeader,
				 char *dataContent, int32_t lengthContent) =0;

    /**
     * Activate timer.
     *
     * @param packet
     *    The ZRTP packet to send as RTP extension header and
     *    as SRTP packet content.
     * @return
     *    zero if activation failed, one if timer was activated
     */
    virtual int32_t activateTimer(int32_t time) =0;
};

#endif // ZRTPCALLBACK

