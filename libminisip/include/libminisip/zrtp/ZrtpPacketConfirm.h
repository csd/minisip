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
#ifndef _ZRTPPACKETCONFIRM_H_
#define _ZRTPPACKETCONFIRM_H_

#include <libminisip/zrtp/ZrtpPacketBase.h>

class ZrtpPacketConfirm : public ZrtpPacketBase {

    private:
	Confirm_t *confirmHeader;
 public:
    ZrtpPacketConfirm();		/* Creates a Confirm packet with default data */
    ZrtpPacketConfirm(uint8_t *data, uint8_t* content);	/* Creates a Confirm packet from received data */
    virtual ~ZrtpPacketConfirm();
    
    const uint8_t *getPlainText()     { return confirmHeader->plaintext; };
    uint8_t getSASFlag()              { return confirmHeader->flag; }
    const uint8_t *getHmac()          { return confirmHeader->hmac; };
    
    void setPlainText(uint8_t *text)  { memcpy(confirmHeader->plaintext, text, 15); };
    void setSASFlag(uint8_t flg)      { confirmHeader->flag = flg; };
    void setHmac(uint8_t *text)       { memcpy(confirmHeader->hmac, text, 32); };

};

#endif // ZRTPPACKETCONFIRM

