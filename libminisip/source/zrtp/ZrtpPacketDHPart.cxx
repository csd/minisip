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

#include <libminisip/zrtp/ZrtpPacketDHPart.h>
#include <malloc.h>


ZrtpPacketDHPart::ZrtpPacketDHPart(PKType pkt) {

    int length = sizeof(zrtpPacketHeader_t) + sizeof(DHPart_t);
    void *p;

    length += ((pkt == DH3072) ? 384 : 512); // length according to DH type
    p = malloc(length);

    if ( p == NULL) {
	// TODO error handling
    }

    pktype = pkt;
    
    zrtpHeader = (zrtpPacketHeader_t *)&((DHPartPacket_t *)p)->hdr;	// the standard header
    pv = ((char *)p) + sizeof(zrtpPacketHeader_t); 			// point to the public key value
    DHPartHeader = (DHPart_t *)(((char *)p) + sizeof(zrtpPacketHeader_t) + ((pkt == DH3072) ? 384 : 512));

    setZrtpId();
    setLength(DHPART_LENGTH + MESSAGE_LENGTH + ((pkt == DH3072) ? 96 : 128));
}

ZrtpPacketDHPart::ZrtpPacketDHPart(char *data, PKType pkt) {
    zrtpHeader = (zrtpPacketHeader_t *)&((DHPartPacket_t *)data)->hdr;	// the standard header
    pv = data + sizeof(zrtpPacketHeader_t);
    DHPartHeader = (DHPart_t *)(data + sizeof(zrtpPacketHeader_t) + ((pkt == DH3072) ? 384 : 512));

    pktype = pkt;
}
