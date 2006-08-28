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
 * @author: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#include <libminisip/zrtp/ZrtpPacketClearAck.h>

ZrtpPacketClearAck::ZrtpPacketClearAck() {
    DEBUGOUT((fprintf(stdout, "Creating ClearAck packet without data\n")));

    allocated = malloc(sizeof (ClearAck_t));
    if (allocated == NULL) {
    }
    zrtpHeader = (zrtpPacketHeader_t *)&((ClearAck_t *)allocated)->hdr;	// the standard header

    setZrtpId();
    setLength(MESSAGE_LENGTH);
    setMessage((uint8_t*)ClearAckMsg);
}

ZrtpPacketClearAck::ZrtpPacketClearAck(uint8_t *data) {
    DEBUGOUT((fprintf(stdout, "Creating Conf2Ack packet from data\n")));

    allocated = NULL;
    zrtpHeader = (zrtpPacketHeader_t *)&((ClearAck_t *)data)->hdr;	// the standard header
}

ZrtpPacketClearAck::~ZrtpPacketClearAck() {
    DEBUGOUT((fprintf(stdout, "Deleting ClearAck packet: alloc: %x\n", allocated)));
    if (allocated != NULL) {
	free(allocated);
    }
}
