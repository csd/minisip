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

#ifndef _DH_H__
#define _DH_H__

#include <libmcrypto/config.h>

#include <stdint.h>

#include <openssl/crypto.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include <openssl/dh.h>

class LIBMCRYPTO_API DHMethods {
    
private:
    DH *ctx;
    
public:
    DHMethods(int32_t pkLength);
    ~DHMethods();

    int32_t generateKey()                    { return DH_generate_key(ctx); };
    int32_t getPubKeySize()                  { return BN_num_bytes((ctx->pub_key)); };
    int32_t getPubKeyBytes(uint8_t *buffer)  { return BN_bn2bin(ctx->pub_key, buffer); };

    int32_t computeKey(uint8_t *pubKeyBytes, int32_t length, uint8_t *secret);
};

#endif // DH_H__
