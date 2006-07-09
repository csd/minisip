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

#include <libminisip/zrtp/ZrtpPacketConfirm.h>
#include <malloc.h>

ZrtpPacketConfirm::ZrtpPacketConfirm() {
    DEBUGOUT((fprintf(stdout, "Creating Confirm packet without data\n")));

    allocated = malloc(sizeof (ConfirmPacket_t));
    if (allocated == NULL) {
    }
    zrtpHeader = (zrtpPacketHeader_t *)&((ConfirmPacket_t *)allocated)->hdr;	// the standard header
    confirmHeader = (Confirm_t *)&((ConfirmPacket_t *)allocated)->confirm;

    setZrtpId();
    setLength(MESSAGE_LENGTH);
}

ZrtpPacketConfirm::ZrtpPacketConfirm(uint8_t* data, uint8_t* content) {
    DEBUGOUT((fprintf(stdout, "Creating Confirm packet from data\n")));

    allocated = NULL;
    zrtpHeader = (zrtpPacketHeader_t *)&((ConfirmPacket_t *)data)->hdr;	// the standard header
    confirmHeader = (Confirm_t *)content; 
}

ZrtpPacketConfirm::~ZrtpPacketConfirm() {
    DEBUGOUT((fprintf(stdout, "Deleting Confirm packet: alloc: %x\n", allocated)));
    if (allocated != NULL) {
	free(allocated);
    }
}
