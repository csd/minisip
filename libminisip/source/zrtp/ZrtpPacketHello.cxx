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

#include <libminisip/zrtp/ZrtpPacketHello.h>
#include <malloc.h>


ZrtpPacketHello::ZrtpPacketHello() {
    DEBUGOUT((fprintf(stdout, "Creating Hello packet without data\n")));

    allocated = malloc(sizeof (HelloPacket_t));

    if (allocated == NULL) {
    }

    zrtpHeader = (zrtpPacketHeader_t *)&((HelloPacket_t *)allocated)->hdr;	// the standard header
    helloHeader = (Hello_t *)&((HelloPacket_t *)allocated)->hello;

    setZrtpId();
    setLength(HELLO_LENGTH + MESSAGE_LENGTH);
    setMessage((uint8_t*)HelloMsg);

    setClientId((uint8_t*)clientId);
    setVersion((uint8_t*)zrtpVersion);

    setHashType(0, supportedHashes[0]);
    setHashType(1, supportedHashes[1]);
    setHashType(2, supportedHashes[2]);
    setHashType(3, supportedHashes[3]);
    setHashType(4, supportedHashes[4]);

    setCipherType(0, supportedCipher[0]);
    setCipherType(1, supportedCipher[1]);
    setCipherType(2, supportedCipher[2]);
    setCipherType(3, supportedCipher[3]);
    setCipherType(4, supportedCipher[4]);

    setPubKeyType(0, supportedPubKey[0]);
    setPubKeyType(1, supportedPubKey[1]);
    setPubKeyType(2, supportedPubKey[2]);
    setPubKeyType(3, supportedPubKey[3]);
    setPubKeyType(4, supportedPubKey[4]);

    setSasType(0, supportedSASType[0]);
    setSasType(1, supportedSASType[1]);
    setSasType(2, supportedSASType[2]);
    setSasType(3, supportedSASType[3]);
    setSasType(4, supportedSASType[4]);
}

ZrtpPacketHello::ZrtpPacketHello(uint8_t *data) {
    DEBUGOUT((fprintf(stdout, "Creating Hello packet from data\n")));

    allocated = NULL;
    zrtpHeader = (zrtpPacketHeader_t *)&((HelloPacket_t *)data)->hdr;	// the standard header
    helloHeader = (Hello_t *)&((HelloPacket_t *)data)->hello;
}

ZrtpPacketHello::~ZrtpPacketHello() {
    DEBUGOUT((fprintf(stdout, "Deleting Hello packet: alloc: %x\n", allocated)));
    if (allocated != NULL) {
	free(allocated);
    }
}
