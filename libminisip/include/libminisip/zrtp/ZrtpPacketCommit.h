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
#ifndef _ZRTPPACKETCOMMIT_H_
#define _ZRTPPACKETCOMMIT_H_

#include <libminisip/zrtp/ZrtpPacketBase.h>

class ZrtpPacketCommit : public ZrtpPacketBase {

 protected:
    Commit_t *commitHeader;

 public:
    ZrtpPacketCommit();		 /* Creates a Commit packet with default data */
    ZrtpPacketCommit(char *data); /* Creates a Commit packet from received data */

    char *getHashType()    { return commitHeader->hash; };
    char *getCipherType()  { return commitHeader->cipher; };
    char *getPubKeysType() { return commitHeader->pubkey; };
    char *getSasType()     { return commitHeader->sas; };
    char *getZid()         { return commitHeader->zid; };
    char *getHvi()         { return commitHeader->hvi; };

    void setHashType(char *text)    { strncpy(commitHeader->hash, text, 8); };
    void setCipherType(char *text)  { strncpy(commitHeader->cipher, text, 8); };
    void setPubKeyType(char *text)  { strncpy(commitHeader->pubkey, text, 8); };
    void setSasType(char *text)     { strncpy(commitHeader->sas, text, 8); };
    void setZid(char *text)         { strncpy(commitHeader->zid, text, 12); };
    void setHvi(char *text)         { strncpy(commitHeader->hvi, text, 32); };
 private:
};

#endif // ZRTPPACKETCOMMIT

