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

#include <libminisip/zrtp/ZrtpPacketHelloAck.h>
#include <malloc.h>

ZrtpPacketHelloAck::ZrtpPacketHelloAck() {

  void *p = malloc(sizeof (HelloAck_t));
  if ( p == NULL) {
  }
    zrtpHeader = (zrtpPacketHeader_t *)&((HelloAck_t *)p)->hdr;	// the standard header

    setZrtpId();
    setLength(MESSAGE_LENGTH);
    setMessage(HelloAckMsg);
}

ZrtpPacketHelloAck::ZrtpPacketHelloAck(char *data) {
    zrtpHeader = (zrtpPacketHeader_t *)&((HelloAck_t *)data)->hdr;	// the standard header
}
