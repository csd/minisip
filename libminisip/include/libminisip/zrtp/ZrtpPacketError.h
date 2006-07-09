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
#ifndef _ZRTPPACKETERROR_H_
#define _ZRTPPACKETERROR_H_

#include <libminisip/zrtp/ZrtpPacketBase.h>

class ZrtpPacketError : public ZrtpPacketBase {

 protected:
    Error_t* errorHeader;

 public:
    ZrtpPacketError();		/* Creates a Error packet with default data */
    ZrtpPacketError(char* data);	/* Creates a Error packet from received data */
    virtual ~ZrtpPacketError();

    uint8_t* getErrorType() { return errorHeader->type; };

    void setErrorType(uint8_t *text) { memcpy(errorHeader->type, text, 8); };

 private:
};

#endif // ZRTPPACKETERROR

