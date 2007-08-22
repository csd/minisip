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

#ifndef _ZRTPDH_H__
#define _ZRTPDH_H__

#include <libmcrypto/config.h>

#include <libmutil/mtypes.h>

/**
 * Implementation of Diffie-Helman for ZRTP
 *
 * This class uses the OpenSSL fucntions to generate and compute the
 * Diffie-Helman public and secret data and the shared secret. According to
 * the ZRTP specification we use the MODP groups as defined by RFC3526 for
 * length 3072 and 4096.
 */

class LIBMCRYPTO_API ZrtpDH {
    
private:
    void *priv;
    
public:
    ZrtpDH(int32_t pkLength);
    ~ZrtpDH();

    int32_t generateKey();
    int32_t getSecretSize() const;
    int32_t getPubKeySize() const;
    int32_t getPubKeyBytes(uint8_t *buf) const;
    int32_t computeKey(uint8_t *pubKeyBytes, int32_t length, uint8_t *secret);
    void random(uint8_t *buf, int32_t length) const;
    int32_t checkPubKey(uint8_t *pubKeyBytes, int32_t length) const;
};

#endif // ZRTPDH_H
