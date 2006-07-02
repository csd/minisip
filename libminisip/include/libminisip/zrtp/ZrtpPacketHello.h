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
#ifndef _ZRTPPACKETHELLO_H_
#define _ZRTPPACKETHELLO_H_

#include <string.h>
#include <libminisip/zrtp/ZrtpPacketBase.h>

class ZrtpPacketHello : public ZrtpPacketBase {

 protected:
    Hello_t *helloHeader;

 public:
    ZrtpPacketHello();		 /* Creates a Hello packet with default data */
    ZrtpPacketHello(uint8_t *data); /* Creates a Hello packet from received data */
    virtual ~ZrtpPacketHello();

    uint8_t *getVersion()  { return helloHeader->version; };
    uint8_t *getClientId() { return helloHeader->clientId; };
    bool isPassive()       { return ((helloHeader->flag & 0x1) == 0x1); };

    uint8_t *getHashType(uint32_t number)    { return helloHeader->hashes[number]; };
    uint8_t *getCipherType(uint32_t number)  { return helloHeader->ciphers[number]; };
    uint8_t *getPubKeysType(uint32_t number) { return helloHeader->pubkeys[number]; };
    uint8_t *getSasType(uint32_t number)     { return helloHeader->sas[number]; };
    uint8_t *getZid()                        { return helloHeader->zid; };

    void setVersion(uint8_t *text)                   { memcpy(helloHeader->version, text, 4); }
    void setClientId(uint8_t *text)                  { memcpy(helloHeader->clientId, text, 15); }
    void setHashType(uint32_t number, char *text)    { memcpy(helloHeader->hashes[number], text, 8); };
    void setCipherType(uint32_t number, char *text)  { memcpy(helloHeader->ciphers[number], text, 8); };
    void setPubKeyType(uint32_t number, char *text)  { memcpy(helloHeader->pubkeys[number], text, 8); };
    void setSasType(uint32_t number, char *text)     { memcpy(helloHeader->sas[number], text, 8); };
    void setZid(uint8_t *text)                       { memcpy(helloHeader->zid, text, 12); };
 private:
};

#endif // ZRTPPACKETHELLO

