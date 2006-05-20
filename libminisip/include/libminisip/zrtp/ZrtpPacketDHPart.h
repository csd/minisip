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
#ifndef _ZRTPPACKETDHPART_H_
#define _ZRTPPACKETDHPART_H_

#include <libminisip/zrtp/ZrtpPacketBase.h>

class ZrtpPacketDHPart : public ZrtpPacketBase {

 protected:
    char *pv;
    DHPart_t *DHPartHeader;

 public:
    ZrtpPacketDHPart(PKType pkt);		/* Creates a DHPart packet with default data */
    ZrtpPacketDHPart(char *data, PKType pkt);   /* Creates a DHPart packet from received data */

    char *getPv()             { return pv; }
    char *getRs1Id()          { return DHPartHeader->rs1Id; };
    char *getRs2Id()          { return DHPartHeader->rs2Id; };
    char *getSigsId()         { return DHPartHeader->sigsId; };
    char *getSrtpsId()        { return DHPartHeader->srtpsId; };
    char *getOtherSecrectId() { return DHPartHeader->otherSecretId; };

    void setPv(char *text) 	      { strncpy(pv, text, ((pktype == DH3072) ? 384 :512)); };
    void setRs1Id(char *text)         { strncpy(DHPartHeader->rs1Id, text, 8); };
    void setRs2Id(char *text)         { strncpy(DHPartHeader->rs2Id, text, 8); };
    void setSigsId(char *text)        { strncpy(DHPartHeader->sigsId, text, 8); };
    void setSrtpsId(char *text)       { strncpy(DHPartHeader->srtpsId, text, 8); };
    void setOtherSecretId(char *text) { strncpy(DHPartHeader->otherSecretId, text, 8); };

 private:
    PKType pktype;
};

#endif // ZRTPPACKETDHPART

