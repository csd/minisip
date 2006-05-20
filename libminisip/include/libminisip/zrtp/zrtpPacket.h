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

#ifndef ZRTPPACKET_H
#define ZRTPPACKET_H

#include <config.h>
#include<libminisip/libminisip_config.h>

#define	ZRTP_EXT_PACKET		0x505a

#define ZRTP_WORD_SIZE		4
#define ZRTP_MSG_SIZE		8 	// 2 * WORD_SIZE

#define MESSAGE_LENGTH		2
typedef struct zrtpPacketHeader {
    uint16_t    zrtpId;
    uint16_t    length;
    char        message[8];
} zrtpPacketHeader_t;


#define HELLO_LENGTH            48 /* plus the MESSAGE_LENGTH = 50 */
typedef struct Hello { 
    char	version[4];
    char	clientId[15];
    uint8_t	flag;
    char        hashes[5][8];
    char        ciphers[5][8];
    char	pubkeys[5][8];
    char	sas[5][8];
    uint8_t     zid[12];
} Hello_t;

typedef struct HelloPacket {
    zrtpPacketHeader_t hdr;
    Hello_t hello;
} HelloPacket_t;

typedef struct HelloAck {	/* Length is MESSAGE_LENGTH */
    zrtpPacketHeader_t hdr;
} HelloAck_t;

#define COMMIT_LENGTH           19 /* plus MESSAGE_LENGTH = 21 */
typedef struct Commit {
    char	zid[12];
    char        hash[8];
    char        cipher[8];
    char	pubkey[8];
    char	sas[8];
    char	hvi[32];
} Commit_t;

typedef struct CommitPacket {
    zrtpPacketHeader_t hdr;
    Commit_t commit;
} CommitPacket_t;

#define DHPART_LENGTH		10 /* plus MESSAGE_LENGTH + pvr length */
typedef struct DHPart {
    char rs1Id[8];
    char rs2Id[8];
    char sigsId[8];
    char srtpsId[8];
    char otherSecretId[8];
}  DHPart_t;

typedef struct DHPartPacket {
    zrtpPacketHeader_t hdr;
} DHPartPacket_t;


#define CONFIRM_LENGTH		12 /* plus MESSAGE_LENGTH = 14 */
typedef struct Confirm {
    char	plaintext[15];
    uint8_t	flag;
    char	hmac[32];
} Confirm_t;

typedef struct ConfirmPacket {
    zrtpPacketHeader_t hdr;
} ConfirmPacket_t;

#define CONF2ACK_LENGTH         2
typedef struct Conf2Ack {
    zrtpPacketHeader_t hdr;
} Conf2Ack_t;

#define ERROR_LENGTH		2 /* plus MESSAGE_LENGTH = 4 */
typedef struct Error {
    char type[8];
} Error_t;

typedef struct ErrorPacket {
    zrtpPacketHeader_t hdr;
    Error_t error;
} ErrorPacket_t;


#endif // ZRTPPACKET_H

