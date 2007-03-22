/**
Copyright (C) 2005, 2004 Erik Eliasson, Pan Xuan

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
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

* Authors: Erik Eliasson <eliasson@it.kth.se>
*          Pan Xuan <xuan@kth.se>

*/

#ifndef GDENUM_H
#define GDENUM_H

class GDEnum{
public:
	GDEnum();
	~GDEnum();
	enum SW_1_2{
		
		SUCCESS = 0x9000,
		
		APPLET_SELECT_FAILURE = 0x6999,
		
		PIN_RETRY_TIMER_2 = 0x63C2,
		
		PIN_RETRY_TIMER_1 = 0x63C1,
		
		PIN_RETRY_TIMER_0 = 0x63C0,
		
		PIN_NOT_FOUND = 0x6A88,
		
		AUTH_LEVEL_INSUFFICIENT = 0x6982,
		
		GET_DH_PUBKEY_FAILURE = 0x6001,
		
		ENCRY_ENV_KEY_GEN_FAILURE = 0x6002,

		SIGNING_FAILURE = 0x6004,

		DH_TEK_GEN_FAILURE = 0x6006,
		
		PK_TEK_GEN_FAILURE = 0x6007,
		
		RANDOM_GEN_FAILURE = 0x6008,

		DH_PRIKEY_GEN_FAILURE = 0x6009

		
	
	};

	enum CLA{
		
		GD_CLA = 0x00,
		
		MIKEY_CLA = 0xB0
	};

	enum INS{

		VERIFY_INS = 0x10,

		UPDATE_INS = 0xD0,

		DH_PARA_INS = 0x20,

		PKE_INS = 0x30,

		KEMAC_INS = 0x32,

		RAND_INS = 0x40,

		SIGMAC_INS = 0x42,

		DERIVE_KEY_INS = 0x44,

		TGK_INS = 0x45,

		GEN_KEYPAIR_INS = 0xD3,

		PUT_PUBKEY_INS = 0xD4,

		GET_PUBKEY_INS = 0xD5,

		PUT_PRIKEY_INS = 0xD6,

		GET_PRIKEY_INS = 0xD7,

		SELECT_KEYRING_INS = 0xD8,

		PUT_KEYRING_INS = 0xD9
	
	};

	enum verifyMode{
		
		USER_PIN_VERIFY = 0,

		ADMIN_PIN_VERIFY
		
	};

	enum authLevel{
		
		UNVERIFIED = 0,
		
		USER_VERIFIED,
		
		ADMIN_VERIFIED
	};

	enum cardBlockLevel{

		UNBLOCKED = 0,

		USER_BLOCKED,

		ADMIN_BLOCKED
	};

};


#endif
